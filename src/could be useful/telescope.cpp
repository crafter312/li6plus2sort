#include "telescope.h"
#include "constants.h"
#include "histOn.h"

bool const telescope::relativity=1;

telescope::telescope(TRandom * ran0, int id0, histo_read * Histo1)
{
  id = id0;
  int tele_found = 0;
  Histo = Histo1;
  ran = ran0;
  ifstream ifile;
  ifile.open("datfiles/teles.dat");

  float target_offset = -0.0180753; //target ladder thickness + offset from blocker
  target_offset = -0.0225;

  xcenter = 0.;
  ycenter = 0.;
  zcenter = 0.;
  xhoriz = 0.; 
  yhoriz = 0.;
  zhoriz = 0.;
  xdiag = 0.;
  ydiag = 0.;
  zdiag = 0.;

  activex = 6.4;
  activey = 6.4;

  for (int i = 0; i<3; i++)
    {
      rcenter[i] = 0.;
      rback[i] = 0.;
      rdiag[i] = 0.;
      rnormal[i] = 0.;
      rfront[i] = 0.;
    }

  while (ifile >> itele >> xcenter >> ycenter >> zcenter >> xhoriz >> yhoriz >> zhoriz >> xdiag >> ydiag >> zdiag)
    {
      if (itele == id)
	{
	  tele_found = 1;
	  break;
	}
      else
	{
	  continue;
	}
    }
  zcenter = zcenter + target_offset;
  if (tele_found != 1)
    {
      cout << "telescope mapping not found" << endl;
      abort();
    }

  //some function should be called HERE to turn the values from the data file into vectors//
  //pass it into the Tele class init below//

  findVectors(rcenter, rback, rdiag, rnormal, rfront, xcenter, ycenter, zcenter, xhoriz, yhoriz, zhoriz, xdiag, ydiag, zdiag);

  Tele = CTele(rcenter,rfront,rback,activex,activey);
  
  Tele.getCsICenters(id);

  losses = new CLosses(8);
  
  dfb_max = 3.;
  //dfb_max = 20.; //new value for testing
  
  ostringstream outstring;
  for (int icsi=0;icsi<4;icsi++)
    {
      int ii = icsi + 4*id;
      outstring.str("");
      outstring << "pid"<<ii;
      string name = outstring.str();
      Pid[icsi] = new pid(name); 
    }

  /*
  string name="Hydrogen.loss";
  Loss[0] = new CLoss(name,1.);
  Loss[1] = new CLoss(name,2.);
  Loss[2] = new CLoss(name,3.);
  name="Helium.loss";
  Loss[3] = new CLoss(name,3.);
  Loss[4] = new CLoss(name,4.);
  name = "Lithium.loss";
  Loss[6] = new CLoss(name,6.);
  Loss[7] = new CLoss(name,7.);
  Loss[8] = new CLoss(name,8.);
  name = "Beryllium.loss";
  Loss[9] = new CLoss(name,7.);
  name = "Boron.loss";
  Loss[10] = new CLoss(name,10.);
  Loss[11] = new CLoss(name,11.);
  name = "Carbon.loss";
  Loss[12] = new CLoss(name,9.);
  Loss[13] = new CLoss(name,10.);
  Loss[14] = new CLoss(name,11.);
  Loss[15] = new CLoss(name,12.);
  Loss[16] = new CLoss(name,13.);
  name = "Nitrogen.loss";
  Loss[17] = new CLoss(name,12.);
  Loss[18] = new CLoss(name,13.);
  Loss[19] = new CLoss(name,14.);
  Loss[20] = new CLoss(name,15.);
  name = "Oxygen.loss";
  Loss[21] = new CLoss(name,13.);
  Loss[22] = new CLoss(name,14.);
  Loss[23] = new CLoss(name,15.);
  Loss[24] = new CLoss(name,16.);
  */

  string name = "cal/p_calibrations.cal";
  calCsi = new calibrate(1,56,name,1);
  name = "cal/d_calibrations.cal";
  calCsid = new calibrate(1,56,name,1);
  name = "cal/Triton_New_ang.cal";
  calCsit = new calibrate(1,56,name,1);

  //name = "cal/Helium3_New_ang.cal";

  //using the alpha calibrations for both 3He and alphas
  name = "cal/new_alpha_calibrations_new_energies.cal";
  calCsiHe3 = new calibrate(1,56,name,1);
  calCsiA = new calibrate(1,56,name,1);


  name = "cal/Lithium6_New_ang.cal";
  calCsiLi6 = new calibrate(1,56,name,1);

  name = "cal/Beryllium7_New_ang.cal";
  calCsiBe7 =new calibrate(1,56,name,1);

  name = "cal/B10_calibrations.cal";
  calCsiB10 = new calibrate(1,56,name,1);
  name = "cal/Boron11_New_ang.cal";
  calCsiB11 = new calibrate(1,56,name,1);


  name = "cal/C9_calibrations_firstpass.cal";
  calCsiC8 = new calibrate(1,56,name,1);
  calCsiC9 = new calibrate(1,56,name,1);
  name = "cal/C10_calibrations_firstpass.cal";
  calCsiC10 = new calibrate(1,56,name,1);
  name = "cal/C11_calibrations_firstpass.cal";
  calCsiC11 = new calibrate(1,56,name,1);
  name = "cal/C12_New_ang.cal";
  calCsiC12 = new calibrate(1,56,name,1);
  name = "cal/C13_New_ang.cal";
  calCsiC13 = new calibrate(1,56,name,1);

  name = "cal/Nitrogen12_New_ang.cal";
  calCsiN12 = new calibrate(1,56,name,1);
  name = "cal/Nitrogen13_New_ang.cal";
  calCsiN13 = new calibrate(1,56,name,1);
  name = "cal/Nitrogen_New_ang.cal"; //N14
  calCsiN14 = new calibrate(1,56,name,1);
  name = "cal/Nitrogen15_New_ang.cal";
  calCsiN15 = new calibrate(1,56,name,1);


  name = "cal/O13_New_ang.cal";
  calCsiO13 = new calibrate(1,56,name,1);
  name = "cal/O14_New_ang.cal";
  calCsiO14 = new calibrate(1,56,name,1);
  name = "cal/O15_New_ang.cal";
  calCsiO15 = new calibrate(1,56,name,1);
  name = "cal/O16_New_ang.cal";
  calCsiO16 = new calibrate(1,56,name,1);

  name = "cal/F17_New_ang.cal";
  calCsiF17 = new calibrate(1,56,name,1);
 
  name = "cal/fb.cal";
  calFB =new calibrate(1,14,name,2);


  

}
//************************************************
telescope::~telescope()
{
  delete losses;
  /*
  for(int ii =0;ii<25;ii++)
    {
      delete Loss[ii];
    }
  */
}
//***********************************************
void telescope::analyze(int event)
{
  Nsolution = 0;
  Np = 0;
  N6 = 0;
  fenergy = 0;
  benergy = 0;

  const double dz = -11.4824;

  Event = event;

  /*
  //Prints out the angle for the center of each CsI in the system
  //Will need to be rerun when we get the new romer arm coords.
  for(int itele =0;itele<14;itele++)
    {
      for(int icsi=0;icsi<4;icsi++)
	{
	  cout << 4*itele+icsi << " " << Pixels.getCsiCenter(itele,icsi) << endl;
	}
    }
  abort();
  */

  // if(Front.Nstore !=0 || Back.Nstore !=0)
  //   Histo->FBMult[id]->Fill(Front.Nstore,Back.Nstore);

  //if(Front.Nstore != 1) return;
  //if(Back.Nstore != 1) return;

  
  //  //get rid of xtalk between neighboring strips in Front
  //  if (Front.Nstore > 1)
  //    {
  //      if (abs(Front.Order[0].strip - Front.Order[1].strip) == 1)
  //	{
  //	  if (Front.Order[0].energy > 100. && Front.Order[1].energy > Front.Order[0].energy/20.) return; 
  //	}
  //  }
  
  


  // //Getting rid of the xtalk for front and back for large particles
  // if(id ==6 || id==7)
  //   {
  //     if(Front.Nstore >1 && Front.Order[0].energy >100.) //Is it a large pulse?
  // 	{
  // 	  for(int ff = 1;ff<Front.Nstore;ff++)
  // 	    {
	      
  // 	      if(abs(Front.Order[0].strip - Front.Order[ff].strip) <=2) //Is it a neighbor?
  // 		{
  // 		  if(ff != Front.Nstore-1) //Is it the last in the list?
  // 		    {
  // 		      for(int i = ff+1;i<Front.Nstore;i++)
  // 			{
  // 			  Front.Order[i-1] =  Front.Order[i];  //Move everything up.
  // 			}
  // 		    }
  // 		  Front.Nstore--; //Decrease the number of entries
  // 		  ff--;
  // 		}
  // 	    }
  // 	}
  //     if(Back.Nstore >1 && Back.Order[0].energy >100.) //Is it a large pulse?
  // 	{
  // 	  for(int ff = 1;ff<Back.Nstore;ff++)
  // 	    {
  // 	      if(abs(Back.Order[0].strip - Back.Order[ff].strip) <=2) //Is it a neighbor?
  // 		{
  // 		  if(ff != Back.Nstore-1) //Is it the last in the list?
  // 		    {
  // 		      for(int i = ff+1;i<Back.Nstore;i++)
  // 			{
  // 			  Back.Order[i-1] =  Back.Order[i];  //Move everything up.
  // 			}
  // 		    }
  // 		  Back.Nstore--; //Decrease the number of entries
  // 		  ff--;
		  
  // 		}
  // 	    }
  // 	}
  //     if(Front.Nstore > 1)
  // 	{
  // 	  for(int ii =0;ii<Front.Nstore;ii++)
  // 	    {
  // 	      if(Front.Order[ii].strip == 0 || Front.Order[ii].strip == 31)
  // 		{
  // 		  if(ii != Front.Nstore-1)
  // 		    {
  // 		      for(int jj = ii+1;jj<Front.Nstore;jj++)
  // 			{
  // 			  Front.Order[jj-1] = Front.Order[jj];
  // 			}
  // 		    }
  // 		  Front.Nstore--;
  // 		  ii--;
  // 		}
  // 	    }
  // 	}
  //     if(Back.Nstore > 1)
  // 	{
  // 	  for(int ii =0;ii<Back.Nstore;ii++)
  // 	    {
  // 	      if(Back.Order[ii].strip == 0 || Back.Order[ii].strip == 31)
  // 		{
  // 		  if(ii != Back.Nstore-1)
  // 		    {
  // 		      for(int jj = ii+1;jj<Back.Nstore;jj++)
  // 			{
  // 			  Back.Order[jj-1] = Back.Order[jj];
  // 			}
  // 		    }
  // 		  Back.Nstore--;
  // 		  ii--;
  // 		}
  // 	    }
  // 	}
  //   } //end if for id = 6,7
  
  //for(int i =0;i<Front.Nstore;i++)
  //  if(Front.Order[i].energylow !=0)
  //   Front.Order[i].energylow +=4;
  
  int mult = min(Front.Nstore,Back.Nstore);

  if(Front.Nstore ==0 || Back.Nstore ==0) return;
  fenergy = Front.Order[0].energy;
  benergy = Back.Order[0].energy;

  if(fenergy <= 20.)
    {
      //dfb_max = 5.;
      dfb_max = 20.;
    }
  else 
    {
      //dfb_max = 0.1*fenergy;
      dfb_max = 1.*fenergy;
    }

  // cout << fenergy << " vs " << benergy << endl;

  for(int i =0;i<mult;i++)
  {
    float accept = 0.2;
    float fdum = Front.Order[i].energy;
    float bdum = Back.Order[i].energy;
    if(fdum <10.) accept =1./fdum;
    if(fabs(fdum-bdum) < dfb_max && On_FB)
      {
	Histo->FBDiff[id]->Fill(fabs(fdum-bdum));
	Histo->FB[id]->Fill(fdum,bdum);
      }
  }

  fhit = Front.Order[0].strip;
  bhit = Back.Order[0].strip;
  
  theta = Tele.getTheta(fhit,bhit); //not sure if this is the right arg order, goes as "xstrip" and "ystrip" in tele
  phi = Tele.phiRecon;
  
  xhit = -85.*sin(theta)*cos(phi); // negative to switch to old left handed coordinate system. i am using a right handed one
  yhit = 85.*sin(theta)*sin(phi);

  CsIhit = Csi.Order[0].strip + id*4;
  if(id ==6 )
    {
      //      cout << CsIhit << " "<< Csi.Order[0].strip << endl;
    }
  bool matched =0;
  if(Csi.Nstore ==1)
    {
      
      int icsi = Csi.Order[0].strip;
      int ifront = Front.Order[0].strip;
      int iback = Back.Order[0].strip;
      int itele = id;

      
      if (ifront >= FrontLow[icsi] &&
	  ifront <= FrontHigh[icsi] &&
	  iback  >= BackLow[icsi]  &&
	  iback  <= BackHigh[icsi])
	{
	  matched = 1;
	}	
      if(!matched)return;
      
      if (fabs(Front.Order[0].energy - Back.Order[0].energy)> dfb_max) return;
      if (id == 10 && ifront ==2 ) return;
      Histo->HitMap->Fill(xhit,yhit);
      CsIenergy = Csi.Order[0].energyR;
      Csi.Order[0].energyCsI = CsIenergy;
      Sienergy = fenergy;
      Sienergy_raw = Front.Order[0].energyRlow;
     
      if (On_dEE)Histo->dEE[CsIhit]->Fill(CsIenergy,Sienergy);
      
      //gate on one CsI for each strip (closest to the beam) to fill projection spectra with
      if (itele==0)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}
      else if (itele==1)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      else if (itele==2)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}      
      else if (itele==3)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}
      else if (itele==4)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      else if (itele==5)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}
      else if (itele==6)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}
      else if (itele==7)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      else if (itele==8)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      else if (itele==9)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}
      else if (itele==10)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      else if (itele==11)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      else if (itele==12)
	{
	  if (ifront<16)
	    {
	      gateCsI = 1;
	    }
	  else
	    {
	      gateCsI = 2;
	    }
	}
      else if (itele==13)
	{
	  if (ifront<16)
	    {
	      gateCsI = 0;
	    }
	  else
	    {
	      gateCsI = 3;
	    }
	}
      //gate previously 1800 to 1900
      if (CsIenergy > 400 && CsIenergy <= 500 && Csi.Order[0].strip == gateCsI)
	{
	  if(On_dEE_pro)
	    {
	     Histo->dEE_Projections[CsIhit/4][ifront]->Fill(Sienergy);
	     Histo->dEE_ProjectionsRaw[CsIhit/4][ifront]->Fill(Sienergy_raw);
	    }
	}

	
      //  cout << "E from ADC " << CsIenergy << endl;
      
      //      return;
      
      // cout << CsIenergy << " vs " << Sienergy << endl;

      
      float energy = Csi.Order[0].energy;
      
      bool stat = Pid[Csi.Order[0].strip]->getPID(CsIenergy,Sienergy);
      if (!stat) return;
      
      int Z = Pid[Csi.Order[0].strip]->Z;
      int A = Pid[Csi.Order[0].strip]->A;

      if(Z == 2 && A == 4)
	{
	  if (CsIhit == 32)
	    {
	      Histo->shiftedstrips32->Fill(fhit,bhit);
	    }
	  if (CsIhit == 33)
	    {
	      Histo->shiftedstrips33->Fill(fhit,bhit);
	    }
	}

