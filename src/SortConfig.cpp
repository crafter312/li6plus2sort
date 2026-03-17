/**
 * Created by Henry Webb (h.s.webb@wustl.edu) 6 March 2026.
 * This implementation file contains the SortConfig class, which loads and
 * stores all configuration information like filepaths and various settings.
 */

#include "SortConfig.h"

#include <exception>
#include <fstream>
#include <iostream>

#include <stuffing.hpp>

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SortConfig::SortConfig(string configFilePath) {
	cout << "Reading sort code config file..." << endl;

	// Open config file, check that it exists	
	ifstream configfile(configFilePath);
	if (configfile.fail()) throw invalid_argument(string(BOLDRED) + string("Config file ") + configFilePath + string(" does not exist or failed to open") + string(RESET));

	// Read config file
	string line;
	while (getline(configfile, line)) {
		if (line.find("tnlibConfig") != string::npos)
			tnlibConfig = line.substr(line.find('=') + 2);
		else if (line.find("runNumbersFile") != string::npos)
			runNumbersFile = line.substr(line.find('=') + 2);
		else if (line.find("itreeName") != string::npos)
			itreeName = line.substr(line.find('=') + 2);
		else if (line.find("ofileName") != string::npos)
			ofileName = line.substr(line.find('=') + 2);
		else if (line.find("otreeName") != string::npos)
			otreeName = line.substr(line.find('=') + 2);
		else if (line.find("lossDir") != string::npos)
			lossDir = line.substr(line.find('=') + 2);
		else if (line.find("targetSuffix") != string::npos)
			targetSuffix = line.substr(line.find('=') + 2);
		else if (line.find("calDir") != string::npos)
			calDir = line.substr(line.find('=') + 2);
		else if (line.find("frontEcalFile") != string::npos)
			frontEcalFile = line.substr(line.find('=') + 2);
		else if (line.find("backEcalFile") != string::npos)
			backEcalFile = line.substr(line.find('=') + 2);
		else if (line.find("deltaEcalFile") != string::npos)
			deltaEcalFile = line.substr(line.find('=') + 2);
		else if (line.find("frontTimecalFile") != string::npos)
			frontTimecalFile = line.substr(line.find('=') + 2);
		else if (line.find("backTimecalFile") != string::npos)
			backTimecalFile = line.substr(line.find('=') + 2);
		else if (line.find("deltaTimecalFile") != string::npos)
			deltaTimecalFile = line.substr(line.find('=') + 2);
	}
	configfile.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



