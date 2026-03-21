// Code to analyze data from Gobbi Si-Si array + TexNeut neutron detector
// Originally written by Nicolas Dronchi, 2020
// Heavily modified by Henry Webb (h.s.webb@wustl.edu), August 2025
// Now skips unpacking, reads values from SpecTcl-generated ROOT file
// (i.e. SpecTcl now does the unpacking). Uses TNLIB TexNeut analysis
// library written by Alex Alafa.

#include <atomic>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <ROOT/TBufferMerger.hxx>
#include <ROOT/TTreeProcessorMT.hxx>
#include <TH1I.h>
#include <TFile.h>
#include <TTree.h>

#include <config.hpp>
#include <detector.hpp>
#include <eventclass.hpp>
#include <stuffing.hpp>
#include <tof_needs.hpp>

#include "Gobbi.h"
#include "histo.h"
#include "Input.h"
#include "SortConfig.h"

using namespace std;

int main() {

	clock_t t;
	t = clock();
	
	// Load config file for sort code
	SortConfig sortConfig("../config/sort.config");

	// Enable implicit multi-threading
	int nthreads = 6;
	ROOT::EnableImplicitMT(nthreads);
	
	// Setup for multi-threaded progress bar
	const size_t updateRate = sortConfig.GetUpdateRate();
	std::atomic<long long> globalProcessed{0};
	mutex consoleMutex; // To prevent text scrambling

	// TNLIB (Alex's TexNeut library) setup
	config configFile(sortConfig.GetTnlibConfig());
	detector texneut;
	texneut.fillmaps(configFile.GetExpInfoDir(), configFile.GetBarMapFile(), configFile.GetPosMapFile(), configFile.GetGainFile());

	// Create the TBufferMerger: this class orchestrates the parallel writing to an output ROOT file
	string ofname = configFile.GetOutputDir() + sortConfig.GetOfileName();
	ROOT::TBufferMerger merger(ofname.c_str(), "RECREATE");
	
	// Initialize some variables up here so that they are accessible inside the lambda function
	int runnum;
	size_t numentries = 0;
	
	// Counters for certain particle combinations, using atomic to be thread-safe
	// Start with 6Li -> npa
	atomic<size_t> count_ap0n{0};
	atomic<size_t> count_ap_withn{0};
	atomic<size_t> count_ap1n{0};
	atomic<size_t> count_ap2n{0};
	atomic<size_t> count_ap3n{0};
	
	/******** EVENT PROCESSING LAMBDA FUNCTION ********/
	
	// Define the function that will process a subrange of the tree.
	// The function must receive only one parameter, a TTreeReader,
	// and it must be thread safe. To enforce the latter requirement,
	// TBufferMerger::GetFile will be used for the output file.
	auto f = [&](TTreeReader &reader) {
		Input input(reader);

		// Output using thread safe file
		auto f = merger.GetFile();
		f->cd();

		const char* otname = sortConfig.GetOtreeName().c_str();

		// Initialize analysis classes
		histo Histo(f);
		event texneutevent;
		Gobbi gobbi(input, Histo, sortConfig, runnum, texneutevent);
		
		// Thread-local event loop
		size_t localCounter = 0;
		while (reader.Next()) {
		
			// First, take input file from SpecTcl and refactor into usable hit list format
			input.ReadAndRefactor();
			
			// TexNeut analysis
			vector<size_t> texneut_tdcchans;
			vector<int> texneut_tdcts;
			input.GetTDC().FillTexNeutHitVectors(texneut_tdcchans, texneut_tdcts);
			const Input::TexNeutInput& texin = input.GetTexNeut();
			texneutevent.CustomFillNecessary(texin.GetNhits(), texin.chip, texin.chan, texin.a, texin.b, texin.c, texin.t, texneut_tdcchans, texneut_tdcts);
			texneutevent.analyse(texneut, 1234, Triple());
			
			// Gobbi analysis
			gobbi.analyze();
			
			// Handle progress bar
			localCounter++;
			if (localCounter >= updateRate) {
				long long total = globalProcessed.fetch_add(localCounter);
				lock_guard<mutex> lock(consoleMutex);
				long double percentage = (long double)total / numentries * 100.0;
				cout << "\r[ " << setw(7) << fixed << setprecision(4) 
				     << percentage << "% ] Processing entries..." << setw(10) << " " << flush;

				localCounter = 0;
			}
		}

		// Adding counters here that will tick up for different particle combinations
		// All counters should be of type atomic<> for thread safety
		count_ap0n += gobbi.a_p_0n;
		count_ap1n += gobbi.a_p_1n;
		count_ap2n += gobbi.a_p_2n;
		count_ap3n += gobbi.a_p_3n;
		count_ap_withn += gobbi.a_p_withn;
	};
	
	/******** RUN NUMBER LOOP ********/

	string runNumbersFile = sortConfig.GetRunNumbersFile();
	ifstream runFile(runNumbersFile);
	if (runFile.fail()) throw invalid_argument(string(BOLDRED) + string("Run numbers file ") + runNumbersFile + std::string(" does not exist or failed to open") + std::string(RESET));

	// First, loop through runs and find the total number of entries
	string itname = sortConfig.GetItreeName();
	ostringstream datastring;
	for (;;) {
		runFile >> runnum;
		if (runFile.eof() || runFile.bad()) break;

		datastring.str("");
		datastring << configFile.GetTNDataDir() << "run-" << runnum << ".root";

		// Check status of input run data file
		TFile *file = TFile::Open(datastring.str().c_str());
		if (!file || file->IsZombie()) continue;

		// Check if tree exists in the file
		TTree *tree = (TTree*)file->Get(itname.c_str());
		if (!tree) {
			file->Close();
			continue;
		}
		numentries += tree->GetEntries();
		file->Close();
	}

	// Then, loop through run numbers from numbers.beam and perform analysis on each
	runFile.clear();
	runFile.seekg(0);
	for (;;) {
		runFile >> runnum;
		if (runFile.eof() || runFile.bad()) break;

		datastring.str("");
		datastring << configFile.GetTNDataDir() << "run-" << runnum << ".root";

		// Check status of input run data file
		size_t numentries_singlefile;
		{
			TFile *file = TFile::Open(datastring.str().c_str());
			if (!file || file->IsZombie()) {
				cerr << "Error opening file for run " << runnum << "!" << endl;
				continue;
			}

			// Check if tree exists in the file
			TTree *tree = (TTree*)file->Get(itname.c_str());
			if (!tree) {
				cerr << "Tree '" << itname << "' not found in file for run " << runnum << "!" << endl;
				file->Close();
				continue;
			}
			numentries_singlefile = tree->GetEntries();
			file->Close();
		}

		cout << "Processing TTree in file: " << datastring.str() << " (" << numentries_singlefile << ")" << endl;

		// Create a TTreeProcessorMT: this class orchestrates the parallel processing of an input tree
		ROOT::TTreeProcessorMT tp(datastring.str().c_str(), itname.c_str());

		// Execute multi-threaded tree processing
		tp.Process(f);
		cout << endl;
	}

	t = clock() - t;
	cout << "run time: " << (float)t/CLOCKS_PER_SEC/60 << " min" << endl;

	cout << "************************************************************************" << endl;
	cout << "EVENT COUNTERS                                                         " << endl;
	cout << "Neutron time gates are in Gobbi.h and counters at bottom of Gobbi main  " << endl;
	cout << "The PSD time gates are based on offset-adjusted TDC spectra             " << endl;
	cout << "These counts are also gated on OR A time                                " << endl;
	cout << "1p + 1a + 0n: " << count_ap0n << endl;
	cout << "1p + 1a + 1n: " << count_ap1n << endl;
	cout << "1p + 1a + 2n: " << count_ap2n << endl;
	cout << "1p + 1a + 3n: " << count_ap3n << endl;
	cout << "1p + 1a with any number of neutrons: " << count_ap_withn << endl;

	return 0;
}



