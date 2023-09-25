#include "solution.h"

class parType
{
 public:
  int Z,A;
  parType(){};
  parType(int Z, int A);
  void zeroMask();
  void setMask();
  void init(int Z, int A);

  solution *Sol[6];
  int mult;
  bool mask[6];
};
