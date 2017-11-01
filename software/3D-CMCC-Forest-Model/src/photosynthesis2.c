/*
 * photosynthesis2.c
 *
 *  Created on: 11 set 2017
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "photosynthesis.h"
#include "canopy_evapotranspiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;


void total_photosynthesis_biome (cell_t *const c, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual)
{

	double cond_corr;        /* (umol/m2/s/Pa) leaf conductance corrected for CO2 vs. water vapor */
	double leafN;            /* (kg Nleaf/m2) leaf N per unit leaf area */
	double ppfd;             /* (umol/m2 covered/sec) photosynthetic Photon Flux Density  */
	double leaf_day_mresp;   /* (umol/m2/s) day leaf m. resp, proj. area basis */
	double assimilation;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];


	/* This function is a wrapper and replacement for the photosynthesis code which used to be in the central bgc.c code.  At Mott Jolly's request, all of the science code is being moved into funtions. */

	/* psn_struct psn_sun, psn_shade; */

	/****************************************************************************************/

	/* SUNLIT canopy fraction photosynthesis */

	/* convert conductance from m/s --> umol/m2/s/Pa, and correct for CO2 vs. water vapor */
	cond_corr                    = s->value[LEAF_SUN_CONDUCTANCE] * 1e6 / ( 1.6 * Rgas * ( meteo_daily->tday + TempAbs ) );
	leafN                        = s->value[LEAF_SUN_N];
	/* convert from mass to molar units, and from a daily rate to a rate per second (umol/m2/s) */
	leaf_day_mresp               = s->value[DAILY_LEAF_SUN_MAINT_RESP] / ( meteo_daily->daylength_sec * GC_MOL * 1e-6 );
	ppfd                         = s->value[PPFD_ABS_SUN];

	/* call Farquhar for sun leaves */
	assimilation = Farquhar (c, height, dbh, age, species, meteo_daily, meteo_annual, cond_corr, leafN, ppfd, leaf_day_mresp);

	/* converting umolC/sec --> gC/m2/day) */
	s->value[ASSIMILATION_SUN]   = ( assimilation + leaf_day_mresp ) * s->value[LAI_SUN_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

	/****************************************************************************************/

	/* SHADED canopy fraction photosynthesis */

	/* convert conductance from m/s --> umol/m2/s/Pa, and correct for CO2 vs. water vapor */
	cond_corr                    = s->value[LEAF_SHADE_CONDUCTANCE] * 1e6 / ( 1.6 * Rgas * ( meteo_daily->tday + TempAbs ) );
	leafN                        = s->value[LEAF_SHADE_N];
	/* convert from mass to molar units, and from a daily rate to a rate per second (umol/m2/s) */
	leaf_day_mresp               = s->value[DAILY_LEAF_SHADE_MAINT_RESP] / ( meteo_daily->daylength_sec * GC_MOL * 1e-6 );
	ppfd                         = s->value[PPFD_ABS_SHADE];

	/* call Farquhar for shade leaves */
	assimilation = Farquhar (c, height, dbh, age, species, meteo_daily, meteo_annual, cond_corr, leafN, ppfd, leaf_day_mresp );

	/* converting umolC/sec --> gC/m2/day) */
	s->value[ASSIMILATION_SHADE] = ( assimilation + leaf_day_mresp ) * s->value[LAI_SHADE_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6 ;

//	if (s->value[LAI_PROJ] > 3)
//	{
//		printf("%g sun\n", s->value[PPFD_ABS_SUN]);
//		printf("%g shade\n", s->value[PPFD_ABS_SHADE]);
//		getchar();
//	}

	/****************************************************************************************/

	/* total assimilation */
	s->value[ASSIMILATION] = s->value[ASSIMILATION_SUN] + s->value[ASSIMILATION_SHADE];
	logger(g_debug_log, "ASSIMILATION = %g gC/m^2/day\n", s->value[ASSIMILATION]);

	s->value[MONTHLY_ASSIMILATION]       += s->value[ASSIMILATION];
	s->value[MONTHLY_ASSIMILATION_SUN]   += s->value[ASSIMILATION_SUN];
	s->value[MONTHLY_ASSIMILATION_SHADE] += s->value[ASSIMILATION_SHADE];

	s->value[YEARLY_ASSIMILATION]        += s->value[ASSIMILATION];
	s->value[YEARLY_ASSIMILATION_SUN]    += s->value[ASSIMILATION_SUN];
	s->value[YEARLY_ASSIMILATION_SHADE]  += s->value[ASSIMILATION_SHADE];

}

double Farquhar (cell_t *const c, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily,
		const meteo_annual_t *const meteo_annual, const double cond_corr, const double leafN, const double ppfd, const double leaf_day_mresp )
{
	/*
		The following variables are assumed to be defined in the psn struct
		at the time of the function call:
		pa         (Pa) atmospheric pressure
		co2        (ppm) atmospheric [CO2]
		t          (deg C) air temperature
		lnc        (kg Nleaf/m2) leaf N concentration, per unit projected LAI
		flnr       (kg NRub/kg Nleaf) fraction of leaf N in Rubisco
		ppfd       (umol photons/m2/s) PAR flux density, per unit projected LAI
		g          (umol CO2/m2/s/Pa) leaf-scale conductance to CO2, proj area basis
		dlmr       (umol CO2/m2/s) day leaf maint resp, on projected leaf area basis

		The following variables in psn struct are defined upon function return:
		Ci         (Pa) intercellular [CO2]
		Ca         (Pa) atmospheric [CO2]
		O2         (Pa) atmospheric [O2]
		gamma      (Pa) CO2 compensation point, in the absence of maint resp.
		Kc         (Pa) MM constant for carboxylation
		Ko         (Pa) MM constant for oxygenation
		Vmax       (umol CO2/m2/s) max rate of carboxylation
		Jmax       (umol electrons/m2/s) max rate electron transport
		J          (umol RuBP/m2/s) rate of RuBP regeneration
		Av         (umol CO2/m2/s) carboxylation limited assimilation
		Aj         (umol CO2/m2/s) RuBP regeneration limited assimilation
		A          (umol CO2/m2/s) final assimilation rate
	 */

	/* the weight proportion of Rubisco to its nitrogen content, fnr, is calculated from the relative proportions of the basic amino acids
		that make up the enzyme, as listed in the Handbook of Biochemistry, Proteins, Vol III, p. 510, which references: Kuehn and McFadden, Biochemistry, 8:2403, 1969 */
	static double fnr = 7.16;      /* kg Rub/kg NRub */

	/* the following enzyme kinetic constants are from:
		Woodrow, I.E., and J.A. Berry, 1980. Enzymatic regulation of photosynthetic
		CO2 fixation in C3 plants. Ann. Rev. Plant Physiol. Plant Mol. Biol.,
		39:533-594.
		Note that these values are given in the units used in the paper, and that
		they are converted to units appropriate to the rest of this function before
		they are used. */
	/* I've changed the values for Kc and Ko from the Woodrow and Berry
		reference, and am now using the values from De Pury and Farquhar,
		1997. Simple scaling of photosynthesis from leaves to canopies
		without the errors of big-leaf models. Plant, Cell and Env. 20: 537-557.
		All other parameters, including the q10's for Kc and Ko are the same
		as in Woodrow and Berry. */

	static double Kc25   = 404;    /* (ubar) MM const carboxylase, 25 deg C */
	static double q10Kc  = 2.1;    /* (DIM) Q_10 for Kc */
	static double Ko25   = 248.0;  /* (mbar) MM const oxygenase, 25 deg C */
	static double q10Ko  = 1.2;    /* (DIM) Q_10 for Ko */
	static double act25  = 3.6;    /* (umol/mgRubisco/min) Rubisco activity */
	static double q10act = 2.4;    /* (DIM) Q_10 for Rubisco activity */
	static double pabs   = 0.85;   /* (DIM) fPAR effectively absorbed by PSII */

	/* local variables */
	double Kc;                     /* (Pa) MM constant for carboxylase reaction */
	double Ko;                     /* (Pa) MM constant for oxygenase reaction */
	double act;                    /* (umol CO2/kgRubisco/s) Rubisco activity */
	double Jmax;                   /* (umol/m2/s) max rate electron transport */
	double ppe;                    /* (mol/mol) photons absorbed by PSII per e- transported */
	double Vmax;                   /* (umol/m2/s) max rate carboxylation */
	double J;                      /* (umol/m2/s) rate of RuBP regeneration */
	double gamma;                  /* (Pa) CO2 compensation point, no Rd */
	double Ca;                     /* (Pa) atmospheric [CO2] pressure */
	double O2;                     /* (Pa) atmospheric [O2] */
	double Av;                     /* (umol/m2/s) carboxylation limited assimilation */
	double Aj;                     /* (umol/m2/s) RuBP regeneration limited assimilation */
	double A;                      /* (umol/m2/s) final assimilation rate */
	double Ci;                     /* (Pa) intercellular [CO2] */
	double Rd;                     /* (umol/m2/s) day leaf m. resp, proj. area basis */
	double var_a, var_b, var_c, det;


	//todo move in species.txt (this should be the only variable for all photosynthesis)
	double flnr = 0.1; /* (kg NRub/kg Nleaf) fract. of leaf N in Rubisco */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* begin by assigning local variables */
	Rd = leaf_day_mresp;                /* (umol/m2/s) day leaf m. resp, proj. area basis */

	/* convert atmospheric CO2 from ppmV --> Pa */
	Ca = meteo_annual->co2Conc * meteo_daily->air_pressure / 1e6;

	/* set parameters for C3 */
	ppe = 2.6;

	/* calculate atmospheric O2 in Pa, assumes 20.9% O2 by volume */
	O2  = (O2CONC / 100. ) * meteo_daily->air_pressure;

	/* correct kinetic constants for temperature, and do unit conversions */
	Ko  = Ko25 * pow ( q10Ko , ( meteo_daily->tavg - 25. ) / 10. );
	Ko *= 100.;   /* mbar --> Pa */

	if ( meteo_daily->tday > 15. )
	{
		Kc  = Kc25  * pow ( q10Kc , ( meteo_daily->tday - 25. ) / 10. );
		act = act25 * pow ( q10act, ( meteo_daily->tday - 25. ) / 10. );
	}
	else
	{
		Kc  = Kc25  * pow ( 1.8 * q10Kc,  ( meteo_daily->tday - 15. ) / 10.) / q10Kc;
		act = act25 * pow ( 1.8 * q10act, ( meteo_daily->tday - 15. ) / 10.) / q10act;
	}

	Kc *= 0.1;                  /* ubar --> Pa */
	//fixme check for g Kg
	act = act  * 1e6 / 60.;     /* umol/mg/min --> umol/kg/s */

	/* calculate gamma (Pa) CO2 compensation point, in the absence of maint resp, assumes Vomax/Vcmax = 0.21 */
	gamma = 0.5 * 0.21 * Kc * O2 / Ko;

	/* calculate Vmax (umol CO2/m2/s) max rate of carboxylation from leaf nitrogen data and Rubisco activity */
	Vmax = leafN * flnr * fnr * act;

	/* calculate Jmax = f(Vmax), reference:	Wullschleger, S.D., 1993.  Biochemical limitations to carbon assimilation in C3 plants -
	 * A retrospective analysis of the A/Ci curves from	109 species. Journal of Experimental Botany, 44:907-920.
	 */
	/* compute (umol electrons/m2/s) max rate electron transport */
	Jmax = 2.1 * Vmax;

	/* calculate J = f(Jmax, ppfd), reference:
		de Pury and Farquhar 1997
		Plant Cell and Env.
	 */
	var_a  = 0.7;
	var_b  = -Jmax - (ppfd * pabs / ppe );
	var_c  = Jmax  *  ppfd * pabs / ppe;

	/* compute (umol RuBP/m2/s) rate of RuBP regeneration */
	J      = ( -var_b - sqrt ( var_b * var_b - 4. * var_a * var_c ) ) / ( 2. * var_a );

	/* solve for Av and Aj using the quadratic equation, substitution for Ci
		from A = g(Ca-Ci) into the equations from Farquhar and von Caemmerer:

		       Vmax (Ci - gamma)
		Av =  -------------------   -   Rd
		      Ci + Kc (1 + O2/Ko)


		         J (Ci - gamma)
		Aj  =  -------------------  -   Rd
	           4.5 Ci + 10.5 gamma
	 */

	/* quadratic solution for Av */
	var_a =  -1. / cond_corr;
	var_b = Ca + ( Vmax - Rd ) / cond_corr + Kc * (1. + O2 / Ko );
	var_c = Vmax * ( gamma - Ca ) + Rd * ( Ca + Kc * ( 1. + O2 / Ko ) );
	det   = var_b * var_b - 4. * var_a * var_c;

	/* check condition */
	CHECK_CONDITION( det , <, 0.0);

	/* compute Av (umol CO2/m2/s) carboxylation limited assimilation */
	Av = ( -var_b + sqrt( det ) ) / ( 2. * var_a );

	/* quadratic solution for Aj */
	var_a = -4.5 / cond_corr;
	var_b = 4.5 * Ca + 10.5 * gamma + J/cond_corr - 4.5 * Rd / cond_corr;
	var_c = J * ( gamma - Ca ) + Rd * ( 4.5 * Ca + 10.5 * gamma);
	det   = var_b * var_b - 4. * var_a * var_c;

	/* check condition */
	CHECK_CONDITION( det , <, 0.0);

	/* compute (umol CO2/m2/s) RuBP regen limited assimilation */
	Aj = ( -var_b + sqrt( det ) ) / ( 2. * var_a );

	/* compute (umol CO2/m2/s) final assimilation rate */
	/* estimate A as the minimum of (Av,Aj) */
	if ( Av < Aj ) A = Av;
	else           A = Aj;

	/* compute (Pa) intercellular [CO2] */
	//fixme currently not used
	Ci = Ca - ( A / cond_corr );

	/* compute assimilation (umol/m2/s) */
	return A;


}

