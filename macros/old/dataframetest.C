#include <ROOT/RDataFrame.hxx>
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

#include <cmath>

void refactor() {
	ROOT::EnableImplicitMT(4); // Use multiple threads

	// Open the ROOT file and check for success
	TFile *file = TFile::Open("run-420.root");
	if (!file || file->IsZombie()) {
		std::cerr << "Error opening file!" << std::endl;
		return;
	}

	// Check if tree exists in the file
	TTree *tree = (TTree*)file->Get("t");
	if (!tree) {
		std::cerr << "Tree 't' not found in the file!" << std::endl;
		file->Close();
		return;
	}

	// Create a DataFrame and define the histogram
	ROOT::RDataFrame d(*tree);
	auto myHisto = d.Histo1D({"hist", "hist", 200u, 850., 1050.}, "SpecTcl_hinp1_mb1_e_01.10");
	
	// Create a canvas to draw the histogram
	TCanvas* c = new TCanvas("c", "Canvas", 800, 600);
	c->SetLogy();
	myHisto->Draw(); // This triggers the event loop and fills the histogram

	// Optional: Save the canvas to a file
	c->SaveAs("histogram.png");

	// Print columns' names, contents of entry selection
	auto colNames = d.GetColumnNames();
	auto select = d.Filter("rdfentry_>20 && rdfentry_<30");
	for (auto &&colName : colNames) {
		auto f = [colName](double i, ULong64_t e){
			if (!std::isnan(i)) std::cout << "entry: " << e << ", " << colName << ": " << i << std::endl;
		};
		select.Foreach(f, {colName, "rdfentry_"});
	}

	// Close the file
	file->Close();
}
