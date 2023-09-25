#ifndef einstein_
#define einstein_
#include "kinematics.h"
#include <cmath>
#include <iostream>

/**
 * This class performs relativistics kinematics
 */

class CEinstein : public CKinematics
{
 public:
  float const c;
  float const nMass;
  float const scale;
  CEinstein();
  //void AddVelocities(float*, float*, float, float*);
  //void FindCenterOfMass(float* , float, float*, float);
  //float getVelocity(float,float);
  float getMomentum(float eKin,float mass);
  float getKE(float pc,float mass);
  float transformMomentum(float* mom,float* vreference,float energyTot,
    float*momNew);
  float gamma(float vel);
};
#endif 
