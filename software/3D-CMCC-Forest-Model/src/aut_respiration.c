/*
- * autotrophic_respiration.c
 *
 *  Created on: 25/set/2013
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aut_respiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void maintenance_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	double MR_ref = 0.218;          /* Reference MR respiration linear N relationship with MR being kgC/kgN/day, 0.218 from Ryan 1991, 0.1584 Campioli et al., 2013 and from Dufrene et al 2005 */

	//NOTE: Atkin et al. 2008 and Cox et al., reports 25 °C for both
	double Q10_temp = 20.0;                /* T_base temperature for respiration, 15°C for Damesin et al., 2001, 20°C Thornton */
	//double Q10_temp_accl = 20.0; //25;   /* T_base temperature for acclimation in respiration (25°C) Atkin et al., 2008 GCB, Cox et al., 2000 Nature */

	double q10_tavg = 2.0;                 /* fractional change in rate with a T 10 °C increase in temperature: 2.2 from Schwalm & Ek, 2004; Kimball et al., 1997, 1.5 Mahecha Science, 2010 */
	double q10_tday;
	double q10_tnight;
	double q10_tsoil;

	double acc_const = -0.00703;           /* temperature correction factor for acclimation -0.00703 Atkin et al., 2008 GCB, -0.00794 Smith & Dukes 2012; 0.0078 Hidy et al., 2016 GMD */

	/* exponent for Temperature */
	double exponent_tday;
	double exponent_tnight;
	double exponent_tavg;
	double exponent_tsoil;

	/* nitrogen pools in gN/m2 */
	double leaf_N;
	double froot_N;
	double croot_N;
	double stem_N;
	double branch_N;

	species_t *s;
	s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**MAINTENANCE_RESPIRATION**\n");

	/** maintenance respiration routine **/

	/* Uses reference values at 20 deg C (Q10_temp) and an empirical relationship between
	tissue N content and respiration rate given in:

	Ryan, M.G., 1991. Effects of climate change on plant respiration.
	Ecological Applications, 1(2):157-167.

	Uses the same value of Q_10 (2.0) for all compartments, leaf, stem,
	coarse and fine roots.

	From Ryan's figures and regressions equations, the maintenance respiration
	in kgC/day per kg of tissue N is:
	MR_ref = 0.218 (kgC/kgN/d)
	 */
	/*** temperature dependent changes in Q10 function ***/
	/* based on:
	 * McGuire et al., 1992, Global Biogeochemical Cycles
	 * Tjoelker et al., 2001, Global Change Biology
	 * Smith and Dukes, 2013, Global Change Biology
	 * */

	/*** NOTE: Type I acclimation (Atkin & Tjoelker 2003; Atkin et al., 2005) ***/
	/* Q10 dependent changes on temperature */
	q10_tday        = 3.22 - 0.046 * meteo_daily->tday;
	q10_tnight      = 3.22 - 0.046 * meteo_daily->tnight;
	q10_tavg        = 3.22 - 0.046 * meteo_daily->tavg;
	q10_tsoil       = 3.22 - 0.046 * meteo_daily->tsoil;

	/* compute exponents */
	exponent_tday   = (meteo_daily->tday - Q10_temp)   / 10.0;
	exponent_tnight = (meteo_daily->tnight - Q10_temp) / 10.0;
	exponent_tavg   = (meteo_daily->tavg - Q10_temp)   / 10.0;
	exponent_tsoil  = (meteo_daily->tsoil - Q10_temp)  / 10.0;

	/* Nitrogen content tN/area --> gN/m2 */
	leaf_N          = (s->value[LEAF_N]             * 1e6 /g_settings->sizeCell);
	froot_N         = (s->value[FROOT_N]            * 1e6 /g_settings->sizeCell);
	stem_N          = (s->value[STEM_LIVE_WOOD_N]   * 1e6 /g_settings->sizeCell);
	croot_N         = (s->value[CROOT_LIVE_WOOD_N]  * 1e6 /g_settings->sizeCell);
	branch_N        = (s->value[BRANCH_LIVE_WOOD_N] * 1e6 /g_settings->sizeCell);

	/* note: values are computed in gC/m2/day */

	/*******************************************************************************************************************/
	/* Leaf maintenance respiration is calculated separately for day and night */

	/* day time leaf maintenance respiration */
	s->value[DAILY_LEAF_MAINT_RESP]   = ( leaf_N * MR_ref * pow(q10_tday, exponent_tday) * (meteo_daily->daylength / 24.));

	/* night time leaf maintenance respiration */
	s->value[NIGHTLY_LEAF_MAINT_RESP] = ( leaf_N * MR_ref * pow(q10_tnight, exponent_tnight) * (1. - (meteo_daily->daylength/24.)));

	/* total (all day) leaf maintenance respiration */
	s->value[TOT_DAY_LEAF_MAINT_RESP] = s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];

	/*******************************************************************************************************************/
	/* fine roots maintenance respiration */

	s->value[FROOT_MAINT_RESP]  = froot_N * MR_ref * pow(q10_tsoil, exponent_tsoil);

	/*******************************************************************************************************************/
	/* live coarse root maintenance respiration */

	s->value[CROOT_MAINT_RESP]  = croot_N * MR_ref * pow(q10_tsoil, exponent_tsoil);

	/*******************************************************************************************************************/
	/* live stem maintenance respiration */

	s->value[STEM_MAINT_RESP]   = stem_N * MR_ref * pow(q10_tavg, exponent_tavg);

	/*******************************************************************************************************************/
	/* live branch maintenance respiration */

	s->value[BRANCH_MAINT_RESP] = branch_N * MR_ref * pow(q10_tavg, exponent_tavg);

	/*******************************************************************************************************************/

	/* if acclimation for autotrophic respiration = "on" */
	if ( g_settings->Resp_accl )
	{
		/*** NOTE: Type II acclimation (Atkin & Tjoelker 2003; Atkin et al., 2005, Smith and Dukes, 2013) ***/
		/* FOLLOWING Atkin et al., 2008 "Using temperature-dependent changes in leaf scaling relationships
		 * to quantitatively account for thermal acclimation of respiration in a coupled global climate–vegetation model",
		 * 14, 1–18, GCB */

		/* Leaf maintenance respiration is calculated separately for day and night */

		/* day time leaf maintenance respiration */
		s->value[DAILY_LEAF_MAINT_RESP]   *= pow ( 10., ( acc_const * ( meteo_daily->ten_day_avg_tday - Q10_temp ) ) );

		/* night time leaf maintenance respiration */
		s->value[NIGHTLY_LEAF_MAINT_RESP] *= pow ( 10., ( acc_const * ( meteo_daily->ten_day_avg_tnight - Q10_temp ) ) );

		/* total (all day) leaf maintenance respiration */
		s->value[TOT_DAY_LEAF_MAINT_RESP]  = s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];

		/*******************************************************************************************************************/
		/* fine roots maintenance respiration */

		s->value[FROOT_MAINT_RESP]        *= pow ( 10., ( acc_const * ( meteo_daily->ten_day_avg_tsoil - Q10_temp ) ) );

		/*******************************************************************************************************************/
		/* live coarse root maintenance respiration */

		s->value[CROOT_MAINT_RESP]        *= pow (10., ( acc_const * ( meteo_daily->ten_day_avg_tsoil - Q10_temp ) ) );

		/*******************************************************************************************************************/
		/* live stem maintenance respiration */

		s->value[STEM_MAINT_RESP]         *= pow ( 10., ( acc_const * ( meteo_daily->ten_day_avg_tavg - Q10_temp ) ) );

		/*******************************************************************************************************************/
		/* live branch maintenance respiration */

		s->value[BRANCH_MAINT_RESP] *= pow ( 10., ( acc_const * ( meteo_daily->ten_day_avg_tavg - Q10_temp ) ) );

		/*******************************************************************************************************************/
	}

	/* COMPUTE TOTAL MAINTENANCE RESPIRATION */
	s->value[TOTAL_MAINT_RESP] = s->value[TOT_DAY_LEAF_MAINT_RESP]+
			s->value[FROOT_MAINT_RESP]+
			s->value[STEM_MAINT_RESP]+
			s->value[CROOT_MAINT_RESP]+
			s->value[BRANCH_MAINT_RESP];
	s->value[TOTAL_MAINT_RESP_tC] = s->value[TOTAL_MAINT_RESP] / 1e6 * g_settings->sizeCell ;

	logger(g_debug_log, "daily total maintenance respiration = %g gC/m2/day\n", s->value[TOTAL_MAINT_RESP]);

	/* cumulate */
	s->value[MONTHLY_TOTAL_MAINT_RESP] += s->value[TOTAL_MAINT_RESP];
	s->value[YEARLY_TOTAL_MAINT_RESP]  += s->value[TOTAL_MAINT_RESP];

	/*******************************************************************************************************************/
	/* cell level */
	c->daily_leaf_maint_resp           += s->value[TOT_DAY_LEAF_MAINT_RESP];
	c->daily_stem_maint_resp           += s->value[STEM_MAINT_RESP];
	c->daily_froot_maint_resp          += s->value[FROOT_MAINT_RESP];
	c->daily_branch_maint_resp         += s->value[BRANCH_MAINT_RESP];
	c->daily_croot_maint_resp          += s->value[CROOT_MAINT_RESP];
	c->daily_maint_resp                += s->value[TOTAL_MAINT_RESP];
	c->monthly_maint_resp              += s->value[TOTAL_MAINT_RESP];
	c->annual_maint_resp               += s->value[TOTAL_MAINT_RESP];

	CHECK_CONDITION(s->value[TOTAL_MAINT_RESP], <, 0);
}

