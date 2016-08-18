/*
 * maintenance_respiration.c
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
extern logger_t* g_log;

void maintenance_respiration(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	double t1;
	double q10 = 2.0;
	double mrpern = 0.218;     /* linear N relationship with MR being kgC/kgN/day, 0.218 from Ryan 1991, 0.1584 Campioli et al., 2013 and from Dufrene et al 2005 */
	double exponent_tday;
	double exponent_tnight;
	double exponent_tavg;
	double exponent_tsoil;
	double Q10_temp = 20.0;    /* t_base temperature for respiration, 15°C for Damesin et al., 2001 */

	/* nitrogen pools in gN/m2 */
	double leaf_N;
	double fine_root_N;
	double coarse_root_N;
	double stem_N;
	double branch_N;

	species_t *s;
	s  = &c->heights[height].ages[age].species[species];

	logger(g_log, "\n**MAINTENANCE_RESPIRATION**\n");

	//	//fixme in this case when LAI = 0 there's no respiration
	//		/* compute effective canopy cover */
	//		if(s->value[LAI] < 1.0)
	//		{
	//			/* special case when LAI = < 1.0 */
	//			leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	//		}
	//		else
	//		{
	//			leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];
	//		}
	//		/* check for the special case in which is allowed to have more 100% of grid cell covered */
	//		if(leaf_cell_cover_eff > 1.0)
	//		{
	//			leaf_cell_cover_eff = 1.0;
	//		}
	//
	//	leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* Nitrogen content tN/area --> gN/m2 */
	leaf_N = (s->value[LEAF_N] * 1000000.0 /g_settings->sizeCell);
	fine_root_N = (s->value[FINE_ROOT_N] * 1000000.0 /g_settings->sizeCell);
	coarse_root_N = (s->value[COARSE_ROOT_N] * 1000000.0 /g_settings->sizeCell);
	stem_N = (s->value[STEM_N] * 1000000.0 /g_settings->sizeCell);
	branch_N = (s->value[BRANCH_N] * 1000000.0 /g_settings->sizeCell);

	/* compute exponents */
	/* exponent for day time temperature */
	exponent_tday = (meteo_daily->tday - Q10_temp) / 10.0;

	/* exponent for night time temperature */
	exponent_tnight = (meteo_daily->tnight - Q10_temp) / 10.0;

	/* exponent for daily average temperature */
	exponent_tavg = (meteo_daily->tavg - Q10_temp) / 10.0;

	/* exponent for soil temperature */
	exponent_tsoil = (meteo_daily->tsoil - Q10_temp) / 10.0;

	/* maintenance respiration routine */

	/* Uses reference values at 20 deg C and an empirical relationship between
	tissue N content and respiration rate given in:

	Ryan, M.G., 1991. Effects of climate change on plant respiration.
	Ecological Applications, 1(2):157-167.

	Uses the same value of Q_10 (2.0) for all compartments, leaf, stem,
	coarse and fine roots.

	From Ryan's figures and regressions equations, the maintenance respiration
	in kgC/day per kg of tissue N is:
	mrpern = 0.218 (kgC/kgN/d)
	*/

	/* note: values are computed in gC/m2/day */

	/*******************************************************************************************************************/
	/* Leaf maintenance respiration is calculated separately for day and night */
	t1 = leaf_N * mrpern;

	/* day time leaf maintenance respiration */
	s->value[DAILY_LEAF_MAINT_RESP] = (t1 * pow(q10, exponent_tday) * (meteo_daily->daylength/24.0));
	logger(g_log, "daily leaf maintenance respiration = %g gC/m2/day\n", s->value[DAILY_LEAF_MAINT_RESP]);

	/* night time leaf maintenance respiration */
	s->value[NIGHTLY_LEAF_MAINT_RESP] = (t1 * pow(q10, exponent_tnight) * (1.0 - (meteo_daily->daylength/24.0)));
	logger(g_log, "nightly leaf maintenance respiration = %g gC/m2/day\n", s->value[NIGHTLY_LEAF_MAINT_RESP]);

	/* total (all day) leaf maintenance respiration */
	s->value[TOT_DAY_LEAF_MAINT_RESP]= s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];
	logger(g_log, "daily total leaf maintenance respiration = %g gC/m2/day\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* fine roots maintenance respiration */
	t1 = fine_root_N * mrpern;

	s->value[FINE_ROOT_MAINT_RESP] = t1 * pow(q10, exponent_tsoil);
	logger(g_log, "daily fine root maintenance respiration = %g gC/m2/day\n", s->value[FINE_ROOT_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* live stem maintenance respiration */
	t1 = stem_N * mrpern;

	s->value[STEM_MAINT_RESP] = t1 * pow(q10, exponent_tavg);
	logger(g_log, "daily stem maintenance respiration = %g gC/m2/day\n", s->value[STEM_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* live branch maintenance respiration */
	t1 = branch_N * mrpern;

	s->value[BRANCH_MAINT_RESP] = t1 * pow(q10, exponent_tavg);
	logger(g_log, "daily branch maintenance respiration = %g gC/m2/day\n", s->value[BRANCH_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* live coarse root maintenance respiration */
	t1 = coarse_root_N * mrpern;

	/* live coarse root maintenance respiration */
	s->value[COARSE_ROOT_MAINT_RESP] = t1 * pow(q10, exponent_tsoil);
	logger(g_log, "daily coarse root maintenance respiration = %g gC/m2/day\n", s->value[COARSE_ROOT_MAINT_RESP]);

	/*******************************************************************************************************************/
	/* COMPUTE TOTAL MAINTENANCE RESPIRATION */
	s->value[TOTAL_MAINT_RESP]= s->value[TOT_DAY_LEAF_MAINT_RESP]+
			s->value[FINE_ROOT_MAINT_RESP]+
			s->value[STEM_MAINT_RESP]+
			s->value[COARSE_ROOT_MAINT_RESP]+
			s->value[BRANCH_MAINT_RESP];
	logger(g_log, "daily total maintenance respiration = %g gC/m2/day\n", s->value[TOTAL_MAINT_RESP]);


	/*******************************************************************************************************************/
	/* it converts value of GPP gC/m2/day in gC/m2 area covered/day */
	//test 6 July 2016 removing
	//s->value[TOTAL_MAINT_RESP] *= leaf_cell_cover_eff;
	//logger(g_log, "TOTAL maintenance respiration = %g gC/m2 area covered/day\n", s->value[TOTAL_MAINT_RESP]);

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

void growth_respiration(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].ages[age].species[species];


	logger(g_log, "\n**GROWTH_RESPIRATION**\n");

	//fixme in this case when LAI = 0 there's no respiration
	//		/* compute effective canopy cover */
	//		if(s->value[LAI] < 1.0)
	//		{
	//			/* special case when LAI = < 1.0 */
	//			leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	//		}
	//		else
	//		{
	//			leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];
	//		}
	//		/* check for the special case in which is allowed to have more 100% of grid cell covered */
	//		if(leaf_cell_cover_eff > 1.0)
	//		{
	//			leaf_cell_cover_eff = 1.0;
	//		}

	/* values are computed in gC/m2/day */
	/* leaf growth respiration */
	s->value[LEAF_GROWTH_RESP] = (s->value[C_TO_LEAF] * 1000000.0/g_settings->sizeCell) * GRPERC;
	logger(g_log, "daily leaf growth respiration = %g gC/m2/day\n", s->value[LEAF_GROWTH_RESP]);

	/* fine root growth respiration */
	s->value[FINE_ROOT_GROWTH_RESP] = (s->value[C_TO_FINEROOT] *1000000.0/(g_settings->sizeCell))* GRPERC;
	logger(g_log, "daily fine root growth respiration = %g gC/m2/day\n", s->value[FINE_ROOT_GROWTH_RESP]);

	/* stem growth respiration */
	s->value[STEM_GROWTH_RESP] = (s->value[C_TO_STEM] * 1000000.0/(g_settings->sizeCell))* GRPERC;
	logger(g_log, "daily stem growth respiration = %g gC/m2/day\n", s->value[STEM_GROWTH_RESP]);

	/* coarse root respiration */
	s->value[COARSE_ROOT_GROWTH_RESP] = (s->value[C_TO_COARSEROOT] * 1000000.0/(g_settings->sizeCell))* GRPERC;
	logger(g_log, "daily coarse root growth respiration = %g gC/m2/day\n", s->value[COARSE_ROOT_GROWTH_RESP]);

	/* branch and bark growth respiration */
	s->value[BRANCH_GROWTH_RESP] = (s->value[C_TO_BRANCH] * 1000000.0/(g_settings->sizeCell))* GRPERC;
	logger(g_log, "daily branch growth respiration = %g gC/m2/day\n", s->value[BRANCH_GROWTH_RESP]);

	/* COMPUTE TOTAL GROWTH RESPIRATION */
	s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] +
			s->value[FINE_ROOT_GROWTH_RESP] +
			s->value[STEM_GROWTH_RESP] +
			s->value[COARSE_ROOT_GROWTH_RESP] +
			s->value[BRANCH_GROWTH_RESP];
	logger(g_log, "daily total growth respiration = %g gC/m2/day\n", s->value[TOTAL_GROWTH_RESP]);

	/* it converts value of GPP gC/m2/day in gC/m2 area covered/day */
	///test 6 July 2016 removing
	//s->value[TOTAL_GROWTH_RESP] *= leaf_cell_cover_eff;
	//logger(g_log, "TOTAL growth respiration = %g gC/m2 area covered/day\n", s->value[TOTAL_GROWTH_RESP]);

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
}

void autotrophic_respiration(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	logger(g_log, "\n**AUTOTROPHIC_RESPIRATION**\n");

	if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
	{
		/* COMPUTE TOTAL A RESPIRATION */
		s->value[TOTAL_AUT_RESP] = s->value[TOTAL_GROWTH_RESP] + s->value[TOTAL_MAINT_RESP];

		/* among pools */
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
	logger(g_log, "daily total autotrophic respiration = %g gC/m2/day\n", s->value[TOTAL_AUT_RESP]);
	logger(g_log, "daily total autotrophic respiration = %g tC/cell/day \n", (s->value[TOTAL_AUT_RESP] /1000000.0) * g_settings->sizeCell);

	c->daily_aut_resp += s->value[TOTAL_AUT_RESP];
	c->daily_aut_resp_tC +=  s->value[TOTAL_AUT_RESP] / 1000000 * g_settings->sizeCell;
	c->monthly_aut_resp += s->value[TOTAL_AUT_RESP];
	c->monthly_aut_resp_tC +=  s->value[TOTAL_AUT_RESP] / 1000000 * g_settings->sizeCell;
	c->annual_aut_resp += s->value[TOTAL_AUT_RESP];
	c->annual_aut_resp_tC += s->value[TOTAL_AUT_RESP] / 1000000 * g_settings->sizeCell;

	/* check */
	CHECK_CONDITION(s->value[TOTAL_AUT_RESP], < 0);
}




