#include "parType.h"

parType::parType(int Z0, int A0)
{
  init(Z0,A0);
}
//*****************************
void parType::zeroMask()
{
  for (int i=0;i<6;i++) mask[i] = false;
}
//*****************************
void parType::setMask()
{
  for (int i=0;i<6;i++) mask[i] = true;
}
//**************************************
void parType::init(int Z0, int A0)
{
  Z = Z0;
  A = A0;
}
