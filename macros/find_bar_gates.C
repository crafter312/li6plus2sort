#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

void find_bar_gates(const char* filename = "sort_all_TNredrawn.root") {
    // 1. Manually specify problematic bar numbers to skip
    std::vector<int> badBars = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 51, 78, 80, 86, 87, 95}; // Add your known bad bar indices here

    // Ensure the output directory for PNG fit plots exists
    gSystem->MakeDirectory("finger_plots");

		std::string dataDir = "/home/Li6Webb/Desktop/Li6Plus2IAS/li6plus2sort/RootFiles/";
    TFile *file = TFile::Open((dataDir + filename).c_str(), "READ");
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

    std::ofstream outFile("bar_gates.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open output file bar_gates.txt" << std::endl;
        file->Close();
        return;
    }

    // 6-Gaussian fit function formula
    TString fitFormula = "gaus(0) + gaus(3) + gaus(6) + gaus(9) + gaus(12) + gaus(15)";
    TF1 *fitFunc = new TF1("fitFunc", fitFormula.Data(), -1.2, 1.2);

    // Dedicated canvas for plotting fit results
    TCanvas *c1 = new TCanvas("c1", "Bar Fit Verification", 800, 600);

    for (int bar = 0; bar < 96; ++bar) {
        // Check if current bar is in the badBars vector
        if (std::find(badBars.begin(), badBars.end(), bar) != badBars.end()) {
            std::cout << "Skipping known bad bar: " << bar << std::endl;
            outFile << bar << " 0 0 0 0 0\n";
            continue;
        }

        TString histName = Form("h_bar_%d", bar);
        TH1D *h = new TH1D(histName.Data(), Form("Bar %d Lnorm Projection;Lnorm;Counts", bar), 200, -1.2, 1.2);

        // Draw 1D projection for current bar
        TString drawExpr = Form("texneut.Lnorm>>%s", histName.Data());
        TString cutExpr  = Form("texneut.bar==%d && !texneut.isSaturated", bar);
        tpar->Draw(drawExpr.Data(), cutExpr.Data(), "goff");

        // Use TSpectrum to identify the 6 peak positions along Y-axis projection
        TSpectrum *spec = new TSpectrum(6);
        int nPeaks = spec->Search(h, 0.8, "goff", 0.05);

        double *xpeaks = spec->GetPositionX();
        std::vector<double> peakVec(xpeaks, xpeaks + nPeaks);
        std::sort(peakVec.begin(), peakVec.end());

        // Fallback to evenly spaced initial guesses if TSpectrum finds fewer than 6 peaks
        if (peakVec.size() < 6) {
            peakVec.clear();
            for (int i = 0; i < 6; ++i) {
                peakVec.push_back(-0.85 + i * 0.34);
            }
        }

        // Set initial parameters for each Gaussian component
        for (int i = 0; i < 6; ++i) {
            double meanGuess = peakVec[i];
            int bin = h->GetXaxis()->FindBin(meanGuess);
            double ampGuess = h->GetBinContent(bin);

            fitFunc->SetParameter(i * 3 + 0, ampGuess);
            fitFunc->SetParameter(i * 3 + 1, meanGuess);
            fitFunc->SetParLimits(i * 3 + 1, meanGuess - 0.1, meanGuess + 0.1);
            fitFunc->SetParameter(i * 3 + 2, 0.05);
            fitFunc->SetParLimits(i * 3 + 2, 0.01, 0.15);
        }

        // Perform fit; using "R" option to apply range and draw fit on histogram
        h->Fit(fitFunc, "QNR");

        // Save canvas image to finger_plots directory
        c1->cd();
        h->Draw();
        fitFunc->Draw("SAME");
        c1->SaveAs(Form("finger_plots/bar_%d_fit.png", bar));

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
        outFile << bar;
        for (int i = 0; i < 5; ++i) {
            double mu1 = gaussParams[i].first;
            double sig1 = std::abs(gaussParams[i].second);
            double mu2 = gaussParams[i+1].first;
            double sig2 = std::abs(gaussParams[i+1].second);

            double sep = (mu1 * sig2 + mu2 * sig1) / (sig1 + sig2);
            outFile << " " << sep;
        }
        outFile << "\n";

        delete spec;
        delete h;
    }

    delete c1;
    delete fitFunc;
    outFile.close();
    file->Close();
    std::cout << "Calculated gates and generated fit plots in ./finger_plots/" << std::endl;
}
