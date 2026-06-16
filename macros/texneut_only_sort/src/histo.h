#ifndef histo_
#define histo_

#include <vector>

#include <ROOT/TBufferMerger.hxx>
#include <TTree.h>

#include <eventclass.hpp>

#include "OutStructs.h"

class histo {

private:

	std::shared_ptr<ROOT::TBufferMergerFile> file_read; // thread-safe output ROOT file pointer

	event& texneut; // hold TexNeut event object reference for easy variable retrieval and saving

	// Variables for global tree branches
	size_t texneutmult{0};                          // number of successful pairs of hits per event in TexNeut, a.k.a. "bars"
	std::vector<OutStructs::TexNeutHit> texneutout; // hit list from TexNeut data containing bar-wise information, should be "texneutmult" in length

public:

	histo(std::shared_ptr<ROOT::TBufferMergerFile>, event& texneutevent);
	~histo();

	// Global tree for storing pre-solution variables
	TTree* tpar;

	void Fill();

};

#endif
