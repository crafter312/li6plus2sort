#include "pid.h"

/**
 * Constructor reads in banana gates
 */
pid::pid(string file)
{
  string name;

  name = "zline/"+file +".zline";
  cout << "opening zline file: " << name << endl;

  ifstream ifile(name.c_str());
  if (!ifile.is_open()) 
  {
    cout << "could not open zline file " << name << endl;
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
      mass = getMass(Z,A);
      return true;
    }
  }
  return false;
}

float pid::getMass(int iZ,int iA)
{
  //All masses are hard coded in. If adding new isotope, just copy/paste
  if (iZ == 1)
  {
    if (iA == 1)  return 1.+ 7.2889/m0;
    else if (iA == 2) return 2. + 13.135/m0;
    else if (iA == 3) return 3.+ 14.949/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 2)
  {
    if (iA == 3) return 3. + 14.931/m0;
    else if (iA == 4) return 4. + 2.424/m0;
    else if (iA == 6) return 6. + 17.592/m0;
    else if (iA == 8) return 8. + 31.609/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 3)
  {
    if (iA == 6) return 6.+14.086/m0;
    else if(iA == 7) return 7. + 14.907/m0;
    else if (iA == 8) return 8. + 20.945/m0;
    else if (iA == 9) return 9. + 24.954/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 4)
  {
    if (iA == 7) return 7. + 15.768/m0;
    else if (iA == 8) return 8. + 4.941/m0;
    else if (iA == 9) return 9. + 11.348/m0;
    else if (iA == 10) return 10. + 12.607/m0;
    else if (iA == 11) return 11. + 20.177/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ==5)
  {
    if (iA == 8) return 8. + 22.921/m0;
    else if (iA == 10) return 10. + 12.050/m0;
    else if (iA == 11) return 11. + 8.667/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 6)
  {
    if (iA == 9) return 9. + 28.910/m0;
    else if (iA == 10) return 10. + 15.698/m0;
    else if (iA == 11) return 11. + 10.649/m0;
    else if (iA == 12) return 12.;
    else if (iA == 13) return 13. + 3.125/m0;
    else if (iA == 14) return 14. + 3.019/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else
  {
    cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
    abort();
    return 0;
  }
}

