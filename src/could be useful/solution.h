#ifndef _solution
#define _solution

#include <cmath>

#define rel

#ifdef rel
#include "einstein.h"
#else
#include "newton.h"
#endif

/**
 * ~\brief find detected particles from hira data
 * 
 * This class finds the incident particles from the 
 * detected Si and CsI info
 */
class solution
{
  public:
#ifdef rel
  CEinstein Kinematics;
#else
  CNewton Kinematics;
#endif

  float denergy;
  float energy;
  float penergy;
  float energyR; //!< raw csi channels
  float energyCsI;
  int icsi;
  int iPie;
  int iRing;
  int ifront;
  int iback;
  int ide;
  int ipid;
  int itele;
  bool be8;
  // int istrip;
  float distTarget;
  float mass;
  int A;
  int iZ;
  int iA;
  float Ekin;
  float energyTot;
  float Etot_cm;
  float Xpos;
  float Ypos;
  float theta;
  float phi;
  float theta_cm;
  float velocity;
  float velocity_cm;
  float Vvect[3];
  float Vcmvect[3];
  float Mvect[3];
  float MomCM[3];
  float momentum;
  float momentumCM;
  float EnergyTot;
  float MomRot[3];
  float MomRot2[3];
  float energyCM;
  float rigidity; //rigidity detected at the S800, before energy losses

  float KE;
  float deltathP_lab;
  float deltathP_cm;
  float deltathA_lab;
  float deltathA_cm;
  float Vlab;
  int Nbefore;
  int Norder;

  int setPID(int);
  int setEnergy(float deltaE, float E);
  float angle();
  void getMomentum();
};


#endif
