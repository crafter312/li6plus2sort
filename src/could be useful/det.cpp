#include "det.h"
#include <fstream>
#include <iostream>
#include <bitset>


//constructor
det::det(histo * Histo1)
{
  Histo = Histo1;
  gobbi = new Gobbi(Histo1);
}

det::~det()
{
  delete gobbi;
}

//*************************************************************
/**
* unpacks a physics event from the data stream
\param point0 - pointer to location in data stream

*/
bool det::unpack(unsigned short *point)
{
  bool goodGobbi;
  goodGobbi = gobbi->unpack(point);  //suck out the info in the qdc

  if (!goodGobbi) {cout << "some error somewhere!, investigate" << endl;}

  //TODO plot E-dE events
  //TODO plot other useful things like xy-distribution, theta distribution, ...

  //TODO send E-dE info to PID and return what we got, 6He, p, 6Li?  

  //TODO put info correl file

  //TODO account for energy loss in target

  

  
  return true;
}
