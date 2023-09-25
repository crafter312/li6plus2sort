#include <algorithm>
#include <iostream>
void timePulser()
{

  const int nbins = 2000; //4000 for HG Front
  const int nbinslg = 2000;
  float source[nbins] = {0.};
  float source2[nbinslg] = {0.};

  TFile *in = new TFile("../root/TimeCalib.root");
  ofstream ofile("timePulser_back.cal"); // Change
  ofstream ofile2("timePulser_backLG.cal");

  gROOT->cd();
  TCanvas *mycan = (TCanvas*)gROOT->FindObject("mycan");
  if(!mycan)
    {
      mycan = new TCanvas("mycan","mycan");
    }

  string name;
  ostringstream outstring;

  float Reflow = 0;
  float Refhi = 0;

  for(int itele =0;itele <14;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  outstring.str("");
	  outstring << "back/time/TB" << itele << "_" << istrip; // Change
	  name = outstring.str();
	  TH1I  *myhist = (TH1I*)in->Get(name.c_str());
	  myhist->Draw();
	  myhist->GetXaxis()->SetRangeUser(000,16000);
	  mycan->Modified();
	  mycan->Update();
	  TSpectrum *myS = new TSpectrum();
	  for(int i=0;i< nbins;i++)source[i]=myhist->GetBinContent(i+1);
	  myS->Search(myhist,3);
	  mycan->Update();
	  float *pos = myS->GetPositionX();

	  sort(pos,pos + myS->GetNPeaks());

	  //write itele, strip, slope ,int

	  float slope =0.;
	  float inter =0.;
	  int NN = myS->GetNPeaks();

	  if(itele ==0 && istrip ==0)
	    {
	      Reflow = pos[NN-1];
	      Refhi = pos[NN];
	    }

	  slope = (Refhi-Reflow)/(pos[NN]-pos[NN-1]); 
	  inter = Reflow - slope*pos[NN-1];

	  // if(slope >10.)
	  //  {
	  //    slope = 1.;
	  //    inter = 0.;
	  //  }

	  ofile << itele << " " << istrip << " ";
	  ofile << slope << " " << inter << endl;

	  if(NN =0) cout << "HEY" << endl;

	  if(0)
	    {
	      TMarker * mark;
	      mark=(TMarker*)mycan->WaitPrimitive("TMarker");
	      delete mark;
	    }

	  if(itele ==6 || itele ==7)
	    {
	      outstring.str("");
	      outstring << "back/timelg/TBLG" << itele << "_" << istrip; // Change
	      name = outstring.str();
	      TH1I  *myhist2 = (TH1I*)in->Get(name.c_str());
	      myhist2->Draw();
	      myhist2->GetXaxis()->SetRangeUser(9000,16000);
	      mycan->Modified();
	      mycan->Update();
	      TSpectrum *myS2 = new TSpectrum();
	      for(int i=0;i< nbinslg;i++)source2[i]=myhist2->GetBinContent(i+1);
	      myS2->Search(myhist2,3);
	      mycan->Update();
	      float *pos2 = myS2->GetPositionX();

	      sort(pos2,pos2 + myS2->GetNPeaks());

	      //write itele, strip, slope ,int
	      
	      slope =0;
	      inter =0;
	      NN = 0;
	      NN = myS2->GetNPeaks();
	      
	      
	      slope = (Refhi-Reflow)/(pos2[NN]-pos2[NN-1]); 
	      inter = Reflow - slope*pos2[NN-1];
	      if(slope >10.)
		{
		  slope = 1.;
		  inter = 0.;
		}



	      ofile2 << itele << " " << istrip << " ";
	      ofile2 << slope << " " << inter << endl;
	      if(0)
		{
		  TMarker * mark;
		  mark=(TMarker*)mycan->WaitPrimitive("TMarker");
		  delete mark;
		}

	    }
	}
    }
  
  mycan->Update();
  return;
}
