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
  TFile file_cd2("../sort_He6_CD2.root");
  TH1I* hist_cd2 = (TH1I*) file_cd2.Get("InvMass/7Li/Ex_7Li_p6He_timegate");
  TFile file_c("../sort_He6_C.root");
  TH1I* hist_c = (TH1I*) file_c.Get("InvMass/7Li/Ex_7Li_p6He_timegate");

  TH2S frame("frame","",10,0,7,10,0,250);
  frame.GetXaxis()->SetTitle("E_{T} (MeV)");
  frame.GetYaxis()->SetTitle("Counts");
  frame.GetYaxis()->SetTitleOffset(1.3);
  frame.GetXaxis()->CenterTitle();
  frame.GetYaxis()->CenterTitle();
  frame.SetStats(kFALSE);
  frame.Draw("same");

  hist_cd2->Draw("same");

  hist_c->Scale(int_cd2/(int_c*ratio));
  //for (int i=0;i<400;i++) hist_c->SetBinError(i,0.);
  hist_c->SetLineColor(2);
  hist_c->Draw("Same Hist");

  TLatex tt;
  tt.SetNDC();
  tt.SetTextSize(0.1);
  tt.DrawLatex(.6,.8,"#it{p}+^{6}He");

  canvas.cd(2);
  TH1I* hist_da_cd2 = (TH1I*) file_cd2.Get("InvMass/6Li/Erel_6Li_da");
  TH1I* hist_da_c = (TH1I*) file_c.Get("InvMass/6Li/Erel_6Li_da");

  TH2S frame2("frame2","",10,0,2.5,10,0,9000);

  frame2.GetXaxis()->SetTitle("E_{T} (MeV)");
  frame2.GetYaxis()->SetTitle("Counts");
  frame2.GetYaxis()->SetTitleOffset(1.3);
  frame2.GetXaxis()->CenterTitle();
  frame2.GetYaxis()->CenterTitle();
  frame2.SetStats(kFALSE);
  frame2.Draw("same");


  hist_da_cd2->Draw("same");
  hist_da_c->Scale(int_cd2/(int_c*ratio));
  for (int i=0;i<400;i++) hist_da_c->SetBinError(i,0.);
  hist_da_c->SetLineColor(2);
  hist_da_c->Draw("same");

  tt.DrawLatex(.6,.8,"#it{d}+#alpha");

  canvas.cd(3);
  TH1I* hist_ta_cd2 = (TH1I*) file_cd2.Get("InvMass/7Li/Erel_7Li_ta");
  TH1I* hist_ta_c = (TH1I*) file_c.Get("InvMass/7Li/Erel_7Li_ta");

  TH2S frame3("frame3","",10,0,5,10,0,450);
  frame3.GetXaxis()->SetTitle("E_{T} (MeV)");
  frame3.GetYaxis()->SetTitle("Counts");
  frame3.GetYaxis()->SetTitleOffset(1.3);
  frame3.GetXaxis()->CenterTitle();
  frame3.GetYaxis()->CenterTitle();
  frame3.SetStats(kFALSE);
  frame3.Draw("same");


  hist_ta_cd2->Draw("same");
  hist_ta_c->Scale(int_cd2/(int_c*ratio));
  for (int i=0;i<400;i++) hist_ta_c->SetBinError(i,0.);
  hist_ta_c->SetLineColor(2);
  hist_ta_c->Draw("same");

  tt.DrawLatex(.6,.8,"#it{t}+#alpha");


  canvas.cd(4);
  TH1I* hist_pa_cd2 = (TH1I*) file_cd2.Get("InvMass/5Li/Erel_5Li_pa");
  TH1I* hist_pa_c = (TH1I*) file_c.Get("InvMass/5Li/Erel_5Li_pa");

  TH2S frame4("frame4","",10,0,5,10,0,320);
  frame4.GetXaxis()->SetTitle("E_{T} (MeV)");
  frame4.GetYaxis()->SetTitle("Counts");
  frame4.GetYaxis()->SetTitleOffset(1.3);
  frame4.GetXaxis()->CenterTitle();
  frame4.GetYaxis()->CenterTitle();
  frame4.SetStats(kFALSE);
  frame4.Draw("same");


  hist_pa_cd2->Draw("same");
  hist_pa_c->Scale(int_cd2/(int_c*ratio));
  for (int i=0;i<400;i++) hist_pa_c->SetBinError(i,0.);
  hist_pa_c->SetLineColor(2);
  hist_pa_c->Draw("same");
  tt.DrawLatex(.6,.8,"#it{p}+#alpha");


  canvas.Print("CarbonBackground.png", "png");

