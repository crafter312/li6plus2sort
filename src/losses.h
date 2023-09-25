#ifndef losses_
#define losses_
#include "loss2.h"

class CLosses
{
 private:
   CLoss2 ** loss;
   int Zmax;
 public:
   CLosses(int,string);
   ~CLosses();
   float getEin(float,float,int,float);
   float getEout(float,float,int,float);

};

#endif
