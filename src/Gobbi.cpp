#include "Gobbi.h"

//Nicolas Dronchi 2022_04_04
//Class written to handle all specifics of the Gobbi array
//such as communicating with HINP, calibrations, Checking for charge sharing in neighbor
//calculating geometry

using namespace std;

Gobbi::Gobbi(histo * Histo1)
{
  Histo = Histo1;
  ADC = new HINP();
  Targetdist = 23.95;//23.95;//24.1;//23.5; //cm
  TargetThickness = 2.65;//3.2;//2.65; //mg/cm^2 for CD2 tar1
  //TargetThickness = 3.8; //mg/cm^2

  for (int id=0;id<4;id++)
  {
    Silicon[id] = new silicon(TargetThickness);
    Silicon[id]->init(id); //tells Silicon what position it is in
    Silicon[id]->SetTargetDistance(Targetdist);
  }

  FrontEcal = new calibrate(4, Histo->channum, "Cal/FrontEcal.dat", 1, true);
  BackEcal = new calibrate(4, Histo->channum, "Cal/BackEcal.dat", 1, true);
  DeltaEcal = new calibrate(4, Histo->channum, "Cal/DeltaEcal.dat", 1, true);
  FrontTimecal = new calibrate(4, Histo->channum, "Cal/FrontTimecal.dat",1, false);
  BackTimecal = new calibrate(4, Histo->channum, "Cal/BackTimecal.dat",1, false);  
  DeltaTimecal = new calibrate(4, Histo->channum, "Cal/DeltaTimecal.dat",1, false);
}

Gobbi::~Gobbi()
{
  delete ADC;
  delete FrontEcal;
  delete BackEcal;
  delete DeltaEcal;
  delete FrontTimecal;
  delete BackTimecal;
  delete DeltaTimecal;
  //delete[] Silicon; //not needed as it is in automatic memory, didn't call with new
}

