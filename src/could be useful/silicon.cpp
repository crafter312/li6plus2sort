#include "silicon.h"

//***********************************************************************//
//  Russian Detector -- itele == 0
//  S2 Detector      -- itele == 1
// 

silicon::silicon(TRandom * ran0, histo * Histo0)
{
  Histo = Histo0;
  ran = ran0;

  xmarker[0]=0x1ff3;

  tdc = new TDC1190(1,32,64);

  //read in calibrations
  int NSpies = 16;
  int NSrings = 48;
  int NRpies = 64;
  int NRrings = 32;

  string name;


  //need to check these calibrations, were done in Texas 8/2015, as of  12/2015

  //Russian Si Calibrations chan->MeV
  name = "cal/rPies.cal";
  calRPie = new calibrate(1,NRpies,name,1);
  name = "cal/rRings.cal";
  calRRing = new calibrate(1,NRrings,name,1);


  //S2 Si Calibrations chan->MeV
  name = "cal/S2Pies.cal";
  calSPie = new calibrate(1,NSpies,name,1);
  name = "cal/S2Rings.cal";
  calSRing = new calibrate(1,NSrings,name,1);
  
  //CsI Calibrations cham->MeV
  //  name = "cal/CsI.cal";
  //calCsI = new calibrate(1,32,name,1);




  Telescope = new telescope*[2];
  for (int i=0;i<2;i++)
    {
      Telescope[i] = new telescope(ran,i,Histo);
    }
  cout << "HERE!!!!!!!!!!!!!!" << endl;
  ifstream ifile("TexasMap.dat");
  if(!ifile.is_open())
    {
      cout << "CsI map not found" << endl;
      abort();
    }
  getline(ifile,name);
  int itele,ipie,icsi;
  for(;;)
    {
      ifile >> itele >> ipie >> icsi;
      if(ifile.eof())break;
      if(ifile.bad())break;

      Telescope[itele]->load(ipie,icsi);

    }

  ifile.close();

  //high low correlations zero arrays
  for (int i=0;i<2;i++)
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
//*************************************************************
bool silicon::unpack(unsigned short *&point,int runno)
{
  bool stat = true;
  stat = unpackSi_HINP4(point);
  if (!stat) return stat;
  stat = unpackCsi(point,runno);
  return stat; 
}
//*************************************************************
bool silicon::unpackCsi(unsigned short *&point,int runno)
{
  NE = 0;
  CsIM =0;

  // cout << "point in Csi " << hex << *point << endl; 
  
  /*
  unsigned short * pig = point;
  for (int i=0;i<80;i++)
    {
      cout << dec<< *pig << " " << hex<< *pig << endl;
      pig++;
    }
  cout << endl;
  abort();
  //return true;
  */
  //  point +=3;
  for (int iadc = 0;iadc<1;iadc++)
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

	  // cout << "id " << id << endl;

	  if(id < 32)
	    {

	      // float energy = calCsi->getEnergy(0,id,ienergy+ran->Rndm());
	      DataE[NE].id = id;
	      DataE[NE].ienergy = ienergy;
	      //DataE[NE].energy = energy;
	      if(id<0)cout << id << " =id ERROR ienergy=  " << ienergy << endl;
	      Histo->ECsI[id]->Fill(ienergy);
	      


	      float energy = 0; //FIX THIS!!!!!! KB
	      
	      int itele = (int)floor(DataE[NE].id/16);
	      int icsi = DataE[NE].id - 16*itele; 

              if (ienergy > 150)
 		{

		  CsIM++;
		  Telescope[itele]->Csi.Add(icsi,0,energy,DataE[NE].ienergy,0.);
 		}
	      //cout << Telescope[itele]->Csi.Order[0].strip;
	      //cout << " " << Telescope[itele]->Csi.Order[0].energy << endl;
	      
	     NE++;
	    }
	      

	}
      

      //check for ffff's
     unsigned short f1 = *point;
     point++;
     unsigned short f2 = *point;
     point++;
     if(f1 != 0xffff && f2 != 0xffff) return false;
    }


  NT = 0;
  // cout << "tdc->Ndata = " << tdc->Ndata << endl;
  for (int itdc = 0;itdc<1;itdc++)
    {
      
      //check for ffff's
      unsigned short f3 = *point;
      unsigned short f4 = *(point+1);
      if (f3 == 0xffff && f4 == 0xffff) 
	{
	  point += 2;
	  continue;
	} 
      
      point = tdc->read(point);  // suck out the info in the tdc
      if(tdc->notTDCerror==1){ cout << eventNum << endl; return false;}
      for (int i=0;i<tdc->Ndata;i++)
	{
	  
          int id = tdc->dataOut[i].channel;
	  
          int itime = tdc->dataOut[i].time;
	  if (id < 32)
	    {
	      //DataT[NT].id = id;
	      //DataT[NT].itime = itime;
	      Histo->TCsI[id]->Fill(itime);
	      NT++;
	    }
	}
      
      
      //check for ffff's
      unsigned short f1 = *point;
      point++;
      unsigned short f2 = *point;
      point++;
      if(f1 != 0xffff && f2 != 0xffff) return false;
      
      
    }

  /*
  //cout << NE << " " << NT << endl;
  // match up energies to times
  for (int ie=0;ie<NE;ie++)
    {
      DataE[ie].itime = -1;
      //cout << "DataE[ie].id = " <<DataE[ie].id << endl;
      //cout << "DataT[ie].id = " << DataT[ie].id << endl;
      //cout <<"NE = " << NE << " NT = " << NT <<endl;
      for (int it=0;it<NT;it++)
	{

          if (DataE[ie].id == DataT[it].id ) 	      //we have matched
	    {
	      DataE[ie].itime = DataT[it].itime;
	      //     int itele = DataE[ie].id/4;
	      //int icsi = DataE[ie].id%4;
	      int itele = (int)floor(DataE[NE].id/16);
	      int icsi = DataE[NE].id - 16*itele; 
	      float energy=0.;
	      if(DataE[ie].ienergy >150)// && DataE[ie].itime > 500 && DataE[ie].itime < 1500)
		{
		  Telescope[itele]->Csi.Add(icsi,0,energy,DataE[ie].ienergy,(float)DataE[ie].itime);
		  
// 		  cout <<"itele = " <<itele << " icsi = " << icsi <<" Time =" << DataE[ie].itime << " Energy = " <<DataE[ie].ienergy << endl;

// 		  cout << "icsi = " <<  Telescope[itele]->Csi.Order[0].strip << " energy = " << Telescope[itele]->Csi.Order[0].energyR;
// 		  cout << " time = " << Telescope[itele]->Csi.Order[0].time  << endl;
// 		  cout << endl;

		}

	    }
	  else if (DataE[ie].id < DataT[it].id) break; // no match found
	}
    }
*/     






  bool stat = true;
  return stat;
}


