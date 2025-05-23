//
// Build a dictionary.
//
// $Id: classes.h,v 1.8 2010/04/12 18:12:28  Exp $
// $Author:  $
// $Date: 2010/04/12 18:12:28 $
// 
// Original author Rob Kutschke, modified by wes
//

#include "canvas/Persistency/Common/Wrapper.h"
#include "ubobj/CRT/CRTSimData.hh"
#include "ubobj/CRT/CRTHit.hh"
#include "ubobj/CRT/CRTTrack.hh"
#include "ubobj/CRT/CRTTzero.hh"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "canvas/Persistency/Common/Assns.h"
////#include "MSetCRTFrag.hh"
#include <utility>
#include <vector>
//#include <artdaq-core/Data/Fragment.hh>
#include <map>

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//






//template class std::vector< anab::T0 >;
//template class art::Wrapper< std::vector< anab::T0 >  >;


//template class std::pair< art::Ptr<crt::CRTTzero>,        art::Ptr<crt::CRTHit>    >;
//template class std::pair< art::Ptr<crt::CRTHit>,        art::Ptr<crt::CRTTzero>    >;
template class art::Assns<crt::CRTTzero,crt::CRTHit, void>;
template class art::Wrapper<art::Assns<crt::CRTTzero,crt::CRTHit, void> >;

template class art::Assns<crt::CRTHit,crt::CRTTzero, void>;
template class art::Wrapper<art::Assns<crt::CRTHit,crt::CRTTzero, void> >;

template class art::Assns<recob::Track,crt::CRTTrack, void>;
template class art::Wrapper< art::Assns<recob::Track,crt::CRTTrack, void>  >;

template class art::Assns<crt::CRTTrack,recob::Track, void>;
template class art::Wrapper< art::Assns<crt::CRTTrack,recob::Track, void>  >;

template class art::Assns<recob::Track,crt::CRTHit, void>;
template class art::Wrapper< art::Assns<recob::Track,crt::CRTHit, void>  >;

template class art::Assns<crt::CRTHit,recob::Track, void>;
template class art::Wrapper< art::Assns<crt::CRTHit,recob::Track, void>  >;

template class art::Assns<recob::Track,crt::CRTTzero, void>;
template class art::Wrapper< art::Assns<recob::Track,crt::CRTTzero, void>  >;

template class art::Assns<crt::CRTTzero,recob::Track, void>;
template class art::Wrapper< art::Assns<crt::CRTTzero,recob::Track, void>  >;




////template class std::vector<crt::MSetCRTFrag>;
////template class art::Wrapper< std::vector<crt::MSetCRTFrag> >;

//template class std::vector< std::vector< artdaq::Fragment> >;
//template class art::Wrapper< std::vector< std::vector< artdaq::Fragment> > >;

