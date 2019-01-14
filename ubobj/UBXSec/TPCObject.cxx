#include "TPCObject.h"
#include <vector>

namespace ubana {

  TPCObject::TPCObject() {
    fOrigin      = ubana::kUnknown;
    fOriginExtra = ubana::kNotSet;
  }

  TPCObject::~TPCObject(){
  }

  // Setter methoths
  void TPCObject::SetTracks      (std::vector<recob::Track> tracks)    { this->fTracks = tracks;    }
  void TPCObject::SetPFPs        (std::vector<recob::PFParticle> pfps) { this->fPFParticles = pfps; }
  void TPCObject::SetVertex      (recob::Vertex vertex)                { this->fVertex = vertex;    }
  void TPCObject::SetOrigin      (ubana::TPCObjectOrigin origin)       { this->fOrigin = origin;    }
  void TPCObject::SetOriginExtra (ubana::TPCObjectOriginExtra origin)  { this->fOriginExtra = origin;    }
  void TPCObject::SetMultiplicity(int p, int t, int s)                 { this->fPfpMult = p; this->fTrackMult = t; this->fShowerMult = s;}
// void TPCObject::SetTrackScore(std::vector<double> score)                 { this->fPfpScore = score;}

  // Getter methods
  const std::vector<recob::Track>       & TPCObject::GetTracks()      const { return this->fTracks;      }
  const std::vector<recob::PFParticle>  & TPCObject::GetPFPs()        const { return this->fPFParticles; }
  const recob::Vertex                   & TPCObject::GetVertex()      const { return this->fVertex;      }
  const ubana::TPCObjectOrigin          & TPCObject::GetOrigin()      const { return this->fOrigin;      }
  const ubana::TPCObjectOriginExtra     & TPCObject::GetOriginExtra() const { return this->fOriginExtra; }


  const size_t TPCObject::GetNTracks()  const { return (this->fTracks).size();      }
  const size_t TPCObject::GetNPFP()     const { return (this->fPFParticles).size(); }
  const size_t TPCObject::GetNShowers() const { return (this->fShowers).size();     }

  const void TPCObject::GetMultiplicity(int &p, int &t, int &s) const { p = this->fPfpMult; t = this->fTrackMult; s = this->fShowerMult;}
// const void TPCObject::GetTrackScore(std::vector<double> &score) const { score = this->fPfpScore;}

  const int  TPCObject::GetNTracksCloseToVertex(double tolerance) const {

    if (tolerance < 0.) {
      std::cerr << "[TPCObject] tolerance can't be less than zero" << std::endl;
      throw std::exception();
    }

    // Output
    int multiplicity = 0;

    // Vertex
//    double v[3];
  //  this->fVertex.XYZ(v);
    //TVector3 vtx(v[0], v[1], v[2]);
const auto& vtx = this->fVertex.position();
    // Loop over tracks and calulate multiplicity
    for (auto t : this->fTracks) {

   //   TVector3 start = t.Vertex();
     // TVector3 end = t.End();

      //if ( (vtx-start).Mag() < tolerance   ||   (vtx-end).Mag() < tolerance ) {
     const auto& start = t.Vertex();
const auto& end = t.End();
if ( (vtx-start).R() < tolerance   ||   (vtx-end).R() < tolerance ) {
     multiplicity++;
      }
    }

    return multiplicity;
  }


  std::string TPCObjectOriginToString(TPCObjectOrigin val){
    switch (val){
      case kUnknown:
        return "kUnknown";
        break;
      case kBeamNeutrino:
        return "kBeamNeutrino";
        break;
      case kCosmicRay:
        return "kCosmicRay";
        break;
      case kMixed:
        return "kMixed";
        break;
      default:
        return "???";
    }
  }

  std::string TPCObjectOriginExtraToString(TPCObjectOriginExtra val){
    switch (val){
      case kNotSet:
        return "kNotSet";
        break;
      case kStoppingMuon:
        return "kStoppingMuon";
        break;
      case kACPT:
        return "kACPT";
        break;
      case kNCPion:
        return "kNCPion";
        break;
      case kNCProton:
        return "kNCProton";
        break;
      default:
        return "???";
    }
  }
}
