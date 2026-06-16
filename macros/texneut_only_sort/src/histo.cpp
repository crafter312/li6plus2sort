#include "histo.h"

#include <cmath>

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

histo::histo(shared_ptr<ROOT::TBufferMergerFile> f, event& texneutevent) : texneut(texneutevent) {
	file_read = f;
	file_read->cd();

	// Create global tree for storing pre-solution variables
	tpar = new TTree("tpar", "tpar");
	tpar->Branch("texneutmult", &texneutmult);
	tpar->Branch("texneut", &texneutout);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

histo::~histo() {
  file_read->Write();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void histo::Fill() {

	// Transfer TexNeut values to output class
	texneutout.clear();
	texneutmult = texneut.get_coupledhits();
	for (size_t i = 0; i < texneutmult; i++) {
		OutStructs::TexNeutHit texneuthit;
		texneuthit.bar = texneut.get_barshit(i);
		texneuthit.chip_top = texneut.get_chip(i, "top");
		texneuthit.chip_bot = texneut.get_chip(i, "bot");
		texneuthit.chan_top = texneut.get_chan(i, "top");
		texneuthit.chan_bot = texneut.get_chan(i, "bot");
		texneuthit.Aint_top = texneut.get_Aint(i, "top");
		texneuthit.Aint_bot = texneut.get_Aint(i, "bot");
		texneuthit.Bint_top = texneut.get_Bint(i, "top");
		texneuthit.Bint_bot = texneut.get_Bint(i, "bot");
		texneuthit.Cint_top = texneut.get_Cint(i, "top");
		texneuthit.Cint_bot = texneut.get_Cint(i, "bot");
		texneuthit.Tint_top = texneut.get_Tint(i, "top");
		texneuthit.Tint_bot = texneut.get_Tint(i, "bot");
		texneuthit.TDCchannel_top = texneut.get_TDCchannel(i, "top");
		texneuthit.TDCchannel_bot = texneut.get_TDCchannel(i, "bot");
		texneuthit.TDCvalue_top = texneut.get_TDCvalue(i, "top");
		texneuthit.TDCvalue_bot = texneut.get_TDCvalue(i, "bot");
		texneuthit.PSD_top = texneut.get_PSD(i, "top");
		texneuthit.PSD_bot = texneut.get_PSD(i, "bot");
		texneuthit.PSD = texneut.get_PSD(i, "pre");
		texneuthit.E_top = texneut.get_E(i, "top");
		texneuthit.E_bot = texneut.get_E(i, "bot");
		texneuthit.E_tot = texneut.get_E(i, "pre");

		// NOTE: Here, reorder the Cartesian axes from TexAT coordinates (Z up) to standard beam physics coordinates (Z beam axis)
		// See TNLIB detector.cpp for more details, this also swaps from a right handed to a left handed coordinate system
		// I also recalculate the spherical coordinates because I think Alex does it wrong
		texneuthit.xi = texneut.get_hitcoord(i, 0);
		texneuthit.yi = texneut.get_hitcoord(i, 2);
		texneuthit.zi = texneut.get_hitcoord(i, 1);
    texneuthit.x = texneut.get_flight_cart(i, 0);
		texneuthit.y = texneut.get_flight_cart(i, 2);
		texneuthit.z = texneut.get_flight_cart(i, 1);
    texneuthit.rho = texneut.get_flight_sphere(i, 0);
		texneuthit.theta = acos(texneuthit.z / texneuthit.rho);
		texneuthit.phi = (texneuthit.y < 0 ? -1. : 1.) * acos(texneuthit.x / sqrt((texneuthit.x*texneuthit.x) + (texneuthit.y*texneuthit.y)));

		// Calculate custom parameters for ease of per-crystal gating
		if (!isnan(texneuthit.TDCvalue_top) && !isnan(texneuthit.TDCvalue_bot)) {
			texneuthit.c_r = sqrt((texneuthit.TDCvalue_top*texneuthit.TDCvalue_top) + (texneuthit.TDCvalue_bot*texneuthit.TDCvalue_bot));
			texneuthit.c_th = atan(texneuthit.TDCvalue_top / texneuthit.TDCvalue_bot);
		}

		texneutout.push_back(texneuthit);
	}

	// Fill global pre-solution tree
	tpar->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



