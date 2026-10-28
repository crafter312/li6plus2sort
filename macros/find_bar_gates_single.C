#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TMarker.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

void find_bar_gates_single(const char* filename = "sort_all_tcal.root") {
    // 1. Manually specify problematic bar numbers to skip
    std::vector<int> badBars = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 51, 78, 80, 86, 87, 95}; // Add your known bad bar indices here

		std::string rootDir = "/home/Li6Webb/Desktop/Li6Plus2IAS/li6plus2sort/";
    TFile *file = TFile::Open((rootDir + "RootFiles/" + filename).c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open ROOT file " << filename << std::endl;
        return;
    }

    TTree *tpar = (TTree*)file->Get("tpar");
    if (!tpar) {
        std::cerr << "Error: Tree 'tpar' not found." << std::endl;
        file->Close();
        return;
    }

		// Ensure the output directory for PNG fit plots exists
		std::string outDir = rootDir + "macros/finger_plots";
    gSystem->MakeDirectory(outDir.c_str());

    // 6-Gaussian fit function formula
    TString fitFormula = "gaus(0) + gaus(3) + gaus(6) + gaus(9) + gaus(12) + gaus(15)";
    TF1 *fitFunc = new TF1("fitFunc", fitFormula.Data(), -1.2, 1.2);

    // Dedicated canvas for plotting fit results
    TCanvas *c1 = new TCanvas("c1", "Bar Fit Verification", 800, 600);

		// Bars to be re-fit: 9, 11, 15, 18, 20, 22, 50, 56, 61, 67, 68, 71, 76, 94
		// Bars that need extra work: 28, 63
    const int bar = 63;

    // Check if current bar is in the badBars vector
    if (std::find(badBars.begin(), badBars.end(), bar) != badBars.end()) {
      std::cout << "Skipping known bad bar: " << bar << std::endl;
			delete c1;
			delete fitFunc;
			file->Close();
      return;
    }

    TString histName = Form("h_bar_%d", bar);
    TH1D *h = new TH1D(histName.Data(), Form("Bar %d Lnorm Projection;Lnorm;Counts", bar), 200, -1.2, 1.2);
		h->SetStats(0);

    // Draw 1D projection for current bar
    TString drawExpr       = Form("texneut.Lnorm>>%s", histName.Data());
    TString cutExpr        = Form("texneut.bar==%d && !texneut.isSaturated && !texneut.outsideMinBoundary && texneut.aboveThreshold", bar);
		if (bar == 28) cutExpr = Form("texneut.bar==%d && !texneut.isSaturated && !texneut.outsideMinBoundary && texneut.E_tot>800", bar);
    tpar->Draw(drawExpr.Data(), cutExpr.Data());

    // The automatic macro uses a TSpectrum to get peak location estimates
		// For this manual macro for cases where the TSpectrum's initial fit
		// values were inadequate, just use TMarker to click on the peaks
    TMarker *mark;
    for (int i = 0; i < 6; ++i) {
			mark = (TMarker*)c1->WaitPrimitive("TMarker");
			double x = mark->GetX();
			delete mark;
			std::cout << "peak " << i << " guess: " << x << std::endl;
      int bin = h->GetXaxis()->FindBin(x);
      double ampGuess = h->GetBinContent(bin) * 0.85;

      fitFunc->SetParameter(i * 3 + 0, ampGuess);
			fitFunc->SetParLimits(i * 3 + 0, ampGuess*0.8, ampGuess*1.2);
      fitFunc->SetParameter(i * 3 + 1, x);
      fitFunc->SetParLimits(i * 3 + 1, x - 0.15, x + 0.15);
      fitFunc->SetParameter(i * 3 + 2, 0.07);
      fitFunc->SetParLimits(i * 3 + 2, 0.01, 0.25);
    }

    // Perform fit; using "R" option to apply range and draw fit on histogram
    h->Fit(fitFunc, "NR");

    // Save canvas image to finger_plots directory
    c1->cd();
    h->Draw();
    fitFunc->Draw("SAME");
		c1->Modified();
		c1->Update();
    c1->SaveAs(Form("%s/bar_%d_fit_manual.png", outDir.c_str(), bar));

    // Extract fit parameters
    std::vector<std::pair<double, double>> gaussParams(6); // {mean, sigma}
    for (int i = 0; i < 6; ++i) {
        gaussParams[i] = {fitFunc->GetParameter(i * 3 + 1), fitFunc->GetParameter(i * 3 + 2)};
    }

    // Ensure ordering by mean position
    std::sort(gaussParams.begin(), gaussParams.end(),
              [](const std::pair<double, double> &a, const std::pair<double, double> &b) {
                  return a.first < b.first;
              });

    // Compute separation boundaries (weighted midpoints)
    std::cout << "Bar: " << bar << std::endl;
		std::cout << "Separations, negative to positive peak:";
    for (int i = 0; i < 5; ++i) {
        double mu1 = gaussParams[i].first;
        double sig1 = std::abs(gaussParams[i].second);
        double mu2 = gaussParams[i+1].first;
        double sig2 = std::abs(gaussParams[i+1].second);

        double sep = (mu1 * sig2 + mu2 * sig1) / (sig1 + sig2);
        std::cout << "\t" << sep;
    }
    std::cout << std::endl;

    delete h;

    delete c1;
    delete fitFunc;
    file->Close();
    std::cout << "Calculated gates and generated fit plots in ./finger_plots/" << std::endl;
}
