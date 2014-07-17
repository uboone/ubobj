////////////////////////////////////////////////////////////////////////
// $Id: DBSCANfinderAna.cxx,v 1.36 2010/09/15  bpage Exp $
//
// module to create a TTree for analysis
//
// \author tjyang@fnal.gov, sowjanyag@phys.ksu.edu
//
////////////////////////////////////////////////////////////////////////
// To reduce memory usage:
// [x] create the data structure connected to the tree only when needed
// [x] reduce the size of the elemental items (Double_t => Float_t could damage precision)
// [x] create a different structure for each tracker, allocate only what needed
// [x] use variable size array buffers for each tracker datum instead of [kMaxTrack]
// [x] turn the truth/GEANT information into vectors
// [ ] move hit_trkid into the track information, remove kMaxTrackers
// [ ] turn the hit information into vectors (~1 MB worth), remove kMaxHits
// [ ] fill the tree branch by branch
// 
// Current implementation:
// There is one tree only, with one set of branches for each tracking algorithm.
// The data structure which hosts the addresses of the tree branches is
// dynamically allocated on demand, and it can be optionally destroyed at the
// end of each event.
// The data structure (AnalysisTreeDataStruct) firectly contains the truth and
// simulation information as C arrays. The data from tracking algorithms is the
// largest, and it is contained in a C++ vector of structures (TrackDataStruct),
// one per algorithm. These structures can also be allocated on demand.
// Each of these structures is connected to a set of branches, one branch per
// data member. Data members are vectors of numbers or vectors of fixed-size
// C arrays. The vector index represents the tracks reconstructed by the
// algorithm, and each has a fixed size pool for hits (do ROOT trees support
// branches with more than one dimension with variable size?).
// The data structures can assign default values to their data, connect to a
// ROOT tree (creating the branches they need) and resize.
// The AnalysisTreeDataStruct is constructed with as many tracking algorithms as
// there are named in the module configuration (even if they are not backed by
// any available tracking data).
// By default construction, TrackDataStruct is initialized in a state which does
// not allow any track (maximum tracks number is zero), and in such state trying
// to connect to a tree has no effect. This is done so that the
// AnalysisTreeDataStruct can be initialized first (and with unusable track data
// structures), and then the TrackDataStruct instances are initialized one by
// one when the number of tracks needed is known.
// A similar mechanism is implemented for the truth information.
// 
// The "UseBuffers: false" mode assumes that on each event a new
// AnalysisTreeDataStruct is created with unusable tracker data, connected to
// the ROOT tree (the addresses of the available branches are assigned), then
// each of the tracking algorithm data is resized to host the correct number
// of reconstructed tracks and connected to the tree. Then the normal process of
// filling the event data and then the tree take place. Finally, the whole
// data structure is freed and the tree is left in a invalid state (branch
// addresses are invalid). It could be possible to make the tree in a valid
// state by resetting the addresses, but there is no advantage in that.
// 
// The "UseBuffers: true" mode assumes that on the first event a new
// AnalysisTreeDataStruct is created and used just as in the other mode
// described above. At the end of the first event, the data structure is left
// around (and the tree is in a valid state). On the next event, all the
// addresses are checked, then for each tracker the data is resized to
// accomodate the right number of tracks for tis event. If the memory is
// increased, the address will be changed. All the branches are reconnected to
// the data structure, and the procedure goes on as normal.
// 
// Note that reducing the maximum number of tracks in a TrackDataStruct does not
// necessarily make memory available, because of how std::vector::resize()
// works; that feature can be implemented, but it currently has not been.
// 
// The BoxedArray<> class is a wrapper around a normal C array; it is needed
// to be able to include such structure in a std::vector. This container
// requires its objects to be default-constructable and copy-constructable,
// and a C array is neither. BoxedArray<> is: the default construction leaves it
// uninitialized (for speed reasons) while the copy construction is performed
// as in a Plain Old Data structure (memcpy; really!).
// 
////////////////////////////////////////////////////////////////////////

#ifndef ANALYSISTREE_H
#define ANALYSISTREE_H

// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/View.h"
#include "art/Persistency/Common/Ptr.h"
#include "art/Persistency/Common/PtrVector.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "art/Framework/Core/FindMany.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Geometry/Geometry.h"
#include "SimulationBase/MCTruth.h"
#include "SimulationBase/MCFlux.h"
#include "Simulation/SimChannel.h"
#include "AnalysisBase/Calorimetry.h"
#include "AnalysisBase/ParticleID.h"
#include "RawData/RawDigit.h"
#include "RawData/BeamInfo.h"
#include "Utilities/LArProperties.h"
#include "Utilities/AssociationUtil.h"
#include "Utilities/DetectorProperties.h"
#include "SummaryData/POTSummary.h"
#include "MCCheater/BackTracker.h"
#include "RecoBase/Track.h"
#include "RecoBase/Cluster.h"
#include "RecoBase/Hit.h"
#include "RecoBase/EndPoint2D.h"
#include "RecoBase/Vertex.h"
#include "SimpleTypesAndConstants/geo_types.h"
#include "RecoObjects/BezierTrack.h"

#include <cstring> // std::memcpy()
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional> // std::mem_fun_ref

#include "TTree.h"
#include "TTimeStamp.h"

constexpr int kNplanes       = 3;     //number of wire planes
constexpr int kMaxHits       = 20000; //maximum number of hits;
constexpr int kMaxTrackHits  = 2000;  //maximum number of hits on a track
constexpr int kMaxTrackers   = 10;    //number of trackers passed into fTrackModuleLabel
constexpr unsigned short kMaxVertices   = 10;    //max number of 3D vertices

/// total_extent\<T\>::value has the total number of elements of an array
template <typename T>
struct total_extent {
  using value_type = size_t;
  static constexpr value_type value
    = sizeof(T) / sizeof(typename std::remove_all_extents<T>::type);
}; // total_extent<>


namespace microboone {

  /// Data structure with all the tree information.
  /// 
  /// Can connect to a tree, clear its fields and resize its data.
  class AnalysisTreeDataStruct {
      public:
    
    /// A wrapper to a C array (needed to embed an array into a vector)
    template <typename Array_t>
    class BoxedArray {
        protected:
      Array_t array; // actual data
      
        public:
      using This_t = BoxedArray<Array_t>;
      typedef typename std::remove_all_extents<Array_t>::type Data_t;
      
      BoxedArray() {} // no initialization
      BoxedArray(const This_t& from)
        { std::memcpy((char*) &(data()), (char*) &(from.data()), sizeof(Array_t)); }
      
      Array_t& data() { return array; }
      const Array_t& data() const { return array; }
      
      //@{
      /// begin/end interface
      static constexpr size_t size() { return total_extent<Array_t>::value; }
      Data_t* begin() { return reinterpret_cast<Data_t*>(&array); }
      const Data_t* begin() const { return reinterpret_cast<const Data_t*>(&array); }
      Data_t* end() { return begin() + size(); }
      const Data_t* end() const { return begin() + size(); }
      //@}
      
      //@{
      /// Array interface
      auto operator[] (size_t index) -> decltype(*array) { return array[index]; }
      auto operator[] (size_t index) const -> decltype(*array) { return array[index]; }
      auto operator+ (ptrdiff_t index) -> decltype(&*array) { return array + index; }
      auto operator+ (ptrdiff_t index) const -> decltype(&*array) { return array + index; }
      auto operator- (ptrdiff_t index) -> decltype(&*array) { return array - index; }
      auto operator- (ptrdiff_t index) const -> decltype(&*array) { return array - index; }
      auto operator* () -> decltype(*array) { return *array; }
      auto operator* () const -> decltype(*array) { return *array; }
      
      operator decltype(&array[0]) () { return &array[0]; }
      operator decltype(&array[0]) () const { return &array[0]; }
      //@}
      
    }; // BoxedArray
    
    /// Tracker algorithm result
    /// 
    /// Can connect to a tree, clear its fields and resize its data.
    class TrackDataStruct {
        public:
      /* Data structure size:
       *
       * TrackData_t<Short_t>                    :  2  bytes/track
       * TrackData_t<Float_t>                    :  4  bytes/track
       * PlaneData_t<Float_t>, PlaneData_t<Int_t>: 12  bytes/track
       * HitData_t<Float_t>                      : 24k bytes/track
       * HitCoordData_t<Float_t>                 : 72k bytes/track
       */
      template <typename T>
      using TrackData_t = std::vector<T>;
      template <typename T>
      using PlaneData_t = std::vector<BoxedArray<T[kNplanes]>>;
      template <typename T>
      using HitData_t = std::vector<BoxedArray<T[kNplanes][kMaxTrackHits]>>;
      template <typename T>
      using HitCoordData_t = std::vector<BoxedArray<T[kNplanes][kMaxTrackHits][3]>>;
      
      size_t MaxTracks; ///< maximum number of storable tracks
      
      Short_t  ntracks;             //number of reconstructed tracks
      PlaneData_t<Float_t>    trkke;
      PlaneData_t<Float_t>    trkrange;
      PlaneData_t<Int_t>      trkidtruth;  //true geant trackid
      PlaneData_t<Short_t>    trkorigin;   //_ev_origin 0: unknown, 1: cosmic, 2: neutrino, 3: supernova, 4: singles
      PlaneData_t<Int_t>      trkpdgtruth; //true pdg code
      PlaneData_t<Float_t>    trkefftruth; //completeness
      PlaneData_t<Float_t>    trkpurtruth; //purity of track
      PlaneData_t<Float_t>    trkpitchc;
      PlaneData_t<Short_t>    ntrkhits;
      HitData_t<Float_t>      trkdedx;
      HitData_t<Float_t>      trkdqdx;
      HitData_t<Float_t>      trkresrg;
      HitCoordData_t<Float_t> trkxyz;

      // more track info
      TrackData_t<Short_t> trkId;
      TrackData_t<Float_t> trkstartx;     // starting x position.
      TrackData_t<Float_t> trkstarty;     // starting y position.
      TrackData_t<Float_t> trkstartz;     // starting z position.
      TrackData_t<Float_t> trkstartd;     // starting distance to boundary.
      TrackData_t<Float_t> trkendx;       // ending x position.
      TrackData_t<Float_t> trkendy;       // ending y position.
      TrackData_t<Float_t> trkendz;       // ending z position.
      TrackData_t<Float_t> trkendd;       // ending distance to boundary.
      TrackData_t<Float_t> trktheta;      // theta.
      TrackData_t<Float_t> trkphi;        // phi.
      TrackData_t<Float_t> trkstartdcosx;
      TrackData_t<Float_t> trkstartdcosy;
      TrackData_t<Float_t> trkstartdcosz;
      TrackData_t<Float_t> trkenddcosx;
      TrackData_t<Float_t> trkenddcosy;
      TrackData_t<Float_t> trkenddcosz;
      TrackData_t<Float_t> trkthetaxz;    // theta_xz.
      TrackData_t<Float_t> trkthetayz;    // theta_yz.
      TrackData_t<Float_t> trkmom;        // momentum.
      TrackData_t<Float_t> trklen;        // length.
      TrackData_t<Short_t> trksvtxid;     // Vertex ID associated with the track start
      TrackData_t<Short_t> trkevtxid;     // Vertex ID associated with the track end
      TrackData_t<Int_t> trkpidpdg;       // particle PID pdg code
      TrackData_t<Float_t> trkpidchi;     // particle PID chisq
      TrackData_t<Float_t> trkpidchipr;   // particle PID chisq for proton
      TrackData_t<Float_t> trkpidchika;   // particle PID chisq for kaon
      TrackData_t<Float_t> trkpidchipi;   // particle PID chisq for pion
      TrackData_t<Float_t> trkpidchimu;   // particle PID chisq for muon
      TrackData_t<Float_t> trkpidpida;    // particle PIDA
      
      // BB vertex info
      unsigned short nvtx;
      std::vector<std::array<Float_t, 3>> vtx;
      
      /// Creates an empty tracker data structure
      TrackDataStruct(): MaxTracks(0) { Clear(); }
      /// Creates a tracker data structure allowing up to maxTracks tracks
      TrackDataStruct(size_t maxTracks): MaxTracks(maxTracks) { Clear(); }
      void Clear();
      void SetMaxTracks(size_t maxTracks)
        { MaxTracks = maxTracks; Resize(MaxTracks); }
      void Resize(size_t nTracks);
      void SetAddresses(TTree* pTree, std::string tracker);
      
