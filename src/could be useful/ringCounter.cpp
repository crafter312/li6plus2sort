
#include "ringCounter.h"

bool const ringCounter::relativity = 1;

void ringCounter::SetDistance(float dist0)
{
  distance = dist0;
}
void ringCounter::SetTargetThickness(float thick0)
{
  TargetThickness = thick0;}

ringCounter::ringCounter(TRandom * ran0, histo_sort * Histo1, float dist0)
{
  ran = ran0;
  Histo = Histo1;
  distance = dist0;

  //make map of Csi associated with Pies
  int id_csi, id_csi_plus, id_csi_minus;
  for (int i=0;i<Npie;i++)
  {
    PieCsiMap[i].N = 0;
    //inner ring
    id_csi = floor((float)(i+4)/32.) + 16;
    if (id_csi > 19) id_csi -= 4.;

    PieCsiMap[i].csi[PieCsiMap[i].N] = id_csi;
    PieCsiMap[i].N++;
      
    //in case pies and Csi are not lined up exactly
    // consider Csi if pie is off by plus-minus one
    id_csi_plus = floor((float)(i+1+4)/32.) + 16;
    if (id_csi_plus > 19) id_csi_plus -= 4.;
    if (id_csi_plus != id_csi)
    {
      PieCsiMap[i].csi[PieCsiMap[i].N] = id_csi_plus;
      PieCsiMap[i].N;
    }
       
    id_csi_minus = floor((float)(i-1+4)/32.) + 16;
    if (id_csi_minus > 19) id_csi_plus -= 4.;
    if (id_csi_minus != id_csi)
    {
      PieCsiMap[i].csi[PieCsiMap[i].N] = id_csi_minus;
      PieCsiMap[i].N;
    }
       
    //outer ring
    id_csi = floor((float)(i+4)/8.);
    if (id_csi == 16) id_csi = 0.;

    PieCsiMap[i].csi[PieCsiMap[i].N] = id_csi;
    PieCsiMap[i].N++;
      
    id_csi_plus = floor((float)(i+1+4)/8.);
    if (id_csi_plus == 16) id_csi_plus = 0.;
    if (id_csi_plus != id_csi)
    {
      PieCsiMap[i].csi[PieCsiMap[i].N] = id_csi_plus;
      PieCsiMap[i].N;
    }

    id_csi_minus = floor((float)(i-1+4)/8.);
    if (id_csi_minus == 16) id_csi_minus = 0.;
    if (id_csi_minus != id_csi)
    {
      PieCsiMap[i].csi[PieCsiMap[i].N] = id_csi_minus;
      PieCsiMap[i].N;
    }
       
  }

  counter = 0;
  string name;
  ostringstream outstring;  
  for (int i=0;i<20;i++)
  {
    outstring.str("");
    outstring << "pid" << i;
    name = outstring.str();

    Pid[i] = new pid(name);
  }

  //loading energy loss tables for target
  losses = new CLosses(10,".loss");
  //loading energy loss table fo rthe Al plate
  losses_Al = new CLosses(2,"_Al.loss");

  //Proton Calibrations
  name = "cal/p_calibrations.cal";
  calProton = new calibrate(1,20,name,1);

  name = "cal/d_calibrations.cal";
  calDeuteron = new calibrate(1,20,name,1);

  name = "cal/a_calibrations.cal";
  calAlpha = new calibrate(1,20,name,1);

  
  //time gates for s800 coincidence
  int one,two,three;
  ifstream fileTime("cal/csiTime.gate");
  for (int i=0;i<20;i++)
  {
    fileTime >> one >> two >> three;
    csiTimeMin[one]= two;
    csiTimeMax[one]= three;
  }
  fileTime.close();
  for (int i=0;i<5;i++)
  {
    protonYield_s800[i]=0;
    protonYield_Ar31[i]=0;
    protonYield_Ar31_S28[i]=0;
    protonYield_S29[i]=0;
    protonYield_S29_P27[i]=0;
    protonYield_S29_Si26[i]=0;    
    protonYield_P28[i]=0;
    protonYield_Si27[i]=0;
  }
  alphaYield_s800 = 0;
  alphaYield_Ar31 = 0;
  alphaYield_Ar31_S28 = 0;
}

