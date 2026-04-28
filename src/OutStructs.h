#ifndef OutStructs_H
#define OutStructs_H

#include <cmath>

namespace OutStructs {

	// Class for holding TexNeut variables for output, per hit
	struct TexNeutHit {
		int bar;                // bar number (see TNLIB detector.cpp)
		int chip_top, chip_bot; // PSD chip numbers for top and bottom PMTs
		int chan_top, chan_bot; // PSD channel numbers for top and bottom PMTs
		int Aint_top, Aint_bot, Bint_top, Bint_bot, Cint_top, Cint_bot, Tint_top, Tint_bot; // A, B, C, and T integrals for top and bottom PMTs
		int TDCchannel_top, TDCchannel_bot; // TDC channels for top and bottom PMTs
		double TDCvalue_top, TDCvalue_bot;  // TDC times for top and bottom PMTs
		double PSD_top, PSD_bot, PSD;       // top, bottom, and top + bottom PMT PSD parameter
		double E_top, E_bot, E_tot;         // A integral + B integral for top, bottom, and top + bottom
		double xi, yi, zi;                  // p-Terphenyl crystal indices of hit
		double x, y, z;                     // Cartesian coordinates of neutron hit relative to target center
		double rho, theta, phi;             // spherical coordinates of neutron hit relative to target center
		double c_r, c_th;                   // radius and polar angle in top:bot integral space for per-crystal gating (finger plots)

		void clear() {
			bar = -1;
			chip_top = -1;
			chip_bot = -1;
			chan_top = -1;
			chan_bot = -1;
			Aint_top = -1;
			Aint_bot = -1;
			Bint_top = -1;
			Bint_bot = -1;
			Cint_top = -1;
			Cint_bot = -1;
			Tint_top = -1;
			Tint_bot = -1;
			TDCchannel_top = -1;
			TDCchannel_bot = -1;
			TDCvalue_top = -1;
			TDCvalue_bot = -1;
			PSD_top = NAN;
			PSD_bot = NAN;
			PSD = NAN;
			E_top = NAN;
			E_bot = NAN;
			E_tot = NAN;
			xi = NAN;
			yi = NAN;
			zi = NAN;
		  x = NAN;
			y = NAN;
			z = NAN;
		  rho = NAN;
			theta = NAN;
			phi = NAN;
			c_r = NAN;
			c_th = NAN;
		}
	};

}

#endif
