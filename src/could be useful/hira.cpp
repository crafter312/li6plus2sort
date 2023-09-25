#include "hira.h"


hira::hira(TRandom * ran0, histo_sort * Histo0)
{
  type = 0;
  Histo = Histo0;
  ran = ran0;
  init();
}

hira::hira(TRandom * ran0, histo_read * Histo1)
{
  type = 1;
  Histo_read = Histo1;
  ran = ran0;
  init();
}


hira::hira(TRandom * ran0, histo_sort * Histo0, histo_read * Histo1)
{
  type = 2;
  Histo_read = Histo1;
  Histo = Histo0;
  ran = ran0;
  init();
}


void hira::init()
{
 
  xmarker[0]=0x1ff3;

  TDC =  new TDC1190(3,20,128); //registers 3 hits, reference channel 33, 128 total channels

  // //make map of chips
//  ifstream ifile("datfiles/S4Rings_new.txt");
  ifstream ifile("datfiles/S4Rings.txt");

  if (!ifile.is_open())
  {
    cout << "Ring map not found" << endl;
    abort();
  }
  string name;
  getline(ifile,name);
  int CB,Ch,iR;
  for (;;)
  {
    ifile >> CB >> Ch >> iR;
    if (ifile.eof()) break;
    if (ifile.bad()) break;

    RingMap[CB-5][Ch] = iR;
  }
  ifile.close();
  ifile.clear();
//  ifstream ifile2("datfiles/S4Pies.txt");
  ifstream ifile2("datfiles/S4Pies_old.txt");

  if (!ifile2.is_open())
  {
    cout << "Pie map not found" << endl;
    abort();
  }

  getline(ifile2,name);
  int iP;
  for (;;)
  {
    ifile2 >> CB >> Ch >> iP;
    if (ifile2.eof()) break;
    if (ifile2.bad()) break;

    PieMap[CB-1][Ch] = iP;
    //cout << "iP = " << iP << " " << CB << " " << Ch << endl;
  }
  ifile2.close();
  ifile2.clear();

  // ifstream ifile("datfiles/chipmap.dat");

  // if (!ifile.is_open())
  //   {
  //     cout << "chip map not found" << endl;
  //     abort();
  //   }

  // string name;
  // getline(ifile,name);
  // int i1,iMB,cbf,cbb,cbfl,cbbl;
  // for (;;)
  //   {
  //     ifile >> i1 >> iMB >> cbf>> cbb;
  //     if (ifile.eof()) break;
  //     if (ifile.bad()) break;

  //     iMB -= 1;
  //     Map[iMB][cbf].front = true;
  //     Map[iMB][cbf].itele = i1;

  //     Map[iMB][cbb].front = false;
  //     Map[iMB][cbb].itele = i1;
 
  //   }
  // ifile.close();
  // ifile.clear();


  /*
  ifstream csimap("datfiles/csimap.dat");
  if(!csimap.is_open())
    {
      cout << "Could not find CsI mapping file" << endl;
      abort();
    }

  getline(csimap,name);

  int itel,ich,icsi;
  for(;;)
    {
      csimap >> itel >> ich >> icsi;
      if(csimap.eof()) break;
      if(csimap.bad()) break;
      //      cout << itel << " " << ich << " " << icsi <<endl;
      CsIMap[icsi].itele = itel;
      CsIMap[icsi].iCsi = ich;
    }
  csimap.close();
  csimap.clear();
  */  


  //read in calibrations
  int Ntele = 1;
  int Nstrip = 128;

  name = "cal/Pies.cal";
  calPies = new calibrate(Ntele,Nstrip,name,1);

  name = "cal/Rings.cal";
  calRings = new calibrate(Ntele,Nstrip,name,1);


  name = "cal/p_calibrations.cal";
  //name = "cal/Proton_New_ang.cal";
  calCsi = new calibrate(1,20,name,1);


  float dist2Si = 30.91; //cm for Ar30 run
  RingCounter = new ringCounter(ran,Histo,dist2Si);
  XY_mon = new XYmon(dist2Si);
  
  // Telescope = new telescope*[14];
  // for (int i=0;i<14;i++)
  //   {
  //     Telescope[i] = new telescope(ran,i,Histo_read);
      
  //     Telescope[i]->load(0, 0, 15,15,
  //              16,16,31,31,
  //              0, 15,15,0,
  //              16,31,31,16);
      

  //   }

  NHira = 0;
  Nfiber = 0;

  //high low correlations zero arrays
  for (int i=0;i<14;i++)
  {
    for (int j=0;j<32;j++)
    {
      fsumN[i][j] = 0.;
      fsumx[i][j] = 0.;
      fsumxx[i][j] = 0.;
      fsumyx[i][j] = 0.;
      fsumy[i][j] = 0.;
  
      bsumN[i][j] = 0.;
      bsumx[i][j] = 0.;
      bsumxx[i][j] = 0.;
      bsumyx[i][j] = 0.;
      bsumy[i][j] = 0.;
    }
  }

}
//*************************************************************//
//bool hira::unpack(unsigned short *&point,int runno) -- SG 2020/11/03
bool hira::unpack(unsigned short *&point, unsigned short *&tdcpoint, int runno)
{
  bool stat = true;
  stat = unpackSi_HINP4(point);
  if (!stat)
  {
    cout << "Bad hira" << endl;
    return stat;
  }
  NHira++;

  stat = unpackCsi(point,tdcpoint,runno);

  if (!stat)
  {
    //cout << "Bad CsI" << endl;
    return stat;
  }
  stat = unpackFiber(point);
  if(stat)
    Nfiber++;
  else
  {
    //cout << "bad Fiber" << endl;
  }

  return stat;
}
//************************************************************
bool hira::unpackFiber(unsigned short *&point)
{

  unsigned short f3 = *point;
  unsigned short f4 = *(point+1);

  // check to see if there in no data first
  if (f3 == 0xffff && f4 == 0xffff)
  {
    point += 2;
    return false;
  }
  //suck out qdc info
  ADC.number = 0;
  point = ADC.read(point);

  for (int i=0;i<ADC.number;i++)
  {
    if (ADC.underflow[i]) continue;
    if (ADC.overflow[i]) continue;
    int id = ADC.channel[i];
    int ienergy = ADC.data[i];
    
    if (id == 0) XY_mon->horz->dynode = ienergy + ran->Rndm();
    else if (id == 1) XY_mon->horz->A = ienergy + ran->Rndm() - 360;// - 139.;
    else if (id == 2) XY_mon->horz->B = ienergy + ran->Rndm() - 360;// - 110.;
    else if (id == 3) XY_mon->horz->C = ienergy + ran->Rndm() - 380;// - 128.;
    else if (id == 4) XY_mon->horz->D = (ienergy + ran->Rndm() - 370)*1.3;// - 108.;
    //the slight gain adjust on horz->D helps with straightening the HorzHitMap

    else if (id == 5) XY_mon->vert->dynode = ienergy + ran->Rndm();
    else if (id == 6) XY_mon->vert->A = ienergy + ran->Rndm() - 325; // - 138.;
    else if (id == 7) XY_mon->vert->B = ienergy + ran->Rndm() - 375; //- 147.;
    else if (id == 8) XY_mon->vert->C = ienergy + ran->Rndm() - 345; //- 159.;
    else if (id == 9) XY_mon->vert->D = ienergy + ran->Rndm() - 365; //- 142.;
    
  }

  XY_mon->make_2d();

  if (XY_mon->vert->has_data)
  {
    Histo->VertA->Fill(XY_mon->vert->A);
    Histo->VertB->Fill(XY_mon->vert->B);
    Histo->VertC->Fill(XY_mon->vert->C);
    Histo->VertD->Fill(XY_mon->vert->D);
    Histo->VertDyn->Fill(XY_mon->vert->dynode);
    Histo->VertHitMap->Fill(XY_mon->vert->pmtx,XY_mon->vert->pmty);
    Histo->Fiber_Y->Fill(XY_mon->y);
    Histo->Fiber_Yid->Fill(XY_mon->vert->posID);
  }

  if (XY_mon->horz->has_data)
  {
    Histo->HorzA->Fill(XY_mon->horz->A);
    Histo->HorzB->Fill(XY_mon->horz->B);
    Histo->HorzC->Fill(XY_mon->horz->C);
    Histo->HorzD->Fill(XY_mon->horz->D);
    Histo->HorzDyn->Fill(XY_mon->horz->dynode);
    Histo->HorzHitMap->Fill(XY_mon->horz->pmtx,XY_mon->horz->pmty);
    if(XY_mon->horz->dynode > 300.)
      Histo->HorzHitMap_energygate->Fill(XY_mon->horz->pmtx,XY_mon->horz->pmty);

    Histo->DynodevsSum->Fill(XY_mon->horz->dynode,XY_mon->horz->total);
    Histo->AvsSum->Fill(XY_mon->horz->A,XY_mon->horz->total);
    Histo->BvsSum->Fill(XY_mon->horz->B,XY_mon->horz->total);
    Histo->CvsSum->Fill(XY_mon->horz->C,XY_mon->horz->total);
    Histo->DvsSum->Fill(XY_mon->horz->D,XY_mon->horz->total);
    Histo->Fiber_X->Fill(XY_mon->x);
    Histo->Fiber_Xid->Fill(XY_mon->horz->posID);
  }
  
  if(XY_mon->has_data)
  {
    Histo->Fiber_XY->Fill(XY_mon->x,XY_mon->y);
    Histo->Fiber_XYid->Fill(XY_mon->horz->posID,XY_mon->vert->posID);
  }
  
  //check for ffff's
  unsigned short f1 = *point;
  point++;
  unsigned short f2 = *point;
  point++;
  if(f1 != 0xffff && f2 != 0xffff)
  {
    cout << "didnt read right" <<endl;
    return false;
  }
  
  return true;
}

