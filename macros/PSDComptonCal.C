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

void PSDComptonCal(const char* filename = "sort_TexNeut_137Cs_cal_UPDATED.root") {

	// 1. Manually specify problematic bar numbers to skip
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
	string outDir = rootDir + "macros/psd_cal";
	gSystem->MakeDirectory(outDir.c_str());

	string outFileName = outDir + "/bar_psd_cal.txt";
	ofstream outcal(outFileName);
	if (!outcal.is_open()) {
		cerr << "Error: Cannot open output file " << outFileName << endl;
		file->Close();
		return;
	}

	// Dedicated canvas for plotting fit results
	TCanvas *c_fits = new TCanvas("c_fits", "Prompt Gamma Fits", 1800, 700);
	c_fits->Divide(3, 2); // 3 columns, 2 rows = 6 sub-pads

	// Main bar loop
	TMarker* mark;
	for (size_t bar = 0; bar < 96; bar++) {
		cout << "Processing bar " << bar << "..." << endl;

		// Check if current bar is in the badBars vector
		if (find(badBars.begin(), badBars.end(), bar) != badBars.end()) {
			cout << "Skipping known bad bar " << bar << endl;
			continue;
		}

		// Loop 6 crystals in each bar
		for (size_t crystal = 0; crystal < 6; crystal++) {
			cout << "Crystal " << crystal << endl;
			c_fits->cd(crystal + 1);

			// Create and fill histogram
			TString hname  = TString::Format("h_time_bar%zu_crystal%zu", bar, crystal);
			TString htitle = TString::Format("Crystal = %zu; E_{tot} (arb. units); Counts per bin", crystal);
			TH1I *h = new TH1I(hname, htitle, 600, 0, 7500);

			TString expr = TString::Format("texneut.E_tot>>%s", hname.Data());
			TString cut  = TString::Format("texneut.bar==%zu && texneut.yi==%zu", bar, crystal);
			tree->Draw(expr, cut, "GOFF"); // "GOFF" so we can draw it manually onto the specific pad

			// Center on mean
			double mean = h->GetMean();
			h->GetXaxis()->SetRangeUser(0, mean*2.);

			/******** FITTING ROUTINE ********/

			int nbins = h->GetXaxis()->GetLast() - h->GetXaxis()->GetFirst();
			double xmin = h->GetXaxis()->GetBinLowEdge(h->GetXaxis()->GetFirst());
			double xmax = h->GetXaxis()->GetBinUpEdge(h->GetXaxis()->GetLast());

			// 1. Create a temporary clone and smooth it to handle noise/fluctuations
			TH1* h_smooth = (TH1*)h->Clone("h_smooth_temp");
			// Smooth multiple times (or use a wider window) to wash out statistical noise
			for (int s = 0; s < 5; s++)
				h_smooth->Smooth(1); // ROOT's built-in 3-bin running average (1-2-1 weighted)

			// 2. Locate the Compton edge using a wider finite-difference window on the smoothed histogram
			// Skip the first 15% of bins to avoid low-energy peaks or electronic noise.
			int start_bin = nbins * 0.40;
			int end_bin = nbins * 0.95;
			double min_derivative = 0.0;
			int edge_bin = start_bin;
			int window = 3; // Look across a multi-bin window to avoid local bumps
			for (int i = start_bin + window; i < end_bin - window; i++) {

				// Use a wider stencil for the derivative to ignore local jaggedness
				double deriv = h_smooth->GetBinContent(i + window) - h_smooth->GetBinContent(i - window);
				if (deriv < min_derivative) {
					min_derivative = deriv;
					edge_bin = i;
				}
			}

			double x_ce = h_smooth->GetBinCenter(edge_bin);
			delete h_smooth; // Clean up temporary histogram

			// 3. Set fit range on the original histogram:
			// Lower bound: in the middle of the plateau (cutting out the lower-end peak/noise)
			// Upper bound: extends to the edge of the histogram to capture the background
			double fit_min = x_ce - (x_ce - h->GetBinCenter(start_bin)) * 0.6; 
			double fit_max = xmax; 

			// 4. Define the complementary error function (erfc) model with a constant background term:
			// [0] = Amplitude (half-height)
			// [1] = Compton edge position (ch_ce)
			// [2] = Gaussian broadening width (sigma)
			// [3] = Background offset
			TF1* f_erfc = new TF1("f_erfc", "[0] * TMath::Erfc((x - [1]) / (sqrt(2.0) * [2])) + [3]", fit_min, fit_max);

			// 5. Set sensible initial parameter guesses based on original data
			double plateau_height = h->GetBinContent(h->FindBin(x_ce - 25.0));
			double background = h->GetBinContent(nbins); // estimate background at far right
    
			f_erfc->SetParameter(0, plateau_height / 2.0);
			f_erfc->SetParameter(1, x_ce);
			f_erfc->SetParameter(2, 20.0); // Initial guess for detector resolution sigma
			f_erfc->SetParameter(3, background);

			// Set parameter bounds for stability during minimization
			f_erfc->SetParLimits(1, xmin, xmax);
			f_erfc->SetParLimits(2, 1.0, 200.0); // Sigma must be positive

			// 6. Perform the fit on the ORIGINAL histogram ("R" enforces the custom fit range)
			h->Fit(f_erfc, "R+");

			// Output results
			cout << "--- Compton Edge Fit Results ---" << endl;
			cout << "Edge Position: " << f_erfc->GetParameter(1) << " +/- " << f_erfc->GetParError(1) << endl;
			cout << "Resolution (sigma): " << f_erfc->GetParameter(2) << " +/- " << f_erfc->GetParError(2) << endl;
			outcal << bar << "\t" << crystal << "\t" << f_erfc->GetParameter(1) << endl;
			h->Draw();
		}

		// Update pad rendering for visual inspection
		c_fits->Update();

		// (Optional) Save output figures:
		c_fits->SaveAs(TString::Format("%s/Ecal_fits_bar_%zu.png", outDir.c_str(), bar));

		// Put this in if you wish to pause in between each iteration
		mark = (TMarker*)c_fits->WaitPrimitive("TMarker");
		delete mark;

		gSystem->ProcessEvents();
		for (size_t i = 1; i <= 6; i++) {
			TVirtualPad *pad = c_fits->GetPad(i);
			if (pad) pad->Clear();
		}
		c_fits->Modified();
		c_fits->Update();
	}

	delete c_fits;
	outcal.close();
	file->Close();
}
