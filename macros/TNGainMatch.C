#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1I.h>
#include <TMarker.h>
#include <TTree.h>

using namespace std;

/**
 * NOTE THAT THIS MACRO SHOULD BE RUN FROM INSIDE THE MACRO
 * DIRECTORY, SINCE ALL FILE PATHS BELOW ARE RELATIVE.
 */

void TNGainMatch() {

	// Macro parameters
	string ifname = "texneut_only_sort/RootFiles/sort_TexNeut_137Cs_cal.root";
	string tname = "tpar";
	size_t nbars = 96; // 16 bars wide, 6 layers deep for 96 total bars
	string histArg = "texneut.c_th>>hist";

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

	// Output file for storing gain matching values as function of bar #.
	// I'll output always the X scaling value. If this value is < 1, then
	// one should invert and apply to Y when reading the file in later.
	ofstream ofile("../Cal/TNGainScalings.dat");

	
	// Some things from the CsICal.C CsI calibration macro I'm basing this on
	// that I copied, not sure what they do
	gROOT->Reset();
	gStyle->SetPalette(1);
	gStyle->SetOptStat(1);

	// Find canvas or make new one
	gROOT->cd();
	TCanvas *mycan = (TCanvas*)gROOT->FindObjectAny("mycan");
	if (!mycan) mycan = new TCanvas("mycan", "mycan");

	// Make histogram for polar angle finger plot
	TH1I hist("hist", "hist", 200, 0, M_PI/2.);

	// Loop through all TexNeut bars and carry out gain matching procedure
	string histGate;
	TMarker* mark;
	double peak1, peak2;
	double peak1_lo, peak1_hi, peak2_lo, peak2_hi;
	double a;
	for (size_t i = 0; i < nbars; i++) {
		histGate = "texneut.bar==" + to_string(i) + " && texneut.c_r>300";
		tpar->Draw(histArg.c_str(), histGate.c_str());
		mycan->Modified();
		mycan->Update();
		
		// Use TMarker object to get approximate locations of peaks and fit bounds
		mark = (TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 1st peak initial guess
		peak1 = mark->GetX();
		delete mark;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 2nd peak initial guess
		peak1_lo = mark->GetX();
		delete mark;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 1st peak low bound
		peak1_hi = mark->GetX();
		delete mark;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 1st peak hi bound
		peak2 = mark->GetX();
		delete mark;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 2nd peak low bound
		peak2_lo = mark->GetX();
		delete mark;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker"); // Get the 2nd peak hi bound
		peak2_hi = mark->GetX();
		delete mark;

		cout << "peak1: " << peak1 << ", range: [" << peak1_lo << ", " << peak1_hi << "]" << endl;
		cout << "peak2: " << peak2 << ", range: [" << peak2_lo << ", " << peak2_hi << "]" << endl;

		//// Fit peaks

		// 1st peak
		double out1[3] = {0.}; 
		TF1* fit1 = new TF1("fit1", "gaus(0)", peak1_lo, peak1_hi);
		fit1->SetParameters(0, .8*hist.GetBinContent(hist.GetBin(peak1)));
		fit1->SetParameters(1, peak1);
		fit1->SetParameters(2, .1);
		fit1->SetParLimits(1, peak1_lo, peak1_hi);
		fit1->SetParLimits(2, 0, .5);
		hist.Fit(fit1, "R+");
		fit1->GetParameters(out1);

		// 2nd peak
		double out2[3] = {0.}; 
		TF1* fit2 = new TF1("fit2", "gaus(0)", peak2_lo, peak2_hi);
		fit2->SetParameters(0, .8*hist.GetBinContent(hist.GetBin(peak2)));
		fit2->SetParameters(1, peak2);
		fit2->SetParameters(2, .1);
		fit2->SetParLimits(1, peak2_lo, peak2_hi);
		fit2->SetParLimits(2, 0, .5);
		hist.Fit(fit2, "R+");
		fit2->GetParameters(out2);

		// Calculate scaling value for gain matching and output
		a = sqrt(tan(out1[1])*tan(out2[1]));
		ofile << i << " " << a << endl;
		cout << "bar #: " << i << ", a: " << a << endl;

		// Wait point at end of loop
		// This makes it so that you have to click to go on to the next bar
		mark = (TMarker*)mycan->WaitPrimitive("TMarker");	
		delete mark;    
	}

	ofile.close();
	ifile->Close();
}
