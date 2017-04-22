/*
 * canopy_radiation_sw_band.c
 *
 *  Created on: 05 lug 2016
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"
#include "canopy_radiation_sw_band.h"

#define TEST


extern logger_t* g_debug_log;

void canopy_sw_band_abs_trans_refl_radiation(cell_t *const c, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily,
		double Light_abs_frac, double Light_abs_frac_sun, double Light_abs_frac_shade, double Light_refl_par_frac, double Light_refl_par_frac_sun, double Light_refl_par_frac_shade,
		double Light_refl_sw_frac, double Light_refl_sw_frac_sun, double Light_refl_sw_frac_shade)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* note: This function works at layer/class level computing absorbed transmitted and reflected PAR, NET RADIATION
	 * and PPFD through different height classes/layers considering at square meter takes into account coverage,
	 * it means that a square meter grid cell * represents overall grid cell (see Duursma and Makela, 2007) */

	/* it follows a little bit different rationale compared to BIOME-BGC approach
	* in BIOME_BGC:
	* apar = par * (1 - (exp(- K * LAI)));
	* apar_sun = par * (1 - (exp(- K * LAI_SUN)));
	* apar_shade = apar- apar_sun;
	*
	* in 3D-CMCC FEM:
	* apar_sun = par * (1 - (exp(- K * LAI_SUN_EXP)));
	* par_transm_sun  = par - apar_sun;
	* apar_shade = par_transm_sun * (1 - (exp(- K * LAI_SHADE_EXP)));
	* apar = apar_sun + apar_shade;
	*
	* then it consider that an amount of sunlit leaf are not completely outside the canopy
	* but there's an exponential decay of absorption also for sunlit foliage	 *
	*/

	/***********************************************************************************************/

	/* light reflection, absorption and transmission */
	logger(g_debug_log,"\n*Light reflection, absorption and transmission*\n");

	/***********************************************************************************************/
	/* PAR computation (molPAR/m2 covered/day) */
	logger(g_debug_log,"\n-PAR-\n");

	/** sun leaves **/

	s->value[PAR]             = meteo_daily->par * s->value[DAILY_CANOPY_COVER_EXP];
	s->value[PAR_REFL_SUN]    = s->value[PAR] * Light_refl_par_frac_sun /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[PAR_SUN]         = s->value[PAR] - s->value[PAR_REFL_SUN];
	s->value[APAR_SUN]        = s->value[PAR_SUN] * Light_abs_frac_sun /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[TRANSM_PAR_SUN]  = s->value[PAR_SUN] - s->value[APAR_SUN];

	/* check PAR balance for sun leaves */
	CHECK_CONDITION ( fabs ( ( s->value[PAR_SUN] - s->value[TRANSM_PAR_SUN] ) - s->value[APAR_SUN] ), >, eps );
	CHECK_CONDITION ( fabs ( ( s->value[PAR] - s->value[TRANSM_PAR_SUN] ) - ( s->value[PAR_REFL_SUN] + s->value[APAR_SUN] ) ), >, eps );

	/** shaded leaves **/

	s->value[PAR_REFL_SHADE]   = s->value[TRANSM_PAR_SUN] * Light_refl_par_frac_shade /** s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[PAR_SHADE]        = s->value[TRANSM_PAR_SUN] - s->value[PAR_REFL_SHADE];
	s->value[APAR_SHADE]       = s->value[PAR_SHADE] * Light_abs_frac_shade /** s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[TRANSM_PAR_SHADE] = s->value[PAR_SHADE] - s->value[APAR_SHADE];

	/* check PAR balance for shaded leaves */
	CHECK_CONDITION ( fabs ( ( s->value[PAR_SHADE] - s->value[TRANSM_PAR_SHADE] ) - s->value[APAR_SHADE] ), >, eps );
	CHECK_CONDITION ( fabs ( ( s->value[TRANSM_PAR_SUN] - s->value[TRANSM_PAR_SHADE] ) - ( s->value[PAR_REFL_SHADE] + s->value[APAR_SHADE] ) ), >, eps );

	/** overall canopy **/

	s->value[APAR]             = s->value[APAR_SUN] + s->value[APAR_SHADE];
	s->value[PAR_REFL]         = s->value[PAR_REFL_SUN] + s->value[PAR_REFL_SHADE];
	s->value[TRANSM_PAR]       = s->value[TRANSM_PAR_SHADE];

	logger(g_debug_log, "-Par incoming = %g molPAR/m^2/day\n", meteo_daily->par);
	logger(g_debug_log, "-Par reflected = %g molPAR/m^2/day\n", s->value[PAR_REFL_SUN]);
	logger(g_debug_log, "-Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR_SUN]);
	logger(g_debug_log, "-Apar sun = %g molPAR/m^2 covered/day\n", s->value[APAR_SUN]);
	logger(g_debug_log, "-Transmitted Par sun = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR_SUN]);
	logger(g_debug_log, "-Par reflected = %g molPAR/m^2/day\n", s->value[PAR_REFL_SHADE]);
	logger(g_debug_log, "-Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR_SHADE]);
	logger(g_debug_log, "-Apar shade = %g molPAR/m^2 covered/day\n", s->value[APAR_SHADE]);
	logger(g_debug_log, "-Transmitted Par shade = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR_SHADE]);
	logger(g_debug_log, "-Apar total = %g molPAR/m^2 covered/day\n", s->value[APAR]);
	logger(g_debug_log, "-Transmitted Par total = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR]);

	/* check */
	CHECK_CONDITION(s->value[APAR],       <, ZERO );
	CHECK_CONDITION(s->value[TRANSM_PAR], <, ZERO );
	CHECK_CONDITION(s->value[APAR] + s->value[TRANSM_PAR], <, ZERO );
	CHECK_CONDITION(fabs((s->value[APAR] + s->value[TRANSM_PAR] + s->value[PAR_REFL] )-s->value[PAR]), >, eps);


	/***********************************************************************************************/
	/* Short Wave computation (W/m2 covered) */
	logger(g_debug_log,"\n-Short Wave-\n");

	/** sun leaves **/

	s->value[SW_RAD]              = meteo_daily->sw_downward_W * s->value[DAILY_CANOPY_COVER_EXP];
	s->value[SW_RAD_REFL_SUN]     = s->value[SW_RAD] * Light_refl_sw_frac_sun /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[SW_RAD_SUN]          = s->value[SW_RAD] - s->value[SW_RAD_REFL_SUN];
	s->value[SW_RAD_ABS_SUN]      = s->value[SW_RAD_SUN] * Light_abs_frac_sun /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[SW_RAD_TRANSM_SUN]   = s->value[SW_RAD_SUN] - s->value[SW_RAD_ABS_SUN];

	/* check Short Wave balance for sun leaves */
	CHECK_CONDITION ( fabs ( ( s->value[SW_RAD_SUN] - s->value[SW_RAD_TRANSM_SUN] ) - s->value[SW_RAD_ABS_SUN] ), >, eps );
	CHECK_CONDITION ( fabs ( ( s->value[SW_RAD] - s->value[SW_RAD_TRANSM_SUN] ) - ( s->value[SW_RAD_REFL_SUN] + s->value[SW_RAD_ABS_SUN] ) ), >, eps );

	/** shaded leaves **/

	s->value[SW_RAD_REFL_SHADE]   = s->value[SW_RAD_TRANSM_SUN] * Light_refl_sw_frac_shade /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[SW_RAD_SHADE]        = s->value[SW_RAD_TRANSM_SUN] - s->value[SW_RAD_REFL_SHADE];
	s->value[SW_RAD_ABS_SHADE]    = s->value[SW_RAD_SHADE] * Light_abs_frac_shade /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[SW_RAD_TRANSM_SHADE] = s->value[SW_RAD_SHADE] - s->value[SW_RAD_ABS_SHADE];

	/* check Short Wave balance for shaded leaves */
	CHECK_CONDITION ( fabs ( ( s->value[SW_RAD_SHADE] - s->value[SW_RAD_TRANSM_SHADE] ) - s->value[SW_RAD_ABS_SHADE] ), >, eps );
	CHECK_CONDITION ( fabs ( ( s->value[SW_RAD_TRANSM_SUN] - s->value[SW_RAD_TRANSM_SHADE] ) - ( s->value[SW_RAD_REFL_SHADE] + s->value[SW_RAD_ABS_SHADE] ) ), >, eps );

	/** overall canopy **/

	s->value[SW_RAD_ABS]          = s->value[SW_RAD_ABS_SUN] + s->value[SW_RAD_ABS_SHADE];
	s->value[SW_RAD_REFL]         = s->value[SW_RAD_REFL_SUN] + s->value[SW_RAD_REFL_SHADE];
	s->value[SW_RAD_TRANSM]       = s->value[SW_RAD_TRANSM_SHADE];

	logger(g_debug_log, "-Short Wave incoming = %g W/m2\n", meteo_daily->sw_downward_W);
	logger(g_debug_log, "-Short Wave reflected = %g W/m2\n", s->value[SW_RAD_REFL_SUN]);
	logger(g_debug_log, "-Short Wave less reflected part = %g W/m2\n", s->value[SW_RAD_SUN]);
	logger(g_debug_log, "-Absorbed Short Wave radiation sun = %g W/m2\n", s->value[SW_RAD_ABS_SUN]);
	logger(g_debug_log, "-Transmitted Short Wave sun = %g W/m2\n", s->value[SW_RAD_TRANSM_SUN]);
	logger(g_debug_log, "-Short Wave reflected = %g W/m2\n", s->value[SW_RAD_REFL_SHADE]);
	logger(g_debug_log, "-Short Wave less reflected part = %g W/m2\n", s->value[SW_RAD_SHADE]);
	logger(g_debug_log, "-Absorbed Short Wave radiation shade = %g W/m2\n", s->value[SW_RAD_ABS_SHADE]);
	logger(g_debug_log, "-Transmitted Short Wave shade = %g W/m2\n", s->value[SW_RAD_TRANSM_SHADE]);
	logger(g_debug_log, "-Short Wave total = %g W/m2\n", s->value[SW_RAD_ABS]);
	logger(g_debug_log, "-Transmitted Short Wave total = %g W/m2\n", s->value[SW_RAD_TRANSM]);

	/* check */
	CHECK_CONDITION(s->value[SW_RAD_ABS],    <, ZERO );
	CHECK_CONDITION(s->value[SW_RAD_TRANSM], <, ZERO);
	CHECK_CONDITION(s->value[SW_RAD_ABS] + s->value[SW_RAD_TRANSM], <, ZERO );
	CHECK_CONDITION(fabs((s->value[SW_RAD_ABS] + s->value[SW_RAD_TRANSM] + s->value[SW_RAD_REFL] )-s->value[SW_RAD]), >, eps);

	/***********************************************************************************************/
	/* PPFD computation (umol/m2 covered/sec) */
	logger(g_debug_log,"\n-PPFD-\n");

	/** sun leaves **/

	s->value[PPFD]              = meteo_daily->ppfd * s->value[DAILY_CANOPY_COVER_EXP];
	s->value[PPFD_REFL_SUN]     = s->value[PPFD] * Light_refl_par_frac_sun /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[PPFD_SUN]          = s->value[PPFD] - s->value[PPFD_REFL_SUN];
	s->value[PPFD_ABS_SUN]      = s->value[PPFD_SUN] * Light_abs_frac_sun /* * s->value[DAILY_CANOPY_COVER_EXP]*/ ;
	s->value[PPFD_TRANSM_SUN]   = s->value[PPFD_SUN] - s->value[PPFD_ABS_SUN];

	/* check PPFD balance for sun leaves */
	CHECK_CONDITION ( fabs ( ( s->value[PPFD_SUN] - s->value[PPFD_TRANSM_SUN] ) - s->value[PPFD_ABS_SUN] ), >, eps );
	CHECK_CONDITION ( fabs ( ( s->value[PPFD] - s->value[PPFD_TRANSM_SUN] ) - ( s->value[PPFD_REFL_SUN] + s->value[PPFD_ABS_SUN] ) ), >, eps );

	/** shaded leaves **/

	s->value[PPFD_REFL_SHADE]   = s->value[PPFD_TRANSM_SUN] * Light_refl_par_frac_shade /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[PPFD_SHADE]        = s->value[PPFD_TRANSM_SUN] - s->value[PPFD_REFL_SHADE];
	s->value[PPFD_ABS_SHADE]    = s->value[PPFD_SHADE] * Light_abs_frac_shade /* * s->value[DAILY_CANOPY_COVER_EXP]*/;
	s->value[PPFD_TRANSM_SHADE] = s->value[PPFD_SHADE] - s->value[PPFD_ABS_SHADE];

	/* check PPFD balance for shaded leaves */
	CHECK_CONDITION ( fabs ( ( s->value[PPFD_SHADE] - s->value[PPFD_TRANSM_SHADE] ) - s->value[PPFD_ABS_SHADE] ), >, eps );
	CHECK_CONDITION ( fabs ( ( s->value[PPFD_TRANSM_SUN] - s->value[PPFD_TRANSM_SHADE] ) - ( s->value[PPFD_REFL_SHADE] + s->value[PPFD_ABS_SHADE] ) ), >, eps );

	/** overall canopy **/

	s->value[PPFD_ABS]          = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
	s->value[PPFD_REFL]         = s->value[PPFD_REFL_SUN] + s->value[PPFD_REFL_SHADE];
	s->value[PPFD_TRANSM]       = s->value[PPFD_TRANSM_SHADE];

	logger(g_debug_log, "-PPFD incoming = %g umol/m2/sec\n", meteo_daily->ppfd);
	logger(g_debug_log, "-PPFD reflected = %g umol/m2/sec\n", s->value[PPFD_REFL_SUN]);
	logger(g_debug_log, "-PPFD less reflected part = %g umol/m2/sec\n", s->value[PPFD_SUN]);
	logger(g_debug_log, "-Absorbed ppfd sun = %g umol/m2/sec\n", s->value[PPFD_ABS_SUN]);
	logger(g_debug_log, "-Transmitted PPFD sun = %g umol/m2/sec\n", s->value[PPFD_TRANSM_SUN]);
	logger(g_debug_log, "-PPFD reflected = %g umol/m2/sec\n", s->value[PPFD_REFL_SHADE]);
	logger(g_debug_log, "-PPFD less reflected part = %g umol/m2/sec\n", s->value[PPFD_SHADE]);
	logger(g_debug_log, "-Absorbed ppfd shade = %g umol/m2/sec\n", s->value[PPFD_ABS_SHADE]);
	logger(g_debug_log, "-Transmitted PPFD shade = %g umol/m2/sec\n", s->value[PPFD_TRANSM_SHADE]);
	logger(g_debug_log, "-PPFD total = %g umol/m2/sec\n", s->value[PPFD_ABS]);
	logger(g_debug_log, "-Transmitted PPFD total = %g umol/m2/sec\n", s->value[PPFD_TRANSM]);

	/* check */
	CHECK_CONDITION(s->value[PPFD_ABS],    < , ZERO );
	CHECK_CONDITION(s->value[PPFD_TRANSM], < , ZERO );
	CHECK_CONDITION(s->value[PPFD_ABS_SUN] + s->value[PPFD_TRANSM], <, ZERO );
	CHECK_CONDITION(fabs((s->value[PPFD_ABS] + s->value[PPFD_TRANSM] + s->value[PPFD_REFL] )-s->value[PPFD]), >, eps);

	s->value[YEARLY_APAR] += s->value[APAR];

	/***********************************************************************************************/

	/* it follows rationale of BIOME-BGC to obtain m2 instead m2/m2 */
	//fixme then recompute transmitted fraction!!!!!!!!!!!!
	/*
	if(s->value[PPFD_ABS_SHADE] < 0.0)
	{
		s->value[PPFD_ABS_SHADE]  = 0.0;
	}
	if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
	{
		s->value[PPFD_ABS_SUN] /= s->value[LAI_SUN];
		s->value[PPFD_ABS_SHADE] /= s->value[LAI_SHADE];
	}
	else
	{
		s->value[PPFD_ABS_SUN] = s->value[PPFD_ABS_SHADE] = 0.0;

	}
	s->value[PPFD_ABS] = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
	*/
}

void canopy_radiation_sw_band(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species,  meteo_daily_t *const meteo_daily)
{
	static int cell_height_class_counter;
	static int layer_height_class_counter;

	double Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade;                      /* (ratio) fraction of PAR and Short Wave radiation absorbed */
	double Light_trasm_frac, Light_trasm_frac_sun, Light_trasm_frac_shade;                /* (ratio) fraction of PAR and Short Wave radiation transmitted */

	double Light_refl_par_frac;                                                           /* (ratio) fraction of PAR reflected from canopy */
	double Light_refl_par_frac_sun;                                                       /* (ratio) fraction of PAR reflected from sun leaves */
	double Light_refl_par_frac_shade;                                                     /* (ratio) fraction of PAR reflected from shaded leaves */
	double Light_refl_sw_frac;                                                            /* (ratio) fraction of Short Wave radiation reflected from the canopy */
	double Light_refl_sw_frac_sun;                                                        /* (ratio) fraction of Short Wave radiation reflected from sun leaves */
	double Light_refl_sw_frac_shade;                                                      /* (ratio) fraction of Short Wave radiation reflected from shaded leaves */
	static double temp_apar;                                                              /* temporary absorbed PAR for layer */
	static double temp_par_refl;                                                          /* temporary reflected PAR for layer */
	static double temp_sw_rad_abs;                                                        /* temporary absorbed short wave for layer */
	static double temp_sw_rad_refl;                                                       /* temporary reflected short wave for layer */
	static double temp_ppfd_abs;                                                          /* temporary absorbed PPFD for layer */
	static double temp_ppfd_refl;                                                         /* temporary reflected PPFD for layer */

	tree_layer_t *l;
	species_t *s;

	l = &c->tree_layers[layer];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	//following Ritchie et al., 1998 and Hydi et al.,2016
	//double actual_albedo;

	logger(g_debug_log, "\n**SHORT WAVE BAND RADIATION**\n");

	/***********************************************************************************************************/

	/* SHORT WAVE RADIATION FRACTIONS */
	/* compute fractions of light intercepted, transmitted and reflected from the canopy */
	/* fraction of light transmitted through the canopy */
	/* note: 21 October 2016, following Duursma and Makela, "LIGHT INTERCEPTION OF NON-HOMOGENEOUS CANOPIES"
	* Tree Physiology Vol. 27, 2007; exp(- s->value[K] * (s->value[LAI]/leaf_cell_cover_eff))
	* and Forrester et al, Forest Ecosystems,2014
	* we currently use approach for homogeneous canopies that improves representation when canopy is not closed
	*/
	if ( s->value[LAI_EXP] )
	{
		Light_trasm_frac = exp(- s->value[K] * s->value[LAI_EXP]);
		Light_trasm_frac_sun = exp(- s->value[K] * s->value[LAI_SUN_EXP]);
		Light_trasm_frac_shade = exp(- s->value[K] * s->value[LAI_SHADE_EXP]);
	}
	else
	{
		Light_trasm_frac = 1.;
		Light_trasm_frac_sun = 1.;
		Light_trasm_frac_shade = 1.;
	}

	/* fraction of light absorbed by the canopy */
	Light_abs_frac = 1. - Light_trasm_frac; //not used
	Light_abs_frac_sun = 1. - Light_trasm_frac_sun;
	Light_abs_frac_shade = 1. - Light_trasm_frac_shade;

	/* fraction of light reflected by the canopy */
	/* for Short Wave radiation and PAR */
	/* following BIOME albedo for PAR is 1/3 of albedo. the absorbed PAR is
	calculated similarly to sw except that albedo is 1/3 for PAR because less
	PAR is reflected than sw_radiation (Jones 1992)*/

#ifdef TEST
	//test 18 november 2016
	//it seems to have much more sense
	if( s->value[LAI_EXP] >= 1. )
	{
		/* short wave */
		Light_refl_sw_frac       = s->value[ALBEDO];
		Light_refl_sw_frac_sun   = s->value[ALBEDO] * ( 1 - exp ( - s->value[K] * s->value[LAI_SUN_EXP]));
		Light_refl_sw_frac_shade = s->value[ALBEDO] * ( 1 - exp ( - s->value[K] * s->value[LAI_SHADE_EXP]));
		/* par */
		Light_refl_par_frac       = s->value[ALBEDO] / 3.;
		Light_refl_par_frac_sun   = (s->value[ALBEDO] / 3.) * ( 1 - exp ( - s->value[K] * s->value[LAI_SUN_EXP] ) );
		Light_refl_par_frac_shade = s->value[ALBEDO] / 3. * ( 1 - exp ( - s->value[K] * s->value[LAI_SHADE_EXP] ) );
	}
	else
	{
		/* short wave */
		Light_refl_sw_frac        = 0.;
		Light_refl_sw_frac_sun    = 0.;
		Light_refl_sw_frac_shade  = 0.;
		/* par */
		Light_refl_par_frac       = 0.;
		Light_refl_par_frac_sun   = 0.;
		Light_refl_par_frac_shade = 0.;
	}
#else

	//test change with;
	//if( s->value[LAI_EXP] >= 1.0 )
	if( s->value[LAI_PROJ] >= 1.0 )
	{
		Light_refl_sw_frac = s->value[ALBEDO];
		Light_refl_sw_frac_sun = s->value[ALBEDO] * ( 1 - exp ( - s->value[K] * s->value[LAI_SUN_EXP]));
		Light_refl_sw_frac_shade = s->value[ALBEDO] * ( 1 - exp ( - s->value[K] * s->value[LAI_SHADE_EXP]));
		Light_refl_par_frac = s->value[ALBEDO]/3.0;
		Light_refl_par_frac_sun = (s->value[ALBEDO]/3.0) * ( 1 - exp ( - s->value[K] * s->value[LAI_SUN_EXP] ) );
		Light_refl_par_frac_shade = s->value[ALBEDO]/3.0 * ( 1 - exp ( - s->value[K] * s->value[LAI_SHADE_EXP] ) );
	}
	//test change with;
	//if( s->value[LAI_EXP] >= 1.0 )
	else if ( !s->value[LAI_PROJ])
	{
		Light_refl_sw_frac = 0.0;
		Light_refl_sw_frac_sun = 0.0;
		Light_refl_sw_frac_shade = 0.0;
		Light_refl_par_frac = 0.0;
		Light_refl_par_frac_sun = 0.0;
		Light_refl_par_frac_shade = 0.0;
	}
	else
	{
		Light_refl_sw_frac = s->value[ALBEDO] * s->value[LAI_EXP];
		Light_refl_sw_frac_sun = s->value[ALBEDO] * ( 1 - exp ( - s->value[K] * s->value[LAI_SUN_EXP] ) );
		Light_refl_sw_frac_shade = s->value[ALBEDO] * ( 1 - exp ( - s->value[K] * s->value[LAI_SHADE_EXP] ) );
		Light_refl_par_frac = (s->value[ALBEDO]/3.0) * s->value[LAI_EXP];
		Light_refl_par_frac_sun = (s->value[ALBEDO]/3.0) * ( 1 - exp ( - s->value[K] * s->value[LAI_SUN_EXP] ) );
		Light_refl_par_frac_shade =  (s->value[ALBEDO]/3.0) * ( 1 - exp ( - s->value[K] * s->value[LAI_SHADE_EXP] ) );
	}
#endif

	logger(g_debug_log, "*Fractions of light absorbed, transmitted and reflected*\n\n");
	logger(g_debug_log, "Light_abs_frac_sun = %g %%\n", Light_abs_frac_sun * 100);
	logger(g_debug_log, "LightTrasm_sun = %g %%\n\n", Light_trasm_frac_sun * 100);
	logger(g_debug_log, "Light_abs_frac_shade = %g %%\n", Light_abs_frac_shade * 100);
	logger(g_debug_log, "LightTrasm_sun = %g %%\n\n", Light_trasm_frac_shade * 100);
	logger(g_debug_log, "Light_abs_frac (sun+shaded) = %g %%\n", Light_abs_frac * 100);
	logger(g_debug_log, "LightTrasm (sun+shaded)= %g %%\n\n", Light_trasm_frac * 100);
	logger(g_debug_log, "Light_refl_sw_rad_canopy_frac = %g %%\n", Light_refl_sw_frac_sun * 100);
	logger(g_debug_log, "LightReflec_par = %g %%\n\n", Light_refl_par_frac * 100);


	//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
	//fixme following MAESPA (Duursma et al.,) and from Campbell & Norman (2000, p. 259) dominated layers should have just shaded leaves

	/* RADIATION */
	/*****************************************************************************************************************/
	/* first height class in the cell is processed */
	if( !layer_height_class_counter && !cell_height_class_counter )
	{
		/* reset temporary values when the first height class in layer is processed */
		temp_apar        = 0.;
		temp_par_refl    = 0.;
		temp_sw_rad_abs  = 0.;
		temp_sw_rad_refl = 0.;
		temp_ppfd_abs    = 0.;
		temp_ppfd_refl   = 0.;
	}
	/*****************************************************************************************************************/

	/* shared functions among all class/layers */
	/* counters */
	++layer_height_class_counter;
	++cell_height_class_counter;

	logger(g_debug_log, "layer_height_class_counter = %d\n", layer_height_class_counter);

	/*************************************************************************/
	/* compute reflected, absorbed and transmitted Par, Short Wave radiation and PPFD class level */
	canopy_sw_band_abs_trans_refl_radiation (c, height, dbh, age, species, meteo_daily, Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade,
			Light_refl_par_frac, Light_refl_par_frac_sun, Light_refl_par_frac_shade, Light_refl_sw_frac, Light_refl_sw_frac_sun, Light_refl_sw_frac_shade);

	/*************************************************************************/
	/* temporary absorbed and reflected values */
	/* update temporary absorbed and reflected PAR for lower layer */
	logger(g_debug_log,"\ntemporary cumulated absorbed and reflect light\n");
	temp_apar       += s->value[APAR];
	c->apar         += s->value[APAR];
	logger(g_debug_log,"cum apar = %g\n", c->apar);
	temp_par_refl   += s->value[PAR_REFL];
	c->par_refl     += s->value[PAR_REFL];
	logger(g_debug_log,"cum par_refl = %g\n", c->par_refl);

	/* update temporary absorbed and transmitted Short Wave radiation lower layer */
	temp_sw_rad_abs  += s->value[SW_RAD_ABS];
	c->sw_rad_abs    += s->value[SW_RAD_ABS];
	logger(g_debug_log,"cum sw_rad_abs = %g\n", c->sw_rad_abs);
	temp_sw_rad_refl += s->value[SW_RAD_REFL];
	c->sw_rad_refl   += s->value[SW_RAD_REFL];
	logger(g_debug_log,"cum sw_rad_refl = %g\n", c->sw_rad_refl);

	/* update temporary absorbed and transmitted PPFD lower layer */
	temp_ppfd_abs    += s->value[PPFD_ABS];
	c->ppfd_abs      += s->value[PPFD_ABS];
	logger(g_debug_log,"cum ppfd_abs = %g\n", c->ppfd_abs);
	temp_ppfd_refl   += s->value[PPFD_REFL];
	c->ppfd_refl     += s->value[PPFD_REFL];
	logger(g_debug_log,"cum ppfd_refl = %g\n", c->ppfd_refl);

	/*****************************************************************************************************************/
	/* when matches the last height class in the layer is processed */
	if ( l->layer_n_height_class == layer_height_class_counter )
	{
		logger(g_debug_log,"\n************************************\n");
		logger(g_debug_log,"last height class in layer processed\n");
		logger(g_debug_log,"update radiation values for lower layer\n");
		/* compute values for lower layer when last height class in layer is processed */
		/* compute par for lower layer */
		meteo_daily->par           -= (temp_apar + temp_par_refl);

		/* compute Short Wave radiation for lower layesr */
		meteo_daily->sw_downward_W -= (temp_sw_rad_abs + temp_sw_rad_refl);

		/* compute ppfd for lower layer */
		meteo_daily->ppfd          -= (temp_ppfd_abs + temp_ppfd_refl);

		logger(g_debug_log, "Radiation for lower layers\n");
		logger(g_debug_log, "PAR = %g molPAR/m2/day\n", meteo_daily->par);
		logger(g_debug_log, "Short Wave = %g W/m2\n", meteo_daily->sw_downward_W);
		logger(g_debug_log, "PPFD = %g umol/m2/sec\n", meteo_daily->ppfd);

		/* reset temporary values when the last height class in layer is processed */
		temp_apar        = 0.;
		temp_par_refl    = 0.;
		temp_sw_rad_abs  = 0.;
		temp_sw_rad_refl = 0.;
		temp_ppfd_abs    = 0.;
		temp_ppfd_refl   = 0.;

		/* reset counter */
		layer_height_class_counter = 0;
	}

	/*************************************************************************/
	/* when matches the last height class in the cell is processed */
	//fixme fixme fixme fixme fixme sometimes it doesn't go in  caused by the a jump in "cell_height_class_counter"
	//as it is now is used just for print data but it should be fixed
	if ( c->heights_count == cell_height_class_counter )
	{
		logger(g_debug_log,"\n************************************\n");
		logger(g_debug_log, "last height class in cell processed\n");
		logger(g_debug_log, "Radiation for soil\n");
		logger(g_debug_log, "PAR = %g molPAR/m2/day\n", meteo_daily->par);
		logger(g_debug_log, "Short Wave = %g W/m2\n", meteo_daily->sw_downward_W);
		logger(g_debug_log, "PPFD = %g umol/m2/sec\n", meteo_daily->ppfd);
		logger(g_debug_log,"\n***********************************\n");
	}
	/*****************************************************************************************************************/
}
