#ifndef ringCounter_
#define ringCounter_

#include "histo_sort.h"
#include "elist.h"
#include "pid.h"
#include <iostream>
#include <sstream>
#include "solution.h"
#include "losses.h"
#include "TRandom.h"
#include "pid.h"
#include "parameters.h"
#include "calibrate.h"
#include "constants.h"


struct pieCsiMap
{
  int N;
  int NcsiThere;
  int csi[6];
  bool there[6];
};

struct s800_results
{
  bool trig_coin;
  bool trig_singles;
  bool trig_s800_singles;
  int Zbeam;
  int Abeam;
  int Zresidue;
  int Aresidue;
  void Reset()
  {
    trig_coin = false;
    trig_singles = false;
    trig_s800_singles = false;
    Zbeam =-1;
    Abeam=-1;
    Zresidue=-1;
    Aresidue=-1;
  }
};

class ringCounter
{
 public:
  ringCounter(TRandom* ran, histo_sort *Histo0, float dist2Si);
  ~ringCounter();

  static bool const relativity;

  int multProton;
  int multAlpha;
  int multiHit();
  int NestDim;

  void match();
  int NestArray[50];
  int arrayD[50];
  int arrayB[50];
  float deMin;
  int dstripMin;
  
  void loop(int);
  elist Pie;
  elist Ring;
  elist Csi;

  void SetDistance(float);
  void SetTargetThickness(float);
//  void analysis();
  int analysis(s800_results); // SG 2020/10/27 

  void reset();
  int matchWithCsi();
  void csical(int icsi1, int i2);

  int Nsolution;
  solution Solution[21];
  pid * Pid[20];

  
  int csiTimeMin[20];
  int csiTimeMax[20];

  int protonYield_s800[5];
  int protonYield_Ar31[5];
  int protonYield_Ar31_S28[5];
  int protonYield_S29[5];
  int protonYield_S29_P27[5];
  int protonYield_S29_Si26[5];
  int protonYield_P28[5];
  int protonYield_Si27[5];

  int alphaYield_s800;
  int alphaYield_Ar31;
  int alphaYield_Ar31_S28;
  
  bool proton_present;

  CLosses *losses;
  CLosses *losses_Al;
  double getMass(int,int);

  float TargetThickness;

  float velocity_before;

  int K35multipiecounter=0;

  
 private:
  TRandom *ran;
  histo_sort * Histo;
  float pie_energy;
  float ring_energy;
  float distance;
  int counter;

  pieCsiMap PieCsiMap[Npie];
  void getMomentum();

  calibrate * calProton;
  calibrate * calDeuteron;
  calibrate * calAlpha;
};


#endif
