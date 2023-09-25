#include "calibrate.h"
#include "math.h"

/**
 * Constructor
  \param Nstrip0 is number of strips or csi in a telescope
  \param name is string contain the file of coefficients
  \param order is order of polynomial

 */
calibrate::calibrate(int Ntele0,int Nstrip0,string name,int order0)
{
  Nstrip = Nstrip0;
  Ntele = Ntele0;
  order = order0;
  //cout << Nstrip << " " << name << endl;
  Coeff = new coeff*[Ntele];
  for (int i=0;i<Ntele;i++)
  {
    Coeff[i] = new coeff [Nstrip];
  }

  ifstream file(name.c_str());
  string title;
  //  getline(file,title);
  // cout << title << endl;
  //getline(file,title);

  int itele,istrip;
  float slope, intercept, a2,a3;
  for(;;)
  {
    file >>  itele >> istrip >> slope >> intercept;
    if (order >=2) file >> a2;
    else a2 = 0.;
    if (order == 3) file >> a3;
    else a3 = 0.;
    if (file.eof()) break;
    if (file.bad()) break;

    Coeff[itele][istrip].slope = slope;
    Coeff[itele][istrip].intercept = intercept;
    Coeff[itele][istrip].a2 = a2;
    Coeff[itele][istrip].a3 = a3;
  }
  file.close();
  file.clear();  

}
//*****************************************************
  /**
   * destructor
   */
calibrate::~calibrate()
{
  for (int i=0;i<Ntele;i++)
  {
    delete [] Coeff[i];
  }

  delete [] Coeff;
}
//*****************************************
  /**
   * returns the calibrated energy
\param istrip - number of the strip or detector
\param channel - raw channels from the ADC, etc
  */
float calibrate::getEnergy(int itele,int istrip,float channel)
{
  float fact = channel*Coeff[itele][istrip].slope + Coeff[itele][istrip].intercept;
  if (order == 1) return fact;
  fact += pow(channel,2)*Coeff[itele][istrip].a2;
  if (order == 2) return fact;
  if (order == 3)return pow(channel,3)*Coeff[itele][istrip].a3 + fact;
  else abort();
}


float calibrate::reverseCal(int itele, int istrip, float energy)
{
  float fact = (energy - Coeff[itele][istrip].intercept)/Coeff[itele][istrip].slope;
  return fact;
}