//*************************************************************//
bool hira::unpackCsi(unsigned short *&point, unsigned short *&tdcpoint, int runno)
{
  NE = 0;
  CsIM = 0;

  for (int iadc=0;iadc<1;iadc++)
  {
    //check for ffff's
    unsigned short f3 = *point;
    unsigned short f4 = *(point+1);

    if (f3 == 0xffff && f4 == 0xffff)
    {
      point += 2;
      continue;
    }
    ADC.number = 0;
    point = ADC.read(point);  // suck out the info in the qdc
    
    for (int i=0;i<ADC.number;i++)
    {
      if (ADC.underflow[i]) continue;
      if (ADC.overflow[i]) continue;
          int id = ADC.channel[i] + 32*iadc;
          int ienergy = ADC.data[i];
      //cout << id << " " << ienergy << endl;
      
      if(id < 20)
      {
        int iCsi = id; //CsIMap[id].iCsi;
        //int itele = CsIMap[id].itele;

        float energy = calCsi->getEnergy(0,id,ienergy+ran->Rndm());
        //   cout << "ID = " << id << endl;
        //          cout << id << " " << iCsi << endl;
        
        DataE[NE].id = iCsi;
        DataE[NE].ienergy = ienergy;
        DataE[NE].energy = energy;
        Histo->ECsI[iCsi]->Fill(ienergy);
        Histo->ECsISum->Fill(iCsi,ienergy);
        Histo->ECsICSum->Fill(iCsi,energy);

        /*
        if (ienergy > 20)
        {

          RingCounter->Csi.Add(DataE[NE].id,0.,0.,DataE[NE].ienergy,0.);
          CsIM++;
          //Telescope[itele]->Csi.Add(icsi,energy,0.,DataE[NE].ienergy,0.);
        }
        */
        //cout << Telescope[itele]->Csi.Order[0].strip;
        //cout << " " << Telescope[itele]->Csi.Order[0].energy << endl;
        
        NE++;
      }
    }
  } //end loop over iadc
  
  //check for ffff's
  unsigned short f1 = *point++;
  unsigned short f2 = *point++;

  if(f1 != 0xffff && f2 != 0xffff)
  {
    cout << "didnt read Ecsi right" <<endl;
    return false;
  }

  NT = 0;
  for (int itdc = 0;itdc<1;itdc++)
  {
    //check for ffff's
    unsigned short f3 = *point;
    unsigned short f4 = *(point+1);
    
    if (f3 == 0xffff && f4 == 0xffff) 
    {
      cout << "no Csi TDC" << endl;
      point += 2;
      continue;
    }
    // TDC.number = 0;
    // point = TDC.read(point);  // suck out the info in the qdc
    // for (int i=0;i<TDC.number;i++)
    //     {


    //       if (TDC.underflow[i]) continue;
    //       if (TDC.overflow[i]) continue;
    //     int id = TDC.channel[i] + 32*itdc;
    //     int itime = TDC.data[i];
    //       if (id < 56)
    //         {
    //           DataT[NT].id = id;
    //           DataT[NT].itime = itime;
    //           Histo->TCsI[id]->Fill(itime);
    //           NT++;
    //         }
    //     }
    //      cout << "TDC being called" << endl;

    // S Gillespie 2020/11/03
    // Getting start of TDC point here
    tdcpoint = point;
    point = TDC->read(point);  // suck out the info in the tdc

    for (int i=0;i<TDC->Ndata;i++)
    {
      int id = TDC->dataOut[i].channel;
      int itime = TDC->dataOut[i].time;

      if (id < 20)
      {
        DataT[NT].id = id;
        DataT[NT].itime = itime;
        Histo->TCsI[id]->Fill(itime/10.);
        
        NT++;
      }
      else if (id == 65)
      {
        T_RFSCIN = itime;
        //Histo->T_RFSCNI->Fill(itime/10.);
      }
      else if (id == 66)
      {
        T_A1900 = itime;
        Histo->T_A1900->Fill(itime/10.);
      }
      else if (id == 67)
      {
        T_RFCYC = itime;
        Histo->T_RFCYC->Fill(itime/10.);
      }
      else if (id == 27)
      {
        T_CsiTrig = itime;
      }

    }
    
    //check for ffff's
    unsigned short f1 = *point;
    point++;
    unsigned short f2 = *point;
    point++;
    if(f1 != 0xffff && f2 != 0xffff)
    {
      cout << "did not read CsI TDC correctly" << endl;
      return false;
    }
  }

  int Nfound = 0;
  int Nnotfound = 0;
  // match up energies to times
  for (int ie=0;ie<NE;ie++)
  {
    DataE[ie].itime = -1;
    bool found = false;
    for (int it=0;it<NT;it++)
    {
      if (DataE[ie].id == DataT[it].id )           //we have matched
      {
        found  = true;
        DataE[ie].itime = DataT[it].itime;
        //          int itele = DataE[ie].id/4;
        int icsi = DataE[ie].id;
        if(DataE[ie].energy >Csi_energy_min)
        { // && DataE[ie].itime > 500 && DataE[ie].itime < 1500)
          RingCounter->Csi.Add(icsi,DataE[ie].energy,0.,
                               DataE[ie].ienergy,DataE[ie].itime);

          Histo->ET_csi[icsi]->Fill(DataE[ie].ienergy,DataE[ie].itime/10.);
          //cout << "filling " << DataE[ie].ienergy << " " << DataE[ie].itime/10. << endl;
          Nfound++;
        }
      }
    }

    //if (!found)
      //{
      //  if(DataE[ie].energy >Csi_energy_min)
    //  {
    //    int icsi = DataE[ie].id;
    //    Nnotfound++;
        //          Telescope[itele]->Csi.Add(icsi,DataE[ie].energy,0.,DataE[ie].ienergy,0.);
        
    //    RingCounter->Csi.Add(icsi,DataE[ie].energy,0.,DataE[ie].ienergy,0.);
    //  }
      //}
  }

  /*
  if(Nfound+Nnotfound != NE)
  cout << "NE= " << NE << " " << Nfound+Nnotfound << " " << Nfound << " " << Nnotfound << " " <<  RingCounter->Csi.Nstore << endl;
  */

  bool stat = true;
  return stat;
}
// //***************************************************************//
// //unpacking the XLM with internal ADC's
// bool hira::unpackSi_adc(unsigned short *&point)
// {
  
