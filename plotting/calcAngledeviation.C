void calcAngledeviation()
{
  TCanvas *c1 = new TCanvas("c1", "compare");

  gStyle->SetOptStat(11);

  string histname = "InvMass/6Li/Erel_da_cosThetaH";

  TFile *input = new TFile("../sort.root", "read");
  TH2I *hist = (TH2I*)input->Get(histname.c_str());

  //Erel_da_cosThetaH = new TH2I("Erel_da_cosThetaH","",200,0,3,25,-1,1);

  TH1D * proj[25];
  double meanval[25];
  double errmeanval[25];
  double costhetaH[25];
  double errcosthetaH[25];

  ostringstream name;
  for (int i=0;i<25;i++)
  {
    name.str("");
    name << "_px" << i;
    proj[i] = hist->ProjectionX(name.str().c_str(),i+1,i+2);

    TF1 *peak = new TF1("peak","gaus(0)",0.65,0.8); //setting used for timing
    proj[i]->Fit("peak","R","",0.65,0.8); //setting used for timing

    proj[i]->Draw();

    cout << peak->GetParameter(0) << " " << peak->GetParameter(1) << " " << peak->GetParameter(2) << endl;
    meanval[i] = peak->GetParameter(1) - 0.7117; //subtract off the value Erel should be
    errmeanval[i] = peak->GetParameter(2);
    costhetaH[i] = ((TAxis*)hist->GetYaxis())->GetBinCenter(i)+0.08;
    cout << meanval[i] << " " << costhetaH[i] << endl;
    errcosthetaH[i] = 0.08/2;
  }

  TGraph * Angledeviation = new TGraphErrors(25,costhetaH,meanval, errcosthetaH, errmeanval);
  Angledeviation->GetYaxis()->SetRangeUser(-0.05,0.12); 
  Angledeviation->Draw();

}
