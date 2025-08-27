/* Written by Henry Webb (h.s.webb@wustl.edu), August 2025
 * This class handles reading information from a SpecTcl-
 * generated ROOT file and reformatting it into a more
 * easily usable form. The resulting variables are stored
 * in this class on a per-event basis and can be accessed
 * via getter functions.
 */

#include "Input.h"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/******** STATIC FUNCTIONS ********/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

vector<string> Input::GenerateColumnNames(const string& parname) {
	vector<string> columns;
	string b, c;
	for (int board = 1; board <= BOARD_COUNT; board++) {
		for (int chan = 0; chan < CHAN_COUNT; chan++) {
			b = (board < 10 ? "0" : "") + to_string(board);
			c = (chan < 10 ? "0" : "") + to_string(chan);
			columns.push_back("SpecTcl_hinp1_mb1_" + parname + "_" + b + "." + c);
		}
	}
	return columns;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/******** NON-STATIC FUNCTIONS ********/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Input::Input(TTreeReader& r) : reader(r) {

	// Generate column names for reading from input tree
	vector<string> e_columns   = GenerateColumnNames("e");
	vector<string> eLo_columns = GenerateColumnNames("eLo");
	vector<string> t_columns   = GenerateColumnNames("t");

	// Create reader values for all columns, iteratively
	for (int i = 0; i < NCOLUMNS; i++) {
		eRVs.push_back({reader, e_columns[i].c_str()});
		eLoRVs.push_back({reader, eLo_columns[i].c_str()});
		tRVs.push_back({reader, t_columns[i].c_str()});
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Input::~Input() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Input::ReadAndRefactor() {
	Nhits = 0;
	hits_board.clear();
	hits_chan.clear();
	hits_e.clear();
	hits_eLo.clear();
	hits_t.clear();

	// Loop through HINP boards and channels and retrieve hit information
	size_t index;
	double e;
	for (size_t board = 0; board < BOARD_COUNT; board++) {
		for (size_t chan = 0; chan < CHAN_COUNT; chan++) {
			index = (board * CHAN_COUNT) + chan; // make sure this matches the order from GenerateColumnNames
			e = *(eRVs[index]);

			if (std::isnan(e) || (e == 0)) continue;
			Nhits++;
			hits_board.push_back(board+1);
			hits_chan.push_back(chan);
			hits_e.push_back((size_t)e);
			hits_eLo.push_back((size_t)(*(eLoRVs[index])));
			hits_t.push_back((size_t)(*(tRVs[index])));
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



