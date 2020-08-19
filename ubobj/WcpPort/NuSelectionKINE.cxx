#include "NuSelectionKINE.h"
#include <vector>

using namespace nsm;

  NuSelectionKINE::NuSelectionKINE() {
	  NuSelectionKINE::KineInfo _KineInfo_init = {0};
	  _KineInfo_ = _KineInfo_init;
  }
  
  void NuSelectionKINE::reset(){
	  NuSelectionKINE::KineInfo _KineInfo_init = {0};
	  _KineInfo_ = _KineInfo_init;
  }

  void NuSelectionKINE::SetKineInfo(NuSelectionKINE::KineInfo KINE_input){ this->_KineInfo_=KINE_input; }

  const NuSelectionKINE::KineInfo & NuSelectionKINE::GetKineInfo() const { return this->_KineInfo_; }
