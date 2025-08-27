// Code to analyze data from Gobbi Si-Si array + TexNeut neutron detector
// Originally written by Nicolas Dronchi, 2020
// Heavily modified by Henry Webb (h.s.webb@wustl.edu), 2025

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

				//TODO: PUT OTHER EVENT PROCESSING/ANALYSIS STEPS HERE (input values are stored in Input class, need to pass around as function arguments)

			}
			f->Write();
		};

		// Execute multi-threaded tree processing
		tp.Process(f);




//TODO: REMOVE ALL BELOW ITEMS ONCE ABOVE MODIFICATIONS ARE COMPLETE



    for(;;)
    //for(int cnt=0; cnt < 10000; cnt++)  // loop over items in a evtfile
    {
      //cout << "cnt " << cnt << endl;
      int const hBufferWords = 4;
      int const hBufferBytes = hBufferWords*2;
      unsigned short hBuffer[hBufferWords];
      
      //read a section 8 bytes long and split into 2 byte chunks 
      //(each 2byte chunk is little endian)
      evtfile.read((char*)hBuffer,hBufferBytes);
      //example 0x0032    0x0000    0x001e    0x0000
      //        nbytes    nbytes2   type      type2
        
      if(evtfile.eof())
      {
        cout << "eof found" << endl;
        break;
      }
 
      point = hBuffer;
      int nbytes = *point++;
      int nbytes2 = *point++;
      int type = *point++;
      int type2 = *point;

      //if there is an issue reading ring buffer, look at these
      //cout << hex << nbytes << " " << nbytes2 << " " << type << " " << type2 << endl;
      //if done properly, nbytes and nbytes2 should be combined but 
      //values are never large enough
      
      int dBufferBytes = nbytes - 8;
      // if nbytes = 0032 in hex, then =50-8=42
      
      int dBufferWords = dBufferBytes/2;
      // 21 2byte pairs
    
      unsigned short dBuffer[dBufferWords];
      evtfile.read((char*)dBuffer,dBufferBytes);
      point = dBuffer;

      unsigned short *pos = point;

      //debug code to look at contents of each ring buffer
      //cout << "dbufferwords " << dBufferWords << endl;
      //for (int i=0; i<dBufferWords; i++)
      //{
      //  cout << hex<< *pos++ << " ";
      //}
      //cout << dec << endl;
      
      
      //unknown of what causes these large readouts. We only want events 
      //that come in coincidence 2, then get read out imediately
      //if(nbytes > 100 && type != 2 && physicsEvent > 1)
      //{
      //  cout << "occurs at " << physicsEvent << endl;
      //  cout << "nbytes " << nbytes << " " << nbytes2 << " " << type << " " << type2 << endl;
      //  continue;
      //}


      if (type == 30) //type 30 gives physics information
      {
        NphysicsEvent++;
        //eventually unpack the point
        bool stat = gobbi.unpack(point);
        //if (!stat) break;
      }
      else if (type == 20)
      {
        ScalerBuffer.increment(point);
        NscalerBuffer++;
      }
      else if (type == 31)
      {
        Nscalercounter++;
        //cout << "I don't know what this is yet. please help me." << endl;
      }
      else if (type == 1)
      {
        //header length of 104 indicated by first part of buffer 0x0068
        runno = *point; //The run number is stored in the first part of the buffer header
        cout << "run number = " << runno << ", should match " << runnum << endl;    
      }
      else if (type == 2)
      {
        cout << "got type == 2, flag for end of run" << endl;
        break;
      }
      else if (type == 3)
      {
        Npauses++;
      }
      else if (type == 4)
      {
        Nresumes++;
      }
      else
      {
        cout << " unknown event type " << type << " found" << endl;
        break;
      }
    }//loop over items in a evtfile

    evtfile.close();
    evtfile.clear(); //clear event status in case we had a bad file
  }//loop over number.beam values

  cout << "Front Nstore " << gobbi.counter2 << endl;
  cout << "Front neighbours " << gobbi.counter << endl;
  cout << "physics Event Counters = " << NphysicsEvent << endl;
  cout << "scaler buffers = " << NscalerBuffer << endl;
  cout << "confirm number of scalers = " << Nscalercounter << endl;
  cout << "Numbers of pauses = " << Npauses << endl;
  cout << "Number of resumes = " << Nresumes << endl;

  ScalerBuffer.print();

  t = clock() - t;
  cout << "run time: " << (float)t/CLOCKS_PER_SEC/60 << " min" << endl;

  delete Histo;
  return 0;
}


