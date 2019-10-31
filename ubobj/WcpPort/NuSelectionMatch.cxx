#include "NuSelectionMatch.h"
#include <vector>

using namespace nsm;

  NuSelectionMatch::NuSelectionMatch() {
    _completeness=-1.;
    _completeness_energy=-1.;
    _purity=-1.;
    _purity_xy=-1.;
    _purity_xz=-1.;
  }

  NuSelectionMatch::NuSelectionMatch(float completeness,
				     float completeness_energy,
				     float purity,
				     float purity_xy,
				     float purity_xz){
    _completeness=completeness;
    _completeness_energy=completeness_energy;
    _purity=purity;
    _purity_xy=purity_xy;
    _purity_xz=purity_xz;
  }

  //NuSelectionMatch::~NuSelectionMatch(){
  //}

  void NuSelectionMatch::SetCompleteness(float completeness){ this->_completeness = completeness; }
  void NuSelectionMatch::SetCompletenessEnergy(float  completeness_energy){ this->_completeness_energy = completeness_energy; }
  void NuSelectionMatch::SetPurity(float purity){ this->_purity = purity; }
  void NuSelectionMatch::SetPurityXY(float purity_xy){ this->_purity_xy = purity_xy; }
  void NuSelectionMatch::SetPurityXZ(float purity_xz){ this->_purity_xz = purity_xz; }

  const float & NuSelectionMatch::GetCompleteness() const { return this->_completeness; }
  const float & NuSelectionMatch::GetCompletenessEnergy() const { return this->_completeness_energy; }
  const float & NuSelectionMatch::GetPurity() const { return this->_purity; }
  const float & NuSelectionMatch::GetPurityXY() const { return this->_purity_xy; }
  const float & NuSelectionMatch::GetPurityXZ() const { return this->_purity_xz; }

