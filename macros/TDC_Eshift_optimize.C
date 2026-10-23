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

void TDC_Eshift_optimize(const char* filename = "sort_all_tcal.root") {

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

	string gainsFileName = rootDir + "config/pmtgains_matched.txt";
	ifstream ingain(gainsFileName);
	if (!ingain.is_open()) {
		cerr << "Error: Cannot open input file " << gainsFileName << endl;
		file->Close();
		return;
	}

	// Ensure the output directory for PNG fit plots exists
	string outDir = rootDir + "macros/tdc_optimization";
	gSystem->MakeDirectory(outDir.c_str());

	string outFileName = outDir + "/bar_Eshifts.txt";
	ofstream outshifts(outFileName);
	if (!outshifts.is_open()) {
		cerr << "Error: Cannot open output file " << outFileName << endl;
		file->Close();
		return;
	}

	// Dedicated canvas for plotting fit results
	TCanvas *c_fits = new TCanvas("c_fits", "Prompt Gamma Fits", 1800, 700);
	c_fits->Divide(5, 2); // 5 columns, 2 rows = 10 sub-pads
	TCanvas *c_results = new TCanvas("c_results", "Prompt Gamma Fit Results");

	// Main bar loop
	TMarker* mark;
	int bar,pmttop,pmtbot;
	double topgain,botgain;
	while (ingain.good()) {
		ingain >> bar >> pmttop >> topgain >> pmtbot >> botgain;
		cout << "Processing bar " << bar << "..." << endl;

		// Check if current bar is in the badBars vector
		if (find(badBars.begin(), badBars.end(), bar) != badBars.end()) {
			cout << "Skipping known bad bar " << bar << "..." << endl;
			continue;
		}

		// Create a TGraph to store sigma vs. energy shift for the current bar
		TGraphErrors* g_sigma_vs_shift = new TGraphErrors();
		int point_idx = 0;

		// Loop over energy shifts from 0 to 180 in increments of 20 (exactly 10 points)
		for (double shift = 0; shift <= 180; shift += 20) {
			cout << "Trying shift = " << shift << endl;
				
			// Select the sub-pad (1 through 10)
			c_fits->cd(point_idx + 1);

			// 1. Create an explicit histogram with 100 bins (-170 to -70 ns)
			TString hname = TString::Format("h_time_bar%d_shift_%d", bar, (int)shift);
			TH1F *h_time = new TH1F(
				hname,
				TString::Format("Shift = %d; Time (ns); Counts", (int)shift), 
				100, -170, -70
			);

			// 2. Build the TTree::Draw expression using your weighted average formula
			TString expr = TString::Format(
				"((texneut.TDCvalue_top * (texneut.E_top - (%f * %f))) +"
				" (texneut.TDCvalue_bot * (texneut.E_bot - (%f * %f)))"
				") / (texneut.E_tot - (%f * (%f + %f))) >> %s",
				shift, topgain, shift, botgain, shift, topgain, botgain, hname.Data()
			);

			TString cut = TString::Format("texneut.bar == %d", bar);

			// Execute TTree::Draw ("GOFF" so we can draw it manually onto the specific pad)
			tree->Draw(expr, cut, "GOFF");

			// 3. Find the prompt gamma peak reliably by scanning from the left (-170 ns)
			double peak_mean = -140.0; // Default fallback
			int nbins = h_time->GetNbinsX();
			
			for (int i = 3; i <= nbins - 2; ++i) {
				double current_val = h_time->GetBinContent(i);
				if (current_val > 5.0 && 
					current_val >= h_time->GetBinContent(i-1) && 
					current_val >= h_time->GetBinContent(i-2) && 
					current_val >= h_time->GetBinContent(i+1)) {
					peak_mean = h_time->GetBinCenter(i);
					break; // First local peak from the left
				}
			}

			// 4. Set local fit limits around the prompt gamma peak
			double fit_min = peak_mean - 12.0; 
			double fit_max = peak_mean + 3.0;  

			// 5. Fit the prompt gamma peak with a Gaussian
			TF1 *f_gaus = new TF1("f_gaus", "gaus", fit_min, fit_max);
			f_gaus->SetParameters(h_time->GetBinContent(h_time->FindBin(peak_mean)), peak_mean, 1.0);

			// Fit options: "R" (range), "Q" (quiet), "W" (empty bins weight = 1)
			h_time->Fit(f_gaus, "R Q W");

			// Draw histogram and fit to the active pad
			h_time->Draw("E");

			// 6. Extract the sigma of the Gaussian fit and its uncertainty
			double sigma = f_gaus->GetParameter(2);
			double sigma_err = f_gaus->GetParError(2);

			// Store point in TGraphErrors (X error = 0, Y error = sigma fit error)
			g_sigma_vs_shift->SetPoint(point_idx, shift, sigma);
			g_sigma_vs_shift->SetPointError(point_idx, 0.0, sigma_err);
			
			point_idx++;
		}

		// Update pad rendering for visual inspection
		c_fits->Update();

		// 7. Fit the Sigma vs. Shift graph with a 2nd order polynomial ("pol2")
		TString fit_name = TString::Format("f_pol2_bar%d", bar);
		TF1 *f_pol2 = new TF1(fit_name, "pol2", 0, 180);
		g_sigma_vs_shift->Fit(f_pol2, "R Q");

		// Draw result
		c_results->cd();
		g_sigma_vs_shift->SetTitle(TString::Format("Bar %d Optimization; Energy Shift; Sigma (ns)", bar));
		g_sigma_vs_shift->SetMarkerStyle(20);        // Full circle marker
		g_sigma_vs_shift->SetMarkerColor(kBlue);     // Blue data points
		g_sigma_vs_shift->SetLineColor(kBlack);      // Black error bars

		// Draw graph with points only ("P"), axes ("A"), and no connecting lines
		g_sigma_vs_shift->Draw("AP");

		// Style and draw the fit function in red
		f_pol2->SetLineColor(kRed);
		f_pol2->SetLineWidth(2);
		f_pol2->Draw("SAME");

		c_results->Update();

		// 8. Calculate the optimal shift from the minimum of the polynomial (x = -p1 / (2 * p2))
		double p1 = f_pol2->GetParameter(1);
		double p2 = f_pol2->GetParameter(2);

		double optimal_shift = 0.0;
		if (p2 != 0) {
				optimal_shift = -p1 / (2.0 * p2);
		}

		Printf("Bar %d -> Optimal Energy Shift: %.2f", bar, optimal_shift);
		outshifts << bar << "\t" << optimal_shift << endl;

		// (Optional) Save output figures:
		c_fits->SaveAs(TString::Format("%s/fits_bar_%d.png", outDir.c_str(), bar));
		c_results->SaveAs(TString::Format("%s/results_bar_%d.png", outDir.c_str(), bar));

		// Put this in if you wish to pause in between each iteration
		//mark = (TMarker*)c_results->WaitPrimitive("TMarker");
		//delete mark;

		gSystem->ProcessEvents();
		for (size_t i = 1; i <= 10; i++) {
			TVirtualPad *pad = c_fits->GetPad(i);
			if (pad) pad->Clear();
		}
		c_fits->Modified();
		c_fits->Update();
		c_results->Clear();

		delete f_pol2;
		delete g_sigma_vs_shift;
	}

	delete c_fits;
	delete c_results;
	outshifts.close();
	ingain.close();
	file->Close();
}