//*****************************************//
void ringCounter::reset()
{
  Pie.reset();
  Ring.reset();
  Csi.reset();

  //reset the solutions
  for(int i=0;i<Nsolution;i++)
  {
    Solution[i].denergy = 0;
    Solution[i].energy = 0;
    Solution[i].penergy = 0;
    Solution[i].energyR =0; 
    Solution[i].energyCsI = 0;
    Solution[i].icsi =-1;
    Solution[i].iPie =-1;
    Solution[i].iRing = -1;
    Solution[i].mass = 0;
    Solution[i].Ekin = 0;
    Solution[i].iZ =-1;
    Solution[i].iA = -1;
    Solution[i].energyTot = 0;
    for(int j=0;j<3;j++)
    {
      Solution[i].Vvect[j] = 0;
      Solution[i].Mvect[j] = 0;
      Solution[i].MomCM[j] = 0;
    }
      Solution[i].energyCM = 0;
      Solution[i].momentum = 0;
      Solution[i].momentumCM =0;
    }

  Nsolution = 0;

}

//*************************************//
int ringCounter::analysis(s800_results S800_results) // SG 2020/10/27
//void ringCounter::analysis()
{
  match();
  
  counter++;
  proton_present = false;

  Histo->RingsMult->Fill(Ring.Nstore);
  Histo->PiesMult->Fill(Pie.Nstore);
  Histo->CsIMult->Fill(Csi.Nstore);

  // if(Csi.Nstore >=3)
  //   {
  //     cout << "Nstore = " << Csi.Nstore << endl;
      
  //     for(int i=1;i<Csi.Nstore;i++)
  //     {
  //       if(Csi.Order[i].energy == Csi.Order[i-1].energy)
  //         {
  //           cout << i << " " << Csi.Order[i].strip << " " << Csi.Order[i].energy << endl;
  //         }
  //     }
  //   }
  Nsolution = 0;
  //must have pie,ring
  if (!(Pie.Nstore >0 && Ring.Nstore > 0)) return 3;

  // cout << "Before = " << Ring.Nstore << endl;

  //sum neighboring strips in ring dimension
  Ring.Neighbours();
  //  Pie.NeighboursPies();

  Ring.Threshold(0.4);
  Pie.Threshold(0.4);

  if(!(Pie.Nstore>0 && Ring.Nstore>0)) return 4;
  

  // cout << "After = " << Ring.Nstore << endl;

  Histo->PievsRing->Fill(Pie.Order[0].energy,Ring.Order[0].energy);
  Histo->EdiffvsPie->Fill(Pie.Order[0].energy,Pie.Order[0].energy-Ring.Order[0].energy);
    
  // add-back histogram fill stuff =============================================
  // ===========================================================================

  //added by jinyu may22,2019
  for(Int_t iorder=0;iorder<Pie.Nstore;iorder++)
  {
    if(Pie.Order[iorder].neighbours==0) {
      Histo->Pienn->Fill(Pie.Order[iorder].energy);
    }
    if(Pie.Order[iorder].neighbours==1) {
      Histo->Pieabwn1->Fill(Pie.Order[iorder].energy);
      Histo->PieEadd1E0->Fill(Pie.Order[iorder].energyMax,Pie.Order[iorder].energy-Pie.Order[iorder].energyMax);
    }
    if(Pie.Order[iorder].neighbours==2) {
      Histo->Pieabwn2->Fill(Pie.Order[iorder].energy);
      Histo->PieEadd2E0->Fill(Pie.Order[iorder].energyMax,Pie.Order[iorder].energy-Pie.Order[iorder].energyMax);
    }
  }

  for(Int_t iorder=0;iorder<Ring.Nstore;iorder++)
  {
    if(Ring.Order[iorder].neighbours==0) {
        Histo->Ringnn->Fill(Ring.Order[iorder].energy);
    }
    if(Ring.Order[iorder].neighbours==1) {
        Histo->Ringabwn1->Fill(Ring.Order[iorder].energy);
        Histo->RingEadd1E0->Fill(Ring.Order[iorder].energyMax,Ring.Order[iorder].energy-Ring.Order[iorder].energyMax);
    }
    if(Ring.Order[iorder].neighbours==2) {
        Histo->Ringabwn2->Fill(Ring.Order[iorder].energy);
        Histo->RingEadd2E0->Fill(Ring.Order[iorder].energyMax,Ring.Order[iorder].energy-Ring.Order[iorder].energyMax);
    }
  }
    //end of added by jinyu May222019
// ==================================================================
// ==================================================================*/
  

  for(int i=0;i<Pie.Nstore;i++)
  {
    //    cout << "Pie = " << Pie.Order[i].strip << endl;
    Histo->PCSum_AfterAddback->Fill(Pie.Order[i].strip,Pie.Order[i].energy);
    if(Pie.Order[i].neighbours>0)
      Histo->EpiesC_AfterAddback[Pie.Order[i].strip]->Fill(Pie.Order[i].energy);
    else if(Pie.Order[i].neighbours==0)
      Histo->EpiesC_AfterAddback_NoNeighbor[Pie.Order[i].strip]->Fill(Pie.Order[i].energy);
  }

  for(int i=0;i<Ring.Nstore;i++)
  {
    //    cout << "Ring = " << Ring.Order[i].strip << endl;
    Histo->RCSum_AfterAddback->Fill(Ring.Order[i].strip,Ring.Order[i].energy);
    if(Ring.Order[i].neighbours>0)
    {
      //cout << Ring.Order[i].neighbours << endl;
      Histo->EringsC_AfterAddback[Ring.Order[i].strip]->Fill(Ring.Order[i].energy);
    }
    else if(Ring.Order[i].neighbours==0)
      Histo->EringsC_AfterAddback_NoNeighbor[Ring.Order[i].strip]->Fill(Ring.Order[i].energy);
  }
    
  if (!(Pie.Nstore >0 && Ring.Nstore > 0 && Csi.Nstore >0)) return 5;


  //check mapping
  if (Csi.Nstore == 1)
  {
    int id_csi = Csi.Order[0].strip;

    //      cout << id_csi << endl;
    
    if (id_csi < 16) Histo->csiInner->Fill(Ring.Order[0].strip);
    else Histo->csiOuter->Fill(Ring.Order[0].strip);
    
    if (id_csi == 0) Histo->csi0->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 4) Histo->csi4->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 8) Histo->csi8->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 12) Histo->csi12->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 16) Histo->csi16->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 17) Histo->csi17->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 18) Histo->csi18->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
    if (id_csi == 19) Histo->csi19->Fill(Pie.Order[0].strip,Ring.Order[0].strip);
  }
  
  //if(Csi.Nstore == 1 && Pie.Nstore == 1)
  //E-De maps for singles, make sure pies are in front of Csi

  // if (Csi.Nstore >=1)// && S800_results.trig_singles)
  //   {


  //     int id_csi = Csi.Order[0].strip;

  //     for (int ipi = 0;ipi<1;ipi++)
  //     {

  //       if (id_csi < 16) 
  //         {
  //           int id_csi2 = floor((float)(Pie.Order[ipi].strip+4)/8.) ;
  //           if (id_csi2 == 16) id_csi2 = 0.;

  //           if (id_csi == id_csi2)
  //         Histo->dee[id_csi]->Fill(Csi.Order[0].energyR,Pie.Order[ipi].energy);
  //         }
  //       else
  //         {

  //           int  id_csi2 = floor((float)(Pie.Order[ipi].strip+4)/32.) + 16;
  //           if (id_csi2 > 19) id_csi2 -= 4.;
  //           if (id_csi == id_csi2)
  //         Histo->dee[id_csi]->Fill(Csi.Order[0].energyR,Pie.Order[ipi].energy);
       
  //         }
  //     }
  //     Histo->csiPie->Fill(id_csi,Pie.Order[0].strip);

  //   }



  
  int Ncsi_before = Csi.Nstore;
  //remove csi with large times // random S800 Csi coincidences
  for (int i=Csi.Nstore-1;i>=0;i--)
  {
    if (Csi.Order[i].time/10. > csiTimeMax[Csi.Order[i].strip]) Csi.Remove(i);
  }
  if (Csi.Nstore == 0) return 6;


  //E-De maps for S800 coincidences
  if (Csi.Nstore >=1)// && S800_results.trig_coin)
  {   
    int id_csi = Csi.Order[0].strip;

    for (int ipi = 0;ipi<1;ipi++)
    {

      if (id_csi < 16) 
      {
        int id_csi2 = floor((float)(Pie.Order[ipi].strip+4)/8.) ;
        if (id_csi2 == 16) id_csi2 = 0.;

        if (id_csi == id_csi2)
          Histo->dee_S800[id_csi]->Fill(Csi.Order[0].energyR,Pie.Order[ipi].energy);
      }
      else
      {
        int  id_csi2 = floor((float)Pie.Order[ipi].strip/32.) + 16;
        if (id_csi == id_csi2)
          Histo->dee_S800[id_csi]->Fill(Csi.Order[0].energyR,Pie.Order[ipi].energy);
      }
    }
    Histo->csiPie->Fill(id_csi,Pie.Order[0].strip);

  }
   

  //  if (!S800_results.trig_coin) return;

  //cout << Pie.Nstore << " " << Ring.Nstore << " " << Csi.Nstore << endl;

  int NsiHits = min(Pie.Nstore,Ring.Nstore);

  //for now only consider multiplicity 2 at most
  // if (NsiHits > 2) NsiHits = 2;

  // match pies and strips   
  multiHit();

  // match si solution with Csi possibilities 
  int Nmatched = matchWithCsi();

  //look for ambiguities, i.e.,  if there were two possible Si solutions
  // and the two Si-Pie energies are very similar, maybe the match ring-pie
  //combinations are swapped by mistake
  if (Nmatched  == 0 && Nsolution == 2 && Csi.Nstore == 2)
  {
    if (abs(Solution[0].denergy-Solution[1].denergy)< .3)
    {
      int ring0 = Solution[0].iRing;
      int ring1 = Solution[1].iRing;

      Solution[0].iRing = ring1;
      Solution[1].iRing = ring0;
      Nmatched = matchWithCsi(); 
    }
  }
   
  //
  //  if (Csi.Nstore == 2 && Nmatched == 0)
  //  {
  //  cout << "Nsolutions = " << Nsolution << " counter= " << counter <<endl;
  //  cout << "pies =  " << Pie.Nstore << endl;
  //  for (int i=0;i<Pie.Nstore;i++) cout <<Pie.Order[i].energy << " " << Pie.Order[i].strip<< endl;
  //  cout << "rings = " << Ring.Nstore << endl; 
  //  for (int i=0;i<Ring.Nstore;i++) cout <<Ring.Order[i].energy << " " << Ring.Order[i].strip << endl;
  //    
  //  }
  //

  for(int i=0;i<Nsolution;i++)
  {
    int icsi = Solution[i].icsi;
    if(icsi >=0)
      Histo->dee[icsi]->Fill(Solution[i].energyR,Solution[i].denergy);
  }
  

  //Get the kinematics for the detected and identified particles
  //calculates energy, momentum, and velocity
  getMomentum();
  
  return 0;
}