//       if (CsIhit == 25)
// 	{
// 	  if (Z == 6)
// 	    {
// 	      if (A == 9 || A == 8)
// 		{
// 		  Histo->dEE_CsIReactions_25->Fill(CsIenergy);
// 		}
// 	    }
// 	}
//       if (CsIhit == 26)
// 	{
// 	  if (Z == 6)
// 	    {
// 	      if (A == 9 || A == 8)
// 		{
// 		  Histo->dEE_CsIReactions_26->Fill(CsIenergy);
// 		}
// 	    }
// 	}
//       if (CsIhit == 28)
// 	{
// 	  if (Z == 6)
// 	    {
// 	      if (A == 9 || A == 8)
// 		{
// 		  Histo->dEE_CsIReactions_28->Fill(CsIenergy);
// 		}
// 	    }
// 	}
//       if (CsIhit == 31)
// 	{
// 	  if (Z == 6)
// 	    {
// 	      if (A == 9 || A == 8)
// 		{
// 		  Histo->dEE_CsIReactions_31->Fill(CsIenergy);
// 		}
// 	    }
// 	}
      if (Z > 0 && A > 0)
	{



	  Solution[Nsolution].penergy = energy;

          energy = light2energy(Z,A,CsIhit,energy);
	  if (energy < 0.) return;


	  float sumEnergy = energy + Sienergy;


	  //energy loss in target
	  int ipid =0;

	  /*
	  if(Z ==1)
	    ipid = A-1;
	  else if(Z==4)
	    ipid = 9;
	  else if (Z==6)
	    {
	      ipid = A+1;
	    }
	  else if(Z==7)
	    {
	      ipid =A+5;	      
	    }
	  else if(Z==8)
	    {
	      ipid = A+8;
	    }
	  else 
	    ipid = A;
	  */

	  //	  cout << "ipid" << ipid << " " << A <<  " " << Z << endl;

	  //float light = Csi.Order[0].energyR;
	  float light = Csi.Order[0].energy; //to calibrate stuff based on proton energy


	  float thick = 193./2./cos(theta);
	  //float Ein = Loss[ipid]->getEin(sumEnergy,thick);
	  float Ein = losses->getEin(sumEnergy,thick,Z,A);
	  float Ekin = Ein;


	  if(Z==3 && A ==6)
	    {
	      //	      Histo->Etot->Fill(Ekin);
	      if (On_Eloss)Histo->ELoss[CsIhit]->Fill(Sienergy);
	      Histo->Light[CsIhit]->Fill(sumEnergy); //adding back energy lost in Be/Si
	    }

	  
	  Solution[Nsolution].energy = energy;
	  Solution[Nsolution].energyR = Csi.Order[0].energyR;
          Solution[Nsolution].energyCsI = Csi.Order[0].energyCsI;
	  Solution[Nsolution].denergy = Sienergy;
	  Solution[Nsolution].ifront = fhit;
	  Solution[Nsolution].iback = bhit;
	  Solution[Nsolution].icsi = Csi.Order[0].strip;
	  Solution[Nsolution].itele = id;
	  Solution[Nsolution].iZ = Z;
	  Solution[Nsolution].iA = A;
	  Solution[Nsolution].mass = getMass(Z,A);
	  Solution[Nsolution].Ekin = Ekin;
	  Solution[Nsolution].theta = theta;
	  Solution[Nsolution].phi = phi;
	  Nsolution=1;

	}
    }
  else if(Csi.Nstore>1 && Front.Nstore >=1 && Back.Nstore >=1)
    {
      multiHitCsi();
    }

   Addfake();
   Addfake_channeling();
   Addfake_CsIreaction();
   Addfake_CsIreaction_C11();
   Addfake_C11inC10();
  //Addfake2();
  //Addfake3();
  //Addfake4();
  getMomentum(); //Adds energytotal and momentum to Solutions
    
}
//*******************************************************
void telescope::reset()
{

  multFront = 0;
  multBack = 0;

  for(int i =0;i<Nsolution;i++)
    {
      Solution[i].penergy = 0;
      Solution[i].energyCsI = 0.;
      Solution[i].energy = 0;
      Solution[i].energyR = 0;
      Solution[i].denergy = 0;
      Solution[i].ifront =0;
      Solution[i].iback = 0;
      Solution[i].icsi = 0;
      Solution[i].itele = 0;
      Solution[i].iZ = 0;
      Solution[i].iA = 0;
      Solution[i].mass = 0;
      Solution[i].Ekin = 0;
      Solution[i].theta = 0;
      Solution[i].phi = 0;
      Solution[i].energyTot = 0.;
      Solution[i].momentum = 0.;
      Solution[i].velocity = 0.;
      Solution[i].Mvect[0] = 0.;
      Solution[i].Mvect[1] = 0.;
      Solution[i].Mvect[2] = 0.;

    }

  Nsolution =0;

  Front.reset();
  Back.reset();
  Csi.reset();



}

