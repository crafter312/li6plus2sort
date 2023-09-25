#ifndef loss2_
#define loss2_
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

/**
 * !\brief energy loss of particles in an absorber
 */

class CLoss2
{
 public:
  int N;
  float *Ein;
  float *dedx;
  
  CLoss2(string);
  ~CLoss2();
  
  float getEout(float,float,int);
  float getEin(float,float,int);
  float getDedx(float,int);



};
#endif
