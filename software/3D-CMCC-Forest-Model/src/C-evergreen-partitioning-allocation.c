/*
 * C-evergreen-partitioning-allocation.c
 *
 *  Created on: 20/nov/2013
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "lai.h"
#include "turnover.h"
#include "dendometry.h"
#include "biomass.h"
extern settings_t* g_settings;
extern logger_t* g_log;

/* Evergreen carbon allocation routine */
void Daily_C_Evergreen_Partitioning_Allocation (species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int years, const int DaysInMonth, const int height, const int age, const int species)
{
	//CTEM VERSION

	int i;

	//allocation parameter. their sum must be = 1
	double  s0Ctem = s->value[S0CTEM];
	double  r0Ctem = s->value[R0CTEM];
	//double  f0Ctem = s->value[F0CTEM];
	double const omegaCtem = s->value[OMEGA_CTEM];
	double pS_CTEM;
	double pR_CTEM;
	double pF_CTEM;
	//double max_DM_foliage;
	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	double Light_trasm;
	//	double Perc_fine;
	//	double Perc_coarse;
	//	double oldW;

	double r0Ctem_increment;
	double old_r0Ctem = r0Ctem;
	double s0Ctem_increment;
	double old_s0Ctem = s0Ctem;

	//Marconi here the allocation of biomass reserve is divided in fineroot and leaves following the
	//allocation ratio parameter between them. That because
	//in evergreen we don't have bud burst phenology phase, and indeed there are two phenology phases;
	//the former in which carbon is allocated in fineroot and foliage, the latter in
	// every pool except foliage

	/* both used in case of retranslocation of carbon to reserve */
	double old_leaf_c;
	double old_fineroot_c;

	logger(g_log, "ALLOCATION_ROUTINE\n\n");

	logger(g_log, "Carbon allocation routine for evergreen\n");

	i = c->heights[height].z;

	//	if(day == 0 && month == 0)
	//	{
	//		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
	//		{
	//			s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
	//			logger(g_log, "Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);
	//		}
	//	}


	/* following Arora and Boer 2005 */
	Light_trasm = exp(- s->value[K] * s->value[LAI]);

	//7 May 2012
	//compute static ratio of allocation between fine
	//fixme see if change with new parameters checked in "Pool_fraction"
	//	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
	//	logger(g_log, "Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
	//	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
	//	logger(g_log, "Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
	//	Perc_coarse = 1- Perc_fine;
	//logger(g_log, "Percentage of coarse root against total root= %f %%\n", Perc_coarse * 100 );

	/* Partitioning ratios from Arora and Boer 2005 */
	pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
	logger(g_log, "Roots CTEM ratio layer = %g %%\n", pR_CTEM * 100);
	pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
	logger(g_log, "Stem CTEM ratio = %g %%\n", pS_CTEM * 100);
	pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
	logger(g_log, "Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);
	CHECK_CONDITION(fabs(pR_CTEM + pS_CTEM + pF_CTEM), > 1 + 1e-4);


	old_leaf_c = s->value[LEAF_C];
	old_fineroot_c = s->value[FINE_ROOT_C];

	if (s->management == 0)
	{
		//logger(g_log, "Management type = TIMBER\n");
	}
	else
	{
		logger(g_log, "min r0 ctem = %f\n",s->value[MIN_R0CTEM] );
		logger(g_log, "max s0 ctem = %f\n",s->value[MAX_S0CTEM] );
		logger(g_log, "years for conversion = %f\n",s->value[YEARS_FOR_CONVERSION]);

		//considering a LINEAR increment
		//allocation ratio to roots
		r0Ctem -= s->value[MIN_R0CTEM];
		r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
		r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
		logger(g_log, "new r0_CTEM = %f \n", r0Ctem);

		if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
		{
			logger(g_log, "ERROR IN r0Ctem !!! \n");
		}

		//considering a LINEAR decrement
		//allocation ratio to stem + bb
		s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
		s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
		s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
		logger(g_log, "new s0_CTEM = %f \n", s0Ctem);

		if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)
		{
			logger(g_log, "ERROR IN s0Ctem !!! \n");
		}

	}
	logger(g_log, "LAI = %f \n", s->value[LAI]);
	logger(g_log, "PEAK LAI = %f \n", s->value[PEAK_LAI]);
	logger(g_log,"PHENOLOGY PHASE (CASE): %d\n", s->phenology_phase);


	switch (s->phenology_phase)
	{
	/************************************************************/
	case 1:

		/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
		 * and Barbaroux et al., 2002, the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
		 * the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
		 * sharing the daily remaining amount (taking into account respiration costs)of NSC */

		/* partitioning */
		if (s->value[NPP_tC] > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
				logger(g_log, "Allocating only into foliage and fine root pools\n");
				s->value[C_TO_LEAF] = s->value[NPP_tC] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
				s->value[C_TO_FINEROOT] = s->value[NPP_tC] - s->value[C_TO_LEAF];
				s->value[C_TO_RESERVE] = 0.0;
			}
			/* it needs */
			else if (s->value[RESERVE_C] > 0.0 && s->value[RESERVE_C] < s->value[MIN_RESERVE_C])
			{
				logger(g_log, "Allocating only into reserve pool (low reserves, positive NPP)\n");
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_RESERVE] = s->value[NPP_tC];
			}
			CHECK_CONDITION(s->value[RESERVE_C], < 0.0);

			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;
		}
		else
		{
			logger(g_log, "Allocating only into reserve pool (low reserves, negative NPP)\n");
			s->value[C_TO_LEAF] = 0.0;
			s->value[C_TO_FINEROOT] = 0.0;
			s->value[C_TO_RESERVE] = s->value[NPP_tC];
			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;
		}
		CHECK_CONDITION(s->value[RESERVE_C], < 0.0);
		break;
	case 2:
		logger(g_log, "allocating into the three pools Ws+Wr+Wreserve\n");

		/* partitioning */
		if (s->value[NPP_tC] > 0.0)
		{
			/* REPRODUCTION ONLY FOR NEEDLE LEAF */
			if(s->value[PHENOLOGY] == 1.2)
			{
				//NPP for reproduction
				s->value[C_TO_FRUIT] = s->value[NPP_tC] * s->value[FRUIT_PERC];
				s->value[NPP_tC] -= s->value[C_TO_FRUIT];
				logger(g_log, "including Biomass increment into cones = %f tC/area\n", s->value[C_TO_FRUIT]);
			}
			else
			{
				s->value[C_TO_FRUIT] = 0.0;
			}

			s->value[C_TO_COARSEROOT] = s->value[NPP_tC] * pR_CTEM;
			s->value[C_TO_FINEROOT] = 0.0;
			s->value[C_TO_RESERVE] = s->value[NPP_tC] * pF_CTEM;
			s->value[C_TO_TOT_STEM] = s->value[NPP_tC] * pS_CTEM;
			s->value[C_TO_STEM] = (s->value[NPP_tC] * pS_CTEM) * (1.0 - s->value[FRACBB]);
			s->value[C_TO_BRANCH] = (s->value[NPP_tDM] * pS_CTEM) * s->value[FRACBB];
			s->value[C_TO_LEAF] = 0.0;

		}
		else
		{
			s->value[C_TO_RESERVE] = s->value[NPP_tC];
			s->value[C_TO_FINEROOT] = 0.0;
			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_TOT_STEM] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_LEAF] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;
		}
		break;
	}

	logger(g_log, "\n*Carbon allocation*\n");

	/* update live_total wood fraction based on age */
	live_total_wood_age (&c->heights[height].ages[age], &c->heights[height].ages[age].species[species]);

	/* update leaf biomass through turnover */
	Turnover(&c->heights[height].ages[age].species[species], c);

	/* update class level carbon biomass pools */
	s->value[LEAF_C] += s->value[C_TO_LEAF];
	logger(g_log, "Leaf Biomass (Wf) = %f tC/area\n", s->value[LEAF_C]);

	s->value[STEM_C] += s->value[C_TO_STEM];
	logger(g_log, "Stem Biomass (Ws) = %f tC/area\n", s->value[STEM_C]);

	s->value[BRANCH_C] += s->value[C_TO_BRANCH];
	logger(g_log, "Branch and Bark Biomass (Wbb) = %f tC/area\n", s->value[BRANCH_C]);

	s->value[RESERVE_C] += s->value[C_TO_RESERVE];
	logger(g_log, "Reserve Biomass (Wres) = %f tC/area\n", s->value[RESERVE_C]);

	s->value[FINE_ROOT_C] += s->value[C_TO_FINEROOT];
	logger(g_log, "Fine Root Biomass (Wrf) = %f tC/area\n", s->value[FINE_ROOT_C]);

	s->value[COARSE_ROOT_C] += s->value[C_TO_COARSEROOT];
	logger(g_log, "Coarse Root Biomass (Wrc) = %f tC/area\n", s->value[COARSE_ROOT_C]);

	s->value[TOT_ROOT_C] =  s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	logger(g_log, "Total Root Biomass (Wr TOT) = %f tC/area\n", s->value[TOT_ROOT_C]);

	s->value[TOT_STEM_C] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
	logger(g_log, "Total Stem Biomass (Wts)= %f tC/area\n", s->value[TOT_STEM_C]);

	s->value[FRUIT_C] += s->value[C_TO_FRUIT];
	logger(g_log, "Fuit Biomass (Wfruit)= %f tC/area\n", s->value[FRUIT_C]);

	s->value[STEM_LIVE_WOOD_C] += (s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "Live Stem Biomass (Ws) = %f tC/area\n", s->value[STEM_LIVE_WOOD_C]);

	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	logger(g_log, "Dead Stem Biomass (Ws) = %f tC/area\n", s->value[STEM_DEAD_WOOD_C]);

	s->value[COARSE_ROOT_LIVE_WOOD_C] += (s->value[C_TO_COARSEROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "Live Coarse Biomass (Ws) = %f tC/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

	s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_log, "Dead Coarse Biomass (Ws) = %f tC/area\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	s->value[BRANCH_LIVE_WOOD_C] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "Live Stem Branch Biomass (Ws) = %f tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);

	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + /*s->value[FRUIT_C] +*/ s->value[RESERVE_C];
	logger(g_log, "Total Carbon Biomass (W) = %f tC/area\n", s->value[TOTAL_C]);

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>1e-4);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])-s->value[COARSE_ROOT_C]),>1e-4);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>1e-4);

	Average_tree_biomass (&c->heights[height].ages[age].species[species]);

	/* to avoid "jumps" of dbh it has computed only one monthly */
	if(day == 0)
	{
		Dendrometry (c, &c->heights[height].ages[age].species[species], &c->heights[height], years);
	}

	logger(g_log, "\n-Daily increment in carbon pools-\n");
	logger(g_log, "C_TO_LEAF = %f tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_log, "C_TO_FINEROOT = %f tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_log, "C_TO_COARSEROOT = %f tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_log, "C_TO_STEM = %f tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_log, "C_TO_RESERVE = %f tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_log, "C_TO_BRANCH = %f tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_log, "C_TO_FRUIT = %f tC/cell/day\n", s->value[C_TO_FRUIT]);
	logger(g_log, "C_TO_LITTER = %f tC/cell/day\n", s->value[C_TO_LITTER]);

	/* update Leaf Area Index */
	Daily_lai (&c->heights[height].ages[age].species[species]);
	c->daily_lai[i] = s->value[LAI];
	c->daily_layer_reserve_c[i] = s->value[RESERVE_C];

	/* update class level annual carbon biomass increment in tC/cell/year */
	s->value[DEL_Y_WTS] += s->value[C_TO_TOT_STEM];
	s->value[DEL_Y_WS] += s->value[C_TO_STEM];
	s->value[DEL_Y_WF] += s->value[C_TO_LEAF];
	s->value[DEL_Y_WFR] += s->value[C_TO_FINEROOT];
	s->value[DEL_Y_WCR] += s->value[C_TO_COARSEROOT];
	s->value[DEL_Y_WRES] += s->value[C_TO_RESERVE];
	s->value[DEL_Y_WR] += s->value[C_TO_ROOT];
	s->value[DEL_Y_BB] += s->value[C_TO_BRANCH];

	/* update layer level daily carbon biomass increment in tC/cell/day */
	c->daily_delta_wts[i] = s->value[C_TO_TOT_STEM];
	c->daily_delta_ws[i] = s->value[C_TO_STEM];
	c->daily_delta_wf[i] = s->value[C_TO_LEAF];
	c->daily_delta_wbb[i] = s->value[C_TO_BRANCH];
	c->daily_delta_wfr[i] = s->value[C_TO_FINEROOT];
	c->daily_delta_wcr[i] = s->value[C_TO_COARSEROOT];
	c->daily_delta_wres[i] = s->value[C_TO_RESERVE];

	/* update dendrometry variables */
	if(c->height_class_in_layer_dominant_counter>1)
	{
		c->annual_layer_avDBH[i] = (c->annual_layer_avDBH[i] + s->value[AVDBH]) / c->height_class_in_layer_dominant_counter;
	}
	else
	{
		c->annual_layer_avDBH[i] = s->value[AVDBH];
	}

	/* update layer level annual carbon increments and pools in tC/cell/year */
	c->annual_delta_ws[i] += s->value[C_TO_STEM];
	c->annual_layer_stem_c[i] = s->value[STEM_C];
	c->annual_layer_live_stem_c[i] = s->value[STEM_LIVE_WOOD_C];
	c->annual_layer_stem_sapwood_c[i] = s->value[STEM_SAPWOOD_C];
	c->annual_delta_wres[i] += s->value[C_TO_RESERVE];
	c->annual_layer_reserve_c[i] = s->value[RESERVE_C];
	c->annual_delta_wf[i] += s->value[C_TO_LEAF];
	c->annual_layer_leaf_c[i] = s->value[LEAF_C];
	c->annual_delta_wts[i] += s->value[C_TO_TOT_STEM];
	c->annual_layer_tot_stem_c[i] = s->value[TOT_STEM_C];
	c->annual_delta_wbb[i] += s->value[C_TO_BRANCH];
	c->annual_layer_branch_c[i] = s->value[BRANCH_C];
	c->annual_layer_live_branch_c[i] = s->value[BRANCH_LIVE_WOOD_C];
	c->annual_layer_branch_sapwood_c[i] = s->value[BRANCH_SAPWOOD_C];
	c->annual_delta_wfr[i] += s->value[C_TO_FINEROOT];
	c->annual_layer_fineroot_c[i] = s->value[FINE_ROOT_C];
	c->annual_delta_wcr[i] += s->value[C_TO_COARSEROOT];
	c->annual_layer_coarseroot_c[i] = s->value[COARSE_ROOT_C];
	c->annual_layer_live_coarseroot_c[i] = s->value[COARSE_ROOT_LIVE_WOOD_C];
	c->annual_layer_coarse_root_sapwood_c[i] = s->value[COARSE_ROOT_SAPWOOD_C];
	c->annual_layer_sapwood_c[i] = s->value[TOT_SAPWOOD_C];

	/* update cell level carbon biomass in gC/m2/day */
	c->daily_leaf_carbon += s->value[C_TO_LEAF] * 1000000.0 / g_settings->sizeCell ;
	c->daily_stem_carbon += s->value[C_TO_STEM] * 1000000.0 / g_settings->sizeCell ;
	c->daily_fine_root_carbon += s->value[C_TO_FINEROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_coarse_root_carbon += s->value[C_TO_COARSEROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_branch_carbon += s->value[C_TO_BRANCH] * 1000000.0 / g_settings->sizeCell ;
	c->daily_reserve_carbon += s->value[C_TO_RESERVE] * 1000000.0 / g_settings->sizeCell ;
	c->daily_root_carbon += s->value[C_TO_ROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_litter_carbon += s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell ;

	/* update cell level carbon biomass in tC/cell/day */
	c->daily_leaf_carbon_tC += s->value[C_TO_LEAF];
	c->daily_stem_carbon_tC += s->value[C_TO_STEM];
	c->daily_fine_root_carbon_tC += s->value[C_TO_FINEROOT];
	c->daily_coarse_root_carbon_tC += s->value[C_TO_COARSEROOT];
	c->daily_branch_carbon_tC += s->value[C_TO_BRANCH];
	c->daily_reserve_carbon_tC += s->value[C_TO_RESERVE];
	c->daily_root_carbon_tC += s->value[C_TO_ROOT];
	c->daily_litter_carbon_tC += s->value[C_TO_LITTER];

	logger(g_log, "******************************\n");
}

/**/

