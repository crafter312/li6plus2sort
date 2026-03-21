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

void Diamondcal() {

	//Th-229 source
	float alphaE[5] = {4.8453,5.830,6.341,7.0669,8.376};
	float alphaE_corr[5] = {4.66,5.668,6.184,6.920,8.244}; 
	float centroids_06[5] = {1229.82,1486.76,1623.53,1813.72,2150.36}; //for 0.6 atten
	float centroids_04[5] = {862.905,1032.46,1123.24,1251.22,1477.71}; //for 0.4 atten


	//Root output file
  TFile * outfile = new TFile("DiamondFit.root", "RECREATE");
  outfile->cd();

  //Delta E spectrum
  TGraph * dfit_06 = new TGraph(5,centroids_06,alphaE_corr);
  dfit_06->SetMarkerStyle(20);
  dfit_06->SetMarkerSize(1.5);
  
  TGraph * dfit_04 = new TGraph(5,centroids_04,alphaE_corr);
  dfit_04->SetMarkerStyle(20);
  dfit_04->SetMarkerSize(1.5);
  
  TF1* f1 = new TF1("f1","pol1",centroids_06[0],centroids_06[4]);
  
  dfit_06->Fit("f1","R+");
	dfit_06->DrawClone("AP");
	
	TF1* f1_04 = new TF1("f1_04","pol1",centroids_04[0],centroids_04[4]);
  
  dfit_04->Fit("f1_04","R+");
	dfit_04->DrawClone("AP");
	
	outfile->Write();
  outfile->Close();
}
