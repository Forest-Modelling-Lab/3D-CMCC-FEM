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
	double MR_ref = 0.218;     /* Reference MR respiration linear N relationship with MR being kgC/kgN/day, 0.218 from Ryan 1991, 0.1584 Campioli et al., 2013 and from Dufrene et al 2005 */
	double Q10_temp = 20.0;    /* t_base temperature for respiration, 15°C for Damesin et al., 2001 */

	double q10_tavg = 2.0;     /* fractional change in rate with a T 10 °C increase in temperature  2.2 from Schwalm & Ek, 2004; Kimball et al., 1997 */
	double q10_tday;
	double q10_tnight;
	double q10_tsoil;

	double exponent_tday;
	double exponent_tnight;
	double exponent_tavg;
	double exponent_tsoil;

	/* nitrogen pools in gN/m2 */
	double leaf_N;
	double leaf_falling_N;
	double fine_root_N;
	double coarse_root_N;
	double stem_N;
	double branch_N;

	species_t *s;
	s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**MAINTENANCE_RESPIRATION**\n");

	/** maintenance respiration routine **/

	/* Uses reference values at 20 deg C and an empirical relationship between
	tissue N content and respiration rate given in:

	Ryan, M.G., 1991. Effects of climate change on plant respiration.
	Ecological Applications, 1(2):157-167.

	Uses the same value of Q_10 (2.0) for all compartments, leaf, stem,
	coarse and fine roots.

	From Ryan's figures and regressions equations, the maintenance respiration
	in kgC/day per kg of tissue N is:
	MR_ref = 0.218 (kgC/kgN/d)
	 */

	//totest using weighted average Temperature of the current days and the four days before

	if ( g_settings->Q10_fixed )
	{
		q10_tday = q10_tavg;
		q10_tnight = q10_tavg;
		q10_tsoil  = q10_tavg;
	}
	else
	{
		/* if used recompute q10_tavg based on:
		 * McGuire et al., 1992, Global Biogeochemical Cycles
		 * Tjoelker et al., 2001, Global Change Biology
		 * Smith and Dukes, 2013, Global Change Biology
		 * */

		q10_tday =  3.22 - 0.046 * meteo_daily->tday;
		q10_tnight =  3.22 - 0.046 * meteo_daily->tnight;

		//note: for stem, branch, fine and coarse root we used the 5 days weighted average
#if 1
		q10_tavg = 3.22 - 0.046 * meteo_daily->tavg;

		q10_tsoil  =  3.22 - 0.046 * meteo_daily->tsoil;
#else
		q10_tavg = 3.22 - 0.046 * meteo_daily->five_day_tavg;

		q10_tsoil  =  3.22 - 0.046 * meteo_daily->five_day_tsoil;
#endif
	}

	/* Nitrogen content tN/area --> gN/m2 */
	leaf_N = (s->value[LEAF_N] * 1000000.0 /g_settings->sizeCell);
	leaf_falling_N = (s->value[LEAF_FALLING_N] * 1000000.0 /g_settings->sizeCell);
	fine_root_N = (s->value[FINE_ROOT_N] * 1000000.0 /g_settings->sizeCell);
	coarse_root_N = (s->value[COARSE_ROOT_N] * 1000000.0 /g_settings->sizeCell);
	stem_N = (s->value[STEM_N] * 1000000.0 /g_settings->sizeCell);
	branch_N = (s->value[BRANCH_N] * 1000000.0 /g_settings->sizeCell);

	/* compute exponents */
	/* exponent for day time temperature */
	exponent_tday = (meteo_daily->tday - Q10_temp) / 10.0;

	/* exponent for night time temperature */
	exponent_tnight = (meteo_daily->tnight - Q10_temp) / 10.0;

	/*******************************************************************************************************************/
	//note: for stem, branch, fine and coarse root we used the 5 days weighted average
