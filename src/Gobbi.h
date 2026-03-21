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
 * 
 * Modified by Henry Webb (h.s.webb@wustl.edu) and Johnathan
 * Phillips (j.s.phillips@wustl.edu) March 2026 for experiment
 * at TAMU Cyclotron Institute
 */

#include "calibrate.h"
#include "correl2.h"
#include "histo.h"
#include "Input.h"
#include "silicon.h"
#include "solution.h"
#include "SortConfig.h"

#include <eventclass.hpp> // TNLIB TexNeut event class

#include <iostream>
#include <string>

class Gobbi {

public:
	Gobbi(Input& in, histo& hist, SortConfig& config, int run, event& neut);
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

	histo& Histo;
	event& texneut;
	calibrate* FrontEcal;
	calibrate* BackEcal;
	calibrate* DeltaEcal;
	calibrate* FrontTimecal;
	calibrate* BackTimecal;
	calibrate* DeltaTimecal;

	calibrate* DiamondEcal;

	silicon* Silicon[4];
	solution neutSol;
	correl2 Correl;

	int counter = 0;
	int counter2 = 0;

	correl2 Correl_saved;
	int passnum = 0;
	solution* swapfrag;
	solution* oldfrag;
	
	std::vector<float> diamond_Ecal;
	
	int runnum;
	int diamond_calch = -1;
	
	//TexNeut TDC gates
	float TN_TDClow = -147;
	float TN_TDChigh = -60;
	
	//Neutron multiplicity
	int num_neut;
	
	//also record highest multiplicity
	int num_neut_highest = 0;
	
	//Record particle combinations, start with most important
	int a_p_0n = 0;
	int a_p_withn = 0; //Just needs to be mult > 0
	int a_p_1n = 0;
	int a_p_2n = 0;
	int a_p_3n = 0;

private:
	const Input::GobbiInput& input;
	const Input::QDCInput& input_qdc;
	const Input::TDCInput& input_tdc;

  // This function is meant to take the neutron kinematic information
  // from TexNeut, put it in a solution, and transfer said solution
  // to the correl class for further analysis.
  void TransferNeutSols();

};


#endif