void growth_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**GROWTH_RESPIRATION**\n");

	/* compute growth respiration fraction */
	growth_respiration_frac ( a, s );

	/*******************************************************************************************************/

	/* values are computed in gC/m2/day */
	if ( s->value[C_TO_LEAF] > 0. )   s->value[LEAF_GROWTH_RESP]   = (s->value[C_TO_LEAF]   * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];
	if ( s->value[C_TO_FROOT] > 0. )  s->value[FROOT_GROWTH_RESP]  = (s->value[C_TO_FROOT]  * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];
	if ( s->value[C_TO_STEM] > 0. )   s->value[STEM_GROWTH_RESP]   = (s->value[C_TO_STEM]   * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];
	if ( s->value[C_TO_CROOT] > 0. )  s->value[CROOT_GROWTH_RESP]  = (s->value[C_TO_CROOT]  * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];
	if ( s->value[C_TO_BRANCH] > 0. ) s->value[BRANCH_GROWTH_RESP] = (s->value[C_TO_BRANCH] * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];

	/* COMPUTE TOTAL GROWTH RESPIRATION */
	s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] +
			s->value[FROOT_GROWTH_RESP] +
			s->value[STEM_GROWTH_RESP] +
			s->value[CROOT_GROWTH_RESP] +
			s->value[BRANCH_GROWTH_RESP];
	s->value[TOTAL_GROWTH_RESP_tC] = s->value[TOTAL_GROWTH_RESP] / 1e6 * g_settings->sizeCell ;

	logger(g_debug_log, "daily total growth respiration = %g gC/m2/day\n", s->value[TOTAL_GROWTH_RESP]);

	/* cumulate */
	s->value[MONTHLY_TOTAL_GROWTH_RESP] += s->value[TOTAL_GROWTH_RESP];
	s->value[YEARLY_TOTAL_GROWTH_RESP]  += s->value[TOTAL_GROWTH_RESP];

	/* pools */
	c->daily_leaf_growth_resp           += s->value[LEAF_GROWTH_RESP];
	c->daily_stem_growth_resp           += s->value[STEM_GROWTH_RESP];
	c->daily_froot_growth_resp          += s->value[FROOT_GROWTH_RESP];
	c->daily_branch_growth_resp         += s->value[BRANCH_GROWTH_RESP];
	c->daily_croot_growth_resp          += s->value[CROOT_GROWTH_RESP];

	/* total */
	c->daily_growth_resp                += s->value[TOTAL_GROWTH_RESP];
	c->monthly_growth_resp              += s->value[TOTAL_GROWTH_RESP];
	c->annual_growth_resp               += s->value[TOTAL_GROWTH_RESP];

	/* check */
	CHECK_CONDITION(s->value[TOTAL_GROWTH_RESP], <, 0);

	/* reset previous day carbon increments among pools */
	/* note: THESE VARIABLES MUST BE RESETTED HERE!!! */
	//	s->value[C_TO_LEAF]             = 0.;
	//	s->value[C_TO_ROOT]             = 0.;
	//	s->value[C_TO_FROOT]            = 0.;
	//	s->value[C_TO_CROOT]            = 0.;
	//	s->value[C_TO_TOT_STEM]         = 0.;
	//	s->value[C_TO_STEM]             = 0.;
	//	s->value[C_TO_BRANCH]           = 0.;
	//	s->value[C_TO_RESERVE]          = 0.;
	//	s->value[C_TO_FRUIT]            = 0.;
	//	s->value[C_LEAF_TO_LITR]        = 0.;
	//	s->value[C_FROOT_TO_LITR]       = 0.;
	//	s->value[C_LEAF_TO_RESERVE]     = 0.;
	//	s->value[C_FROOT_TO_RESERVE]    = 0.;
}

