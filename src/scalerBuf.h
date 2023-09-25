#include <iostream>
#include <string>
#include <fstream>
using namespace std;


class scalerBuffer
{
   public:
   scalerBuffer(int);
   ~scalerBuffer();
   void increment(unsigned short * point);
   void print();
   unsigned long long int * totals;
   string * titles;
   int Nchannels;
   int Nbuffers;
};
