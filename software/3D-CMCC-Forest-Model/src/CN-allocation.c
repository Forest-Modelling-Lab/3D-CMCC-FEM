/*
 * CN-allocation.c
 *
 * Created on: 01 lug 2016
 *   Author: alessio
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
#include "check_balance.h"

extern logger_t* g_debug_log;

void carbon_allocation(species_t *const s)
{
	/* it allocates Daily assimilated Carbon for both deciduous and evergreen daily */
	logger(g_debug_log, "\n****CARBON BIOMASS ALLOCATION****\n");

	/* update class level carbon biomass pools */
	s->value[LEAF_C] += s->value[C_TO_LEAF];
	//s->value[LEAF_C] -= s->value[C_LEAF_TO_LITTER];
	logger(g_debug_log, "Leaf Biomass (Wl) = %g tC/area\n", s->value[LEAF_C]);

	s->value[FINE_ROOT_C] += s->value[C_TO_FINEROOT];
	//s->value[FINE_ROOT_C] -= s->value[C_FINE_ROOT_TO_LITTER];
	logger(g_debug_log, "Fine Root Biomass (Wrf) = %g tC/area\n", s->value[FINE_ROOT_C]);

	s->value[RESERVE_C] += s->value[C_TO_RESERVE];
	s->value[RESERVE_C] += (s->value[C_LEAF_TO_RESERVE] + s->value[C_FINEROOT_TO_RESERVE]);;
	logger(g_debug_log, "Reserve Biomass (Wres) = %g tC/area\n", s->value[RESERVE_C]);

	s->value[STEM_C] += s->value[C_TO_STEM];
	logger(g_debug_log, "Stem Biomass (Ws) = %g tC/area\n", s->value[STEM_C]);

	s->value[BRANCH_C] += s->value[C_TO_BRANCH];
	logger(g_debug_log, "Branch and Bark Biomass (Wbb) = %g tC/area\n", s->value[BRANCH_C]);

	s->value[COARSE_ROOT_C] += s->value[C_TO_COARSEROOT];
	logger(g_debug_log, "Coarse Root Biomass (Wcr) = %g tC/area\n", s->value[COARSE_ROOT_C]);

	s->value[TOT_ROOT_C] = s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	logger(g_debug_log, "Total Root Biomass (Wtr) = %g tC/area\n", s->value[TOT_ROOT_C]);

	s->value[TOT_STEM_C] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
	logger(g_debug_log, "Total Stem Biomass (Wts)= %g tC/area\n", s->value[TOT_STEM_C]);

	s->value[FRUIT_C] += s->value[C_TO_FRUIT];
	logger(g_debug_log, "Fuit Biomass (Wfruit)= %g tC/area\n", s->value[FRUIT_C]);

	s->value[LITTER_C] += s->value[C_TO_LITTER] + s->value[C_FRUIT_TO_LITTER];
	logger(g_debug_log, "Litter Biomass (Wlitter)= %g tC/area\n", s->value[LITTER_C]);

	/* live-dead wood biomass */
	//test
	/*
	s->value[STEM_LIVE_WOOD_C] += (s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C] -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	*/
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Biomass (Wsl) = %g tC/area\n", s->value[STEM_LIVE_WOOD_C]);

	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Stem Biomass (Wsd) = %g tC/area\n", s->value[STEM_DEAD_WOOD_C]);

	//test
	/*
	s->value[COARSE_ROOT_LIVE_WOOD_C] += (s->value[C_TO_COARSEROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD];
	*/
	s->value[COARSE_ROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Coarse Biomass (Wcrl) = %g tC/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

	s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Coarse Biomass (Wcrd) = %g tC/area\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	//test
	/*
	s->value[BRANCH_LIVE_WOOD_C] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C] -= s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD];
	*/
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Branch Biomass (Wbbl) = %g tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);

	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Stem Branch Biomass (Wbbd) = %g tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];
	logger(g_debug_log, "Total Carbon Biomass (Wtot) = %g tC/area\n", s->value[TOTAL_C]);

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>eps);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])-s->value[COARSE_ROOT_C]),>eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>eps);
}

/********************************************************************************************************************************************/

