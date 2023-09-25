#include "ring.h"

ringC::ringC(TRandom * ran0, histo_sort * Histo1)
{
  ran = ran0;
  Histo = Histo1;
 
}
//*****************************************
void ringC::reset()
{
  Pie.reset();
  Strip.reset();
  Csi.reset();
}

//*************************************
void ringC::analysis()
{

  float r_min = 1.;
  float r_max = 13.01/2.;
  int Npie = 128;
  int Nstrip = 256;
  float dist = 40.;
  
  Nsolution = 0;
  //must have pie, strip, and Csi Information
    if (!(Pie.Nstore >0 && Strip.Nstore > 0 && Csi.Nstore >0)) return;

  //check mapping
  if (Csi.Nstore == 1)
    {
      int id_csi = Csi.Order[0].strip;
      if (id_csi == 0) Histo->csi0->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 4) Histo->csi4->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 8) Histo->csi8->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 12) Histo->csi12->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 16) Histo->csi16->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 17) Histo->csi17->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 18) Histo->csi18->Fill(Pie.Order[0].strip,Strip.Order[0].strip);
      if (id_csi == 19) Histo->csi19->Fill(Pie.Order[0].strip,Strip.Order[0].strip);       }

  
  //sum neighboring strips in ring
   Strip.Neighbours();

   int NsiHits = min(Pie.Nstore,Strip.Nstore);

   //for now only consider multiplicity 2 at most
   if (NsiHits > 2) NsiHits = 2;

   // match pies and strips   
   multiHit();

   
      
   // match Si and Csi
   for (int i=0;i<Nsolution;i++)
     {

       float radius = (float)Solution[i].iStrip/(float)Nstrip*(r_max-r_min)+r_min;
       Solution[i].theta = atan(radius/dist);
       Solution[i].phi = (float)Solution[i].iPie/(float)Npie*acos(-1.)*2.;
       
       int id_csi;
       if (Solution[i].iStrip < Nstrip/2) //inner ring
	 {

	   id_csi = floor((float)Solution[i].iPie/32.) + 16;
	 }
       else // outer ring
	 {
	   id_csi = floor((float)(Solution[i].iPie+4)/8.) ;
	   if (id_csi == 16) id_csi = 0.;
	 }

       bool found = false;
       for (int icsi = 0;icsi<Csi.Nstore;icsi++)
         {
           if (Csi.Order[icsi].strip == id_csi)
	     {
    	   Solution[i].energy = Csi.Order[icsi].energy;
	   found = true;

	   
      	   continue;
       	 

	     }

	 }
     }
}

//***************************************************
//extracts multiple particle from strip data 
int ringC::multiHit()
{
  int Ntries = min(Strip.Nstore,Pie.Nstore);
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
	  benergy = Pie.Order[arrayB[i]].energy;
	  fenergy = Strip.Order[i].energy;
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
	  fenergy = Pie.Order[i].energy;
	  Solution[i].denergy = fenergy;
	  Solution[i].iStrip = Strip.Order[i].strip;
	  Solution[i].iPie= Pie.Order[arrayB[i]].strip;
        }

      Nsolution = NestDim;
      
      break;
    }

  return Nsolution;
}

//***************************************************
  //recursive subroutine  used for multihit subroutine
void ringC::loop(int depth)
{
  if (depth == NestDim )
    {
      // do stuff here
      int dstrip = 0;
      float de = 0.;
      for (int i=0;i<NestDim;i++)
	{
	  benergy = Pie.Order[NestArray[i]].energy;
	  fenergy = Strip.Order[i].energy;
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
//********************************************
