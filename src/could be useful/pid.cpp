#include "pid.h"

/**
 * Constructor reads in banana gates
 */
pid::pid(string file)
{
  string name;
  //name = "zlineBackground/"+file+".zline";
  name = "zline/"+file +".zline";
  //name = "zlineC12_cal/"+file +".zline";
  //cout << name << endl;
  ifstream ifile(name.c_str());
  if (!ifile.is_open()) 
  {
    cout << "could not open file " << name << endl;
  }
  ifile >> nlines;
  par = new ZApar*[nlines];
  for (int i=0;i<nlines;i++)
  {
    par[i] = new ZApar(ifile);
  }
  ifile.close();
  ifile.clear();
  Z=-1;
  A=-1;
}
//************************
  /**
   * destructor
   */
pid::~pid()
{
  for (int i=0;i<nlines;i++) delete par[i];
  delete [] par;
}
/**
 * returns true if particle is in a banana gate.
 * returns false otherwise. The parameters Z and A
 * are loaded with the detected particle's values.
\param x energy of particle
\param y energy loss of particle
 */
//*********************************
bool pid::getPID(float x, float y)
{
  Z = 0;
  A = 0;
  for (int i=0;i<nlines;i++)
  {
    if (par[i]->inBanana(x,y))
    {
      Z = par[i]->Z;
      A = par[i]->A;
      mass = par[i]->mass;
      return true;
    }
  }
  return false;
}



