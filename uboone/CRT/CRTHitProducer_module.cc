////////////////////////////////////////////////////////////////////////
// Class:       CRTHitProducer
// Module Type: producer
// File:        CRTHitProducer_module.cc
// Description: Module to obtain XY (ZY, XZ) coincidences within a CRT plane.
// Generated at Thu Apr 27 08:11:58 2017 by David Lorca Galindo using artmod
// from cetpkgsupport v1_11_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "CRTBernFEBDAQCore/Overlays/BernZMQFragment.hh"
#include <artdaq-core/Data/Fragment.hh>

#include "art/Framework/Services/Optional/TFileService.h"

#include "ubooneobj/CRT/CRTHit.hh"
#include "ubooneobj/CRT/CRTTrack.hh"
#include "uboone/CRT/CRTAuxFunctions.hh"

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

#include <memory>

namespace bernfebdaq {
  class CRTHitProducer;
}

class bernfebdaq::CRTHitProducer : public art::EDProducer {
public:
  explicit CRTHitProducer(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  CRTHitProducer(CRTHitProducer const &) = delete;
  CRTHitProducer(CRTHitProducer &&) = delete;
  CRTHitProducer & operator = (CRTHitProducer const &) = delete;
  CRTHitProducer & operator = (CRTHitProducer &&) = delete;

  // Required functions.
  void produce(art::Event & evt) override;

  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:

  // Declare member data here.

  art::ServiceHandle<art::TFileService> tfs;

  std::string  raw_data_label_;
  std::string  SiPMpositions_;
  std::string  FEBDelays_;
  std::string  CRTGains_;
  std::string  CRTPedestals_;
 
  double max_time_difference_ ;//max time for coincidence                                                                                                
  double Lbar_; //Width for scintillator strip
  double Lbartop_; //Width for scintillator strip_top
  int store_hit_;
  int verbose_;
  double Emin_;//Energy threshold for hit     


  //std::map<std::pair<int,std::pair<double,double> >, BernZMQEvent const*> HitCollection;
  
  std::map <int, std::vector<double> > sensor_pos; //key = FEB*100+ch
  std::map <int, double > FEBDel; //key = FEB;
  std::map<int, std::pair<double,double> > SiPMgain; //key = FEB*100+ch
  std::map<int, std::pair<double,double> > SiPMpedestal; //key = FEB*100+ch
  
  //quallity plots
  TH2F* HitDistBot;
  TH2F* HitDistFT;
  TH2F* HitDistPipe;
  TH2F* HitDistTop;

  TH1F* hxtot;
  TH1F* hytot;
  TH1F* hztot;

  TH2F* FEBvsFEB;
  TH1F* TimeDiff;
  //quallity plots

  TTree*       my_tree_;
  double xtot=-10000., ytot=-10000., ztot=-10000.;
  double hit_time_ns = -1e18;
  double hit_time_s = -1e18;
  int plane = -1;
  double td = -1e19;
  std::map< uint8_t, std::vector<std::pair<int,float> > > pesmap;

  //test
  TH1F* hPES;
  TH1F* hPEStot;
  //test

};


bernfebdaq::CRTHitProducer::CRTHitProducer(fhicl::ParameterSet const & p)
  : 
  raw_data_label_(p.get<std::string>("raw_data_label")),
  SiPMpositions_(p.get<std::string>("CRTpositions_file")),
  FEBDelays_(p.get<std::string>("FEBDelays_file")),
  CRTGains_(p.get<std::string>("CRTgains_file")),
  CRTPedestals_(p.get<std::string>("CRTpedestals_file")),
  max_time_difference_(p.get<double>("max_time_difference")),
  Lbar_(p.get<double>("Lbar",10.8)),
  Lbartop_(p.get<double>("Lbartop",11.2)),
  store_hit_(p.get<int>("store_hit")),
  verbose_(p.get<int>("verbose")),
  Emin_(p.get<double>("Emin"))//,
{
  // Call appropriate produces<>() functions here.
  if(store_hit_ == 1)  
    produces< std::vector<crt::CRTHit>   >(); //substitute
}

void bernfebdaq::CRTHitProducer::produce(art::Event & evt)
{
  // Implementation of required member function here.

  //vaciar HitCollection
  std::map<std::pair<int,std::pair<double,double> >, BernZMQEvent const*> HitCollection;

  //hit collection on this event	    
  std::unique_ptr<std::vector<crt::CRTHit> > CRTHiteventCol(new std::vector<crt::CRTHit>);
  
  art::Handle< std::vector<artdaq::Fragment> > rawHandle;
  // evt.getByLabel(raw_data_label_, "BernZMQ", rawHandle); //artdaq //daq
  evt.getByLabel(raw_data_label_, rawHandle); //Converted files //crtdaq
  
  //check to make sure the data we asked for is valid                                                                                                  
  if(!rawHandle.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << evt.event() << " has zero"
              << " BernFEB fragments " << " in module " << raw_data_label_ << std::endl;
    std::cout << std::endl;
    return;
  }
  
  //get better access to the data                                                                                                        
  std::vector<artdaq::Fragment> const& rawFragments(*rawHandle);
  
  for(auto const& frag : rawFragments){//A
    //overlay this so it's in the "BernFragment" format. Same data!                                                     
    BernZMQFragment bfrag(frag);
    
    //Grab the metadata.                                                                                                        
    //See bernfebdaq-core/bernfebdaq-core/Overlays/BernFEBFragment.hh                                                                                  
    auto bfrag_metadata = bfrag.metadata();
    size_t   nevents    = bfrag_metadata->n_events();   //number of BernFEBEvents in this packet                                                       
    auto time_start_seconds = bfrag_metadata->time_start_seconds();     //last second.                                          
    //auto time_start_ns = bfrag_metadata->time_start_nanosec();     //last second.                                                          
    //auto time_end_ns = bfrag_metadata->time_end_nanosec();     //last second + 1s.                                                                  
    auto FEB_MAC =  bfrag_metadata->feb_id();     //mac addresss of this packet                                             
    auto FEB_ID = crt::auxfunctions::getFEBN(FEB_MAC); //FEB ID  
    FEB_ID = FEB_MAC; //only for converted files
    auto feb_tevt = FEB_ID;
    double FEB_del = crt::auxfunctions::getFEBDel(FEB_ID,FEBDel); //cable_length FEB delay in ns.         

    for(size_t i_e=0; i_e<nevents; ++i_e){//B_1
      BernZMQEvent const* this_event = bfrag.eventdata(i_e); //get the single hit/event                                                                    
      
      bool sptevent0 = this_event->IsReference_TS0();
      bool sptevent1 = this_event->IsReference_TS1();
      
      if( (!sptevent0) && (!sptevent1)  ){//B_2
	
	auto time_ts0 = this_event->Time_TS0();         //grab the event time from last second  //in ns
	//auto time_ts0 = this_event->Time_TS1();         //grab the event time from last second                                                              
	
	double corrected_time = GetCorrectedTime(time_ts0,*bfrag_metadata);      //correct this time                                                         
	corrected_time = corrected_time + FEB_del;// in ns

	double time_tevt_s = time_start_seconds;
	double time_tevt_ns = corrected_time;
	std::pair<double,double> timepair_tevt;
	timepair_tevt = std::make_pair(time_tevt_s,time_tevt_ns);
	
	std::pair<int,std::pair<double,double> > keypair_tevt;
	keypair_tevt = std::make_pair(feb_tevt,timepair_tevt);
	HitCollection[keypair_tevt]=this_event;
	
	for(auto itA = begin(HitCollection); itA != end(HitCollection); ++itA){//C
	  
	  std::pair<int,std::pair<double,double> > keypair_st = (*itA).first;
	  BernZMQEvent const* event_st = (*itA).second;
	  
	  auto feb_st = keypair_st.first;	
	  std::pair<double,double> timepair_st;
	  timepair_st = keypair_st.second;
	  double time_st_s = timepair_st.first;
	  double time_st_ns = timepair_st.second;
	  
	  auto time_diff = abs( (time_tevt_ns) - (time_st_ns) );//in ns	  
	  
	  if( (time_tevt_s == time_st_s) && (time_diff<max_time_difference_) && (feb_st !=  feb_tevt) ){//D //COINCIDENCE without Tcorrection.
	    
	    double max_temp1_tevt = -1;
	    unsigned int maxSipm1_tevt = -1;
	    double max_temp2_tevt = -1;
	    unsigned int maxSipm2_tevt = -1;
	    
	    for(size_t i_chan=0; i_chan<32; ++i_chan){ //1st max                                                                                             
	      if(this_event->adc[i_chan]>max_temp1_tevt){
		max_temp1_tevt=this_event->adc[i_chan];
		maxSipm1_tevt=i_chan;
	      }
	    }//1st max                                                                                                                                       
	    
	    if (maxSipm1_tevt % 2 == 0){//2nd max
	      maxSipm2_tevt = maxSipm1_tevt+1;
	    } else if(maxSipm1_tevt % 2 == 1) {
	      maxSipm2_tevt = maxSipm1_tevt-1;
	    }
	    max_temp1_tevt=this_event->adc[maxSipm1_tevt];
	    max_temp2_tevt=this_event->adc[maxSipm2_tevt];
	    //2ndmax 
	    
	    if(maxSipm2_tevt < maxSipm1_tevt){//swap                                                                                                         
	      unsigned int temp = maxSipm1_tevt;
	      maxSipm1_tevt=maxSipm2_tevt;
	      maxSipm2_tevt=temp;
	      double maxtemp = max_temp1_tevt;
	      max_temp1_tevt = max_temp2_tevt;
	      max_temp2_tevt =maxtemp;
	    }//swap                                                                                                                                          
	    
	    int key_tevt1 = feb_tevt*100+maxSipm1_tevt;
	    int key_tevt2 = feb_tevt*100+maxSipm2_tevt;
	    
	    std::pair<double,double> gain_tevt1 = crt::auxfunctions::getGain(key_tevt1, SiPMgain); 
	    std::pair<double,double> pedestal_tevt1 = crt::auxfunctions::getGain(key_tevt1, SiPMpedestal);
	    double pesmax_tevt1 = (max_temp1_tevt - pedestal_tevt1.first) / gain_tevt1.first;
	    std::vector<double> pos_tevt1 = crt::auxfunctions::getPos(key_tevt1, sensor_pos);
	    
	    std::pair<double,double> gain_tevt2 = crt::auxfunctions::getGain(key_tevt2, SiPMgain);
	    std::pair<double,double> pedestal_tevt2 = crt::auxfunctions::getGain(key_tevt2, SiPMpedestal);
	    double pesmax_tevt2 = (max_temp2_tevt - pedestal_tevt2.first) / gain_tevt2.first;
	    std::vector<double> pos_tevt2 = crt::auxfunctions::getPos(key_tevt2, sensor_pos);

	    double LBar = Lbar_;
	    if(pos_tevt1[3]==3) LBar = Lbartop_;//for top	    
	    std::vector<double> interpos_tevt = crt::auxfunctions::inter_X(pesmax_tevt1, pos_tevt1, pesmax_tevt2, pos_tevt2, LBar);
	    double interpos_tevt_err = crt::auxfunctions::inter_X_error(pesmax_tevt1, pesmax_tevt2, LBar);
	    
	    double max_temp1_st = -1;
            unsigned int maxSipm1_st = -1;
            double max_temp2_st = -1;
            unsigned int maxSipm2_st = -1;

            for(size_t i_chan=0; i_chan<32; ++i_chan){ //1st max                                                                                                              
              if(event_st->adc[i_chan]>max_temp1_st){
                max_temp1_st=event_st->adc[i_chan];
                maxSipm1_st=i_chan;
              }
            }//1st max                                                                                                                                                        

            if (maxSipm1_st % 2 == 0){//2nd max                                                                                                              
              maxSipm2_st = maxSipm1_st+1;
            } else if(maxSipm1_st % 2 == 1) {
              maxSipm2_st = maxSipm1_st-1;
            }
	    max_temp1_st=event_st->adc[maxSipm1_st];
            max_temp2_st=event_st->adc[maxSipm2_st];
            //2ndmax                                                                                                                                                          

            if(maxSipm2_st < maxSipm1_st){//swap                                                                                                                          
              unsigned int temp = maxSipm1_st;
              maxSipm1_st=maxSipm2_st;
              maxSipm2_st=temp;
              double maxtemp = max_temp1_st;
              max_temp1_st = max_temp2_st;
              max_temp2_st =maxtemp;
            }//swap  

	    int key_st1 = feb_st*100+maxSipm1_st;
	    int key_st2 = feb_st*100+maxSipm2_st;
	    
	    std::pair<double,double> gain_st1 = crt::auxfunctions::getGain(key_st1, SiPMgain);
	    std::pair<double,double> pedestal_st1 = crt::auxfunctions::getGain(key_st1, SiPMpedestal);
	    double pesmax_st1 = (max_temp1_st - pedestal_st1.first) / gain_st1.first;                                                                 
	    std::vector<double> pos_st1 = crt::auxfunctions::getPos(key_st1, sensor_pos);
	    
	    std::pair<double,double> gain_st2 = crt::auxfunctions::getGain(key_st2, SiPMgain);
	    std::pair<double,double> pedestal_st2 = crt::auxfunctions::getGain(key_st2, SiPMpedestal);
	    double pesmax_st2 = (max_temp2_st - pedestal_st2.first) / gain_st2.first;
	    std::vector<double> pos_st2 = crt::auxfunctions::getPos(key_st2, sensor_pos);
	    
	    std::vector<double> interpos_st = crt::auxfunctions::inter_X(pesmax_st1, pos_st1, pesmax_st2, pos_st2, LBar);
	    double interpos_st_err = crt::auxfunctions::inter_X_error(pesmax_st1, pesmax_st2, LBar);
	    
	    double PEStot = pesmax_tevt1 + pesmax_tevt2 + pesmax_st1 + pesmax_st2;    
	    hPEStot->Fill(PEStot);
	    hPES->Fill(pesmax_tevt1);
	    hPES->Fill(pesmax_tevt2);
	    hPES->Fill(pesmax_st1);
	    hPES->Fill(pesmax_st2);
	    
	    //pos vector/ 0=x 1=y 2=z 3=plane 4=layer 5=orientation;   //clean up!!! after MC
	    if( (pos_tevt1[3]==pos_st1[3] ) 
		&& (pos_tevt1[4]!=pos_st1[4]) 
		&& ( (std::abs(pos_tevt1[4]-pos_st1[4])<2) || ((pos_tevt1[3]==2) && (pos_tevt1[4]==0 && pos_st1[4]==2)) || ((pos_tevt1[3]==2) && (pos_tevt1[4]==2 && pos_st1[4]==0)) )
		&& (pos_tevt1[5]!=pos_st1[5])
		&& (max_temp1_tevt>500)  
		&& (max_temp2_tevt>500) 
		&& (max_temp1_st>500) 
		&& (max_temp1_st>500) 
		&& (max_temp1_tevt<4000) 
		&& (max_temp2_tevt<4000) 
		&& (max_temp1_st<4000) 
		&& (max_temp1_st<4000)   ){//E
	      // && (PEStot>Emin_)
	      	      
	      
	      //correct time propagation along the fiber, 6.2 ns/m.  Ttrue = Treg - correction.
	      time_tevt_ns = crt::auxfunctions::getTcorr(interpos_tevt, interpos_st, time_tevt_ns);
	      time_st_ns = crt::auxfunctions::getTcorr(interpos_st, interpos_tevt, time_st_ns);
	      
	      time_diff = abs( (time_tevt_ns) - (time_st_ns) );//in ns, corrected
	      //correct time propagation along the fiber, 6.2 ns/m.  Ttrue = Treg - correction.
	      
	      //quality plots
	      FEBvsFEB->Fill(feb_tevt,feb_st);
	      TimeDiff->Fill(time_diff);
	      //quality plots
	      
	      
	      if( (interpos_tevt[6]==1) && (interpos_st[6] != 1) ){xtot=interpos_tevt[0];} 
	      if( (interpos_tevt[6]==2) && (interpos_st[6] != 2) ){ytot=interpos_tevt[1];}	
	      if( (interpos_tevt[6]==3) && (interpos_st[6] != 3) ){ztot=interpos_tevt[2];}

	              
	      if( (interpos_st[6]==1) && (interpos_tevt[6] != 1) ){xtot=interpos_st[0];}
	      if( (interpos_st[6]==2) && (interpos_tevt[6] != 2) ){ytot=interpos_st[1];}
	      if( (interpos_st[6]==3) && (interpos_tevt[6] != 3) ){ztot=interpos_st[2];}
	      
	      
	      if( (interpos_st[6] !=1) && (interpos_tevt[6] != 1) ){xtot=(interpos_tevt[0] + interpos_st[0])/2;}     
	      if( (interpos_st[6] !=2) && (interpos_tevt[6] != 2) ){ytot=(interpos_tevt[1] + interpos_st[1])/2;}	
	      if( (interpos_st[6] !=3) && (interpos_tevt[6] != 3) ){ztot=(interpos_tevt[2] + interpos_st[2])/2;}
	      
	      hxtot->Fill(xtot);
	      hytot->Fill(ytot);
	      hztot->Fill(ztot);


	      hit_time_ns = (time_tevt_ns + time_st_ns)/2;
	      hit_time_s = time_tevt_s;
	      plane = pos_tevt1[3]; 
	      
	      crt::CRTHit CRTHitevent;
	      
	      CRTHitevent.x_pos= xtot;
	      CRTHitevent.x_err= sqrt( pow(interpos_tevt_err,2) + pow(interpos_st_err,2) );
	      CRTHitevent.y_pos= ytot;
	      CRTHitevent.y_err= sqrt( pow(interpos_tevt_err,2) + pow(interpos_st_err,2) );
	      CRTHitevent.z_pos= ztot;
	      CRTHitevent.z_err= sqrt( pow(interpos_tevt_err,2) + pow(interpos_st_err,2) );
	      
	      CRTHitevent.ts0_s = hit_time_s; //errors!!  missing
	      CRTHitevent.ts0_ns = hit_time_ns; //errors!!  missing
	      //  CRTHitevent.ts0_s_err = sqrt(hit_time_s); //errors!!  missing //just filling
	      //CRTHitevent.ts0_ns_err = sqrt(hit_time_ns); //errors!!  missing
	      
	      CRTHitevent.ts1_ns = (this_event->Time_TS1() -  event_st->Time_TS1() ) /2    ; //errors!!  missing
	      //CRTHitevent.ts1_ns_err = sqrt ( (this_event->Time_TS1() -  event_st->Time_TS1() ) /2  )  ; //errors!!  missing
	      	      
	      CRTHitevent.plane = plane;
	      
	      std::vector<uint8_t> ids;
	      ids.push_back(feb_tevt);
	      ids.push_back(feb_st);
	      CRTHitevent.feb_id = ids;
	      
	      std::vector<std::pair<int,float> > vec_pes_tevt;
	      std::vector<std::pair<int,float> > vec_pes_st;
	      
	      for(size_t i_chan=0; i_chan<32; ++i_chan){ //1st max                                                                                         
		int key_tevt = feb_tevt*100+i_chan;
		std::pair<double,double> gain_tevt = crt::auxfunctions::getGain(key_tevt, SiPMgain);
		std::pair<double,double> pedestal_tevt = crt::auxfunctions::getGain(key_tevt, SiPMpedestal);
		double pes_tevt = ( (this_event->adc[i_chan]) - pedestal_tevt.first) / gain_tevt.first;
		std::pair<int,float> pair_tevt = std::make_pair(i_chan,pes_tevt);
		vec_pes_tevt.push_back(pair_tevt);
		
		int key_st = feb_st*100+i_chan;
		std::pair<double,double> gain_st = crt::auxfunctions::getGain(key_st, SiPMgain);
		std::pair<double,double> pedestal_st = crt::auxfunctions::getGain(key_st, SiPMpedestal);
		double pes_st = ( (event_st->adc[i_chan]) - pedestal_st.first) / gain_st.first;
		std::pair<int,float> pair_st = std::make_pair(i_chan,pes_st);
		vec_pes_st.push_back(pair_st);
	      }
	      
	      std::map< uint8_t, std::vector<std::pair<int,float> > > pesmap_hit;
	      pesmap_hit[feb_tevt] = vec_pes_tevt;
	      pesmap_hit[feb_st] = vec_pes_st;
	      CRTHitevent.pesmap = pesmap_hit;
	      
	      CRTHitevent.peshit = pesmax_tevt1 + pesmax_tevt2 + pesmax_st1 + pesmax_st2;    
	      
	      
	      //Store CRTHit in Collection
	      CRTHiteventCol->emplace_back(CRTHitevent);
	      my_tree_->Fill();
	      //Store CRTHit in Collection
	      
	      //quality plot
	      if(plane==0){
		HitDistBot->Fill(ztot,xtot);
	      }                                               
	      if(plane==1){                                                                                                            
		HitDistFT->Fill(ztot, ytot);
	      }
	      if(plane==2){              
		HitDistPipe->Fill(ztot, ytot);
	      }
	      if(plane==3){
		HitDistTop->Fill(ztot,xtot);
	      }                                                                              
	      //quality plot
	      

	      //verbose 
	      if(verbose_ == 1){
		std::cout.precision(19);
		std::cout<<"CRTHit in plane: "<<interpos_tevt[3]<<"  Interaction point--> x: "<<xtot<<"  y: "<<ytot<<"  z: "<<ztot<<std::endl;  
		std::cout<<"in time (ns): "<<hit_time_ns<<std::endl;
		std::cout<<"Time tevt_s: "<<time_tevt_s<< "   time st_s: " << time_st_s<<std::endl;
		std::cout<<"Time tevt_ns: "<<time_tevt_ns<< "   time st_ns: " << time_st_ns<<std::endl;
		std::cout<<"Time difference (ns): "<<time_diff<<std::endl;		
		std::cout<<"Coincidence between feb_tevt "<<feb_tevt<<" and feb_st: " <<feb_st <<std::endl;
		std::cout<<"max_tevt1: "<<max_temp1_tevt<<" max_tevt2: "<<max_temp2_tevt<<" max_st1: "<<max_temp1_st<<" max_st2: "<<max_temp2_st<<std::endl;
		std::cout<<"pedestal1_tevt: "<<pedestal_tevt1.first<<" pedestal2_tevt: "<<pedestal_tevt2.first<<" pedestal1_st: "<<pedestal_st1.first<<" pedestal2_st: "<<pedestal_st2.first<<std::endl;
		std::cout<<"gain1_tevt: "<<gain_tevt1.first<<" gain2_tevt: "<<gain_tevt2.first<<" gain1_st: "<<gain_st1.first<<" gain2_st: "<<gain_st2.first<<std::endl;
		std::cout<<"pesmax_tevt1: "<<pesmax_tevt1<<" pesmax_tevt2: "<<pesmax_tevt2<<" pesmax_st1: "<<pesmax_st1<<" pesmax_st2: "<<pesmax_st2<<std::endl;
		std::cout<<"pos1_tevt: x: "<<pos_tevt1[0]<<" y: "<<pos_tevt1[1] <<" z: " <<pos_tevt1[2]<<std::endl;
		std::cout<<"pos2_tevt: x: "<<pos_tevt2[0]<<" y: "<<pos_tevt2[1] <<" z: " <<pos_tevt2[2]<<std::endl;
		std::cout<<"pos1_st: x: "<<pos_st1[0]<<" y: "<<pos_st1[1] <<" z: " <<pos_st1[2]<<std::endl;
		std::cout<<"pos2_st: x: "<<pos_st2[0]<<" y: "<<pos_st2[1] <<" z: " <<pos_st2[2]<<std::endl;
		std::cout<<"Interpos_tevt x: "<<interpos_tevt[0]<<" y: "<<interpos_tevt[1]<<" z: "<<interpos_tevt[2]<< " in plane "<<interpos_tevt[3]<<" and layer "<<interpos_tevt[4]<<" with orientation "<<interpos_tevt[5]<<" interpolada en eje: "<<interpos_tevt[6]<<std::endl;
		std::cout<<"Interpos_st   x: "<<interpos_st[0]<<" y: "<<interpos_st[1]<<" z: "<<interpos_st[2]<< " in plane "<<interpos_st[3]<<" and layer "<<interpos_st[4]<<" with orientation "<<interpos_st[5]<<" interpolada en eje: "<<interpos_st[6]<<std::endl;
		
		getchar();
	      }
	      //verbose
	      
	      
	    }//E
	  }//D
	}//C
      }//B_2
    }//B_1
  }//A
  
  //store collection into event
  if(store_hit_ == 1)  
    evt.put(std::move(CRTHiteventCol));
  
}

void bernfebdaq::CRTHitProducer::beginJob()
{
  my_tree_ = tfs->make<TTree>("my_tree","CRT Tree");
  my_tree_->Branch("hit_time_s", &hit_time_s, "time (s)");
  my_tree_->Branch("hit_time_ns", &hit_time_ns, "time (ns)");
  my_tree_->Branch("timediff", &td, "timediff (ns)");
  my_tree_->Branch("CRTplane", &plane, "(0=Bot, 1=FT, 2=PS, 3=Top)");
  my_tree_->Branch("pes_info", &pesmap, "...");
  my_tree_->Branch("Xreco", &xtot, "Xreco (cm)");
  my_tree_->Branch("Yreco", &ytot, "Yreco (cm)");
  my_tree_->Branch("Zreco", &ztot, "Zreco (cm)");


  crt::auxfunctions::FillPos(SiPMpositions_, sensor_pos); //key = FEB*100+ch  //fill sipm positions      
  crt::auxfunctions::FillFEBDel(FEBDelays_, FEBDel); //key = FEB  //fill FEB delays
  crt::auxfunctions::FillGain(CRTGains_, SiPMgain); //key = FEB*100+ch  //fill sipms gain
  crt::auxfunctions::FillGain(CRTPedestals_, SiPMpedestal); //key = FEB*100+ch  //same for pedestals 

  double inch =2.54; //inch in cm 
  HitDistBot = tfs->make<TH2F>("hBottom","Bottom",125,-700+205*inch,-700+205*inch+125*10.89,60,-300+50.4*inch,-300+50.4*inch+60*10.89);
  HitDistBot->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  HitDistBot->GetYaxis()->SetTitle("Lenght along the drift (cm)");
  HitDistBot->GetZaxis()->SetTitle("Entries/bin");
  HitDistBot->SetOption("COLZ");

  HitDistFT = tfs->make<TH2F>("hFeedthroughSide","Feedthrough Side",125,-704+205*inch,-704+205*inch+125*10.89,60,-308-19.1*inch,-308-19.1*inch+60*10.89);
  HitDistFT->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  HitDistFT->GetYaxis()->SetTitle("Height (cm)");
  HitDistFT->GetZaxis()->SetTitle("Entries/bin");
  HitDistFT->SetOption("COLZ");

  HitDistPipe = tfs->make<TH2F>("hPipeSide","Pipe Side",125,-704+205*inch,-704+205*inch+125*10.89,60,-294-19.1*inch,-294-19.1*inch+60*10.89);
  HitDistPipe->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  HitDistPipe->GetYaxis()->SetTitle("Height (cm)");
  HitDistPipe->GetZaxis()->SetTitle("Entries/bin");
  HitDistPipe->SetOption("COLZ");

  HitDistTop = tfs->make<TH2F>("hTop","Top",125,-701+205*inch,-701+205*inch+125*11.38,80,2-170-300+50.4*inch,2-170-300+50.4*inch+80*11.38);
  HitDistTop->GetXaxis()->SetTitle("Lenght along the beam (cm)");
  HitDistTop->GetYaxis()->SetTitle("Lenght along the drift (cm)"); 
  HitDistTop->GetZaxis()->SetTitle("Entries/bin"); 
  HitDistTop->SetOption("COLZ");

  FEBvsFEB = tfs->make<TH2F>("hFEBvsFEB","FEBvsFEB",130,0,130,130,0,130);
  FEBvsFEB->GetXaxis()->SetTitle("FEB ID");
  FEBvsFEB->GetYaxis()->SetTitle("FEB ID");
  FEBvsFEB->SetOption("COLZ");

  TimeDiff = tfs->make<TH1F>("hCoincidencetimedifference","Coincidence time difference",400,0,400);
  TimeDiff->GetXaxis()->SetTitle("Time Difference (ns)");
  TimeDiff->GetYaxis()->SetTitle("Entries/bin");

  hxtot = tfs->make<TH1F>("hxtot","xtot",70000,-200,500);
  hxtot->GetXaxis()->SetTitle("Reconstructed X (cm)");
  hxtot->GetYaxis()->SetTitle("Entries/bin");

  hytot = tfs->make<TH1F>("hytot","ytot",60000,-300,300);
  hytot->GetXaxis()->SetTitle("Reconstructed Y (cm)");
  hytot->GetYaxis()->SetTitle("Entries/bin");

  hztot = tfs->make<TH1F>("hztot","ztot",130000,-100,1200);
  hztot->GetXaxis()->SetTitle("Reconstructed Z (cm)");
  hztot->GetYaxis()->SetTitle("Entries/bin");

  hPES = tfs->make<TH1F>("hPES","PES",150,0,150);
  hPES->GetXaxis()->SetTitle(" pes");
  hPES->GetYaxis()->SetTitle("Entries/bin");

  hPEStot = tfs->make<TH1F>("hPEStot","PEStot",500,0,500);
  hPEStot->GetXaxis()->SetTitle(" pestot");
  hPEStot->GetYaxis()->SetTitle("Entries/bin");
}

void bernfebdaq::CRTHitProducer::endJob()
{
  // Implementation of optional member function here.
}

DEFINE_ART_MODULE(bernfebdaq::CRTHitProducer)
