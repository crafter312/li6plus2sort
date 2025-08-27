#include <ROOT/RDataFrame.hxx>
#include <TFile.h>
#include <TTree.h>

#include <vector>
#include <tuple>
#include <cmath> // For std::isnan

#define BOARD_COUNT 12
#define CHAN_COUNT 32
#define NCOLUMNS BOARD_COUNT * CHAN_COUNT
#define NCOLUMNS2 NCOLUMNS * 2
#define NCOLUMNS3 NCOLUMNS * 3

/* THIS FILE DOESN'T WORK BECAUSE THE DEFINE FUNCTION
 * DOES NOT ACCEPT TEMPLATE FUNCTIONS AS AN ARGUMENT
 */

// Helper function for hit creation
std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>>
create_hits_helper(const std::vector<size_t>& e_values, const std::vector<size_t>& eLo_values, const std::vector<size_t>& t_values) {
	std::vector<size_t> hits_board;
	std::vector<size_t> hits_chan;
	std::vector<size_t> hits_e;
	std::vector<size_t> hits_eLo;
	std::vector<size_t> hits_t;

	size_t index, e;
	for (size_t board = 0; board < BOARD_COUNT; board++) {
		for (size_t chan = 0; chan < CHAN_COUNT; chan++) {
			index = (board * CHAN_COUNT) + chan; // make sure this matches the order from generate_column_names
			e = e_values[index];

			if (std::isnan(e) || (e == 0)) continue;
			hits_board.push_back(board);
			hits_chan.push_back(chan);
			hits_e.push_back(e);
			hits_eLo.push_back(eLo_values[index]);
			hits_t.push_back(t_values[index]);
		}
	}

	return std::make_tuple(hits_board, hits_chan, hits_e, hits_eLo, hits_t);
}

// Define hit generation lambda, assuming that the
// parameters are accepted in the same order as the
// column names are combined above, and that all the
// board/channel loops are in the same order
template<typename... Args>
std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>>
create_hits(Args... columns) {
	std::vector<size_t> e_values;
	std::vector<size_t> eLo_values;
	std::vector<size_t> t_values;

	// Loop through packed variadic argument list, casting
	// everything to an unsigned integer (because that's what)
	// it originally was pre-SpecTcl
	size_t i = 0;
	for (double& col : {columns...}) {
		if (i < NCOLUMNS) { e_values.push_back((size_t)col); }
		else if (i < NCOLUMNS2) { eLo_values.push_back((size_t)col); }
		else if (i < NCOLUMNS3) { t_values.push_back((size_t)col); }
		i++;
	}

	return create_hits_helper(e_values, eLo_values, t_values);
};

std::vector<std::string> generate_column_names(const std::string& parname) {
	std::vector<std::string> columns;
	std::string b, c;
	for (int board = 1; board <= BOARD_COUNT; board++) {
		for (int chan = 0; chan < CHAN_COUNT; chan++) {
			b = (board < 10 ? "0" : "") + std::to_string(board);
			c = (chan < 10 ? "0" : "") + std::to_string(chan);
			columns.push_back("SpecTcl_hinp1_mb1_" + parname + "_" + b + "." + c);
		}
	}
	return columns;
}

void refactor() {
	ROOT::EnableImplicitMT(4); // Use multiple threads

	// Open the ROOT file and check for success
	std::string ifname = "run-420";
	TFile *file = TFile::Open((ifname + ".root").c_str());
	if (!file || file->IsZombie()) {
		std::cerr << "Error opening file!" << std::endl;
		return;
	}

	// Check if tree exists in the file
	TTree *tree = (TTree*)file->Get("t");
	if (!tree) {
		std::cerr << "Tree 't' not found in the file!" << std::endl;
		file->Close();
		return;
	}

	// Create a DataFrame
	ROOT::RDataFrame df(*tree);
	ROOT::RDF::RNode node(df) ;
	ROOT::RDF::Experimental::AddProgressBar(node);

	// Generate column names
	std::vector<std::string> e_columns   = generate_column_names("e");
	std::vector<std::string> eLo_columns = generate_column_names("eLo");
	std::vector<std::string> t_columns   = generate_column_names("t");
	size_t n_e_cols   = e_columns.size();
	size_t n_eLo_cols = eLo_columns.size();
	size_t n_t_cols   = t_columns.size();
	size_t e_eLo_size = n_e_cols + n_eLo_cols;
	size_t total_size = e_eLo_size + n_t_cols;

	// Combine all columns names into a single vector
	std::vector<std::string> all_columns;
	all_columns.insert(all_columns.end(), e_columns.begin(), e_columns.end());
	all_columns.insert(all_columns.end(), eLo_columns.begin(), eLo_columns.end());
	all_columns.insert(all_columns.end(), t_columns.begin(), t_columns.end());

	// Define hits column
	auto hits_columns = df.Define("hits", create_hits, all_columns);

	// Extract hits data into separate columns
	auto df2 = hits_columns
		.Define("board", "std::get<0>(hits)")
		.Define("chan", "std::get<1>(hits)")
		.Define("e", "std::get<2>(hits)")
		.Define("eLo", "std::get<3>(hits)")
		.Define("t", "std::get<4>(hits)");

	df2.Display({"board", "chan", "e", "eLo", "t"})->Print();
}
