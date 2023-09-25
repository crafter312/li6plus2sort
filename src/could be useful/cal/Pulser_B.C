#include <algorithm>
#include <iostream>
void Pulser_B()
{

  const int nbins = 4000;
  float source[nbins] = {0.};

  TFile *in = new TFile("../root/Pulser_MB3.root"); // Change
  ofstream ofile("Si_Pulser_Back_new.dat"); // Change

  gROOT->cd();
  TCanvas *mycan = (TCanvas*)gROOT->FindObject("mycan");
  if(!mycan)
    {
      mycan = new TCanvas("mycan","mycan");
      mycan->SetRightMargin(0.05);
      mycan->SetTopMargin(0.02);
      mycan->SetTicks(1,1);
    }

  string name;
  ostringstream outstring;

  for(int itele =6;itele <8;itele++)
    {
      for(int istrip = 0;istrip<32;istrip++)
	{
	  outstring.str("");
	  outstring << "back/lgr/EBLG" << itele << "_" << istrip; // Change
	  name = outstring.str();
	  TH1I  *myhist = (TH1I*)in->Get(name.c_str());
	  myhist->Draw();
	  myhist->GetXaxis()->SetRangeUser(0,16000);
	  mycan->Modified();
	  mycan->Update();
	  TSpectrum *myS = new TSpectrum();
	  for(int i=0;i< nbins;i++)source[i]=myhist->GetBinContent(i+1);
	  myS->Search(myhist,6);
	  mycan->Update();
	  float *pos = myS->GetPositionX();

	  sort(pos,pos + myS->GetNPeaks());
	  ofile << itele << " " << istrip << "  " << myS->GetNPeaks()<< " ";
	  for(int aa = 0;aa < myS->GetNPeaks();aa++)
	    ofile << pos[aa] << " " ;
	  ofile << endl;

	  TMarker * mark;
	  mark=(TMarker*)mycan->WaitPrimitive("TMarker");
	  delete mark;
	}
    }

	  
  mycan->Update();
  return;
}