bool Gobbi::unpack(unsigned short *point)
{
  //reset the Silicon class
  for (int i=0;i<4;i++){ Silicon[i]->reset();}

  bool stat = true;
  stat = ADC->unpackSi_HINP4(point);
  if (!stat)
  {
    cout << "Bad hira" << endl;
    return stat;
  }

  for (int i=0; i<ADC->NstripsRead; i++)
  {
    //check if there is an array for determined chip# and chan#
    if (ADC->board[i] > 12 || ADC->chan[i] >= Histo->channum)
    {
      cout << "ADC->NstripsRead " << ADC->NstripsRead << endl;
      cout << "i " << i << endl;
      cout << "Board " << ADC->board[i] << " and chan " << ADC->chan[i];
      cout << " unpacked but not saved" << endl;
      return true;
    }

    float Energy = 0;
    float time = 0; //can be calibrated or shifted later

    //Use calibration to get Energy and fill elist class in silicon
    if (ADC->board[i] == 1 || ADC->board[i] == 3 || ADC->board[i] == 5 || ADC->board[i] == 7)
    {
      int quad = (ADC->board[i] - 1)/2;
      Energy = FrontEcal->getEnergy(quad, ADC->chan[i], ADC->high[i]);
      time = FrontTimecal->getTime(quad, ADC->chan[i], ADC->time[i]);

      Histo->sumFrontE_R->Fill(quad*Histo->channum + ADC->chan[i], ADC->high[i]);
      Histo->sumFrontTime_R->Fill(quad*Histo->channum + ADC->chan[i], ADC->time[i]);
      Histo->sumFrontE_cal->Fill(quad*Histo->channum + ADC->chan[i], Energy);
      Histo->sumFrontTime_cal->Fill(quad*Histo->channum + ADC->chan[i], time);

      Histo->FrontE_R[quad][ADC->chan[i]]->Fill(ADC->high[i]);
      Histo->FrontElow_R[quad][ADC->chan[i]]->Fill(ADC->low[i]);
      Histo->FrontTime_R[quad][ADC->chan[i]]->Fill(ADC->time[i]);
      Histo->FrontE_cal[quad][ADC->chan[i]]->Fill(Energy);      

      //if (Energy > .5 && ADC->time[i] > 3420 && ADC->time[i] < 6380 && (quad != 1 || Energy > 1.8))
      //need to set thresholds just above noise
      //if (quad == 1 && (

      if (Energy > .5 && (quad != 1 || Energy > 2))
      {
          Silicon[quad]->Front.Add(ADC->chan[i], Energy, ADC->low[i], ADC->high[i], time);
          Silicon[quad]->multFront++;
      }
    }
    if (ADC->board[i] == 2 || ADC->board[i] == 4 || ADC->board[i] == 6 || ADC->board[i] == 8)
    {
      int quad = (ADC->board[i]/2)-1;
      Energy = BackEcal->getEnergy(quad, ADC->chan[i], ADC->high[i]);
      time = BackTimecal->getTime(quad, ADC->chan[i], ADC->time[i]);

      Histo->sumBackE_R->Fill(quad*Histo->channum + ADC->chan[i], ADC->high[i]);
      Histo->sumBackTime_R->Fill(quad*Histo->channum + ADC->chan[i], ADC->time[i]);
      Histo->sumBackE_cal->Fill(quad*Histo->channum + ADC->chan[i], Energy);
      Histo->sumBackTime_cal->Fill(quad*Histo->channum + ADC->chan[i], time);

      Histo->BackE_R[quad][ADC->chan[i]]->Fill(ADC->high[i]);
      Histo->BackElow_R[quad][ADC->chan[i]]->Fill(ADC->low[i]);
      Histo->BackTime_R[quad][ADC->chan[i]]->Fill(ADC->time[i]);
      Histo->BackE_cal[quad][ADC->chan[i]]->Fill(Energy);  

      if (Energy > .5)
      {      
          Silicon[quad]->Back.Add(ADC->chan[i], Energy, ADC->low[i], ADC->high[i], time);
          Silicon[quad]->multBack++;
      }
    }
    if (ADC->board[i] == 9 || ADC->board[i] == 10 || ADC->board[i] == 11 || ADC->board[i] == 12)
    {
      int quad = (ADC->board[i]-9);
      Energy = DeltaEcal->getEnergy(quad, ADC->chan[i], ADC->high[i]);
      time = DeltaTimecal->getTime(quad, ADC->chan[i], ADC->time[i]);

      Histo->sumDeltaE_R->Fill(quad*Histo->channum + ADC->chan[i], ADC->high[i]);
      Histo->sumDeltaTime_R->Fill(quad*Histo->channum + ADC->chan[i], ADC->time[i]);
      Histo->sumDeltaE_cal->Fill(quad*Histo->channum + ADC->chan[i], Energy);
      Histo->sumDeltaTime_cal->Fill(quad*Histo->channum + ADC->chan[i], time);

      Histo->DeltaE_R[quad][ADC->chan[i]]->Fill(ADC->high[i]);
      Histo->DeltaElow_R[quad][ADC->chan[i]]->Fill(ADC->low[i]);
      Histo->DeltaTime_R[quad][ADC->chan[i]]->Fill(ADC->time[i]);
      Histo->DeltaE_cal[quad][ADC->chan[i]]->Fill(Energy);  

      //if(Energy > .2 && ADC->time[i] > 1765 && ADC->time[i] < 8600)
      if(Energy > .2)
      {
        //if (quad == 0 && ADC->chan[i] == 0) cout << "EE " << Energy << endl;
        
        Silicon[quad]->Delta.Add(ADC->chan[i], Energy, ADC->low[i], ADC->high[i], time);
        Silicon[quad]->multDelta++;
      }
    }
  }
  //data is unpacked and stored into Silicon class at this point


  //This is the spot if we run Silicon->Neighbours()
  for (int id=0;id<4;id++) 
  {
    Silicon[id]->Front.Neighbours(id);
    Silicon[id]->Back.Neighbours(id);
    Silicon[id]->Delta.Neighbours(id);
  }

  //Fill summary after addback
  int sumchan = 0;
  for (int id=0;id<4;id++) 
  {
    for (int n=0; n<Silicon[id]->Front.Nstore; n++)
    {
      sumchan = id*Histo->channum + Silicon[id]->Front.Order[n].strip;
      Histo->sumFrontE_addback->Fill(sumchan, Silicon[id]->Front.Order[n].energy);
    }
    for (int n=0; n<Silicon[id]->Back.Nstore; n++)
    {
      sumchan = id*Histo->channum + Silicon[id]->Back.Order[n].strip;
      Histo->sumBackE_addback->Fill(sumchan, Silicon[id]->Back.Order[n].energy);
    }
    for (int n=0; n<Silicon[id]->Delta.Nstore; n++)
    {
      sumchan = id*Histo->channum + Silicon[id]->Delta.Order[n].strip;
      Histo->sumDeltaE_addback->Fill(sumchan, Silicon[id]->Delta.Order[n].energy);
    }
  }

  
  for (int id=0;id<4;id++) 
  {
    if (Silicon[id]->Front.Nstore == 1 && Silicon[id]->Delta.Nstore == 1)
    {
      Histo->frontdeltastripnum[id]->Fill(Silicon[id]->Front.Order[0].strip, Silicon[id]->Delta.Order[0].strip);
      Histo->timediff[id]->Fill(Silicon[id]->Front.Order[0].time - Silicon[id]->Delta.Order[0].time);
    }
  }

  //look at all the information/multiplicities and determine how to match up the information
  int totMulti = 0;
  for (int id=0;id<4;id++) 
  {
    //save comp time if the event is obvious
    if (Silicon[id]->Front.Nstore ==1 && Silicon[id]->Back.Nstore ==1 && Silicon[id]->Delta.Nstore ==1)
    {
      totMulti += Silicon[id]->simpleFront();
      //cout << "simple " << totMulti << endl;
    }
    else //if higher multiplicity then worry about picking the right one
    {    //this also handles the case where Nstore=0 for any of the chanels
      totMulti += Silicon[id]->multiHit();
      //cout << "multi " << totMulti << endl;
    }
  }

  //plot E vs dE bananas and hitmap of paired dE,E events
  for (int id=0;id<4;id++) 
  {
    //cout << "id " << id << "   Nsol " << Silicon[id]->Nsolution << endl;
    for (int isol=0;isol<Silicon[id]->Nsolution; isol++)
    {
      //fill in hitmap of gobbi
      Silicon[id]->position(isol); //calculates x,y pos, and lab angle
      //cout << "isol " << isol << endl; 
      //cout << "cos " << cos(Silicon[id]->Solution[isol].theta) << endl;
      //cout << "the " << Silicon[id]->Solution[isol].theta*180./3.1415 << endl;
      //cout << "front strip " << Silicon[id]->Solution[isol].ifront << "  back strip " << Silicon[id]->Solution[isol].iback << endl;
      //cout << "x " << Silicon[id]->Solution[isol].Xpos << "  y " << Silicon[id]->Solution[isol].Ypos << endl;
      //cout << "E " << Silicon[id]->Solution[isol].energy << "  dE " << Silicon[id]->Solution[isol].denergy << endl;


      //these events are all blocked by the beam donut, radius 1.9cm
      if (1.5 > sqrt( pow(Silicon[id]->Solution[isol].Xpos,2)+ pow(Silicon[id]->Solution[isol].Ypos,2)) )
        continue;

      //fill in dE-E plots to select particle type
      float Ener = Silicon[id]->Solution[isol].energy + Silicon[id]->Solution[isol].denergy*(1-cos(Silicon[id]->Solution[isol].theta));

      Histo->DEE[id]->Fill(Ener, Silicon[id]->Solution[isol].denergy*cos(Silicon[id]->Solution[isol].theta));

      Histo->xyhitmap->Fill(Silicon[id]->Solution[isol].Xpos, Silicon[id]->Solution[isol].Ypos);
      //fill hist on theta/phi angles
      double th = Silicon[id]->Solution[isol].theta*180./pi; //in deg, not rad
      Histo->Evstheta[id]->Fill(th, Silicon[id]->Solution[isol].energy);
      Histo->Evstheta_all->Fill(th, Silicon[id]->Solution[isol].energy);
      Histo->Theta->Fill(th);

      //used to get high energy calibration points
      //need to convert interlaced strip numbers to calibration strip numbers
      int chan = Silicon[id]->Solution[isol].ifront;
      if (chan%2 ==0) //evens
      {
        chan = chan/2 + 16;
      }
      else //odds
      {
        chan = (chan -1)/2;
      }

      //make a correction to the energy based on angle
      float angle_Ecorr =  1.0277e-5*pow(th,3) + 1.6125e-3*pow(th,2) + 8.3097e-4*th - 1.0227e-3;
      //cout << "th " << th << " Angle corr " << angle_Ecorr << " MeV" << endl;
      float Ecorr = Silicon[id]->Solution[isol].energy + angle_Ecorr;
      float Ecorr_R = FrontEcal->reverseCal(id,Silicon[id]->Solution[isol].ifront, Ecorr);
      //if (Ecorr > 5)      
        //cout << "EnergyR " << Silicon[id]->Solution[isol].energyR << " Ecorr " << Ecorr << " Ecorr_R " << Ecorr_R << endl;

      Histo->AngleCorrE[id][chan]->Fill(Ecorr);
      Histo->AngleCorr_noCorr[id][chan]->Fill(Silicon[id]->Solution[isol].energy);
      Histo->AngleCorrE_R[id][chan]->Fill(Ecorr_R);
      
      Histo->AngleCorrFrontE_cal->Fill(id*Histo->channum + Silicon[id]->Solution[isol].ifront, Ecorr);

      int chandE = Silicon[id]->Solution[isol].ide;
      if (chan%2 ==0) //evens
      {
        chandE = chandE/2 + 16;
      }
      else //odds
      {
        chandE = (chandE -1)/2;
      }

      //make a correction to the energy based on angle
      float angle_dEcorr =  -1.0971e-5*pow(th,3) - 1.1446e-3*pow(th,2) - 8.9371e-4*th + 1.0879e-3;
      //float angle_dEcorr =  2.0527e-6*pow(th,3) - 1.4281e-3*pow(th,2) + 1.5589e-4*th - 7.3389e-4;//no Au foilloss
      //cout << "th " << th << " Angle corr " << angle_dEcorr << " MeV" << endl;
      float dEcorr = Silicon[id]->Solution[isol].denergy + angle_dEcorr;
      float dEcorr_R = DeltaEcal->reverseCal(id,Silicon[id]->Solution[isol].ide, dEcorr);

      Histo->AngleCorrDeltaE[id][chandE]->Fill(dEcorr);
      Histo->AngleCorrDeltaE_noCorr[id][chandE]->Fill(Silicon[id]->Solution[isol].denergy);
      Histo->AngleCorrDeltaE_R[id][chandE]->Fill(dEcorr_R);
      
      Histo->AngleCorrDeltaE_cal->Fill(id*Histo->channum + Silicon[id]->Solution[isol].ide, dEcorr);

      float Etot = Silicon[id]->Solution[isol].energy + Silicon[id]->Solution[isol].denergy;
      Histo->sumEtot_cal->Fill(id*Histo->channum + Silicon[id]->Solution[isol].ifront, Etot);
      Histo->AngleCorrSum_cal->Fill(id*Histo->channum + Silicon[id]->Solution[isol].ifront, Ecorr+dEcorr);

    }
  }

  //calculate and determine particle identification PID in the silicon
  int Pidmulti = 0;
  for (int id=0;id<4;id++) 
  {
    Pidmulti += Silicon[id]->getPID();
  }

  //hitmaps and other plots based on Pid
  for (int id=0;id<4;id++) 
  {
    for (int isol=0; isol<Silicon[id]->Nsolution; isol++)
    {
      float xpos = Silicon[id]->Solution[isol].Xpos;
      float ypos = Silicon[id]->Solution[isol].Ypos;
      //protons
      if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 1)
      {
        Histo->protonhitmap->Fill(xpos, ypos);
        Histo->dTime_proton->Fill(Silicon[id]->Solution[isol].timediff);
      }
      //deuterons
      if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 2)
      {
        Histo->deuteronhitmap->Fill(xpos, ypos);
        Histo->dTime_deuteron->Fill(Silicon[id]->Solution[isol].timediff);
      }
      //tritons
      if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 3)
      {
        Histo->tritonhitmap->Fill(xpos, ypos);
        Histo->dTime_triton->Fill(Silicon[id]->Solution[isol].timediff);
      }
      //alphas
      if (Silicon[id]->Solution[isol].iZ == 2 && Silicon[id]->Solution[isol].iA == 4)
      {
        Histo->alphahitmap->Fill(xpos, ypos);
        Histo->dTime_alpha->Fill(Silicon[id]->Solution[isol].timediff);
      }
      //He6
      if (Silicon[id]->Solution[isol].iZ == 2 && Silicon[id]->Solution[isol].iA == 6)
      {
        Histo->He6hitmap->Fill(xpos, ypos);
        Histo->dTime_He6->Fill(Silicon[id]->Solution[isol].timediff);
      }
      if (Silicon[id]->Solution[isol].iZ == 3)
      {
        Histo->Lihitmap->Fill(xpos, ypos);
        Histo->dTime_Li->Fill(Silicon[id]->Solution[isol].timediff);
      }
    }
  }

  //calc sumEnergy,then account for Eloss in target, then set Ekin and momentum of solutions
  //Eloss files are loaded in silicon
  for (int id=0;id<4;id++) 
  {
    Silicon[id]->calcEloss();
  }



  for (int id=0;id<4;id++) 
  {
    for (int isol=0; isol<Silicon[id]->Nsolution; isol++)
    {
      if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 1)
      {
        Histo->ProtonEnergy->Fill(Silicon[id]->Solution[isol].Ekin, Silicon[id]->Solution[isol].theta*180./pi);
      }
    }
  }






  //transfer Solution class to Correl
  Correl.reset();
  int goodMult = 0;
  for (int id=0;id<4;id++) 
  {
    for (int isol=0; isol<Silicon[id]->Nsolution; isol++)
    {
      //only keep solutions that have a pid
      if(Silicon[id]->Solution[isol].ipid)
      {
        Correl.load(&Silicon[id]->Solution[isol]);
        goodMult++;
      }
    }
  }

  if (goodMult >= 2)
  {

    //list all functions to look for correlations here
    corr_4He();
    corr_5He();
    corr_6He();
    corr_5Li();
    corr_6Li();
    corr_7Li();
    corr_6Be();
    corr_7Be();
    corr_8Be();
    corr_9B();
    

    //lots of Li6 but they don't come in with anything. Could be Li6 + n    
    if (goodMult == 2)
    {
      //cout << "goodMult=2" << endl;
      int pos = 0;
      int particlenum[2] = {0,0};
      for (int id=0;id<4;id++)
      {
        //cout << "id" << id << endl;
        for (int isol=0; isol<Silicon[id]->Nsolution; isol++)
        {
          //cout << "  isol" << isol << endl;
          int pidnum = 0;
          if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 1)
            pidnum = 1;
          if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 2)
            pidnum = 2;
          if (Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 3)
            pidnum = 3;
          if (Silicon[id]->Solution[isol].iZ == 2 && Silicon[id]->Solution[isol].iA == 3)
            pidnum = 4;
          if (Silicon[id]->Solution[isol].iZ == 2 && Silicon[id]->Solution[isol].iA == 4)
            pidnum = 5;
          if (Silicon[id]->Solution[isol].iZ == 2 && Silicon[id]->Solution[isol].iA == 6)
            pidnum = 6;
          if (Silicon[id]->Solution[isol].iZ == 3 && Silicon[id]->Solution[isol].iA == 6)
            pidnum = 7;
          if (Silicon[id]->Solution[isol].iZ == 3 && Silicon[id]->Solution[isol].iA == 7)
            pidnum = 8;
          
          //cout << "    pidnum " << pidnum << endl;
          if (pidnum > 0)
          {
            //cout << "    ????pidnum " << pidnum << endl;
            particlenum[pos] = pidnum;
            pos++;
          }

        }
      }
      //cout << "After    1=" << particlenum[0] << "   2=" << particlenum[1] << endl;
      Histo->CorrelationTable->Fill(particlenum[0],particlenum[1]);
    }
  }

  
  return true;
}

