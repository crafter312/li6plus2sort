//written by nicolas dronchi to get a plot for the invariant mass
void compare()
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


  TCanvas *c1 = new TCanvas("c1", "compare");

  gStyle->SetOptStat(00);

  string iso = "Ex_7Li_ta";
  string histname = "InvMass/7Li/"+iso;

  TFile *input = new TFile("../sort_Li_CD2.root", "read");
  TH1I *hist = (TH1I*)input->Get(histname.c_str());


  TFile *input2 = new TFile("../sort_Li_C.root", "read");
  TH1I *hist2 = (TH1I*)input2->Get(histname.c_str());
  hist2->SetLineColor(2);
  hist2->GetXaxis()->SetTitle("E* (MeV)");
  //hist2->GetYaxis()->SetTitle("count difference");

  //hist2->Add(hist, 1);

  hist->Rebin(2);
  hist2->Rebin(2);


  double zerodegC = 1.198412447;
  double zerodegCD2 = 2.050215789;
  double ratio = 1.52;

  hist->Scale(zerodegCD2/(zerodegC*ratio));
  hist->Add(hist2,-1);


  //hist2->SetLineColor(2);
  hist->GetYaxis()->SetRangeUser(0,700);
  hist->GetXaxis()->SetRangeUser(2,8);

  //

  hist->Draw("hist");
  //hist2->Draw("same");

  //TLegend *legend = new TLegend(0.65,0.78,0.915,0.93);
  //legend->AddEntry(hist, "CD_{2} target");
  //legend->AddEntry(hist2, "Carbon target");
  //legend->Draw();



  string printname = "ta_Libeam_Carbonbkg_subtract.png";
  c1->Print(printname.c_str(), "png");

}
