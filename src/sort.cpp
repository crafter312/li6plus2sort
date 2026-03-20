// Code to analyze data from Gobbi Si-Si array + TexNeut neutron detector
// Originally written by Nicolas Dronchi, 2020
// Heavily modified by Henry Webb (h.s.webb@wustl.edu), August 2025
// Now skips unpacking, reads values from SpecTcl-generated ROOT file
// (i.e. SpecTcl now does the unpacking)

#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
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

#include "Gobbi.h"
#include "histo.h"
#include "Input.h"
#include "SortConfig.h"

using namespace std;

int main() {

	clock_t t;
	t = clock();

	// Enable implicit multi-threading
	int nthreads = 6;
	ROOT::EnableImplicitMT(nthreads);

	// Load config file for sort code
	SortConfig sortConfig("../config/sort.config");

	// TNLIB (Alex's TexNeut library) setup
	config configFile(sortConfig.GetTnlibConfig());
	detector texneut;
	texneut.fillmaps(configFile.GetExpInfoDir(), configFile.GetBarMapFile(), configFile.GetPosMapFile(), configFile.GetGainFile());

	// Create the TBufferMerger: this class orchestrates the parallel writing to an output ROOT file
	string ofname = configFile.GetOutputDir() + sortConfig.GetOfileName();
	ROOT::TBufferMerger merger(ofname.c_str(), "RECREATE");

	// Define the function that will process a subrange of the tree.
	// The function must receive only one parameter, a TTreeReader,
	// and it must be thread safe. To enforce the latter requirement,
	// TBufferMerger::GetFile will be used for the output file.
	int rcount = 0;
	auto f = [&](TTreeReader &reader) {
		Input input(reader);

		//detector texneut; // contains mapping information for TexNeut

		// Output using thread safe file
		auto f = merger.GetFile();
		f->cd();

		const char* otname = sortConfig.GetOtreeName().c_str();

		// TODO: modify histo and Gobbi classes to work in this new multi-threaded framework with pre-unpacked ROOT file
		// Build three classes that are used to organize and unpack each physicsevent
		cout << "Init histo" << endl;
		histo Histo(f);
		cout << "Init Gobbi" << endl;
		Gobbi gobbi(input, Histo, sortConfig); //tex
		cout << "Thread-wise event loop starting..." << endl;
		// Event loop
		while (reader.Next()) {
			input.ReadAndRefactor();
			//cout << "here post read and refactor" << endl;
			//tex.CustomFillNecessary(input.getNeutMult(), input.getNeutE(), ...); // input.getNeutE(), ... -> type: std::vector<size_t>&
			//bool good = tex.analyze();
			//if (!good) continue;
			//gobbi.LoadTexNeutSolution();
			gobbi.analyze();
			//cout << "here post Gobbi analyze" << endl;
			rcount++;
			//if (rcount == 100) break;

		}
		rcount = 0;
		cout << "Thread-wise event loop finished!" << endl;

	};

	string runNumbersFile = sortConfig.GetRunNumbersFile();
	ifstream runFile(runNumbersFile);
	if (runFile.fail()) throw invalid_argument(string(BOLDRED) + string("Run numbers file ") + runNumbersFile + std::string(" does not exist or failed to open") + std::string(RESET));

	// Loop through run numbers
	string itname = sortConfig.GetItreeName();
	int runnum;
	ostringstream datastring;
	for (;;) {
		runFile >> runnum;
		if (runFile.eof() || runFile.bad()) break;

		datastring.str("");
		datastring << configFile.GetTNDataDir() << "run-" << runnum << ".root"; //TODO: make sure to match SpecTcl output file name format, (use setfill('0') << setw(4) or something similar if necessary)

		// Check status of input run data file
		size_t numentries;
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
			numentries = tree->GetEntries();
			file->Close();
		}

		cout << "Processing TTree in file: " << datastring.str() << " (" << numentries << ")" << endl;

		// Create a TTreeProcessorMT: this class orchestrates the parallel processing of an input tree
		ROOT::TTreeProcessorMT tp(datastring.str().c_str(), itname.c_str());

		// Execute multi-threaded tree processing
		tp.Process(f);
	} // loop over run numbers from number.beam

	t = clock() - t;
	cout << "run time: " << (float)t/CLOCKS_PER_SEC/60 << " min" << endl;

	return 0;
}