//   /*
//     unsigned short * pig = point;
//     for (int i=0;i<100;i++)
//     {
//     cout << dec<< *pig << " " << hex<< *pig << endl;
//     pig++;
//     }
//     abort();
//   */




//   unsigned short marker;
//   unsigned short * nextMB = point;
//   for (int iMB=0;iMB<2;iMB++)
//     {
//       point = nextMB;
//       cout << iMB << " " << hex <<*point;
//       marker = *point++;
//       //if(marker !=xmarker[iMB])point++;
//       if (marker != xmarker[iMB]) return false;
      
//       int NstripsRead = 0;
//       unsigned short chipWords = *point;
//       nextMB = point + chipWords + 2;
//       //if (chipWords > 400) return false;  // please fix Kyle
//       if (chipWords == 0)
//         {
//       NstripsRead = 0;
//       return (bool) 1;
//         }
//       point += 2;
//       NstripsRead = *point;

//       if (NstripsRead > 384) return false; // bad buffer
//       point += 5;

//       //  cout << chipWords << " " << NstripsRead*3+7 <<  " " << NstripsRead <<  endl;
//       for (int istrip = 0;istrip < NstripsRead;istrip++)
//         {

//           unsigned short id = *point++;
//           unsigned short chipNum = (id&0x1FE0)>>5;
//           unsigned short chanNum = id & 0x1F;
//           unsigned short ienergy = *point++;
//       unsigned short ilowenergy = *point++;
//           unsigned short itime =  *point++;