//***************************************************************
  //unpacking the XLM with ADC on the CHIP BOARDS (HINP 4)
bool silicon::unpackSi_HINP4(unsigned short *&point)
{

  /*  
  cout << "HEY" << endl;
  unsigned short * pig = point;
  for (int i=0;i<30;i++)
    {
      cout << dec<< *pig << " " << hex<< *pig << endl;
      pig++;
    }
  abort();
  */
  



  unsigned short marker;
  //    cout << iMB << " " << hex <<*point;
  marker = *point++;
  if (marker != xmarker[0]) return false;
      
  int NstripsRead = 0;
  unsigned short chipWords = *point;
  int NWords = *point;
  unsigned short * endHINP = point;
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
    return false;

  //  cout << "NStripsRead = " << NstripsRead << endl;
  if (NstripsRead > 384)
    {
      point +=8;
      return false; // bad buffer
    }
  point += 5;

  NstripsRead /= 4;
  // cout << NstripsRead << endl;

  //cout << NstripsRead << endl;

  // cout <<  "NStrips " << NstripsRead <<  endl;
  float pie1E = 0.;
  float pie2E = 0.;
  float time1E = 0.;
  float time2E = 0.;
  float ring1E = 0.;
  float ring2E = 0.;
  for (int istrip = 0;istrip < NstripsRead;istrip++)
    {
      
      unsigned short id = *point++;
      unsigned short chipNum = (id&0x1FE0)>>5;
      unsigned short chanNum = id & 0x1F;
      unsigned short ienergy = *point++;
      unsigned short ilowenergy = *point++;
      unsigned short itime =  *point++;
      unsigned short underOver = 0;   //No under or overflow in HINP4
      
      

      if(chipNum%2 == 0)
	{
	  chipNum /=2;
	  chanNum +=16;
	}
      else
	{
	  chipNum = chipNum/2 + 1;
	}
      
      if(chipNum > 6)
	{
	  //cout << endl;
	  //cout << "chipNum is too big" << endl;
	  //cout << "eventNum = " << dec << eventNum ;
	  //	  cout << " chip id = " << std::setfill('0') << std::setw(4) << hex << id<< endl;
	  //cout << dec;
	  //  abort();
	  return false;
	}      
      if (chanNum > 31)
	{
	  //  cout << "chanNum too big" << endl;
	  return false;
	}
      
      if(chipNum ==0)
	{
	  // cout << "invalid chip number" << endl;
	  return false;
	}

      int iRpie = -1;
      int iRring = -1;
      int iSpie = -1;
      int iSring = -1;
      bool bring = 0;
      bool bpie = 0; 
      int itele = -1;


      //mapping of chips to pies and rings on Russiand and S2
      if(chipNum <=3)
	{
	  itele = 0;
	  if(chipNum !=3)
	    {
	      bpie = 1;

	      if(chipNum ==1)
		{
		  if(chanNum <16)
		    {
		      iRpie = 15-chanNum;
		    }
		  else
		    {
		      iRpie = 47-chanNum;
		    }
		}
	      if(chipNum ==2)
		{
		  iRpie = 63 - chanNum;
		}
	      
	      if(iRpie ==21)
		{
		  pie1E = ienergy;
		  time1E = itime;
		}
	      if(iRpie ==22)
		{
		  pie2E = ienergy;
		  time2E = itime;
		}
	      
	      //   cout << "irpie = " << iRpie << " " << chanNum<< " " << chipNum << endl;

	      Histo->RusPiesR[iRpie]->Fill(ienergy);
	      Histo->RusPiesRLG[iRpie]->Fill(ilowenergy);
		
	      
	    }
	  
	  if(chipNum ==3)
	    {
	      bring = 1;
	      if(chanNum ==15)
		ring1E = ienergy;
	      if(chanNum ==16)
		ring2E = ienergy;

	      //	      iRring = chanNum;	
	      if(chanNum >=16)
		    {
		      iRring = chanNum-16;
		    }
		  else
		    {
		      iRring = chanNum +16;
		    }

	      
	      Histo->RusRingsR[iRring]->Fill(ienergy);
	      Histo->RusRingsRLG[iRring]->Fill(ilowenergy);
	    }
	  
	  //  Histo->E1E2pie->Fill(pie1E,pie2E);
	  Histo->E1E2ring->Fill(ring1E,ring2E);
	}
      else
	{
	  itele =1;
	  if(chipNum ==4)
	    {
	      bpie = 1;
	      if(chanNum < 16)
		{
		  // cout << "Data from an unused chip" << endl;
		  return false;
		}
	      iSpie = chanNum-16;;
	      Histo->S2PiesR[iSpie]->Fill(ienergy);
	      Histo->S2PiesRLG[iSpie]->Fill(ilowenergy);
	    } 
	  else
	    {
	      bring =1;
	      if(chipNum == 5)
		{
		  if(chanNum >=16)
		    {
		      iSring = chanNum-16;
		    }
		  else
		    {
		      iSring = chanNum +16;
		    }

		}
	      else if(chipNum ==6)
		iSring = chanNum+16;

	      Histo->S2RingsR[iSring]->Fill(ienergy);
	      Histo->S2RingsRLG[iSring]->Fill(ilowenergy);
	    }
	}



      //calibrations start

      if(bpie)
	{
	  float energy  = 0.;
	  float time = itime;
	  if(itele ==0)
	    {
	      energy = calRPie->getEnergy(0,iRpie,ienergy+ran->Rndm());
	      //Histo->RusPiesC[iRpie]->Fill(energy);
	      //Histo->ERPCSum->Fill(iRpie,energy);
	      if(energy > 1.)
		{
		  if(iRpie !=21 && iRpie !=22)
		    Telescope[itele]->Pie.Add(iRpie,underOver,energy,ienergy,time);
		}
	    }
	  else
	    {
	      energy = calSPie->getEnergy(0,iSpie,ienergy+ran->Rndm());
	      Histo->S2PiesC[iSpie]->Fill(energy);
	      Histo->ESPCSum->Fill(iSpie,energy);
	      if(energy > 1.)
		{
		  Telescope[itele]->Pie.Add(iSpie,underOver,energy,ienergy,time);
		}

	    }

      int blockme = Telescope[0]->Block(iRring);
      //cout << iRpie << endl;
      if(iRpie==16)
	{
	  if(blockme==1) Histo->RusPiesBlock1->Fill(ienergy);
	  if(blockme==2) Histo->RusPiesBlock2->Fill(ienergy);
	  if(blockme==3) Histo->RusPiesBlock3->Fill(ienergy);
	  if(blockme==4) Histo->RusPiesBlock4->Fill(ienergy);
	  if(blockme==5) Histo->RusPiesBlock5->Fill(ienergy);
	  if(blockme==6) Histo->RusPiesBlock6->Fill(ienergy);
	  if(blockme==7) Histo->RusPiesBlock7->Fill(ienergy);
	  if(blockme==8) Histo->RusPiesBlock8->Fill(ienergy);
	}


	}
      else
	{
	  float energy = 0.;
	  float time = itime;
	  if(itele==0)
	    {
	      energy = calRRing->getEnergy(0,iRring,ienergy+ran->Rndm());
	      Histo->RusRingsC[iRring]->Fill(energy);
	      Histo->ERRCSum->Fill(iRring,energy);
	      if(energy > 1.)
		{
		  Telescope[itele]->Ring.Add(iRring,underOver,energy,ienergy,time);
		}
	    }
	  else
	    {
	      energy = calSRing->getEnergy(0,iSring,ienergy+ran->Rndm());
	      Histo->S2RingsC[iSring]->Fill(energy);
	      Histo->ESRCSum->Fill(iSring,energy);
	      if(energy > 1.)
		{
		  Telescope[itele]->Ring.Add(iSring,underOver,energy,ienergy,time);
		}

	    }

	  int blockme = Telescope[0]->Block(iRring);  
      //cout << blockme << endl;
      if(iRpie==16)
	{

	  // cout << "ladaga" << endl;
	  if(blockme==1) Histo->RusPiesBlock1->Fill(ienergy);
	  if(blockme==2) Histo->RusPiesBlock2->Fill(ienergy);
	  if(blockme==3) Histo->RusPiesBlock3->Fill(ienergy);
	  if(blockme==4) Histo->RusPiesBlock4->Fill(ienergy);
	  if(blockme==5) Histo->RusPiesBlock5->Fill(ienergy);
	  if(blockme==6) Histo->RusPiesBlock6->Fill(ienergy);
	  if(blockme==7) Histo->RusPiesBlock7->Fill(ienergy);
	  if(blockme==8) Histo->RusPiesBlock8->Fill(ienergy);
	}


	}
    }



  
  //pie1E == 21
  //pie2E == 22

  int i21 = -1;
  int i22 = -1;


  //Removes the xtalk from the russian pies

  if(pie1E >0 && pie2E >0)
    {
      Ncross++;
      Histo->E1E2pie->Fill(pie1E,pie2E);

      if(pie1E >= pie2E) // 21 is larger than 22
	{
	  Histo->p21p22Raw->Fill(pie1E+pie2E);
	  float energy = calRPie->getEnergy(0,21,pie2E+pie1E+ran->Rndm());
	  float time = time2E;
	  if(energy > 1.)
	    Telescope[0]->Pie.Add(21,0,energy,pie1E+pie2E,time);

	}
      else // 22 is larger than 21
	{
	  Histo->p22p21Raw->Fill(pie1E+pie2E);
	  float energy = calRPie->getEnergy(0,22,pie2E+pie1E+ran->Rndm());
	  float time = time2E;
	  if(energy >1.)
	    Telescope[0]->Pie.Add(22,0,energy,pie1E+pie2E,time);
	}
    }

  //Fill Russian Calibrated spectra
  
  for(int i=0;i<Telescope[0]->Pie.Nstore;i++)
    {
      float energy = Telescope[0]->Pie.Order[i].energy;
      int ii = Telescope[0]->Pie.Order[i].strip;
      Histo->RusPiesC[ii]->Fill(energy);
      Histo->ERPCSum->Fill(ii,energy);
    }

  point = endHINP;


  if (*point == 0xe0fe) return false;
  //  cout << "point in si " << hex<< *point << " " << dec << *point << endl;
  //  point++;
  return true;
}

