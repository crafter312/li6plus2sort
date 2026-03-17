/**
 * Created by Henry Webb (h.s.webb@wustl.edu) 6 March 2026.
 * This header file contains the SortConfig class, which loads and stores
 * all configuration information like filepaths and various settings.
 */

#ifndef SortConfig_H
#define SortConfig_H

#include <string>

class SortConfig {
private:
	std::string tnlibConfig;
	std::string runNumbersFile;
	std::string itreeName;
	std::string ofileName;
	std::string otreeName;
	std::string lossDir;
	std::string targetSuffix;
	std::string calDir;
	std::string frontEcalFile;
	std::string backEcalFile;
	std::string deltaEcalFile;
	std::string frontTimecalFile;
	std::string backTimecalFile;
	std::string deltaTimecalFile;

public:
	SortConfig(std::string configFilePath);

	// Getters
	std::string GetTnlibConfig() { return tnlibConfig; }
	std::string GetRunNumbersFile() { return runNumbersFile; }
	std::string GetItreeName() { return itreeName; }
	std::string GetOfileName() { return ofileName; }
	std::string GetOtreeName() { return otreeName; }
	std::string GetLossDir() { return lossDir; }
	std::string GetTargetSuffix() { return targetSuffix; }
	std::string GetCalDir() { return calDir; }
	std::string GetFrontEcalFile() { return frontEcalFile; }
	std::string GetBackEcalFile() { return backEcalFile; }
	std::string GetDeltaEcalFile() { return deltaEcalFile; }
	std::string GetFrontTimecalFile() { return frontTimecalFile; }
	std::string GetBackTimecalFile() { return backTimecalFile; }
	std::string GetDeltaTimecalFile() { return deltaTimecalFile; }

};

#endif