//           unsigned short underOver = 0;
         

//           if (chipNum%2 == 0)
//         {
//           chanNum = 31- 2*chanNum-1;
//           chipNum /= 2;
//         }
//       else
//         {
//           chipNum = chipNum/2 + 1;
//           if(iMB !=2) chanNum = 31 - 2*chanNum;
//         }
      
//       //  cout << id << " " << chipNum << " " << chanNum << " " << ienergy << " " << itime << " " << iMB << endl;
//           bool bfront = Map[iMB][chipNum].front;
//           bool bA = Map[iMB][chipNum].A;
//           int itele = Map[iMB][chipNum].itele - 1;



//       //if (iMB == 0) cout << itele << " " << chipNum << endl;

//       if(itele <0)     return false;
//           bool bhigh = true; 

//       if (iMB == 2)
//         {
//           bhigh = true;

//               if (!bA) chanNum += 16;

//         }

//       if(bfront) chanNum = 31- chanNum;


//       if (chanNum > 31)
//         {
//           cout << "chanNum too big" << endl;
//               return false;
//         }
//           if (chipNum > 12)
//         {
//           cout << "chipNum too big " << chipNum << endl;
//               return false;
//         }


//       if (bhigh && bfront)
//         {

//           float energy = calFront->getEnergy(itele,chanNum,ienergy+ran->Rndm());
//           //if(itele ==6 || itele ==7) energy = energy*1.05; //Second Run!!!!!!


//           float time = calFrontT->getEnergy(itele,chanNum,itime+ran->Rndm());
//           Histo->EFTSum[itele]->Fill(chanNum,time);
//           //          Histo->SiFTime->Fill(time);

//           //Recalibrating
//           if(itele ==6 || itele ==7)
//                energy = calrecal->getEnergy(itele-6,chanNum,energy);

//           Histo->EfrontR[itele][chanNum]->Fill(ienergy);
//           Histo->TfrontR[itele][chanNum]->Fill(time);
//           Histo->EfrontC[itele][chanNum]->Fill(energy);
//           //Histo->EFSum[itele]->Fill(chanNum,ienergy);
//           // Histo->EFCSum[itele]->Fill(chanNum,energy);

//               if(energy >0.75)
//         {
//           Telescope[itele]->Front.Add(chanNum,underOver,energy,ienergy,time);
//         }
        