void nitrogen_allocation (species_t *const s)
{
	/* it allocates Daily assimilated Nitrogen for both deciduous and evergreen */
	logger(g_debug_log, "\n****BIOMASS POOLS UPDATE****\n");

	/* update class level carbon Nitrogen pools */
	s->value[N_TO_LEAF] = s->value[C_TO_LEAF] / s->value[CN_LEAVES];
	s->value[LEAF_N] += s->value[N_TO_LEAF];
	//s->value[LEAF_C] -= s->value[C_LEAF_TO_LITTER];
	logger(g_debug_log, "Leaf Nitrogen (Wl) = %g tN/area\n", s->value[LEAF_N]);

	s->value[N_TO_FINEROOT] = s->value[C_TO_FINEROOT] / s->value[CN_FINE_ROOTS];
	s->value[FINE_ROOT_N] += s->value[N_TO_FINEROOT];
	//s->value[FINE_ROOT_C] -= s->value[C_FINE_ROOT_TO_LITTER];
	logger(g_debug_log, "Fine Root Nitrogen (Wrf) = %g tN/area\n", s->value[FINE_ROOT_N]);

//	s->value[RESERVE_C] += s->value[C_TO_RESERVE];
//	s->value[RESERVE_C] += (s->value[C_LEAF_TO_RESERVE] + s->value[C_FINEROOT_TO_RESERVE]);;
//	logger(g_debug_log, "Reserve Nitrogen (Wres) = %g tN/area\n", s->value[RESERVE_C]);

	s->value[N_TO_STEM] = s->value[C_TO_STEM] / s->value[CN_LIVE_WOODS];
	s->value[STEM_N] += s->value[N_TO_STEM];
	logger(g_debug_log, "Stem Nitrogen (Ws) = %g tN/area\n", s->value[STEM_N]);

	s->value[BRANCH_N] = s->value[C_TO_BRANCH] / s->value[CN_LIVE_WOODS];
	s->value[BRANCH_N] += s->value[N_TO_BRANCH];
	logger(g_debug_log, "Branch and Bark Nitrogen (Wbb) = %g tN/area\n", s->value[BRANCH_N]);

	s->value[N_TO_COARSEROOT] = s->value[C_TO_COARSEROOT] / s->value[CN_LIVE_WOODS];
	s->value[COARSE_ROOT_N] += s->value[N_TO_COARSEROOT];
	logger(g_debug_log, "Coarse Root Nitrogen (Wcr) = %g tN/area\n", s->value[COARSE_ROOT_N]);

#if 0
	s->value[TOT_ROOT_N] = s->value[COARSE_ROOT_N] + s->value[FINE_ROOT_N];
	logger(g_debug_log, "Total Root Nitrogen (Wtr) = %g tN/area\n", s->value[TOT_ROOT_C]);

	s->value[TOT_STEM_C] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
	logger(g_debug_log, "Total Stem Nitrogen (Wts)= %g tN/area\n", s->value[TOT_STEM_C]);

	s->value[FRUIT_C] += s->value[C_TO_FRUIT];
	logger(g_debug_log, "Fuit Nitrogen (Wfruit)= %g tN/area\n", s->value[FRUIT_C]);

	s->value[LITTER_C] += s->value[C_TO_LITTER] + s->value[C_FRUIT_TO_LITTER];
	logger(g_debug_log, "Litter Nitrogen (Wlitter)= %g tN/area\n", s->value[LITTER_C]);


	/* live-dead wood Nitrogen */
	//test
	/*
	s->value[STEM_LIVE_WOOD_C] += (s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C] -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	*/
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Nitrogen (Wsl) = %g tN/area\n", s->value[STEM_LIVE_WOOD_C]);

	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Stem Nitrogen (Wsd) = %g tN/area\n", s->value[STEM_DEAD_WOOD_C]);

	//test
	/*
	s->value[COARSE_ROOT_LIVE_WOOD_C] += (s->value[C_TO_COARSEROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD];
	*/
	s->value[COARSE_ROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Coarse Nitrogen (Wcrl) = %g tN/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

	s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Coarse Nitrogen (Wcrd) = %g tN/area\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	//test
	/*
	s->value[BRANCH_LIVE_WOOD_C] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C] -= s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD];
	*/
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Branch Nitrogen (Wbbl) = %g tN/area\n", s->value[BRANCH_LIVE_WOOD_C]);

	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Stem Branch Nitrogen (Wbbd) = %g tN/area\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];
	logger(g_debug_log, "Total Carbon Nitrogen (Wtot) = %g tN/area\n", s->value[TOTAL_C]);

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>eps);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])-s->value[COARSE_ROOT_C]),>eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>eps);
#endif

}
