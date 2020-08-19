#ifndef NuSelectionKINE_h
#define NuSelectionKINE_h

#include <vector>

namespace nsm{
  // selected kinematic variables
  // neutrino energy
  // pi0 mass
  class NuSelectionKINE{
  public:
  NuSelectionKINE();
  void reset(); 

  struct KineInfo{
    // energy ...
    float kine_reco_Enu; // kinetic energy  + additional energy ...
    float kine_reco_add_energy;  // mass, binding energy ...
    std::vector<float> *kine_energy_particle;  // energy of each particle
    std::vector<int> *kine_energy_info; // what kind of energy reconstruction?
    std::vector<int> *kine_particle_type;
    std::vector<int> *kine_energy_included; // included in the neutrino energy calculation?

    // pi0 information ...
    float kine_pio_mass; // mass
    int kine_pio_flag; // 0 not filled, 1, with vertex: CCpio, 2 without vertex: NCpi0
    float kine_pio_energy_1;
    float kine_pio_theta_1;
    float kine_pio_phi_1;
    float kine_pio_dis_1;
    float kine_pio_energy_2;
    float kine_pio_theta_2;
    float kine_pio_phi_2;
    float kine_pio_dis_2;
    float kine_pio_angle;
  }; 
  void SetKineInfo(KineInfo);
  const KineInfo & GetKineInfo() const;

  private:
	KineInfo _KineInfo_;
  };	
}

#endif