      size_t GetMaxTracks() const { return MaxTracks; }
      size_t GetMaxPlanesPerTrack(int /* iTrack */ = 0) const
        { return (size_t) kNplanes; }
      size_t GetMaxHitsPerTrack(int /* iTrack */ = 0, int /* ipl */ = 0) const
        { return (size_t) kMaxTrackHits; }
      
    }; // class TrackDataStruct
    
    
/*    /// information from the run
    struct RunData_t {
        public:
      RunData_t() { Clear(); }
      void Clear() {}
    }; // struct RunData_t
*/
    /// information from the subrun
    struct SubRunData_t {
      SubRunData_t() { Clear(); }
      void Clear() { pot = -99999.; }
      Double_t pot; //protons on target
    }; // struct SubRunData_t

//    RunData_t    RunData; ///< run data collected at begin of run
    SubRunData_t SubRunData; ///< subrun data collected at begin of subrun

    //run information
    Int_t      run;                  //run number
    Int_t      subrun;               //subrun number
    Int_t      event;                //event number
    Double_t   evttime;              //event time in sec
    Double_t   beamtime;             //beam time
  //  Double_t   pot;                  //protons on target moved in subrun data
    Double_t   taulife;              //electron lifetime
    Char_t     isdata;               //flag, 0=MC 1=data

    // hit information (non-resizeable, 45x kMaxHits = 900k bytes worth)
    Int_t    no_hits;                  //number of hits
    Char_t   hit_plane[kMaxHits];      //plane number
    Short_t  hit_wire[kMaxHits];       //wire number
    Short_t  hit_channel[kMaxHits];    //channel ID
    Double_t hit_peakT[kMaxHits];      //peak time
    Float_t  hit_charge[kMaxHits];     //charge (area)
    Float_t  hit_ph[kMaxHits];         //amplitude
    Float_t  hit_startT[kMaxHits];     //hit start time
    Short_t  hit_trkid[kMaxTrackers][kMaxHits];      //is this hit associated with a reco track?
    
    //track information
    Char_t   kNTracker;
    std::vector<TrackDataStruct> TrackData;
    
    //mctruth information
    Int_t     mcevts_truth;    //number of neutrino Int_teractions in the spill
    Int_t     nuPDG_truth;     //neutrino PDG code
    Int_t     ccnc_truth;      //0=CC 1=NC
    Int_t     mode_truth;      //0=QE/El, 1=RES, 2=DIS, 3=Coherent production
    Double_t  enu_truth;       //true neutrino energy
    Double_t  Q2_truth;        //Momentum transfer squared
    Double_t  W_truth;         //hadronic invariant mass
    Int_t     hitnuc_truth;    //hit nucleon
    Double_t  nuvtxx_truth;    //neutrino vertex x
    Double_t  nuvtxy_truth;    //neutrino vertex y
    Double_t  nuvtxz_truth;    //neutrino vertex z
    Double_t  nu_dcosx_truth;  //neutrino dcos x
    Double_t  nu_dcosy_truth;  //neutrino dcos y
    Double_t  nu_dcosz_truth;  //neutrino dcos z
    Double_t  lep_mom_truth;   //lepton momentum
    Double_t  lep_dcosx_truth; //lepton dcos x
    Double_t  lep_dcosy_truth; //lepton dcos y
    Double_t  lep_dcosz_truth; //lepton dcos z

    //flux information
    Double_t  tpx_flux;        //Px of parent particle leaving BNB target
    Double_t  tpy_flux;        //Py of parent particle leaving BNB target
    Double_t  tpz_flux;        //Pz of parent particle leaving BNB target
    Int_t     tptype_flux;     //Type of parent particle leaving BNB target

    //genie information
    size_t MaxGeniePrimaries;
    Int_t     genie_no_primaries;
    std::vector<Int_t>     genie_primaries_pdg;
    std::vector<Double_t>  genie_Eng;
    std::vector<Double_t>  genie_Px;
    std::vector<Double_t>  genie_Py;
    std::vector<Double_t>  genie_Pz;
    std::vector<Double_t>  genie_P;
    std::vector<Int_t>     genie_status_code;
    std::vector<Double_t>  genie_mass;
    std::vector<Int_t>     genie_trackID;
    std::vector<Int_t>     genie_ND;
    std::vector<Int_t>     genie_mother;
    
    //geant information
    size_t MaxGEANTparticles;
    Int_t     no_primaries;      //number of primary geant particles
    Int_t     geant_list_size;  //number of all geant particles
    Int_t     geant_list_size_in_tpcFV;
    std::vector<Int_t>     pdg;
    std::vector<Double_t>  Eng;
    std::vector<Double_t>  Px;
    std::vector<Double_t>  Py;
    std::vector<Double_t>  Pz;
    std::vector<Double_t>  StartPointx;
    std::vector<Double_t>  StartPointy;
    std::vector<Double_t>  StartPointz;
    std::vector<Double_t>  EndPointx;
    std::vector<Double_t>  EndPointy;
    std::vector<Double_t>  EndPointz;
    std::vector<Int_t>     NumberDaughters;
    std::vector<Int_t>     TrackId;
    std::vector<Int_t>     Mother;
    std::vector<Int_t>     process_primary;
    std::vector<Int_t>     MergedId; //geant track segments, which belong to the same particle, get the same

    // more geant information
    size_t MaxGEANTparticlesInTPCFV;
    std::vector<Int_t>   geant_tpcFV_status;
    std::vector<Int_t>   geant_tpcFV_trackId;
    std::vector<Int_t>   geant_tpcFV_pdg;
    
    std::vector<Double_t>  geant_tpcFV_orig_E;
    std::vector<Double_t>  geant_tpcFV_orig_px;
    std::vector<Double_t>  geant_tpcFV_orig_py;
    std::vector<Double_t>  geant_tpcFV_orig_pz;
    std::vector<Double_t>  geant_tpcFV_orig_startx;
    std::vector<Double_t>  geant_tpcFV_orig_starty;
    std::vector<Double_t>  geant_tpcFV_orig_startz;
    std::vector<Double_t>  geant_tpcFV_orig_startt;
    std::vector<Double_t>  geant_tpcFV_orig_endx;
    std::vector<Double_t>  geant_tpcFV_orig_endy;
    std::vector<Double_t>  geant_tpcFV_orig_endz;
    std::vector<Double_t>  geant_tpcFV_orig_endt;

    std::vector<Double_t>  geant_tpcFV_startx;      // starting x position.
    std::vector<Double_t>  geant_tpcFV_starty;      // starting y position.
    std::vector<Double_t>  geant_tpcFV_startz;      // starting z position.
    std::vector<Double_t>  geant_tpcFV_startd;      // starting distance to boundary.
    std::vector<Double_t>  geant_tpcFV_endx;        // ending x position.
    std::vector<Double_t>  geant_tpcFV_endy;        // ending y position.
    std::vector<Double_t>  geant_tpcFV_endz;        // ending z position.
    std::vector<Double_t>  geant_tpcFV_endd;        // ending distance to boundary.
    std::vector<Double_t>  geant_tpcFV_theta;       // theta.
    std::vector<Double_t>  geant_tpcFV_phi;         // phi.
    std::vector<Double_t>  geant_tpcFV_theta_xz;    // theta_xz.
    std::vector<Double_t>  geant_tpcFV_theta_yz;    // theta_yz.
    std::vector<Double_t>  geant_tpcFV_mom;         // momentum.
    std::vector<Double_t>  geant_tpcFV_len;         // length.
    
    
    /// Constructor; clears all fields
    AnalysisTreeDataStruct(size_t nTrackers = 0)
      { SetTrackers(nTrackers); Clear(); }

    TrackDataStruct& GetTrackerData(size_t iTracker)
      { return TrackData.at(iTracker); }
    const TrackDataStruct& GetTrackerData(size_t iTracker) const
      { return TrackData.at(iTracker); }
    
    
    /// Clear all fields if this object (not the tracker algorithm data)
    void ClearLocalData();
    
    /// Clear all fields
    void Clear();
    
    
    /// Allocates data structures for the given number of trackers (no Clear())
    void SetTrackers(size_t nTrackers) { TrackData.resize(nTrackers); }
    
    /// Resize the data strutcure for GEANT particles
    void ResizeGEANT(int nParticles);
    
    /// Resize the data strutcure for GEANT particles in fiducial volume
    void ResizeGEANTinTPCFV(int nParticles);
    
    /// Resize the data strutcure for Genie primaries
    void ResizeGenie(int nPrimaries);
    
    
    /// Connect this object with a tree
    void SetAddresses(TTree* pTree, const std::vector<std::string>& trackers);
    
    
    /// Returns the number of trackers for which data structures are allocated
    size_t GetNTrackers() const { return TrackData.size(); }
    
    /// Returns the number of hits for which memory is allocated
    size_t GetMaxHits() const { return kMaxHits; }
    
    /// Returns the number of trackers for which memory is allocated
    size_t GetMaxTrackers() const { return TrackData.capacity(); }
    
    /// Returns the number of GEANT particles for which memory is allocated
    size_t GetMaxGEANTparticles() const { return MaxGEANTparticles; }
    
    /// Returns the number of GEANT particles in FV for which memory is allocated
    size_t GetMaxGEANTparticlesInTPCFV() const { return MaxGEANTparticlesInTPCFV; }
    
    /// Returns the number of GENIE primaries for which memory is allocated
    size_t GetMaxGeniePrimaries() const { return MaxGeniePrimaries; }
    
    
  private:
    /// Little helper functor class to create or reset branches in a tree
    class BranchCreator {
        public:
      TTree* pTree; ///< the tree to be worked on
      BranchCreator(TTree* tree): pTree(tree) {}

      //@{
      /// Create a branch if it does not exist, and set its address
      void operator()
        (std::string name, void* address, std::string leaflist /*, int bufsize = 32000 */)
        {
          if (!pTree) return;
          TBranch* pBranch = pTree->GetBranch(name.c_str());
          if (!pBranch) {
            pTree->Branch(name.c_str(), address, leaflist.c_str() /*, bufsize */);
            LOG_DEBUG("AnalysisTreeStructure")
              << "Creating branch '" << name << " with leaf '" << leaflist << "'";
          }
          else if (pBranch->GetAddress() != address) {
            pBranch->SetAddress(address);
            LOG_DEBUG("AnalysisTreeStructure")
              << "Reassigning address to branch '" << name << "'";
          }
          else {
            LOG_DEBUG("AnalysisTreeStructure")
              << "Branch '" << name << "' is fine";
          }
        } // operator()
      void operator()
        (std::string name, void* address, const std::stringstream& leaflist /*, int bufsize = 32000 */)
        { return this->operator() (name, address, leaflist.str() /*, int bufsize = 32000 */); }
      template <typename T>
      void operator()
        (std::string name, std::vector<T>& data, std::string leaflist /*, int bufsize = 32000 */)
        { return this->operator() (name, (void*) data.data(), leaflist /*, int bufsize = 32000 */); }
      //@}
    }; // class BranchCreator

  }; // class AnalysisTreeDataStruct
  
  
  /**
   * @brief Creates a simple ROOT tree with tracking and calorimetry information
   * 
   * <h2>Configuration parameters</h2>
   * - <b>UseBuffers</b> (default: false): if enabled, memory is allocated for
   *   tree data for all the run; otherwise, it's allocated on each event, used
   *   and freed; use "true" for speed, "false" to save memory
   */
  class AnalysisTree : public art::EDAnalyzer {

  public:

    explicit AnalysisTree(fhicl::ParameterSet const& pset);
    virtual ~AnalysisTree();

    /// read access to event
    void analyze(const art::Event& evt);
  //  void beginJob() {}
    void beginSubRun(const art::SubRun& sr);

  private:

    void   HitsPurity(std::vector< art::Ptr<recob::Hit> > const& hits, Int_t& trackid, Float_t& purity, double& maxe);
    double length(const recob::Track& track);
    double length(const simb::MCParticle& part, TVector3& start, TVector3& end);
    double bdist(const TVector3& pos);

    TTree* fTree;

    // event information is huge and dynamic;
    // run information is much smaller and we still store it statically
    // in the event
    AnalysisTreeDataStruct* fData;
//    AnalysisTreeDataStruct::RunData_t RunData;
    AnalysisTreeDataStruct::SubRunData_t SubRunData;

    std::string fDigitModuleLabel;
    std::string fHitsModuleLabel;
    std::string fLArG4ModuleLabel;
    std::string fCalDataModuleLabel;
    std::string fGenieGenModuleLabel;
    std::string fG4ModuleLabel;
    std::vector<std::string> fTrackModuleLabel;
    std::vector<std::string> fVertexModuleLabel;
    std::vector<std::string> fCalorimetryModuleLabel;
    std::vector<std::string> fParticleIDModuleLabel;
    std::string fPOTModuleLabel;
    bool fUseBuffer; ///< whether to use a permanent buffer (faster, huge memory)