int Gobbi::match()
{
  //match dE, Efront, Eback as one hit
  int temp = 0;
  return temp;
}

float Gobbi::getEnergy(int board, int chan, int Ehigh)
{
  return Ehigh*(float)slopes[board][chan] + (float)intercepts[board][chan];
}




void Gobbi::corr_4He()
{
  // p+t
  if(Correl.proton.mult == 1 && Correl.H3.mult == 1)
  {
    float const Q4He = mass_alpha - (mass_p + mass_t);
    Correl.zeroMask();
    Correl.proton.mask[0]=1;
    Correl.H3.mask[0]=1;
    Correl.makeArray(1);

    float Erel_4He = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_4He - Q4He;

    Histo->Erel_4He_pt->Fill(Erel_4He);
    Histo->Ex_4He_pt->Fill(Ex);
    Histo->ThetaCM_4He_pt->Fill(thetaCM*180./acos(-1));
    Histo->VCM_4He_pt->Fill(Correl.velocityCM);

    if (Erel_4He > 4.0)
    {
      float xpos = Correl.frag[0]->Xpos;
      float ypos = Correl.frag[0]->Ypos;
      Histo->He4_p_hitmap->Fill(xpos, ypos);
      xpos = Correl.frag[1]->Xpos;
      ypos = Correl.frag[1]->Ypos;
      Histo->He4_t_hitmap->Fill(xpos, ypos);

      //fill in dE-E plots to select particle type
      float Ener = Correl.frag[0]->energy + Correl.frag[0]->denergy*(1-cos(Correl.frag[0]->theta));

      Histo->DEE_He4[Correl.frag[0]->itele]->Fill(Ener, Correl.frag[0]->denergy*cos(Correl.frag[0]->theta));

      Ener = Correl.frag[1]->energy + Correl.frag[1]->denergy*(1-cos(Correl.frag[1]->theta));
  
      Histo->DEE_He4[Correl.frag[1]->itele]->Fill(Ener, Correl.frag[1]->denergy*cos(Correl.frag[1]->theta));
    }

    Histo->Erel_pt_costhetaH->Fill(Erel_4He,Correl.cos_thetaH);
  }

  // d+d
  if(Correl.H2.mult == 2)
  {
    float const Q4He = mass_alpha - (mass_d + mass_d);
    Correl.zeroMask();
    Correl.H2.mask[0]=1;
    Correl.H2.mask[1]=1;
    Correl.makeArray(1);

    float Erel_4He = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_4He - Q4He;

    Histo->Erel_4He_dd->Fill(Erel_4He);
    Histo->Ex_4He_dd->Fill(Ex);
    Histo->ThetaCM_4He_dd->Fill(thetaCM*180./acos(-1));
    Histo->VCM_4He_dd->Fill(Correl.velocityCM);

    Histo->Erel_dd_costhetaH->Fill(Erel_4He,Correl.cos_thetaH);

  }




}

