#include "ZApar.h"

/**
 * constructor reads in a banana gate
\param ifile is an ifstream object of an open file containing gate
*/
ZApar::ZApar(std::ifstream& ifile)
{
  ifile >> Z >> A;
  mass = (float)A;
  ifile >> n;  // number of points
  x = new float [n];
  y = new float [n];
  //cout << Z << " " << A << endl;
  for (int i=0;i<n;i++)
  {
    ifile >> x[i] >> y[i];
    //cout<<i << " "  << x[i] << " " << y[i] << endl;
  }
}
//*****************************************
  /**
   * destructor
   */
 ZApar::~ZApar()
{
  delete [] x;
  delete [] y;
}
//*******************************************
  /**
   * returns true if particle is in banana gate
\param xx is energy of particle
\param yy is energy loss of particle
  */
bool ZApar::inBanana(float xx, float yy)
{
  if (TMath::IsInside(xx,yy,n,x,y)) return true;
  else return false; 
}
