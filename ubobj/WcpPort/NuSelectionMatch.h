#ifndef NuSelectionMatch_h
#define NuSelectionMatch_h

#include <vector>

namespace nsm{

  class NuSelectionMatch{
  public:
    NuSelectionMatch();
    NuSelectionMatch(float completeness,
		     float completeness_energy,
		     float purity,
		     float purity_xy,
		     float purity_xz);
    //virtual ~NuSelectionMatch();

    void SetCompleteness(float);
    void SetCompletenessEnergy(float);
    void SetPurity(float);
    void SetPurityXY(float);
    void SetPurityXZ(float);

    const float & GetCompleteness() const;
    const float & GetCompletenessEnergy() const;
    const float & GetPurity() const;
    const float & GetPurityXY() const;
    const float & GetPurityXZ() const;

  private:
    float _completeness;
    float _completeness_energy;
    float _purity;
    float _purity_xy;
    float _purity_xz;
  };
}

#endif