//*************************************************************
int telescope::multiHitCsi()
{
  // find number of soultions ,i.e. back and front pairs of strips 
  // with the same energy 
   
  
  int isol = multiHit();
  if (isol <=0) return 0;


  //now assign each of these solutions a Csi detector location 
  int mult[4]={0};  //array for multipility of Si solution for each Csi
  int sil[4][10]; //contains a lits of solutions for each Csi
  for (int i=0;i<Nsolution;i++)
    {
      int ifront = Solution[i].ifront;
      int iback = Solution[i].iback;
      for (int icsi=0;icsi<4;icsi++)
	{
	  if (ifront >= FrontLow[icsi] &&
	      ifront <= FrontHigh[icsi] &&
	      iback  >= BackLow[icsi]  &&
	      iback  <= BackHigh[icsi])
	    {
	      sil[icsi][mult[icsi]] = i;
	      mult[icsi]++;
	      Solution[i].icsi = icsi;
	      break;
	    }
	}
    }



  //make array of detect csi energies
  float energy[4]={-1.};
  float energyR[4]={-1};
  for (int i=0;i<Csi.mult;i++) 
    {
      energy[Csi.Order[i].strip] = Csi.Order[i].energy;
      Csi.Order[i].energyCsI = Csi.Order[i].energyR;
      energyR[Csi.Order[i].strip] = Csi.Order[i].energyR;
    }




  //loop over csi location
  for (int icsi = 0;icsi<4;icsi++)
    
    {
      // no solution for this location, ignore
      if (mult[icsi] == 0) continue;
      // more than one si solution for a single Csi location
      else if (mult[icsi] > 1)
	{
          for (int j=0;j<mult[icsi];j++) 
	    Solution[sil[icsi][j]].ipid = 0;
          continue;
	}
      // only one si solution for this csi location
      else
	{
	  int ii = sil[icsi][0];
	  //now see if this csi fired 
	  if (energy[icsi] <= 0.) 
	    {
	      //no csi recorded for this event
	      //stopped in silicon
	      Solution[ii].energy = 0.;
	      Solution[ii].iZ = 0;
	      continue;
	    }
	  
	  CsIhit = icsi + id*4;

	 
	  if (On_dEE)Histo->dEE[CsIhit]->Fill(energyR[icsi],Solution[ii].denergy);
	    
	  if (CsIhit == 28 && energyR[icsi] > 1800 && energyR[icsi] < 1900 && Solution[ii].ifront < 16)
	    {
	      //  Histo->CsI_28_projections[Solution[ii].ifront]->Fill(Solution[ii].denergy);
	    }
	  if (CsIhit == 40 && energyR[icsi] > 1800 && energyR[icsi] < 1900 && Solution[ii].ifront < 16)
	    {
	      //  Histo->CsI_40_projections[Solution[ii].ifront]->Fill(Solution[ii].denergy);
	    }
	  if (CsIhit == 41 && energyR[icsi] > 1800 && energyR[icsi] < 1900 && Solution[ii].ifront < 16)
	    {
	      // Histo->CsI_41_projections[Solution[ii].ifront]->Fill(Solution[ii].denergy);
	    }
	  if (CsIhit == 42 && energyR[icsi] > 1800 && energyR[icsi] < 1900 && Solution[ii].ifront >= 16)
	    {
	      //  Histo->CsI_42_projections[Solution[ii].ifront - 16]->Fill(Solution[ii].denergy);
	    }
	  if (CsIhit == 43 && energyR[icsi] > 1800 && energyR[icsi] < 1900 && Solution[ii].ifront >= 16)
	    {
	      //  Histo->CsI_43_projections[Solution[ii].ifront - 16]->Fill(Solution[ii].denergy);
	    }

	  //continue;

	  bool stat = Pid[icsi]->getPID(energyR[icsi],Solution[ii].denergy);
	  if(!stat)
	    {
	      Solution[ii].energy =0.;
	      Solution[ii].iZ =0;
	      continue;
	    }
	  int Z = Pid[icsi]->Z;
	  int A = Pid[icsi]->A;



	  int ifront = Solution[ii].ifront;
	  int iback = Solution[ii].iback;
	  
	  theta = Tele.getTheta(ifront,iback); //same concerns as above
	  phi = Tele.phiRecon;

	  if(Z >0 && A>0)
	    {
	      Solution[ii].penergy = energy[icsi];

	      energy[icsi] = light2energy(Z,A,CsIhit,energy[icsi]);

	      float sumenergy = energy[icsi] + Solution[ii].denergy;

	      /*
	      //energy loss in target
	      int ipid =0;
	      
	      if(Z ==1)
		ipid = A-1;
	      else if(Z==4)
		ipid = 9;
	      else if (Z==6)
		{
		  ipid = A+1;
		}
	      else if(Z==7)
		{
		  ipid =A+5;	      
		}
	      else if(Z==8)
		{
		  ipid = A+8;
		}
	      else 
		ipid = A;
	      */	      

	      float thick = 193./2./cos(theta);
	      //float Ein = Loss[ipid]->getEin(sumenergy,thick);
	      float Ein = losses->getEin(sumenergy,thick,Z,A);
	      float Ekin = Ein;

	      Solution[ii].energy = energy[icsi];
	      Solution[ii].energyR = energyR[icsi];
	      Solution[ii].icsi = Csi.Order[ii].strip;
              Solution[ii].energyCsI = Csi.Order[ii].energyCsI;
	      Solution[ii].iZ = Z;
	      Solution[ii].iA = A;
	      Solution[ii].mass = getMass(Z,A);
	      Solution[ii].Ekin = Ekin;
	      Solution[ii].theta = theta;
	      Solution[ii].phi = phi;
	      Solution[ii].itele = id;

	    }
	}
    }
  
  return 1;
}
//****************************************************
//recursive subroutine  used for multihit subroutine
void telescope::loop(int depth)
{
  if (depth == NestDim )
    {
      // do stuff here
      int dstrip = 0;
      float de = 0.;
      for (int i=0;i<NestDim;i++)
	{
	  benergy = Back.Order[NestArray[i]].energy;
	  fenergy = Front.Order[i].energy;
          de += abs(benergy-fenergy);
	}


      if (dstrip < dstripMin)
	{
          dstripMin = dstrip;
          for (int i=0;i<NestDim;i++) 
	    arrayD[i] = NestArray[i];
	}


      if (de < deMin)
	{
          deMin = de;
          for (int i=0;i<NestDim;i++) 
	    arrayB[i] = NestArray[i];
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

//***************************************************
//extracts multiple particle from strip data 
int telescope::multiHit()
{
  int Ntries = min(Front.Nstore,Back.Nstore);
  if (Ntries > 4) Ntries =4;
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
	  benergy = Back.Order[arrayB[i]].energy;
	  fenergy = Front.Order[i].energy;
	  float accept = 0.2;
	  if(fenergy < 10.) accept =1.5/fenergy;
	  
	  if (fabs(benergy-fenergy) >fenergy*accept)
	    {
	      leave = 1;
	      break;
	    }
	}
      
      if (leave) continue;
      // now load solution
      for (int i=0;i<NestDim;i++)
	{
	  fenergy = Front.Order[i].energy;
	  Solution[i].denergy = fenergy;
	  Solution[i].ifront = Front.Order[i].strip;
	  Solution[i].iback = Back.Order[arrayB[i]].strip;
          Solution[i].itele = id;
        }

      Nsolution = NestDim;
      
      break;
    }

  return Nsolution;
}

//***********************************************************
void telescope::load(int F0low, int F1low,int F2low, int F3low,
		     int F0hi,  int F1hi, int F2hi,  int F3hi,
		     int B0low, int B1low,int B2low, int B3low,
		     int B0hi,  int B1hi, int B2hi,  int B3hi)
{
  FrontLow[0] = F0low;
  FrontLow[1] = F1low;
  FrontLow[2] = F2low;
  FrontLow[3] = F3low;

  FrontHigh[0] = F0hi;
  FrontHigh[1] = F1hi;
  FrontHigh[2] = F2hi;
  FrontHigh[3] = F3hi;

  BackLow[0] = B0low;
  BackLow[1] = B1low;
  BackLow[2] = B2low;
  BackLow[3] = B3low;

  BackHigh[0] = B0hi;
  BackHigh[1] = B1hi;
  BackHigh[2] = B2hi;
  BackHigh[3] = B3hi;

}
//***************************************************************
void telescope::Addfake()
{
  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 6 && Solution[i].iA ==10)
	{
	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = Solution[i].penergy;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE = 0.;
	  float dE0 = Solution[i].denergy;
	  float dE_loss = 0.;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)500,CsI0);
	      dE_loss = ran->Rndm()*min((float)40,dE0);

	      CsIE = CsI0;	      

	      dE = dE0 - dE_loss;
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 6 && A ==9)
		    {
		      break;
		    }
		}
   
	      counter++;
	      if(counter ==100) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	
	  fakeE = calCsiC9->getEnergy(0,CsiHit,fakeE);

	  float sumEnergy = fakeE + dE;

	  //now mostly channeling so use the original Si and Csi energy
	  //not quite right right as 9C and 10C have different
	  // energy calibrations
	  // sumEnergy = fakeE + Solution[i].denergy;
	  float thick = 193./2./cos(theta);

	  float Ein = losses->getEin(sumEnergy,thick,6,9);
	  //float Ein = Loss[15]->getEin(sumEnergy,thick);
	  float Ekin = Ein;
	  Solution[Nsolution].Ekin = Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 99;
	  Solution[Nsolution].iA = 99;
	  Solution[Nsolution].mass =getMass(6,9);
	  if(counter !=100) Nsolution++;

	}

    }

}

