/** ****************************************************************************
 * @file UbooneOpticalFilter.h
 * @brief Definition of basic information for the common optical filter
 * @author wketchum@fnal.gov
 * 
 * ****************************************************************************/

#ifndef UBOONEOBJ_UBOONEOPTICALFILTER_H
#define UBOONEOBJ_UBOONEOPTICALFILTER_H

#include <stdint.h>

namespace uboone {

  class UbooneOpticalFilter {

  public:
  UbooneOpticalFilter():
    fPE_Beam(-999),fPE_Veto(-999),fPMT_MaxFraction(-999),
      fPE_Beam_Total(-999), fPE_Veto_Total(-999)
      {} 
    
#ifndef __GCCXML__
  public:
      
    UbooneOpticalFilter(float pe_b, float pe_veto, float pmt_maxfrac,
			float pe_b_tot, float pe_v_tot)
      { fPE_Beam = pe_b; fPE_Veto = pe_veto; fPMT_MaxFraction = pmt_maxfrac;
	fPE_Beam_Total = pe_b_tot; fPE_Veto_Total = pe_v_tot;
      }

    float PE_Beam() const;         /// integrated pe in "beam" window, bins above thresh
    float PE_Veto() const;         /// integrated pe in "veto" region, bins above thresh
    float PE_Beam_Total() const;         /// integrated pe in "beam" window, all bins
    float PE_Veto_Total() const;         /// integrated pe in "veto" region, all bins
    float PMT_MaxFraction() const; /// max fraction of pe in single PMT

#endif // !__GCCXML__
  private:

    float fPE_Beam;
    float fPE_Veto;
    float fPMT_MaxFraction;
    float fPE_Beam_Total;
    float fPE_Veto_Total;

  }; // class UbooneOpticalFilter()
  
#ifndef __GCCXML__
  inline float uboone::UbooneOpticalFilter::PE_Beam() const { return fPE_Beam; }
  inline float uboone::UbooneOpticalFilter::PE_Veto() const { return fPE_Veto; }
  inline float uboone::UbooneOpticalFilter::PMT_MaxFraction() const { return fPMT_MaxFraction; }
  inline float uboone::UbooneOpticalFilter::PE_Beam_Total() const { return fPE_Beam_Total; }
  inline float uboone::UbooneOpticalFilter::PE_Veto_Total() const { return fPE_Veto_Total; }
#endif // !__GCCXML__
  
} // namespace uboone


#endif // UBOONEOBJ_UBOONEOPTICALFILTER_H

////////////////////////////////////////////////////////////////////////