void autotrophic_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	if ( g_settings->Prog_Aut_Resp )
	{

		logger(g_debug_log, "\n**AUTOTROPHIC_RESPIRATION**\n");

		/* class level among pools */
		s->value[LEAF_AUT_RESP]     = s->value[TOT_DAY_LEAF_MAINT_RESP] + s->value[LEAF_GROWTH_RESP];
		s->value[STEM_AUT_RESP]     = s->value[STEM_MAINT_RESP] + s->value[STEM_GROWTH_RESP];
		s->value[BRANCH_AUT_RESP]   = s->value[BRANCH_MAINT_RESP] + s->value[BRANCH_GROWTH_RESP];
		s->value[FROOT_AUT_RESP]    = s->value[FROOT_MAINT_RESP] + s->value[FROOT_GROWTH_RESP];
		s->value[CROOT_AUT_RESP]    = s->value[CROOT_MAINT_RESP] + s->value[CROOT_GROWTH_RESP];

		/* total */
		s->value[TOTAL_AUT_RESP]    = s->value[TOTAL_GROWTH_RESP] + s->value[TOTAL_MAINT_RESP];
		s->value[TOTAL_AUT_RESP_tC] = s->value[TOTAL_AUT_RESP] / 1e6 * g_settings->sizeCell ;

		/***************************************************************************************/

		/* cell level among pools */
		c->daily_leaf_aut_resp     += s->value[TOT_DAY_LEAF_MAINT_RESP] + s->value[LEAF_GROWTH_RESP];
		c->daily_stem_aut_resp     += s->value[STEM_MAINT_RESP] + s->value[STEM_GROWTH_RESP];
		c->daily_branch_aut_resp   += s->value[BRANCH_MAINT_RESP] + s->value[BRANCH_GROWTH_RESP];
		c->daily_froot_aut_resp    += s->value[FROOT_MAINT_RESP] + s->value[FROOT_GROWTH_RESP];
		c->daily_croot_aut_resp    += s->value[CROOT_MAINT_RESP] + s->value[CROOT_GROWTH_RESP];
	}
	else
	{
		/* COMPUTE TOTAL A RESPIRATION (using fixed ratio)*/
		s->value[TOTAL_AUT_RESP] = s->value[GPP] * g_settings->Fixed_Aut_Resp_rate;
	}

	logger(g_debug_log, "daily total autotrophic respiration = %g gC/m2/day\n", s->value[TOTAL_AUT_RESP]);

	/* monthly */
	s->value[MONTHLY_LEAF_AUT_RESP]        += s->value[LEAF_AUT_RESP];
	s->value[MONTHLY_FROOT_AUT_RESP]       += s->value[FROOT_AUT_RESP];
	s->value[MONTHLY_STEM_AUT_RESP]        += s->value[STEM_AUT_RESP];
	s->value[MONTHLY_CROOT_AUT_RESP]       += s->value[CROOT_AUT_RESP];
	s->value[MONTHLY_BRANCH_AUT_RESP]      += s->value[BRANCH_AUT_RESP];
	s->value[MONTHLY_TOTAL_AUT_RESP]       += s->value[TOTAL_AUT_RESP];

	/* annual */
	s->value[YEARLY_LEAF_AUT_RESP]         += s->value[LEAF_AUT_RESP];
	s->value[YEARLY_FROOT_AUT_RESP]        += s->value[FROOT_AUT_RESP];
	s->value[YEARLY_STEM_AUT_RESP]         += s->value[STEM_AUT_RESP];
	s->value[YEARLY_CROOT_AUT_RESP]        += s->value[CROOT_AUT_RESP];
	s->value[YEARLY_BRANCH_AUT_RESP]       += s->value[BRANCH_AUT_RESP];
	s->value[YEARLY_TOTAL_AUT_RESP]        += s->value[TOTAL_AUT_RESP];

	/* cell level */
	c->daily_aut_resp                      += s->value[TOTAL_AUT_RESP];
	c->daily_aut_resp_tC                   += s->value[TOTAL_AUT_RESP] / 1e6 * g_settings->sizeCell;
	c->monthly_aut_resp                    += s->value[TOTAL_AUT_RESP];
	c->monthly_aut_resp_tC                 += s->value[TOTAL_AUT_RESP] / 1e6 * g_settings->sizeCell;
	c->annual_aut_resp                     += s->value[TOTAL_AUT_RESP];
	c->annual_aut_resp_tC                  += s->value[TOTAL_AUT_RESP] / 1e6 * g_settings->sizeCell;

	/* check */
	CHECK_CONDITION(s->value[TOTAL_AUT_RESP], <, 0);
}

void growth_respiration_frac ( const age_t *const a, species_t *const s )
{

	double min_grperc = 0.25;  /* minimum fraction of growth respiration at maximum age see Ryan et al., */
	double max_grperc = 0.35;  /* minimum fraction of growth respiration at maximum age see Ryan et al., */

	int min_age;               /* minimum age for max growth respiration fraction */
	int max_age;               /* maximum age for min growth respiration fraction */

	min_age = 1;
	max_age = (int)s->value[MAXAGE];

	logger(g_debug_log, "\n**GROWTH_RESPIRATION FRACTION**\n");

	/* age-dependant growth respiration fraction */
	/* see Waring and Running 1998, "Forest Ecosystem - Analysis at Multiple Scales"
	 * see Ryan 1991, Ecological Applications
	 * see Ryan 1991, Tree Physiology*/

	s->value[EFF_GRPERC] = (min_grperc - max_grperc) / (max_age - min_age) * (a->value - min_age) + max_grperc;
	if( a->value > s->value[MAXAGE]) s->value[EFF_GRPERC] = min_grperc;
	logger(g_debug_log, "EFF_GRPERC = %g\n", s->value[EFF_GRPERC]);
}