//         }
//       if (bhigh && !bfront)
//         {
//           float energy = calBack->getEnergy(itele,chanNum,ienergy+ran->Rndm());
//           float time = calBackT->getEnergy(itele,chanNum,itime+ran->Rndm());
//           Histo->EBTSum[itele]->Fill(chanNum,time);
//           Histo->SiBTime->Fill(time);




//           Histo->EbackR[itele][chanNum]->Fill(ienergy);
//           Histo->TbackR[itele][chanNum]->Fill(time);
//           Histo->EbackC[itele][chanNum]->Fill(energy);
//           //Histo->EBSum[itele]->Fill(chanNum,ienergy);
//           // Histo->EBCSum[itele]->Fill(chanNum,energy);

//           if(energy > 0.75)
//         Telescope[itele]->Back.Add(chanNum,underOver,energy,ienergy,time);

//         }

//       if(bhigh && bfront)
//         {
//           for (int i=0;i<Telescope[itele]->Front.Nstore;i++)
//         {
//                   if (Telescope[itele]->Front.Order[i].strip == chanNum)
//             Telescope[itele]->Front.Order[i].energylow = 0.;

//         }
//         }

//       if(bhigh && !bfront)
//         {
//           for (int i=0;i<Telescope[itele]->Back.Nstore;i++)
//         {
//                   if (Telescope[itele]->Back.Order[i].strip == chanNum)
//             Telescope[itele]->Back.Order[i].energylow = 0.;
//         }
//         }


//       if (!bhigh && bfront)
//         {

//           float time = calFrontTLG->getEnergy(itele,chanNum,itime+ran->Rndm());
//           Histo->SiFTime->Fill(time);

//           Histo->EfrontLGR[itele][chanNum]->Fill(ienergy);
//           Histo->TfrontLG[itele][chanNum]->Fill(time);
//           //Histo->EFLSum[itele]->Fill(chanNum,ienergy);

//           float energy = calLFront->getEnergy(itele-6,chanNum,ienergy+ran->Rndm());
//           energy = calFront->getEnergy(itele,chanNum,energy+ran->Rndm());
//           Histo->EfrontLGC[itele][chanNum]->Fill(energy);

//           //Recalibrating
//           if(itele ==6 || itele ==7)
//         energy = calrecal->getEnergy(itele-6,chanNum,energy);


//           for (int i=0;i<Telescope[itele]->Front.Nstore;i++)
//         {
//                   if (Telescope[itele]->Front.Order[i].strip == chanNum)
//             {
//               Telescope[itele]->Front.Order[i].energyRlow = ienergy;
//               Telescope[itele]->Front.Order[i].energylow = energy;
//                       double ienergyHigh=Telescope[itele]->Front.Order[i].energyR;
//                       if (ienergyHigh < 15000 && ienergy > 50 && ienergy <15000)
//             {
//               float Ratio = 0.;
//                           fsumN[itele-6][chanNum]++;
//                           fsumx[itele-6][chanNum] += (double)ienergy;
//                           fsumxx[itele-6][chanNum] += pow((double)ienergy,2);
//                           fsumy[itele-6][chanNum] += ienergyHigh;
//                           fsumyx[itele-6][chanNum] += ienergyHigh*(double)ienergy;
//             }
                      
//               break;
//             }
//         }
          
//         }
      
      
//       if (!bhigh && !bfront)
//         {

//           float time = calBackTLG->getEnergy(itele,chanNum,itime+ran->Rndm());
//           Histo->SiBTime->Fill(time);
//           Histo->EbackLGR[itele][chanNum]->Fill(ienergy);
//           Histo->TbackLG[itele][chanNum]->Fill(time);
//               float energy = ienergy;
//           energy = calBDual->getEnergy(itele-6,chanNum,ienergy+ran->Rndm());
//           if(ienergy > energy)energy = ienergy;
//           float corenergy = energy;
//           Histo->EbackLGCC[itele][chanNum]->Fill(energy);
//           energy = calLBack->getEnergy(itele-6,chanNum,energy+ran->Rndm());
//           energy = calBack->getEnergy(itele,chanNum,energy+ran->Rndm());
//           Histo->EbackLGC[itele][chanNum]->Fill(energy);
//           //Histo->EBLSum[itele]->Fill(chanNum,ienergy);

//           for (int i=0;i<Telescope[itele]->Back.Nstore;i++)
//         {
//           if (Telescope[itele]->Back.Order[i].strip == chanNum)
//             {
//                       Telescope[itele]->Back.Order[i].energyRlow = ienergy;
//                       Telescope[itele]->Back.Order[i].energylow = energy;
//                       double ienergyHigh=Telescope[itele]->Back.Order[i].energyR;
//                       if (ienergyHigh < 15000 && ienergy > 50 && ienergy <15000)
//             {
//                           bsumN[itele-6][chanNum]++;
//                           bsumx[itele-6][chanNum] += (double)corenergy;
//                           bsumxx[itele-6][chanNum] += pow((double)corenergy,2);
//                           bsumy[itele-6][chanNum] += ienergyHigh;
//                           bsumyx[itele-6][chanNum] += ienergyHigh*(double)corenergy;
//             }
                      
//               break;
//             }
//         }
          
