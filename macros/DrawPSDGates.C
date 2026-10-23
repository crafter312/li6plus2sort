#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TMarker.h>
#include <TSpectrum.h>
#include <TSystem.h>
#include <TTree.h>

using namespace std;

void DrawPSDGates(const char* filename = "sort_all_tcal.root") {

	// 1. Manually specify problematic bar numbers to skip
	vector<int> badBars = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 51, 78, 80, 86, 87, 95}; // Add your known bad bar indices here

	string rootDir = "/home/Li6Webb/Desktop/Li6Plus2IAS/li6plus2sort/";
	string fname = rootDir + "RootFiles/" + filename;
	TFile *file = TFile::Open(fname.c_str(), "READ");
	if (!file || file->IsZombie()) {
		cerr << "Error: Cannot open ROOT file " << fname << endl;
		return;
	}

	TTree *tree = (TTree*)file->Get("tpar");
	if (!tree) {
		cerr << "Error: Tree 'tpar' not found" << endl;
		file->Close();
		return;
	}

	// Ensure the output directory for PNG fit plots exists
	string outDir = rootDir + "macros/psd_neutron_gates";
	gSystem->MakeDirectory(outDir.c_str());

	string outFileName = outDir + "/neutron_cuts.txt";
	ofstream outcuts(outFileName);
	if (!outcuts.is_open()) {
		cerr << "Error: Cannot open output file " << outFileName << endl;
		file->Close();
		return;
	}

	// Dedicated canvas for plotting fit results
	TCanvas *c_fits = new TCanvas("c_gates", "PSD Neutron Gates");

	// Main bar loop
	TMarker* mark;
	for (size_t bar = 0; bar < 96; bar++) {
		cout << "Processing bar " << bar << "..." << endl;

		// Check if current bar is in the badBars vector
		if (find(badBars.begin(), badBars.end(), bar) != badBars.end()) {
			cout << "Skipping known bad bar " << bar << endl;
			continue;
		}

		// Crystal loop
		for (size_t crystal = 0; crystal < 6; crystal++) {

			// Create histogram and fill
			TString hname  = TString::Format("h_psd_bar%zu_crystal%zu", bar, crystal);
			TString htitle = TString::Format("Bar %zu, Crystal %zu;E_{tot} (KeVee);PSD (tail/peak)", bar, crystal);
			TH2D *h_psd    = new TH2D(hname, htitle, 200, 0, 24000, 200, 0.5, 4);
			h_psd->SetStats(0); // Hides the statistics box
			TString pars   = TString::Format("texneut.PSD:texneut.E_tot>>h_psd_bar%zu_crystal%zu", bar, crystal);
			TString gate   = TString::Format("texneut.bar==%zu && texneut.yi==%zu && !texneut.isSaturated && !texneut.outsideMinBoundary", bar, crystal);
			tree->Draw(pars, gate, "colz");
			c_fits->Update();

			// Prompt the user to draw the graphical cut
			gROOT->SetEditorMode("CutG");
			TCutG *cutg = (TCutG*)c_fits->WaitPrimitive("CUTG", "CutG");

			if (cutg) {
				cutg->SetName(TString::Format("neutron_cut_bar%zu_crystal%zu", bar, crystal));

				int nPoints = cutg->GetN();
				double *x = cutg->GetX();
				double *y = cutg->GetY();

				// Write coordinates to file
				outcuts << bar << "\t" << crystal << endl;
				for (size_t i = 0; i < nPoints; i++)
					outcuts << x[i] << (i == nPoints - 1 ? "\n" : "\t");
				for (size_t i = 0; i < nPoints; i++)
					outcuts << y[i] << (i == nPoints - 1 ? "\n" : "\t");
			}

			// Update pad rendering for visual inspection
			c_fits->Update();

			// (Optional) Save output figures:
			c_fits->SaveAs(TString::Format("%s/neutron_psd_cut_bar%zu_crystal%zu.png", outDir.c_str(), bar, crystal));

			// Put this in if you wish to pause in between each iteration
			mark = (TMarker*)c_fits->WaitPrimitive("TMarker");
			delete mark;

			gSystem->ProcessEvents();
			c_fits->Clear();
			c_fits->Modified();
			c_fits->Update();
		}
	}

	delete c_fits;
	outcuts.close();
	file->Close();
}