//***************************************************
//extracts multiple particle from strip data 
int ringCounter::multiHit()
{
  // cout << endl;
  // cout << "Pie Mult = "<< Pie.Nstore << endl;
  // for(int i =0;i<Pie.Nstore;i++)
  //   {
  //     cout << i << " " << Pie.Order[i].strip << " " << Pie.Order[i].energy << endl; 
  //   }
  // cout << "Ring Mult = "<< Ring.Nstore << endl;
  // for(int i =0;i<Ring.Nstore;i++)
  //   {
  //     cout << i << " " << Ring.Order[i].strip << " " << Ring.Order[i].energy << endl; 
  //   }
  
  int Ntries = min(Ring.Nstore,Pie.Nstore);
  if (Ntries > 6) Ntries =6;
  Nsolution = 0;
  if (Ntries <= 0) return 0;
  
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
      pie_energy = Pie.Order[arrayB[i]].energy;
      ring_energy = Ring.Order[i].energy;
      float accept = ring_pie_matching_acceptance;
      if(pie_energy < 10. && pie_energy >0.) accept =1.5/pie_energy;
      
      if (fabs(pie_energy-ring_energy) >pie_energy*accept)
      {
        leave = 1;
        break;
      }
    }
    if (leave) continue;

    // now load solution
    for (int i=0;i<NestDim;i++)
    {
      pie_energy = Pie.Order[i].energy;
      Solution[i].denergy = pie_energy;
      Solution[i].iRing = Ring.Order[i].strip;
      Solution[i].iPie= Pie.Order[arrayB[i]].strip;
    }

    Nsolution = NestDim;
    
    break;
  }

  // cout << "Nsolutions = " << Nsolution << endl;;
  // for(int i =0;i<Nsolution;i++)
  //   {
  //     cout << "Pie = " << Solution[i].iPie << " Ring = " << Solution[i].iRing << endl;
  //   }
  // cout << endl;
  

  return Nsolution;
}

