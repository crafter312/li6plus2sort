
//07/22/23 -ND
//This is actually just one big comment for the multihit subroutine that matches up events.
//Here I go through an example of two events that get matched up.

//recursive subroutine  used for multihit subroutine
void silicon::loop(int depth)
{
  cout << "current depth=" << depth << "  NestDim"<< endl;
  cout << "NestArray: ";
  for (int i=0;i<NestDim;i++) { cout << NestArray[i] << " ";}
  cout << endl;

  if (depth == NestDim) //depth starts at 0
  {
    int dstrip = 0;
    float de = 0.;
    for (int i=0;i<NestDim;i++)
    {
      dstrip += abs(Delta.Order[NestArray[i]].strip
            -Front.Order[i].strip);
      de += abs(Back.Order[NestArray[i]].energy
            -Front.Order[i].energy);
    }

    cout << " dstrip < dstripMin : " << dstrip << " < " << dstripMin << endl;

    if (dstrip < dstripMin)
    {
      dstripMin = dstrip;
      for (int i=0;i<NestDim;i++) {arrayD[i] = NestArray[i];}
    }
    cout << " de < deMin : " << de << " < " << deMin << endl;

    if (de < deMin)
    {
      deMin = de;
      for (int i=0;i<NestDim;i++) {arrayB[i] = NestArray[i];}
    }
    cout << "leave" << endl;
    return;
  }

  for (int i=0;i<NestDim;i++)
  {
    NestArray[depth] = i;
    int leave = 0;
    for (int j=0;j<depth;j++)
    {
      if (NestArray[j] == i)
      {
        leave =1;
        break; 
      }
    }
    if (leave) continue;
    loop(depth+1);
  }
}

//***************************************************
//extracts multiple particle from strip data 
int silicon::multiHit()
{

  cout << "start of multiHit()" << endl;

  int Ntries = min(Front.Nstore,Back.Nstore);
  Ntries = min(Ntries,Delta.Nstore);

  if (Ntries > 4) Ntries =4;
  Nsolution = 0;
  if (Ntries <= 0) return 0;

  cout << "going to have Ntries=" << Ntries << endl;
  
  for (NestDim = Ntries;NestDim>0;NestDim--)
  {
    dstripMin = 1000;
    deMin = 10000.;
    
    //look for best solution
    loop(0);
    
    //check to see if best possible solution is reasonable
    int leave = 0;
    for (int i=0;i<NestDim;i++)
    {
      if (abs(Delta.Order[arrayD[i]].strip - Front.Order[i].strip) > 2) 
      {
        leave = 1;
        break;
      }
      if (fabs(Back.Order[arrayB[i]].energy - Front.Order[i].energy) > 2.) 
      {
        leave = 1;
        break;
      }
      float timediff = Front.Order[i].time - Delta.Order[arrayD[i]].time;
      //if ( timediff < -500. || timediff > 100) 
      //{
      //  Nsolution = 0;
      //  return 0;
      //}
    }
      
    if (leave) continue;
    // now load solution
    for (int i=0;i<NestDim;i++)
    {
      float timediff = Front.Order[i].time - Delta.Order[arrayD[i]].time;
      Solution[i].energy = Front.Order[i].energy;
      Solution[i].energyR = Front.Order[i].energyR;
      Solution[i].denergy = Delta.Order[arrayD[i]].energy;
      Solution[i].ifront = Front.Order[i].strip;
      Solution[i].iback = Back.Order[arrayB[i]].strip;
      Solution[i].ide = Delta.Order[arrayD[i]].strip;
      Solution[i].itele = id;
      Solution[i].timediff = timediff;
      //Solution[i].Nbefore = Front.Order[i].Nbefore;
      //Solution[i].Norder = Front.Order[i].Norder;
    }

    Nsolution = NestDim;
    
    break;
  }

  if (Nsolution == 2) { abort(); }

  return Nsolution;
}





////////////////////////////////////////////////////////////////////////////////////////////

start with the data
Front ordered: 
 #0  strip13  energy 5.61734
 #1  strip15  energy 3.29919
Back ordered: 
 #0  strip17  energy 5.87104
 #1  strip23  energy 3.60316
