////////////////////////////////////////////////////////////////////////
// \file SpaceChargeMicroBooNE.cxx
//
// \brief implementation of class for storing/accessing space charge distortions for MicroBooNE
//
// \author mrmooney@bnl.gov
// 
////////////////////////////////////////////////////////////////////////

// C++ language includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "math.h"
#include "stdio.h"

// LArSoft includes
#include "uboone/SpaceCharge/SpaceChargeMicroBooNE.h"

// Framework includes
#include "cetlib/exception.h"

//-----------------------------------------------
spacecharge::SpaceChargeMicroBooNE::SpaceChargeMicroBooNE(
  fhicl::ParameterSet const& pset
)
{
  Configure(pset);
}

//------------------------------------------------
bool spacecharge::SpaceChargeMicroBooNE::Configure(fhicl::ParameterSet const& pset)
{  
  fEnableSimulationSCE = pset.get<bool>("EnableSimulationSCE");
  fEnableCorrectionsSCE = pset.get<bool>("EnableCorrectionsSCE");

  if(fEnableSimulationSCE == true)
  {
    fRepresentationType = pset.get<std::string>("RepresentationType");
    fInputFilename = pset.get<std::string>("InputFilename");

    std::string fname;
    cet::search_path sp("FW_SEARCH_PATH");
    sp.find_file(fInputFilename,fname);

    std::unique_ptr<TFile> infile(new TFile(fname.c_str(), "READ"));
    if(!infile->IsOpen()) throw cet::exception("SpaceChargeMicroBooNE") << "Could not find the space charge effect file '" << fname << "'!\n";

    if(fRepresentationType == "Parametric")
    {      
      for(int i = 0; i < 5; i++)
      {
        g1_x[i] = (TGraph*)infile->Get(Form("deltaX/g1_%d",i));
        g2_x[i] = (TGraph*)infile->Get(Form("deltaX/g2_%d",i));
        g3_x[i] = (TGraph*)infile->Get(Form("deltaX/g3_%d",i));   
        g4_x[i] = (TGraph*)infile->Get(Form("deltaX/g4_%d",i));
        g5_x[i] = (TGraph*)infile->Get(Form("deltaX/g5_%d",i));

        g1_y[i] = (TGraph*)infile->Get(Form("deltaY/g1_%d",i));
        g2_y[i] = (TGraph*)infile->Get(Form("deltaY/g2_%d",i));
        g3_y[i] = (TGraph*)infile->Get(Form("deltaY/g3_%d",i));   
        g4_y[i] = (TGraph*)infile->Get(Form("deltaY/g4_%d",i));
        g5_y[i] = (TGraph*)infile->Get(Form("deltaY/g5_%d",i));
        g6_y[i] = (TGraph*)infile->Get(Form("deltaY/g6_%d",i));

        g1_z[i] = (TGraph*)infile->Get(Form("deltaZ/g1_%d",i));
        g2_z[i] = (TGraph*)infile->Get(Form("deltaZ/g2_%d",i));
        g3_z[i] = (TGraph*)infile->Get(Form("deltaZ/g3_%d",i));   
        g4_z[i] = (TGraph*)infile->Get(Form("deltaZ/g4_%d",i));
      }

      g1_x[5] = (TGraph*)infile->Get("deltaX/g1_5");
      g2_x[5] = (TGraph*)infile->Get("deltaX/g2_5");
      g3_x[5] = (TGraph*)infile->Get("deltaX/g3_5");   
      g4_x[5] = (TGraph*)infile->Get("deltaX/g4_5");
      g5_x[5] = (TGraph*)infile->Get("deltaX/g5_5");

      g1_y[5] = (TGraph*)infile->Get("deltaY/g1_5");
      g2_y[5] = (TGraph*)infile->Get("deltaY/g2_5");
      g3_y[5] = (TGraph*)infile->Get("deltaY/g3_5");   
      g4_y[5] = (TGraph*)infile->Get("deltaY/g4_5");
      g5_y[5] = (TGraph*)infile->Get("deltaY/g5_5");
      g6_y[5] = (TGraph*)infile->Get("deltaY/g6_5");
      
      g1_x[6] = (TGraph*)infile->Get("deltaX/g1_6");
      g2_x[6] = (TGraph*)infile->Get("deltaX/g2_6");
      g3_x[6] = (TGraph*)infile->Get("deltaX/g3_6");   
      g4_x[6] = (TGraph*)infile->Get("deltaX/g4_6");
      g5_x[6] = (TGraph*)infile->Get("deltaX/g5_6");              
    }

    infile->Close();
  }

  if(fEnableCorrectionsSCE == true)
  {
    // Grab other parameters from pset  
  }

  return true;
}