void telescope::Addfake_channeling()
{
  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 6 && Solution[i].iA ==10)
	{
	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;
          Solution[Nsolution].energyCsI = Solution[i].energyCsI;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = Solution[i].energy;
	  int counter = 0;
	  float CsIE = 0.;
	  float CsIE_channeling = 0.;
	  float CsIE_channeling_penergy = 0.;
	  float dE0 = Solution[i].denergy;
	  float dE = 0.;
	  float dE_loss = 0.;

	  for(;;)
	    {
	      dE_loss = ran->Rndm()*min((float)40,dE0);

	      fakeE = Solution[i].energy + dE_loss;

	      fakeE = calCsiC10->reverseCal(0,Solution[i].icsi + id*4,fakeE); //going from ion energy to proton energy

	      CsIE = calCsi->reverseCal(0,Solution[i].icsi + id*4,fakeE); //going to ADC channel
	     

	      dE = dE0 - dE_loss;
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 6 && A ==9)
		    {
		      break;
		    }
		}
	      
	      counter++;
	      if(counter ==1000) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = calCsiC9->getEnergy(0,CsiHit,fakeE);

	  float sumEnergy = fakeE + dE;

	  //now mostly channeling so use the original Si and Csi energy
	  //not quite right right as 9C and 10C have different
	  // energy calibrations
	  // sumEnergy = fakeE + Solution[i].denergy;
	  float thick = 193./2./cos(theta);

	  float Ein = losses->getEin(sumEnergy,thick,6,9);
	  //float Ein = Loss[15]->getEin(sumEnergy,thick);
	  float Ekin = Ein;
	  Solution[Nsolution].Ekin = Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 89;
	  Solution[Nsolution].iA = 99;
	  Solution[Nsolution].mass = getMass(6,9);
	  if(counter !=1000) Nsolution++;

	}

    }
}