//         }          
      
//     }

//     }
//   point = nextMB;
//   if (*point == 0xe0fe) return false;
//   //  cout << "point in si " << hex<< *point << " " << dec << *point << endl;
//   return true;
// }


// //***************************************************************//
// //** unpacking with XLM and sis modules//
// bool hira::unpackSi_sis(unsigned short *&point)
// {

//   unsigned short marker;
//   for (int iXLM=0;iXLM<3;iXLM++)
//     {
//       marker = *point++;
//       unsigned short *epoint = point;
//       if (marker == xmarker[iXLM]) //XLM1
//         {

//       // in princle words and words2 are both part of 
//       // a int32 parameter, but in practicable terms
//       // words2 should always be zero
//           unsigned short words = *epoint++;   // 
//           unsigned short words2 = *epoint++;
//       if (words2 != 0) 
//         {
//               cout << "words2= " << words2 << endl;
//               return false;
//         }
//           point = epoint + words;
//           unsigned short channelCount = *epoint++;  //number of data in XLM
//       //if (channelCount > 0) cout << iXLM << "  " << channelCount << endl;
//           epoint += 4;
//       nXLM[iXLM] = channelCount;

//       for (int idata = 0;idata<channelCount;idata++)
//         {
//               chanXLM[iXLM][idata] = *epoint++;
//               if (chanXLM[iXLM][idata] == 0) return false;
//         }
       
//         }
//       else cout << "XLM" << iXLM << "  missing" << endl; 
//     }
//   int itry = 0;
//   for(;;)
//     {
//       if (itry >10) return false;
//       marker = *point++;
//       if (marker == 0xfadc) break;
//       itry++;
//     }

//   point++;
//   for (int iXLM = 0; iXLM<3;iXLM++)
//     {
//       unsigned short words = *point++;
//       unsigned short words2 = *point++;
      
//       //cout << iXLM << " " << words << endl;

//       if (words2 != 0) 
//     {
//       cout << "Words in FADC .ne. 0" << endl;
//       return false;
//     }

//       if (words != nXLM[iXLM])
//         {
//           cout << " XLM and FDC channels do not match for XLM " << iXLM <<endl;
//           return false;
//     }
 

//       for (int i=0;i<words;i++)
//     {
         
//           unsigned short ienergy = *point++;
//           unsigned short time = *point++;
//           unsigned short chan = chanXLM[iXLM][i];

//       //by chip
//           unsigned short chipNum =  (chan >> 5) & 0xff;
//           unsigned short chanNum = chan & 0x0F;
//       unsigned short underOver = 0;
//           //cout << "chipNum = " << chipNum << " chanNum= " << chanNum << endl;


//       //by chip board
//       bool secondChip = false;
//           if (chipNum%2 == 0)
//         {
//           if (iXLM ==2) secondChip = true;
//           else chanNum += 16;
//           chipNum /= 2;
//         }
//       else
//         {
//           chipNum = chipNum/2 + 1;
//         }


//       bool bfront = Map[iXLM][chipNum].front;
//           bool bA = Map[iXLM][chipNum].A;
//           int itele = Map[iXLM][chipNum].itele - 1;
//       if(itele <0)     return false;
//           bool bhigh = true; 

//       if (iXLM == 2)
//         {
//               if (secondChip) bhigh = false;
//               else bhigh = true;

//               if (!bA) chanNum += 16;

//         }

//           /*
//         cout << i << " e= " << ienergy << " t= " << time << " chipNum= " << 
//         chipNum << " chanNum= " << chanNum << " bfront= " 
//         << bfront << " bA=" << bA << " itele= " << itele << " bhigh= "
//         << bhigh << endl; 
//       */
//       if (bhigh && bfront)
//         {
//           float energy = calFront->getEnergy(itele,chanNum,ienergy+ran->Rndm());
//           Histo->EfrontR[itele][chanNum]->Fill(ienergy);
//           Histo->TfrontR[itele][chanNum]->Fill(time);
//           Histo->EfrontC[itele][chanNum]->Fill(energy);


//               Telescope[itele]->Front.Add(chanNum,underOver,energy,ienergy,time);

//         }
//       if (bhigh && !bfront)
//         {
//               float energy = calBack->getEnergy(itele,chanNum,ienergy+ran->Rndm());
//           Histo->EbackR[itele][chanNum]->Fill(ienergy);
//           Histo->TbackR[itele][chanNum]->Fill(time);
//           Histo->EbackC[itele][chanNum]->Fill(energy);

//               Telescope[itele]->Back.Add(chanNum,underOver,energy,ienergy,time);

//         }

//       if (!bhigh && bfront)
//         {
//           Histo->EfrontLGR[itele][chanNum]->Fill(ienergy);
//           Histo->TfrontLG[itele][chanNum]->Fill(time);


