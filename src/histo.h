#ifndef histo_
#define histo_

#include <ROOT/TBufferMergerFile.hxx>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TH2I.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class histo {

protected:
	std::shared_ptr<TBufferMergerFile> file_read; // thread-safe output ROOT file pointer

public:
	histo(std::shared_ptr<TBufferMergerFile>);
	~histo();

	//TODO: replace with CMake/compile-time variables
	static const int E_boardnum = 8;  // number of boards associated with E silicon detectors
	static const int dE_boardnum = 4; // number of boards associated with dE silicon detectors
	static const int boardnum = 12;   // total number of boards (not used; was 16, should be 12)
	static const int channum = 32;    // number of channels on each board

	TDirectoryFile* dirSummary;

	TDirectory* dir1dFrontE_R;
	TDirectory* dir1dBackE_R;
	TDirectory* dir1dDeltaE_R;
	TDirectory* dir1dFrontlowE_R;
	TDirectory* dir1dBacklowE_R;
	TDirectory* dir1dDeltalowE_R;
	TDirectory* dir1dFrontE_cal;
	TDirectory* dir1dBackE_cal;
	TDirectory* dir1dDeltaE_cal;
	TDirectory* dirAngleCorrFrontE;
	TDirectory* dirAngleCorrDeltaE;
	TDirectory* dir1dFrontTime_R;
	TDirectory* dir1dBackTime_R;
	TDirectory* dir1dDeltaTime_R;

	TDirectoryFile* dirDEEplots; // directory for deltaE-E plots used in particle identificaiton
	TDirectoryFile* dirhitmaps;  // directory for all particle type hitmaps

	TDirectoryFile* dirInvMass;  // directory for all correlations and inv-mass
	TDirectory* dir4He;
	TDirectory* dir5He;
	TDirectory* dir6He;
	TDirectory* dir5Li;
	TDirectory* dir6Li;
	TDirectory* dir7Li;
	TDirectory* dir6Be;
	TDirectory* dir7Be;
	TDirectory* dir8Be;
	TDirectory* dir9B;

	// Summary plots
	TH2I* sumFrontE_R;
	TH2I* sumBackE_R;
	TH2I* sumDeltaE_R;
	TH2I* sumFrontE_cal;
	TH2I* sumFrontE_addback;
	TH2I* sumBackE_cal;
	TH2I* sumBackE_addback;
	TH2I* sumDeltaE_cal;
	TH2I* sumDeltaE_addback;
	TH2I* sumEtot_cal;
	TH2I* AngleCorrSum_cal;
	TH2I* AngleCorrFrontE_cal;
	TH2I* AngleCorrDeltaE_cal;
	TH2I* sumFrontTime_R;
	TH2I* sumFrontTime_cal;
	TH2I* sumBackTime_R;
	TH2I* sumBackTime_cal;
	TH2I* sumDeltaTime_R;
	TH2I* sumDeltaTime_cal;
	TH2I* FrontvsBack;

	TH2I* sumFrontTimeMult1_cal;

	// E silicon plots
	TH1I* FrontE_R[4][channum];
	TH1I* FrontElow_R[4][channum];
	TH1I* FrontTime_R[4][channum];
	TH1I* FrontE_cal[4][channum];
	TH1I* BackE_R[4][channum];
	TH1I* BackElow_R[4][channum];
	TH1I* BackTime_R[4][channum];
	TH1I* BackE_cal[4][channum];

	TH1I* AngleCorrE[4][channum];
	TH1I* AngleCorr_noCorr[4][channum];
	TH1I* AngleCorrE_R[4][channum];

	// Delta E silicon plots
	TH1I* DeltaE_R[4][channum];
	TH1I* DeltaElow_R[4][channum];
	TH1I* DeltaTime_R[4][channum];
	TH1I* DeltaE_cal[4][channum];

	TH1I* AngleCorrDeltaE[4][channum];
	TH1I* AngleCorrDeltaE_noCorr[4][channum];
	TH1I* AngleCorrDeltaE_R[4][channum];

	// DeltaE-E plots
	TH2I* DEE_simple[4];
	TH2I* frontdeltastripnum[4];
	TH2I* DEE[4];
	TH1I* timediff[4];

	TH2I* xyhitmap_allE;
	TH2I* xyhitmap;
	TH2I* protonhitmap;
	TH2I* deuteronhitmap;
	TH2I* tritonhitmap;
	TH2I* alphahitmap;
	TH2I* He6hitmap;
	TH2I* Lihitmap;
	TH2I* hitmapof_p;
	TH2I* hitmapof_6He;

	TH2I* Evstheta[4];
	TH2I* Evstheta_all;
	TH1I* Theta;

	TH2I* ProtonEnergy;

	TH1I* dTime_proton;
	TH1I* dTime_deuteron;
	TH1I* dTime_triton;
	TH1I* dTime_alpha;
	TH1I* dTime_He6;
	TH1I* dTime_Li;
	TH2I* CorrelationTable;

	/******** CORRELATIONS AND INVARIANT MASS PLOTS ********/

	// He4
	TH1I* Erel_4He_pt;
	TH1I* Ex_4He_pt;
	TH1I* ThetaCM_4He_pt;
	TH1I* VCM_4He_pt;
	TH2I* He4_p_hitmap;
	TH2I* He4_t_hitmap;
	TH2I* DEE_He4[4];
	TH2I* Erel_pt_costhetaH;

	TH1I* Erel_4He_dd;
	TH1I* Ex_4He_dd;
	TH1I* ThetaCM_4He_dd;
	TH1I* VCM_4He_dd;
	TH2I* Erel_dd_costhetaH;

	// He5
	TH1I* Erel_5He_dt;
	TH1I* Ex_5He_dt;
	TH1I* ThetaCM_5He_dt;
	TH1I* VCM_5He_dt;

	// He6
	TH1I* Erel_6He_tt;
	TH1I* Ex_6He_tt;
	TH1I* ThetaCM_6He_tt;
	TH1I* VCM_6He_tt;

	// Li5
	TH1I* Erel_5Li_pa;
	TH1I* Ex_5Li_pa;
	TH1I* ThetaCM_5Li_pa;
	TH1I* VCM_5Li_pa;

	TH1I* Erel_5Li_d3He;
	TH1I* Ex_5Li_d3He;
	TH1I* ThetaCM_5Li_d3He;
	TH1I* VCM_5Li_d3He;

	// Li6
	TH1I* Erel_6Li_da;
	TH1I* Ex_6Li_da_trans;
	TH1I* Ex_6Li_da_long;
	TH1I* Ex_6Li_da;
	TH1I* cos_thetaH_da;
	TH1I* ThetaCM_6Li_da;
	TH1I* VCM_6Li_da;
	TH2I* VCM_vs_ThetaCM;
	TH1I* cos_da_thetaH;
	TH2I* Erel_da_cosThetaH;
	TH1I* deutE_gate;
	TH1I* alphaE_gated;
	TH2I* deutE_gate_cosThetaH;
	TH2I* alphaE_gate_cosThetaH;
	TH1I* react_origin_tdiff;

	// Li7
	TH1I* Erel_7Li_p6He;
	TH2I* Erel_7Li_p6He_Q;
	TH2I* Erel_7Li_cosThetaH;
	TH1I* Erel_7Li_p6He_lowres;
	TH1I* Ex_7Li_p6He_transverse;
	TH1I* Ex_7Li_p6He_transverse2;
	TH1I* Erel_7Li_p6He_pFor;
	TH1I* Ex_7Li_p6He_timegate;
	TH1I* cos_thetaH;
	TH1I* cos_thetaH_lowErel;
	TH1I* missingmass;
	TH2I* Erel_missingmass;
	TH1I* Qvalue;
	TH1I* Qvalue2;
	TH1I* Ex_7Li_p6He_clean;
	TH1I* Ex_7Li_p6He;
	TH1I* ThetaCM_7Li_p6He;
	TH1I* VCM_7Li_p6He;
	TH1I* VCM_7Li_p6He_lowErel;
	TH1I* dTime_7Li_proton;
	TH1I* dTime_7Li_He6;
	TH2I* Ex_7Li_p6He_ExvsEp;

	TH1I* Erel_7Li_ta;
	TH1I* Ex_7Li_ta;
	TH1I* Ex_7Li_ta_trans;
	TH1I* Ex_7Li_ta_long;
	TH1I* Ex_7Li_ta_bad;
	TH1I* ThetaCM_7Li_ta;
	TH1I* VCM_7Li_ta;
	TH1I* cos_ta_thetaH;
	TH2I* Erel_ta_cosThetaH;
	TH1I* Ex_tar;
	TH2I* Erel_vs_Extar;
	TH1I* Ex_7Li_ta_timegate;
	TH2I* hitmapcheck1;
	TH2I* hitmapcheck2;
	TH1I* dTime_7Li_triton;
	TH1I* dTime_7Li_alpha;
	TH1I* seperate_quad_Ex_7Li_ta;
	TH2I* DEE_shoulderevents;

	// Be6
	TH1I* Erel_6Be_2pa;
	TH1I* ThetaCM_6Be_2pa;
	TH1I* VCM_6Be_2pa;

	// Be7
	TH1I* Erel_7Be_a3He;
	TH1I* Ex_7Be_a3He;
	TH1I* ThetaCM_7Be_a3He;
	TH1I* VCM_7Be_a3He;

	TH1I* Erel_7Be_p6Li;
	TH1I* Ex_7Be_p6Li;
	TH1I* ThetaCM_7Be_p6Li;
	TH1I* VCM_7Be_p6Li;

	// Be8
	TH1I* Erel_8Be_aa;
	TH1I* Ex_8Be_aa;
	TH1I* ThetaCM_8Be_aa;
	TH1I* VCM_8Be_aa;
	TH2I* Erel_aa_cosThetaH;

	TH1I* Erel_8Be_p7Li;
	TH1I* Ex_8Be_p7Li;
	TH1I* Ex_8Be_p7Li_trans;
	TH1I* ThetaCM_8Be_p7Li;
	TH1I* VCM_8Be_p7Li;
	TH1I* cos_p7Li_thetaH;
	TH2I* Erel_p7Li_cosThetaH;

	TH1I* ProtonEnergies_p7Li;
	TH1I* LithiumEnergies_p7Li;

	TH1I* Ex_8Be_p7Li_timegate;

	TH1I* dTime_8Be_proton;
	TH1I* dTime_8Be_Li7;

	TH1I* Erel_8Be_pta;
	TH1I* Ex_8Be_pta;
	TH1I* Ex_8Be_pta_trans;
	TH1I* ThetaCM_8Be_pta;
	TH1I* VCM_8Be_pta;
	TH1I* cos_pta_thetaH;
	TH2I* Erel_pta_cosThetaH;

	TH1I* Erel_7Li_ta_fake;
	TH1I* Ex_7Li_ta_fake;

	TH1I* Ex_8Be_7LiGate;

	// B9
	TH1I* Erel_9B_paa;
	TH1I* Ex_9B_paa;
	TH1I* Ex_9B_p8Be;
	TH1I* Ex_9B_aa;
	TH1I* ThetaCM_9B_paa;
	TH1I* VCM_9B_paa;

};

#endif
