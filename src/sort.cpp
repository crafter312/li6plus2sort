//file to unpack data from the neutron detector, ND 2020

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include "Gobbi.h"
#include "histo.h"
#include "scalerBuf.h"

using namespace std;

int main()
{
  clock_t t;
  t = clock();

  int NphysicsEvent = 0;
  int NscalerBuffer = 0;
  int Nscalercounter = 0;
  int Npauses = 0;
  int Nresumes = 0;
  int runno = 0;
  int runnum = 0;
  unsigned short *point;

  string directory = "/data2/li7_may2022/";
                     // /data2/li7_may2022/run1033/run-1033-00.evt
  ostringstream outstring;

  //build three classes that are used to organize and unpack each physicsevent
  histo * Histo = new histo();
  Gobbi gobbi(Histo);
  scalerBuffer ScalerBuffer(14);
  

  ifstream runFile;
  ifstream evtfile;
  runFile.open("numbers.beam");

  for (;;)
  {
    runFile >> runnum;
    if (runFile.eof()) break;
    if (runFile.bad()) break;
    if (evtfile.is_open()) 
      cout << "problem previous file not closed" << endl;
  
    for (int iExtra=0;iExtra<3;iExtra++) //loop over split evtfiles
    {
      outstring.str("");
      //get the correct file name with potential of extra split evtfiles
      outstring << directory << "run" << runnum << "/run-" << setfill('0') << setw(4) << runnum;
      outstring << "-" << setfill('0') << setw(2) << iExtra << ".evt";

      evtfile.clear();
      evtfile.open(outstring.str().c_str(), ios::binary);

      //move past events that don't have split files
      if (iExtra>0 && !evtfile){break;}

      if (!evtfile.is_open())
      {
        cout << "could not open event file" << endl;
        cout << outstring.str() << endl;
        abort();
        return 1;
      }

      if(evtfile.bad())
      {
        cout << " bad found" << endl;
        break;
      }

      cout << "reading file: " << outstring.str() << endl;

      for(;;)
      //for(int cnt=0; cnt < 10000; cnt++)  // loop over items in a evtfile
      {
        //cout << "cnt " << cnt << endl;
        int const hBufferWords = 4;
        int const hBufferBytes = hBufferWords*2;
        unsigned short hBuffer[hBufferWords];
        
        //read a section 8 bytes long and split into 2 byte chunks 
        //(each 2byte chunk is little endian)
        evtfile.read((char*)hBuffer,hBufferBytes);
        //example 0x0032    0x0000    0x001e    0x0000
        //        nbytes    nbytes2   type      type2
          
        if(evtfile.eof())
        {
          cout << "eof found" << endl;
          break;
        }
   
        point = hBuffer;
        int nbytes = *point++;
        int nbytes2 = *point++;
        int type = *point++;
        int type2 = *point;

        //if there is an issue reading ring buffer, look at these
        //cout << hex << nbytes << " " << nbytes2 << " " << type << " " << type2 << endl;
        //if done properly, nbytes and nbytes2 should be combined but 
        //values are never large enough
        
        int dBufferBytes = nbytes - 8;
        // if nbytes = 0032 in hex, then =50-8=42
        
        int dBufferWords = dBufferBytes/2;
        // 21 2byte pairs
      
        unsigned short dBuffer[dBufferWords];
        evtfile.read((char*)dBuffer,dBufferBytes);
        point = dBuffer;

        unsigned short *pos = point;

        //debug code to look at contents of each ring buffer
        //cout << "dbufferwords " << dBufferWords << endl;
        //for (int i=0; i<dBufferWords; i++)
        //{
        //  cout << hex<< *pos++ << " ";
        //}
        //cout << dec << endl;
        
        
        //unknown of what causes these large readouts. We only want events 
        //that come in coincidence 2, then get read out imediately
        //if(nbytes > 100 && type != 2 && physicsEvent > 1)
        //{
        //  cout << "occurs at " << physicsEvent << endl;
        //  cout << "nbytes " << nbytes << " " << nbytes2 << " " << type << " " << type2 << endl;
        //  continue;
        //}


        if (type == 30) //type 30 gives physics information
        {
          NphysicsEvent++;
          //eventually unpack the point
          bool stat = gobbi.unpack(point);
          //if (!stat) break;
        }
        else if (type == 20)
        {
          ScalerBuffer.increment(point);
          NscalerBuffer++;
        }
        else if (type == 31)
        {
          Nscalercounter++;
          //cout << "I don't know what this is yet. please help me." << endl;
        }
        else if (type == 1)
        {
          //header length of 104 indicated by first part of buffer 0x0068
          runno = *point; //The run number is stored in the first part of the buffer header
          cout << "run number = " << runno << ", should match " << runnum << endl;    
        }
        else if (type == 2)
        {
          cout << "got type == 2, flag for end of run" << endl;
          break;
        }
        else if (type == 3)
        {
          Npauses++;
        }
        else if (type == 4)
        {
          Nresumes++;
        }
        else
        {
          cout << " unknown event type " << type << " found" << endl;
          break;
        }
      }//loop over items in a evtfile

      evtfile.close();
      evtfile.clear(); //clear event status in case we had a bad file

    }//loop over split event file
  }//loop over number.beam values

  cout << "Front Nstore " << gobbi.counter2 << endl;
  cout << "Front neighbours " << gobbi.counter << endl;
  cout << "physics Event Counters = " << NphysicsEvent << endl;
  cout << "scaler buffers = " << NscalerBuffer << endl;
  cout << "confirm number of scalers = " << Nscalercounter << endl;
  cout << "Numbers of pauses = " << Npauses << endl;
  cout << "Number of resumes = " << Nresumes << endl;

  ScalerBuffer.print();

  t = clock() - t;
  cout << "run time: " << (float)t/CLOCKS_PER_SEC/60 << " min" << endl;

  delete Histo;
  return 0;
}


