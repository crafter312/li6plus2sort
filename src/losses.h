#ifndef losses_
#define losses_
#include "loss2.h"
#include "SortConfig.h"

class CLosses
{
 private:
   CLoss2 ** loss;
   int Zmax;
 public:
   CLosses(int,SortConfig&);
   ~CLosses();
   float getEin(float,float,int,float);
   float getEout(float,float,int,float);

};

#endif