void telescope::Addfake_CsIreaction()
{
  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 6 && Solution[i].iA ==10)
	{
	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = Solution[i].penergy;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE = 0.;
	  float dE0 = Solution[i].denergy;
	  float E_loss = 0.;
	  int channeling = 0;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)500,CsI0);
	      E_loss = ran->Rndm()*min((float)500,CsI0);

	      CsIE = CsI0 - E_loss;	      
	      

	      dE = dE0;
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 6 && A ==9)
		    {
		      break;
		    }
		}
   
	      counter++;
	      if(counter ==1000) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = calCsi->getEnergy(0,CsiHit,CsIE);
	  fakeE = calCsiC9->getEnergy(0,CsiHit,fakeE);

	  float sumEnergy = fakeE + dE;

	  //now mostly channeling so use the original Si and Csi energy
	  //not quite right right as 9C and 10C have different
	  // energy calibrations
	  // sumEnergy = fakeE + Solution[i].denergy;
	  float thick = 193./2./cos(theta);

	  float Ein = losses->getEin(sumEnergy,thick,6,9);
	  //float Ein = Loss[15]->getEin(sumEnergy,thick);
	  float Ekin = Ein;
	  Solution[Nsolution].Ekin = Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 79;

	  Solution[Nsolution].iA = 99;
	  Solution[Nsolution].mass = getMass(6,9);
	  if(counter !=1000) Nsolution++;

	}

    }

}

