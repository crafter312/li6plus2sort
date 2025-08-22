#include <ROOT/TBufferMerger.hxx>
#include <ROOT/TThreadedObject.hxx>
#include <ROOT/TTreeProcessorMT.hxx>
#include <TFile.h>
#include <TGProgressBar.h>
#include <TGFrame.h>
#include <TTree.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#define BOARD_COUNT 12
#define CHAN_COUNT 32

// Number of columns per parameter type in the input file
#define NCOLUMNS BOARD_COUNT * CHAN_COUNT

using namespace std;

struct Progress {
	float mi{0.};
	float ma{100.};
	float pos{0.};
	
	void SetRange(float a, float b) {
		mi = min(a, b);
		ma = max(a, b);
	}

	void SetPos(float p) {
		pos = max(mi, min(ma, p));
	}

	void Increment(float i) {
		pos += i;
		pos = max(mi, min(ma, pos));
	}

	void Print() {
		cout << "\rProgress: " << (pos - mi) / (ma - mi) * 100. << flush;
	}
};

vector<string> generate_column_names(const string& parname) {
	vector<string> columns;
	string b, c;
	for (int board = 1; board <= BOARD_COUNT; board++) {
		for (int chan = 0; chan < CHAN_COUNT; chan++) {
			b = (board < 10 ? "0" : "") + to_string(board);
			c = (chan < 10 ? "0" : "") + to_string(chan);
			columns.push_back("SpecTcl_hinp1_mb1_" + parname + "_" + b + "." + c);
		}
	}
	return columns;
}

void refactor_processor() {

	// Get number of entries from input file
	string iprefix = "run-462";
	string path = "../RootFiles/";
	size_t numentries;
	{
		TFile *file = TFile::Open((path + iprefix + ".root").c_str());
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
		numentries = tree->GetEntries();
	}

	string ifname = path + iprefix + ".root";	
	cout << "Processing input file " << ifname << "..." << endl;
	cout << "Total entries in tree: " << numentries << endl;

	// Enable implicity multi-threading
	int nthreads = 4;
	ROOT::EnableImplicitMT(nthreads);

	// TThreadedObject for tracking progress
	const size_t updateRate = 10000;
	ROOT::TThreadedObject<Progress> progressBar;
	{
		auto pBar = progressBar.Get();
		pBar->SetRange(0., numentries);
		pBar->SetPos(0.);
	}

	// Create a TTreeProcessorMT: specify the file and the tree in it
	ROOT::TTreeProcessorMT tp(ifname.c_str(), "t");
	
	// Create the TBufferMerger: this class orchestrates the parallel writing
	ROOT::TBufferMerger merger((path + iprefix + "-par.root").c_str());

	// Generate column names for reading from input tree
	vector<string> e_columns   = generate_column_names("e");
	vector<string> eLo_columns = generate_column_names("eLo");
	vector<string> t_columns   = generate_column_names("t");

	// Define the function that will process a subrange of the tree.
	// The function must receive only one parameter, a TTreeReader,
	// and it must be thread safe. To enforce the latter requirement,
	// TBufferMerger::GetFile will be used for the output file.
	auto f = [&](TTreeReader &reader) {
		// Get progress bar
		auto pBar = progressBar.Get();
		pair<Long64_t, Long64_t> range = reader.GetEntriesRange();
		pBar->SetRange((float)get<0>(range), (float)get<1>(range));
		pBar->SetPos((float)get<0>(range));

		// Create reader values for all columns, iteratively
		vector<TTreeReaderValue<double>> eRVs;
		vector<TTreeReaderValue<double>> eLoRVs;
		vector<TTreeReaderValue<double>> tRVs;
		for (int i = 0; i < NCOLUMNS; i++) {
			eRVs.push_back({reader, e_columns[i].c_str()});
			eLoRVs.push_back({reader, eLo_columns[i].c_str()});
			tRVs.push_back({reader, t_columns[i].c_str()});
		}

		vector<size_t> hits_board;
		vector<size_t> hits_chan;
		vector<size_t> hits_e;
		vector<size_t> hits_eLo;
		vector<size_t> hits_t;

		// Get thread safe file and create thread-local tree for output
		auto f = merger.GetFile();
		TTree tpar("tpar", "tpar");
		tpar.Branch("board", &hits_board);
		tpar.Branch("chan", &hits_chan);
		tpar.Branch("e", &hits_e);
		tpar.Branch("eLo", &hits_e);
		tpar.Branch("t", &hits_t);

		size_t index;
		double e;
		size_t dn = 0;
		while (reader.Next()) {
			hits_board.clear();
			hits_chan.clear();
			hits_e.clear();
			hits_eLo.clear();
			hits_t.clear();
			for (size_t board = 0; board < BOARD_COUNT; board++) {
				for (size_t chan = 0; chan < CHAN_COUNT; chan++) {
					index = (board * CHAN_COUNT) + chan; // make sure this matches the order from generate_column_names
					e = *(eRVs[index]);

					if (std::isnan(e) || (e == 0)) continue;
					hits_board.push_back(board+1);
					hits_chan.push_back(chan);
					hits_e.push_back((size_t)e);
					hits_eLo.push_back((size_t)(*(eLoRVs[index])));
					hits_t.push_back((size_t)(*(tRVs[index])));
				}
			}
			tpar.Fill();

			// Handle progress bar
			dn++;
			if (dn == updateRate) {
				pBar->Increment((float)updateRate);
				pBar->Print();
				dn = 0;
			}
		}
		f->Write();
	};

	// Execute multi-threaded tree processing
	tp.Process(f);
	
	cout << endl;
}



