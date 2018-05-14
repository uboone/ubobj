////////////////////////////////////////////////////////////////////////
// Class:       MergeTest
// Module Type: analyzer
// File:        MergeTest_module.cc
//
// Generated at Mon Jul  3 03:51:03 2017 by David Lorca Galindo using artmod
// from cetpkgsupport v1_11_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardata/Utilities/AssociationUtil.h"

#include <artdaq-core/Data/Fragment.hh>

#include "art/Framework/Services/Optional/TFileService.h"

#include "ubooneobj/CRT/CRTHit.hh"
#include "ubooneobj/CRT/CRTTrack.hh"
#include "uboone/CRT/CRTAuxFunctions.hh"
#include "ubooneobj/RawData/DAQHeaderTimeUBooNE.h"

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3S.h"
#include "TProfile.h"
#include "TF1.h"
#include "TDatime.h"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <typeinfo>


namespace crt {
  class MergeTest;
}

class crt::MergeTest : public art::EDAnalyzer {
public:
  explicit MergeTest(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  MergeTest(MergeTest const &) = delete;
  MergeTest(MergeTest &&) = delete;
  MergeTest & operator = (MergeTest const &) = delete;
  MergeTest & operator = (MergeTest &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;
  
  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:

  art::ServiceHandle<art::TFileService> tfs;
  // Declare member data here.
  
  uint32_t fEvtNum; //Number of current event                       
  uint32_t frunNum;                //Run Number taken from event  
  uint32_t fsubRunNum;             //Subrun Number taken from event         
  std::string  data_labeltrack_;
  std::string  data_labelhit_;
  std::string  data_label_flash_;
  std::string  data_label_DAQHeader_;
  int fHardDelay_;
  int verbose_;


  //art::InputTag opFlashTag("opflashSat");

  //TTree*       fTree;
  TH1F* hFlashTimeDis;
  TH1F* hFlashTimeDis_b0;
  TH1F* hTFvsTH_t1;
  TH2F* hTFvsTH_t1_2d;
  TH1F* hTFvsTH_t0;
  TH2F* hTFvsTH_t0_2d;
  TH2F* hTFvsTH_t0_t1;
  TH2F* hTFvsTH_plane_t0;

  TH2F* hNFlavsNHit;

  TH2F* hBot;
  TH2F* hFT;
  TH2F* hPipe;
  TH2F* hTop;

  TH1F* hYdiff;
  TH1F* hZdiff;

  TH1F* hNHitperFla;
  TH1F* hNHitperFla0;
  TH1F* hNHitperFla1;
  TH1F* hNHitperFla2;
  TH1F* hNHitperFla3;
  TH2F* hNHitperFla2D;

  TH1F* hNTraperFla;

  TH2F* hTra_tl_len;

  
  /* //for Tree
  uint32_t fTriTim_sec;
  uint32_t fTriTim_nsec;
  double fY;
  double fZ;
  double fTimFla;
  double fAbsTimFla;
  
  //for Tree*/

  
};


crt::MergeTest::MergeTest(fhicl::ParameterSet const & p)
  : EDAnalyzer(p),
    data_labeltrack_(p.get<std::string>("data_labeltrack")),
    data_labelhit_(p.get<std::string>("data_labelhit")),
    data_label_flash_(p.get<std::string>("data_label_flash_")),
    data_label_DAQHeader_(p.get<std::string>("data_label_DAQHeader_")),
    fHardDelay_(p.get<int>("fHardDelay",40000)),
    verbose_(p.get<int>("verbose"))
    // More initializers here.
{
}

void crt::MergeTest::analyze(art::Event const & evt)
{
  // Implementation of required member function here.
  
  frunNum    = evt.run();
  fsubRunNum = evt.subRun();
  fEvtNum = evt.event();
  
  art::Timestamp evtTime = evt.time();
  auto evt_time_sec = evtTime.timeHigh();
  auto evt_time_nsec = evtTime.timeLow();

  //  double  evt_timeGPS_sec = evt_time_sec;
  //double  evt_timeGPS_nsec = evt_time_nsec;
  
  //get DAQ Header                                                                  
  //Commentar para old swizzler, sin DAQ Header
  art::Handle< raw::DAQHeaderTimeUBooNE > rawHandle_DAQHeader;  
  evt.getByLabel(data_label_DAQHeader_, rawHandle_DAQHeader);
  
  //check to make sure the data we asked for is valid 
  if(!rawHandle_DAQHeader.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
	      << ", event " << evt.event() << " has zero"
	      << " DAQHeaderTimeUBooNE  " << " in with label " << data_label_DAQHeader_ << std::endl;    
    return;
  }
  
  
  raw::DAQHeaderTimeUBooNE const& my_DAQHeader(*rawHandle_DAQHeader);
  art::Timestamp evtTimeGPS = my_DAQHeader.gps_time();  
  double evt_timeGPS_sec = evtTimeGPS.timeHigh();
  double evt_timeGPS_nsec = evtTimeGPS.timeLow();
  art::Timestamp evtTimeNTP = my_DAQHeader.ntp_time();
  double evt_timeNTP_sec = evtTimeNTP.timeHigh();
  double evt_timeNTP_nsec = evtTimeNTP.timeLow();
  double timstp_diff = std::abs(evt_timeGPS_nsec - evt_timeNTP_nsec);
  
  if(verbose_==1){
    std::cout<< "Run:  "<<frunNum << "   subRun: " <<fsubRunNum<<std::endl;
    std::cout<<"event: "<<fEvtNum <<std::endl;
    std::cout.precision(19);
    std::cout<<"  GPS time second:  "<<evt_timeGPS_sec<<std::endl;
    std::cout<<"  GPS time nano_second:  "<<evt_timeGPS_nsec<<std::endl;
    std::cout<<"  NTP time second:  "<<evt_timeNTP_sec<<std::endl;    
    std::cout<<"  NTP time nano_second:  "<<evt_timeNTP_nsec<<std::endl;
    std::cout<<"  event time second:  "<<evt_time_sec<<std::endl;
    std::cout<<"  event time nano_second:  "<<evt_time_nsec<<std::endl;
    std::cout<<"  difference between GPS and NTP:  "<<evt_timeGPS_nsec - evt_timeNTP_nsec<<" ns"<<std::endl;
    std::cout<<"  ABS difference between GPS and NTP:  "<<timstp_diff<<" ns"<<std::endl;
    
    if( (evt_time_sec==evt_timeGPS_sec) && (evt_time_nsec==evt_timeGPS_nsec))  std::cout<<" Event time type is: GPS  "<<std::endl;
    if( (evt_time_sec==evt_timeNTP_sec) && (evt_time_nsec==evt_timeNTP_nsec))  std::cout<<" Event time type is: NTP  "<<std::endl;
    //getchar();
  }  
  
  //Comentar para old swizzle sin header

  
  //get Optical Flash
  art::Handle< std::vector<recob::OpFlash> > rawHandle_OpFlash;
  evt.getByLabel(data_label_flash_, rawHandle_OpFlash);
  
  std::vector<recob::OpFlash> const& OpFlashCollection(*rawHandle_OpFlash);
  
  if(verbose_==1){ 
    std::cout<<"  OpFlashCollection.size()  "<<OpFlashCollection.size()<<std::endl; 
  }
  
  //get Optical Flash
  
  
  //get CRTHits
  art::Handle< std::vector<crt::CRTHit> > rawHandle_hit;
  evt.getByLabel(data_labelhit_, rawHandle_hit); //
  
  //check to make sure the data we asked for is valid
  if(!rawHandle_hit.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << evt.event() << " has zero"
              << " CRTHits " << " in module " << data_labelhit_ << std::endl;
    std::cout << std::endl;
    return;
  }
  
  //get better access to the data    //CRTHit collection on this event                                                
  std::vector<crt::CRTHit> const& CRTHitCollection(*rawHandle_hit);
  
  if(verbose_==1){ 
    std::cout<<"  CRTHitCollection.size()  "<<CRTHitCollection.size()<<std::endl; 
    //  getchar();   
  }
  //get CRTHits

  /*  
  //get CRTTracks
  art::Handle< std::vector<crt::CRTTrack> > rawHandle_track;
  evt.getByLabel(data_labeltrack_, rawHandle_track); 
  
  //check to make sure the data we asked for is valid
  if(!rawHandle_track.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << evt.event() << " has zero"
              << " CRTTracks " << " in module " << data_labeltrack_ << std::endl;
    std::cout << std::endl;
    return;
  }
  
  //get better access to the data    //CRTTrack collection on this event                                                
  std::vector<crt::CRTTrack> const& CRTTrackCollection(*rawHandle_track);
  
  if(verbose_==1){ 
    std::cout<<"  CRTTrackCollection.size()  "<<CRTTrackCollection.size()<<std::endl; 
    getchar();   
  }
  //get CRTTracks
  */

  
  if(CRTHitCollection.size()>0){//A
    
    hNFlavsNHit->Fill(OpFlashCollection.size(),CRTHitCollection.size());

    int Hmatchcounter0=0;
    int Hmatchcounter1=0;
    int Hmatchcounter2=0;
    int Hmatchcounter3=0;
    
    
    for(std::vector<int>::size_type i = 0; i != OpFlashCollection.size(); i++) {//B
      
      recob::OpFlash my_OpFlash = OpFlashCollection[i];
      
      auto Yflash = my_OpFlash.YCenter();
      auto Zflash = my_OpFlash.ZCenter();
      auto PEflash = my_OpFlash.TotalPE();
      auto Timeflash = my_OpFlash.Time(); //in us from trigger time
      auto Timeflash_ns = (Timeflash * 1000);
      auto Timeflash_ns_GPS = evt_timeGPS_nsec + (Timeflash * 1000);      
      //int flash_time_GPS_ns = evt_timeGPS_nsec + (Timeflash * 1000);  //do not trust on this      
      int fbeam = my_OpFlash.OnBeamTime();
      uint32_t Flash_sec = evt_timeGPS_sec;
      
      hFlashTimeDis->Fill(Timeflash);
      
      if(verbose_==1){ 
	std::cout<<"event: "<<fEvtNum<<std::endl;
	std::cout<<"Flash: "<<i<<std::endl;
	std::cout<<"Beam: "<<fbeam<<std::endl;
	std::cout<<"Zflash: "<<Zflash<<std::endl;
	std::cout<<"Yflash: "<<Yflash<<std::endl;
	std::cout<<"PEflash: "<<PEflash<<std::endl;
	std::cout.precision(19);
	std::cout<<" "<<std::endl;
	std::cout<<"Flash time: "<<Flash_sec<< " seconds"<<std::endl;
	std::cout<<"Flash time: "<<Timeflash<< "  us w.r.t to trigger"<<std::endl;
	std::cout<<"Flash time: "<<Timeflash_ns<< "  ns w.r.t to trigger"<<std::endl;
	std::cout<<"Flash time: "<<Timeflash_ns_GPS<< "  ns in GPS"<<std::endl;
      	std::cout<<" "<<std::endl;
      }
      
      
            
      
      if(fbeam == 0){//C solo onbeam
	int Hmatchcounter=0;

	hFlashTimeDis_b0->Fill(Timeflash);
	
	for(std::vector<int>::size_type j = 0; j != CRTHitCollection.size(); j++) {//D
	  
	  crt::CRTHit my_CRTHit = CRTHitCollection[j];
	  
          int Hit_sec = my_CRTHit.ts0_s;
          
          int Hit_T1_nsec = my_CRTHit.ts1_ns + fHardDelay_;
          int Hit_T0_nsec = my_CRTHit.ts0_ns;
	  
          int diff_sec = Flash_sec - Hit_sec;
          int diff_secABS = std::abs(diff_sec);
	  
          int diffT1_nsec = Timeflash_ns - Hit_T1_nsec;
          int diffT1_nsecABS = std::abs(diffT1_nsec);
	  
          int diffT0_nsec = Timeflash_ns_GPS - Hit_T0_nsec;
          int diffT0_nsecABS = std::abs(diffT0_nsec);
	  
	 	 	  
	  //if( (diff_secABS<3)  &&  (diffT1_nsecABS<1000 )  ){//E                                                                                        
	  //if( (diff_secABS<1)  &&  (diffT1_nsecABS<600 ) &&  (diffT1_nsecABS>450 )  ){//E
	  if( (diff_secABS<1)    ){//E
	    
	    Hmatchcounter++;          
	    

            hTFvsTH_t1->Fill(diffT1_nsec);
            hTFvsTH_t1_2d->Fill(diff_sec , diffT1_nsecABS);
	    
            hTFvsTH_t0->Fill(diffT0_nsec);
	    hTFvsTH_t0_2d->Fill(diff_sec , diffT0_nsecABS);	                

	    hTFvsTH_t0_t1->Fill(diffT0_nsecABS, diffT1_nsecABS);
            hTFvsTH_plane_t0->Fill(my_CRTHit.plane, diffT0_nsecABS);
	    
	    if(my_CRTHit.plane == 0){
	      Hmatchcounter0++;
	      hBot->Fill(my_CRTHit.z_pos, my_CRTHit.x_pos);
	    }
	    if(my_CRTHit.plane == 1){
	      Hmatchcounter1++;
	      hFT->Fill(my_CRTHit.z_pos, my_CRTHit.y_pos);
	    }
	    if(my_CRTHit.plane == 2){
	      Hmatchcounter2++;
	      hPipe->Fill(my_CRTHit.z_pos, my_CRTHit.y_pos);
	    }
	    if(my_CRTHit.plane == 3){
	      Hmatchcounter3++;
	      hTop->Fill(my_CRTHit.z_pos, my_CRTHit.x_pos);  	    
	    }

            if(verbose_==1){
	      std::cout<<"Flash_sec - Hit_sec: "<<diff_sec<<std::endl;
	      std::cout<<"ABS( Flash_sec - Hit_sec ): "<<diff_secABS<<std::endl;
	      std::cout<<" "<<std::endl;
	      std::cout<<"Hit_ns(T1): "<<Hit_T1_nsec<<std::endl;
	      std::cout<<"Flash_nsec(w.r.t. Trigger): "<<Timeflash_ns<<std::endl;
	      std::cout<<"     Flash_nsec - Hit_nsec(T1)  : "<<diffT1_nsec<<std::endl;
	      std::cout<<"ABS( Flash_nsec - Hit_nsec(T1) ): "<<diffT1_nsecABS<<std::endl;
	      std::cout<<" "<<std::endl;
	      std::cout<<"Hit_ns(T0): "<<Hit_T0_nsec<<std::endl;
	      std::cout<<"Flash_nsec(GPS units): "<<Timeflash_ns_GPS<<std::endl;
	      std::cout<<"     Flash_nsec - Hit_nsec(T0)  : "<<diffT0_nsec<<std::endl;
	      std::cout<<"ABS( Flash_nsec - Hit_nsec(T0) ): "<<diffT0_nsecABS<<std::endl;
              getchar();
            }
	    
	  }//E 
	}//D
	//std::cout<<"Hmatchcounter: "<<Hmatchcounter<<std::endl;
	//getchar();
	hNHitperFla->Fill(Hmatchcounter);

	/*
	int Tmatchcounter=0;
	for(std::vector<int>::size_type a = 0; a != CRTTrackCollection.size(); a++) {//T

	  crt::CRTTrack my_CRTTrack = CRTTrackCollection[a];
	  
	  int Track_sec = my_CRTTrack.ts0_s;
	  
          int Track_T1_nsec = my_CRTTrack.ts1_ns + fHardDelay_;
          int Track_T0_nsec = my_CRTTrack.ts0_ns;
	  
          int Tdiff_sec = Flash_sec - Track_sec;
          int Tdiff_secABS = std::abs(Tdiff_sec);
	  
          int TdiffT1_nsec = Timeflash_ns - Track_T1_nsec;
          int TdiffT1_nsecABS = std::abs(TdiffT1_nsec);
	  
          int TdiffT0_nsec = Timeflash_ns_GPS - Track_T0_nsec;
          int TdiffT0_nsecABS = std::abs(TdiffT0_nsec);


	  double Tlength = my_CRTTrack.length;
	  double Tra_tl = my_CRTTrack.ts0_ns_h1 - my_CRTTrack.ts0_ns_h2;
	  Tra_tl = std::abs(Tra_tl);
	  hTra_tl_len->Fill(Tlength, Tra_tl);
	  
	  double TY = my_CRTTrack.y1_pos - my_CRTTrack.y2_pos; //correct by Y position in TPC
	  double TZ = my_CRTTrack.z1_pos - my_CRTTrack.z2_pos; //correct by Z position in TPC	  

	  double hY = TY - Yflash;
	  double hZ = TZ - Zflash;

	  //if( (Tdiff_secABS<3)  &&  (TdiffT1_nsecABS<1000 )  ){//V
	  if( (Tdiff_secABS<1)  &&  (TdiffT1_nsecABS<600 ) &&  (TdiffT1_nsecABS>450 )  ){//C
	    Tmatchcounter++;
	   
	    hZdiff->Fill(hZ);
	    hYdiff->Fill(hY);


            if(verbose_==1){
	      std::cout.precision(19);
	      std::cout<<"Flash_sec - Track_sec: "<<Tdiff_sec<<std::endl;
	      std::cout<<"ABS( Flash_sec - Track_sec ): "<<Tdiff_secABS<<std::endl;
	      std::cout<<" "<<std::endl;
	      std::cout<<"Track_ns(T1): "<<Track_T1_nsec<<std::endl;
	      std::cout<<"Flash_nsec(w.r.t. Trigger): "<<Timeflash_ns<<std::endl;
	      std::cout<<"     Flash_nsec - Track_nsec(T1)  : "<<TdiffT1_nsec<<std::endl;
	      std::cout<<"ABS( Flash_nsec - Track_nsec(T1) ): "<<TdiffT1_nsecABS<<std::endl;
	      std::cout<<" "<<std::endl;
	      std::cout<<"Track_ns(T0): "<<Track_T0_nsec<<std::endl;
	      std::cout<<"Flash_nsec(GPS units): "<<Timeflash_ns_GPS<<std::endl;
	      std::cout<<"     Flash_nsec - Track_nsec(T0)  : "<<TdiffT0_nsec<<std::endl;
	      std::cout<<"ABS( Flash_nsec - Track_nsec(T0) ): "<<TdiffT0_nsecABS<<std::endl;
	      getchar();
	    }




	  }//V

	}//T
	hNTraperFla->Fill(Tmatchcounter);
	*/

      }//C
    }//B  

    hNHitperFla0->Fill(Hmatchcounter0);
    hNHitperFla1->Fill(Hmatchcounter1);
    hNHitperFla2->Fill(Hmatchcounter2);
    hNHitperFla3->Fill(Hmatchcounter3);
    
    hNHitperFla2D->Fill(0.,Hmatchcounter0);
    hNHitperFla2D->Fill(1.,Hmatchcounter1);
    hNHitperFla2D->Fill(2.,Hmatchcounter2);
    hNHitperFla2D->Fill(3.,Hmatchcounter3);
    
    
  }//A
  
  
  
}

void crt::MergeTest::beginJob()
{
  // Implementation of optional member function here.

  
  hFlashTimeDis = tfs->make<TH1F>("hFlashTimDis","hFlashTimDis",2000,-5,25);
  hFlashTimeDis->GetXaxis()->SetTitle("Flash Time w.r.t. trigger (us)");
  hFlashTimeDis->GetYaxis()->SetTitle("Entries/bin");  

  hFlashTimeDis_b0 = tfs->make<TH1F>("hFlashTimDis_b0","hFlashTimDis_b0",2000,-5,25);
  hFlashTimeDis_b0->GetXaxis()->SetTitle("Flash_bo Time w.r.t. trigger (us)");
  hFlashTimeDis_b0->GetYaxis()->SetTitle("Entries/bin");

  hTFvsTH_t1 = tfs->make<TH1F>("hBeamMatching","hBeamMatching",500,-1000,1000);
  hTFvsTH_t1->GetXaxis()->SetTitle("Flash Time w.r.t. trigger - CRTHit Time_t1 (ns)");
  hTFvsTH_t1->GetYaxis()->SetTitle("Entries/bin");

  hTFvsTH_t1_2d = tfs->make<TH2F>("hBeamMatching2","hBeamMatching2",6,-3,3,500,0,1000);
  hTFvsTH_t1_2d->GetXaxis()->SetTitle("Flash Time - CRTHit Time (s)");
  hTFvsTH_t1_2d->GetYaxis()->SetTitle("Flash Time w.r.t Trigger - CRTHit_Time_t1 (ns)");
  hTFvsTH_t1_2d->SetOption("COLZ"); 

  hTFvsTH_t0 = tfs->make<TH1F>("hGPSMatching","GPSMatching",2000,0,2000000);
  hTFvsTH_t0->GetXaxis()->SetTitle("Flash_Time_GPS - CRTHit_Time_T0 (ns)");
  hTFvsTH_t0->GetYaxis()->SetTitle("Entries/bin");

  hTFvsTH_t0_2d = tfs->make<TH2F>("hGPSMatching2","hGPSMatching2",6,-3,3,2000,0,2000000);
  hTFvsTH_t0_2d->GetXaxis()->SetTitle("Flash Time - CRTHit Time (s)");
  hTFvsTH_t0_2d->GetYaxis()->SetTitle("Flasf_Time_GPS - CRTHit_Time_T0 (ns)");
  hTFvsTH_t0_2d->SetOption("COLZ"); 

  hTFvsTH_t0_t1 = tfs->make<TH2F>("hGPSBeamMatching","hGPSBeamMatching",2000,0,2000000,500,0,1000);
  hTFvsTH_t0_t1->GetXaxis()->SetTitle("Flash_Time_GPS - CRTHit_Time_T0 (ns)");
  hTFvsTH_t0_t1->GetYaxis()->SetTitle("Flash Time w.r.t Trigger - CRTHit_Time_t1 (ns)");
  hTFvsTH_t0_t1->SetOption("COLZ"); 

  hTFvsTH_plane_t0 = tfs->make<TH2F>("hGPSBeamMatchingPlane","hGPSBeamMatchingPlane",4,0,4, 4000,0,2000000);
  hTFvsTH_plane_t0->GetXaxis()->SetTitle("CRT plane (0=bottom, 1=FT, 2=Pipe, 3=Top))");
  hTFvsTH_plane_t0->GetYaxis()->SetTitle("Flash Time_GPS - CRTHit Time_t0 (ns)");
  hTFvsTH_plane_t0->SetOption("COLZ"); 


  hNFlavsNHit = tfs->make<TH2F>("hNFlavsNHit","hNFlavsNHit",30,0,30,100,0,300);
  hNFlavsNHit->GetXaxis()->SetTitle("Number of Flashes per event");
  hNFlavsNHit->GetYaxis()->SetTitle("Number of CRT Hits per event");
  hNFlavsNHit->GetZaxis()->SetTitle("Entries/bin");
  hNFlavsNHit->SetOption("COLZ");

  hNHitperFla = tfs->make<TH1F>("hNHitperFla","hNHitperFla",205,-5,200);
  hNHitperFla->GetXaxis()->SetTitle("N^{o} of CRTHits per Flash");
  hNHitperFla->GetYaxis()->SetTitle("Entries/bin");

  hNHitperFla0 = tfs->make<TH1F>("hNHitperFlaBot","hNHitperFlaBot",205,-5,200);
  hNHitperFla0->GetXaxis()->SetTitle("N^{o} of CRTHits per Flash in Bottom");
  hNHitperFla0->GetYaxis()->SetTitle("Entries/bin");

  hNHitperFla1 = tfs->make<TH1F>("hNHitperFlaFT","hNHitperFlaFT",205,-5,200);
  hNHitperFla1->GetXaxis()->SetTitle("N^{o} of CRTHits per Flash in FT");
  hNHitperFla1->GetYaxis()->SetTitle("Entries/bin");

  hNHitperFla2 = tfs->make<TH1F>("hNHitperFlaPipe","hNHitperFlaPipe",205,-5,200);
  hNHitperFla2->GetXaxis()->SetTitle("N^{o} of CRTHits per Flash in Pipe");
  hNHitperFla2->GetYaxis()->SetTitle("Entries/bin");

  hNHitperFla3 = tfs->make<TH1F>("hNHitperFlaTop","hNHitperFlaTop",205,-5,200);
  hNHitperFla3->GetXaxis()->SetTitle("N^{o} of CRTHits per Flash in Top");
  hNHitperFla3->GetYaxis()->SetTitle("Entries/bin");
	
  hNHitperFla2D = tfs->make<TH2F>("hNHitperEvtPlane","hNHitperEvtPlane",4,0,4,205,-5,200);
  hNHitperFla2D->GetXaxis()->SetTitle("CRT plane (0=bottom, 1=FT, 2=Pipe, 3=Top))");
  hNHitperFla2D->GetYaxis()->SetTitle("N^{o} of CRTHits in event");
  hNHitperFla2D->SetOption("COLZ"); 


  hNTraperFla = tfs->make<TH1F>("hNTrackperFla","hNTrackperFla",30,-5,25);
  hNTraperFla->GetXaxis()->SetTitle("N^{o} of CRTTrack per Flash");
  hNTraperFla->GetYaxis()->SetTitle("Entries/bin");

  hTra_tl_len = tfs->make<TH2F>("hTra_tl_len","hTra_tl_len",120, 0, 1200, 120, 0, 120);
  hTra_tl_len->GetXaxis()->SetTitle("Track lenght (cm)");
  hTra_tl_len->GetYaxis()->SetTitle("Track time (ns)");
  hTra_tl_len->SetOption("COLZ"); 

  
  hZdiff = tfs->make<TH1F>("hZdiff","hZdiff",100,-500,500);
  hZdiff->GetXaxis()->SetTitle("ZTrack - ZFlash (cm)");
  hZdiff->GetYaxis()->SetTitle("Entries/bin");

  hYdiff = tfs->make<TH1F>("hYdiff","hYdiff",100,-500,500);
  hYdiff->GetXaxis()->SetTitle("YTrack - YFlash (cm)");
  hYdiff->GetYaxis()->SetTitle("Entries/bin");
 
  double inch =2.54; //inch in cm
  hBot = tfs->make<TH2F>("hBottom","Bottom",125,-700+205*inch,-700+205*inch+125*10.89,60,-300+50.4*inch,-300+50.4*inch+60*10.89);
  hBot->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  hBot->GetYaxis()->SetTitle("Lenght along the drift (cm)");
  hBot->GetZaxis()->SetTitle("Entries/bin");
  hBot->SetOption("COLZ");

  hFT = tfs->make<TH2F>("hFeedthroughSide","Feedthrough Side",125,-704+205*inch,-704+205*inch+125*10.89,60,-308-19.1*inch,-308-19.1*inch+60*10.89);
  hFT->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  hFT->GetYaxis()->SetTitle("Height (cm)");
  hFT->GetZaxis()->SetTitle("Entries/bin");
  hFT->SetOption("COLZ");

  hPipe = tfs->make<TH2F>("hPipeSide","Pipe Side",125,-704+205*inch,-704+205*inch+125*10.89,60,-294-19.1*inch,-294-19.1*inch+60*10.89);
  hPipe->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  hPipe->GetYaxis()->SetTitle("Height (cm)");
  hPipe->GetZaxis()->SetTitle("Entries/bin");
  hPipe->SetOption("COLZ");

  hTop = tfs->make<TH2F>("hTop","Top",125,-701+205*inch,-701+205*inch+125*11.38,80,2-170-300+50.4*inch,2-170-300+50.4*inch+80*11.38);
  hTop->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  hTop->GetYaxis()->SetTitle("Lenght along the drift (cm)"); 
  hTop->GetZaxis()->SetTitle("Entries/bin"); 
  hTop->SetOption("COLZ");


  /*
  hTFvsTT = tfs->make<TH1F>("hTFvsTT","hTFvsTT",1000000,0,10000000);//1ms max
  hTFvsTT->GetXaxis()->SetTitle("Track time - Flash time (ns)");
  hTFvsTT->GetYaxis()->SetTitle("Entries/bin");

  hMulFT = tfs->make<TH1F>("hMulFT","hMulFT",50,0,50);//
  hMulFT->GetXaxis()->SetTitle("Multiplicity (Tracks per Flash)");
  hMulFT->GetYaxis()->SetTitle("Entries/bin");


  hMulFTvsTdis = tfs->make<TH2F>("hMulFTvsTdis","hMulFTvsTdis",50,0,50,1000000,0,10000000);
  hMulFTvsTdis->GetXaxis()->SetTitle("Multiplicity (Tracks per Flash)");
  hMulFTvsTdis->GetYaxis()->SetTitle("Track time - Flash time (ns)");
  hMulFTvsTdis->GetZaxis()->SetTitle("Entries/bin");
  hMulFTvsTdis->SetOption("COLZ");
  */
}

void crt::MergeTest::endJob()
{
  // Implementation of optional member function here.
  
  
  /*	  
  //OLD
  //uint32_t Hit_sec = my_CRTHit.ts0_s;
  //uint32_t Flash_sec = evt_timeGPS_sec;
  
  uint32_t Hit_nsec = my_CRTHit.ts1_ns + fHardDelay_;
  //uint32_t Flash_nsec = Timeflash * 1000;
  
  int dif_sec = Flash_sec - Hit_sec;
  int dif_nsec = Flash_nsec - Hit_nsec;
  int dif_secABS = std::abs(dif_sec);
  int dif_nsecABS = std::abs(dif_nsec);
  //OLD
  
  if( (dif_secABS<3)  &&  (dif_nsecABS<1000 )  ){//E
  
  hTFvsTH->Fill(dif_nsec);
  hTFvsTH_2d->Fill(dif_sec , dif_nsecABS);
  
  hTFvsTH_t0->Fill(Timeflash_ns_GPS - my_CRTHit.ts0_ns);
  hTFvsTH_t0_2d->Fill(Timeflash_ns_GPS - my_CRTHit.ts0_ns, dif_nsec);
  hTFvsTH_t0_plane->Fill(my_CRTHit.plane, Timeflash_ns_GPS - my_CRTHit.ts0_ns);
  
  if(verbose_==1){
  std::cout<<"Flash_sec - Hit_sec: "<<Flash_sec - Hit_sec<<std::endl;
  std::cout<<"Flash_nsec - Hit_nsec: "<<Flash_nsec - Hit_nsec<<std::endl;
  std::cout<<"Flash_nsec - Hit_nsec: "<<dif_secABS<<std::endl;
  getchar();
  }
  
  }//E
  //OLD
  
  */
  
  
}

DEFINE_ART_MODULE(crt::MergeTest)