//***************************************************************
silicon::~silicon()
{

  cout << "start RusS2 destr" << endl;
  //high-low correlations

  /*   
  for (int i=0;i<2;i++)
    for (int j=0;j<32;j++)
      {
        double delta = fsumN[i][j]*fsumxx[i][j]- pow(fsumx[i][j],2);
        if (delta == 0) continue;
        double slope = fsumN[i][j]*fsumyx[i][j] - fsumx[i][j]*fsumy[i][j];
        slope /= delta;
        double intercept = (fsumy[i][j] - slope*fsumx[i][j])/fsumN[i][j];
	cout << i << " " << j << " " << slope << " " << intercept << endl;

      }

  for (int i=0;i<2;i++)
    for (int j=0;j<32;j++)
      {
        double delta = bsumN[i][j]*bsumxx[i][j]- pow(bsumx[i][j],2);
        if (delta == 0) continue;
        double slope = bsumN[i][j]*bsumyx[i][j] - bsumx[i][j]*bsumy[i][j];
        slope /= delta;
        double intercept = (bsumy[i][j] - slope*bsumx[i][j])/bsumN[i][j];
	cout << i << " " << j << " " << slope << " " << intercept << endl;

      }
  */  





  for(int i=0;i<2;i++)
    {
      delete Telescope[i];
    }
  delete [] Telescope;

  cout << "stop RusS2 destr" << endl;
}

//**********************************************
void silicon::reset()
{
  fred = false;
  for (int i=0;i<2;i++) Telescope[i]->reset();
}