    /// Returns the number of trackers configured
    size_t GetNTrackers() const { return fTrackModuleLabel.size(); }
    
    /// Creates the structure for the tree data; optionally initializes it
    void CreateData(bool bClearData = false)
      {
        if (!fData) fData = new AnalysisTreeDataStruct(GetNTrackers());
        else {
          fData->SetTrackers(GetNTrackers());
          if (bClearData) fData->Clear();
        }
      } // CreateData()
    
    /// Sets the addresses of all the tree branches, creating the missing ones
    void SetAddresses()
      {
        CheckData("SetAddress()"); CheckTree("SetAddress()");
        fData->SetAddresses(fTree, fTrackModuleLabel);
      } // SetAddresses()
    
    /// Sets the addresses of all the tree branches of the specified tracking algo,
    /// creating the missing ones
    void SetTrackerAddresses(size_t iTracker)
      {
        CheckData("SetTrackerAddresses()"); CheckTree("SetTrackerAddresses()");
        if (iTracker >= fData->GetNTrackers()) {
          throw art::Exception(art::errors::LogicError)
            << "AnalysisTree::SetTrackerAddresses(): no tracker #" << iTracker
            << " (" << fData->GetNTrackers() << " available)";
        }
        fData->GetTrackerData(iTracker) \
          .SetAddresses(fTree, fTrackModuleLabel[iTracker]);
      } // SetTrackerAddresses()
    
    /// Create the output tree and the data structures, if needed
    void CreateTree(bool bClearData = false);
    
    /// Destroy the local buffers (existing branches will point to invalid address!)
    void DestroyData() { if (fData) { delete fData; fData = nullptr; } }
    
    /// Helper function: throws if no data structure is available
    void CheckData(std::string caller) const
      {
        if (fData) return;
        throw art::Exception(art::errors::LogicError)
          << "AnalysisTree::" << caller << ": no data";
      } // CheckData()
    /// Helper function: throws if no tree is available
    void CheckTree(std::string caller) const
      {
        if (fTree) return;
        throw art::Exception(art::errors::LogicError)
          << "AnalysisTree::" << caller << ": no tree";
      } // CheckData()
  }; // class microboone::AnalysisTree
} // namespace microboone


namespace { // local namespace
  /// Simple stringstream which empties its buffer on operator() call
  class AutoResettingStringSteam: public std::ostringstream {
      public:
    AutoResettingStringSteam& operator() () { str(""); return *this; }
  }; // class AutoResettingStringSteam

  /// Fills a sequence of TYPE elements
  template <typename ITER, typename TYPE>
  inline void FillWith(ITER from, ITER to, TYPE value)
    { std::fill(from, to, value); }

  /// Fills a sequence of TYPE elements
  template <typename ITER, typename TYPE>
  inline void FillWith(ITER from, size_t n, TYPE value)
    { std::fill(from, from + n, value); }

  /// Fills a container with begin()/end() interface
  template <typename CONT, typename V>
  inline void FillWith(CONT& data, const V& value)
    { FillWith(data.begin(), data.end(), value); }

} // local namespace


//------------------------------------------------------------------------------
//---  AnalysisTreeDataStruct::TrackDataStruct
//---

void microboone::AnalysisTreeDataStruct::TrackDataStruct::Resize(size_t nTracks)
{
  MaxTracks = nTracks;
  
  trkId.resize(MaxTracks);
  trkstartx.resize(MaxTracks);
  trkstarty.resize(MaxTracks);
  trkstartz.resize(MaxTracks);
  trkstartd.resize(MaxTracks);
  trkendx.resize(MaxTracks);
  trkendy.resize(MaxTracks);
  trkendz.resize(MaxTracks);
  trkendd.resize(MaxTracks);
  trktheta.resize(MaxTracks);
  trkphi.resize(MaxTracks);
  trkstartdcosx.resize(MaxTracks);
  trkstartdcosy.resize(MaxTracks);
  trkstartdcosz.resize(MaxTracks);
  trkenddcosx.resize(MaxTracks);
  trkenddcosy.resize(MaxTracks);
  trkenddcosz.resize(MaxTracks);
  trkthetaxz.resize(MaxTracks);
  trkthetayz.resize(MaxTracks);
  trkmom.resize(MaxTracks);
  trklen.resize(MaxTracks);
  trksvtxid.resize(MaxTracks);
  trkevtxid.resize(MaxTracks);
  // PID variables
  trkpidpdg.resize(MaxTracks);
  trkpidchi.resize(MaxTracks);
  trkpidchipr.resize(MaxTracks);
  trkpidchika.resize(MaxTracks);
  trkpidchipi.resize(MaxTracks);
  trkpidchimu.resize(MaxTracks);
  trkpidpida.resize(MaxTracks);
  
  trkke.resize(MaxTracks);
  trkrange.resize(MaxTracks);
  trkidtruth.resize(MaxTracks);
  trkorigin.resize(MaxTracks);
  trkpdgtruth.resize(MaxTracks);
  trkefftruth.resize(MaxTracks);
  trkpurtruth.resize(MaxTracks);
  trkpitchc.resize(MaxTracks);
  ntrkhits.resize(MaxTracks);
  
  trkdedx.resize(MaxTracks);
  trkdqdx.resize(MaxTracks);
  trkresrg.resize(MaxTracks);
  trkxyz.resize(MaxTracks);
  
  vtx.resize(kMaxVertices);
  
} // microboone::AnalysisTreeDataStruct::TrackDataStruct::Resize()

void microboone::AnalysisTreeDataStruct::TrackDataStruct::Clear() {
  Resize(MaxTracks);
  ntracks = 0;
  
  FillWith(trkId        , -9999  );
  FillWith(trkstartx    , -99999.);
  FillWith(trkstarty    , -99999.);
  FillWith(trkstartz    , -99999.);
  FillWith(trkstartd    , -99999.);
  FillWith(trkendx      , -99999.);
  FillWith(trkendy      , -99999.);
  FillWith(trkendz      , -99999.);
  FillWith(trkendd      , -99999.);
  FillWith(trktheta     , -99999.);
  FillWith(trkphi       , -99999.);
  FillWith(trkstartdcosx, -99999.);
  FillWith(trkstartdcosy, -99999.);
  FillWith(trkstartdcosz, -99999.);
  FillWith(trkenddcosx  , -99999.);
  FillWith(trkenddcosy  , -99999.);
  FillWith(trkenddcosz  , -99999.);
  FillWith(trkthetaxz   , -99999.);
  FillWith(trkthetayz   , -99999.);
  FillWith(trkmom       , -99999.);
  FillWith(trklen       , -99999.);
  FillWith(trksvtxid    , -1);
  FillWith(trkevtxid    , -1);
  FillWith(trkpidpdg    , -1);
  FillWith(trkpidchi    , -99999.);
  FillWith(trkpidchipr  , -99999.);
  FillWith(trkpidchika  , -99999.);
  FillWith(trkpidchipi  , -99999.);
  FillWith(trkpidchimu  , -99999.);
  FillWith(trkpidpida   , -99999.);
  
  for (size_t iTrk = 0; iTrk < MaxTracks; ++iTrk){
    
    // the following are BoxedArray's;
    // their iterators traverse all the array dimensions
    FillWith(trkke[iTrk]      , -99999.);
    FillWith(trkrange[iTrk]   , -99999.);
    FillWith(trkidtruth[iTrk] , -99999 );
    FillWith(trkorigin[iTrk] , -1 );
    FillWith(trkpdgtruth[iTrk], -99999 );
    FillWith(trkefftruth[iTrk], -99999.);
    FillWith(trkpurtruth[iTrk], -99999.);
    FillWith(trkpitchc[iTrk]  , -99999.);
    FillWith(ntrkhits[iTrk]   ,  -9999 );
    
    FillWith(trkdedx[iTrk], 0.);
    FillWith(trkdqdx[iTrk], 0.);
    FillWith(trkresrg[iTrk], 0.);
    
    FillWith(trkxyz[iTrk], 0.);
    
  } // for track

  // BB vertices
  std::array<Float_t, 3> zeroes = {0.};
  FillWith(vtx, zeroes);
  
} // microboone::AnalysisTreeDataStruct::TrackDataStruct::Clear()