//           for (int i=0;i<Telescope[itele]->Front.Nstore;i++)
//         {
//                   if (Telescope[itele]->Front.Order[i].strip == chanNum)
//             {
//                       Telescope[itele]->Front.Order[i].energyRlow = ienergy;
//                       double ienergyHigh=Telescope[itele]->Front.Order[i].energyR;
//                       if (ienergyHigh < 15000 && ienergy > 50 && ienergy <15000)
//             {
//               float Ratio = 0.;
//                           fsumN[itele-6][chanNum]++;
//                           fsumx[itele-6][chanNum] += (double)ienergy;
//                           fsumxx[itele-6][chanNum] += pow((double)ienergy,2);
//                           fsumy[itele-6][chanNum] += ienergyHigh;
//                           fsumyx[itele-6][chanNum] += ienergyHigh*(double)ienergy;
//             }
                      
//               break;
//             }
//         }


//         }          

//       if (!bhigh && !bfront)
//         {
//           Histo->EbackLGR[itele][chanNum]->Fill(ienergy);
//           Histo->TbackLG[itele][chanNum]->Fill(time);

//           for (int i=0;i<Telescope[itele]->Back.Nstore;i++)
//         {
//                   if (Telescope[itele]->Back.Order[i].strip == chanNum)
//             {
//                       Telescope[itele]->Back.Order[i].energyRlow = ienergy;
//                       double ienergyHigh=Telescope[itele]->Back.Order[i].energyR;
//                       if (ienergyHigh < 15000 && ienergy > 50 && ienergy <15000)
//             {
//                           bsumN[itele-6][chanNum]++;
//                           bsumx[itele-6][chanNum] += (double)ienergy;
//                           bsumxx[itele-6][chanNum] += pow((double)ienergy,2);
//                           bsumy[itele-6][chanNum] += ienergyHigh;
//                           bsumyx[itele-6][chanNum] += ienergyHigh*(double)ienergy;
//             }
                      
//               break;
//             }
//         }

//         }          

//       if (chanNum > 31)
//         {
//           cout << "chanNum too big" << endl;
//               return false;
//         }
//           if (chipNum > 11)
//         {
//           cout << "chipNum too big" << endl;
//               return false;
//         }
//     }

//       marker = *point++;
//       if (marker != 0xaaaa) cout << "marker = " << marker << endl;
//     }
  



//   return true;
// }
//***************************************************************//

