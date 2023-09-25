#ifndef calibrate_
#define calibrate_
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

/**
 * storge of calibration coefficients
 */
struct coeff
{
  float slope; //!< slope for calibration
  float intercept; //!< intercept for calibration
  float a2;  //!< quadratic coeff if needed
  float a3; //!< cubic coeff if needed
};

/**
 *!\brief all things for calibrations
 *
 * class to store calibration coefficients and return calibrated
 * data
 */







class calibrate
{
 public:
  calibrate(int Ntele,int Nstrip,string file,int order =1);
  ~calibrate();
  float getEnergy(int itele,int istrip,float channel);
  float reverseCal(int itele, int istrip, float energy);
  int order;
  int Nstrip;  //!< number of strips
  int Ntele;   //!<number of telescopes
  coeff ** Coeff;  //!< array with calibration coefficients for each strip

};
#endif