void Gobbi::corr_5He()
{
  // d+t
  if(Correl.H2.mult == 1 && Correl.H3.mult == 1)
  {
    //float const Q5Li = mass_5Li - (mass_p + mass_alpha);
    Correl.zeroMask();
    Correl.H2.mask[0]=1;
    Correl.H3.mask[0]=1;
    Correl.makeArray(1);

    float Erel_5He = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    //float Ex = Erel_5Li - Q5Li;

    Histo->Erel_5He_dt->Fill(Erel_5He);
    //Histo->Ex_5Li_pa->Fill(Ex);
    Histo->ThetaCM_5He_dt->Fill(thetaCM*180./acos(-1));
    Histo->VCM_5He_dt->Fill(Correl.velocityCM);
  }
}


void Gobbi::corr_6He()
{
  // t+t
  if(Correl.H3.mult == 2)
  {
    float const Q6He = mass_6He - (2*mass_t);
    Correl.zeroMask();
    Correl.H3.mask[0]=1;
    Correl.H3.mask[1]=1;
    Correl.makeArray(1);

    float Erel_6He = Correl.findErel();
    //cout << "6He " << Erel_6He << endl;
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_6He - Q6He;

    Histo->Erel_6He_tt->Fill(Erel_6He);
    Histo->Ex_6He_tt->Fill(Ex);
    Histo->ThetaCM_6He_tt->Fill(thetaCM*180./acos(-1));
    Histo->VCM_6He_tt->Fill(Correl.velocityCM);
  }
}

