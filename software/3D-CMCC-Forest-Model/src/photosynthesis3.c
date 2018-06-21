/*
 * photosynthesis3.c
 *
 *  Created on: 27/feb/2018
 *      Author: alessio-cmcc
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

//extern logger_t* g_debug_log;
extern settings_t* g_settings;

void photosynthesis_FvCB_BB (cell_t *const c, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual)
{

	double cond_corr;        /* (umol/m2/s/Pa) leaf conductance corrected for CO2 vs. water vapor */
	double leafN;            /* (gNleaf/m2) leaf N per unit leaf area */
	double par_abs;          /* (umol/m2 covered/sec) absorbed par */
	double leaf_day_mresp;   /* (umol/m2/s) day leaf m. resp, proj. area basis */
	double psn;              /* photosynthesis (gC/m2/day) */
	int sun_shade;           /* 0 for sun 1 for shaded */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* This function is a wrapper and replacement for the photosynthesis code which used to be in the central bgc.c code */
	/* note: the basis of this photosynthesis code is the DePury and Farquhar two-leaf model of photosynthesis
	 * (Farquhar, Caemmerer et al. 1980; De Pury and Farquhar 1997).*/

	/************************************************************************************************************************************/

	if ( s->value[LEAF_SUN_N] > 0. )
	{
		/* SUNLIT canopy fraction photosynthesis per unit area */
		sun_shade = 0;

		/* convert conductance from m/s --> umol/m2/s/Pa, and correct for CO2 vs. water vapor
		(see for correction also Nobel 1991; Jones 1992 and Landsberg and Sands book pg 54) */
		cond_corr                    = 0.; /* not used if Ball Berry */

		/* convert Leaf Nitrogen from tN/cell --> to gN m-2 one-sided leaf area */
		leafN                        = ( s->value[LEAF_SUN_N] * 1e6 / g_settings->sizeCell ) / s->value[LAI_SUN_PROJ];

		/* convert from mass to molar units, and from a daily rate to a rate per second (umol/m2/s) */
		//note: since absorbed radiation is scaled to the 24 hours also leaf day resp should be scaled to 24 hours
		leaf_day_mresp               = ( s->value[DAILY_LEAF_SUN_MAINT_RESP] / ( 86400. * GC_MOL * 1e-6 ) ) / s->value[LAI_SUN_PROJ];

		/* convert absorbed par per projected LAI molPAR/m2/day --> umol/m-2 one-sided leaf area/sec */
		par_abs                      = ( s->value[APAR_SUN] * 1e6 / 86400. ) / s->value[LAI_SUN_PROJ];

		/* call Farquhar for sun leaves leaves photosynthesis */
		psn = Farquhar_BB (c, s, meteo_daily, meteo_annual, cond_corr, leafN, par_abs, leaf_day_mresp, sun_shade);

		/* Canopy net assimilation and converting from umol/m2 leaf/sec gC/m2/day and to LAI for canopy computation */
		s->value[ASSIMILATION_SUN]   = psn * s->value[LAI_SUN_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

		/* check */
		if ( psn <= 0 ) s->value[ASSIMILATION_SUN] = 0.;
	}

	/************************************************************************************************************************************/

	if ( s->value[LEAF_SHADE_N] > 0. )
	{
		/* SHADED canopy fraction photosynthesis per unit area */
		sun_shade = 1;

		/* convert conductance from m/s --> umol/m2/s/Pa, and correct for CO2 vs. water vapor
		(see for correction also Nobel 1991; Jones 1992 and Landsberg and Sands book pg 54) */
		cond_corr                    = 0.; /* not used if Ball Berry */

		/* convert Leaf Nitrogen from tN/cell --> to gN m-2 one-sided leaf area */
		leafN                        = ( s->value[LEAF_SHADE_N] * 1e6 / g_settings->sizeCell ) / s->value[LAI_SHADE_PROJ];

		/* convert from mass to molar units, and from a daily rate to a rate per second (umol/m2/s) */
		//note: since absorbed radiation is scaled to the 24 hours also leaf day resp should be scaled to 24 hours
		leaf_day_mresp               = ( s->value[DAILY_LEAF_SHADE_MAINT_RESP] / ( 86400. * GC_MOL * 1e-6 ) ) / s->value[LAI_SHADE_PROJ];

		/* convert absorbed par per projected LAI molPAR/m2/day --> umol/m-2 one-sided leaf area/sec */
		par_abs                      = ( s->value[APAR_SHADE] * 1e6 / 86400. ) / s->value[LAI_SHADE_PROJ];

		/* call Farquhar for shade leaves photosynthesis */
		psn = Farquhar_BB (c, s, meteo_daily, meteo_annual, cond_corr, leafN, par_abs, leaf_day_mresp, sun_shade );

		/* Canopy net assimilation (photosynthesis) and converting from umol/m2 leaf/sec gC/m2/day and to LAI for canopy computation */
		s->value[ASSIMILATION_SHADE] = psn * s->value[LAI_SHADE_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

		/* check */
		if ( psn <= 0 ) s->value[ASSIMILATION_SHADE] = 0.;
	}

	/************************************************************************************************************************************/
	/* as In Wohlfahrt & Gu 2015 Plant Cell and Environment */
	/* "GPP is intended as an integration of apparent photosynthesis (true photosynthesis minus photorespiration), NOT
	gross (true) photosynthesis" */

	/* total net assimilation */
	s->value[ASSIMILATION] = s->value[ASSIMILATION_SUN] + s->value[ASSIMILATION_SHADE];

	/* check condition */
	CHECK_CONDITION( s->value[ASSIMILATION] , <, 0.0);

	/************************************************************************************************************************************/
	/* gpp */

	/* note: for the final flux assignment, the assimilation output needs to have the maintenance respiration rate added... */
	s->value[GPP_SUN]                     = s->value[ASSIMILATION_SUN]   + s->value[DAILY_LEAF_SUN_MAINT_RESP];
	s->value[GPP_SHADE]                   = s->value[ASSIMILATION_SHADE] + s->value[DAILY_LEAF_SHADE_MAINT_RESP];

	/* to avoid negative values when Rd > Assimilation */
	if (s->value[GPP_SUN]  < 0.) s->value[GPP_SUN]   = 0.;
	if (s->value[GPP_SHADE]< 0.) s->value[GPP_SHADE] = 0.;

	/* total gpp */
	s->value[GPP]                         = s->value[GPP_SUN] + s->value[GPP_SHADE];

	/* check condition */
	CHECK_CONDITION( s->value[GPP]       , <, 0.0);
	CHECK_CONDITION( s->value[GPP_SUN]   , <, 0.0);
	CHECK_CONDITION( s->value[GPP_SHADE] , <, 0.0);

	/* gC/m2/day --> tC/cell/day */
	s->value[GPP_tC]                      = s->value[GPP] / 1e6 * g_settings->sizeCell ;

	/************************************************************************************************************************************/

	/* class level */
	s->value[MONTHLY_ASSIMILATION]       += s->value[ASSIMILATION];
	s->value[MONTHLY_ASSIMILATION_SUN]   += s->value[ASSIMILATION_SUN];
	s->value[MONTHLY_ASSIMILATION_SHADE] += s->value[ASSIMILATION_SHADE];

	s->value[YEARLY_ASSIMILATION]        += s->value[ASSIMILATION];
	s->value[YEARLY_ASSIMILATION_SUN]    += s->value[ASSIMILATION_SUN];
	s->value[YEARLY_ASSIMILATION_SHADE]  += s->value[ASSIMILATION_SHADE];

	s->value[MONTHLY_GPP]                += s->value[GPP];
	s->value[MONTHLY_GPP_SUN]            += s->value[GPP_SUN];
	s->value[MONTHLY_GPP_SHADE]          += s->value[GPP_SHADE];

	s->value[YEARLY_GPP]                 += s->value[GPP];
	s->value[YEARLY_GPP_SUN]             += s->value[GPP_SUN];
	s->value[YEARLY_GPP_SHADE]           += s->value[GPP_SHADE];

	/* cell level */
	c->daily_gpp                         += s->value[GPP];
	c->monthly_gpp                       += s->value[GPP];
	c->annual_gpp                        += s->value[GPP];

	c->daily_ass                         += s->value[ASSIMILATION];
	c->monthly_ass                       += s->value[ASSIMILATION];
	c->annual_ass                        += s->value[ASSIMILATION];

	c->daily_gpp_tC                      += s->value[GPP_tC];
	c->monthly_gpp_tC                    += s->value[GPP_tC];
	c->annual_gpp_tC                     += s->value[GPP_tC];

	/* yearly veg days counter */
	if ( s->value[GPP] > 0. )
	{
		++s->counter[YEARLY_VEG_DAYS];
	}

	/************************************************************************************************************************************/

	/* test: compute actual quantum canopy efficiency (molC/molphotons PAR) */

	if ( s->value[ASSIMILATION]       > 0. ) s->value[ALPHA_EFF]          = ( s->value[ASSIMILATION]       / GC_MOL ) / s->value[PAR];
	else                                         s->value[ALPHA_EFF]          = 0.;
	if ( s->value[ASSIMILATION_SUN]   > 0. ) s->value[ALPHA_EFF_SUN]      = ( s->value[ASSIMILATION_SUN]   / GC_MOL ) / s->value[PAR_SUN];
	else                                         s->value[ALPHA_EFF_SUN]      = 0.;
	if ( s->value[ASSIMILATION_SHADE] > 0. ) s->value[ALPHA_EFF_SHADE]    = ( s->value[ASSIMILATION_SHADE] / GC_MOL ) / s->value[PAR_SHADE];
	else                                         s->value[ALPHA_EFF_SHADE]    = 0.;

	/************************************************************************************************************************************/

}

double Farquhar_BB (cell_t *const c, species_t *const s,const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual,
		const double cond_corr, const double leafN, const double par_abs, const double leaf_day_mresp, const int sun_shade )
{
	/* Farquhar, von Caemmerer and Berry (1980) Planta. 149: 78-90. */

	/* the weight proportion of Rubisco to its nitrogen content, fnr, is calculated from the relative proportions of the basic amino acids
		that make up the enzyme, as listed in the Handbook of Biochemistry, Proteins, Vol III, p. 510, which references: Kuehn and McFadden, Biochemistry, 8:2403, 1969 */

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

	int err;


#if 0
	/* Badger and Collatz 1977 */
	const double Kc25          = 404;    /* (ubar or umol mol-1) Michaelis-Menten const carboxylase, 25 deg C  Badger and Collatz value*/
	const double Ea_Kc         = 59400;  /* (J mol-1) Activation energy for carboxylase */
	const double Ko25          = 248000; /* (ubar or umol mol-1) Michaelis-Menten const oxygenase, 25 deg C 248 Badger and Collatz, 278.4 Bernacchi et al., 2001 */
	const double Ea_Ko         = 36000;  /* (J mol-1) Activation energy for oxygenase */
#else
	/* Bernacchi et al., 2001 */
	const double Kc25          = 404.9;  /* (ubar or umol mol-1) Michaelis-Menten const carboxylase, 25 deg C  Badger and Collatz value*/
	const double Ea_Kc         = 79430;  /* (J mol-1) Activation energy for carboxylase */
	const double Ko25          = 278400; /* (ubar or umol mol-1) Michaelis-Menten const oxygenase, 25 deg C 248 Badger and Collatz, 278.4 Bernacchi et al., 2001 */
	const double Ea_Ko         = 36380;  /* (J mol-1) Activation energy for oxygenase */
#endif

	const double act25         = 60;     /* (umol CO2 g-1 Rubisco s-1) specific activity of Rubisco at 25 °C */
	const double phiII         = 0.85;   /* (DIM) fraction of PAR effectively absorbed by photosytem II (leaf absorptance); 0.8 for Bonan et al., 2011 */
	const double fnr           = 7.16;   /* (DIM) g Rubisco/gN Rubisco weight proportion of rubisco relative to its N content Kuehn and McFadden (1969) */
	const double thetaII       = 0.7;    /* (DIM) curvature of the light-response curve of electron transport (DePury and Farquhar, 1997, Bonan et al., 2011) */
	const double ppe           = 2.6;    /* (mol e- /mol photons) photons absorbed by PSII per e- transported (quantum yield of electron transport) dePury and Farquhar 1997*/

	/* temperature control */
	const double Ea_V          = 51560;  /* (J mol-1) Activation energy for J see Maespa */
	double S_V           = 472.;   /* (JK-1 mol) Vmax temperature response parameter */
	const double H_V           = 144568; /* (J mol-1) Vmax curvature parameter */
	const double Ea_J          = 43790;  /* (J mol-1) Activation energy for J see Maespa */
	double S_J           = 710 ;   /* (JK-1 mol) electron-transport temperature response parameter */
	const double H_J           = 220000; /* (J mol-1) curvature parameter of J */
	//const double Ea_Rub        = ?????;  /* (kJ mol-1) Activation energy for Rubisco */

	/* local variables */
	double Kc;                            /* (umol/mol) Michaelis-Menten constant for carboxylase reaction */
	double Ko;                            /* (umol/mol) Michaelis-Menten constant for oxygenase reaction */
	double Kmfn;                          /* (umol/mol) Effective Michaelis-Menten coefficient of Rubisco activity */
	double Vcmax25;                       /* (umol/m2/s) Leaf-scale maximum carboxylation rate, 25°C */
	double Vcmax;                         /* (umol/m2/s) Actual Leaf-scale maximum carboxylation rate */
	double Jmax25;                        /* (umol/m2/s) Maximum rate of RuBP (ribulose-1,5-bisphosphate) regeneration, 25 °C */
	double Jmax;                          /* (umol/m2/s) rate of RuBP (ribulose-1,5-bisphosphate) regeneration */
	double J;                             /* (umol/m2/s) Current rate of RuBP (ribulose-1,5-bisphosphate) regeneration */
	double pabsII;                        /* (molPAR/m2/s) PAR effectively absorbed by the phosystemII */
	double gamma_star;                    /* (umol/mol) CO2 compensation point without dark respiration */
	double gamma_unstar;                  /* (umol/mol) CO2 compensation point */
	double Ca;                            /* (Pa) atmospheric [CO2] pressure */
	double O2umol;                        /* (umol/mol) Oxygen partial pressure (umol mol-1) */
	double Av;                            /* (umol/m2/s) carboxylation rate for limited assimilation (synonym of Vc) */
	double Aj;                            /* (umol/m2/s) RuBP (ribulose-1,5-bisphosphate) regeneration limited assimilation */
	double A;                             /* (umol/m2/s) final assimilation rate */
	double Ci;                            /* (Pa) intercellular [CO2] */
	double Rd;                            /* (umol/m2/s) (umol/m2/s) day leaf m. resp, proj. area basis */
	double tleaf;                         /* (°C) leaf temperature (assumed equal to Tair) */
	double tleaf_K;                       /* (Kelvin) leaf temperature (assumed equal to Tair) */
	double tleaf10;                       /* (°C) 10 day mean leaf temperature (assumed equal to Tair) */
	double tleaf10_K;                     /* (Kelvin) 10 day mean leaf temperature (assumed equal to Tair) */
	double rel_hum;                       /* (fraction) relative humidity at leaf surface */
	double vpd;                           /* (kPa) Vapor pressure deficit */
	double temp_corr;                     /* temperature function */
	double high_temp_corr;                /* high temperature inhibition */
	double var_a, var_b, var_c;

	double g0 = 0;                        /* (mol/m2/s) stomatal conductance when A = 0 at the light compensation point */
	double g1;                            /* () empirical coefficient */
	double gsdiva;                        /* gs divided A */
	double gs;                            /* (mol/m2/s) stomatal conductance to umolCO2 */
	double gsmin;                         /* (mol/m2/s) minimum stomatal conductance to umolCO2 */
	double gsmax;                         /* (mol/m2/s) maximum stomatal conductance to umolCO2 */
	double gl;                            /**/
	double gl_bl;                         /* (mol/m2/s) boundary layer conductance */
	double gl_c;                          /* (mol/m2/s) cuticular conductance */
	double cic;
	double cij;

	//todo todo todo todo todo move in species.txt (this should be the only variable for all photosynthesis)
	const double beta       = 1.67; /* Jmax:Vcmax note: in Medlyn et al., 2002 */
	//const double beta       = 2.1; /* ratio between Vcmax and Jmax see dePury and Farquhar 1997; for fagus see Liozon et al., (2000) and Castanea */

	const double test_Vcmax = 55 ; /* (umol/m2/sec) Vcmax for fagus see Deckmyn et al., 2004 GCB */
	const double test_Jmax  = 100; /* (umol/m2/sec) Jmax for fagus see Deckmyn et al., 2004 GCB */

	const int test_assimilation = 0; /* 0 uses min (Av, Aj), 1 only Av, 2 only Aj */

	double conv;                           /* conversion factor for conductance from m/s to mol/m/sec Chen et al., 1999 */


	/* conversion factor for conductance from m/s to mol/m/sec Chen et al., 1999 */
	conv = ( meteo_daily->air_pressure / 1e3 ) / ( Rgas * ( meteo_daily->tday + TempAbs ) );


	if ( ! string_compare_i(s->name, "Fagussylvatica") )
	{
		g1 = 4; // 6.99;

		//fixme
		s->value[MAXCOND]   = 0.005;
		s->value[N_RUBISCO] = 0.1;

	}
	else if ( ! string_compare_i(s->name, "Pinusylvestris") )
	{
		g1 = 1.8;

		//fixme
		s->value[MAXCOND]   = 0.002;
		s->value[N_RUBISCO] = 0.055;
	}
	else if ( ! string_compare_i(s->name, "Piceaabies") )
	{
		g1 = 2;

		//fixme
		s->value[MAXCOND]   = 0.002;
		s->value[N_RUBISCO] = 0.035;
	}
	else if ( ! string_compare_i(s->name, "Pinuspinaster") )
	{
		g1 = 1.8;

		//fixme
		s->value[MAXCOND]   = 0.002;
		s->value[N_RUBISCO] = 0.055;
	}


	/*
	 * some parameter values (to be included in species.txt):
	 * Vcmax = 55 (umol/m2/sec) for fagus see Deckmyn et al., 2004 GCB
	 * Vcmax = 36.21 (umol/m2/sec) for fagus see Scartazza et al., 2015
	 * Vcmax = 26.66 (umolCO2/m2/sec) for p. sylvestris see Sampson et al., (2006)
	 * Vcmax = 33.7 (umolCO2/m2/sec) for q. robur see Samspon et al. (2006)
	 * Vcmax = 72 (mmol/m2/sec) for p. sylvestris see Sampson et al., 2001
	 * Jmax  = 100 (umol/m2/sec) for fagus see Deckmyn et al., 2004 GCB
	 * Jmax  = 104 (umol/m2/sec) for fagus see Scartazza et al., 2015
	 * beta  = 2.48 for p. sylvestris see Samspon et al., (2006)
	 * beta  = 2.42 for Q. robur see Samspon et al., (2006)
	 * beta  = 2.8 for F. sylvatica see Scartazza et al., 2015
	 */

	/* (umol/m2/s) day leaf m. resp, proj. area basis */
	//note: BIOME-BGC assumes leaf main respiration during daylight as dark respiration
	//note: 3D-CMCC following Dufrene et al. considers light inhibition phenomenon in the daylight respiration routine
	Rd        = leaf_day_mresp;

	/* assign defualt value to gsmin */
	gsmin     = 1e-9;

	/* conversion maximum stomatal conductance m/sec to mmol/m2/sec see Korner et al., 1979 */
	gsmax     = s->value[MAXCOND] * 1e3 * conv;

	/* conversion boundary layer conductance m/sec to mmol/m2/sec see Korner et al., 1979 */
	gl_bl     = s->value[BLCOND] * 1e3 * conv;

	/* conversion cuticular conductance m/sec to mmol/m2/sec see Korner et al., 1979 */
	gl_c      = s->value[CUTCOND] * 1e3 * conv;

	/* convert atmospheric CO2 from ppmV */
	Ca        = meteo_annual->co2Conc;

	/* Oxygen partial pressure (umol mol-1) */
	O2umol    = O2CONC * 1e4;

	/* relative humidity at leaf surface */
	rel_hum   = meteo_daily->rh_f / 100.;

	/* convert VPD from hPa to kPa */
	vpd       = meteo_daily->vpd / 10.;

	/* calculate leaf temperature */
	tleaf     = meteo_daily->tday;
	tleaf_K   = meteo_daily->tday + TempAbs;

	tleaf10   = meteo_daily->ten_day_avg_tday;
	tleaf10_K = meteo_daily->ten_day_avg_tday + TempAbs;


	printf("%g\n", conv);
	printf("%g\n", gsmax);
	//exit(1);


	/****************************** ARRHENIUS KINETICS *****************************/
	/*******************************************************************************/

	/* the enzyme kinetics built into this model are based on Medlyn (1999) */

	/* correct kinetic constants for temperature (umol/mol) */
	Ko   = Ko25 * exp ( Ea_Ko * ( tleaf - 25. ) / ( 298. * Rgas * ( tleaf + TempAbs ) ) );

	/* Arrhenius coefficient for Rubisco for temperature (umol/mol) */
	Kc   = Kc25 * exp ( Ea_Kc * ( tleaf - 25. ) / ( 298. * Rgas * ( tleaf + TempAbs ) ) );

	/* effective Michaelis-Menten coefficient of Rubisco activity (umol/mol) */
	Kmfn = Kc * ( 1. + O2umol / Ko );

	/*******************************************************************************/

	/* compute gamma (umol/mol) CO2 compensation point due to photorespiration, in the presence of respiration*/
	gamma_unstar = CP * AIRMASS * meteo_daily->air_pressure / ( meteo_daily->lh_vap * WATERMASS);

	/* calculate gamma_star (umol/mol) CO2 compensation point due to photorespiration, in the absence of respiration */
	/* note: Bernacchi et al., 2001 method (umol/mol) */
	gamma_star = 42.75 * exp ( 37830. * ( tleaf - 25.) / ( Rgas * ( tleaf + TempAbs ) * ( 25. + TempAbs ) ) );

	/*******************************************************************************/

	/* G0 must be converted to CO2 */
	g0     /= GCtoGW;

	/*******************************************************************************/

	/* Ball and Berry 1987 as modified by Leuning 1990 */
	/*
	 as in the form described in Leuning 1990 (Aust J Plant Phys)

	               g1 A hs
		gs = g0 ---------------
	            Ca - gamma_unstar
	 */

	/* stomatal conductance divided by Assimilation (gs/An) in units of H2O */
	//note: I added soil water
	gsdiva  = g0 + g1 * rel_hum / ( Ca - gamma_unstar ) * s->value[F_SW];

	/* gsdiva must be converted to CO2 */
	gsdiva /= GCtoGW;

	/*****************************************************************************/

	/* Medlyn et al., 2011 Stomatal optmization model */

	/*
	                            g1         An
	 	 gs = g0 + 1.6 (1 + ----------) * ----
							radq(vpd)      Ca
	*/

	/* stomatal conductance divided by Assimilation (gs/An) in units of H2O */
	//note: 1.6 is not used since we need gCO2, I added soil water
	gsdiva  = ( g0  + /*1.6 * */ ( 1. + ( g1 / sqrt( vpd ) ) ) * ( 1. / Ca ) ) * s->value[F_SW];

	/*****************************************************************************/

	/* note: accounting also for leaf cuticular and boundary layer conductances */
	gsdiva  = ( gl_bl * ( gsdiva + gl_c ) ) / ( gl_bl + gsdiva + gl_c );

	/*****************************************************************************/

#if 0

	//not currently used
	//note: modified version of the BIOME-BGC original code
	//note: if accepted move to species.txt

	/* assign Vcmax from species.txt parameter value */
	Vcmax = /*s->value[VCMAX]*/ test_Vcmax;

#else
	/*******************************************************************************/

	/* calculate Vcmax from leaf nitrogen data and Rubisco activity see Harrison et al., 2009 PCE */

	/* kg Nleaf   kg NRub    kg Rub      umol            umol
	   -------- X -------  X ------- X ---------   =   --------
	      m2      kg Nleaf   kg NRub   kg RUB * s       m2 * s

	     (leafN)  X  (flnr)  X  (fnr)  X   (act)   =    (Vcmax)
	 */

	/* calculate Vcmax (umol CO2/m2/s) max rate of carboxylation from leaf nitrogen data and Rubisco activity */
	/* see: Woodrow and Berry (1988); Field (1983); Harley et al., (1992); Medlyn et al., (1999) */

	/* "Vcmax is more realistically formulated as a dynamic quantity that depends on the leaf area–based concentration of Rubisco and the enzyme activity"
	 * references: Niinemets and Tenhunen 1997; Thornton and Zimmermann 2007 */

	/* compute Vcmax25 at 25 °C Bonan et al., (2011) and F_NUTR as in Bonan et al., (2012)*/
	Vcmax25   = leafN * s->value[N_RUBISCO] * fnr * act25 /** s->value[F_NUTR]*/;

	/* temperature corrector factor */
	temp_corr      = exp ( Ea_V * ( tleaf - 25. ) / ( Rgas * tleaf_K * 298.) );

	if ( g_settings->Photo_accl )
	{
		/** acclimation for temperature as in Kattge and Knorr (2007) and CLM5.0 version **/
		/* for Vcmax */
		S_V = 668.39 - 1.07 * ( tleaf10_K - TempAbs );
	}
	
	/* high temperature inhibition factor */
	if ( tleaf > 0.)
	{
		high_temp_corr = ( 1. + exp ( ( S_V * 298. - H_V ) / ( Rgas * tleaf_K ) ) ) /
				( 1. + exp ( ( S_V * tleaf_K - H_V ) / ( Rgas * tleaf_K ) ) );
	}
	else
	{
		high_temp_corr =  1.;
	}

	/* check condition */
	CHECK_CONDITION( temp_corr      , < , 0. );
	CHECK_CONDITION( high_temp_corr , < , 0. );

	/* correct Vcmax25 for temperature Medlyn et al., (1999) with F_SW from Bonan et al., (2011) */
	Vcmax     = Vcmax25 * temp_corr * high_temp_corr * s->value[F_SW];

	/* check condition */
	CHECK_CONDITION( Vcmax , < , 0. );

#endif

	/*******************************************************************************/

#if 0

	//not currently used
	//note: modified version of the BIOME-BGC original code
	//note: if accepted move to species.txt

	/* assign Jmax from species.txt parameter value */
	Jmax = /*s->value[JMAX]*/test_Jmax;

#else

	/* calculate Jmax = f(Vmax), reference:	Wullschleger, S.D., 1993.  Biochemical limitations to carbon assimilation in C3 plants -
	 * A retrospective analysis of the A/Ci curves from	109 species. Journal of Experimental Botany, 44:907-920. */

	/* compute (umol electrons/m2/s) max rate electron transport */
	/* a simplifying assumption that empirically relates the maximum rate of electron transport to maximum carboxylation velocity
	 * see: Wullschleger (1993); Field (1983); Harley et al., (1992); Watanabe et al., (1994); DePury and Farquhar (1997);
	 * Medlyn et al., (1999); Peterson et al., (1999); Liozon et al., (2000); Leuning et al., (2002); Bonan et al., (2011)*/

	/* compute Jmax at 25 °C Bonan et al., (2011) */
	Jmax25         = beta * Vcmax25;

	if ( g_settings->Photo_accl )
	{
		/** acclimation for temperature as in Kattge and Knorr (2007) and CLM5.0 version **/
		/* acclimation for Jmax25 as in Kattge and Knorr (2007) */
		Jmax25    = ( 2.59 - 0.035 * ( tleaf10_K - TempAbs ) ) * Vcmax25;
	}

#endif

	/*******************************************************************************/

	/* temperature corrector factor */
	temp_corr      = exp ( Ea_J * ( tleaf - 25. ) / ( Rgas * tleaf_K * 298.) );

	if ( g_settings->Photo_accl )
	{
		/** acclimation for temperature as in Kattge and Knorr (2007) and CLM5.0 version **/
		/* for Jmax */
		S_J = 659.70 - 0.75 * ( tleaf10_K - TempAbs );
	}

	/* high temperature inhibition factor */
	if ( tleaf > 0.)
	{
		high_temp_corr = ( 1. + exp ( ( S_J * 298. - H_J ) / ( Rgas * 298. ) ) ) /
				( 1. + exp ( ( S_J * tleaf_K - H_J ) / ( Rgas * tleaf_K ) ) );
	}
	else
	{
		high_temp_corr = 1.;
	}

	/* check condition */
	CHECK_CONDITION( temp_corr      , < , 0. );
	CHECK_CONDITION( high_temp_corr , < , 0. );

	/* correct Jmax25 for temperature dePury and Farquhar (1997) */
	Jmax           = Jmax25 * temp_corr * high_temp_corr;

	/* check condition */
	CHECK_CONDITION( Jmax , < , 0.);

	/*******************************************************************************/

	/* irradiance dependence of electron transport (the "non-rectangular hyperbola") */
	/* from the equation of de Pury and Farquhar (1997) Plant Cell and Env. and Bernacchi et al., (2003) Plant Cell and Env. */
	/*
	   theta J^2 - (pabsII + Jmax) J + pabsII Jmax = 0

	 */

	/* compute PAR effectively absorbed by photosystem II */
	pabsII = ( par_abs * phiII ) / ppe;

	/* calculate J = f(Jmax, ppfd) */
	/* smaller root of the quadratic solution to the following equation (see Maespa) */
	var_a  = thetaII;
	var_b  = -Jmax - pabsII;
	var_c  =  Jmax * pabsII;

	/* compute (umol RuBP/m2/s) rate of RuBP (ribulose-1,5-bisphosphate) regeneration */
	/* Solves the quadratic equation - finds SMALLER root. */
	J      = QuadM ( var_a, var_b, var_c, &err );

	/* RuBP regeneration rate */
	J     /= 4.;

	/*******************************************************************************/

	/* Simultaneous solution when Rubisco activity is limiting */

	var_a = g0 + gsdiva * ( Vcmax - Rd );
	var_b =  ( 1. - Ca * gsdiva) * ( Vcmax - Rd ) + g0 * ( Kmfn - Ca ) - gsdiva * ( Vcmax * gamma_star + Kmfn * Rd );
	var_c = -( 1. - Ca * gsdiva) * ( Vcmax  * gamma_star + Kmfn * Rd ) - g0 * Kmfn * Ca;

	/* Solves the quadratic equation - finds LARGER root. */
	cic   = QuadP ( var_a, var_b, var_c, &err );

	if ( err == 1 || cic < 0. || cic > Ca ) Av = 0.;
	else Av    = Vcmax * ( cic - gamma_star ) / ( cic + Kmfn );

	/*******************************************************************************/

	/* Simultaneous solution when electron transport rate is limiting */

	var_a = g0 + gsdiva * ( J - Rd );
	var_b =  ( 1. - Ca * gsdiva ) * ( J - Rd ) + g0 * ( 2. * gamma_star - Ca ) - gsdiva * ( J * gamma_star + 2.* gamma_star * Rd );
	var_c = -( 1. - Ca * gsdiva ) * gamma_star * ( J + 2. * Rd ) - g0 * 2. * gamma_star * Ca;

	/* Solves the quadratic equation - finds LARGER root. */
	cij  = QuadP ( var_a, var_b, var_c, &err );

	Aj   = J * ( cij - gamma_star ) / ( cij + 2. * gamma_star );

	if ( Aj - Rd < 1e-6 )
	{
		/* Below light compensation point */
		cij = Ca;

		Aj = J * ( cij - gamma_star ) / ( cij + 2. * gamma_star );
	}

	/*******************************************************************************/

	/* compute (umol/m2/s) final assimilation rate */
	/* estimate A as the minimum of (Av,Aj) */
	A = MIN ( Av, Aj );

	/*******************************************************************************/

	/* compute actual stomatal conductance (mol/m2/s) */
	gs = g0 + gsdiva * A;

	/* Set nearly zero conductance (for numerical reasons) */
	if ( gs < gsmin ) gs = gsmin;

	/* Set at maximum value if current conductance exceeds maximum value */
	//note: to compare with gsmax which is is H20 rate gs has to be converted too in H20 rate
	if ( ( gs * GCtoGW ) > gsmax )
	{
		gs = gsmax;

		/* Now that GS is known, solve for CI and A as in the Jarvis model */

		/* Solution when Rubisco activity is limiting */
		var_a = 1. / gs;
		var_b = ( Rd - Vcmax ) / gs - Ca - Kc;
		var_c = Vcmax * ( Ca - gamma_star ) - Rd * ( Ca + Kc );

		/* Solves the quadratic equation - finds SMALLER root. */
		Av = QuadM ( var_a, var_b, var_c, &err );

		/* Solution when electron transport rate is limiting */
		var_a = 1. / gs;
		var_b = ( Rd - J ) / gs - Ca - 2. * gamma_star;
		var_c = J * ( Ca - gamma_star ) - Rd * ( Ca + 2. * gamma_star);

		/* Solves the quadratic equation - finds SMALLER root. */
		Aj = QuadM ( var_a, var_b, var_c, &err );

		/* compute (umol/m2/s) final assimilation rate */
		/* estimate A as the minimum of (Av,Aj) */
		A = MIN ( Av, Aj );

	}

	if ( ! sun_shade )
	{
		/* convert stomatal conductance from mol/m2/s to m/s and converted to H20 */
		s->value[STOMATAL_SUN_CONDUCTANCE]  = gs / 1e3 * ( GCtoGW * Rgas * ( meteo_daily->tday + TempAbs ) );

		Ci = Ca - A / s->value[STOMATAL_SUN_CONDUCTANCE];
	}
	else
	{
		/* convert stomatal conductance from mol/m2/s to m/s and converted to H20 */
		s->value[STOMATAL_SHADE_CONDUCTANCE] = gs / 1e3 * ( GCtoGW * Rgas * ( meteo_daily->tday + TempAbs ) );

		Ci = Ca - A / s->value[STOMATAL_SHADE_CONDUCTANCE];
	}

	/*******************************************************************************/

	/* test */
	if ( ! sun_shade )
	{
		/* sun leaves */
		s->value[A_SUN]                  = A;
		if ( Av < Aj )s->value[Av_SUN]   = Av;
		else          s->value[Aj_SUN]   = Aj;

		s->value[YEARLY_A_SUN]    += A;
		s->value[YEARLY_Av_SUN]   += s->value[Av_SUN];
		s->value[YEARLY_Aj_SUN]   += s->value[Aj_SUN];
		CHECK_CONDITION ( fabs ( s->value[YEARLY_A_SUN] - ( s->value[YEARLY_Av_SUN] + s->value[YEARLY_Aj_SUN] ) ) , > , eps );
	}
	else
	{
		/* shaded leaves */
		s->value[A_SHADE]                  = A;
		if ( Av < Aj )s->value[Av_SHADE]   = Av;
		else          s->value[Aj_SHADE]   = Aj;

		s->value[YEARLY_A_SHADE]    += A;
		s->value[YEARLY_Av_SHADE]   += s->value[Av_SHADE];
		s->value[YEARLY_Aj_SHADE]   += s->value[Aj_SHADE];
		CHECK_CONDITION ( fabs ( s->value[YEARLY_A_SHADE] - ( s->value[YEARLY_Av_SHADE] + s->value[YEARLY_Aj_SHADE] ) ) , > , eps );
	}

	/* total leaves */
	s->value[A_TOT]  = s->value[A_SUN]  + s->value[A_SHADE];
	s->value[Av_TOT] = s->value[Av_SUN] + s->value[Av_SHADE];
	s->value[Aj_TOT] = s->value[Aj_SUN] + s->value[Aj_SHADE];

	s->value[YEARLY_A_TOT]  += s->value[A_SUN]  + s->value[A_SHADE];
	s->value[YEARLY_Av_TOT] += s->value[Av_SUN] + s->value[Av_SHADE];
	s->value[YEARLY_Aj_TOT] += s->value[Aj_SUN] + s->value[Aj_SHADE];
	CHECK_CONDITION ( fabs ( s->value[YEARLY_A_TOT] - ( s->value[YEARLY_Av_TOT] + s->value[YEARLY_Aj_TOT] ) ) , > , eps );

	/* return assimilation rate (umol/m2/s) */
	return A;

}

