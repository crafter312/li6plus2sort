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

	string iprefix = "run-511-par";
	string path = "../../data/";
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

	//Root output file
  TFile * outfile = new TFile("SiUnpacker.root", "RECREATE");
  outfile->cd();

  //Delta E spectrum
  TH2I * DeltaSumCal = new TH2I("DeltaESum_Cal","",128,0,128,2000,0,80);
  TH2I * FrontSumCal = new TH2I("FrontESum_Cal","",128,0,128,2000,0,80);
  TH2I * BackSumCal = new TH2I("BackESum_Cal","",128,0,128,2000,0,80);

	//E vs time for b1 chan 0
	TH2I * EvsT_B1C0 = new TH2I("EvsT_B1C0","",100,0,1000,8192,0,16384);

	TDirectoryFile * EPlots = new TDirectoryFile("EPlots","EPlots");
	TDirectory * EBoards[BOARD_COUNT];
	TH1I * eplot[BOARD_COUNT][CHAN_COUNT];

	TDirectoryFile * TimePlots = new TDirectoryFile("TimePlots","TimePlots");
	TDirectory * TimeBoards[BOARD_COUNT];
	//1D time
	TimePlots->cd();
	TH1I * timeplot[BOARD_COUNT][CHAN_COUNT];
	for (int i=0;i<BOARD_COUNT;i++) {
	
		string direcname = "Board" + to_string(i+1);
		TimeBoards[i] = TimePlots->mkdir(direcname.c_str(),direcname.c_str());
		TimeBoards[i]->cd();
		for (int j=0;j<CHAN_COUNT;j++) {
			string tplotname = "Time_" + to_string(i+1) + "_" + to_string(j);
			timeplot[i][j] = new TH1I(tplotname.c_str(),"",8192,0,16384);
		}
		
		EBoards[i] = EPlots->mkdir(direcname.c_str(),direcname.c_str());
		EBoards[i]->cd();
		for (int j=0;j<CHAN_COUNT;j++) {
			string plotname = "E_" + to_string(i+1) + "_" + to_string(j);
			eplot[i][j] = new TH1I(plotname.c_str(),"",4096,0,8192);
		}
		
	}

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
  TTreeReaderValue<vector<size_t>> time(reader, "t");

  while (reader.Next()) {
    vector<size_t> evec = *energy;
    vector<size_t> boardvec = *board;
    vector<size_t> chanvec = *channel;
    vector<size_t> timevec = *time;

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
        cout << "invalid board nu  outfile->Close();mber, skip event" << endl;
        continue;
      }
      
      // Look at T vs E for channel 0
      if (boardvec[i] == 9 && chanvec[i] == 4) {
      	EvsT_B1C0->Fill(evec[i],timevec[i]);
      }
      timeplot[boardvec[i]-1][chanvec[i]]->Fill(timevec[i]);
      eplot[boardvec[i]-1][chanvec[i]]->Fill(evec[i]);
    }
    
  }

	outfile->Write();
  outfile->Close();
}