void Gobbi::corr_5Li()
{

  // p+He4
  if(Correl.proton.mult == 1 && Correl.alpha.mult == 1)
  {
 
    float const Q5Li = mass_5Li - (mass_p + mass_alpha);
    Correl.zeroMask();
    Correl.proton.mask[0]=1;
    Correl.alpha.mask[0]=1;
    Correl.makeArray(1);

    float Erel_5Li = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_5Li - Q5Li;

    Histo->Erel_5Li_pa->Fill(Erel_5Li);
    Histo->Ex_5Li_pa->Fill(Ex);
    Histo->ThetaCM_5Li_pa->Fill(thetaCM*180./acos(-1));
    Histo->VCM_5Li_pa->Fill(Correl.velocityCM);
  }

  // H2+He3
  if(Correl.H2.mult == 1 && Correl.He3.mult == 1)
  {

    float const Q5Li = mass_5Li - (mass_d + mass_3He);
    Correl.zeroMask();
    Correl.H2.mask[0]=1;
    Correl.He3.mask[0]=1;
    Correl.makeArray(1);

    float Erel_5Li = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_5Li - Q5Li;

    Histo->Erel_5Li_d3He->Fill(Erel_5Li);
    Histo->Ex_5Li_d3He->Fill(Ex);
    Histo->ThetaCM_5Li_d3He->Fill(thetaCM*180./acos(-1));
    Histo->VCM_5Li_d3He->Fill(Correl.velocityCM);
  }
}


void Gobbi::corr_6Li()
{
  // D+alpha
  if(Correl.H2.mult == 1 && Correl.alpha.mult == 1)
  {
    //cout << "inside corr_6Li_da()" << endl;
 
    float const Q6Li = mass_6Li - (mass_d + mass_alpha);
    Correl.zeroMask();
    Correl.H2.mask[0]=1;
    Correl.alpha.mask[0]=1;
    Correl.makeArray(1);

    float Erel_6Li = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_6Li - Q6Li;

    Histo->Erel_6Li_da->Fill(Erel_6Li);
    Histo->Ex_6Li_da->Fill(Ex);

    Histo->cos_thetaH_da->Fill(Correl.cos_thetaH);
    Histo->ThetaCM_6Li_da->Fill(thetaCM*180./acos(-1));
    Histo->VCM_6Li_da->Fill(Correl.velocityCM);
    Histo->VCM_vs_ThetaCM->Fill(thetaCM*180./acos(-1), Correl.velocityCM);
    
    if(fabs(Correl.cos_thetaH) < .3)
      Histo->Ex_6Li_da_trans->Fill(Ex);
    if(fabs(Correl.cos_thetaH) > .7)
      Histo->Ex_6Li_da_long->Fill(Ex);


    Histo->cos_da_thetaH->Fill(Correl.cos_thetaH);
    Histo->Erel_da_cosThetaH->Fill(Erel_6Li,Correl.cos_thetaH);

    if (Ex > 2 && Ex < 2.5)
    {
      //cout << "deut Ekin " << Correl.frag[0]->Ekin << endl;
      //cout << "alpha Ekin " << Correl.frag[1]->Ekin << endl;
      Histo->deutE_gate->Fill(Correl.frag[0]->Ekin);
      Histo->alphaE_gated->Fill(Correl.frag[1]->Ekin);

      Histo->deutE_gate_cosThetaH->Fill(Correl.frag[0]->Ekin,Correl.cos_thetaH);
      Histo->alphaE_gate_cosThetaH->Fill(Correl.frag[1]->Ekin,Correl.cos_thetaH);
    }

    for (int id=0;id<4;id++) 
    {
      for (int isol=0; isol<Silicon[id]->Nsolution; isol++)
      {
        //pick out
        if(Silicon[id]->Solution[isol].iZ == 1 && Silicon[id]->Solution[isol].iA == 2)
        {
          Silicon[id]->Solution[isol].iZ = 1;
          Silicon[id]->Solution[isol].iA = 3; //whoops now it is a triton
          Silicon[id]->Solution[isol].mass = Mass_t;

          //now we should redo energy loss calc          

          float sumEnergy = Silicon[id]->Solution[isol].denergy + Silicon[id]->Solution[isol].energy;
          float pc_before = sqrt(pow(sumEnergy+Silicon[id]->Solution[isol].mass,2) - pow(Silicon[id]->Solution[isol].mass,2));
          float velocity_before = pc_before/(sumEnergy+Silicon[id]->Solution[isol].mass);

          float thick = TargetThickness/2/cos(Silicon[id]->Solution[isol].theta);

          float ein = Silicon[id]->losses->getEin(sumEnergy,thick,Silicon[id]->Solution[isol].iZ,Silicon[id]->Solution[isol].mass/m0);

          Silicon[id]->Solution[isol].Ekin = ein;
          //calc momentum vector, energyTot, and velocity
          Silicon[id]->Solution[isol].getMomentum();

          //reload the solution in and it will be a missidentified triton
          //particle[3] is the H3_fake particle
          Correl.particle[3]->Sol[0] = &Silicon[id]->Solution[isol];
          Correl.particle[3]->mult++;
        }
      }
    }
    
    float const Q7Li = mass_7Li - (mass_t + mass_alpha);
    Correl.zeroMask();
    Correl.H2.mask[0]=0;
    Correl.H3_fake.mask[0]=1;
    Correl.alpha.mask[0]=1;   
    Correl.makeArray(1);
    float Erel_7Li = Correl.findErel();
    float Ex_7Li = Erel_7Li - Q7Li;
    //cout << "Erel " << Erel_7Li << endl;
    //cout << "Ex " << Ex_7Li << endl;
    Histo->Ex_7Li_ta_bad->Fill(Ex_7Li);


  }
}



