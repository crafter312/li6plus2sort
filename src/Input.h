#ifndef Input_H
#define Input_H

/* Written by Henry Webb (h.s.webb@wustl.edu), August 2025
 * This class handles reading information from a SpecTcl-
 * generated ROOT file and reformatting it into a more
 * easily usable form. The resulting variables are stored
 * in this class on a per-event basis and can be accessed
 * via getter functions.
 */

#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <string>
#include <vector>

//TODO: replace these with CMake/compile-time variables
#define BOARD_COUNT 12
#define CHAN_COUNT 32

// Number of columns per parameter type in the input file
#define NCOLUMNS BOARD_COUNT * CHAN_COUNT

class Input {

public:
	Input(TTreeReader&);
	~Input();

	void ReadAndRefactor();

	size_t GetNhits() const   { return Nhits; }
	size_t GetBoard(size_t i) { return hits_board[i]; }
	size_t GetChan(size_t i)  { return hits_chan[i]; }
	size_t GetE(size_t i)     { return hits_e[i]; }
	size_t GetELo(size_t i)   { return hits_eLo[i]; }
	size_t GetT(size_t i)     { return hits_t[i]; }

private:
	// TTreeReader reference for input
	TTreeReader& reader;

	// Column names for reading from input tree
	std::vector<std::string> e_columns;
	std::vector<std::string> eLo_columns;
	std::vector<std::string> t_columns;

	// Vectors for input branch readers
	std::vector<TTreeReaderValue<double>> eRVs;
	std::vector<TTreeReaderValue<double>> eLoRVs;
	std::vector<TTreeReaderValue<double>> tRVs;

	// Vectors for Gobbi (HINP) hit values
	size_t Nhits{0};
	std::vector<size_t> hits_board;
	std::vector<size_t> hits_chan;
	std::vector<size_t> hits_e;
	std::vector<size_t> hits_eLo;
	std::vector<size_t> hits_t;

	/******** PRIVATE STATIC HELPER FUNCTIONS ********/

	static std::vector<std::string> GenerateColumnNames(const std::string&);

};

#endif