//------------------------------------------------
bool spacecharge::SpaceChargeMicroBooNE::Update(uint64_t ts) 
{
  if (ts == 0) return false;

  return true;
}

//----------------------------------------------------------------------------
/// Return boolean indicating whether or not to turn simulation of SCE on
bool spacecharge::SpaceChargeMicroBooNE::EnableSimulationSCE() const
{
  return fEnableSimulationSCE;
}

//----------------------------------------------------------------------------
/// Return boolean indicating whether or not to apply SCE corrections
bool spacecharge::SpaceChargeMicroBooNE::EnableCorrectionsSCE() const
{
  return fEnableCorrectionsSCE;
}

//----------------------------------------------------------------------------
/// Primary working method of service that provides position offsets to be
/// used in ionization electron drift
std::vector<double> spacecharge::SpaceChargeMicroBooNE::GetPosOffsets(double xVal, double yVal, double zVal) const
{
  std::vector<double> thePosOffsets;

  if(IsInsideBoundaries(xVal,yVal,zVal) == false)
  {
    thePosOffsets.resize(3,0.0);
  }
  else
  {
    if(fRepresentationType == "Parametric")
      thePosOffsets = GetPosOffsetsParametric(xVal,yVal,zVal);
    else
      thePosOffsets.resize(3,0.0);
  }

  return thePosOffsets;
}

//----------------------------------------------------------------------------
/// Provides position offsets using a parametric representation
std::vector<double> spacecharge::SpaceChargeMicroBooNE::GetPosOffsetsParametric(double xVal, double yVal, double zVal) const
{
  std::vector<double> thePosOffsetsParametric;

  double xValNew = TransformX(xVal);
  double yValNew = TransformY(yVal);
  double zValNew = TransformZ(zVal);

  thePosOffsetsParametric.push_back(GetOnePosOffsetParametric(xValNew,yValNew,zValNew,"X"));
  thePosOffsetsParametric.push_back(GetOnePosOffsetParametric(xValNew,yValNew,zValNew,"Y"));
  thePosOffsetsParametric.push_back(GetOnePosOffsetParametric(xValNew,yValNew,zValNew,"Z"));

  return thePosOffsetsParametric;
}