void telescope::Addfake_CsIreaction_C11()
{
  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 6 && Solution[i].iA ==11)
	{
	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = Solution[i].penergy;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE = 0.;
	  float dE0 = Solution[i].denergy;
	  float E_loss = 0.;
	  int channeling = 0;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)800,CsI0);
	      E_loss = ran->Rndm()*min((float)500,CsI0);

	      CsIE = CsI0 - E_loss;	      
	      

	      dE = dE0;
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 6 && A ==9)
		    {
		      break;
		    }
		}
   
	      counter++;
	      if(counter ==1000) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = calCsi->getEnergy(0,CsiHit,CsIE);
	  fakeE = calCsiC9->getEnergy(0,CsiHit,fakeE);

	  float sumEnergy = fakeE + dE;

	  //now mostly channeling so use the original Si and Csi energy
	  //not quite right right as 9C and 10C have different
	  // energy calibrations
	  // sumEnergy = fakeE + Solution[i].denergy;
	  float thick = 193./2./cos(theta);

	  float Ein = losses->getEin(sumEnergy,thick,6,9);
	  //float Ein = Loss[15]->getEin(sumEnergy,thick);
	  float Ekin = Ein;
	  Solution[Nsolution].Ekin = Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 69; //nice

	  Solution[Nsolution].iA = 99;
	  Solution[Nsolution].mass = (6,9);
	  if(counter !=1000) Nsolution++;

	}

    }

}
void telescope::Addfake_C11inC10()
{
  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 6 && Solution[i].iA ==11)
	{
	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = Solution[i].penergy;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE = 0.;
	  float dE0 = Solution[i].denergy;
	  float E_loss = 0.;
	  int channeling = 0;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)800,CsI0);
	      E_loss = ran->Rndm()*min((float)500,CsI0);

	      CsIE = CsI0 - E_loss;	      
	      

	      dE = dE0;
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 6 && A ==10)
		    {
		      break;
		    }
		}
   
	      counter++;
	      if(counter ==1000) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = calCsi->getEnergy(0,CsiHit,CsIE);
	  fakeE = calCsiC10->getEnergy(0,CsiHit,fakeE);

	  float sumEnergy = fakeE + dE;

	  //now mostly channeling so use the original Si and Csi energy
	  //not quite right right as 9C and 10C have different
	  // energy calibrations
	  // sumEnergy = fakeE + Solution[i].denergy;
	  float thick = 193./2./cos(theta);

	  float Ein = losses->getEin(sumEnergy,thick,6,9);
	  //float Ein = Loss[15]->getEin(sumEnergy,thick);
	  float Ekin = Ein;
	  Solution[Nsolution].Ekin = Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 59; //nice

	  Solution[Nsolution].iA = 99;
	  Solution[Nsolution].mass = (6,10);
	  if(counter !=1000) Nsolution++;

	}

    }

}
//***************************************************************
void telescope::Addfake2()
{

  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 7 && Solution[i].iA ==14)
	{
	  //cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = 0.;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE0 = Solution[i].denergy;
	  float dE = 0.;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)500,CsI0);
	      CsIE = CsI0;

	      dE = dE0 - ran->Rndm()*min((float)50,dE0);
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 7 && A ==13)
		    {
		      break;
		    }

		}
	      counter++;
	      if(counter ==100) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = Solution[i].energy;

	  float sumEnergy = fakeE + dE;
	  float thick = 193./2./cos(theta);

	  //float Ein = Loss[15]->getEin(sumEnergy,thick);
	  float Ein = losses->getEin(sumEnergy,thick,7,13);
	  float Ekin = Ein;

	  Solution[Nsolution].Ekin =Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 98;
	  Solution[Nsolution].iA = 98;
	  Solution[Nsolution].mass =13;
	  if(counter !=100) Nsolution++;

	}

    }


}
//***************************************************************
void telescope::Addfake3()
{


  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 8 && Solution[i].iA ==14)
	{


	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = 0.;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE0 = Solution[i].denergy;
	  float dE = 0.;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)500,CsI0);
	      CsIE = CsI0;

	      dE = dE0 - ran->Rndm()*min((float)50,dE0);
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 8 && A ==13)
		    {
		      break;
		    }

		}
	      counter++;
	      if(counter ==100) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = Solution[i].energy;

	  float sumEnergy = fakeE + dE;
	  float thick = 193./2./cos(theta);

	  //float Ein = Loss[19]->getEin(sumEnergy,thick);
	  float Ein = losses->getEin(sumEnergy,thick,8,13);
	  float Ekin = Ein;



	  Solution[Nsolution].Ekin =Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 97;
	  Solution[Nsolution].iA = 97;
	  Solution[Nsolution].mass =13;
	  if(counter !=100) Nsolution++;

	}

    }


}

