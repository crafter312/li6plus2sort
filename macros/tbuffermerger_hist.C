#include <ROOT/TBufferMerger.hxx>
#include <ROOT/TTreeProcessorMT.hxx>
#include <TH1I.h>
#include <TRandom.h>
#include <TTree.h>

#include <string>

void tbuffermerger_hist() {

	// Enable implicity multi-threading
	int nthreads = 4;
	ROOT::EnableImplicitMT(nthreads);

	// Create a TTreeProcessorMT: specify the file and the tree in it
	string ifname = "../RootFiles/run-462.root";
	ROOT::TTreeProcessorMT tp(ifname.c_str(), "t");
	
	// Create the TBufferMerger: this class orchestrates the parallel writing
	string ofname = "../RootFiles/test.root";
	ROOT::TBufferMerger merger(ofname.c_str(), "RECREATE");

	// Define the function that will process a subrange of the tree.
	// The function must receive only one parameter, a TTreeReader,
	// and it must be thread safe. To enforce the latter requirement,
	// TBufferMerger::GetFile will be used for the output file.
	auto f = [&](TTreeReader &reader) {
		double whatever;

		// Get thread safe file and create thread-local tree for output
		auto f = merger.GetFile();
		f->cd();
		TTree t("t", "t");
		t.Branch("whatever", &whatever);

		TDirectory* dir = f->mkdir("dir");
		TH1I h("h", "h", 100, -1., 1.);
		h.SetDirectory(dir);

		TRandom rand;
		while (reader.Next()) {
			whatever = rand.Gaus();
			t.Fill();
			h.Fill(whatever);
		}
		h.Write();
		f->Write();
	};

	// Execute multi-threaded tree processing
	tp.Process(f);
}



