#include "NuSelectionSTM.h"
#include <vector>

using namespace nsm;

  NuSelectionSTM::NuSelectionSTM() {
    _event_type=-1;
    _flag_low_energy=-1;
    _flag_LM=-1;
    _flag_TGM=-1;
    _flag_STM=-1;
    _flag_full_detector_dead=-1;
    _cluster_length=-1.;
  }

  NuSelectionSTM::NuSelectionSTM( int event_type,
		     int flag_low_energy,
		     int flag_LM,
		     int flag_TGM,
		     int flag_STM,
		     int flag_full_detector_dead,
		     float cluster_length){
    
    _event_type=event_type;
    _flag_low_energy=flag_low_energy;
    _flag_LM=flag_LM;
    _flag_TGM=flag_TGM;
    _flag_STM=flag_STM;
    _flag_full_detector_dead=flag_full_detector_dead;
    _cluster_length=cluster_length;

  }

    void NuSelectionSTM::SetEventType(int event_type){ this->_event_type=event_type; }
    void NuSelectionSTM::SetLowEnergy(int flag_low_energy){ this->_flag_low_energy=flag_low_energy; }
    void NuSelectionSTM::SetLM(int flag_LM){ this->_flag_LM=flag_LM; }
    void NuSelectionSTM::SetTGM(int flag_TGM){ this->_flag_TGM=flag_TGM; }
    void NuSelectionSTM::SetSTM(int flag_STM){ this->_flag_STM=flag_STM; }
    void NuSelectionSTM::SetFullDead(int flag_full_detector_dead){ this->_flag_full_detector_dead=flag_full_detector_dead; }
    void NuSelectionSTM::SetClusterLength(float cluster_length){ this->_cluster_length=cluster_length; }

    const int & NuSelectionSTM::GetEventType() const { return this->_event_type; }
    const int & NuSelectionSTM::GetLowEnergy() const { return this->_flag_low_energy; }
    const int & NuSelectionSTM::GetLM() const { return this->_flag_LM; }
    const int & NuSelectionSTM::GetTGM() const { return this->_flag_TGM; }
    const int & NuSelectionSTM::GetSTM() const { return this->_flag_STM; }
    const int & NuSelectionSTM::GetFullDead() const { return this->_flag_full_detector_dead; }
    const float & NuSelectionSTM::GetClusterLength() const { return this->_cluster_length; }

