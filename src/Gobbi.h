#ifndef Gobbi_H
#define Gobbi_H

/* Nicolas Dronchi 2022_04_04
 * Class written to handle all specifics of the Gobbi array
 * such as communicating with HINP, calibrations, checking
 * for charge sharing in neighbor calculating geometry.
 * 
 * Modified by Henry Webb (h.s.webb@wustl.edu), August 2025
 * Replaces use of HINP class for unpacking with Input class
 * for reading values from a SpecTcl-generated ROOT file.
 * This essentially offloads the work of unpacking to SpecTcl,
 * leaving this code to only do the analysis work.
 */

#include "calibrate.h"
#include "correl2.h"
#include "histo.h"
#include "Input.h"
#include "silicon.h"

#include <iostream>
#include <string>

class Gobbi {

public:
	Gobbi(Input&, histo&);
	~Gobbi();

	bool analyze();
	int match();

	float getEnergy(int board, int chan, int Ehigh);

	void corr_4He();
	void corr_5He();
	void corr_6He();
	void corr_5Li();
	void corr_6Li();
	void corr_7Li();
	void corr_6Be();
	void corr_7Be();
	void corr_8Be();
	void corr_9B();

	double Targetdist;
	float TargetThickness;

	double** slopes;
	double** intercepts;

	Input& input;
	histo& Histo;
	calibrate* FrontEcal;
	calibrate* BackEcal;
	calibrate* DeltaEcal;
	calibrate* FrontTimecal;
	calibrate* BackTimecal;
	calibrate* DeltaTimecal;

	silicon* Silicon[4];
	correl2 Correl;

	int counter = 0;
	int counter2 = 0;

	correl2 Correl_saved;
	int passnum = 0;
	solution* swapfrag;
	solution* oldfrag;
};


#endif