/////////////////////////////////////////////////////////////////////////////////////
  TCanvas canvas2("sub2","",1200,600);
  canvas2.cd();

  TPad* pad1 = new TPad("pad1", "",0.0,0.0,0.7,1.0);
  TPad* pad2 = new TPad("pad2", "",0.7,0.0,1.0,1.0);
  pad1->Draw();
  pad2->Draw();

  TH1I* hist_Ex_cd2 = (TH1I*) file_cd2.Get("InvMass/7Li/Ex_7Li_p6He");
  TH1I* hist_Ex_cd2_trans = (TH1I*) file_cd2.Get("InvMass/7Li/Ex_7Li_p6He_transverse");
  TH1I* hist_Ex_c = (TH1I*) file_c.Get("InvMass/7Li/Ex_7Li_p6He");

  pad2->cd();
  
  hist_Ex_cd2_trans->GetXaxis()->SetRangeUser(10,13);
  hist_Ex_cd2_trans->GetYaxis()->SetRangeUser(0,80);
  hist_Ex_cd2_trans->GetXaxis()->SetTitle("E_{x} (MeV)");
  hist_Ex_cd2_trans->GetXaxis()->SetLabelFont(63);
  hist_Ex_cd2_trans->GetXaxis()->SetLabelSize(20);
  hist_Ex_cd2_trans->GetXaxis()->SetTitleFont(63);
  hist_Ex_cd2_trans->GetXaxis()->SetTitleSize(28);
  hist_Ex_cd2_trans->GetXaxis()->SetTitleOffset(1.1);
  hist_Ex_cd2_trans->GetXaxis()->CenterTitle();

  //hist_Ex_cd2->SetMarkerStyle(20);
  hist_Ex_cd2_trans->Draw();  
  tt.SetTextFont(63);
  tt.SetTextSizePixels(36);
  tt.DrawLatex(0.25,0.87,"(b) Trans Gate");


  pad1->cd();
  hist_Ex_cd2->Rebin(2);
  hist_Ex_cd2->GetXaxis()->SetRangeUser(10,18);
  hist_Ex_cd2->GetYaxis()->SetRangeUser(0,250);
  hist_Ex_cd2->GetXaxis()->SetTitle("E_{x} (MeV)");
  hist_Ex_cd2->GetXaxis()->SetLabelFont(63);
  hist_Ex_cd2->GetXaxis()->SetLabelSize(20);
  hist_Ex_cd2->GetXaxis()->SetTitleFont(63);
  hist_Ex_cd2->GetXaxis()->SetTitleSize(28);
  hist_Ex_cd2->GetXaxis()->SetTitleOffset(1.1);
  hist_Ex_cd2->GetXaxis()->CenterTitle();
  hist_Ex_cd2->GetYaxis()->SetLabelFont(63);
  hist_Ex_cd2->GetYaxis()->SetLabelSize(20);
  hist_Ex_cd2->GetYaxis()->SetTitleFont(63);
  hist_Ex_cd2->GetYaxis()->SetTitleSize(28);
  hist_Ex_cd2->GetYaxis()->SetTitle("Counts");
  //hist_Ex_cd2->GetYaxis()->SetTitleOffset(1.2);
  hist_Ex_cd2->GetYaxis()->CenterTitle();

  //hist_Ex_cd2->SetMarkerStyle(20);
  hist_Ex_cd2->Draw();  


  hist_Ex_c->Scale(int_cd2/(int_c*ratio));
  //for (int i=0;i<400;i++) hist_c->SetBinError(i,0.);
  hist_Ex_c->SetLineColor(2);
  hist_Ex_c->SetMarkerStyle(21);
  hist_Ex_c->SetMarkerColor(2);
  hist_Ex_c->Rebin(4);
  hist_Ex_c->Scale(.5);
  hist_Ex_c->Draw("Same hist");


  //TLegend *legend = new TLegend(0.59,0.8,0.875,0.93);
  TLegend *legend = new TLegend(0.65,0.78,0.915,0.93);
  legend->AddEntry(hist_Ex_cd2, "CD_{2} target");
  legend->AddEntry(hist_Ex_c, "Carbon target");
  legend->Draw();

  tt.SetTextSizePixels(36);
  tt.DrawLatex(0.30,0.87,"(a) ^{6}He + p");

  canvas2.Print("Li7_p6He.png", "png");

}
