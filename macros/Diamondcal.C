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
	float centroids_06[5] = {1229.82,1486.76,1623.53,1813.72,2150.36}; //for 0.6 atten (-10 V)
	float centroids_04[5] = {862.905,1032.46,1123.24,1251.22,1477.71}; //for 0.4 atten (-10 V)
	float centroids_04_20V[5] = {873.834,1044.92,1137.44,1263.93,1491.38}; //for 0.4 atten (-20 V)
	float centroids_04_30V[5] = {867.652,1034.81,1128.44,1255.62,1482.76}; //for 0.4 atten (-30 V)


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
  
  TGraph * dfit_04_20V = new TGraph(5,centroids_04_20V,alphaE_corr);
  dfit_04_20V->SetMarkerStyle(20);
  dfit_04_20V->SetMarkerSize(1.5);
  
  TGraph * dfit_04_30V = new TGraph(5,centroids_04_30V,alphaE_corr);
  dfit_04_30V->SetMarkerStyle(20);
  dfit_04_30V->SetMarkerSize(1.5); 
 
  TF1* f1 = new TF1("f1","pol1",centroids_06[0],centroids_06[4]);
  
  dfit_06->Fit("f1","R+");
	dfit_06->DrawClone("AP");
	
	TF1* f1_04 = new TF1("f1_04","pol1",centroids_04[0],centroids_04[4]);
  
  dfit_04->Fit("f1_04","R+");
	dfit_04->DrawClone("AP");
	
	TF1* f1_04_20V = new TF1("f1_04_20V","pol1",centroids_04_20V[0],centroids_04_20V[4]);
  
  dfit_04_20V->Fit("f1_04_20V","R+");
	dfit_04_20V->DrawClone("AP");
	
	TF1* f1_04_30V = new TF1("f1_04_30V","pol1",centroids_04_30V[0],centroids_04_30V[4]);
  
  dfit_04_30V->Fit("f1_04_30V","R+");
	dfit_04_30V->DrawClone("AP");
	
	outfile->Write();
  outfile->Close();
}
