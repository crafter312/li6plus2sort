#ifndef _ZApar
#define _ZApar
#include "TMath.h"
#include <fstream>
#include <iostream>
using namespace std;

/**
 *!\brief stores a banana gate
 */

class ZApar
{
 public:
  int Z;  //!< proton number of particle
  float mass; //!< mass of particle in AMU
  int A; //!< mass number of particle

  ZApar(std::ifstream & ifile);
  ZApar(){};
  ~ZApar();
  bool inBanana(float x, float y);

  int n; //!<number of points
  float *x; //!<pointer to x array
  float *y; //!<pointer to y array
};
#endif
