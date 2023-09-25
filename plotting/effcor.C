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


  double int_c = 2.736;
  double int_cd2 = 16.994;
  double ratio = 1.52;
  double ff=1;
  ratio=ff*ratio;
  TCanvas canvas("can","",1500,1000);


  TFile file_cd2("../sort_He6_CD2.root");
  TH1I* hist_cd2 = (TH1I*) file_cd2.Get("InvMass/7Li/Ex_7Li_p6He_timegate");

  TFile file_c("../sort_He6_C.root");
  TH1I* hist_c = (TH1I*) file_c.Get("InvMass/7Li/Ex_7Li_p6He_timegate");

  hist_c->Scale(int_cd2/(int_c*ratio));
  hist_cd2->Add(hist_c, -1);


  hist_cd2->GetXaxis()->SetTitle("E_{T} (MeV)");
  hist_cd2->GetYaxis()->SetTitle("Counts");
  hist_cd2->GetYaxis()->SetTitleOffset(1.3);
  hist_cd2->GetXaxis()->CenterTitle();
  hist_cd2->GetYaxis()->CenterTitle();

  hist_cd2->SetStats(kFALSE);
  


  //

  TFile file_bkg("sim_bkg.root");
  TH1I* hist_eff = (TH1I*) file_bkg.Get("Ex");


  double max = 0;
  double eff[400];
  //find max value in response to uniform energy
  for (int i=0;i<400;i++)
  {
    if (hist_eff->GetBinContent(i) > max) max = hist_eff->GetBinContent(i);
  }
  //normalize efficiency to max value
  for (int i=0;i<400;i++)
  {
    if (hist_eff->GetBinContent(i) > 0)
    {
      eff[i] = max/hist_eff->GetBinContent(i);
    }
    else
    {
      eff[i] = 0;
    }
  }
  //set bin content to account for efficiency
  for (int i=0;i<400;i++)
  {
    hist_cd2->SetBinContent(i, hist_cd2->GetBinContent(i)*eff[i]); 
  }

  hist_cd2->Rebin(2);
  hist_cd2->GetXaxis()->SetRangeUser(10,15);
  hist_cd2->GetYaxis()->SetRangeUser(0,900);


  TFile file_out("out_sub.root", "RECREATE");
  hist_cd2->Write();


  hist_cd2->Draw();

  canvas.Print("temp.png", "png");

}
