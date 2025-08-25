#ifndef _Input
#define _Input

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
	vector<size_t> hits_board;
	vector<size_t> hits_chan;
	vector<size_t> hits_e;
	vector<size_t> hits_eLo;
	vector<size_t> hits_t;

	/******** PRIVATE STATIC HELPER FUNCTIONS ********/

	static std::vector<std::string> Input::GenerateColumnNames(const string&);

}

#endif
