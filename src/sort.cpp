// Code to analyze data from Gobbi Si-Si array + TexNeut neutron detector
// Originally written by Nicolas Dronchi, 2020
// Heavily modified by Henry Webb (h.s.webb@wustl.edu), August 2025

#include <ROOT/TBufferMerger.hxx>
#include <ROOT/TThreadedObject.hxx>
#include <ROOT/TTreeProcessorMT.hxx>
#include <TFile.h>
#include <TTree.h>

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Gobbi.h"
#include "histo.h"

using namespace std;

int main() {

	clock_t t;
	t = clock();

	// Enable implicit multi-threading
	int nthreads = 4;
	ROOT::EnableImplicitMT(nthreads);

	string directory = "/data2/li7_may2022/"; //TODO: replace with CMake/compile-time variable, make sure is correct directory post-experiment

	// Create the TBufferMerger: this class orchestrates the parallel writing to an output ROOT file
	ROOT::TBufferMerger merger("sort.root", "RECREATE");

	// Define the function that will process a subrange of the tree.
	// The function must receive only one parameter, a TTreeReader,
	// and it must be thread safe. To enforce the latter requirement,
	// TBufferMerger::GetFile will be used for the output file.
	auto f = [&](TTreeReader &reader) {
		Input input(reader);

		// Output using thread safe file
		auto f = merger.GetFile();

		// TODO: modify histo and Gobbi classes to work in this new multi-threaded framework with pre-unpacked ROOT file
		// Build three classes that are used to organize and unpack each physicsevent
		histo Histo(f);
		Gobbi gobbi(input, Histo);

		// Event loop
		while (reader.Next()) {
			input.ReadAndRefactor();
			gobbi.analyze();
		}
	};

	ifstream runFile;
	runFile.open("numbers.beam");

	// Loop through run numbers
	int runnum;
	ostringstream datastring;
	for (;;) {
		runFile >> runnum;
		if (runFile.eof() || runFile.bad()) break;

		datastring.str("");
		datastring << directory << "run" << runnum << "/run-" << setfill('0') << setw(4) << runnum << ".root"; //TODO: make sure to match SpecTcl output file name format

		// Check status of input run data file
		size_t numentries;
		{
			TFile *file = TFile::Open(datastring.str().c_str());
			if (!file || file->IsZombie()) {
				cerr << "Error opening file for run " << runnum << "!" << endl;
				continue;
			}

			// Check if tree exists in the file
			TTree *tree = (TTree*)file->Get("t");
			if (!tree) {
				cerr << "Tree 't' not found in file for run " << runnum << "!" << endl;
				file->Close();
				continue;
			}
			numentries = tree->GetEntries();
			file->Close();
		}

		cout << "Processing TTree in file: " << datastring.str() << " (" << numentries << ")" << endl;

		// Create a TTreeProcessorMT: this class orchestrates the parallel processing of an input tree
		ROOT::TTreeProcessorMT tp(datastring.c_str(), "t");

		// Execute multi-threaded tree processing
		tp.Process(f);
	} // loop over run numbers from number.beam

  t = clock() - t;
  cout << "run time: " << (float)t/CLOCKS_PER_SEC/60 << " min" << endl;

  return 0;
}