//***************************************************************
void telescope::Addfake4()
{


  for(int i =0;i<Nsolution;i++)
    {
      if(Solution[i].iZ == 8 && Solution[i].iA ==15)
	{


	  // cout << "solution = " << Nsolution << endl;
	  Solution[Nsolution].ifront = Solution[i].ifront;
	  Solution[Nsolution].iback = Solution[i].iback;
	  Solution[Nsolution].icsi = Solution[i].icsi;
	  Solution[Nsolution].itele = Solution[i].itele;
	  Solution[Nsolution].theta = Solution[i].theta;
	  Solution[Nsolution].phi = Solution[i].phi;
	  Solution[Nsolution].denergy = Solution[i].denergy;

	  float CsI0 = Solution[i].energyR;
	  float fakeE = 0.;
	  int counter = 0;
	  float CsIE = 0.;
	  float dE0 = Solution[i].denergy;
	  float dE = 0.;
	  for(;;)
	    {
	      // CsIE = CsI0 - ran->Rndm()*min((float)500,CsI0);
	      CsIE = CsI0;

	      dE = dE0 - ran->Rndm()*min((float)50,dE0);
	      //dE = dE0;

	      bool stat = Pid[Solution[i].icsi]->getPID(CsIE,dE);
	      if(stat)
		{
		  int Z = Pid[Solution[i].icsi]->Z;
		  int A = Pid[Solution[i].icsi]->A;
		  if(Z == 8 && A ==13)
		    {
		      break;
		    }

		}
	      counter++;
	      if(counter ==100) break;
	    }

	  //if(counter > 30)cout << "counter " <<counter << endl;
	  int CsiHit = Solution[i].icsi + id*4;
	 
	  //fakeE  =calCsi->getEnergy(0,CsiHit,CsIE);
	  // fakeE = calCsiO14->getEnergy(0,CsiHit,fakeE);

	  fakeE = Solution[i].energy;

	  float sumEnergy = fakeE + dE;
	  float thick = 193./2./cos(theta);

	  //float Ein = Loss[19]->getEin(sumEnergy,thick);
	  float Ein = losses->getEin(sumEnergy,thick,8,13);
	  float Ekin = Ein;



	  Solution[Nsolution].Ekin =Ekin;
	  Solution[Nsolution].energy = fakeE;
	  Solution[Nsolution].denergy = dE;
	  Solution[Nsolution].energyR = CsIE;
	  Solution[Nsolution].iZ = 96;
	  Solution[Nsolution].iA = 96;
	  Solution[Nsolution].mass =13;
	  if(counter !=100) Nsolution++;

	}

    }


}



