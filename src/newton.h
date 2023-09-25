#ifndef newton_
#define newton_
#include "kinematics.h"
#include <cmath>
#include <iostream>

/**
 * This class perfoms Newtonian kinematics
 */

class CNewton : public CKinematics
{
 public:
  float const c;
  float const nMass;
  float const scale;
  CNewton();
  float getMomentum(float eKin,float mass);
  float transformMomentum(float* mom,float* vreference,float energyTot,
    float*momNew);
  float gamma(float vel);
};
#endif 
