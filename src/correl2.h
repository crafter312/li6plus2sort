
#include "parType.h"
#include "constants.h"
#include <vector>

using namespace std;

class correl2
{
 public:


 #ifdef rel
  CEinstein Kinematics;
 #else
  CNewton Kinematics;
 #endif


  correl2();
  void zeroMask();
  void reset();
  void makeArray(bool flagMask = 0);
  void load(solution * fragment);
  void getJacobi();
  float findErel();
  float missingmass();
  float Qvalue();
  float Qvalue2();
  float TargetEx();
  int Nparticles;
  int N;
  solution * frag[7];

  parType proton;
  parType H2;
  parType H3;
  parType H3_fake;
  parType He3;
  parType alpha;
  parType He6;
  parType Li6;
  parType Li7;
  parType Li8;
  parType Li9;
  parType Be7;
  parType Be9;



  vector<parType*> particle;

  float Vcmframevector[3];
  float Vcmframe;
  float VcmframePerp;
  float VcmframePara;
  float TKEL;
  float Erel;
  float KEToT;
  float thetaCM;
  float phiCM;
  float thetaInReactCM;

  float check_ke;
  float check_mass;


  int ifront;
  int istrip;
  int itele;
  // int Norder;
  float Vlab;

  float cos_thetaH;

  float thetaAlpha;
  float phiAlpha;
  float Thetaopen;
  float ppThetaRel;
  float cosThetapp;
  // float protvel1; 
  // float protvel2; 
  float pTheta0;
  float pTheta2;
  float pTheta3;
  float pTheta1;
  float aTheta0;
  float pThetaMax;
  float aTheta1;
  float thetadip;
  float Epp;
  float CosAngle;

  float CosAngleY1;
  float CosAngleY2;
  float Ey1;
  float Ey2;

  float cosBeamCMangle; //-ND

  float Ey1_10;
  float Ey2_10;
  float CosAngleAlphaY1_10;
  float CosAngleAlphaY2_10;

  float Epp_10;
  float CosAngle_10;

  float CosAngleAlpha;
  float CosAngleAlphaY1;
  float CosAngleAlphaY2;
  float Eaa;
  float ThetaP;
  float ThetaA1;
  float ThetaA2;
  float PhiP;
  float PhiA1;
  float PhiA2;
  float cosAA;
 
  float aarelbeamangle;
  float prelbeamangle;

  float Eap_min, Eap_max;

  float ThetaA1_P, ThetaA2_P;

  float cosAlpha[2];
  float cosProton0[2];
  float cosProton1[2];
  float energy3;

  float velocityCM;
  float momentumCM;


  //jacobi

  float cosThetaT;
  float cosThetaY[2];
  float cosThetaV;
  float getAlphaMom();
  float x_T;
  float x_Y[2];
  void rotate();


  float momC[3];
  float Mtot[3];
  float cosThetaC;
  float PperpC;
  float PparaC;
  float PtotC;
  float cosAlphaQ;

};