//----------------------------------------------------------------------------
/// Provides one position offset using a parametric representation, for a given
/// axis
double spacecharge::SpaceChargeMicroBooNE::GetOnePosOffsetParametric(double xValNew, double yValNew, double zValNew, std::string axis) const
{      
  double parA[6][7];
  double parB[6];
  
  for(int j = 0; j < 6; j++)
  {
    for(int i = 0; i < 7; i++)
      parA[j][i] = 0.0;
  
    parB[j] = 0.0;
  }
  
  if(axis == "X")
  {
    for(int j = 0; j < 7; j++)
    {
      parA[0][j] = g1_x[j]->Eval(zValNew);
      parA[1][j] = g2_x[j]->Eval(zValNew);
      parA[2][j] = g3_x[j]->Eval(zValNew);
      parA[3][j] = g4_x[j]->Eval(zValNew);
      parA[4][j] = g5_x[j]->Eval(zValNew);
    }
  
    f1_x->SetParameters(parA[0]);
    f2_x->SetParameters(parA[1]);
    f3_x->SetParameters(parA[2]);
    f4_x->SetParameters(parA[3]);
    f5_x->SetParameters(parA[4]);
  }
  else if(axis == "Y")
  {
    for(int j = 0; j < 6; j++)
    {
      parA[0][j] = g1_y[j]->Eval(zValNew);
      parA[1][j] = g2_y[j]->Eval(zValNew);
      parA[2][j] = g3_y[j]->Eval(zValNew);
      parA[3][j] = g4_y[j]->Eval(zValNew);
      parA[4][j] = g5_y[j]->Eval(zValNew);
      parA[5][j] = g6_y[j]->Eval(zValNew);
    }
  
    f1_y->SetParameters(parA[0]);
    f2_y->SetParameters(parA[1]);
    f3_y->SetParameters(parA[2]);
    f4_y->SetParameters(parA[3]);
    f5_y->SetParameters(parA[4]);
    f6_y->SetParameters(parA[5]);
  }
  else if(axis == "Z")
  {
    for(int j = 0; j < 5; j++)
    {
      parA[0][j] = g1_z[j]->Eval(zValNew);
      parA[1][j] = g2_z[j]->Eval(zValNew);
      parA[2][j] = g3_z[j]->Eval(zValNew);
      parA[3][j] = g4_z[j]->Eval(zValNew);
    }
  
    f1_z->SetParameters(parA[0]);
    f2_z->SetParameters(parA[1]);
    f3_z->SetParameters(parA[2]);
    f4_z->SetParameters(parA[3]);
  }
  
  double aValNew;
  double bValNew;
  
  if(axis == "Y")
  {
    aValNew = xValNew;
    bValNew = yValNew;
  }
  else
  {
    aValNew = yValNew;
    bValNew = xValNew;
  }
  
  double offsetValNew = 0.0;
  if(axis == "X")
  {
    parB[0] = f1_x->Eval(aValNew);
    parB[1] = f2_x->Eval(aValNew);
    parB[2] = f3_x->Eval(aValNew);
    parB[3] = f4_x->Eval(aValNew);
    parB[4] = f5_x->Eval(aValNew);
  
    fFinal_x->SetParameters(parB);
    offsetValNew = 100.0*fFinal_x->Eval(bValNew);
  }
  else if(axis == "Y")
  {
    parB[0] = f1_y->Eval(aValNew);
    parB[1] = f2_y->Eval(aValNew);
    parB[2] = f3_y->Eval(aValNew);
    parB[3] = f4_y->Eval(aValNew);
    parB[4] = f5_y->Eval(aValNew);
    parB[5] = f6_y->Eval(aValNew);
  
    fFinal_y->SetParameters(parB);
    offsetValNew = 100.0*fFinal_y->Eval(bValNew);
  }
  else if(axis == "Z")
  {
    parB[0] = f1_z->Eval(aValNew);
    parB[1] = f2_z->Eval(aValNew);
    parB[2] = f3_z->Eval(aValNew);
    parB[3] = f4_z->Eval(aValNew);
  
    fFinal_z->SetParameters(parB);
    offsetValNew = 100.0*fFinal_z->Eval(bValNew);
  }
  
  return offsetValNew;
}

//----------------------------------------------------------------------------
/// Transform X to SCE X coordinate:  [2.56,0.0] --> [0.0,2.50]
double spacecharge::SpaceChargeMicroBooNE::TransformX(double xVal) const
{
  double xValNew;
  xValNew = 2.50 - (2.50/2.56)*(xVal/100.0);
  xValNew -= 1.25;

  return xValNew;
}

//----------------------------------------------------------------------------
/// Transform Y to SCE Y coordinate:  [-1.165,1.165] --> [0.0,2.50]
double spacecharge::SpaceChargeMicroBooNE::TransformY(double yVal) const
{
  double yValNew;
  yValNew = (2.50/2.33)*((yVal/100.0)+1.165);
  yValNew -= 1.25;

  return yValNew;
}

//----------------------------------------------------------------------------
/// Transform Z to SCE Z coordinate:  [0.0,10.37] --> [0.0,10.0]
double spacecharge::SpaceChargeMicroBooNE::TransformZ(double zVal) const
{
  double zValNew;
  zValNew = (10.0/10.37)*(zVal/100.0);

  return zValNew;
}

//----------------------------------------------------------------------------
/// Check to see if point is inside boundaries of map (allow to go slightly out of range)
bool spacecharge::SpaceChargeMicroBooNE::IsInsideBoundaries(double xVal, double yVal, double zVal) const
{
  bool isInside = true;

  if((xVal < 0.0) || (xVal > 260.0) || (yVal < -120.0) || (yVal > 120.0) || (zVal < 0.0) || (zVal > 1050.0))
  {
    isInside = false;
  }

  return isInside;
}