void Gobbi::corr_7Li()
{
  // p + 6He
  if(Correl.proton.mult == 1 && Correl.He6.mult == 1)
  {
    //cout << "inside corr_7Li_he6p()" << endl;
 
    float const Q7Li = mass_7Li - (mass_p + mass_6He);
    //cout << Q7Li << endl;
    Correl.zeroMask();
    Correl.proton.mask[0]=1;
    Correl.He6.mask[0]=1;   
    Correl.makeArray(1);

    float Erel_7Li = Correl.findErel();
    float missingmass = Correl.missingmass();
    float getqvalue = Correl.Qvalue();
    float getelqvalue = Correl.Qvalue2();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_7Li - Q7Li;

    //angle between beam axis and the momentum vector between the center
    // of mass and the heavy fragment
    //float mag2 = 0;
    //for (int j=0;j<3;j++) mag2 += pow(Correl.frag[1]->MomCM[j],2);
    //float cosbeamCMtoHF = Correl.frag[1]->MomCM[2]/sqrt(mag2);
    //cout << "cosbeamCMtoHF " << cosbeamCMtoHF << endl;

    Histo->Erel_7Li_p6He->Fill(Erel_7Li);
    Histo->Erel_7Li_p6He_Q->Fill(Erel_7Li,getqvalue);
    Histo->Erel_7Li_p6He_lowres->Fill(Erel_7Li);

    if (abs(Correl.cos_thetaH) < 0.7)
    {
      Histo->Ex_7Li_p6He_transverse->Fill(Ex);
    }
    if (Correl.cos_thetaH > -0.7)
    {
      Histo->Ex_7Li_p6He_transverse2->Fill(Ex);
    }
    else  Histo->Erel_7Li_p6He_pFor->Fill(Erel_7Li);

    //frag[0] is always triton, frag[1] is always alpha
    // -2000 < dT_proton < 0 && -500 < dT_He6 < 200
    if (Correl.frag[0]->timediff < 0 && Correl.frag[0]->timediff > -1300) 
      if (Correl.frag[1]->timediff < 200 && Correl.frag[1]->timediff > -500)
        Histo->Ex_7Li_p6He_timegate->Fill(Ex);


    Histo->cos_thetaH->Fill(Correl.cos_thetaH);
    Histo->missingmass->Fill(missingmass-Mass_n);
    Histo->Qvalue->Fill(getqvalue);
    Histo->Qvalue2->Fill(getelqvalue);
    Histo->Erel_missingmass->Fill(Erel_7Li,missingmass-Mass_n);
    if (missingmass-Mass_n > -2 && missingmass-Mass_n < 2)
    {
      if (Correl.frag[0]->timediff < 0 && Correl.frag[0]->timediff > -2000) 
        if (Correl.frag[1]->timediff < 200 && Correl.frag[1]->timediff > -500)
          Histo->Ex_7Li_p6He_clean->Fill(Ex);
    } 

    Histo->Erel_7Li_cosThetaH->Fill(Erel_7Li,Correl.cos_thetaH);
    if (Erel_7Li < 1.)
    {
      Histo->cos_thetaH_lowErel->Fill(Correl.cos_thetaH);
      //Histo->DEE_shoulderevents->Fill(Correl.frag[0]->energy,Correl.frag[0]->denergy);
      //Histo->DEE_shoulderevents->Fill(Correl.frag[1]->energy,Correl.frag[1]->denergy);
    }
    if (Ex > 11.6 && Ex < 11.8)
    {
      Histo->hitmapcheck1->Fill(Correl.frag[0]->Xpos,Correl.frag[0]->Ypos);
      Histo->hitmapcheck2->Fill(Correl.frag[1]->Xpos,Correl.frag[1]->Ypos);
      Histo->DEE_shoulderevents->Fill(Correl.frag[0]->energy,Correl.frag[0]->denergy);
      Histo->DEE_shoulderevents->Fill(Correl.frag[1]->energy,Correl.frag[1]->denergy);
    }

    Histo->Ex_7Li_p6He_ExvsEp->Fill(Ex, Correl.frag[0]->energy);


    Histo->Ex_7Li_p6He->Fill(Ex);
    //cout << Ex << endl;
    Histo->ThetaCM_7Li_p6He->Fill(thetaCM*180./acos(-1));
    Histo->VCM_7Li_p6He->Fill(Correl.velocityCM);
    if(Erel_7Li < 2.)
    {
      Histo->VCM_7Li_p6He_lowErel->Fill(Correl.velocityCM);
    }

    Histo->dTime_7Li_proton->Fill(Correl.frag[0]->timediff);
    Histo->dTime_7Li_He6->Fill(Correl.frag[1]->timediff);
    

  }

  // H3 + He4
  if(Correl.H3.mult == 1 && Correl.alpha.mult == 1)
  {
    //cout << "inside corr_7Li_ta()" << endl;
 
    float const Q7Li = mass_7Li - (mass_t + mass_alpha);
    Correl.zeroMask();
    Correl.H3.mask[0]=1;
    Correl.alpha.mask[0]=1;   
    Correl.makeArray(1);

    float Erel_7Li = Correl.findErel();
    float Ex_tar = Correl.TargetEx();

    float thetaCM = Correl.thetaCM;
    float Ex = Erel_7Li - Q7Li;

    Histo->Erel_7Li_ta->Fill(Erel_7Li);
    Histo->Ex_7Li_ta->Fill(Ex);
    Histo->ThetaCM_7Li_ta->Fill(thetaCM*180./acos(-1));
    Histo->VCM_7Li_ta->Fill(Correl.velocityCM);

    if(fabs(Correl.cos_thetaH) < .5)
      Histo->Ex_7Li_ta_trans->Fill(Ex);
    if(fabs(Correl.cos_thetaH) > .7)
      Histo->Ex_7Li_ta_long->Fill(Ex);

    Histo->cos_ta_thetaH->Fill(Correl.cos_thetaH);
    Histo->Erel_ta_cosThetaH->Fill(Erel_7Li,Correl.cos_thetaH);
    Histo->Ex_tar->Fill(Ex_tar);
    Histo->Erel_vs_Extar->Fill(Erel_7Li,Ex_tar);



    //frag[0] is always proton, frag[1] is always He6
    // -2000 < dT_proton < 0 && -500 < dT_He6 < 200
    if (Correl.frag[0]->timediff < 0 && Correl.frag[0]->timediff > -2000) 
      if (Correl.frag[1]->timediff < 200 && Correl.frag[1]->timediff > -500)
        Histo->Ex_7Li_ta_timegate->Fill(Ex);

    //if (Ex > 3.5 && Ex < 4.3)
    //{
    //  Histo->hitmapcheck1->Fill(Correl.frag[0]->Xpos,Correl.frag[0]->Ypos);
    //  Histo->hitmapcheck2->Fill(Correl.frag[1]->Xpos,Correl.frag[1]->Ypos);
    //  //Histo->DEE_shoulderevents->Fill(Correl.frag[0]->energy,Correl.frag[0]->denergy);
    //  //Histo->DEE_shoulderevents->Fill(Correl.frag[1]->energy,Correl.frag[1]->denergy);
    //}
    Histo->dTime_7Li_triton->Fill(Correl.frag[0]->timediff);
    Histo->dTime_7Li_alpha->Fill(Correl.frag[1]->timediff);

    if (Correl.frag[0]->itele != Correl.frag[1]->itele)
    {
      Histo->seperate_quad_Ex_7Li_ta->Fill(Ex);
    }


  }
}



