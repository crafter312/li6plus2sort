#include "HINP.h"
#include <iostream>
#include <iomanip>
using namespace std;

//unpacking the XLM with ADC on the CHIP BOARDS (HINP 4)

//constructor sets multiplicities to 0
HINP::HINP()
{
  multEfront = 0;
  multEback = 0;
  multdE = 0;
}


/* Example in the buffer
13 1ff3 10 0 8 0 0 0 0 28 412 ee 38b1 6b 30f d5 3815 0 a2 d

13          words=19 after this
1ff3        marker, unique to the motherboard
10 0        Nwords=16 (don't know the use of this)
8         NstripsRead, must be divisible by 4
0 0 0 0       *point+5, some junk that needs to be skipped, could be time stamp
28 412 ee 38b1      id,Ehigh,Elow,time
6b 30f d5 3815      id,Ehigh,Elow,time

0 a2 d    just some junk included in the buffer?
*/

bool HINP::unpackSi_HINP4(unsigned short *&point)
{
  //reset multiplicities at the start
  multEfront = 0;
  multEback = 0;
  multdE = 0;

  unsigned short *endpos = point;
  unsigned short words = *point++;
  endpos += words;

  unsigned short marker;

  marker = *point++;
  
  if (marker != 0x1ff0)
  { 
    cout << "Did not read the proper XLM marker. Was " << hex << marker << " expected 0x1ff3" << dec <<endl;
    return false;
  }
  
  NstripsRead = 0;
  NWords = *point;
  //unsigned short * endHINP = point;

  if (NWords > 2048)
  {
    point+=10;
    return false;
    //return true; // KB - 10/26/20
  }

  point += 2;

  NstripsRead = *point;

  if(NstripsRead %4 !=0)
  {
    cout << "NstripsRead %4 !=0" << endl;
    point+=8;
    return false;
  }

  NstripsRead /= 4;

  if (NstripsRead > 512) 
  {
    cout << "NstripsRead %4 !=0" << endl;
    point +=8;
    return false; // bad buffer
    //return true; //need to return to this KB - 10/26/20
  }

  if (NstripsRead > maxlen)
  {
    cout << "please increase the value of maxlen" << endl;
    cout << "must be at least " << NstripsRead << endl;
    return false;
  }

  point += 5;
    
  //cout << "NStrips " << NstripsRead <<  endl;
  for (int istrip = 0;istrip < NstripsRead;istrip++)
  {
    unsigned short id = *point++;
    unsigned short chipNum = (id&0x1FE0)>>5;
    unsigned short chanNum = id & 0x1F;
    unsigned short ienergy = *point++;
    unsigned short ilowenergy = *point++;
    unsigned short itime =  *point++;
    unsigned short boardNum;

    //cout << "id " << id << endl;
    cout << "chipNum " <<chipNum << endl;
    cout << "chanNum " << chanNum << endl;
    //cout << "ienergy " << ienergy << endl;
    //cout << "ilowenergy " << ilowenergy << endl;
    //cout << "itime " << itime << endl;


    //each board has 2 chips, convert so that it is per Board
    if (chipNum%2 == 0)
    { //for even chipnum (second half of chip)
      chanNum = 2*chanNum;//nick
      //chanNum = 16+chanNum;
      //chipNum /= 2;
      boardNum = chipNum/2;
    }
    else
    { //for odd chipnum (first half of chip)
      chanNum = 2*chanNum+1; //nick
      //chanNum = chanNum;
      boardNum = chipNum/2 + 1;
    }
    cout << "  boardNum " << boardNum << endl;
    cout << "  chanNum " << chanNum << endl;
    cout << " " << endl;

    
    //save information to arrays
    board[istrip] = boardNum;
    chan[istrip] = chanNum;
    high[istrip] = ienergy;
    low[istrip] = ilowenergy;
    time[istrip] = itime;

  } //end loop over strips
    
  //point = endHINP;

  return true;
}

