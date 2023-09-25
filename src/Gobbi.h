#ifndef Gobbi_H
#define Gobbi_H
#include <iostream>
#include <string>
#include "HINP.h"
#include "histo.h"
#include "silicon.h"
#include "correl2.h"
#include "calibrate.h"


//Nicolas Dronchi 2022_04_04
//Class written to handle all specifics of the Gobbi array
//such as communicating with HINP, calibrations, Checking for charge sharing in neighbor
//calculating geometry

class Gobbi
{
 public:
  double Targetdist;
  float TargetThickness;

  Gobbi(histo * Histo1);
  ~Gobbi();

  bool unpack(unsigned short *point);
  //void ReadCalibration(string filename);
  int match();

  float getEnergy(int board, int chan, int Ehigh);

  double ** slopes;
  double ** intercepts;


  histo * Histo;
  HINP * ADC;
  calibrate * FrontEcal;
  calibrate * BackEcal;
  calibrate * DeltaEcal;
  calibrate * FrontTimecal;
  calibrate * BackTimecal;
  calibrate * DeltaTimecal;

  silicon *Silicon[4];
  correl2 Correl;

  int counter = 0;
  int counter2 = 0;

  void corr_4He();
  void corr_5He();
  void corr_6He();
  void corr_5Li();
  void corr_6Li();
  void corr_7Li();
  void corr_6Be();
  void corr_7Be();
  void corr_8Be();
  void corr_9B();

  correl2 Correl_saved;
  int passnum = 0;
  solution * swapfrag;
  solution * oldfrag;
};


#endif