void microboone::AnalysisTreeDataStruct::TrackDataStruct::SetAddresses(
  TTree* pTree, std::string tracker
) {
  if (MaxTracks == 0) return; // no tracks, no tree!
  
  microboone::AnalysisTreeDataStruct::BranchCreator CreateBranch(pTree);

  AutoResettingStringSteam sstr;
  sstr() << kMaxTrackHits;
  std::string MaxTrackHitsIndexStr("[" + sstr.str() + "]");
  
  std::string TrackLabel = tracker;
  std::string BranchName;

  BranchName = "ntracks_" + TrackLabel;
  CreateBranch(BranchName, &ntracks, BranchName + "/S");
  std::string NTracksIndexStr = "[" + BranchName + "]";
  
  BranchName = "trkId_" + TrackLabel;
  CreateBranch(BranchName, trkId, BranchName + NTracksIndexStr + "/S");
  
  BranchName = "trkke_" + TrackLabel;
  CreateBranch(BranchName, trkke, BranchName + NTracksIndexStr + "[3]/F");
  
  BranchName = "trkrange_" + TrackLabel;
  CreateBranch(BranchName, trkrange, BranchName + NTracksIndexStr + "[3]/F");
  
  BranchName = "trkidtruth_" + TrackLabel;
  CreateBranch(BranchName, trkidtruth, BranchName + NTracksIndexStr + "[3]/I");

  BranchName = "trkorigin_" + TrackLabel;
  CreateBranch(BranchName, trkorigin, BranchName + NTracksIndexStr + "[3]/S");
  
  BranchName = "trkpdgtruth_" + TrackLabel;
  CreateBranch(BranchName, trkpdgtruth, BranchName + NTracksIndexStr + "[3]/I");
  
  BranchName = "trkefftruth_" + TrackLabel;
  CreateBranch(BranchName, trkefftruth, BranchName + NTracksIndexStr + "[3]/F");
  
  BranchName = "trkpurtruth_" + TrackLabel;
  CreateBranch(BranchName, trkpurtruth, BranchName + NTracksIndexStr + "[3]/F");
  
  BranchName = "trkpitchc_" + TrackLabel;
  CreateBranch(BranchName, trkpitchc, BranchName + NTracksIndexStr + "[3]/F");
  
  BranchName = "ntrkhits_" + TrackLabel;
  CreateBranch(BranchName, ntrkhits, BranchName + NTracksIndexStr + "[3]/S");
  
  BranchName = "trkdedx_" + TrackLabel;
  CreateBranch(BranchName, trkdedx, BranchName + NTracksIndexStr + "[3]" + MaxTrackHitsIndexStr + "/F");
  
  BranchName = "trkdqdx_" + TrackLabel;
  CreateBranch(BranchName, trkdqdx, BranchName + NTracksIndexStr + "[3]" + MaxTrackHitsIndexStr + "/F");
  
  BranchName = "trkresrg_" + TrackLabel;
  CreateBranch(BranchName, trkresrg, BranchName + NTracksIndexStr + "[3]" + MaxTrackHitsIndexStr + "/F");
  
  BranchName = "trkxyz_" + TrackLabel;
  CreateBranch(BranchName, trkxyz, BranchName + NTracksIndexStr + "[3]" + MaxTrackHitsIndexStr + "/F");
  
  BranchName = "trkstartx_" + TrackLabel;
  CreateBranch(BranchName, trkstartx, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkstarty_" + TrackLabel;
  CreateBranch(BranchName, trkstarty, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkstartz_" + TrackLabel;
  CreateBranch(BranchName, trkstartz, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkstartd_" + TrackLabel;
  CreateBranch(BranchName, trkstartd, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkendx_" + TrackLabel;
  CreateBranch(BranchName, trkendx, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkendy_" + TrackLabel;
  CreateBranch(BranchName, trkendy, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkendz_" + TrackLabel;
  CreateBranch(BranchName, trkendz, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkendd_" + TrackLabel;
  CreateBranch(BranchName, trkendd, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trktheta_" + TrackLabel;
  CreateBranch(BranchName, trktheta, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkphi_" + TrackLabel;
  CreateBranch(BranchName, trkphi, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkstartdcosx_" + TrackLabel;
  CreateBranch(BranchName, trkstartdcosx, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkstartdcosy_" + TrackLabel;
  CreateBranch(BranchName, trkstartdcosy, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkstartdcosz_" + TrackLabel;
  CreateBranch(BranchName, trkstartdcosz, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkenddcosx_" + TrackLabel;
  CreateBranch(BranchName, trkenddcosx, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkenddcosy_" + TrackLabel;
  CreateBranch(BranchName, trkenddcosy, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkenddcosz_" + TrackLabel;
  CreateBranch(BranchName, trkenddcosz, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkthetaxz_" + TrackLabel;
  CreateBranch(BranchName, trkthetaxz, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkthetayz_" + TrackLabel;
  CreateBranch(BranchName, trkthetayz, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trkmom_" + TrackLabel;
  CreateBranch(BranchName, trkmom, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trklen_" + TrackLabel;
  CreateBranch(BranchName, trklen, BranchName + NTracksIndexStr + "/F");
  
  BranchName = "trksvtxid_" + TrackLabel;
  CreateBranch(BranchName, trksvtxid, BranchName + NTracksIndexStr + "/S");
  
  BranchName = "trkevtxid_" + TrackLabel;
  CreateBranch(BranchName, trkevtxid, BranchName + NTracksIndexStr + "/S");

  BranchName = "nvtx_" + TrackLabel;
  CreateBranch(BranchName, &nvtx, BranchName + "/S");

  sstr() << kMaxVertices;
  std::string MaxVerticesIndexStr("[" + sstr.str() + "]");
  
  BranchName = "vtx_" + TrackLabel;
  CreateBranch(BranchName, vtx, BranchName + MaxVerticesIndexStr + "[3]" + "/F");

  BranchName = "trkpidpdg_" + TrackLabel;
  CreateBranch(BranchName, &trkpidpdg, BranchName + "/I");

  BranchName = "trkpidchi_" + TrackLabel;
  CreateBranch(BranchName, &trkpidchi, BranchName + "/F");

  BranchName = "trkpidchipr_" + TrackLabel;
  CreateBranch(BranchName, &trkpidchipr, BranchName + "/F");

  BranchName = "trkpidchika_" + TrackLabel;
  CreateBranch(BranchName, &trkpidchika, BranchName + "/F");

  BranchName = "trkpidchipi_" + TrackLabel;
  CreateBranch(BranchName, &trkpidchipi, BranchName + "/F");

  BranchName = "trkpidchimu_" + TrackLabel;
  CreateBranch(BranchName, &trkpidchimu, BranchName + "/F");

  BranchName = "trkpidpida_" + TrackLabel;
  CreateBranch(BranchName, &trkpidpida, BranchName + "/F");

} // microboone::AnalysisTreeDataStruct::TrackDataStruct::SetAddresses()

//------------------------------------------------------------------------------
//---  AnalysisTreeDataStruct
//---

void microboone::AnalysisTreeDataStruct::ClearLocalData() {

//  RunData.Clear();
  SubRunData.Clear();

  run = -99999;
  subrun = -99999;
  event = -99999;
  evttime = -99999;
  beamtime = -99999;
  isdata = -99;
  taulife = -99999;

  no_hits = 0;
  
  std::fill(hit_plane, hit_plane + sizeof(hit_plane)/sizeof(hit_plane[0]), -99);
  std::fill(hit_wire, hit_wire + sizeof(hit_wire)/sizeof(hit_wire[0]), -9999);
  std::fill(hit_channel, hit_channel + sizeof(hit_channel)/sizeof(hit_channel[0]), -9999);
  std::fill(hit_peakT, hit_peakT + sizeof(hit_peakT)/sizeof(hit_peakT[0]), -99999.);
  std::fill(hit_charge, hit_charge + sizeof(hit_charge)/sizeof(hit_charge[0]), -99999.);
  std::fill(hit_ph, hit_ph + sizeof(hit_ph)/sizeof(hit_ph[0]), -99999.);
  std::fill(hit_startT, hit_startT + sizeof(hit_startT)/sizeof(hit_startT[0]), -99999.);

  for (size_t iTrk = 0; iTrk < kMaxTrackers; ++iTrk) {
    std::fill(hit_trkid[iTrk], hit_trkid[iTrk] + kMaxHits, -9999);
  }
  
  mcevts_truth = -99999;
  nuPDG_truth = -99999;
  ccnc_truth = -99999;
  mode_truth = -99999;
  enu_truth = -99999;
  Q2_truth = -99999;
  W_truth = -99999;
  hitnuc_truth = -99999;
  nuvtxx_truth = -99999;
  nuvtxy_truth = -99999;
  nuvtxz_truth = -99999;
  nu_dcosx_truth = -99999;
  nu_dcosy_truth = -99999;
  nu_dcosz_truth = -99999;
  lep_mom_truth = -99999;
  lep_dcosx_truth = -99999;
  lep_dcosy_truth = -99999;
  lep_dcosz_truth = -99999;
  tpx_flux = -99999;
  tpy_flux = -99999;
  tpz_flux = -99999;
  tptype_flux = -99999;

  genie_no_primaries = 0;
  no_primaries = 0;
  geant_list_size=0;
  geant_list_size_in_tpcFV = 0;
  
  FillWith(pdg, -99999);
  FillWith(Eng, -99999.);
  FillWith(Px, -99999.);
  FillWith(Py, -99999.);
  FillWith(Pz, -99999.);
  FillWith(StartPointx, -99999.);
  FillWith(StartPointy, -99999.);
  FillWith(StartPointz, -99999.);
  FillWith(EndPointx, -99999.);
  FillWith(EndPointy, -99999.);
  FillWith(EndPointz, -99999.);
  FillWith(NumberDaughters, -99999);
  FillWith(Mother, -99999);
  FillWith(TrackId, -99999);
  FillWith(process_primary, -99999);
  FillWith(MergedId, -99999);
  FillWith(genie_primaries_pdg, -99999);
  FillWith(genie_Eng, -99999.);
  FillWith(genie_Px, -99999.);
  FillWith(genie_Py, -99999.);
  FillWith(genie_Pz, -99999.);
  FillWith(genie_P, -99999.);
  FillWith(genie_status_code, -99999);
  FillWith(genie_mass, -99999.);
  FillWith(genie_trackID, -99999);
  FillWith(genie_ND, -99999);
  FillWith(genie_mother, -99999);
  
  FillWith(geant_tpcFV_status, -99999);
  FillWith(geant_tpcFV_trackId, -99999);
  FillWith(geant_tpcFV_pdg, -99999);
  
  FillWith(geant_tpcFV_orig_E, -99999.);
  FillWith(geant_tpcFV_orig_px, -99999.);
  FillWith(geant_tpcFV_orig_py, -99999.);
  FillWith(geant_tpcFV_orig_pz, -99999.);
  FillWith(geant_tpcFV_orig_startx, -99999.);
  FillWith(geant_tpcFV_orig_starty, -99999.);
  FillWith(geant_tpcFV_orig_startz, -99999.);
  FillWith(geant_tpcFV_orig_startt, -99999.);
  FillWith(geant_tpcFV_orig_endx, -99999.);
  FillWith(geant_tpcFV_orig_endy, -99999.);
  FillWith(geant_tpcFV_orig_endz, -99999.);
  FillWith(geant_tpcFV_orig_endt, -99999.);

  FillWith(geant_tpcFV_startx, -99999.);
  FillWith(geant_tpcFV_starty, -99999.);
  FillWith(geant_tpcFV_startz, -99999.);
  FillWith(geant_tpcFV_startd, -99999.);
  FillWith(geant_tpcFV_endx, -99999.);
  FillWith(geant_tpcFV_endy, -99999.);
  FillWith(geant_tpcFV_endz, -99999.);
  FillWith(geant_tpcFV_endd, -99999.);
  FillWith(geant_tpcFV_theta, -99999.);
  FillWith(geant_tpcFV_phi, -99999.);
  FillWith(geant_tpcFV_theta_xz, -99999.);
  FillWith(geant_tpcFV_theta_yz, -99999.);
  FillWith(geant_tpcFV_mom, -99999.);
  FillWith(geant_tpcFV_len, -99999.);
} // microboone::AnalysisTreeDataStruct::ClearLocalData()


void microboone::AnalysisTreeDataStruct::Clear() {
  ClearLocalData();
  std::for_each
    (TrackData.begin(), TrackData.end(), std::mem_fun_ref(&TrackDataStruct::Clear));
} // microboone::AnalysisTreeDataStruct::Clear()


void microboone::AnalysisTreeDataStruct::ResizeGEANT(int nParticles) {

  // minimum size is 1, so that we always have an address
  MaxGEANTparticles = (size_t) std::max(nParticles, 1);
  
  pdg.resize(MaxGEANTparticles);
  Eng.resize(MaxGEANTparticles);
  Px.resize(MaxGEANTparticles);
  Py.resize(MaxGEANTparticles);
  Pz.resize(MaxGEANTparticles);
  StartPointx.resize(MaxGEANTparticles);
  StartPointy.resize(MaxGEANTparticles);
  StartPointz.resize(MaxGEANTparticles);
  EndPointx.resize(MaxGEANTparticles);
  EndPointy.resize(MaxGEANTparticles);
  EndPointz.resize(MaxGEANTparticles);
  NumberDaughters.resize(MaxGEANTparticles);
  Mother.resize(MaxGEANTparticles);
  TrackId.resize(MaxGEANTparticles);
  process_primary.resize(MaxGEANTparticles);
  MergedId.resize(MaxGEANTparticles);
  
} // microboone::AnalysisTreeDataStruct::ResizeGEANT()


void microboone::AnalysisTreeDataStruct::ResizeGEANTinTPCFV(int nParticles) {

  // minimum size is 1, so that we always have an address
  MaxGEANTparticlesInTPCFV = (size_t) std::max(nParticles, 1);
  
  geant_tpcFV_status.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_trackId.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_pdg.resize(MaxGEANTparticlesInTPCFV);
  
  geant_tpcFV_orig_E.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_px.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_py.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_pz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_startx.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_starty.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_startz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_startt.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_endx.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_endy.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_endz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_orig_endt.resize(MaxGEANTparticlesInTPCFV);

  geant_tpcFV_startx.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_starty.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_startz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_startd.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_endx.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_endy.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_endz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_endd.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_theta.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_phi.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_theta_xz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_theta_yz.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_mom.resize(MaxGEANTparticlesInTPCFV);
  geant_tpcFV_len.resize(MaxGEANTparticlesInTPCFV);

} // microboone::AnalysisTreeDataStruct::ResizeGEANTinTPCFV()


void microboone::AnalysisTreeDataStruct::ResizeGenie(int nPrimaries) {
  
  // minimum size is 1, so that we always have an address
  MaxGeniePrimaries = (size_t) std::max(nPrimaries, 1);
  genie_primaries_pdg.resize(MaxGeniePrimaries);
  genie_Eng.resize(MaxGeniePrimaries);
  genie_Px.resize(MaxGeniePrimaries);
  genie_Py.resize(MaxGeniePrimaries);
  genie_Pz.resize(MaxGeniePrimaries);
  genie_P.resize(MaxGeniePrimaries);
  genie_status_code.resize(MaxGeniePrimaries);
  genie_mass.resize(MaxGeniePrimaries);
  genie_trackID.resize(MaxGeniePrimaries);
  genie_ND.resize(MaxGeniePrimaries);
  genie_mother.resize(MaxGeniePrimaries);

} // microboone::AnalysisTreeDataStruct::ResizeGenie()


void microboone::AnalysisTreeDataStruct::SetAddresses(
  TTree* pTree,
  const std::vector<std::string>& trackers
) {
  BranchCreator CreateBranch(pTree);

  CreateBranch("run",&run,"run/I");
  CreateBranch("subrun",&subrun,"subrun/I");
  CreateBranch("event",&event,"event/I");
  CreateBranch("evttime",&evttime,"evttime/D");
  CreateBranch("beamtime",&beamtime,"beamtime/D");
  CreateBranch("pot",&SubRunData.pot,"pot/D");
  CreateBranch("isdata",&isdata,"isdata/B");
  CreateBranch("taulife",&taulife,"taulife/D");

  CreateBranch("no_hits",&no_hits,"no_hits/I");
  CreateBranch("hit_plane",hit_plane,"hit_plane[no_hits]/B");
  CreateBranch("hit_wire",hit_wire,"hit_wire[no_hits]/S");
  CreateBranch("hit_channel",hit_channel,"hit_channel[no_hits]/S");
  CreateBranch("hit_peakT",hit_peakT,"hit_peakT[no_hits]/D");
  CreateBranch("hit_charge",hit_charge,"hit_charge[no_hits]/F");
  CreateBranch("hit_ph",hit_ph,"hit_ph[no_hits]/F");
  CreateBranch("hit_startT",hit_startT,"hit_startT[no_hits]/F");

  AutoResettingStringSteam sstr;
  sstr() << kMaxTrackHits;
  std::string MaxTrackHitsIndexStr("[" + sstr.str() + "]");

  kNTracker = trackers.size();
  CreateBranch("kNTracker",&kNTracker,"kNTracker/B");
  for(int i=0; i<kNTracker; i++){
    std::string TrackLabel = trackers[i];
    std::string BranchName;

    BranchName = "hit_trkid_" + TrackLabel;
    CreateBranch(BranchName, hit_trkid[i], BranchName + "[no_hits]/S");

    // note that if the tracker data has maximum number of tracks 0,
    // nothing is initialized (branches are not even created)
    TrackData[i].SetAddresses(pTree, TrackLabel);
    
  } // for trackers

  CreateBranch("mcevts_truth",&mcevts_truth,"mcevts_truth/I");
  CreateBranch("nuPDG_truth",&nuPDG_truth,"nuPDG_truth/I");
  CreateBranch("ccnc_truth",&ccnc_truth,"ccnc_truth/I");
  CreateBranch("mode_truth",&mode_truth,"mode_truth/I");
  CreateBranch("enu_truth",&enu_truth,"enu_truth/D");
  CreateBranch("Q2_truth",&Q2_truth,"Q2_truth/D");
  CreateBranch("W_truth",&W_truth,"W_truth/D");
  CreateBranch("hitnuc_truth",&hitnuc_truth,"hitnuc_truth/I");
  CreateBranch("nuvtxx_truth",&nuvtxx_truth,"nuvtxx_truth/D");
  CreateBranch("nuvtxy_truth",&nuvtxy_truth,"nuvtxy_truth/D");
  CreateBranch("nuvtxz_truth",&nuvtxz_truth,"nuvtxz_truth/D");
  CreateBranch("nu_dcosx_truth",&nu_dcosx_truth,"nu_dcosx_truth/D");
  CreateBranch("nu_dcosy_truth",&nu_dcosy_truth,"nu_dcosy_truth/D");
  CreateBranch("nu_dcosz_truth",&nu_dcosz_truth,"nu_dcosz_truth/D");
  CreateBranch("lep_mom_truth",&lep_mom_truth,"lep_mom_truth/D");
  CreateBranch("lep_dcosx_truth",&lep_dcosx_truth,"lep_dcosx_truth/D");
  CreateBranch("lep_dcosy_truth",&lep_dcosy_truth,"lep_dcosy_truth/D");
  CreateBranch("lep_dcosz_truth",&lep_dcosz_truth,"lep_dcosz_truth/D");

  CreateBranch("tpx_flux",&tpx_flux,"tpx_flux/D");
  CreateBranch("tpy_flux",&tpy_flux,"tpy_flux/D");
  CreateBranch("tpz_flux",&tpz_flux,"tpz_flux/D");
  CreateBranch("tptype_flux",&tptype_flux,"tptype_flux/I");

  CreateBranch("genie_no_primaries",&genie_no_primaries,"genie_no_primaries/I");
  CreateBranch("genie_primaries_pdg",genie_primaries_pdg,"genie_primaries_pdg[genie_no_primaries]/I");
  CreateBranch("genie_Eng",genie_Eng,"genie_Eng[genie_no_primaries]/D");
  CreateBranch("genie_Px",genie_Px,"genie_Px[genie_no_primaries]/D");
  CreateBranch("genie_Py",genie_Py,"genie_Py[genie_no_primaries]/D");
  CreateBranch("genie_Pz",genie_Pz,"genie_Pz[genie_no_primaries]/D");
  CreateBranch("genie_P",genie_P,"genie_P[genie_no_primaries]/D");
  CreateBranch("genie_status_code",genie_status_code,"genie_status_code[genie_no_primaries]/I");
  CreateBranch("genie_mass",genie_mass,"genie_mass[genie_no_primaries]/D");
  CreateBranch("genie_trackID",genie_trackID,"genie_trackID[genie_no_primaries]/I");
  CreateBranch("genie_ND",genie_ND,"genie_ND[genie_no_primaries]/I");
  CreateBranch("genie_mother",genie_mother,"genie_mother[genie_no_primaries]/I");

  CreateBranch("no_primaries",&no_primaries,"no_primaries/I");
  CreateBranch("geant_list_size",&geant_list_size,"geant_list_size/I");

  CreateBranch("pdg",pdg,"pdg[geant_list_size]/I");
  CreateBranch("Eng",Eng,"Eng[geant_list_size]/D");
  CreateBranch("Px",Px,"Px[geant_list_size]/D");
  CreateBranch("Py",Py,"Py[geant_list_size]/D");
  CreateBranch("Pz",Pz,"Pz[geant_list_size]/D");
  CreateBranch("StartPointx",StartPointx,"StartPointx[geant_list_size]/D");
  CreateBranch("StartPointy",StartPointy,"StartPointy[geant_list_size]/D");
  CreateBranch("StartPointz",StartPointz,"StartPointz[geant_list_size]/D");
  CreateBranch("EndPointx",EndPointx,"EndPointx[geant_list_size]/D");
  CreateBranch("EndPointy",EndPointy,"EndPointy[geant_list_size]/D");
  CreateBranch("EndPointz",EndPointz,"EndPointz[geant_list_size]/D");
  CreateBranch("NumberDaughters",NumberDaughters,"NumberDaughters[geant_list_size]/I");
  CreateBranch("Mother",Mother,"Mother[geant_list_size]/I");
  CreateBranch("TrackId",TrackId,"TrackId[geant_list_size]/I");
  CreateBranch("MergedId", MergedId, "MergedId[geant_list_size]/I");
  CreateBranch("process_primary",process_primary,"process_primary[geant_list_size]/I");

  CreateBranch("geant_list_size_in_tpcFV",&geant_list_size_in_tpcFV,"geant_list_size_in_tpcFV/I");
  CreateBranch("geant_tpcFV_pdg", geant_tpcFV_pdg, "geant_tpcFV_pdg[geant_list_size_in_tpcFV]/I");
  CreateBranch("geant_tpcFV_status", geant_tpcFV_status, "geant_tpcFV_status[geant_list_size_in_tpcFV]/I");
  CreateBranch("geant_tpcFV_trackId", geant_tpcFV_trackId, "geant_tpcFV_trackId[geant_list_size_in_tpcFV]/I");
  CreateBranch("geant_tpcFV_orig_E", geant_tpcFV_orig_E, "geant_tpcFV_orig_E[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_px", geant_tpcFV_orig_px, "geant_tpcFV_orig_px[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_py", geant_tpcFV_orig_py, "geant_tpcFV_orig_py[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_pz", geant_tpcFV_orig_pz, "geant_tpcFV_orig_pz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_startx", geant_tpcFV_orig_startx, "geant_tpcFV_orig_startx[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_starty", geant_tpcFV_orig_starty, "geant_tpcFV_orig_starty[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_startz", geant_tpcFV_orig_startz, "geant_tpcFV_orig_startz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_startt", geant_tpcFV_orig_startt, "geant_tpcFV_orig_startt[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_endx", geant_tpcFV_orig_endx, "geant_tpcFV_orig_endx[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_endy", geant_tpcFV_orig_endy, "geant_tpcFV_orig_endy[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_endz", geant_tpcFV_orig_endz, "geant_tpcFV_orig_endz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_orig_endt", geant_tpcFV_orig_endt, "geant_tpcFV_orig_endt[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_startx", geant_tpcFV_startx, "geant_tpcFV_startx[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_starty", geant_tpcFV_starty, "geant_tpcFV_starty[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_startz", geant_tpcFV_startz, "geant_tpcFV_startz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_startd", geant_tpcFV_startd, "geant_tpcFV_startd[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_endx", geant_tpcFV_endx, "geant_tpcFV_endx[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_endy", geant_tpcFV_endy, "geant_tpcFV_endy[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_endz", geant_tpcFV_endz, "geant_tpcFV_endz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_endd", geant_tpcFV_endd, "geant_tpcFV_endd[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_theta", geant_tpcFV_theta, "geant_tpcFV_theta[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_phi", geant_tpcFV_phi, "geant_tpcFV_phi[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_theta_xz", geant_tpcFV_theta_xz, "geant_tpcFV_theta_xz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_theta_yz", geant_tpcFV_theta_yz, "geant_tpcFV_theta_yz[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_mom", geant_tpcFV_mom, "geant_tpcFV_mom[geant_list_size_in_tpcFV]/D");
  CreateBranch("geant_tpcFV_len", geant_tpcFV_len, "geant_tpcFV_len[geant_list_size_in_tpcFV]/D");
} // microboone::AnalysisTreeDataStruct::SetAddresses()


//------------------------------------------------------------------------------
//---  AnalysisTree
//---

microboone::AnalysisTree::AnalysisTree(fhicl::ParameterSet const& pset) :
  EDAnalyzer(pset),
  fTree(nullptr), fData(nullptr),
  fDigitModuleLabel         (pset.get< std::string >("DigitModuleLabel")        ),
  fHitsModuleLabel          (pset.get< std::string >("HitsModuleLabel")         ),
  fLArG4ModuleLabel         (pset.get< std::string >("LArGeantModuleLabel")     ),
  fCalDataModuleLabel       (pset.get< std::string >("CalDataModuleLabel")      ),
  fGenieGenModuleLabel      (pset.get< std::string >("GenieGenModuleLabel")     ),
  fG4ModuleLabel            (pset.get< std::string >("G4ModuleLabel")           ),
  fTrackModuleLabel         (pset.get< std::vector<std::string> >("TrackModuleLabel")),
  fVertexModuleLabel        (pset.get< std::vector<std::string> >("VertexModuleLabel")       ),
  fCalorimetryModuleLabel   (pset.get< std::vector<std::string> >("CalorimetryModuleLabel")),
  fParticleIDModuleLabel    (pset.get< std::vector<std::string> >("ParticleIDModuleLabel")   ),
  fPOTModuleLabel           (pset.get< std::string >("POTModuleLabel")          ),
  fUseBuffer                (pset.get< bool >("UseBuffers", false)              )
{
  mf::LogInfo("AnalysisTree") << "Configuration:"
    << "\n  UseBuffers: " << std::boolalpha << fUseBuffer
    ;
  if (GetNTrackers() > kMaxTrackers) {
    throw art::Exception(art::errors::Configuration)
      << "AnalysisTree currently supports only up to " << kMaxTrackers
      << " tracking algorithms, but " << GetNTrackers() << " are specified."
      << "\nYou can increase kMaxTrackers and recompile.";
  } // if too many trackers
  if (fTrackModuleLabel.size() != fCalorimetryModuleLabel.size()){
    throw art::Exception(art::errors::Configuration)
      << "fTrackModuleLabel.size() = "<<fTrackModuleLabel.size()<<" does not match "
      << "fCalorimetryModuleLabel.size() = "<<fCalorimetryModuleLabel.size();
  }
  if (fTrackModuleLabel.size() != fVertexModuleLabel.size()){
    throw art::Exception(art::errors::Configuration)
      << "fTrackModuleLabel.size() = "<<fTrackModuleLabel.size()<<" does not match "
      << "fVertexModuleLabel.size() = "<<fVertexModuleLabel.size();
  }
  if (fTrackModuleLabel.size() != fParticleIDModuleLabel.size()){
    throw art::Exception(art::errors::Configuration)
      << "fTrackModuleLabel.size() = "<<fTrackModuleLabel.size()<<" does not match "
      << "fParticleIDModuleLabel.size() = "<<fParticleIDModuleLabel.size();
  }
} // microboone::AnalysisTree::AnalysisTree()

//-------------------------------------------------
microboone::AnalysisTree::~AnalysisTree()
{
  DestroyData();
}

void microboone::AnalysisTree::CreateTree(bool bClearData /* = false */) {
  if (!fTree) {
    art::ServiceHandle<art::TFileService> tfs;
    fTree = tfs->make<TTree>("anatree","analysis tree");
  }
  CreateData(bClearData);
  SetAddresses();
} // microboone::AnalysisTree::CreateTree()


void microboone::AnalysisTree::beginSubRun(const art::SubRun& sr)
{

  art::Handle< sumdata::POTSummary > potListHandle;
  //sr.getByLabel(fPOTModuleLabel,potListHandle);

  if(sr.getByLabel(fPOTModuleLabel,potListHandle))
    SubRunData.pot=potListHandle->totpot;
  else
    SubRunData.pot=0.;

}

void microboone::AnalysisTree::analyze(const art::Event& evt)
{
  //services
  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<cheat::BackTracker> bt;
  art::ServiceHandle<util::DetectorProperties> detprop;
  art::ServiceHandle<util::LArProperties> LArProp;
  
  // collect the sizes which might me needed to resize the tree data structure:
  bool isMC = !evt.isRealData();
  
  // * hits
  art::Handle< std::vector<recob::Hit> > hitListHandle;
  std::vector<art::Ptr<recob::Hit> > hitlist;
  if (evt.getByLabel(fHitsModuleLabel,hitListHandle))
    art::fill_ptr_vector(hitlist, hitListHandle);

  // * MC truth information
  art::Handle< std::vector<simb::MCTruth> > mctruthListHandle;
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  if (evt.getByLabel(fGenieGenModuleLabel,mctruthListHandle))
    art::fill_ptr_vector(mclist, mctruthListHandle);

  int nGeniePrimaries = 0, nGEANTparticles = 0, nGEANTparticlesInTPCFV = 0;
  
  art::Ptr<simb::MCTruth> mctruth;
  int imc = 0;
  if (isMC) { //is MC
    // GENIE
    if (!mclist.empty()){//at least one mc record
      //if (mclist[0]->NeutrinoSet()){//is neutrino
      //sometimes there can be multiple neutrino interactions in one spill
      //this is trying to find the primary interaction
      //by looking for the highest energy deposition
      std::map<art::Ptr<simb::MCTruth>,double> mctruthemap;
      for (size_t i = 0; i<hitlist.size(); i++){
        //if (hitlist[i]->View() == geo::kV){//collection view
        std::vector<cheat::TrackIDE> eveIDs = bt->HitToEveID(hitlist[i]);
        for (size_t e = 0; e<eveIDs.size(); e++){
          art::Ptr<simb::MCTruth> ev_mctruth = bt->TrackIDToMCTruth(eveIDs[e].trackID);
          mctruthemap[ev_mctruth]+=eveIDs[e].energy;
        }
        //}
      }
      double maxenergy = -1;
      int imc0 = 0;
      for (std::map<art::Ptr<simb::MCTruth>,double>::iterator ii=mctruthemap.begin(); ii!=mctruthemap.end(); ++ii){
        if ((ii->second)>maxenergy){
          maxenergy = ii->second;
          mctruth = ii->first;
          imc = imc0;
        }
        imc0++;
      }

      imc = 0; //set imc to 0 to solve a confusion for BNB+cosmic files where there are two MCTruth
      mctruth = mclist[0];

      if (mctruth->NeutrinoSet()) nGeniePrimaries = mctruth->NParticles();
      
      const sim::ParticleList& plist = bt->ParticleList();
      nGEANTparticles = plist.size();
      // to know the number of particles in FV would require
      // looking at all of them; so we waste some memory here
      nGEANTparticlesInTPCFV = nGEANTparticles;
    } // if have MC truth
    LOG_DEBUG("AnalysisTree") << "Expected "
      << nGEANTparticles << " GEANT particles, "
      << nGeniePrimaries << " GENIE particles";
  } // if MC
  
  CreateData(); // tracker data is created with default constructor
  fData->ResizeGenie(nGeniePrimaries);
  fData->ResizeGEANT(nGEANTparticles);
  fData->ResizeGEANTinTPCFV(nGEANTparticlesInTPCFV);
  fData->ClearLocalData(); // don't bother clearing tracker data yet
  
//  const size_t Nplanes       = 3; // number of wire planes; pretty much constant...
  const size_t NTrackers = GetNTrackers(); // number of trackers passed into fTrackModuleLabel
  const size_t NHits     = hitlist.size(); // number of hits
  
  // make sure there is the data, the tree and everything;
  CreateTree();

  /// transfer the run and subrun data to the tree data object
//  fData->RunData = RunData;
  fData->SubRunData = SubRunData;

  fData->isdata = int(!isMC);
  
  std::vector< art::Handle< std::vector<recob::Track> > > trackListHandle(NTrackers);
  std::vector< std::vector<art::Ptr<recob::Track> > > tracklist(NTrackers);
  for (unsigned int it = 0; it < NTrackers; ++it){
    if (evt.getByLabel(fTrackModuleLabel[it],trackListHandle[it]))
      art::fill_ptr_vector(tracklist[it], trackListHandle[it]);
  }

  art::Handle< std::vector<simb::MCFlux> > mcfluxListHandle;
  std::vector<art::Ptr<simb::MCFlux> > fluxlist;
  if (evt.getByLabel(fGenieGenModuleLabel,mcfluxListHandle))
    art::fill_ptr_vector(fluxlist, mcfluxListHandle);


  fData->run = evt.run();
  fData->subrun = evt.subRun();
  fData->event = evt.id().event();

  art::Timestamp ts = evt.time();
  TTimeStamp tts(ts.timeHigh(), ts.timeLow());
  fData->evttime = tts.AsDouble();

  //copied from MergeDataPaddles.cxx
  art::Handle< raw::BeamInfo > beam;
  if (evt.getByLabel("beam",beam)){
    fData->beamtime = (double)beam->get_t_ms();
    fData->beamtime/=1000.; //in second
  }

//  std::cout<<detprop->NumberTimeSamples()<<" "<<detprop->ReadOutWindowSize()<<std::endl;
//  std::cout<<geom->DetHalfHeight()*2<<" "<<geom->DetHalfWidth()*2<<" "<<geom->DetLength()<<std::endl;
//  std::cout<<geom->Nwires(0)<<" "<<geom->Nwires(1)<<" "<<geom->Nwires(2)<<std::endl;

  //hit information
  fData->no_hits = (int) NHits;
  if (NHits > kMaxHits) {
    // got this error? consider increasing kMaxHits
    // (or ask for a redesign using vectors)
    mf::LogError("AnalysisTree:limits") << "event has " << NHits
      << " hits, only kMaxHits=" << kMaxHits << " stored in tree";
  }
  for (size_t i = 0; i < NHits && i < kMaxHits ; ++i){//loop over hits
    fData->hit_channel[i] = hitlist[i]->Channel();
    fData->hit_plane[i]   = hitlist[i]->WireID().Plane;
    fData->hit_wire[i]    = hitlist[i]->WireID().Wire;
    fData->hit_peakT[i]   = hitlist[i]->PeakTime();
    fData->hit_charge[i]  = hitlist[i]->Charge();
    fData->hit_ph[i]  = hitlist[i]->Charge(true);
    fData->hit_startT[i] = hitlist[i]->StartTime();
    /*
    for (unsigned int it=0; it<fTrackModuleLabel.size();++it){
      art::FindManyP<recob::Track> fmtk(hitListHandle,evt,fTrackModuleLabel[it]);
      if (fmtk.at(i).size()!=0){
        hit_trkid[it][i] = fmtk.at(i)[0]->ID();
      }
      else
        hit_trkid[it][i] = 0;
    }
    */
  }

  //track information for multiple trackers
  for (unsigned int iTracker=0; iTracker < NTrackers; ++iTracker){
    AnalysisTreeDataStruct::TrackDataStruct& TrackerData = fData->GetTrackerData(iTracker);
    
    size_t NTracks = tracklist[iTracker].size();
    // allocate enough space for this number of tracks (but at least for one of them!)
    TrackerData.SetMaxTracks(std::max(NTracks, (size_t) 1));
    TrackerData.Clear(); // clear all the data
    
    TrackerData.ntracks = (int) NTracks;
    
    // now set the tree addresses to the newly allocated memory;
    // this creates the tree branches in case they are not there yet
    SetTrackerAddresses(iTracker);
    if (NTracks > TrackerData.GetMaxTracks()) {
      // got this error? it might be a bug,
      // since we are supposed to have allocated enough space to fit all tracks
      mf::LogError("AnalysisTree:limits") << "event has " << NTracks
        << " " << fTrackModuleLabel[iTracker] << " tracks, only "
        << TrackerData.GetMaxTracks() << " stored in tree";
    }

    // Stuff vertices first so we can define a track->vertex pointer
    art::Handle< std::vector<recob::Vertex> > vtxListHandle;
    std::vector<art::Ptr<recob::Vertex> > vtxlist;
    if(evt.getByLabel(fVertexModuleLabel[iTracker], vtxListHandle)) 
      art::fill_ptr_vector(vtxlist, vtxListHandle);
    double xyz[3] = {0.};
    TrackerData.nvtx = vtxlist.size();
    if(TrackerData.nvtx > kMaxVertices) TrackerData.nvtx = kMaxVertices;
    for(size_t ivx = 0; ivx < TrackerData.nvtx; ++ivx) {
      vtxlist[ivx]->XYZ(xyz);
      TrackerData.vtx[ivx][0] = xyz[0];
      TrackerData.vtx[ivx][1] = xyz[1];
      TrackerData.vtx[ivx][2] = xyz[2];
    }

    for(size_t iTrk=0; iTrk < NTracks; ++iTrk){//loop over tracks

      art::Ptr<recob::Track> ptrack(trackListHandle[iTracker], iTrk);
      const recob::Track& track = *ptrack;
      
      TVector3 pos, dir_start, dir_end, end;
      double tlen = 0., mom = 0.;
      int TrackID = -1;
      
      int ntraj = 0;
      //we need to use Bezier methods for Bezier tracks
      if (fTrackModuleLabel[iTracker].find("beziertracker")!=std::string::npos) {
        trkf::BezierTrack btrack(*ptrack);
        ntraj = btrack.NSegments();
        if(ntraj > 0) {
          double xyz[3];
          btrack.GetTrackPoint(0,xyz);
          pos.SetXYZ(xyz[0],xyz[1],xyz[2]);
          btrack.GetTrackDirection(0,xyz);
          dir_start.SetXYZ(xyz[0],xyz[1],xyz[2]);
          btrack.GetTrackDirection(1,xyz);
          dir_end.SetXYZ(xyz[0],xyz[1],xyz[2]);
          btrack.GetTrackPoint(1,xyz);
          end.SetXYZ(xyz[0],xyz[1],xyz[2]);

          tlen        = btrack.GetLength();
          if (btrack.NumberFitMomentum() > 0)
            mom = btrack.VertexMomentum();
          // fill bezier track reco branches
          TrackID = iTrk;  //bezier has some screwed up track IDs
        }
      }
      else {   //use the normal methods for other kinds of tracks
        ntraj = track.NumberTrajectoryPoints();
        if (ntraj > 0) {
          pos       = track.Vertex();
          dir_start = track.VertexDirection();
          dir_end   = track.EndDirection();
          end       = track.End();

          tlen        = length(track);
          if(track.NumberFitMomentum() > 0)
            mom = track.VertexMomentum();
          // fill non-bezier-track reco branches
          TrackID = track.ID();
        }
      }
      
      if (ntraj > 0) {
        double theta_xz = std::atan2(dir_start.X(), dir_start.Z());
        double theta_yz = std::atan2(dir_start.Y(), dir_start.Z());
        double dpos = bdist(pos);
        double dend = bdist(end);
        
        TrackerData.trkId[iTrk]                 = TrackID;
        TrackerData.trkstartx[iTrk]             = pos.X();
        TrackerData.trkstarty[iTrk]             = pos.Y();
        TrackerData.trkstartz[iTrk]             = pos.Z();
        TrackerData.trkstartd[iTrk]             = dpos;
        TrackerData.trkendx[iTrk]               = end.X();
        TrackerData.trkendy[iTrk]               = end.Y();
        TrackerData.trkendz[iTrk]               = end.Z();
        TrackerData.trkendd[iTrk]               = dend;
        TrackerData.trktheta[iTrk]              = dir_start.Theta();
        TrackerData.trkphi[iTrk]                = dir_start.Phi();
        TrackerData.trkstartdcosx[iTrk]         = dir_start.X();
        TrackerData.trkstartdcosy[iTrk]         = dir_start.Y();
        TrackerData.trkstartdcosz[iTrk]         = dir_start.Z();
        TrackerData.trkenddcosx[iTrk]           = dir_end.X();
        TrackerData.trkenddcosy[iTrk]           = dir_end.Y();
        TrackerData.trkenddcosz[iTrk]           = dir_end.Z();
        TrackerData.trkthetaxz[iTrk]            = theta_xz;
        TrackerData.trkthetayz[iTrk]            = theta_yz;
        TrackerData.trkmom[iTrk]                = mom;
        TrackerData.trklen[iTrk]                = tlen;
      } // if we have trajectory

      // find vertices associated with this track
      art::FindMany<recob::Vertex> fmvtx(trackListHandle[iTracker], evt, fVertexModuleLabel[iTracker]);
      if(fmvtx.isValid()) {
        std::vector<const recob::Vertex*> verts = fmvtx.at(iTrk);
        // should have two at most
        for(size_t ivx = 0; ivx < verts.size(); ++ivx) {
          verts[ivx]->XYZ(xyz);
          // find the vertex in TrackerData to get the index
          short theVtx = -1;
          for(short jvx = 0; jvx < TrackerData.nvtx; ++jvx) {
            if(TrackerData.vtx[jvx][2] == xyz[2]) {
              theVtx = jvx;
              break;
            }
          } // jvx
          // decide if it should be assigned to the track Start or End.
          // A simple dz test should suffice
          if(fabs(xyz[2] - TrackerData.trkstartz[iTrk]) < 
             fabs(xyz[2] - TrackerData.trkendz[iTrk])) {
            TrackerData.trksvtxid[iTrk] = theVtx;
          } else {
            TrackerData.trkevtxid[iTrk] = theVtx;
          }
        } // vertices
      } // fmvtx.isValid()
      
      // find particle ID info
      art::FindMany<anab::ParticleID> fmpid(trackListHandle[iTracker], evt, fParticleIDModuleLabel[iTracker]);
      if(fmpid.isValid()) {
        std::vector<const anab::ParticleID*> pids = fmpid.at(iTrk);
        if(pids.size() > 1) {
          mf::LogError("AnalysisTree:limits")
            << "the " << fTrackModuleLabel[iTracker] << " track #" << iTrk
            << " has " << pids.size() 
            << " set of ParticleID variables. Only one stored in the tree";
        }
        TrackerData.trkpidpdg[iTrk] = pids[0]->Pdg();
        TrackerData.trkpidchi[iTrk] = pids[0]->MinChi2();
        TrackerData.trkpidchipr[iTrk] = pids[0]->Chi2Proton();
        TrackerData.trkpidchika[iTrk] = pids[0]->Chi2Kaon();
        TrackerData.trkpidchipi[iTrk] = pids[0]->Chi2Pion();
        TrackerData.trkpidchimu[iTrk] = pids[0]->Chi2Muon();
        TrackerData.trkpidpida[iTrk] = pids[0]->PIDA();
      } // fmpid.isValid()

      art::FindMany<anab::Calorimetry> fmcal(trackListHandle[iTracker], evt, fCalorimetryModuleLabel[iTracker]);
      if (fmcal.isValid()){
        std::vector<const anab::Calorimetry*> calos = fmcal.at(iTrk);
        if (calos.size() > TrackerData.GetMaxPlanesPerTrack(iTrk)) {
          // if you get this message, there is probably a bug somewhere since
          // the calorimetry planes should be 3.
          mf::LogError("AnalysisTree:limits")
            << "the " << fTrackModuleLabel[iTracker] << " track #" << iTrk
            << " has " << calos.size() << " planes for calorimetry , only "
            << TrackerData.GetMaxPlanesPerTrack(iTrk) << " stored in tree";
        }
        for (size_t ipl = 0; ipl<calos.size(); ++ipl){
          TrackerData.trkke[iTrk][ipl]    = calos[ipl]->KineticEnergy();
          TrackerData.trkrange[iTrk][ipl] = calos[ipl]->Range();
          TrackerData.trkpitchc[iTrk][ipl]= calos[ipl] -> TrkPitchC();
          const size_t NHits = calos[ipl] -> dEdx().size();
          TrackerData.ntrkhits[iTrk][ipl] = (int) NHits;
          if (NHits > TrackerData.GetMaxHitsPerTrack(iTrk, ipl)) {
            // if you get this error, you'll have to increase kMaxTrackHits
            mf::LogError("AnalysisTree:limits")
              << "the " << fTrackModuleLabel[iTracker] << " track #" << iTrk
              << " has " << NHits << " hits on calorimetry plane #" << ipl
              <<", only "
              << TrackerData.GetMaxHitsPerTrack(iTrk, ipl) << " stored in tree";
          }
          for(size_t iTrkHit = 0; iTrkHit < NHits && iTrkHit < TrackerData.GetMaxHitsPerTrack(iTrk, ipl); ++iTrkHit) {
            TrackerData.trkdedx[iTrk][ipl][iTrkHit]  = (calos[ipl] -> dEdx())[iTrkHit];
            TrackerData.trkdqdx[iTrk][ipl][iTrkHit]  = (calos[ipl] -> dQdx())[iTrkHit];
            TrackerData.trkresrg[iTrk][ipl][iTrkHit] = (calos[ipl] -> ResidualRange())[iTrkHit];
            const auto& TrkPos = (calos[ipl] -> XYZ())[iTrkHit];
            auto& TrkXYZ = TrackerData.trkxyz[iTrk][ipl][iTrkHit];
            TrkXYZ[0] = TrkPos.X();
            TrkXYZ[1] = TrkPos.Y();
            TrkXYZ[2] = TrkPos.Z();
          } // for track hits
        } // for calorimetry info
      } // if has calorimetry info

      //track truth information
      if (isMC){
        //get the hits on each plane
        art::FindManyP<recob::Hit>      fmht(trackListHandle[iTracker], evt, fTrackModuleLabel[iTracker]);
        std::vector< art::Ptr<recob::Hit> > allHits = fmht.at(iTrk);
        std::vector< art::Ptr<recob::Hit> > hits[kNplanes];

        for(size_t ah = 0; ah < allHits.size(); ++ah){
          if (/* allHits[ah]->WireID().Plane >= 0 && */ // always true
            allHits[ah]->WireID().Plane <  3){
            hits[allHits[ah]->WireID().Plane].push_back(allHits[ah]);
          }
        }
        
        for (size_t ipl = 0; ipl < 3; ++ipl){
          double maxe = 0;
          HitsPurity(hits[ipl],TrackerData.trkidtruth[iTrk][ipl],TrackerData.trkpurtruth[iTrk][ipl],maxe);
        //std::cout<<"\n"<<iTracker<<"\t"<<iTrk<<"\t"<<ipl<<"\t"<<trkidtruth[iTracker][iTrk][ipl]<<"\t"<<trkpurtruth[iTracker][iTrk][ipl]<<"\t"<<maxe;
          if (TrackerData.trkidtruth[iTrk][ipl]>0){
	    const art::Ptr<simb::MCTruth> mc = bt->TrackIDToMCTruth(TrackerData.trkidtruth[iTrk][ipl]);
	    TrackerData.trkorigin[iTrk][ipl] = mc->Origin();
            const simb::MCParticle *particle = bt->TrackIDToParticle(TrackerData.trkidtruth[iTrk][ipl]);
            double tote = 0;
            std::vector<sim::IDE> vide(bt->TrackIDToSimIDE(TrackerData.trkidtruth[iTrk][ipl]));
            for (const sim::IDE& ide: vide) tote += ide.energy;
            TrackerData.trkpdgtruth[iTrk][ipl] = particle->PdgCode();
            TrackerData.trkefftruth[iTrk][ipl] = maxe/(tote/kNplanes); //tote include both induction and collection energies
          //std::cout<<"\n"<<trkpdgtruth[iTracker][iTrk][ipl]<<"\t"<<trkefftruth[iTracker][iTrk][ipl];
          }
        }
      }//end if (isMC)
    }//end loop over track
  }//end loop over track module labels

  //mc truth information
  if (isMC){
    //save neutrino interaction information
    fData->mcevts_truth = mclist.size();
    if (fData->mcevts_truth > 0){//at least one mc record
      if (mctruth->NeutrinoSet()){
        fData->nuPDG_truth = mctruth->GetNeutrino().Nu().PdgCode();
        fData->ccnc_truth = mctruth->GetNeutrino().CCNC();
        fData->mode_truth = mctruth->GetNeutrino().Mode();
        fData->Q2_truth = mctruth->GetNeutrino().QSqr();
        fData->W_truth = mctruth->GetNeutrino().W();
        fData->hitnuc_truth = mctruth->GetNeutrino().HitNuc();
        fData->enu_truth = mctruth->GetNeutrino().Nu().E();
        fData->nuvtxx_truth = mctruth->GetNeutrino().Nu().Vx();
        fData->nuvtxy_truth = mctruth->GetNeutrino().Nu().Vy();
        fData->nuvtxz_truth = mctruth->GetNeutrino().Nu().Vz();
        if (mctruth->GetNeutrino().Nu().P()){
          fData->nu_dcosx_truth = mctruth->GetNeutrino().Nu().Px()/mctruth->GetNeutrino().Nu().P();
          fData->nu_dcosy_truth = mctruth->GetNeutrino().Nu().Py()/mctruth->GetNeutrino().Nu().P();
          fData->nu_dcosz_truth = mctruth->GetNeutrino().Nu().Pz()/mctruth->GetNeutrino().Nu().P();
        }
        fData->lep_mom_truth = mctruth->GetNeutrino().Lepton().P();
        if (mctruth->GetNeutrino().Lepton().P()){
          fData->lep_dcosx_truth = mctruth->GetNeutrino().Lepton().Px()/mctruth->GetNeutrino().Lepton().P();
          fData->lep_dcosy_truth = mctruth->GetNeutrino().Lepton().Py()/mctruth->GetNeutrino().Lepton().P();
          fData->lep_dcosz_truth = mctruth->GetNeutrino().Lepton().Pz()/mctruth->GetNeutrino().Lepton().P();
        }
        //flux information
        art::Ptr<simb::MCFlux>  mcflux = fluxlist[imc];
        fData->tpx_flux = mcflux->ftpx;
        fData->tpy_flux = mcflux->ftpy;
        fData->tpz_flux = mcflux->ftpz;
        fData->tptype_flux = mcflux->ftptype;

        //genie particles information
        fData->genie_no_primaries = mctruth->NParticles();

        size_t StoreParticles = std::min((size_t) fData->genie_no_primaries, fData->GetMaxGeniePrimaries());
        if (fData->genie_no_primaries > (int) StoreParticles) {
          // got this error? it might be a bug,
          // since the structure should have enough room for everything
          mf::LogError("AnalysisTree:limits") << "event has "
            << fData->genie_no_primaries << " MC particles, only "
            << StoreParticles << " stored in tree";
        }
        for(size_t iPart = 0; iPart < StoreParticles; ++iPart){
          const simb::MCParticle& part(mctruth->GetParticle(iPart));
          fData->genie_primaries_pdg[iPart]=part.PdgCode();
          fData->genie_Eng[iPart]=part.E();
          fData->genie_Px[iPart]=part.Px();
          fData->genie_Py[iPart]=part.Py();
          fData->genie_Pz[iPart]=part.Pz();
          fData->genie_P[iPart]=part.Px();
          fData->genie_status_code[iPart]=part.StatusCode();
          fData->genie_mass[iPart]=part.Mass();
          fData->genie_trackID[iPart]=part.TrackId();
          fData->genie_ND[iPart]=part.NumberDaughters();
          fData->genie_mother[iPart]=part.Mother();
        } // for particle
      } //if neutrino set

      //GEANT particles information
      const sim::ParticleList& plist = bt->ParticleList();
      
      std::string pri("primary");
      int primary=0;
      int geant_particle=0;
      size_t iFVPart = 0;
      for(size_t iPart = 0; iPart < plist.size(); ++iPart)
      {
        const simb::MCParticle* pPart = plist.Particle(iPart);
        if (!pPart) {
          throw art::Exception(art::errors::LogicError)
            << "GEANT particle #" << iPart << " returned a null pointer";
        }
        
        ++geant_particle;
        bool isPrimary = pPart->Process() == pri;
        if (isPrimary) ++primary;
        
        if (iPart < fData->GetMaxGEANTparticles()) {
          fData->process_primary[iPart] = int(isPrimary);
          fData->Mother[iPart]=pPart->Mother();
          fData->TrackId[iPart]=pPart->TrackId();
          fData->pdg[iPart]=pPart->PdgCode();
          fData->Eng[iPart]=pPart->E();
          fData->Px[iPart]=pPart->Px();
          fData->Py[iPart]=pPart->Py();
          fData->Pz[iPart]=pPart->Pz();
          fData->StartPointx[iPart]=pPart->Vx();
          fData->StartPointy[iPart]=pPart->Vy();
          fData->StartPointz[iPart]=pPart->Vz();
          fData->EndPointx[iPart]=pPart->EndPosition()[0];
          fData->EndPointy[iPart]=pPart->EndPosition()[1];
          fData->EndPointz[iPart]=pPart->EndPosition()[2];
          fData->NumberDaughters[iPart]=pPart->NumberDaughters();
        }
        else if (iPart == fData->GetMaxGEANTparticles()) {
          // got this error? it might be a bug,
          // since the structure should have enough room for everything
          mf::LogError("AnalysisTree:limits") << "event has "
            << plist.size() << " MC particles, only "
            << fData->GetMaxGEANTparticles() << " will be stored in tree";
        }
        
        // is it fiducial?
        TVector3 mcstart, mcend;
        double plen = length(*pPart, mcstart, mcend);
        if ( (plen==0) || pPart->PdgCode() > 10000) continue;
        
        if (iFVPart < fData->GetMaxGEANTparticlesInTPCFV()) {
          double mctheta_xz = std::atan2(pPart->Px(), pPart->Pz());
          double mctheta_yz = std::atan2(pPart->Py(), pPart->Pz());
          
          fData->geant_tpcFV_pdg[iFVPart]         = pPart->PdgCode();
          fData->geant_tpcFV_trackId[iFVPart]     = pPart->TrackId();
          fData->geant_tpcFV_status[iFVPart]      = pPart->StatusCode();
          //
          fData->geant_tpcFV_orig_E[iFVPart]      = pPart->E();
          fData->geant_tpcFV_orig_px[iFVPart]     = pPart->Px();
          fData->geant_tpcFV_orig_py[iFVPart]     = pPart->Py();
          fData->geant_tpcFV_orig_pz[iFVPart]     = pPart->Pz();
          fData->geant_tpcFV_orig_startx[iFVPart] = pPart->Vx();
          fData->geant_tpcFV_orig_starty[iFVPart] = pPart->Vy();
          fData->geant_tpcFV_orig_startz[iFVPart] = pPart->Vz();
          fData->geant_tpcFV_orig_startt[iFVPart] = pPart->T();
          fData->geant_tpcFV_orig_endx[iFVPart]   = pPart->EndX();
          fData->geant_tpcFV_orig_endy[iFVPart]   = pPart->EndY();
          fData->geant_tpcFV_orig_endz[iFVPart]   = pPart->EndZ();
          fData->geant_tpcFV_orig_endt[iFVPart]   = pPart->EndT();
          //
          fData->geant_tpcFV_startx[iFVPart]      = mcstart.X();
          fData->geant_tpcFV_starty[iFVPart]      = mcstart.Y();
          fData->geant_tpcFV_startz[iFVPart]      = mcstart.Z();
          fData->geant_tpcFV_endx[iFVPart]        = mcend.X();
          fData->geant_tpcFV_endy[iFVPart]        = mcend.Y();
          fData->geant_tpcFV_endz[iFVPart]        = mcend.Z();
          fData->geant_tpcFV_theta[iFVPart]       = pPart->Momentum().Theta();
          fData->geant_tpcFV_phi[iFVPart]         = pPart->Momentum().Phi();
          fData->geant_tpcFV_theta_xz[iFVPart]    = mctheta_xz;
          fData->geant_tpcFV_theta_yz[iFVPart]    = mctheta_yz;
          fData->geant_tpcFV_mom[iFVPart]         = pPart->Momentum().Vect().Mag();
          fData->geant_tpcFV_len[iFVPart]         = plen;
        }
        else if (iFVPart == fData->GetMaxGEANTparticlesInTPCFV()) {
          // got this error? it might be a bug,
          // since the structure should have enough room for everything
          mf::LogError("AnalysisTree:limits") << "event has "
            << plist.size() << " MC particles in fiducial volume, only "
            << fData->GetMaxGEANTparticlesInTPCFV() << " will be stored in tree";
        }
        ++iFVPart;
      } // for particles
      
      fData->geant_list_size_in_tpcFV = iFVPart;
      fData->no_primaries = primary;
      fData->geant_list_size = geant_particle;
      
      LOG_DEBUG("AnalysisTree") << "Counted "
        << fData->geant_list_size << " GEANT particles ("
        << fData->geant_list_size_in_tpcFV << " in FV), "
        << fData->no_primaries << " primaries, "
        << fData->genie_no_primaries << " GENIE particles";
      
      FillWith(fData->MergedId, 0);

      int currentMergedId = 1;
      for(size_t iPart = fData->geant_list_size; iPart-- > 0; ) {
        if (fData->MergedId[iPart]) continue;
        fData->MergedId[iPart] = currentMergedId;
        int currentMotherId = fData->Mother[iPart];
        int currentMotherTrackId = -1;
        while(currentMotherId > 0) {
          size_t iMother = fData->TrackId.size();
          while (iMother-- > 0) {
            if (fData->TrackId[iMother] != currentMotherId) continue;
            currentMotherTrackId = iMother;
            break;
          } // while
          if (fData->pdg[iPart] != fData->pdg[currentMotherTrackId]) break;
          
          fData->MergedId[currentMotherTrackId] = currentMergedId;
          currentMotherId = fData->Mother[currentMotherTrackId];
        }
        ++currentMergedId;
      } // for merging check
      
    }//if (mcevts_truth){//at least one mc record
  }//if (isMC){
  fData->taulife = LArProp->ElectronLifetime();
  fTree->Fill();
  
  if (mf::isDebugEnabled()) {
    // use mf::LogDebug instead of LOG_DEBUG because we reuse it in many lines;
    // thus, we protect this part of the code with the line above
    mf::LogDebug logStream("AnalysisTreeStructure");
    logStream
      << "Tree data structure contains:"
      << "\n - " << fData->no_hits << " hits (" << fData->GetMaxHits() << ")"
      << "\n - " << fData->genie_no_primaries << " genie primaries (" << fData->GetMaxGeniePrimaries() << ")"
      << "\n - " << fData->geant_list_size << " GEANT particles (" << fData->GetMaxGEANTparticles() << "), "
        << fData->no_primaries << " primaries"
      << "\n - " << fData->geant_list_size_in_tpcFV << " GEANT particles in FV (" << fData->GetMaxGEANTparticlesInTPCFV() << ")"
      << "\n - " << ((int) fData->kNTracker) << " trackers:"
      ;
    
    size_t iTracker = 0;
    for (auto tracker = fData->TrackData.cbegin();
      tracker != fData->TrackData.cend(); ++tracker, ++iTracker
    ) {
      logStream
         << "\n -> " << tracker->ntracks << " " << fTrackModuleLabel[iTracker]
           << " tracks (" << tracker->GetMaxTracks() << ")"
         ;
      for (int iTrk = 0; iTrk < tracker->ntracks; ++iTrk) {
        logStream << "\n    [" << iTrk << "] "<< tracker->ntrkhits[iTrk][0];
        for (size_t ipl = 1; ipl < tracker->GetMaxPlanesPerTrack(iTrk); ++ipl)
          logStream << " + " << tracker->ntrkhits[iTrk][ipl];
        logStream << " hits (" << tracker->GetMaxHitsPerTrack(iTrk, 0);
        for (size_t ipl = 1; ipl < tracker->GetMaxPlanesPerTrack(iTrk); ++ipl)
          logStream << " + " << tracker->GetMaxHitsPerTrack(iTrk, ipl);
        logStream << ")";
      } // for tracks
    } // for trackers
  } // if logging enabled
  
  // if we don't use a permanent buffer (which can be huge),
  // delete the current buffer, and we'll create a new one on the next event
  if (!fUseBuffer) {
    LOG_DEBUG("AnalysisTreeStructure") << "Freeing the tree data structure";
    DestroyData();
  }
} // microboone::AnalysisTree::analyze()

void microboone::AnalysisTree::HitsPurity(std::vector< art::Ptr<recob::Hit> > const& hits, Int_t& trackid, Float_t& purity, double& maxe){

  trackid = -1;
  purity = -1;

  art::ServiceHandle<cheat::BackTracker> bt;

  std::map<int,double> trkide;

  for(size_t h = 0; h < hits.size(); ++h){

    art::Ptr<recob::Hit> hit = hits[h];
    std::vector<sim::IDE> ides;
    //bt->HitToSimIDEs(hit,ides);
    std::vector<cheat::TrackIDE> eveIDs = bt->HitToEveID(hit);

    for(size_t e = 0; e < eveIDs.size(); ++e){
      //std::cout<<h<<" "<<e<<" "<<eveIDs[e].trackID<<" "<<eveIDs[e].energy<<" "<<eveIDs[e].energyFrac<<std::endl;
      trkide[eveIDs[e].trackID] += eveIDs[e].energy;
    }
  }

  maxe = -1;
  double tote = 0;
  for (std::map<int,double>::iterator ii = trkide.begin(); ii!=trkide.end(); ++ii){
    tote += ii->second;
    if ((ii->second)>maxe){
      maxe = ii->second;
      trackid = ii->first;
    }
  }

  //std::cout << "the total energy of this reco track is: " << tote << std::endl;

  if (tote>0){
    purity = maxe/tote;
  }
}

// Calculate distance to boundary.
double microboone::AnalysisTree::bdist(const TVector3& pos)
{
  // Get geometry.
  art::ServiceHandle<geo::Geometry> geom;

  double d1 = pos.X();                             // Distance to right side (wires).
  double d2 = 2.*geom->DetHalfWidth() - pos.X();   // Distance to left side (cathode).
  double d3 = pos.Y() + geom->DetHalfHeight();     // Distance to bottom.
  double d4 = geom->DetHalfHeight() - pos.Y();     // Distance to top.
  double d5 = pos.Z();                             // Distance to front.
  double d6 = geom->DetLength() - pos.Z();           // Distance to back.

  double result = std::min(std::min(std::min(std::min(std::min(d1, d2), d3), d4), d5), d6);
  return result;
}

// Length of reconstructed track, trajectory by trajectory.
double microboone::AnalysisTree::length(const recob::Track& track)
{
  double result = 0.;
  TVector3 disp = track.LocationAtPoint(0);
  int n = track.NumberTrajectoryPoints();

  for(int i = 1; i < n; ++i) {
    const TVector3& pos = track.LocationAtPoint(i);
    //double momentum = track.MomentumAtPoint(i);
    //std::cout<<"\n"<<i<<"\t"<<momentum<<"\n";
    disp -= pos;
    result += disp.Mag();
    disp = pos;
  }
  return result;
}

// Length of MC particle, trajectory by trajectory.
double microboone::AnalysisTree::length(const simb::MCParticle& part, TVector3& start, TVector3& end)
{
  // Get geometry.
  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<util::DetectorProperties> detprop;

  // Get fiducial volume boundary.
  //double xmin = 0.;
  double xmax = 2.*geom->DetHalfWidth();
  double ymin = -geom->DetHalfHeight();
  double ymax = geom->DetHalfHeight();
  double zmin = 0.;
  double zmax = geom->DetLength();

  //const double fSamplingRate = 500;
  //const double fReadOutWindowSize = 3200;
  double        vDrift = 160*pow(10,-6);

  double result = 0.;
  TVector3 disp;
  int n = part.NumberTrajectoryPoints();
  bool first = true;

  for(int i = 0; i < n; ++i) {
    try{
      // check if the particle is inside a TPC  											
      double mypos[3] = {part.Vx(i), part.Vy(i), part.Vz(i)};
      unsigned int tpc   = 0;
      unsigned int cstat = 0;
      geom->PositionToTPC(mypos, tpc, cstat);
    }
    catch(cet::exception &e){
      continue;
    }
    
    double xGen   = part.Vx(i);
    double tGen   = part.T(i);
    //double tDrift = xGen/vDrift;
    
    
//std::cout<<"\n"<<xGen<<"\t"<<tGen<<"\t"<<(-xmax-tGen*vDrift)<<"\t"<<((2*xmax)-(tGen*vDrift));									
       
    if (xGen < (-xmax-tGen*vDrift) || xGen > ((2*xmax)-tGen*vDrift) ) continue;
    if (part.Vy(i) < ymin || part.Vy(i) > ymax) continue;
    if (part.Vz(i) < zmin || part.Vz(i) > zmax) continue;
    // Doing some manual shifting to account for											
    // an interaction not occuring with the beam dump											
    // we will reconstruct an x distance different from 										
    // where the particle actually passed to to the time										
    // being different from in-spill interactions	
    double newX = xGen+tGen*vDrift;	
    

    TVector3 pos(newX,part.Vy(i),part.Vz(i));
    
    if(first){
      start = pos;
    }
    else {
      disp -= pos;
      result += disp.Mag();
    }
    first = false;
    disp = pos;
    end = pos;
  }
  return result;
}


namespace microboone{

  DEFINE_ART_MODULE(AnalysisTree)

}

#endif
