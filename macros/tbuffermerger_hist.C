#include <ROOT/TBufferMerger.hxx>
#include <ROOT/TTreeProcessorMT.hxx>
#include <TH1I.h>
#include <TRandom.h>
#include <TTree.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <cstdlib>
#include <string>

using namespace std;

void tbuffermerger_hist() {

	// Make input file for testing
	string fin_name = "fin.root";
	string tin_name = "tin";
	{
		TFile fin(fin_name.c_str(), "RECREATE");
		fin.cd();
		TTree tin(tin_name.c_str(), tin_name.c_str());
		double gauss;
		tin.Branch("gauss", &gauss);
		
		TRandom rand;
		size_t nentries = 10000;
		for (size_t i = 0; i < nentries; i++) {
			gauss = rand.Gaus();
			tin.Fill();
		}
		fin.Write();
		fin.Close();
	}

	// Enable implicity multi-threading
	const int nthreads = 4;
	ROOT::EnableImplicitMT(nthreads);

	// Create a TTreeProcessorMT from the tree created above
	ROOT::TTreeProcessorMT tp(fin_name.c_str(), tin_name.c_str());
	
	// Create the TBufferMerger: this class orchestrates the parallel writing
	string ofname = "test.root";
	ROOT::TBufferMerger merger(ofname.c_str(), "RECREATE");

	// Define the function that will process a subrange of the tree.
	// The function must receive only one parameter, a TTreeReader,
	// and it must be thread safe. To enforce the latter requirement,
	// TBufferMerger::GetFile will be used for the output file.
	auto f = [&](TTreeReader &reader) {
		TTreeReaderValue<double> gauss(reader, "gauss");
	
		double whatever;

		// Get thread safe file and create thread-local tree for output
		auto f = merger.GetFile();
		f->cd();
		TTree t("t", "t");
		t.Branch("whatever", &whatever);

		TDirectory* dir = f->mkdir("dir");
		dir->cd();
		TH1I h("h", "h", 100, -1., 1.);

		while (reader.Next()) {
			whatever = *gauss;
			t.Fill();
			h.Fill(whatever);
		}
		h.Write();
		f->Write();
	};

	// Execute multi-threaded tree processing
	tp.Process(f);
}



