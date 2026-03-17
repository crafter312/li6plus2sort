//Reads in a tree of data from refactor_processor.C and Ecal.dat files
//Plots the calibrated summary spectra
#include <TFile.h>
#include <TTree.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <TH2I.h>

#define BOARD_COUNT 12
#define CHAN_COUNT 32

using namespace std;

vector<vector<float>> getcalvals(string frontfile, string backfile, string deltfile) {

	vector<vector<float>> calvals;

  ifstream ffront;
  ffront.open(frontfile);

  ifstream fback;
  fback.open(backfile);

  ifstream fdelta;
  fdelta.open(deltfile);

  for (int i=0;i<BOARD_COUNT;i++) {
    for (int j=0;j<CHAN_COUNT;j++) {
      int boardnum = i+1;

      if (boardnum == 1 || boardnum == 3 || boardnum == 5 || boardnum == 7) {
        int temp1;
        int temp2;
        float p0;
        float p1;
        ffront >> temp1 >> temp2 >> p1 >> p0;
        
        calvals.push_back({p0,p1});
      }
      else if (boardnum == 2 ||boardnum == 4 ||boardnum == 6 || boardnum == 8) {
        int temp1;
        int temp2;
        float p0;
        float p1;
        fback >> temp1 >> temp2 >> p1 >> p0;
        
        calvals.push_back({p0,p1});
      }
      else if (boardnum > 8 && boardnum < 13) {
        int temp1;
        int temp2;
        float p0;
        float p1;
        fdelta >> temp1 >> temp2 >> p1 >> p0;
        
        calvals.push_back({p0,p1});
      }
      else {
        cout << "Too many boards" << endl;
        abort();
      }
    }
  }

  ffront.close();
  fback.close();
  fdelta.close();

  return calvals;

}

void calsumplots() {

  // Get number of entries from input file
  //Delta file : run-511-par
  //E file : run-509-510-par

	string iprefix = "run-509-510-par";
	string path = "../../SpecTcl_6Liplus2IAS/";
	size_t numentries;

	TFile *file = TFile::Open((path + iprefix + ".root").c_str());
	if (!file || file->IsZombie()) {
		std::cerr << "Error opening file!" << std::endl;
		return;
	}

	// Check if tree exists in the file
	TTree *tree = (TTree*)file->Get("tpar");
	if (!tree) {
		std::cerr << "Tree 't' not found in the file!" << std::endl;
		file->Close();
		return;
	}
	numentries = tree->GetEntries();

  cout << "Num entries in tree: " << numentries << endl;

  //Delta E spectrum
  TH2I * DeltaSumCal = new TH2I("DeltaESum_Cal","",128,0,128,2000,0,80);
  TH2I * FrontSumCal = new TH2I("FrontESum_Cal","",128,0,128,2000,0,80);
  TH2I * BackSumCal = new TH2I("BackESum_Cal","",128,0,128,2000,0,80);

  //Calibration filepath
  string calpath = "../Cal/";
  string file_front = calpath + "FrontEcal.dat";
  string file_back = calpath + "BackEcal.dat";
  string file_delta = calpath + "DeltaEcal.dat";

  vector<vector<float>> calvec = getcalvals(file_front.c_str(),file_back.c_str(),file_delta.c_str());
  cout << calvec[0][0] << " " << calvec[0][1] << endl;

  TTreeReader reader(tree);
  TTreeReaderValue<vector<size_t>> board(reader, "board");
  TTreeReaderValue<vector<size_t>> channel(reader, "chan");
  TTreeReaderValue<vector<size_t>> energy(reader, "e");

  while (reader.Next()) {
    vector<size_t> evec = *energy;
    vector<size_t> boardvec = *board;
    vector<size_t> chanvec = *channel;

    vector<float> Ecal;

    for (int i=0;i<boardvec.size();i++) {
      int index = (boardvec[i]-1)*32 + chanvec[i];
      Ecal.push_back(evec[i]*calvec[index][1] + calvec[index][0]);

      if (boardvec[i] > 8 && boardvec[i] < 13) {
        int delID = (boardvec[i]-9)*32 + chanvec[i];
        DeltaSumCal->Fill(delID,Ecal[i]);
      }
      else if (boardvec[i] == 1 || boardvec[i] == 3 || boardvec[i] == 5 || boardvec[i] == 7) {
        int fID = ((boardvec[i]-1)/2)*32 + chanvec[i];
        FrontSumCal->Fill(fID,Ecal[i]);
      }
      else if (boardvec[i] == 2 ||boardvec[i] == 4 ||boardvec[i] == 6 || boardvec[i] == 8) {
        int bID = ((boardvec[i]/2)-1)*32 + chanvec[i];
        BackSumCal->Fill(bID,Ecal[i]);
      }
      else {
        cout << "invalid board number, skip event" << endl;
        continue;
      }
    }
    
  }

  if (DeltaSumCal->GetEntries() > 0) DeltaSumCal->DrawClone("colz");
  if (FrontSumCal->GetEntries() > 0) FrontSumCal->DrawClone("colz");
  if (BackSumCal->GetEntries() > 0) BackSumCal->DrawClone("colz");
}
