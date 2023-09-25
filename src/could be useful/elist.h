#ifndef elist_
#define elist_
#include <string>

using namespace std;

struct order
{
  float energy;
  float energyR;  // high gain channels
  float energyRlow;  //low channels
  float energylow;
  float energyMax;
  float energyCsI;
  int strip;
  int overflow;
  int neighbours; //I smell australian here
  float time;
};

int const nnn=60;

/**
 * !\brief Energy ordered list
 *
 * This class creates an energy ordered list of the strips
 * read out from a strip detector, keeping track of the strip 
 * numbers that fired.
 */

class elist
{
 public:
  int Nstore = 0; //number stored in list
  order Order[nnn];
  int mult;
  void Add(int,int,float,float);
  void Add(int,int,float,float,float);
  void Add(int,float,float,float,float);
  void Add(int,float,int,int,float);
  void Add(int, float, int, int);
  void Remove(int);
  int  Reduce(char*);
  void reset();
  void Neighbours();
  void NeighboursPies();
  void Threshold(float);
  float threshold0;
};
#endif