//***************************************************
//recursive subroutine  used for multihit subroutine
void ringCounter::loop(int depth)
{
  if (depth == NestDim )
  {
    // do stuff here
    int dstrip = 0;
    float de = 0.;
    for (int i=0;i<NestDim;i++)
    {
      pie_energy = Pie.Order[NestArray[i]].energy;
      ring_energy = Ring.Order[i].energy;
      de += abs(pie_energy-ring_energy);
    }

    if (dstrip < dstripMin)
    {
      dstripMin = dstrip;
      for (int i=0;i<NestDim;i++) {arrayD[i] = NestArray[i];}
    }

    if (de < deMin)
    {
      deMin = de;
      for (int i=0;i<NestDim;i++) {arrayB[i] = NestArray[i];}
    }

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


//********************************************
ringCounter::~ringCounter()
{
  delete losses;
  delete losses_Al;
  delete calProton;
  return;
}


//********************************
int ringCounter::matchWithCsi()
{
  multProton = 0;
  multAlpha = 0;
  // match Si and Csi
  int Nmatched = 0;
  for (int i=0;i<Nsolution;i++)
  {

    float radius = (float)Solution[i].iRing/(float)Nstrip
                    *(Si_r_max-Si_r_min)+Si_r_min;
    Solution[i].theta = atan(radius/distance);
    Solution[i].phi = ((float)Solution[i].iPie+ran->Rndm())/(float)Npie*acos(-1.)*2.;
       
    int id_csi,id_csi_plus,id_csi_minus;

    if (Solution[i].iRing < 51) //inner ring
    {

      id_csi = floor((float)(Solution[i].iPie+4)/32.) + 16;
      if (id_csi > 19) id_csi -= 4.;

      //in case pies and Csi are not lined up exactly
      // consider Csi if pie is off by plus-minus one
      id_csi_plus = floor((float)(Solution[i].iPie+1+4)/32.) + 16;
      if (id_csi_plus > 19) id_csi_plus -= 4.;


      id_csi_minus = floor((float)(Solution[i].iPie-1+4)/32.) + 16;
      if (id_csi_minus > 19) id_csi_plus -= 4.;
       
    }
    else // outer ring
    {
      id_csi = floor((float)(Solution[i].iPie+4)/8.) ;
      if (id_csi == 16) id_csi = 0.;

      id_csi_plus = floor((float)(Solution[i].iPie+1+4)/8.) ;
      if (id_csi_plus == 16) id_csi_plus = 0.;

      id_csi_minus = floor((float)(Solution[i].iPie-1+4)/8.) ;
      if (id_csi_minus == 16) id_csi_minus = 0.;
       
    }

    bool found = false;
    int Nfound = 0;

    //cout << "Ncsi = " << Csi.Nstore << " " << Nsolution << endl;
    for (int icsi = 0;icsi<Csi.Nstore;icsi++)
    {
      Solution[i].ipid = 0;
      if (Csi.Order[icsi].strip == id_csi ||
          Csi.Order[icsi].strip == id_csi_plus ||
          Csi.Order[icsi].strip == id_csi_minus )
      {

        //cout << icsi << " "<< Csi.Order[icsi].strip  << " " << id_csi << " " << id_csi_plus << " " << id_csi_minus << endl;
        //          Solution[i].energy = Csi.Order[icsi].energyR;
        Solution[i].energyR = Csi.Order[icsi].energyR;
        Solution[i].icsi = Csi.Order[icsi].strip;
        found = true;
        Nfound++;
        Nmatched++;

        // cout << Solution[i].energyR << " " << Solution[i].denergy << endl;
        // cout << Csi.Order[icsi].strip << endl;
        // cout << icsi << " " << i << endl;
        bool FoundPid = Pid[Csi.Order[icsi].strip]->getPID(Solution[i].energyR,
                                Solution[i].denergy);

        float x = Solution[i].theta*cos(Solution[i].phi)*180./acos(-1.);
        float y = Solution[i].theta*sin(Solution[i].phi)*180./acos(-1.);

        if(!FoundPid) continue;
          

        int Z = Pid[Csi.Order[icsi].strip]->Z;
        int A = Pid[Csi.Order[icsi].strip]->A;

        
        if (Pid[Csi.Order[icsi].strip]->Z == 1 
            && Pid[Csi.Order[icsi].strip]->A == 1)
        {
          proton_present = true;
          multProton++;
          Solution[i].ipid = 1; 
          Histo->protonHitMap->Fill(x,y);
              
        }  //proton if
        else if (Pid[Csi.Order[icsi].strip]->Z == 2
                 && Pid[Csi.Order[icsi].strip]->A == 4)
        {
          multAlpha++;
        }

        csical(icsi,i);
        //cout << Z << " " << A << endl;
        //cout << Csi.Order[icsi].strip << " " << Csi.Order[icsi].energyR << endl;
        // cout << Solution[i].denergy << " " << Solution[i].energy << endl;
        float sumEnergy = Solution[i].denergy + Solution[i].energy; //Si + CsI in MeV
        //if(Z == 1 && A ==1)
        if(1)
        {
          //calculate velocity before energy loss corrections, plotted in det.cpp
          float mass_mev = getMass(Z,A)*931.478;
          float pc_before = sqrt(pow(sumEnergy+mass_mev,2) - pow(mass_mev,2));
          velocity_before = pc_before/(sumEnergy+mass_mev);

          //calculate energy lost in Al plate
          //602.1 mg/cm2 Al plate (2.23 mm)
          //857.3 mg/cm2 Al plate (3.175 mm or 1/8")
          float Althick = 857.3/cos(Solution[i].theta);
          float Ebeforetarget = losses_Al->getEin(sumEnergy,Althick,Z,A);
          sumEnergy = Ebeforetarget;

          //Calculate the energy lost in the target
          float thick = TargetThickness/2./cos(Solution[i].theta); //*1.30; //0.540 mm Be target -> 99.79 mg/cm2

          float ein = losses->getEin(sumEnergy,thick,Z,A);  
          
          Solution[i].Ekin = ein;
          Solution[i].iZ = Z;
          Solution[i].iA = A;
          Solution[i].mass = getMass(Z,A);//*931.478;
        }
          
      } //csi match if 

    } //loop over Csi

    if (Nfound > 1)
    {
      return 0;
    }
    // this can happen if pie is close to edge of
    //CSI and two Csi fired, possibly a particle went in one and then
    //out the other. we are going to reject such events for now

  }//loop over solutions

  return Nmatched;
}


void ringCounter::match()
{

  if (Pie.Nstore == 0 && Ring.Nstore == 0 && Csi.Nstore == 0) return;
  //cout << "match() " << Pie.Nstore << " " << Ring.Nstore << " " << Csi.Nstore << endl;

  //find possible Csi detectors which are behind hit pie sectors
  int Csi_NumberPossiblePies[Ncsi]={0};
  int Csi_pies[Ncsi][Npie];
  int Nsolutions = 0;
  for (int ipie = 0;ipie<Pie.Nstore;ipie++)
  {
    PieCsiMap[Pie.Order[ipie].strip].NcsiThere = 0;
    for (int icsi_possible = 0;icsi_possible<PieCsiMap[ipie].N;icsi_possible++)
    {
      PieCsiMap[ipie].there[icsi_possible] = false;
      for (int icsi_det = 0;icsi_det<Csi.Nstore;icsi_det++)
      {
        if (Csi.Order[icsi_det].strip == PieCsiMap[Pie.Order[ipie].strip].csi[icsi_possible])
        {
          PieCsiMap[Pie.Order[ipie].strip].NcsiThere++;
          PieCsiMap[Pie.Order[ipie].strip].there[icsi_possible] = true;
          Nsolutions++;  
          Csi_NumberPossiblePies[icsi_det]++;
          Csi_pies[icsi_det][Csi_NumberPossiblePies[icsi_det]] = ipie;
        }
      }
    }
  }

  //cout << "Nsolutions " << Nsolutions << endl;

  if (Nsolutions == 0) return; // no possible Pie Csi match-ups

  //do any CSI have more than one possible Pie associated with it
  int Nproblems = 0;

  
  for (int icsi=0;icsi<Ncsi;icsi++)
  {
    if (Csi_NumberPossiblePies[icsi] == 1)
    {
      int ipie = Csi_pies[icsi][0];
    }   
  
    if (Csi_NumberPossiblePies[icsi] > 1)
    {
      Nproblems++;
    }
  }
  
  //cout << "Nsol/Nprob " << Nsolutions << " " << Nproblems << endl;       
}

void ringCounter::getMomentum()
{
  for(int i = 0;i<Nsolution;i++)
  {    
    if(Solution[i].iZ ==-1) continue;
    
    if (relativity)
    {
      Solution[i].mass *= m0; //931.478 for AMU->MeV
      Solution[i].getMomentum(); //also calculates energyTot and momentum vectors

      //momentum = Solution[i].Kinematics.getMomentum(Solution[i].Ekin,Solution[i].mass);
      //Solution[i].energyTot = Solution[i].Ekin + Solution[i].mass;
      // cout << " " << Solution[i].mass << endl;
          
    }
    else
    {
      float theta = Solution[i].theta;
      float phi = Solution[i].phi;
      float momentum;
      momentum = sqrt(2.*Solution[i].mass*Solution[i].Ekin);
      Solution[i].mass = 0.;
      Solution[i].Mvect[0] = momentum*sin(theta)*cos(phi);
      Solution[i].Mvect[1] = momentum*sin(theta)*sin(phi);
      Solution[i].Mvect[2] = momentum*cos(theta); 
      Solution[i].momentum = momentum;
    }
          
    Solution[i].velocity = Solution[i].momentum/Solution[i].energyTot;
          
  }

}

void ringCounter::csical(int icsi1, int i2)
{
  //cout << "csical" << endl;

  int iCsi = Csi.Order[icsi1].strip;

  
  //Everything is treated as a proton by default
  //energies is changed for known particles
  float energy = calProton->getEnergy(0,iCsi,Csi.Order[icsi1].energyR);

  if (Pid[iCsi]->Z == 1  && Pid[iCsi]->A == 1)
  {
    //cout << "1 1" << endl;
    Histo->p_ECsI_theta[iCsi]->Fill(Solution[i2].theta,Solution[i2].energyR);
    Histo->p_ECsI_phi[iCsi]->Fill(Solution[i2].phi,Solution[i2].energyR);
    Histo->p_ECsI_ring[iCsi]->Fill(Solution[i2].iRing,Solution[i2].energyR);

    Histo->ECsI_Zgate[iCsi]->Fill(Solution[i2].energyR);

    Histo->ECsI_cal[iCsi]->Fill(energy);
    Histo->ECsI_cal_all->Fill(energy);

    Histo->CaldEE[iCsi]->Fill(energy, Solution[i2].denergy);
    Histo->CaldEE_all->Fill(energy, Solution[i2].denergy);

  }
  else if(Pid[iCsi]->Z == 1 && Pid[iCsi]->A == 2)
  {
    energy = calDeuteron->getEnergy(0,iCsi,Csi.Order[icsi1].energyR);
  }
  
  else if (Pid[iCsi]->Z == 2 && Pid[iCsi]->A == 4)
  {
    //cout << "2 4" << endl;
    energy = calAlpha->getEnergy(0,iCsi,Csi.Order[icsi1].energyR);
    
    Histo->alpha_ECsI_theta[iCsi]->Fill(Solution[i2].theta,Solution[i2].energyR);
    Histo->alpha_ECsI_phi[iCsi]->Fill(Solution[i2].phi,Solution[i2].energyR);
    Histo->alpha_ECsI_ring[iCsi]->Fill(Solution[i2].iRing, Solution[i2].energyR);
  }
  
  Solution[i2].energy = energy;
  
}








double ringCounter::getMass(int iZ,int iA)
{
  //All masses are hard coded in. If adding new isotope, just copy/paste
  if (iZ == 1)
  {
    if (iA == 1)  return 1.+ 7.2889/m0;
    else if (iA == 2) return 2. + 13.135/m0;
    else if (iA == 3) return 3.+ 14.949/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 2)
  {
    if (iA == 3) return 3. + 14.931/m0;
    else if (iA == 4) return 4. + 2.424/m0;
    else if (iA == 6) return 6. + 17.592/m0;
    else if (iA == 8) return 8. + 31.609/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 3)
  {
    if (iA == 6) return 6.+14.086/m0;
    else if(iA == 7) return 7. + 14.907/m0;
    else if (iA == 8) return 8. + 20.945/m0;
    else if (iA == 9) return 9. + 24.954/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 4)
  {
    if (iA == 7) return 7. + 15.768/m0;
    else if (iA == 8) return 8. + 4.941/m0;
    else if (iA == 9) return 9. + 11.348/m0;
    else if (iA == 10) return 10. + 12.607/m0;
    else if (iA == 11) return 11. + 20.177/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ==5)
  {
    if (iA == 8) return 8. + 22.921/m0;
    else if (iA == 10) return 10. + 12.050/m0;
    else if (iA == 11) return 11. + 8.667/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 6)
  {
    if (iA == 9) return 9. + 28.910/m0;
    else if (iA == 10) return 10. + 15.698/m0;
    else if (iA == 11) return 11. + 10.649/m0;
    else if (iA == 12) return 12.;
    else if (iA == 13) return 13. + 3.125/m0;
    else if (iA == 14) return 14. + 3.019/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 7)
  {
    if (iA == 12) return 12. + 17.338/m0;
    else if (iA == 13) return 13. + 5.345/m0;
    else if (iA == 14) return 14. + 2.863/m0;
    else if (iA == 15) return 15. + .101/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 8)
  {
    if (iA == 13) return 13. + 23.115/m0;
    else if (iA == 14) return 14. + 8.007/m0;
    else if (iA == 15) return 2.855/m0;
    else if (iA == 16) return 16. -4.737/m0;
    else if (iA == 17) return 17. - .808/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 9)
  {
    if (iA == 17) return 17.+1.951/m0;
    else if (iA == 18) return 18.  + .873/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort(); 
    }
  }
  else if (iZ == 10)
  {
    if (iA == 17) return 17. +16.500/m0;
    else if (iA == 18) return 18. + 5.317/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }
  else if (iZ == 11)
  {
    if (iA == 21) return 21. - 2.184/m0;
    else if (iA == 22) return 22. -5.181/m0;
    else if (iA == 23) return 23. -9.529/m0;
    else
    {
      cout << "No mass info for Z = " << iZ << "A=" << iA << endl;
      abort();
    }
  }

  else if (iZ == 12)
  {
    if (iA == 24) return 24 - 13.933/m0;
    else if (iA==23) return 23. -5.473/m0;
    else if (iA==25) return 25. -13.192/m0;
    else if (iA==26) return 26. -16.214/m0;
    else
    {
      cout << "No mass info for Z = " << iZ << "A =" << iA << endl;
      abort();
    }
  }

  else if(iZ==13)     
  {
    if(iA==26) return 26 - 12.210/m0;
    else if (iA == 25) return 25. -8.195/m0;
    else if (iA == 24) return 24.- 0.048/m0;
    else if (iA == 27) return 27. -17.196/m0;
    else if (iA == 28) return 28. -16.85/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }     
  } 
  
  else if(iZ==14)
  {
    if (iA==26) return 26 - 7.140/m0;
    else if (iA==27) return 27 - 12.384/m0;
    else if (iA==28) return 28. -21.492/m0;
    else if (iA==25) return 25. +3.827/m0;
    else if (iA==29) return 29. -21.895/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }

  else if(iZ ==15)
  {
    if(iA ==27) return 27. - 0.7224/m0;
    else if (iA==28) return 28. - 7.147/m0;
    else if (iA == 29) return 29. -16.592/m0;
    else if (iA == 30) return 30. -3.156/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }
  else if(iZ ==16)
  {
    if(iA == 28) return 28. + 4.073/m0;
    else if(iA = 29) return 29. - 3.156/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }
  else if(iZ == 17)
  {
    if(iA == 31) return 31. - 7.034/m0;
    else if(iA == 32) return 32. - 13.334/m0;
    else if(iA == 33) return 33. - 21.003/m0;
    else if(iA == 34) return 34. - 24.440/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }

  else if (iZ == 18)
  {
    if(iA== 32) return 32. - 2.2003/m0;
    else if(iA==33) return 33. - 9.3843/m0;
    else if(iA==34) return 34. - 18.3783/m0;
    else if(iA==35) return 35. - 23.0472/m0;
    else if(iA==36) return 36. - 30.2315/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }

  else if (iZ == 19)
  {
    if(iA== 35) return 35. - 11.1729/m0;
    else if(iA==36) return 36. - 17.4170/m0;
    else
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  } 

  else if(iZ==20)
  {
    if(iA==35) return 35. + 4.453/m0; //predicted in 1985, doi=10.1103/PhysRevLett.5.1384
    if(iA==36) return 36. - 6.4836/m0; //found in Lalanne, taken from surbrook PRC103 014323  //6.4511/m0;
    if(iA==37) return 37. - 13.1361/m0;
    if(iA==38) return 38. - 29.7981/m0;
    else 
    {
      cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
      abort();
    }
  }

  cout << "No mass info for Z = "<< iZ << " A =" << iA << endl;
  abort();
  return -1;
}


