/* Written by Henry Webb (h.s.webb@wustl.edu), August 2025
 * This class handles reading information from a SpecTcl-
 * generated ROOT file and reformatting it into a more
 * easily usable form. The resulting variables are stored
 * in this class on a per-event basis and can be accessed
 * via getter functions. Make sure that the static functions
 * match the format of the branch and leaf names in the
 * SpecTcl output tree. It is also important to make sure
 * that you loop through your TTreeReaderValue objects in
 * the same order in which you create them and their strings.
 * 
 * Modified by Henry Webb (h.s.webb@wustl.edu) and Johnathan
 * Phillips (j.s.phillips@wustl.edu) March 2026 for experiment
 * at TAMU Cyclotron Institute
 */

#include "Input.h"

#include <cmath>

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/******** STATIC FUNCTIONS ********/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

vector<string> Input::GenerateColumnNamesHINP(const string& parname) {
	vector<string> columns;
	string b, c;
	for (size_t board = 1; board <= HINP_BOARD_COUNT; board++) {
		for (size_t chan = 0; chan < HINP_CHAN_COUNT; chan++) {
			b = (board < 10 ? "0" : "") + to_string(board);
			c = (chan < 10 ? "0" : "") + to_string(chan);
			columns.push_back("SpecTcl_hinp1_mb1_" + parname + "_" + b + "." + c);
		}
	}
	return columns;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

vector<string> Input::GenerateColumnNamesPSD(const string& parname) {
	vector<string> columns;
	string chi, cha;
	for (size_t chip = 1; chip <= PSD_CHIP_COUNT; chip++) {
		for (size_t chan = 0; chan < PSD_CHAN_COUNT; chan++) {
			chi = (chip < 10 ? "0" : "") + to_string(chip);
			cha = (chan < 10 ? "0" : "") + to_string(chan);
			columns.push_back("SpecTcl_psd1_" + parname + "_" + chi + "." + cha);
		}
	}
	return columns;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

vector<string> Input::GenerateColumnNamesQDC(const string& parname) {
	vector<string> columns;
	string cha;
	for (size_t chan = 0; chan < QDC_CHAN_COUNT; chan++) {
		cha = (chan < 10 ? "0" : "") + to_string(chan);
		columns.push_back("SpecTcl_qdc1_" + cha + "." + parname);
	}
	return columns;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

vector<string> Input::GenerateColumnNamesTDC() {
	vector<string> columns;
	string cha;
	for (size_t chan = 0; chan < TDC_CHAN_COUNT; chan++) {
		for (int hit = 0; hit < TDC_HIT_COUNT; hit++) {
			cha = (chan < 10 ? "0" : "") + to_string(chan);
			columns.push_back("SpecTcl_tdc1_" + cha + "." + to_string(hit));
		}
	}
	return columns;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/******** NON-STATIC FUNCTIONS ********/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Input::Input(TTreeReader& r) : reader(r) {

	// Pre-alocate required vector memory
	gobbi.eRVs.reserve(HINP_NCOLUMNS);
	gobbi.eLoRVs.reserve(HINP_NCOLUMNS);
	gobbi.tRVs.reserve(HINP_NCOLUMNS);
	texneut.aRVs.reserve(PSD_NCOLUMNS);
	texneut.bRVs.reserve(PSD_NCOLUMNS);
	texneut.cRVs.reserve(PSD_NCOLUMNS);
	texneut.tRVs.reserve(PSD_NCOLUMNS);
	qdc.qhRVs.reserve(QDC_CHAN_COUNT);
	qdc.qlRVs.reserve(QDC_CHAN_COUNT);
	tdc.tRVs.reserve(TDC_NCOLUMNS);

	// Generate column names for reading from input tree
	vector<string> e_columns     = GenerateColumnNamesHINP("e");
	vector<string> eLo_columns   = GenerateColumnNamesHINP("eLo");
	vector<string> hinpt_columns = GenerateColumnNamesHINP("t");
	vector<string> a_columns     = GenerateColumnNamesPSD("a");
	vector<string> b_columns     = GenerateColumnNamesPSD("b");
	vector<string> c_columns     = GenerateColumnNamesPSD("c");
	vector<string> psdt_columns  = GenerateColumnNamesPSD("t");
	vector<string> qh_columns    = GenerateColumnNamesQDC("h");
	vector<string> ql_columns    = GenerateColumnNamesQDC("l");
	vector<string> tdct_columns  = GenerateColumnNamesTDC();
	
	//// Create reader values for all columns, iteratively

	// HINP
	for (size_t i = 0; i < HINP_NCOLUMNS; i++) {
		gobbi.eRVs.push_back({reader, e_columns[i].c_str()});
		gobbi.eLoRVs.push_back({reader, eLo_columns[i].c_str()});
		gobbi.tRVs.push_back({reader, hinpt_columns[i].c_str()});
	}

	// PSD
	for (size_t i = 0; i < PSD_NCOLUMNS; i++) {
		texneut.aRVs.push_back({reader, a_columns[i].c_str()});
		texneut.bRVs.push_back({reader, b_columns[i].c_str()});
		texneut.cRVs.push_back({reader, c_columns[i].c_str()});
		texneut.tRVs.push_back({reader, psdt_columns[i].c_str()});
	}

	// QDC
	for (size_t i = 0; i < QDC_CHAN_COUNT; i++) {
		qdc.qhRVs.push_back({reader, qh_columns[i].c_str()});
		qdc.qlRVs.push_back({reader, ql_columns[i].c_str()});
	}

	// TDC
	for (size_t i = 0; i < TDC_NCOLUMNS; i++) {
		tdc.tRVs.push_back({reader, tdct_columns[i].c_str()});
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Input::~Input() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Input::ReadAndRefactor() {
	gobbi.clear();
	texneut.clear();
	qdc.clear();
	tdc.clear();
	
	// Loop through TDC channels to retrieve time hit information
	size_t chan, hit;
	int tdc_t;
	for (size_t i = 0; i < TDC_NCOLUMNS; i++) {
		Double_t testt = *(tdc.tRVs[i]);
		//cout << "test tdc value " << testt << endl;
		//cout << "test tdc value " << (int)testt << endl;
		tdc_t = *(tdc.tRVs[i]); //TODO this returns the max 64-bit value for empty channels, temp cut out > 16384
		//cout << i / (size_t)TDC_HIT_COUNT << " " << tdc_t << endl;

		if (i == 0 && tdc_t != 0) {
			//cout << "bad event " << badevt << ", skip! tdc_t is " << tdc_t << endl;
			badevt++;
			return;
		}
		if (isnan(tdc_t) || (abs(tdc_t) >= 10000)) continue;
		chan = i / (size_t)TDC_HIT_COUNT;
		//cout << chan << " " << t << endl;
		tdc.Nhits[chan]++;
		tdc.t[chan].push_back(tdc_t);
	}
	
	// Loop through HINP boards and channels and retrieve hit information
	size_t e;
	for (size_t i = 0; i < HINP_NCOLUMNS; i++) {
		e = *(gobbi.eRVs[i]); //TODO this returns the max 64-bit value for empty channels, temp cut out > 16384
		if (isnan(e) || (e == 0) || (e >= 16384)) continue;
		gobbi.Nhits++;
		gobbi.board.push_back((i / (size_t)HINP_CHAN_COUNT) + 1);
		gobbi.chan.push_back(i % (size_t)HINP_CHAN_COUNT);
		gobbi.e.push_back(e);
		gobbi.eLo.push_back((size_t)(*(gobbi.eLoRVs[i])));
		gobbi.t.push_back((size_t)(*(gobbi.tRVs[i])));
	}
	
	// Loop through PSD chips and channels and retrieve hit information
	size_t t;
	for (size_t i = 0; i < PSD_NCOLUMNS; i++) {
		t = *(texneut.tRVs[i]); //TODO this returns the max 64-bit value for empty channels, temp cut out > 16384
		if (isnan(t) || (t == 0) || (t >= 16384)) continue;
		texneut.Nhits++;
		texneut.chip.push_back((i / (size_t)PSD_CHAN_COUNT) + 1);
		texneut.chan.push_back(i % (size_t)PSD_CHAN_COUNT);
		texneut.a.push_back((size_t)(*(texneut.aRVs[i])));
		texneut.b.push_back((size_t)(*(texneut.bRVs[i])));
		texneut.c.push_back((size_t)(*(texneut.cRVs[i])));
		texneut.t.push_back(t);
	  }
	
	// Loop through QDC channels to retrieve high and low range hit information
	size_t qh;
	for (size_t i = 0; i < QDC_CHAN_COUNT; i++) {
		qh = *(qdc.qhRVs[i]); //TODO this returns the max 64-bit value for empty channels, temp cut out > 16384
		if (isnan(qh) || (qh == 0) || (qh >= 16384)) continue;
		qdc.Nhits++;
		qdc.chan.push_back(i);
		qdc.qh.push_back(qh);
		qdc.ql.push_back((size_t)(*(qdc.qlRVs[i])));
	}
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



