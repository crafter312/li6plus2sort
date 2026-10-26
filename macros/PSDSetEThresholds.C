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

/**
 * This macro is used to set total energy thresholds for TexNeut,
 * prior to calibrating but after gain matching. This is to remove
 * background during the crystal gating process and to improve
 * time resolution (hopefully). To use, for each bar, click inside
 * the histogram to set the threshold, and click outside the histogram
 * to accept the threshold and move on to the next bar.
 */

using namespace std;

void PSDSetEThresholds(const char* filename = "sort_TexNeut_137Cs_cal_UPDATED.root") {

	// Manually specify problematic bar numbers to skip
	vector<int> badBars = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 51, 78, 80, 86, 87, 95}; // Add your known bad bar indices here

	string rootDir = "/home/Li6Webb/Desktop/Li6Plus2IAS/li6plus2sort/";
	string fname = rootDir + "macros/texneut_only_sort/RootFiles/" + filename;
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
	string outDir = rootDir + "macros/psd_e_thresholds";
	gSystem->MakeDirectory(outDir.c_str());

	string outFileName = outDir + "/bar_e_thresholds.txt";
	ofstream outthresh(outFileName);
	if (!outthresh.is_open()) {
		cerr << "Error: Cannot open output file " << outFileName << endl;
		file->Close();
		return;
	}

	// Dedicated canvas for plotting results
	TCanvas *c_thresholds = new TCanvas("c_thresholds", "Straightened Finger Plots");

	// Main bar loop
	TMarker* mark;
	for (size_t bar = 0; bar < 96; bar++) {
		cout << "Processing bar " << bar << "..." << endl;

		// Check if current bar is in the badBars vector
		if (find(badBars.begin(), badBars.end(), bar) != badBars.end()) {
			cout << "Skipping known bad bar " << bar << endl;
			continue;
		}

		// Draw the 2D histogram for the current bar
		TString hname = Form("h2_bar_%zu", bar);
		TString cut = Form("texneut.bar==%zu && !texneut.isSaturated && !texneut.outsideMinBoundary", bar);
		TString drawCmd = Form("texneut.Lnorm:texneut.E_tot>>%s(200,0,4000,200,-1.2,1.2)", hname.Data());

		tree->Draw(drawCmd, cut, "colz");
		TH2D *h2 = (TH2D*)gDirectory->Get(hname);

		if (!h2 || h2->GetEntries() == 0) {
			cout << "Bar " << bar << " has no entries, skipping" << endl;
			continue;
		}

		c_thresholds->Update();

		// Get histogram axis limits
		double xmin = h2->GetXaxis()->GetXmin();
		double xmax = h2->GetXaxis()->GetXmax();
		double ymin = h2->GetYaxis()->GetXmin();
		double ymax = h2->GetYaxis()->GetXmax();

		// Initial threshold guess (set to 10% across the energy range)
		double threshold = xmin + 0.1 * (xmax - xmin);

		// Draw the initial vertical threshold line
		TLine *line = new TLine(threshold, ymin, threshold, ymax);
		line->SetLineColor(kRed);
		line->SetLineWidth(2);
		line->Draw();
		c_thresholds->Modified();
		c_thresholds->Update();

		while (true) {
			// Wait for a mouse click on the canvas
			mark = (TMarker*)c_thresholds->WaitPrimitive("TMarker");
			if (!mark) continue;
			double x = mark->GetX();
			double y = mark->GetY();
			delete mark;

			// Check if the click is within the histogram X-axis bounds
			if (x >= xmin && x <= xmax && y >= ymin && y <= ymax) {
				threshold = x;
				line->SetX1(threshold);
				line->SetX2(threshold);
				c_thresholds->Modified();
				c_thresholds->Update();
			} else {
				// Clicked outside bounds -> finalize and break out of loop iteration
				cout << "Accepted threshold " << threshold << " for Bar " << bar << "\n\n";
				break;
			}
		}

		// Output bar number and threshold to file, tab-delimited
		outthresh << bar << "\t" << threshold << "\n";

		// Clean up the line object for this iteration
		delete line;

		// Update pad rendering for visual inspection
		c_thresholds->Update();

		// (Optional) Save output figures:
		c_thresholds->SaveAs(TString::Format("%s/Ethresh_bar_%zu.png", outDir.c_str(), bar));

		gSystem->ProcessEvents();
		c_thresholds->Clear();
		c_thresholds->Modified();
		c_thresholds->Update();
	}

	delete c_thresholds;
	outthresh.close();
	file->Close();
}
