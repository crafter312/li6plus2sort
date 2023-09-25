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


  TCanvas canvas("sub","",1500,1000);
  canvas.Divide(2,2);

  canvas.cd(1);
  TFile file_DEMHoff("../sort12C_wTargetEx_forNic.root");
  TH1I* hist_ta = (TH1I*) file_DEMHoff.Get("corr/Li7/Ex7Li_ta");

  //TH2S frame1("frame1","",10,2.5,7,10,0,90000);

  hist_ta->GetXaxis()->SetTitle("E* (MeV)");
  hist_ta->GetYaxis()->SetTitle("Counts");
  hist_ta->GetYaxis()->SetTitleOffset(1.4);
  hist_ta->GetXaxis()->CenterTitle();
  hist_ta->GetYaxis()->CenterTitle();
  hist_ta->SetStats(kFALSE);
  hist_ta->GetYaxis()->SetMaxDigits(3);
  //hist_ta->Rebin(3);
  hist_ta->GetXaxis()->SetRangeUser(2.5,7);
  hist_ta->Draw("same");



  TLatex tt;
  tt.SetNDC(); //normalized device coordinates, coordinates as fractions of window dimensions
  tt.SetTextSize(0.1);
  tt.DrawLatex(.6,.8,"(a) #it{t}+#alpha");


  canvas.cd(2);
  TFile file_cd2("../sort_He6_CD2.root");
  TFile file_c("../sort_He6_C.root");
  TFile file_Li_cd2("../sort_Li_CD2.root");
  TFile file_Li_c("../sort_Li_C.root");

  //TH1I* hist_ta_cd2 = (TH1I*) file_cd2.Get("InvMass/7Li/Ex_7Li_ta");
  //TH1I* hist_ta_c = (TH1I*) file_c.Get("InvMass/7Li/Ex_7Li_ta");
  TH1I* hist_ta_cd2 = (TH1I*) file_Li_cd2.Get("InvMass/7Li/Ex_7Li_ta");
  TH1I* hist_ta_c = (TH1I*) file_Li_c.Get("InvMass/7Li/Ex_7Li_ta");

  //TH2S frame2("frame2","",10,2.5,7,10,0,250);
  //TH2S frame2("frame2","",10,2.5,7,10,0,550);
  hist_ta_cd2->GetXaxis()->SetTitle("E* (MeV)");
  hist_ta_cd2->GetYaxis()->SetTitle("Counts");
  hist_ta_cd2->GetYaxis()->SetTitleOffset(1.4);
  hist_ta_cd2->GetXaxis()->CenterTitle();
  hist_ta_cd2->GetYaxis()->CenterTitle();
  hist_ta_cd2->SetStats(kFALSE);
  //hist_ta_cd2->Draw("same");


  hist_ta_c->Scale(int_cd2/(int_c*ratio));
 // hist_ta_cd2->Add(hist_ta_c,-1);
  //hist_ta_cd2->Rebin(3);

  hist_ta_cd2->GetXaxis()->SetRangeUser(2.5,7);
  //hist_ta_cd2->GetYaxis()->SetRangeUser(0,550);
  hist_ta_cd2->Draw("same hist");

 
  tt.DrawLatex(.6,.8,"(b) t+#alpha");




  canvas.cd(3);

  TFile file_RJCharity("Be12out.root");
  TH1I* hist_knockout = (TH1I*) file_RJCharity.Get("Ex");

  //TH2S frame3("frame3","",10,10,15,10,0,120);
  hist_knockout->GetXaxis()->SetTitle("E* (MeV)");
  hist_knockout->GetYaxis()->SetTitle("Counts");
  hist_knockout->GetYaxis()->SetTitleOffset(1.3);
  hist_knockout->GetXaxis()->CenterTitle();
  hist_knockout->GetYaxis()->CenterTitle();
  hist_knockout->SetStats(kFALSE);
  //hist_knockout->Draw("same");

  hist_knockout->Rebin(2);

  hist_knockout->GetXaxis()->SetRangeUser(10,15);
  //hist_knockout->GetYaxis()->SetRangeUser(0,350);
  hist_knockout->Draw("same");

  tt.DrawLatex(.6,.8,"(c) #it{p}+^{6}He");



  canvas.cd(4);
  TH1I* hist_p6He_cd2 = (TH1I*) file_cd2.Get("InvMass/7Li/Ex_7Li_p6He_timegate");
  TH1I* hist_p6He_c = (TH1I*) file_c.Get("InvMass/7Li/Ex_7Li_p6He_timegate");

  //TH2S frame4("frame4","",10,10,15,10,0,120);
  hist_p6He_cd2->GetXaxis()->SetTitle("E* (MeV)");
  hist_p6He_cd2->GetYaxis()->SetTitle("Counts");
  hist_p6He_cd2->GetYaxis()->SetTitleOffset(1.3);
  hist_p6He_cd2->GetXaxis()->CenterTitle();
  hist_p6He_cd2->GetYaxis()->CenterTitle();
  hist_p6He_cd2->SetStats(kFALSE);
  //hist_p6He_cd2->Draw("same");



  hist_p6He_c->Scale(int_cd2/(int_c*ratio));
  //hist_p6He_cd2->Add(hist_p6He_c, -1);
  hist_p6He_cd2->Rebin(2);
  hist_p6He_cd2->GetXaxis()->SetRangeUser(10,15);
  //hist_p6He_cd2->GetYaxis()->SetRangeUser(0,350);
  hist_p6He_cd2->Draw("same hist");

  tt.DrawLatex(.6,.8,"(d) #it{p}+^{6}He");


  canvas.Print("temp.png", "png");
}
