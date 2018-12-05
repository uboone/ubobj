/**
 * \class CRTData
 *
 * \ingroup crt
 *
 * \brief The basic structure of data coming from the CRT in both
 * Simulations and Data.
 *
 * \author $Author: Kevin Wierman<kevin.wierman@pnnl.gov> $
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2016/12/12 $
 *
 * Contact: kevin.wierman@pnnl.gov
 *
 * Created on: Tuesday, December 13, 2016
 *
 */


#ifndef CRTSimData_hh_
#define CRTSimData_hh_

#include <stdint.h>

namespace crt {

  class CRTSimData {
    public:
    uint32_t fChannel; // The channel number which can be referenced to module, strip via the channel     
    uint32_t fT0; // Time relative to GPS    
    uint32_t fT1; // Time relative to beam trigger    
    uint32_t fADC;  // ADC value returned by CRT
    int fTrackID;

    CRTSimData();
    CRTSimData(uint32_t channel, uint32_t t0, uint32_t t1, uint32_t adc, int trackID);

    //    virtual ~CRTSimData() {};
    ~CRTSimData();

  };  //end class definition


  CRTSimData::CRTSimData(): fChannel(0), fT0(0), fT1(0), fADC(0), fTrackID(-1){
  }
  CRTSimData::CRTSimData(uint32_t channel, uint32_t t0, 
			 uint32_t t1, uint32_t adc,int trackID):
    fChannel(channel),
    fT0(t0),
    fT1(t1),
    fADC(adc), 
    fTrackID(trackID) {
    }
  CRTSimData::~CRTSimData() {
  }
  /*
  uint32_t CRTSimData::Channel() const { 
    return fChannel;
  }
  uint32_t CRTSimData::T0()  const {
    return fT0;
  }
  uint32_t CRTSimData::T1()  const {
    return fT1;
  }
  uint32_t CRTSimData::ADC()  const {
    return fADC;
  }
  int CRTSimData::TrackID()   const {
    return fTrackID;
  }
  */



} // namespace CRT

#endif
