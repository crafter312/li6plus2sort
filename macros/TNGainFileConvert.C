#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void TNGainFileConvert() {
	string ifgains_name = "../Cal/TNGainScalings.dat";
	string ifgains_default_name = "../config/pmtgains_default.txt";

	// First, loop through input files and verify that they have the same number of lines
	{
		ifstream ifgains(ifgains_name);
		size_t count1{0};
		string line;
		if (ifgains.is_open()) while (getline(ifgains, line)) count1++;
		else {
			cout << "File " << ifgains_name << " not opened properly" << endl;
			ifgains.close();
			return;
		}
		ifgains.close();

		ifstream ifgains_default(ifgains_default_name);
		size_t count2{0};
		if (ifgains_default.is_open()) while (getline(ifgains_default, line)) count2++;
		else {
			cout << "File " << ifgains_default_name << " not opened properly" << endl;
			ifgains_default.close();
			return;
		}
		ifgains_default.close();

		if (count1 != count2) {
			cout << "Input files must have equal line count!" << endl;
			return;
		}
	}

	// Open files for real this time
	ifstream ifgains(ifgains_name);
	ifstream ifgains_default(ifgains_default_name);
	ofstream ofgains_matched("../config/pmtgains_matched.txt");

	// Loop through input files and combine into output file
	int bar1, bar2, pmttop, pmtbot;
	double gain, topgain, botgain;
	while (ifgains.good()){
		ifgains >> bar1 >> gain;
		ifgains_default >> bar2 >> pmttop >> topgain >> pmtbot >> botgain;

		if (bar1 != bar2) {
			cout << "ERROR: bar " << bar1 << " and bar " << bar2 << " should be equal, exiting early" << endl;
			ifgains.close();
			ifgains_default.close();
			ofgains_matched.close();
			return;
		}

		// There are three gain cases to consider. By default, the `gain` input from `ifgains`
		// is meant to be applied to the bottom PMT, according to how the gain matching process
		// works in TNGainMatch.C. As such, if the gain is > 1, then the gain should be applied
		// to the bottom PMT. If the gain is > 0 and < 1, then it should be applied to the top
		// PMT. If the gain is -1, then that means that bar is missing or was unable to have
		// a gain value extracted from it.
		if (gain == -1) {
			topgain = 1;
			botgain = 1;
		}
		else if (gain > 1) {
			topgain = 1;
			botgain = gain;
		}
		else if ((gain > 0) && (gain <= 1)) {
			topgain = 1. / gain;
			botgain = 1;
		}
		else {
			cout << "WARNING: invalid gain " << gain << " for bar # " << bar1 << ", leaving gains as 1" << endl;
			topgain = 1;
			botgain = 1;
		}

		ofgains_matched << bar2 << "\t" << pmttop << "\t" << topgain << "\t" << pmtbot << "\t" << botgain << endl;
	}

	ifgains.close();
	ifgains_default.close();
	ofgains_matched.close();
}