void Gobbi::corr_7Be()
{
  // He4+He4
  if(Correl.He3.mult == 1 && Correl.alpha.mult == 1)
  {
    float const Q7Be = mass_7Be - (mass_3He + mass_alpha);
    Correl.zeroMask();
    Correl.He3.mask[0]=1;
    Correl.alpha.mask[0]=1;
    Correl.makeArray(1);

    float Erel_7Be = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_7Be - Q7Be;

    Histo->Erel_7Be_a3He->Fill(Erel_7Be);
    Histo->Ex_7Be_a3He->Fill(Ex);
    Histo->ThetaCM_7Be_a3He->Fill(thetaCM*180./acos(-1));
    Histo->VCM_7Be_a3He->Fill(Correl.velocityCM);
  }
  // p+Li6
  if(Correl.proton.mult == 1 && Correl.Li6.mult == 1)
  {
    float const Q7Be = mass_7Be - (mass_p + mass_6Li);
    Correl.zeroMask();
    Correl.proton.mask[0]=1;
    Correl.Li6.mask[0]=1;
    Correl.makeArray(1);

    float Erel_7Be = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_7Be - Q7Be;

    Histo->Erel_7Be_p6Li->Fill(Erel_7Be);
    Histo->Ex_7Be_p6Li->Fill(Ex);
    Histo->ThetaCM_7Be_p6Li->Fill(thetaCM*180./acos(-1));
    Histo->VCM_7Be_p6Li->Fill(Correl.velocityCM);
  }
}




