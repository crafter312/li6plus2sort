//written by nicolas dronchi to plot gamma spectrum and inv-mass spectrum next to eachother
void CalibrationResonance()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  TStyle * Sty = (TStyle*)gROOT->FindObject("MyStyle");
  if(!Sty)
    {
      Sty = new TStyle("MyStyle","MyStyle");
    }
  Sty->SetOptTitle(0);
  Sty->SetOptStat(0);
  Sty->SetPalette(1,0);
  Sty->SetCanvasColor(10);
  Sty->SetCanvasBorderMode(0);
  Sty->SetFrameLineWidth(3);
  Sty->SetFrameFillColor(10);
  Sty->SetPadColor(10);
  Sty->SetPadTickX(1);
  Sty->SetPadTickY(1);
  Sty->SetPadBottomMargin(.17);
  Sty->SetPadTopMargin(.03);
  Sty->SetPadLeftMargin(.17);
  Sty->SetPadRightMargin(.05);
  Sty->SetHistLineWidth(3);
  Sty->SetHistLineColor(kBlack);
  Sty->SetFuncWidth(3);
  Sty->SetFuncColor(kRed);
  Sty->SetLineWidth(3);
  Sty->SetLabelSize(0.06,"xyz");
  Sty->SetLabelOffset(0.02,"y");
  Sty->SetLabelOffset(0.02,"x");
  Sty->SetLabelColor(kBlack,"xyz");
  Sty->SetTitleSize(0.06,"xyz");
  Sty->SetTitleOffset(1.35,"y");
  Sty->SetTitleOffset(1.1,"x");
  Sty->SetTitleFillColor(10);
  Sty->SetTitleTextColor(kBlack);
  Sty->SetTickLength(.03,"xz");
  Sty->SetTickLength(.02,"y");
  Sty->SetNdivisions(210,"x");
  Sty->SetNdivisions(210,"yz");
  Sty->SetEndErrorSize(10);
  gROOT->SetStyle("MyStyle");
  gROOT->ForceStyle();

  TCanvas * mycan = new TCanvas("mycan","",550,1440);
  //TCanvas * mycan = new TCanvas("mycan","",1280,960);
  //mycan->Divide(1,3,0.0000001,0.0000001);
  mycan->Divide(1,4,0.0000001,0.0000001);
  gStyle->SetOptStat(0);

//Li6 = d + a
  mycan->cd(1);

  string He6beam = "../sort_He6_CD2.root";
  string Li7beam = "../sort_Li_CD2.root";

  TFile *input1 = new TFile(He6beam.c_str(), "read");
  TFile *input2 = new TFile(Li7beam.c_str(), "read");

  string plotpath = "";
  plotpath = "InvMass/6Li/Ex_6Li_da";
  //from cut if(fabs(Correl.cos_thetaH) < .3)
  TH1I *Ex_da = (TH1I*)input1->Get(plotpath.c_str());
  
  gPad->SetTickx();
  gPad->SetTicky();

  Ex_da->GetXaxis()->SetRangeUser(1.5,3);
  Ex_da->GetXaxis()->SetTitle("E_{x} (MeV)");
  Ex_da->GetXaxis()->SetTitleSize(0.07);
  Ex_da->GetXaxis()->SetTitleOffset(1.1);
  Ex_da->GetXaxis()->CenterTitle();
  Ex_da->GetYaxis()->SetTitle("Counts / 10 keV");
  //Ex_da->GetYaxis()->SetTitleOffset(1.2);
  Ex_da->GetYaxis()->CenterTitle();
  Ex_da->Draw();

  TLatex tt;
  tt.SetTextColor(1);
  tt.SetTextAngle(0.);
  tt.SetTextSize(0.1);
  tt.DrawLatex(2.3,3600,"(a) #it{d} + ^{4}He");


//Li7 = t + a
  mycan->cd(2);

  plotpath = "InvMass/7Li/Ex_7Li_ta";
  //from cut if(fabs(Correl.cos_thetaH) < .5)
  TH1I *Ex_ta = (TH1I*)input2->Get(plotpath.c_str());
  //Ex_ta->Rebin(2);

  Ex_ta->GetXaxis()->SetRangeUser(2,6);
  Ex_ta->GetXaxis()->SetTitle("E_{x} (MeV)");
  Ex_ta->GetXaxis()->SetTitleSize(0.07);
  Ex_ta->GetXaxis()->SetTitleOffset(1.1);
  Ex_ta->GetXaxis()->CenterTitle();
  Ex_ta->GetYaxis()->SetTitle("Counts / 20 keV");
  //Ex_ta->GetYaxis()->SetTitleOffset(1.2);
  Ex_ta->GetYaxis()->CenterTitle();
  Ex_ta->Draw();

  tt.DrawLatex(2.5,588,"(b) #it{t} + ^{4}He");


//Be8 = a + a
  mycan->cd(3);

  plotpath = "InvMass/8Be/Ex_8Be_aa";
  //from cut if(fabs(Correl.cos_thetaH) < .5)
  TH1I *Ex_aa = (TH1I*)input2->Get(plotpath.c_str());

  Ex_aa->GetXaxis()->SetRangeUser(-1,5);
  Ex_aa->GetXaxis()->SetTitle("E_{x} (MeV)");
  Ex_aa->GetXaxis()->SetTitleSize(0.07);
  Ex_aa->GetXaxis()->SetTitleOffset(1.1);
  Ex_aa->GetXaxis()->CenterTitle();
  Ex_aa->GetYaxis()->SetTitle("Counts / 10 keV");
  //Ex_aa->GetYaxis()->SetTitleOffset(1.2);
  Ex_aa->GetYaxis()->CenterTitle();
  Ex_aa->Draw();

  tt.DrawLatex(1.6,300,"(c) ^{4}He + ^{4}He");

//Be8 = Li7 + p
  mycan->cd(4);

  plotpath = "InvMass/8Be/Ex_8Be_p7Li";
  //from cut if(fabs(Correl.cos_thetaH) < .5)
  TH1I *Ex_Li7p = (TH1I*)input2->Get(plotpath.c_str());

  Ex_Li7p->GetXaxis()->SetRangeUser(17,21);
  Ex_Li7p->GetXaxis()->SetTitle("E_{x} (MeV)");
  Ex_Li7p->GetXaxis()->SetTitleSize(0.07);
  Ex_Li7p->GetXaxis()->SetTitleOffset(1.1);
  Ex_Li7p->GetXaxis()->CenterTitle();
  Ex_Li7p->GetYaxis()->SetTitle("Counts / 20 keV");
  //Ex_Li7p->GetYaxis()->SetTitleOffset(1.2);
  Ex_Li7p->GetYaxis()->CenterTitle();
  Ex_Li7p->Draw();

  tt.DrawLatex(19.1,121,"(d) #it{p} + ^{7}Li");

  string printname = "temp.png";
  mycan->Print(printname.c_str(), "png");
  mycan->Print("CalResonance.eps", "eps");

}

