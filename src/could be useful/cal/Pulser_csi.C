#include <algorithm>
#include <iostream>
void Pulser_csi()
{

  const int nbins = 4000;
  float source[nbins] = {0.};

  TFile *in = new TFile("../root/CsI_Pulser_Old.root"); // Change
  ofstream ofile("CsI_Pulser_OldPedestal.dat"); // Change

  gROOT->cd();
  TCanvas *mycan = (TCanvas*)gROOT->FindObject("mycan");
  if(!mycan)
    {
      mycan = new TCanvas("mycan","mycan");
      mycan->SetRightMargin(0.05);
      mycan->SetTopMargin(0.02);
      mycan->SetTicks(1,1);
      mycan->SetLogy();
    }

  string name;
  ostringstream outstring;

      for(int istrip = 0;istrip<56;istrip++)
	{
	  outstring.str("");
	  outstring << "CsI/CsIRaw/ECsI" << "_" << istrip; // Change
	  name = outstring.str();
	  TH1I  *myhist = (TH1I*)in->Get(name.c_str());
	  myhist->Draw();
	  myhist->GetXaxis()->SetRangeUser(100,525);
	  mycan->Modified();
	  mycan->Update();
	  TSpectrum *myS = new TSpectrum();
	  for(int i=0;i< nbins;i++)source[i]=myhist->GetBinContent(i+1);
	  myS->Search(myhist,1,"",0.0001);
	  mycan->Update();
	  float *pos = myS->GetPositionX();

	  sort(pos,pos + myS->GetNPeaks());
	  ofile << istrip << "  " << myS->GetNPeaks();
	  for(int aa = 0;aa < myS->GetNPeaks();aa++)
	    ofile << pos[aa] << " " ;
	  ofile << endl;

	  TMarker * mark;
	  mark=(TMarker*)mycan->WaitPrimitive("TMarker");
	  delete mark;
	}

	  
  mycan->Update();
  return;
}
