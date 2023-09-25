#include "loss2.h"
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * constructor
\param filename is name of file containing energy loss tables of a particulat particle
*/

CLoss2::CLoss2(string filename)
{
  //cout << filename.c_str() << endl;
  ifstream File(filename.c_str());
  if (File.is_open() != 1)
  {
    cout << " could not open loss file " << filename;
    return;
  }

  char line[80];
  File.getline(line,80);
  //cout << line << endl;

  File >> N;
  Ein = new float [N];
  dedx = new float [N];

  for (int i=0;i<N;i++) 
  {
    File >> Ein[i] >> dedx[i];
    //cout << Ein[i] << " " << dedx[i] << endl;
  }
}

//****************************************************************
  /**
   * destructor
   */
CLoss2::~CLoss2()
{
  delete [] Ein;
  delete [] dedx;
}
//*****************************************************************
  /*
   * returns the value of DeDx interpolated from table
   \param energy is energy of particle in MeV
   */
float CLoss2::getDedx(float energy, int A)
{
  // linear interpolation
  int istart = 0;

  float epa = energy/(float)A;
  if (epa < Ein[0]) istart = 0;
  else if (epa > Ein[N-1]) istart =  N-1;
  else
  {
    istart = 1;
    for (;;)
    {
      if (epa < Ein[istart]) break;
      istart++;
    }
    istart--;
  }

  float de = (epa-Ein[istart])/(Ein[istart+1]-Ein[istart])
    *(dedx[istart+1]-dedx[istart]) + dedx[istart];

  return de;
}
//********************************************************************
  /**
   * returns the residual energy of particle after passage through absorber
\param energy is initial energy of particle in MeV
\param thick is the thickness of the absorber in mg/cm2
  */
float CLoss2::getEout(float energy, float thick,int A)
{
  float dthick = 1.;
  float de;
  float Eout= energy;
  for(;;)
  {
    float thickness = min(thick,dthick);
    de = getDedx(Eout,A);
    Eout -= de*thickness;
    if (thickness == thick) break;
    thick -= dthick;
  }
   return Eout;
}
//********************************************************************
  /**
   * Determined the inital energy of a particle before entering an 
   * absorber given its residueal
\param energy is the residual energy of the particle
\param thick is the thickness of absorber through which the particle passed.
  */
float CLoss2::getEin(float energy, float thick,int A)
{
  float dthick = 1.;
  float de;
  float Einput= energy;
  for(;;)
  {
    float thickness = min(thick,dthick);
    de = getDedx(Einput,A);
    Einput += de*thickness;
    if (thickness == thick) break;
    thick -= dthick;
  }
  return Einput;
}


