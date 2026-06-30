#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH2I.h>
#include <TMarker.h>
#include <TPolyLine.h>
#include <TTree.h>
#include <TTreeReader.h>

#include "../src/OutStructs.h"

using namespace std;

/**
 * NOTE THAT THIS MACRO SHOULD BE RUN FROM INSIDE THE BUILD
 * DIRECTORY, SINCE ALL FILE PATHS BELOW ARE RELATIVE AND
 * ROOT DICTIONARIES ARE REQUIRED.
 */

void TNCrystalGates() {

	// Macro parameters
	string ifname = "../RootFiles/sort_all.root";
	string tname = "tpar";
	size_t nbars = 96; // 16 bars wide, 6 layers deep for 96 total bars
	vector<size_t> barlist = {12, 28, 29, 56, 57, 63, 64, 65, 69, 79, 89}; // or process specific bars, if desired
	string histArg = "texneut.Aint_top:texneut.Aint_bot>>hist";

	// Open TTree with TexNeut results
	TFile *ifile = TFile::Open(ifname.c_str());
	if (!ifile || ifile->IsZombie()) {
		cout << "Input file " << ifname << " does not exist or cannot be opened" << endl;
		return;
	}
	TTree *tpar = (TTree*)ifile->Get(tname.c_str());
	if (!tpar) {
		cout << "TTree " << tname << " not found in file or could not be opened" << endl;
		ifile->Close();
		return;
	}
	
	// Some things from the CsICal.C CsI calibration macro I'm basing this on
	// that I copied, not sure what they do
	gROOT->Reset();
	gStyle->SetPalette(kBird);
	gStyle->SetOptStat(0);

	// Find canvas or make new one
	gROOT->cd();
	TCanvas *mycan = (TCanvas*)gROOT->FindObjectAny("mycan");
	if (!mycan) mycan = new TCanvas("mycan", "mycan");

	// Make histogram for 2D finger plots
	double max = 8192;
	size_t nbins = 1024; // originally started with 400
	TH2I hist("hist", "hist", nbins, 0, max, nbins, 0, max);

	// Make output file for results
	ofstream fout("../config/TNCrystalParams.txt");
	fout << nbars << endl;

	// Loop through all TexNeut bars
	string histGate, histGate2;
	TMarker* mark;
	double xmax, ymax, checkx, checky;
	double cutx1, cuty1, cutx2, cuty2;
	double cutdiffx, cutdiffy, satCutoff;
	TPolyLine* line;
	TPolyLine* line2;
	TPolyLine* line3;
	vector<vector<double>> centerXs, centerYs, topXs, topYs, botXs, botYs;
	vector<vector<double>> centerKs, topKs, botKs;
	vector<double> Xmins, Ymins;
	//for (size_t i = 59; i < nbars; i++) { // normal loop for all bars
	for (size_t i : barlist) { // loop for specific bar #s
		cout << "==========================================================" << endl;
		cout << "Selecting crystal gates for bar " << i << ":" << endl;
		fout << i << endl;
		histGate = "texneut.bar==" + to_string(i);
		hist.GetXaxis()->SetRangeUser(0, max);
		hist.GetYaxis()->SetRangeUser(0, max);
		tpar->Draw(histArg.c_str(), histGate.c_str(), "colz");

		mycan->Modified();
		mycan->Update();

		// Zoom into histogram by clicking on new upper right corner point
		cout << "Select point as max to zoom in..." << endl;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker");
		xmax = mark->GetX();
		ymax = mark->GetY();
		hist.GetXaxis()->SetRangeUser(0, xmax);
		hist.GetYaxis()->SetRangeUser(0, ymax);
		cout << "Point (" << xmax << ", " << ymax << ") selected, zooming in!" << endl;
		delete mark;

		mycan->Modified();
		mycan->Update();

		// Get cutoff point for if there is saturation in one of the PMTs
		// (this would not work if there was saturation in both PMTs, I
		// guess, although I have yet to see that)
		cout << "Select two points to mark line for saturation cutoff..." << endl;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker");
		cutx1 = mark->GetX();
		cuty1 = mark->GetY();
		fout << cutx1 << "\t" << cuty1 << endl;
		delete mark;
		mark = (TMarker*)mycan->WaitPrimitive("TMarker");
		cutx2 = mark->GetX();
		cuty2 = mark->GetY();
		fout << cutx2 << "\t" << cuty2 << endl;
		delete mark;
		cutdiffx = abs(cutx2 - cutx1);
		cutdiffy = abs(cuty2 - cuty1);
		histGate2 = (cutdiffx > cutdiffy)
			? ("texneut.Aint_top<" + to_string((cuty1 + cuty2) / 2.))
			: ("texneut.Aint_bot<" + to_string((cutx1 + cutx2) / 2.));
		cout << "Saturation removal gate: " << histGate2 << endl;
		histGate += " && (" + histGate2 + ")";
		tpar->Draw(histArg.c_str(), histGate.c_str(), "colz");

		mycan->Modified();
		mycan->Update();

		/******** DRAW CENTER LINE ********/

		// Draw line separating two middle fingers
		cout << "Draw center line, click outside of histogram when done" << endl;
		vector<double> centerline_x;
		vector<double> centerline_y;
		vector<double> centerline_k;
		do {
			mark = (TMarker*)mycan->WaitPrimitive("TMarker");
			checkx = mark->GetX();
			checky = mark->GetY();
			centerline_x.push_back(checkx);
			centerline_y.push_back(checky);
			centerline_k.push_back(checkx + checky);
			delete mark;
		} while ((checkx > 0) && (checkx < xmax) && (checky > 0) && (checky < ymax));
		if ((centerline_x.size() < 3) || (centerline_y.size() < 3)) {
			cout << "ERROR: center line needs at least two points, exiting early" << endl;
			return;
		}

		// Last point is outside histogram to exit loop, remove from vectors
		centerline_x.pop_back();
		centerline_y.pop_back();
		centerline_k.pop_back();

		// Output center line to file
		fout << centerline_x.size() << endl;
		for (size_t i = 0; i < centerline_x.size(); i++)
			fout << centerline_x[i] << "\t" << centerline_y[i] << endl;

		// Save lines for later per-event processing
		centerXs.push_back(centerline_x);
		centerYs.push_back(centerline_y);
		centerKs.push_back(centerline_k);

		// Draw points as line and update canvas
		line = new TPolyLine(centerline_x.size(), &centerline_x[0], &centerline_y[0]);
		line->SetLineColor(kRed);
		line->SetLineWidth(2);
		line->SetLineStyle(2);
		line->Draw();

		mycan->Modified();
		mycan->Update();

		/******** DRAW TOP LINE ********/

		// Draw outline of the top curve of the finger distribution
		cout << "Draw outline of top curve of finger distribution, click outside of histogram when done" << endl;
		vector<double> topline_x;
		vector<double> topline_y;
		vector<double> topline_k;
		do {
			mark = (TMarker*)mycan->WaitPrimitive("TMarker");
			checkx = mark->GetX();
			checky = mark->GetY();
			topline_x.push_back(checkx);
			topline_y.push_back(checky);
			topline_k.push_back(checkx + checky);
			delete mark;
		} while ((checkx > 0) && (checkx < xmax) && (checky > 0) && (checky < ymax));
		if ((topline_x.size() < 3) || (topline_y.size() < 3)) {
			cout << "ERROR: top line needs at least two points, exiting early" << endl;
			return;
		}

		// Last point is outside histogram to exit loop, remove from vectors
		topline_x.pop_back();
		topline_y.pop_back();
		topline_k.pop_back();

		// Assume first point is same as center line, make same
		topline_x[0] = centerline_x[0];
		topline_y[0] = centerline_y[0];
		topline_k[0] = centerline_k[0];

		// Output top line to file
		fout << topline_x.size() << endl;
		for (size_t i = 0; i < topline_x.size(); i++)
			fout << topline_x[i] << "\t" << topline_y[i] << endl;

		// Save lines for later per-event processing
		topXs.push_back(topline_x);
		topYs.push_back(topline_y);
		topKs.push_back(topline_k);
		Xmins.push_back(*min_element(topline_x.begin(), topline_x.end()));

		// Draw points as line and update canvas
		line2 = new TPolyLine(topline_x.size(), &topline_x[0], &topline_y[0]);
		line2->SetLineColor(kRed);
		line2->SetLineWidth(2);
		line2->SetLineStyle(2);
		line2->Draw();

		mycan->Modified();
		mycan->Update();

		/******** DRAW BOTTOM LINE ********/

		// Draw outline of the bottom curve of the finger distribution
		cout << "Draw outline of bottom curve of finger distribution, click outside of histogram when done" << endl;
		vector<double> botline_x;
		vector<double> botline_y;
		vector<double> botline_k;
		do {
			mark = (TMarker*)mycan->WaitPrimitive("TMarker");
			checkx = mark->GetX();
			checky = mark->GetY();
			botline_x.push_back(checkx);
			botline_y.push_back(checky);
			botline_k.push_back(checkx + checky);
			delete mark;
		} while ((checkx > 0) && (checkx < xmax) && (checky > 0) && (checky < ymax));
		if ((botline_x.size() < 3) || (botline_y.size() < 3)) {
			cout << "ERROR: bottom line needs at least two points, exiting early" << endl;
			return;
		}

		// Last point is outside histogram to exit loop, remove from vectors
		botline_x.pop_back();
		botline_y.pop_back();
		botline_k.pop_back();

		// Assume first point is same as center line, make same
		botline_x[0] = centerline_x[0];
		botline_y[0] = centerline_y[0];
		botline_k[0] = centerline_k[0];

		// Output bottom line to file
		fout << botline_x.size() << endl;
		for (size_t i = 0; i < botline_x.size(); i++)
			fout << botline_x[i] << "\t" << botline_y[i] << endl;

		// Save lines for later per-event processing
		botXs.push_back(botline_x);
		botYs.push_back(botline_y);
		botKs.push_back(botline_k);
		Ymins.push_back(*min_element(botline_y.begin(), botline_y.end()));

		// Draw points as line and update canvas
		line3 = new TPolyLine(botline_x.size(), &botline_x[0], &botline_y[0]);
		line3->SetLineColor(kRed);
		line3->SetLineWidth(2);
		line3->SetLineStyle(2);
		line3->Draw();

		mycan->Modified();
		mycan->Update();

		// Require click outside of histogram to continue
		cout << "Click outside of histogram to continue" << endl;
		do {
			mark = (TMarker*)mycan->WaitPrimitive("TMarker");
			checkx = mark->GetX();
			checky = mark->GetY();
			delete mark;
		} while ((checkx > 0) && (checkx < xmax) && (checky > 0) && (checky < ymax));
		delete line;
		delete line2;
	}

	/******** EVENT PROCESSING LOOP ********/
	// This portion of code will loop over all
	// TexNeut hits and events to coordinate
	// transform using the above drawn lines.
	// Eventually, this code will be moved
	// into the sort code for real, and below
	// is just for initial testing.

	// The way this works is a -45 degree line
	// is defined through each point in the
	// histogram, (xp, yp), with form
	// x + y = xp + yp = K. The drawn curves
	// from above are then also defined as
	// parametric functions f(x, y) = x + y
	// = K. One then assumes that the curves
	// are monotonically increasing in K,
	// and can use K as the value of import
	// when interpolating or extrapolating.

	// AGAIN, THE BELOW CODE WAS FOR TESTING
	// ONLY AND IS KEPT FOR POSTERITY

/*

	// This function both interpolates or
	// extrapolates given two points and
	// K = xp + yp.
	auto interExtraPolate = [](double x1, double y1, double x2, double y2, double K) -> pair<double, double> {
		double u1 = x1 + y1;
		double u2 = x2 + y2;
		if (abs(u2 - u1) < 1e-9)
			throw invalid_argument("Cannot interpolate or extrapolate using two equal points"); // avoid divide by zero

		double t = (K - u1) / (u2 - u1);
		double new_x = x1 + (t * (x2 - x1));
		double new_y = y1 + (t * (y2 - y1));

		return make_pair(new_x, new_y);
	};

	// This function finds intersection
	// of line x + y = xp + yp = K with
	// a curve defined by a set of input
	// points, curveX and curveY.
	auto findIntersection = [&interExtraPolate](double xp, double yp, const vector<double>& curveX, const vector<double>& curveY, const vector<double>& curveK) -> pair<double, double> {
		double K = xp + yp;
		if ((curveX.size() != curveY.size()) || (curveX.size() != curveK.size()))
			throw invalid_argument("curveX, curveY, curveK must be same size");

		// Handle interpolation or extrapolation cases for last two points
		double n = curveX.size();
		if (curveK[n - 2] > curveK[n - 1])
			throw invalid_argument("curveK must be monotonically increasing");
		if (K >= curveK[n - 2])
			return interExtraPolate(curveX[n - 2], curveY[n - 2], curveX[n - 1], curveY[n - 1], K);

		// Check all other locations on curve, assume that xp + yp is
		// always larger than i = 0 point on curve
		for (size_t i = 0; i < n - 2; i++) {
			double K1 = curveK[i];
			double K2 = curveK[i + 1];
			if (K2 < K1)
				throw invalid_argument("curveK must be monotonically increasing");
			if ((K >= K1) && (K < K2))
				return interExtraPolate(curveX[i], curveY[i], curveX[i + 1], curveY[i + 1], K);
		}

		throw runtime_error("ERROR: Curve intersection point not found");
		return make_pair(curveX.back(), curveY.back());
	};

	// Define test output tree
	TFile* ofile = new TFile("../RootFiles/crystalgate_test.root", "RECREATE");
	ofile->cd();
	TTree* tcalc = new TTree("tcalc", "tcalc");
	int bar, Aint_top, Aint_bot;
	double Lnorm, Rshift;
	tcalc->Branch("bar", &bar);
	tcalc->Branch("Aint_top", &Aint_top);
	tcalc->Branch("Aint_bot", &Aint_bot);
	tcalc->Branch("Lnorm", &Lnorm);
	tcalc->Branch("Rshift", &Rshift);

	// Main entry loop
	cout << "==========================================================" << endl;
	cout << "Looping through input TTree entries..." << endl;
	TTreeReader reader(tpar);
	TTreeReaderValue<vector<OutStructs::TexNeutHit>> texneutoutRV(reader, "texneut");
	vector<OutStructs::TexNeutHit> texneutout;
	while (reader.Next()) {
		texneutout = *texneutoutRV;

		// Loop through all hits in event
		for (OutStructs::TexNeutHit hit : texneutout) {
			bar = hit.bar;
			if (bar >= nbars) continue;

			Aint_top = hit.Aint_top; // "y"
			Aint_bot = hit.Aint_bot; // "x"
			if (((Aint_bot < centerXs[bar][0]) && (Aint_top < centerYs[bar][0]))
				|| ((Aint_bot < Xmins[bar]) && (Aint_top > centerYs[bar][0]))
				|| ((Aint_top < Ymins[bar]) && (Aint_bot > centerXs[bar][0])))
				continue;

			// Find intersection points on user-drawn curves with -45 degree line passing through point
			pair<double, double> centerP = findIntersection(Aint_bot, Aint_top, centerXs[bar], centerYs[bar], centerKs[bar]);
			pair<double, double> topP = findIntersection(Aint_bot, Aint_top, topXs[bar], topYs[bar], topKs[bar]);
			pair<double, double> botP = findIntersection(Aint_bot, Aint_top, botXs[bar], botYs[bar], botKs[bar]);

			// Calculate distances between points
			double Wtop   = sqrt(pow(topP.first - centerP.first, 2) + pow(topP.second - centerP.second, 2));
			double Wbot   = sqrt(pow(botP.first - centerP.first, 2) + pow(botP.second - centerP.second, 2));
			double Dpoint = sqrt(pow(Aint_bot - centerP.first, 2)   + pow(Aint_top - centerP.second, 2));

			// Calculate final value depending on if point is in top or bottom half
			Lnorm = 0.;
			if (Aint_top > centerP.second)
				Lnorm = Dpoint / Wtop;
			else if (Aint_top < centerP.second)
				Lnorm = -Dpoint / Wbot;

			// Calculate shifted radius
			Rshift = sqrt(pow(Aint_bot - centerXs[bar][0], 2) + pow(Aint_top - centerYs[bar][0], 2));

			tcalc->Fill();
		}
	}
	cout << "Done!" << endl;

	ofile->Write();
	ofile->Close();

*/

	ifile->Close();
}