Delta ordered: 
 #0  strip15  energy 2.14099
 #1  strip12  energy 0.913967

end with the solutions:
solution0
energy 5.61734  ifront 13  iback 17  dE 0.913967  ide 12
solution1
energy 3.29919  ifront 15  iback 23  dE 2.14099  ide 15

////////////////////////////////////////////////////////////////////////////////////////////

/*
start of multiHit()
start with the data
Front ordered: 
 #0  strip13  energy 5.61734
 #1  strip15  energy 3.29919
Back ordered: 
 #0  strip17  energy 5.87104
 #1  strip23  energy 3.60316
Delta ordered: 
 #0  strip15  energy 2.14099
 #1  strip12  energy 0.913967
*/

  int Ntries = min(Front.Nstore,Back.Nstore);
  Ntries = min(Ntries,Delta.Nstore);

  if (Ntries > 4) Ntries =4;
  Nsolution = 0;
  if (Ntries <= 0) return 0;

//going to have Ntries=2

  for (NestDim = Ntries;NestDim>0;NestDim--)
  {
    dstripMin = 1000;
    deMin = 10000.;
    
    //look for best solution
    loop(0);

//current depth=0  NestDim=2
//NestArray={0, 0}
  if (depth == NestDim) //depth starts at 0
  { 
    //we skip most all of the early part
  }
  for (int i=0;i<NestDim;i++)
  {
    NestArray[depth] = i; //depth=0, so NestArray[depth] = 0, and 
//NestArray={0, 0}
    int leave = 0;
    for (int j=0;j<depth;j++) 
    {
      //depth =0 so ignore this loop
    }

//call loop(0+1)
    loop(depth+1);

//current depth=1  NestDim=2
//NestArray={0, 0}
  if (depth == NestDim) //depth now at 1
  { 
    //we skip this
  }

  for (int i=0;i<NestDim;i++) //nestDim=2
  {
    //now i=0
    NestArray[depth] = i; //set NestArray[1] = 0
//NestArray={0, 0}
    int leave = 0;
    for (int j=0;j<depth;j++) //depth=1
    {
      //j=0 <depth=1
      //we want to continue the loop over i because j=0 with NestArray={0, 0} 
      //so we know NestArray[0] == 0
      if (NestArray[j] == i) 
      {
        leave =1;
        break; 
//breaker now
      }
    }
    if (leave) continue;

//now I'm going to post parts of the loop again back at line 216

    //now i=1 of loop
    NestArray[depth] = i; //set NestArray[1] = 1
//NestArray={0, 1}
    int leave = 0;
    for (int j=0;j<depth;j++) //depth=1
    {
      //j=0 <depth=1
      //now NestArray[0] == 1 so we don't break!
      if (NestArray[j] == i) 
      {
        //this is skipped
      }
    }
    //we go deeper into the recursion
//call loop(1+1)
    loop(depth+1);



//current depth=2  NestDim=2
//NestArray: 0 1
  if (depth == NestDim) //now this part is finally true
  {
    int dstrip = 0;
    float de = 0.;
    for (int i=0;i<NestDim;i++)
    {
      dstrip += abs(Delta.Order[NestArray[i]].strip
            -Front.Order[i].strip);
      de += abs(Back.Order[NestArray[i]].energy
            -Front.Order[i].energy);
    }

//this will always be true first time through this section
//dstrip < dstripMin : 5 < 1000
    if (dstrip < dstripMin)
    {
      dstripMin = dstrip;
      for (int i=0;i<NestDim;i++) {arrayD[i] = NestArray[i];}
//now arrayD = {0, 1} which means
//   Front.Order[0] matches Delta.Order[0]
//   Front.Order[1] matches Delta.Order[1]
    }
//again will also be true first time through
//de < deMin : 0.557666 < 10000
    if (de < deMin)
    {
      deMin = de;
      for (int i=0;i<NestDim;i++) {arrayB[i] = NestArray[i];}
//now arrayB = {0, 1} which means
//   Front.Order[0] matches Back.Order[0]
//   Front.Order[1] matches Back.Order[1]
    }

//leave loop(2) back to where we were in loop(1)
    return;

//////Return to line 216 above//////
  for (int i=0;i<NestDim;i++) //nestDim=2
  {
    //here i would be 2 which is not < NestDim =2
  }

//we just finished loop(1)

//now in loop(0) section
//////Return to line 196 above//////
  for (int i=0;i<NestDim;i++) //nestDim=2
  {
    //here i=1
    NestArray[depth] = i; //depth=0, so NestArray[depth] = 1, and 
//NestArray={1, 1}
    int leave = 0;
    for (int j=0;j<depth;j++) 
    {
      //depth =0 so ignore this loop
    }

//call loop(0+1)
    loop(depth+1);

//current depth=1  NestDim=2
//NestArray={1, 1}
  if (depth == NestDim) //depth now at 1
  { 
    //we skip this
  }
  for (int i=0;i<NestDim;i++) //nestDim=2
  {
    //now i=0
    //depth = 1
    NestArray[depth] = i; //set NestArray[1] = 0
//NestArray={1, 0}
    int leave = 0;
    for (int j=0;j<depth;j++) //depth=1
    {
      //j=0 <depth=1
      //so we know NestArray[0] == 1 so we skipp this
      if (NestArray[j] == i)
      {
        //skipped
      }
    }
    if (leave) continue;

//call loop(1+1)
    loop(depth+1);

//current depth=2  NestDim=2
//NestArray: 1 0 
  if (depth == NestDim) //now this part is true again (second time this is true)
  {
    int dstrip = 0;
    float de = 0.;
    for (int i=0;i<NestDim;i++)
    {
      dstrip += abs(Delta.Order[NestArray[i]].strip
            -Front.Order[i].strip);
      de += abs(Back.Order[NestArray[i]].energy
            -Front.Order[i].energy);
    }

//dstrip < dstripMin : 1 < 5
//hey we found a better pairing for the dE-E
    if (dstrip < dstripMin)
    {
      dstripMin = dstrip;
      for (int i=0;i<NestDim;i++) {arrayD[i] = NestArray[i];}
//now arrayD = {1, 0} which means
//   Front.Order[0] matches Delta.Order[1]
//   Front.Order[1] matches Delta.Order[0]
    }

//de < deMin : 4.58603 < 0.557666
//in this case we didn't find a better match for Front/Back
    if (de < deMin)
    {
      //skipped which meanas we keep
//   Front.Order[0] matches Back.Order[0]
//   Front.Order[1] matches Back.Order[1]
    }

//leave loop(2) back to where we were in loop(1)
    return;

//after loop(2) was called, at i=0, restart loop with i=1
//////Return to line 216 above//////
  for (int i=0;i<NestDim;i++) //nestDim=2
  {
    //here i=1
//NestArray={1, 0}
    NestArray[depth] = i; //depth=1, so NestArray[depth] = 1, and 
//NestArray={1, 1}
    int leave = 0;
    for (int j=0;j<depth;j++) //depth=1
    {
      //j=0 <depth=1
      if (NestArray[j] == i) //NestArray[0] = 1, i=1 so true here
      {
        leave =1;
        break;
      }
    }
    if (leave) continue; //we continue the loop to the point i=2 so we exit the loop
  }

//we just finished loop(1) so we fall out to the part after loop(1) was called in loop(0).
//this just finished the part of loop(0) that had i=1, now we would go to i=2
  for (int i=0;i<NestDim;i++)
  {
    NestArray[depth] = i;
    int leave = 0;
    for (int j=0;j<depth;j++)
    {
      //
    }
    if (leave) continue;
    loop(depth+1);
    //this is the spot we ended up after ending loop(1) and loop(0) with i=1
//next is i=2 not less than NestDim = 2 so we finished loop(0) and exit this recursive nightmare
  }

/*
For this starting data
Front ordered: 
 #0  strip13  energy 5.61734
 #1  strip15  energy 3.29919
Back ordered: 
 #0  strip17  energy 5.87104
 #1  strip23  energy 3.60316
Delta ordered: 
 #0  strip15  energy 2.14099
 #1  strip12  energy 0.913967

we end with 
arrayD: 1 0 
arrayB: 0 1 

giving the solutions
solution0
energy5.61734  ifront13  iback17  dE0.913967  ide12
solution1
energy3.29919  ifront15  iback23  dE2.14099  ide15


