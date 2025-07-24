#include "NuSelectionBDT.h"
#include <vector>

using namespace nsm;

  NuSelectionBDT::NuSelectionBDT() {
    NuSelectionBDT::SPID _SPID_init = {0};

    NuSelectionBDT::stkdar _stkdar_init = {0};

    NuSelectionBDT::SPSHWID1 _SPSHWID1_init = {0};
	  NuSelectionBDT::SPSHWID2 _SPSHWID2_init = {0};
	  NuSelectionBDT::SPPi0Tagger1 _SPPi0Tagger1_init = {0};
	  NuSelectionBDT::SPLowEMichel _SPLowEMichel_init = {0};
	  NuSelectionBDT::SPBadReco1 _SPBadReco1_init = {0};
	  NuSelectionBDT::SPBadReco2 _SPBadReco2_init = {0};
	  NuSelectionBDT::SPBadReco3 _SPBadReco3_init = {0};
	  NuSelectionBDT::SPBadReco4 _SPBadReco4_init = {0};
	  NuSelectionBDT::SPHighEoverlap _SPHighEoverlap_init = {0};
	  NuSelectionBDT::SPLowEoverlap _SPLowEoverlap_init = {0};

	  NuSelectionBDT::CosmicTagger _CosmicTagger_init = {0};
	  NuSelectionBDT::GapID _GapID_init = {0};
	  NuSelectionBDT::MipCheck _MipCheck_init = {0};
	  NuSelectionBDT::MipID1 _MipID1_init = {0};
	  NuSelectionBDT::MipID2 _MipID2_init = {0};
	  NuSelectionBDT::Pi0Tagger1 _Pi0Tagger1_init = {0};
	  NuSelectionBDT::Pi0Tagger2 _Pi0Tagger2_init = {0};
	  NuSelectionBDT::MultiGamma1 _MultiGamma1_init = {0};
	  NuSelectionBDT::MultiGamma2 _MultiGamma2_init = {0};
	  NuSelectionBDT::SingleGamma1 _SingleGamma1_init = {0};
	  NuSelectionBDT::SingleGamma2 _SingleGamma2_init = {0};
	  NuSelectionBDT::StemLen _StemLen_init = {0};
	  NuSelectionBDT::LowEMichel _LowEMichel_init = {0};
	  NuSelectionBDT::BrokenMuon _BrokenMuon_init = {0};
	  NuSelectionBDT::MuEnergy _MuEnergy_init = {0};
	  NuSelectionBDT::ShowerAngle _ShowerAngle_init = {0};
	  NuSelectionBDT::BadStem _BadStem_init = {0};
	  NuSelectionBDT::VtxInShw _VtxInShw_init = {0};
	  NuSelectionBDT::BadReco1 _BadReco1_init = {0};
	  NuSelectionBDT::BadReco2 _BadReco2_init = {0};
	  NuSelectionBDT::BadReco3 _BadReco3_init = {0};
	  NuSelectionBDT::BadReco4 _BadReco4_init = {0};
	  NuSelectionBDT::TrackOverCluster _TrackOverCluster_init = {0};
	  NuSelectionBDT::HighEoverlap _HighEoverlap_init = {0};
	  NuSelectionBDT::LowEoverlap _LowEoverlap_init = {0};
	  NuSelectionBDT::MajorCosmicTagger _MajorCosmicTagger_init = {0};
	  NuSelectionBDT::NumuCCTagger _NumuCCTagger_init = {0};
	  NuSelectionBDT::BDTscores _BDTscores_init = {0};

	  NuSelectionBDT::WCPMTInfo _WCPMTInfo_init = {0};

    _stkdar_ = _stkdar_init;

    _SPID_ = _SPID_init;
    _SPSHWID1_ = _SPSHWID1_init;
	  _SPSHWID2_ = _SPSHWID2_init;
	  _SPPi0Tagger1_ = _SPPi0Tagger1_init;
	  _SPLowEMichel_ = _SPLowEMichel_init;
	  _SPBadReco1_ = _SPBadReco1_init;
	  _SPBadReco2_ = _SPBadReco2_init;
	  _SPBadReco3_ = _SPBadReco3_init;
	  _SPBadReco4_ = _SPBadReco4_init;
	  _SPHighEoverlap_ = _SPHighEoverlap_init;
	  _SPLowEoverlap_ = _SPLowEoverlap_init;

    _CosmicTagger_ = _CosmicTagger_init;
	  _GapID_ = _GapID_init;
	  _MipCheck_ = _MipCheck_init;
	  _MipID1_ = _MipID1_init;
	  _MipID2_ = _MipID2_init;
	  _Pi0Tagger1_ = _Pi0Tagger1_init;
	  _Pi0Tagger2_ = _Pi0Tagger2_init;
	  _MultiGamma1_ = _MultiGamma1_init;
	  _MultiGamma2_ = _MultiGamma2_init;
	  _SingleGamma1_ = _SingleGamma1_init;
	  _SingleGamma2_ = _SingleGamma2_init;
	  _StemLen_ = _StemLen_init;
	  _LowEMichel_ = _LowEMichel_init;
	  _BrokenMuon_ = _BrokenMuon_init;
	  _MuEnergy_ = _MuEnergy_init;
	  _ShowerAngle_ = _ShowerAngle_init;
	  _BadStem_ = _BadStem_init;
	  _VtxInShw_ = _VtxInShw_init;
	  _BadReco1_ = _BadReco1_init;
	  _BadReco2_ = _BadReco2_init;
	  _BadReco3_ = _BadReco3_init;
	  _BadReco4_ = _BadReco4_init;
	  _TrackOverCluster_ = _TrackOverCluster_init;
	  _HighEoverlap_ = _HighEoverlap_init;
	  _LowEoverlap_ = _LowEoverlap_init;
	  _MajorCosmicTagger_ = _MajorCosmicTagger_init;
	  _NumuCCTagger_ = _NumuCCTagger_init;
	  _BDTscores_ = _BDTscores_init;

	_WCPMTInfo_ = _WCPMTInfo_init;
  }

  void NuSelectionBDT::reset(){

    NuSelectionBDT::stkdar _stkdar_init = {0};

    NuSelectionBDT::SPID _SPID_init = {0};
    NuSelectionBDT::SPSHWID1 _SPSHWID1_init = {0};
	  NuSelectionBDT::SPSHWID2 _SPSHWID2_init = {0};
	  NuSelectionBDT::SPPi0Tagger1 _SPPi0Tagger1_init = {0};
	  NuSelectionBDT::SPLowEMichel _SPLowEMichel_init = {0};
	  NuSelectionBDT::SPBadReco1 _SPBadReco1_init = {0};
	  NuSelectionBDT::SPBadReco2 _SPBadReco2_init = {0};
	  NuSelectionBDT::SPBadReco3 _SPBadReco3_init = {0};
	  NuSelectionBDT::SPBadReco4 _SPBadReco4_init = {0};
	  NuSelectionBDT::SPHighEoverlap _SPHighEoverlap_init = {0};
	  NuSelectionBDT::SPLowEoverlap _SPLowEoverlap_init = {0};

	  NuSelectionBDT::CosmicTagger _CosmicTagger_init = {0};
	  NuSelectionBDT::GapID _GapID_init = {0};
	  NuSelectionBDT::MipCheck _MipCheck_init = {0};
	  NuSelectionBDT::MipID1 _MipID1_init = {0};
	  NuSelectionBDT::MipID2 _MipID2_init = {0};
	  NuSelectionBDT::Pi0Tagger1 _Pi0Tagger1_init = {0};
	  NuSelectionBDT::Pi0Tagger2 _Pi0Tagger2_init = {0};
	  NuSelectionBDT::MultiGamma1 _MultiGamma1_init = {0};
	  NuSelectionBDT::MultiGamma2 _MultiGamma2_init = {0};
	  NuSelectionBDT::SingleGamma1 _SingleGamma1_init = {0};
	  NuSelectionBDT::SingleGamma2 _SingleGamma2_init = {0};
	  NuSelectionBDT::StemLen _StemLen_init = {0};
	  NuSelectionBDT::LowEMichel _LowEMichel_init = {0};
	  NuSelectionBDT::BrokenMuon _BrokenMuon_init = {0};
	  NuSelectionBDT::MuEnergy _MuEnergy_init = {0};
	  NuSelectionBDT::ShowerAngle _ShowerAngle_init = {0};
	  NuSelectionBDT::BadStem _BadStem_init = {0};
	  NuSelectionBDT::VtxInShw _VtxInShw_init = {0};
	  NuSelectionBDT::BadReco1 _BadReco1_init = {0};
	  NuSelectionBDT::BadReco2 _BadReco2_init = {0};
	  NuSelectionBDT::BadReco3 _BadReco3_init = {0};
	  NuSelectionBDT::BadReco4 _BadReco4_init = {0};
	  NuSelectionBDT::TrackOverCluster _TrackOverCluster_init = {0};
	  NuSelectionBDT::HighEoverlap _HighEoverlap_init = {0};
	  NuSelectionBDT::LowEoverlap _LowEoverlap_init = {0};
	  NuSelectionBDT::MajorCosmicTagger _MajorCosmicTagger_init = {0};
	  NuSelectionBDT::NumuCCTagger _NumuCCTagger_init = {0};
	  NuSelectionBDT::BDTscores _BDTscores_init = {0};

	  NuSelectionBDT::WCPMTInfo _WCPMTInfo_init = {0};

    _stkdar_ = _stkdar_init;

    _SPID_ = _SPID_init;
    _SPSHWID1_ = _SPSHWID1_init;
	  _SPSHWID2_ = _SPSHWID2_init;
	  _SPPi0Tagger1_ = _SPPi0Tagger1_init;
	  _SPLowEMichel_ = _SPLowEMichel_init;
	  _SPBadReco1_ = _SPBadReco1_init;
	  _SPBadReco2_ = _SPBadReco2_init;
	  _SPBadReco3_ = _SPBadReco3_init;
	  _SPBadReco4_ = _SPBadReco4_init;
	  _SPHighEoverlap_ = _SPHighEoverlap_init;
	  _SPLowEoverlap_ = _SPLowEoverlap_init;

    _CosmicTagger_ = _CosmicTagger_init;
	  _GapID_ = _GapID_init;
	  _MipCheck_ = _MipCheck_init;
	  _MipID1_ = _MipID1_init;
	  _MipID2_ = _MipID2_init;
	  _Pi0Tagger1_ = _Pi0Tagger1_init;
	  _Pi0Tagger2_ = _Pi0Tagger2_init;
	  _MultiGamma1_ = _MultiGamma1_init;
	  _MultiGamma2_ = _MultiGamma2_init;
	  _SingleGamma1_ = _SingleGamma1_init;
	  _SingleGamma2_ = _SingleGamma2_init;
	  _StemLen_ = _StemLen_init;
	  _LowEMichel_ = _LowEMichel_init;
	  _BrokenMuon_ = _BrokenMuon_init;
	  _MuEnergy_ = _MuEnergy_init;
	  _ShowerAngle_ = _ShowerAngle_init;
	  _BadStem_ = _BadStem_init;
	  _VtxInShw_ = _VtxInShw_init;
	  _BadReco1_ = _BadReco1_init;
	  _BadReco2_ = _BadReco2_init;
	  _BadReco3_ = _BadReco3_init;
	  _BadReco4_ = _BadReco4_init;
	  _TrackOverCluster_ = _TrackOverCluster_init;
	  _HighEoverlap_ = _HighEoverlap_init;
	  _LowEoverlap_ = _LowEoverlap_init;
	  _MajorCosmicTagger_ = _MajorCosmicTagger_init;
	  _NumuCCTagger_ = _NumuCCTagger_init;
	  _BDTscores_ = _BDTscores_init;

	  _WCPMTInfo_ = _WCPMTInfo_init;
  }

  void NuSelectionBDT::Setstkdar(NuSelectionBDT::stkdar BDT_input){ this->_stkdar_=BDT_input; }

  void NuSelectionBDT::SetSPID(NuSelectionBDT::SPID BDT_input){ this->_SPID_=BDT_input; }
  void NuSelectionBDT::SetSPSHWID1(NuSelectionBDT::SPSHWID1 BDT_input){ this->_SPSHWID1_=BDT_input; }
  void NuSelectionBDT::SetSPSHWID2(NuSelectionBDT::SPSHWID2 BDT_input){ this->_SPSHWID2_=BDT_input; }
  void NuSelectionBDT::SetSPPi0Tagger1(NuSelectionBDT::SPPi0Tagger1 BDT_input){ this->_SPPi0Tagger1_=BDT_input; }
  void NuSelectionBDT::SetSPLowEMichel(NuSelectionBDT::SPLowEMichel BDT_input){ this->_SPLowEMichel_=BDT_input; }
  void NuSelectionBDT::SetSPBadReco1(NuSelectionBDT::SPBadReco1 BDT_input){ this->_SPBadReco1_=BDT_input; }
  void NuSelectionBDT::SetSPBadReco2(NuSelectionBDT::SPBadReco2 BDT_input){ this->_SPBadReco2_=BDT_input; }
  void NuSelectionBDT::SetSPBadReco3(NuSelectionBDT::SPBadReco3 BDT_input){ this->_SPBadReco3_=BDT_input; }
  void NuSelectionBDT::SetSPBadReco4(NuSelectionBDT::SPBadReco4 BDT_input){ this->_SPBadReco4_=BDT_input; }
  void NuSelectionBDT::SetSPHighEoverlap(NuSelectionBDT::SPHighEoverlap BDT_input){ this->_SPHighEoverlap_=BDT_input; }
  void NuSelectionBDT::SetSPLowEoverlap(NuSelectionBDT::SPLowEoverlap BDT_input){ this->_SPLowEoverlap_=BDT_input; }

  void NuSelectionBDT::SetCosmicTagger(NuSelectionBDT::CosmicTagger BDT_input){ this->_CosmicTagger_=BDT_input; }
  void NuSelectionBDT::SetGapID(NuSelectionBDT::GapID BDT_input){ this->_GapID_=BDT_input; }
  void NuSelectionBDT::SetMipCheck(NuSelectionBDT::MipCheck BDT_input){ this->_MipCheck_=BDT_input; }
  void NuSelectionBDT::SetMipID1(NuSelectionBDT::MipID1 BDT_input){ this->_MipID1_=BDT_input; }
  void NuSelectionBDT::SetMipID2(NuSelectionBDT::MipID2 BDT_input){ this->_MipID2_=BDT_input; }
  void NuSelectionBDT::SetPi0Tagger1(NuSelectionBDT::Pi0Tagger1 BDT_input){ this->_Pi0Tagger1_=BDT_input; }
  void NuSelectionBDT::SetPi0Tagger2(NuSelectionBDT::Pi0Tagger2 BDT_input){ this->_Pi0Tagger2_=BDT_input; }
  void NuSelectionBDT::SetMultiGamma1(NuSelectionBDT::MultiGamma1 BDT_input){ this->_MultiGamma1_=BDT_input; }
  void NuSelectionBDT::SetMultiGamma2(NuSelectionBDT::MultiGamma2 BDT_input){ this->_MultiGamma2_=BDT_input; }
  void NuSelectionBDT::SetSingleGamma1(NuSelectionBDT::SingleGamma1 BDT_input){ this->_SingleGamma1_=BDT_input; }
  void NuSelectionBDT::SetSingleGamma2(NuSelectionBDT::SingleGamma2 BDT_input){ this->_SingleGamma2_=BDT_input; }
  void NuSelectionBDT::SetStemLen(NuSelectionBDT::StemLen BDT_input){ this->_StemLen_=BDT_input; }
  void NuSelectionBDT::SetLowEMichel(NuSelectionBDT::LowEMichel BDT_input){ this->_LowEMichel_=BDT_input; }
  void NuSelectionBDT::SetBrokenMuon(NuSelectionBDT::BrokenMuon BDT_input){ this->_BrokenMuon_=BDT_input; }
  void NuSelectionBDT::SetMuEnergy(NuSelectionBDT::MuEnergy BDT_input){ this->_MuEnergy_=BDT_input; }
  void NuSelectionBDT::SetShowerAngle(NuSelectionBDT::ShowerAngle BDT_input){ this->_ShowerAngle_=BDT_input; }
  void NuSelectionBDT::SetBadStem(NuSelectionBDT::BadStem BDT_input){ this->_BadStem_=BDT_input; }
  void NuSelectionBDT::SetVtxInShw(NuSelectionBDT::VtxInShw BDT_input){ this->_VtxInShw_=BDT_input; }
  void NuSelectionBDT::SetBadReco1(NuSelectionBDT::BadReco1 BDT_input){ this->_BadReco1_=BDT_input; }
  void NuSelectionBDT::SetBadReco2(NuSelectionBDT::BadReco2 BDT_input){ this->_BadReco2_=BDT_input; }
  void NuSelectionBDT::SetBadReco3(NuSelectionBDT::BadReco3 BDT_input){ this->_BadReco3_=BDT_input; }
  void NuSelectionBDT::SetBadReco4(NuSelectionBDT::BadReco4 BDT_input){ this->_BadReco4_=BDT_input; }
  void NuSelectionBDT::SetTrackOverCluster(NuSelectionBDT::TrackOverCluster BDT_input){ this->_TrackOverCluster_=BDT_input; }
  void NuSelectionBDT::SetHighEoverlap(NuSelectionBDT::HighEoverlap BDT_input){ this->_HighEoverlap_=BDT_input; }
  void NuSelectionBDT::SetLowEoverlap(NuSelectionBDT::LowEoverlap BDT_input){ this->_LowEoverlap_=BDT_input; }
  void NuSelectionBDT::SetMajorCosmicTagger(NuSelectionBDT::MajorCosmicTagger BDT_input){ this->_MajorCosmicTagger_=BDT_input; }
  void NuSelectionBDT::SetNumuCCTagger(NuSelectionBDT::NumuCCTagger BDT_input){ this->_NumuCCTagger_=BDT_input; }
  void NuSelectionBDT::SetBDTscores(NuSelectionBDT::BDTscores BDT_input){ this->_BDTscores_=BDT_input; }

  void NuSelectionBDT::SetWCPMTInfo(NuSelectionBDT::WCPMTInfo BDT_input){ this->_WCPMTInfo_=BDT_input; }

  const NuSelectionBDT::stkdar & NuSelectionBDT::Getstkdar() const { return this->_stkdar_; }

  const NuSelectionBDT::SPID & NuSelectionBDT::GetSPID() const { return this->_SPID_; }
  const NuSelectionBDT::SPSHWID1 & NuSelectionBDT::GetSPSHWID1() const { return this->_SPSHWID1_; }
  const NuSelectionBDT::SPSHWID2 & NuSelectionBDT::GetSPSHWID2() const { return this->_SPSHWID2_; }
  const NuSelectionBDT::SPPi0Tagger1 & NuSelectionBDT::GetSPPi0Tagger1() const { return this->_SPPi0Tagger1_; }
  const NuSelectionBDT::SPLowEMichel & NuSelectionBDT::GetSPLowEMichel() const { return this->_SPLowEMichel_; }
  const NuSelectionBDT::SPBadReco1 & NuSelectionBDT::GetSPBadReco1() const { return this->_SPBadReco1_; }
  const NuSelectionBDT::SPBadReco2 & NuSelectionBDT::GetSPBadReco2() const { return this->_SPBadReco2_; }
  const NuSelectionBDT::SPBadReco3 & NuSelectionBDT::GetSPBadReco3() const { return this->_SPBadReco3_; }
  const NuSelectionBDT::SPBadReco4 & NuSelectionBDT::GetSPBadReco4() const { return this->_SPBadReco4_; }
  const NuSelectionBDT::SPHighEoverlap & NuSelectionBDT::GetSPHighEoverlap() const { return this->_SPHighEoverlap_; }
  const NuSelectionBDT::SPLowEoverlap & NuSelectionBDT::GetSPLowEoverlap() const { return this->_SPLowEoverlap_; }

  const NuSelectionBDT::CosmicTagger & NuSelectionBDT::GetCosmicTagger() const { return this->_CosmicTagger_; }
  const NuSelectionBDT::GapID & NuSelectionBDT::GetGapID() const { return this->_GapID_; }
  const NuSelectionBDT::MipCheck & NuSelectionBDT::GetMipCheck() const { return this->_MipCheck_; }
  const NuSelectionBDT::MipID1 & NuSelectionBDT::GetMipID1() const { return this->_MipID1_; }
  const NuSelectionBDT::MipID2 & NuSelectionBDT::GetMipID2() const { return this->_MipID2_; }
  const NuSelectionBDT::Pi0Tagger1 & NuSelectionBDT::GetPi0Tagger1() const { return this->_Pi0Tagger1_; }
  const NuSelectionBDT::Pi0Tagger2 & NuSelectionBDT::GetPi0Tagger2() const { return this->_Pi0Tagger2_; }
  const NuSelectionBDT::MultiGamma1 & NuSelectionBDT::GetMultiGamma1() const { return this->_MultiGamma1_; }
  const NuSelectionBDT::MultiGamma2 & NuSelectionBDT::GetMultiGamma2() const { return this->_MultiGamma2_; }
  const NuSelectionBDT::SingleGamma1 & NuSelectionBDT::GetSingleGamma1() const { return this->_SingleGamma1_; }
  const NuSelectionBDT::SingleGamma2 & NuSelectionBDT::GetSingleGamma2() const { return this->_SingleGamma2_; }
  const NuSelectionBDT::StemLen & NuSelectionBDT::GetStemLen() const { return this->_StemLen_; }
  const NuSelectionBDT::LowEMichel & NuSelectionBDT::GetLowEMichel() const { return this->_LowEMichel_; }
  const NuSelectionBDT::BrokenMuon & NuSelectionBDT::GetBrokenMuon() const { return this->_BrokenMuon_; }
  const NuSelectionBDT::MuEnergy & NuSelectionBDT::GetMuEnergy() const { return this->_MuEnergy_; }
  const NuSelectionBDT::ShowerAngle & NuSelectionBDT::GetShowerAngle() const { return this->_ShowerAngle_; }
  const NuSelectionBDT::BadStem & NuSelectionBDT::GetBadStem() const { return this->_BadStem_; }
  const NuSelectionBDT::VtxInShw & NuSelectionBDT::GetVtxInShw() const { return this->_VtxInShw_; }
  const NuSelectionBDT::BadReco1 & NuSelectionBDT::GetBadReco1() const { return this->_BadReco1_; }
  const NuSelectionBDT::BadReco2 & NuSelectionBDT::GetBadReco2() const { return this->_BadReco2_; }
  const NuSelectionBDT::BadReco3 & NuSelectionBDT::GetBadReco3() const { return this->_BadReco3_; }
  const NuSelectionBDT::BadReco4 & NuSelectionBDT::GetBadReco4() const { return this->_BadReco4_; }
  const NuSelectionBDT::TrackOverCluster & NuSelectionBDT::GetTrackOverCluster() const { return this->_TrackOverCluster_; }
  const NuSelectionBDT::HighEoverlap & NuSelectionBDT::GetHighEoverlap() const { return this->_HighEoverlap_; }
  const NuSelectionBDT::LowEoverlap & NuSelectionBDT::GetLowEoverlap() const { return this->_LowEoverlap_; }
  const NuSelectionBDT::MajorCosmicTagger & NuSelectionBDT::GetMajorCosmicTagger() const { return this->_MajorCosmicTagger_; }
  const NuSelectionBDT::NumuCCTagger & NuSelectionBDT::GetNumuCCTagger() const { return this->_NumuCCTagger_; }
  const NuSelectionBDT::BDTscores & NuSelectionBDT::GetBDTscores() const { return this->_BDTscores_; }

  const NuSelectionBDT::WCPMTInfo & NuSelectionBDT::GetWCPMTInfo() const { return this->_WCPMTInfo_; }
