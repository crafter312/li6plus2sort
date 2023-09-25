#include "einstein.h"

using namespace std;

CEinstein::CEinstein():c(30.),nMass(931.478),scale(1.),CKinematics() {} 

//*********************************************************
float CEinstein::getMomentum(float eKin, float mass)
{
  float pc = sqrt(pow(eKin+mass,2) - pow(mass,2));
  return pc;
}

//*********************************************************
float CEinstein::getKE(float pc, float mass)
{
  float ek = sqrt(pow(pc,2) + pow(mass,2))-mass;
  return ek;
}
//********************************************************
  /**
   *transform a momentum vector to new frame
   * and returns the new kinetic energy in MeV
   */
float CEinstein::transformMomentum(float* mom, float *Vreference, 
				  float energyTot, float* momNew)
{

  //find momentum parallel and perpendicular to transfrom velocity
  float dot = 0.;
  float VVreference = 0.;
  float perp[3];
  float para[3];
  float paraOld = 0.;
  for (int i=0;i<3;i++) 
  {
    dot += mom[i]*Vreference[i];
    VVreference += pow(Vreference[i],2);
  }
  //take projections on vreference vector (z-axis down beam)
  for (int i=0;i<3;i++) 
  {
    para[i] = dot/VVreference*Vreference[i];
    perp[i] = mom[i] - para[i];
    paraOld += pow(para[i],2);
  }

  
  paraOld = sqrt(paraOld); // magnitude of parallel mometum
  VVreference = sqrt(VVreference); // magnidtiude of velocity shift

  //transform parallel component
  float gamma = 1./sqrt(1-pow(VVreference/c,2));
  float paraNew = (paraOld - energyTot*VVreference/c)*gamma;

  // add perpendicular and new parallel components
  for (int j=0;j<3;j++)
  {
    momNew[j] = perp[j] + paraNew/paraOld*para[j];
  }


  float energyTotNew = gamma*(energyTot - VVreference*paraOld/c);
  //cout << gamma << " " << energyTot << " " << VVreference << " " << paraOld << endl;
  //cout << mom[0] << " " << mom[1] << " " << mom[2] << endl;
  //cout << Vreference[0] << " " << Vreference[1] << " " << Vreference[1] << endl;
  //cout << "a"<< endl;
  return energyTotNew;
}
//**************************************************
float CEinstein::gamma(float vel)
{
  return 1./sqrt(1-pow(vel/c,2));
}