//**********************************************************
void telescope::getMomentum()
{
  for(int i = 0;i<Nsolution;i++)
    {	

      float theta = Solution[i].theta;
      float phi = Solution[i].phi;
      float momentum;
      if (relativity)
	{
	  Solution[i].mass *= 931.478;
	  momentum = Solution[i].Kinematics.
	    getMomentum(Solution[i].Ekin,Solution[i].mass);
	  Solution[i].energyTot = Solution[i].Ekin + Solution[i].mass;
		  
	}
      else
	{
	  momentum = sqrt(2.*Solution[i].mass*Solution[i].Ekin);
	  Solution[i].mass = 0.;
	}
	      
      Solution[i].Mvect[0] = momentum*sin(theta)*cos(phi);
      Solution[i].Mvect[1] = momentum*sin(theta)*sin(phi);
      Solution[i].Mvect[2] = momentum*cos(theta); 
      Solution[i].momentum = sqrt(pow(Solution[i].Mvect[0],2)
				  +pow(Solution[i].Mvect[1],2)
				  +pow(Solution[i].Mvect[2],2));
      Solution[i].velocity = Solution[i].momentum/Solution[i].energyTot;
	      
    }

}
//***************************************************
// converstion of equilivant proton energy to energy for a given isotope
//i.e. Z and A dependence of CsI light output
float telescope::light2energy(int Z, int A, int CsiHit, float energy)
{
	  
	  if(Z ==1)
	    { 
	      if(A ==2)
		energy = calCsid->getEnergy(0,CsIhit,energy);
	      if(A ==3)
		energy = calCsit->getEnergy(0,CsIhit,energy);
	    }
	  else if(Z == 2)
	    {
	      if(A ==3)
		energy = calCsiHe3->getEnergy(0,CsIhit,energy);
	      else if(A ==4)
		energy = calCsiA->getEnergy(0,CsIhit,energy);
	      else 
		{
		  //cout << "found no calib for " << Z << " " << A << endl; 
		  return -1.;
		}
	    }
	  else if (Z == 3) 
	    {
	      energy = calCsiLi6->getEnergy(0,CsIhit,energy);
	  
	    }
	  else if(Z ==4)
	    {
	      if(A ==7)
		energy = calCsiBe7->getEnergy(0,CsIhit,energy);
	      else 
		{
		  //		  cout << "found no calib for" << Z << " " << A << endl; 
		  //abort();
		}
	    }
	  else if(Z ==5)
	    {
	      if(A==8)
		energy = calCsiB10->getEnergy(0,CsIhit,energy);
	      if(A==10)
		energy = calCsiB10->getEnergy(0,CsIhit,energy);
	      else if(A==11)
		energy = calCsiB11->getEnergy(0,CsIhit,energy);
	      else 
		{
		  //cout << "found no calib for" << Z << " " << A << endl; 
		  //abort();
		}
	    }
	  else if(Z ==6)
	    {
	      if(A ==8)
		energy = calCsiC8->getEnergy(0,CsIhit,energy);
	      else if(A == 9)
		energy = calCsiC9->getEnergy(0,CsIhit,energy);
	      else if(A == 10)
		energy = calCsiC10->getEnergy(0,CsIhit,energy);
	      else if(A == 11)
		energy = calCsiC11->getEnergy(0,CsIhit,energy);
	      else if(A ==12)
		energy = calCsiC12->getEnergy(0,CsIhit,energy);
	      else if(A==13)
		energy = calCsiC13->getEnergy(0,CsIhit,energy);
	      else 
		{
		  cout << "found no calib for" << Z << " " << A << endl; 
		  abort();
		}
	    }
	  else if(Z==8)
	    {
	      if(A==13)
		energy = calCsiO13->getEnergy(0,CsIhit,energy);
	      else if(A==14)
		energy = calCsiO14->getEnergy(0,CsIhit,energy);
	      else if(A==15)
		energy = calCsiO15->getEnergy(0,CsIhit,energy);
	      else if(A==16)
		energy = calCsiO16->getEnergy(0,CsIhit,energy);
	      else 
		{
		  cout << "found no calib for" << Z << " " << A << endl; 
		  abort();
		}
	
	    }
	  else if(Z==7)
	    {
	      if(A==12)
		energy = calCsiN12->getEnergy(0,CsIhit,energy);
	      else if(A==13)
		energy = calCsiN13->getEnergy(0,CsIhit,energy);
	      else if(A==14)
		energy = calCsiN14->getEnergy(0,CsIhit,energy);
	      else if(A==15)
		energy = calCsiN15->getEnergy(0,CsIhit,energy);
	      else 
		{
		  cout << "found no calib for" << Z << " " << A << endl; 
		  abort();
		}
	    }

     return energy;
}

void telescope::findVectors(float *rcenter, float *rback, float *rdiag, float *rnormal, float *rfront, float xcenter0, float ycenter0, float zcenter0, float xhoriz0, float yhoriz0, float zhoriz0, float xdiag0, float ydiag0, float zdiag0)
{
  float m_to_cm = 100.; //to convert from meters to centimeters

  rcenter[0] = m_to_cm*xcenter0;
  rcenter[1] = m_to_cm*ycenter0;
  rcenter[2] = m_to_cm*zcenter0;

  float rfront_mag = sqrt(pow(xhoriz0,2) + pow(yhoriz0,2) + pow(zhoriz0,2));

  rfront[0] = xhoriz0/rfront_mag; //go left to right (beam right point - beam left point)
  rfront[1] = yhoriz0/rfront_mag; 
  rfront[2] = zhoriz0/rfront_mag;

  rdiag[0] = -xdiag0; // beam left point  - beam right point (this is wrong, this is a later comment)
  rdiag[1] = -ydiag0; 
  rdiag[2] = -zdiag0;

  rnormal[0] = rfront[1]*rdiag[2] - rfront[2]*rdiag[1];
  rnormal[1] = rfront[2]*rdiag[0] - rfront[0]*rdiag[2];
  rnormal[2] = rfront[0]*rdiag[1] - rfront[1]*rdiag[0];

  float rnormal_mag = sqrt(pow(rnormal[0],2) + pow(rnormal[1],2) + pow(rnormal[2],2));

  rnormal[0] = rnormal[0]/rnormal_mag;
  rnormal[1] = rnormal[1]/rnormal_mag;
  rnormal[2] = rnormal[2]/rnormal_mag;

  //rback = rfront x rnormal

  rback[0] = rfront[1]*rnormal[2] - rfront[2]*rnormal[1];
  rback[1] = rfront[2]*rnormal[0] - rfront[0]*rnormal[2];
  rback[2] = rfront[0]*rnormal[1] - rfront[1]*rnormal[0];

  float rback_mag = sqrt(pow(rback[0],2) + pow(rback[1],2) + pow(rback[2],2));

  rback[0] = rback[0]/rback_mag;
  rback[1] = rback[1]/rback_mag;  
  rback[2] = rback[2]/rback_mag;
}

double telescope::getMass(int iZ,int iA)
{
  if (iZ == 1)
    {
      if (iA == 1) return 1.+ 7.2889/m0;
      else if (iA == 2) return 2. + 13.135/m0;
      else if (iA == 3) return 3.+ 14.949/m0;
      else
	{
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
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
         cout << iZ << " " << iA << endl;
         abort(); 
	}
    }
  else if (iZ == 9)
    {
      if (iA == 17) return 17.+1.951/m0;
      else if (iA == 18) return 18.  + .873/m0;
      else 
	{
         cout << iZ << " " << iA << endl;
         abort(); 
	}
    }
  else if (iZ == 10)
    {
      if (iA == 17) return 17. +16.500/m0;
      else if (iA == 18) return 18. + 5.317/m0;
      else 
	{
         cout << iZ << " " << iA << endl;
         abort();
	}
    }
  else 
    {
      cout << iZ << " " << iA << endl;
     abort();
    }

}