#if 1
	/* exponent for daily average temperature */
	exponent_tavg = (meteo_daily->tavg - Q10_temp) / 10.0;

	/* exponent for soil temperature */
	exponent_tsoil = (meteo_daily->tsoil - Q10_temp) / 10.0;
#else
	/* exponent for daily average temperature */
	exponent_tavg = (meteo_daily->five_day_tavg - Q10_temp) / 10.0;

	/* exponent for soil temperature */
	exponent_tsoil = (meteo_daily->five_day_tsoil - Q10_temp) / 10.0;
#endif


	/* note: values are computed in gC/m2/day */

	/*******************************************************************************************************************/
	/* Leaf maintenance respiration is calculated separately for day and night */

	/* day time leaf maintenance respiration */
	s->value[DAILY_LEAF_MAINT_RESP] = ( (leaf_N + leaf_falling_N) * MR_ref * pow(q10_tday, exponent_tday) * (meteo_daily->daylength/24.0));
	logger(g_debug_log, "daily leaf maintenance respiration = %g gC/m2/day\n", s->value[DAILY_LEAF_MAINT_RESP]);

	/* night time leaf maintenance respiration */
	s->value[NIGHTLY_LEAF_MAINT_RESP] = ( (leaf_N + leaf_falling_N) * MR_ref * pow(q10_tnight, exponent_tnight) * (1.0 - (meteo_daily->daylength/24.0)));
	logger(g_debug_log, "nightly leaf maintenance respiration = %g gC/m2/day\n", s->value[NIGHTLY_LEAF_MAINT_RESP]);

	/* total (all day) leaf maintenance respiration */
	s->value[TOT_DAY_LEAF_MAINT_RESP] = s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];
	logger(g_debug_log, "daily total leaf maintenance respiration = %g gC/m2/day\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* fine roots maintenance respiration */

	s->value[FINE_ROOT_MAINT_RESP] = fine_root_N * MR_ref * pow(q10_tsoil, exponent_tsoil);
	logger(g_debug_log, "daily fine root maintenance respiration = %g gC/m2/day\n", s->value[FINE_ROOT_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* live stem maintenance respiration */

	s->value[STEM_MAINT_RESP] = stem_N * MR_ref * pow(q10_tavg, exponent_tavg);
	logger(g_debug_log, "daily stem maintenance respiration = %g gC/m2/day\n", s->value[STEM_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* live branch maintenance respiration */

	s->value[BRANCH_MAINT_RESP] = branch_N * MR_ref * pow(q10_tavg, exponent_tavg);
	logger(g_debug_log, "daily branch maintenance respiration = %g gC/m2/day\n", s->value[BRANCH_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* live coarse root maintenance respiration */

	s->value[COARSE_ROOT_MAINT_RESP] = coarse_root_N * MR_ref * pow(q10_tsoil, exponent_tsoil);
	logger(g_debug_log, "daily coarse root maintenance respiration = %g gC/m2/day\n", s->value[COARSE_ROOT_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* COMPUTE TOTAL MAINTENANCE RESPIRATION */
	s->value[TOTAL_MAINT_RESP]= s->value[TOT_DAY_LEAF_MAINT_RESP]+
			s->value[FINE_ROOT_MAINT_RESP]+
			s->value[STEM_MAINT_RESP]+
			s->value[COARSE_ROOT_MAINT_RESP]+
			s->value[BRANCH_MAINT_RESP];
	logger(g_debug_log, "daily total maintenance respiration = %g gC/m2/day\n", s->value[TOTAL_MAINT_RESP]);


	/*******************************************************************************************************************/
	/* it converts value of GPP gC/m2/day in gC/m2 area covered/day */
	c->daily_leaf_maint_resp += s->value[TOT_DAY_LEAF_MAINT_RESP];
	c->daily_stem_maint_resp += s->value[STEM_MAINT_RESP];
	c->daily_fine_root_maint_resp += s->value[FINE_ROOT_MAINT_RESP];
	c->daily_branch_maint_resp += s->value[BRANCH_MAINT_RESP];
	c->daily_coarse_root_maint_resp += s->value[COARSE_ROOT_MAINT_RESP];
	c->daily_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->monthly_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->annual_maint_resp += s->value[TOTAL_MAINT_RESP];

	CHECK_CONDITION(s->value[TOTAL_MAINT_RESP], < 0);
}

void growth_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double min_grperc = 0.25;  /* minimum fraction of growth respiration at maximum age see Ryan et al., */
	double max_grperc = 0.35;  /* minimum fraction of growth respiration at maximum age see Ryan et al., */
	double eff_grperc;         /* effective growth respiration fraction */

	int min_age;               /* minimum age for max growth respiration fraction */
	int max_age;               /* maximum age for min growth respiration fraction */

	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	min_age = 1;
	max_age = (int)s->value[MAXAGE];

	logger(g_debug_log, "\n**GROWTH_RESPIRATION**\n");

	/*******************************************************************************************************/
	/* age-dependant growth respiration fraction */
	/* see Waring and Running 1998, "Forest Ecosystem - Analysis at Multiple Scales"
	 * see Ryan 1991, Ecological Applications
	 * see Ryan 1991, Tree Physiology*/

	eff_grperc = (min_grperc - max_grperc) / (max_age - min_age) * (a->value - min_age) + max_grperc;
	if( a->value > s->value[MAXAGE]) eff_grperc = min_grperc;

	logger(g_debug_log, "GRPERC based on age (test) = %g \n", eff_grperc);

	/*******************************************************************************************************/

	/* note: values of C increments are referred to the C increments of the day before */

	/* to avoid negative values during retranslocation */
	/* values are computed in gC/m2/day */
	if ( s->value[C_TO_LEAF] > 0)
	{
		/* leaf growth respiration */
		s->value[LEAF_GROWTH_RESP] = (s->value[C_TO_LEAF] * 1000000.0/g_settings->sizeCell) * eff_grperc;
	}
	if ( s->value[C_TO_FINEROOT] > 0)
	{
		/* fine root growth respiration */
		s->value[FINE_ROOT_GROWTH_RESP] = (s->value[C_TO_FINEROOT] *1000000.0/(g_settings->sizeCell))* eff_grperc;
	}
	if ( s->value[C_TO_STEM] > 0)
	{
		/* stem growth respiration */
		s->value[STEM_GROWTH_RESP] = (s->value[C_TO_STEM] * 1000000.0/(g_settings->sizeCell))* eff_grperc;
	}
	if ( s->value[C_TO_COARSEROOT] > 0)
	{
		/* coarse root respiration */
		s->value[COARSE_ROOT_GROWTH_RESP] = (s->value[C_TO_COARSEROOT] * 1000000.0/(g_settings->sizeCell))* eff_grperc;
	}
	if ( s->value[C_TO_BRANCH] > 0)
	{
		/* branch and bark growth respiration */
		s->value[BRANCH_GROWTH_RESP] = (s->value[C_TO_BRANCH] * 1000000.0/(g_settings->sizeCell))* eff_grperc;
	}


	/* COMPUTE TOTAL GROWTH RESPIRATION */
	s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] +
			s->value[FINE_ROOT_GROWTH_RESP] +
			s->value[STEM_GROWTH_RESP] +
			s->value[COARSE_ROOT_GROWTH_RESP] +
			s->value[BRANCH_GROWTH_RESP];

	logger(g_debug_log, "daily leaf growth respiration = %g gC/m2/day\n", s->value[LEAF_GROWTH_RESP]);
	logger(g_debug_log, "daily fine root growth respiration = %g gC/m2/day\n", s->value[FINE_ROOT_GROWTH_RESP]);
	logger(g_debug_log, "daily stem growth respiration = %g gC/m2/day\n", s->value[STEM_GROWTH_RESP]);
	logger(g_debug_log, "daily coarse root growth respiration = %g gC/m2/day\n", s->value[COARSE_ROOT_GROWTH_RESP]);
	logger(g_debug_log, "daily branch growth respiration = %g gC/m2/day\n", s->value[BRANCH_GROWTH_RESP]);
	logger(g_debug_log, "daily total growth respiration = %g gC/m2/day\n", s->value[TOTAL_GROWTH_RESP]);

	c->daily_leaf_growth_resp += s->value[LEAF_GROWTH_RESP];
	c->daily_stem_growth_resp += s->value[STEM_GROWTH_RESP];
	c->daily_fine_root_growth_resp += s->value[FINE_ROOT_GROWTH_RESP];
	c->daily_branch_growth_resp += s->value[BRANCH_GROWTH_RESP];
	c->daily_coarse_root_growth_resp += s->value[COARSE_ROOT_GROWTH_RESP];

	c->daily_growth_resp += s->value[TOTAL_GROWTH_RESP];
	c->monthly_growth_resp += s->value[TOTAL_GROWTH_RESP];
	c->annual_growth_resp += s->value[TOTAL_GROWTH_RESP];

	/* check */
	CHECK_CONDITION(s->value[TOTAL_GROWTH_RESP], < 0);

	/* reset previous day carbon increments among pools */
	/* note: THESE VARIABLES MUST BE RESETTED HERE!!! */
	s->value[C_TO_LEAF] = 0.;
	s->value[C_TO_ROOT] = 0.;
	s->value[C_TO_FINEROOT] = 0.;
	s->value[C_TO_COARSEROOT] = 0.;
	s->value[C_TO_TOT_STEM] = 0.;
	s->value[C_TO_STEM] = 0.;
	s->value[C_TO_BRANCH] = 0.;
	s->value[C_TO_RESERVE] = 0.;
	s->value[C_TO_FRUIT] = 0.;
	s->value[C_TO_LITTER] = 0.;
	s->value[C_LEAF_TO_RESERVE] = 0.;
	s->value[C_FINEROOT_TO_RESERVE] = 0.;
}

void autotrophic_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	if ( g_settings->Prog_Aut_Resp )
	{
		/* maintenance respiration */
		maintenance_respiration( c, layer, height, dbh, age, species, meteo_daily );

		/* growth respiration */
		growth_respiration( c, layer, height, dbh, age, species );

		logger(g_debug_log, "\n**AUTOTROPHIC_RESPIRATION**\n");

		/* class level among pools */
		s->value[LEAF_AUT_RESP] = s->value[TOT_DAY_LEAF_MAINT_RESP] + s->value[LEAF_GROWTH_RESP];
		logger(g_debug_log, "daily leaf autotrophic respiration = %g gC/m2/day\n", s->value[BRANCH_GROWTH_RESP]);

		s->value[STEM_AUT_RESP] = s->value[STEM_MAINT_RESP] + s->value[STEM_GROWTH_RESP];
		logger(g_debug_log, "daily stem autotrophic respiration = %g gC/m2/day\n", s->value[STEM_AUT_RESP]);

		s->value[BRANCH_AUT_RESP] = s->value[BRANCH_MAINT_RESP] + s->value[BRANCH_GROWTH_RESP];
		logger(g_debug_log, "daily branch autotrophic respiration = %g gC/m2/day\n", s->value[BRANCH_AUT_RESP]);

		s->value[FINE_ROOT_AUT_RESP] = s->value[FINE_ROOT_MAINT_RESP] + s->value[FINE_ROOT_GROWTH_RESP];
		logger(g_debug_log, "daily fine root autotrophic respiration = %g gC/m2/day\n", s->value[FINE_ROOT_AUT_RESP]);

		s->value[COARSE_ROOT_AUT_RESP] = s->value[COARSE_ROOT_MAINT_RESP] + s->value[COARSE_ROOT_GROWTH_RESP];
		logger(g_debug_log, "daily coarse root autotrophic respiration = %g gC/m2/day\n", s->value[COARSE_ROOT_AUT_RESP]);

		/* COMPUTE TOTAL A RESPIRATION */
		s->value[TOTAL_AUT_RESP] = s->value[TOTAL_GROWTH_RESP] + s->value[TOTAL_MAINT_RESP];

		/***************************************************************************************/

		/* cell level among pools */
		c->daily_leaf_aut_resp += s->value[TOT_DAY_LEAF_MAINT_RESP] + s->value[LEAF_GROWTH_RESP];
		c->daily_stem_aut_resp += s->value[STEM_MAINT_RESP] + s->value[STEM_GROWTH_RESP];
		c->daily_branch_aut_resp += s->value[BRANCH_MAINT_RESP] + s->value[BRANCH_GROWTH_RESP];
		c->daily_fine_root_aut_resp += s->value[FINE_ROOT_MAINT_RESP] + s->value[FINE_ROOT_GROWTH_RESP];
		c->daily_coarse_root_aut_resp += s->value[COARSE_ROOT_MAINT_RESP] + s->value[COARSE_ROOT_GROWTH_RESP];
	}
	else
	{
		/* COMPUTE TOTAL A RESPIRATION (using fixed ratio)*/
		s->value[TOTAL_AUT_RESP] = s->value[DAILY_GPP_gC] * g_settings->Fixed_Aut_Resp_rate;
	}

	logger(g_debug_log, "daily total autotrophic respiration = %g gC/m2/day\n", s->value[TOTAL_AUT_RESP]);
	logger(g_debug_log, "daily total autotrophic respiration = %g tC/cell/day \n", (s->value[TOTAL_AUT_RESP] /1000000.0) * g_settings->sizeCell);

	s->value[MONTHLY_LEAF_AUT_RESP] += s->value[LEAF_AUT_RESP];
	s->value[MONTHLY_FINE_ROOT_AUT_RESP] += s->value[FINE_ROOT_AUT_RESP];
	s->value[MONTHLY_STEM_AUT_RESP] += s->value[STEM_AUT_RESP];
	s->value[MONTHLY_COARSE_ROOT_AUT_RESP] += s->value[COARSE_ROOT_AUT_RESP];
	s->value[MONTHLY_BRANCH_AUT_RESP] += s->value[BRANCH_AUT_RESP];
	s->value[MONTHLY_TOTAL_AUT_RESP] += s->value[TOTAL_AUT_RESP];

	s->value[YEARLY_LEAF_AUT_RESP] += s->value[LEAF_AUT_RESP];
	s->value[YEARLY_FINE_ROOT_AUT_RESP] += s->value[FINE_ROOT_AUT_RESP];
	s->value[YEARLY_STEM_AUT_RESP] += s->value[STEM_AUT_RESP];
	s->value[YEARLY_COARSE_ROOT_AUT_RESP] += s->value[COARSE_ROOT_AUT_RESP];
	s->value[YEARLY_BRANCH_AUT_RESP] += s->value[BRANCH_AUT_RESP];
	s->value[YEARLY_TOTAL_AUT_RESP] += s->value[TOTAL_AUT_RESP];

	c->daily_aut_resp += s->value[TOTAL_AUT_RESP];
	c->daily_aut_resp_tC +=  s->value[TOTAL_AUT_RESP] / 1000000 * g_settings->sizeCell;
	c->monthly_aut_resp += s->value[TOTAL_AUT_RESP];
	c->monthly_aut_resp_tC +=  s->value[TOTAL_AUT_RESP] / 1000000 * g_settings->sizeCell;
	c->annual_aut_resp += s->value[TOTAL_AUT_RESP];
	c->annual_aut_resp_tC += s->value[TOTAL_AUT_RESP] / 1000000 * g_settings->sizeCell;

	/* check */
	CHECK_CONDITION(s->value[TOTAL_AUT_RESP], < 0);
}