//unpacking the XLM with ADC on the CHIP BOARDS (HINP 4)
bool hira::unpackSi_HINP4(unsigned short *&point)
{
  unsigned short marker;

  for(int iMB = 0;iMB<1;iMB++)
  {
    marker = *point++;
    if (marker != xmarker[iMB])
    { 
      cout << "Did not read the proper XLM marker. Was " << hex << marker << " expected " << xmarker[iMB] << dec <<endl;
      return false;
    }
    int NstripsRead = 0;
    unsigned short chipWords = *point;
    int NWords = *point;
    unsigned short * endHINP = point;

    if (NWords > 2048)
    {
      point+=10;
      return false;
      //return true; // KB - 10/26/20
    }

    endHINP += NWords+2;
    //  if (chipWords == 8)
    //  {
    //    NstripsRead = 0;
    //    point += 10;
    //    return (bool) 1;
    //  }
    point += 2;
    NstripsRead = *point;

    if(NstripsRead %4 !=0)
    {
      point+=8;
      return false;
      //return true;
    }

    NstripsRead /= 4;

    if (NstripsRead > 512) 
    {
      point +=8;
      return false; // bad buffer
      //return true; //need to return to this KB - 10/26/20
    }
    point += 5;
      
    //       cout <<  "NStrips " << NstripsRead <<  endl;
    for (int istrip = 0;istrip < NstripsRead;istrip++)
    {
      unsigned short id = *point++;
      unsigned short chipNum = (id&0x1FE0)>>5;
      unsigned short chanNum = id & 0x1F;
      unsigned short ienergy = *point++;
      unsigned short ilowenergy = *point++;
      unsigned short itime =  *point++;
      unsigned short underOver = 0;   //No under or overflow in HINP4
      


      if (chipNum%2 == 0)
      {
        //int dum = 31- 2*chanNum-1; //Using hira
        //chanNum = 31- 2*chanNum-1; //Using hira
        chanNum = 2*chanNum+1; //For S4

        //cout << chanNum << " " << dum << endl;
        chipNum /= 2;
      }
      else
      {
        chipNum = chipNum/2 + 1;
        //chanNum = 31 - 2*chanNum; //Using hira
        chanNum = 2*chanNum; //For S4
      }  
      
      if (chanNum > 31)
      {
        cout << "chanNum too big" << endl;
        return false;
      }
      if (chipNum > 8)
      {
        cout << "chipNum too big " << chipNum << endl;
        return false;
      }

      //Filling the Pies
      if(chipNum >= 1 && chipNum <=4)
      {
        int idum = chanNum;
        int iPie = -1;

        iPie = PieMap[chipNum-1][chanNum];
        
        float energy = calPies->getEnergy(0,iPie,ienergy+ran->Rndm());
        float time = 0.;//calPiesT->getEnergy(0,chanNum,itime+ran->Rndm());
        float lowenergy = 0.;
        Histo->PTSum->Fill(iPie,time);
        //          Histo->SiFTime->Fill(time);

        //Pies raw spectra
        Histo->EpiesR[iPie]->Fill(ienergy); //high gain
        Histo->EpiesLR[iPie]->Fill(ilowenergy); //low gain
        Histo->TpiesR[iPie]->Fill(itime); //time
        Histo->PSum->Fill(iPie,ienergy);

        //Pies calibrated spectra
        Histo->EpiesC[iPie]->Fill(energy); //high gain
        Histo->PCSum->Fill(iPie,energy);
        Histo->PCLSum->Fill(iPie,lowenergy);

        //Pies Hi Lo spectrum
        //Histo->EFHiLo->Fill(lowenergy,energy);

        // if(energy > 20.) //cut off for switching high to low gain energy
        //     {
        //       energy = lowenergy;
        //     }

        if(energy >Si_energy_min)
        {
          //Telescope->Pies.Add(iPie,energy,ilowenergy,ienergy,time);
          RingCounter->Pie.Add(iPie,energy,0.,ienergy,time); 
        }
      }
      
      //filling the rings histograms and calibrating
      else if(chipNum >=5 && chipNum <=8)
      {
        int iRing = -1;
        
        iRing = RingMap[chipNum-5][chanNum];
        if(iRing > 127 || iRing < 0)
        { 
          cout << " RingMap error!!" << endl;
          return false;
        }

        float energy = calRings->getEnergy(0,iRing,ienergy+ran->Rndm());
        float lowenergy = 0.;//calRings->getEnergy(0,iRing,voltage_lo_HL);
        float time = 0.;//calRingsT->getEnergy(0,iRing,itime+ran->Rndm());
        Histo->RTSum->Fill(iRing,time);
        //          Histo->SiFTime->Fill(time);

        //Rings raw spectra
        Histo->EringsR[iRing]->Fill(ienergy); //high gain
        Histo->EringsLR[iRing]->Fill(ilowenergy); //low gain
        Histo->TringsR[iRing]->Fill(itime); //time
        Histo->RSum->Fill(iRing,ienergy);
        
        //Rings calibrated spectra
        Histo->EringsC[iRing]->Fill(energy); //high gain
        Histo->RCSum->Fill(iRing,energy);
        //Rings Hi Lo spectrum
        //Histo->EBHiLo->Fill(lowenergy,energy);

        // if(energy > 20.) //cut off for switching high to low gain energy in MeV
        //     {
        //       //cout << "energy = " << energy << " lowenergy = " << lowenergy << endl;
        //       //if ((lowenergy - energy) > 20.)
        //       //{
        //       //cout << itele << " " << iRing << endl;
        //       //}
        //       energy = lowenergy;
        //     }

        if(energy >Si_energy_min)
        {
          RingCounter->Ring.Add(iRing,energy,0.,ienergy,time);
        }
      }      
    } //end loop over strips
      
    point = endHINP;

  } //end loop over MBnum

  //  if (*point == 0xe0fe) return false;
  //  cout << "point in si " << hex<< *point << " " << dec << *point << endl;
  //  point++;
  return true;
}

//***************************************************************
hira::~hira()
{

  cout << "start Hira destr" << endl;
  //high-low correlations

  /*
  ofstream iFfile("f.dat");
  ofstream iBfile("b.dat");
  
  for (int i=0;i<14;i++)
    {
      for (int j=0;j<32;j++)
       {
       double delta = fsumN[i][j]*fsumxx[i][j]- pow(fsumx[i][j],2);
       if (delta == 0)
     {
       iBfile << i << " " << j << " " << 1 << " " << 0 << endl;
       continue;
       
     }

       double slope = fsumN[i][j]*fsumyx[i][j] - fsumx[i][j]*fsumy[i][j];
       slope /= delta;
       double intercept = (fsumy[i][j] - slope*fsumx[i][j])/fsumN[i][j];
       iFfile << i << " " << j << " " << slope << " " << intercept << endl;

       }
    }
  iFfile.close();
  for (int i=0;i<14;i++)
    {
      for (int j=0;j<32;j++)
       {
       double delta = bsumN[i][j]*bsumxx[i][j]- pow(bsumx[i][j],2);
       if (delta == 0)
     {
       iBfile << i << " " << j << " " << 1 << " " << 0 << endl;
       continue;
       
     }
       double slope = bsumN[i][j]*bsumyx[i][j] - bsumx[i][j]*bsumy[i][j];
       slope /= delta;
       double intercept = (bsumy[i][j] - slope*bsumx[i][j])/bsumN[i][j];
       iBfile << i << " " << j << " " << slope << " " << intercept << endl;

       }
    }
  iBfile.close();
  */

  delete calCsi;
  delete calRings;
  delete calPies;
  delete RingCounter;
  delete XY_mon;
  
  // for(int i=0;i<14;i++)
  //   {
  //     delete Telescope[i];
  //   }
  // delete [] Telescope;

  cout << "stop Hira destr" << endl;
}




//**********************************************
void hira::reset()
{
  XY_mon->reset();
  RingCounter->reset();
  fred = false;
  //  for (int i=0;i<14;i++) Telescope[i]->reset();
}
