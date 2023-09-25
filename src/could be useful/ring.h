#ifndef ringC_
#define ringC_

#include "histo_sort.h"
#include "elist.h"
#include "pid.h"
#include <iostream>
#include "solution.h"
#include "loss.h"
#include "TRandom.h"


class ringC
{
 public:
  ringC(TRandom* ran, histo_sort *Histo0);
  ~ringC();

  int multiHit();
  int NestDim;

  int NestArray[50];
  int arrayD[50];
  int arrayB[50];
  float deMin;
  int dstripMin;
  
  void loop(int);
  elist Pie;
  elist Strip;
  elist Csi;

  void analysis();
  void reset();

  int Nsolution;
  solution Solution[4];
  
 private:
  TRandom *ran;
  histo_sort * Histo;
  float benergy;
  float fenergy;
};


#endif
