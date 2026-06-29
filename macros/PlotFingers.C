#include <iostream>
#include <string>

#include <TCanvas.h>
#include <TFile.h>
#include <TH2I.h>
#include <TMarker.h>
#include <TTree.h>

using namespace std;

void PlotFingers() {

	gROOT->Reset();

	// Set default style attributes
	TStyle * Sty = new TStyle("MyStyle","MyStyle");
	Sty->SetOptTitle(0);
	Sty->SetOptStat(0);
	Sty->SetLineWidth(3);
	Sty->SetPalette(kBird);
	Sty->SetCanvasColor(10);
	Sty->SetCanvasBorderMode(0);
	Sty->SetFrameLineWidth(0);
	Sty->SetFrameFillColor(10);
	Sty->SetPadColor(10);
	Sty->SetPadTickX(1);
	Sty->SetPadTickY(1);
	Sty->SetHistLineWidth(3);
	Sty->SetFuncWidth(3);
	Sty->SetFuncColor(kGreen);
	Sty->SetLineWidth(3);
	Sty->SetLabelSize(0.04,"xyz");
	Sty->SetLabelOffset(0.01,"y");
	Sty->SetLabelOffset(0.01,"x");
	Sty->SetLabelColor(kBlack,"xyz");
	Sty->SetTitleSize(0.05,"y");
	Sty->SetTitleSize(0.05,"x");
	Sty->SetTitleOffset(1.4,"y");
	Sty->SetTitleOffset(0.9,"x");
	Sty->SetTitleFillColor(10);
	Sty->SetTitleTextColor(kBlack);
	Sty->SetTickLength(.05,"xz");
	Sty->SetTickLength(.025,"y");
	Sty->SetNdivisions(10,"y");
	Sty->SetNdivisions(10,"x");
	Sty->SetEndErrorSize(0);
	Sty->SetTextFont(42);
	gROOT->SetStyle("MyStyle");
	gROOT->ForceStyle();

	size_t bar = 1;
	string ifname = "../RootFiles/sort_all.root";
	string tname = "tpar";

	// Open TTree with TexNeut results
	TFile *ifile = TFile::Open(ifname.c_str());
	if (!ifile || ifile->IsZombie()) {
		cout << "Input file " << ifname << " does not exist or cannot be opened" << endl;
		return;
	}
	TTree *tpar = (TTree*)ifile->Get(tname.c_str());
	if (!tpar) {
		cout << "TTree " << tname << " not found in file or could not be opened" << endl;
		ifile->Close();
		return;
	}

	// Calculate fractional margins (default margin is 0.1 fraction of width/height)
	int canw = 900;
	int canh = 800;
	int pixl = 50; // extra left margen in pixels
	int pixr = 50; // extra right margin in pixels
	if ((canw - pixl - pixr) != canh) {
		cout << "ERROR: canvas width minus extra margins must be equal to height" << endl;
		ifile->Close();
		return;
	}

	int histw = (int)(canw * 0.8) - pixr - pixl;
	double ymarg = max(canh - histw, 0) / (2.0 * canh);
	double xmargl = 0.1 + ((double)pixl / (double)canw);
	double xmargr = 0.1 + ((double)pixr / (double)canw);

	TCanvas* mycan = new TCanvas("mycan", "", canw, canh);
	mycan->SetMargin(xmargl, xmargr, ymarg, ymarg); // left, right, bottom, top
	mycan->Draw();

	TH2I* fingers = new TH2I("fingers", "", 800, 0, 8192, 800, 0, 8192);
	string gate = "texneut.bar == " + to_string(bar);
	tpar->Draw("texneut.Aint_top:texneut.Aint_bot>>fingers", gate.c_str(), "colz");
	fingers->GetXaxis()->SetTitle("A integral (bottom, unitless)");
	fingers->GetXaxis()->CenterTitle();
	fingers->GetYaxis()->SetTitle("A integral (top, unitless)");
	fingers->GetYaxis()->CenterTitle();

	mycan->Modified();
	mycan->Update();

	// Zoom into histogram by clicking on new upper right corner point
	cout << "Select point as max to zoom in..." << endl;
	TMarker* mark = (TMarker*)mycan->WaitPrimitive("TMarker");
	double K = mark->GetX() + mark->GetY();
	double xmax = K * .5;
	double ymax = K * .5;
	fingers->GetXaxis()->SetRangeUser(0, xmax);
	fingers->GetYaxis()->SetRangeUser(0, ymax);
	cout << "Point (" << xmax << ", " << ymax << ") selected, zooming in!" << endl;
	delete mark;

	// Draw diagonal lines
	TLine *myline = new TLine();
	myline->SetLineWidth(1);
	myline->SetLineStyle(2);
	myline->DrawLine(0., 0., xmax, ymax);
	myline->DrawLine(0., ymax, xmax, 0.);

	mycan->Modified();
	mycan->Update();

	// Keep histogram open until user clicks inside window
	mark = (TMarker*)mycan->WaitPrimitive("TMarker");
	delete mark;

	ifile->Close();
}
