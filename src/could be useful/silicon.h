#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include "TRandom.h"
#include "histo.h"
#include "calibrate.h"
#include "telescope.h"
#include "caen.h"
#include "TDC1190.h"
#include "pixels.h"


using namespace std;


struct dataE
{
  int ienergy;
  int id;
  int itime;
  float energy;
};

struct dataT
{
  int itime;
  int id;
};

struct map
{
  bool front;
  bool A;
  int itele;
};

class silicon
{
 public:
  silicon(TRandom* ran, histo * Histo0);
  ~silicon();
  bool unpack(unsigned short*& point,int runno);
  bool unpackSi_sis(unsigned short*& point);
  bool unpackSi_adc(unsigned short*& point);
  bool unpackSi_HINP4(unsigned short*& point);
  bool unpackCsi(unsigned short*& point,int runno);
  void analyze();
  telescope **Telescope;
  void reset();

  double Blocker_e;

  int Np;
  int N6;
  int Ncross;

  calibrate * calRPie;
  calibrate * calRRing;
  calibrate * calSPie;
  calibrate * calSRing;
  calibrate * calCsI;

  int eventNum;


 private:
  TRandom * ran;
  unsigned short xmarker[3];


  unsigned short chanXLM[3][400];
  unsigned short nXLM[3];

  map Map[3][13];
  
  histo * Histo;
  caen ADC;
  TDC1190 *tdc;

  dataE DataE[56];
  dataT DataT[56];
 



  int NE;
  int NT;
  int CsIM;


  //high-low correlation
  double fsumN[2][32];
  double fsumx[2][32];
  double fsumxx[2][32];
  double fsumy[2][32];
  double fsumyx[2][32];

  double bsumN[2][32];
  double bsumx[2][32];
  double bsumxx[2][32];
  double bsumy[2][32];
  double bsumyx[2][32];


  bool fred;

};
