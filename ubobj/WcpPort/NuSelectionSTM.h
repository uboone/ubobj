#ifndef NuSelectionSTM_h
#define NuSelectionSTM_h

#include <vector>

namespace nsm{

  class NuSelectionSTM{
  public:
    NuSelectionSTM();
    NuSelectionSTM(int event_type,
		     int flag_low_energy,
		     int flag_LM,
		     int flag_TGM,
		     int flag_STM,
		     int flag_full_detector_dead,
		     float cluster_length);

    void SetEventType(int);
    void SetLowEnergy(int);
    void SetLM(int);
    void SetTGM(int);
    void SetSTM(int);
    void SetFullDead(int);
    void SetClusterLength(float);
    
    const int & GetEventType() const;
    const int & GetLowEnergy() const;
    const int & GetLM() const;
    const int & GetTGM() const;
    const int & GetSTM() const;
    const int & GetFullDead() const;
    const float & GetClusterLength() const;


  private:
    int _event_type;
    int _flag_low_energy;
    int _flag_LM;
    int _flag_TGM;
    int _flag_STM;
    int _flag_full_detector_dead;
    float _cluster_length;
  };
}

#endif