void Gobbi::corr_8Be()
{
  // He4+He4
  if(Correl.alpha.mult == 2)
  {
    //cout << "inside corr_8Be_aa()" << endl;
 
    float const Q8Be = mass_8Be - (2*mass_alpha);
    Correl.zeroMask();
    Correl.alpha.mask[0]=1;
    Correl.alpha.mask[1]=1;
    Correl.makeArray(1);

    float Erel_8Be = Correl.findErel();


    float thetaCM = Correl.thetaCM;
    float Ex = Erel_8Be - Q8Be;

    Histo->Erel_8Be_aa->Fill(Erel_8Be);
    Histo->Ex_8Be_aa->Fill(Ex);

    Histo->Erel_aa_cosThetaH->Fill(Erel_8Be,Correl.cos_thetaH);
    if (Ex < 0.1)
    {
      Histo->ThetaCM_8Be_aa->Fill(thetaCM*180./acos(-1));
      Histo->VCM_8Be_aa->Fill(Correl.velocityCM);
    }

  }

  // p + Li7
  if(Correl.proton.mult == 1 && Correl.Li7.mult == 1)
  {
    //cout << "inside corr_6Li_da()" << endl;
 
    float const Q8Be = mass_8Be - (mass_p + mass_7Li);
    Correl.zeroMask();
    Correl.proton.mask[0]=1;
    Correl.Li7.mask[0]=1;
    Correl.makeArray(1);

    float Erel_8Be = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_8Be - Q8Be;

    Histo->Erel_8Be_p7Li->Fill(Erel_8Be);
    Histo->Ex_8Be_p7Li->Fill(Ex);
    if(fabs(Correl.cos_thetaH) < .5)
      Histo->Ex_8Be_p7Li_trans->Fill(Ex);

    Histo->ThetaCM_8Be_p7Li->Fill(thetaCM*180./acos(-1));
    Histo->VCM_8Be_p7Li->Fill(Correl.velocityCM);
    Histo->cos_p7Li_thetaH->Fill(Correl.cos_thetaH);
    Histo->Erel_p7Li_cosThetaH->Fill(Erel_8Be,Correl.cos_thetaH);


    //look for proton and Li7 cuttoffs
    Histo->ProtonEnergies_p7Li->Fill(Correl.frag[0]->Ekin);
    Histo->LithiumEnergies_p7Li->Fill(Correl.frag[1]->denergy);

    //cout << Correl.frag[0]->Ekin << " " << Correl.frag[1]->Ekin << endl;
    
    Histo->dTime_8Be_proton->Fill(Correl.frag[0]->timediff);
    Histo->dTime_8Be_Li7->Fill(Correl.frag[1]->timediff);


    if (Correl.frag[0]->timediff < 0 && Correl.frag[0]->timediff > -1800)
      if (Correl.frag[1]->timediff < 200 && Correl.frag[1]->timediff > -550)
        Histo->Ex_8Be_p7Li_timegate->Fill(Ex);


  }

  //p+t+a
  if(Correl.proton.mult == 1 && Correl.H3.mult == 1 && Correl.alpha.mult == 1)
  {
    float const Q8Be = mass_8Be - (mass_p + mass_t + mass_alpha);
    Correl.zeroMask();
    Correl.proton.mask[0]=1;
    Correl.H3.mask[0]=1;
    Correl.alpha.mask[0]=1;
    Correl.makeArray(1);

    float Erel_8Be = Correl.findErel();
    float thetaCM = Correl.thetaCM;
    float Ex = Erel_8Be - Q8Be;

    Histo->Erel_8Be_pta->Fill(Erel_8Be);
    Histo->Ex_8Be_pta->Fill(Ex);
    if(fabs(Correl.cos_thetaH) < .5)
      Histo->Ex_8Be_pta_trans->Fill(Ex);
    Histo->ThetaCM_8Be_pta->Fill(thetaCM*180./acos(-1));
    Histo->VCM_8Be_pta->Fill(Correl.velocityCM);

    Histo->cos_pta_thetaH->Fill(Correl.cos_thetaH);
    Histo->Erel_pta_cosThetaH->Fill(Erel_8Be,Correl.cos_thetaH);
  }

  //t+a (missing p)
  if(Correl.proton.mult == 1 && Correl.H3.mult == 1 && Correl.alpha.mult == 1)
  {
    float const Q7Li = mass_7Li - (mass_t + mass_alpha);
    Correl.zeroMask();
    Correl.H3.mask[0]=1;
    Correl.alpha.mask[0]=1;   
    Correl.makeArray(1);

    float Erel_7Li = Correl.findErel();

    float thetaCM = Correl.thetaCM;
    float Ex = Erel_7Li - Q7Li;

    Histo->Erel_7Li_ta_fake->Fill(Erel_7Li);
    Histo->Ex_7Li_ta_fake->Fill(Ex);


    if (Ex < 5 && Ex > 4.4)
    {

      float const Q8Be = mass_8Be - (mass_p + mass_t + mass_alpha);
      Correl.zeroMask();
      Correl.proton.mask[0]=1;
      Correl.H3.mask[0]=1;
      Correl.alpha.mask[0]=1;
      Correl.makeArray(1);

      float Erel_8Be = Correl.findErel();
      float thetaCM = Correl.thetaCM;
      float Exreal = Erel_8Be - Q8Be;

      Histo->Ex_8Be_7LiGate->Fill(Exreal);
    }

  }
}


void Gobbi::corr_9B()
{
  // He4+He4+p
  if(Correl.alpha.mult == 2 && Correl.proton.mult == 1)
  {
    //cout << "inside corr_8Be_aa()" << endl;
 
    float const Q9B = mass_9B - (2*mass_alpha) - mass_p;
    Correl.zeroMask();
    Correl.alpha.mask[0]=1;
    Correl.alpha.mask[1]=1;
    Correl.proton.mask[0]=1;
    Correl.makeArray(1);

    float Erel_9B = Correl.findErel();


    float thetaCM = Correl.thetaCM;
    float Ex = Erel_9B - Q9B;

    Histo->Erel_9B_paa->Fill(Erel_9B);
    Histo->Ex_9B_paa->Fill(Ex);
    Histo->ThetaCM_9B_paa->Fill(thetaCM*180./acos(-1));
    Histo->VCM_9B_paa->Fill(Correl.velocityCM);

    Correl.zeroMask();
    Correl.alpha.mask[0]=true;
    Correl.alpha.mask[1]=true;
    Correl.proton.mask[0] = false;
    Correl.makeArray(true);


    float Erel_8Be = Correl.findErel();

    float Ex_8Be = Erel_8Be + 2.*mass_alpha - mass_8Be;
    Histo->Ex_9B_aa->Fill(Ex_8Be);
    if (fabs(Ex_8Be) < .1) Histo->Ex_9B_p8Be->Fill(Ex);
  }
}

void Gobbi::corr_6Be()
{
  // He4+He4+p
  if(Correl.alpha.mult == 1 && Correl.proton.mult == 2)
  {

 

    Correl.zeroMask();
    Correl.alpha.mask[0]=1;
    Correl.proton.mask[0]=1;
    Correl.proton.mask[1]=1;
    Correl.makeArray(1);

    float Erel_6Be = Correl.findErel();


    float thetaCM = Correl.thetaCM;

    Histo->Erel_6Be_2pa->Fill(Erel_6Be);
    Histo->ThetaCM_6Be_2pa->Fill(thetaCM*180./acos(-1));
    Histo->VCM_6Be_2pa->Fill(Correl.velocityCM);

  }
}
