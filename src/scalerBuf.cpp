/*   class to read scaler buffers and make totals */

#include "scalerBuf.h"
scalerBuffer::scalerBuffer(int N)
{
  // N is the number of scalers it will read and total
  Nchannels = N;
  Nbuffers = 0;
  //arrays for totals and titles for each channel
  totals = new unsigned long long int[N];
  titles = new string[N];

  // read titles from file
  ifstream file("scalerNames.dat");
  if (!file) return;
 
 //read in titles for each scaler
  for (int i=0;i<Nchannels;i++)
  {
    getline(file,titles[i]);
    cout << "scalar " << i << " is " << titles[i] << endl;
    totals[i] = 0;
    if (file.bad()) break;
  }

}
//*************************************************
scalerBuffer::~scalerBuffer()
{
  delete  [] totals;
  delete  [] titles;
}
//****************************************
/*   read and add to totals for a single scaler buffer */
void  scalerBuffer::increment(unsigned short * point)
{
  Nbuffers++;
  point += 8;

  for(int i=0;i<Nchannels;i++)
  {
    unsigned int data1 = *point++;
    //unsigned int data1_old = data1;
    unsigned int data2 = *point++;
    data1 = data1 + (data2<<16);
    //if (data2 != 0) cout << i << data1 << " " << data1_old << " " << data2 << endl;
    totals[i] += data1;
  }
}
//***************************************************
/* print out scaler totals */
void scalerBuffer::print()
{
   cout << Nbuffers<< " Scaler buffers " << endl;
   for (int i=0;i<Nchannels;i++) cout << " " << i << " " << titles[i] << " " << totals[i] << endl;
   cout << endl;

   if (totals[0] > 0)
      {
         double liveTime = double(totals[4])/double(totals[0]);

	 cout << "line fraction = " << liveTime << " ,dead fraction = " << 1.-liveTime << endl;
      }
}
