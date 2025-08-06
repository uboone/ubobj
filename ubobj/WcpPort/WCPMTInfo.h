#ifndef WCPMTInfo_h
#define WCPMTInfo_h

#include <vector>

namespace nsm{

struct WCPMTInfo{
	std::vector<double> *WCPMTInfoPePred;
	std::vector<double> *WCPMTInfoPeMeas;
	std::vector<double> *WCPMTInfoPeMeasErr;
	int WCPMTInfoTPCClusterID;
	int WCPMTInfoFlashID;
	double WCPMTInfoStrength;
	int WCPMTInfoEventType;
	double WCPMTInfoKSDistance;
	double WCPMTInfoChi2;
	int WCPMTInfoNDF;
	double WCPMTInfoClusterLength;
	int WCPMTInfoNeutrinoType;
	double WCPMTInfoFlashTime;
};

}

#endif
