#ifndef det_
#define det_
#include <iostream>
#include <string>
#include "histo.h"
#include "Gobbi.h"
using namespace std;

/**
 *!\brief detector readout
 *
 * responsible for unpacking the data stream for physics events
 */


class det
{
 public:
  det(histo * Histo);
  ~det();
  
  int shortpackets;
  int wordcounterror;
  
  bool unpack(unsigned short *point);
  void analyze(int event, int run);

  histo * Histo;
  Gobbi * gobbi;
};
#endif
