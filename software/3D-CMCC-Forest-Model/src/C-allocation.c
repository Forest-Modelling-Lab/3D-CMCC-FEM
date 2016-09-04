/*
 * C-allocation.c
 *
 *  Created on: 01 lug 2016
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
#include "check_balance.h"

extern logger_t* g_log;

void carbon_allocation(species_t *const s)
{
	/* it allocates for both deciduous and evergreen daily carbon biomass */
	logger(g_log, "\n****BIOMASS POOLS UPDATE****\n");

	/* update class level carbon biomass pools */
	s->value[LEAF_C] += s->value[C_TO_LEAF];
	//s->value[LEAF_C] -= s->value[C_LEAF_TO_LITTER];
	logger(g_log, "Leaf Biomass (Wl) = %g tC/area\n", s->value[LEAF_C]);

	s->value[FINE_ROOT_C] += s->value[C_TO_FINEROOT];
	//s->value[FINE_ROOT_C] -= s->value[C_FINE_ROOT_TO_LITTER];
	logger(g_log, "Fine Root Biomass (Wrf) = %g tC/area\n", s->value[FINE_ROOT_C]);

	s->value[RESERVE_C] += s->value[C_TO_RESERVE];
	s->value[RESERVE_C] += (s->value[C_LEAF_TO_RESERVE] + s->value[C_FINEROOT_TO_RESERVE]);;
	logger(g_log, "Reserve Biomass (Wres) = %g tC/area\n", s->value[RESERVE_C]);

	s->value[STEM_C] += s->value[C_TO_STEM];
	logger(g_log, "Stem Biomass (Ws) = %g tC/area\n", s->value[STEM_C]);

	s->value[BRANCH_C] += s->value[C_TO_BRANCH];
	logger(g_log, "Branch and Bark Biomass (Wbb) = %g tC/area\n", s->value[BRANCH_C]);

	s->value[COARSE_ROOT_C] += s->value[C_TO_COARSEROOT];
	logger(g_log, "Coarse Root Biomass (Wcr) = %g tC/area\n", s->value[COARSE_ROOT_C]);

	s->value[TOT_ROOT_C] =  s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	logger(g_log, "Total Root Biomass (Wtr) = %g tC/area\n", s->value[TOT_ROOT_C]);

	s->value[TOT_STEM_C] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
	logger(g_log, "Total Stem Biomass (Wts)= %g tC/area\n", s->value[TOT_STEM_C]);

	s->value[FRUIT_C] += s->value[C_TO_FRUIT];
	logger(g_log, "Fuit Biomass (Wfruit)= %g tC/area\n", s->value[FRUIT_C]);

	s->value[LITTER_C] += s->value[C_TO_LITTER] + s->value[C_FRUIT_TO_LITTER];
	logger(g_log, "Litter Biomass (Wlitter)= %g tC/area\n", s->value[LITTER_C]);

	/* live-dead wood biomass */
	//test
	/*
	s->value[STEM_LIVE_WOOD_C] += (s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C] -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	*/
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_log, "Live Stem Biomass (Wsl) = %g tC/area\n", s->value[STEM_LIVE_WOOD_C]);

	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	logger(g_log, "Dead Stem Biomass (Wsd) = %g tC/area\n", s->value[STEM_DEAD_WOOD_C]);

	//test
	/*
	s->value[COARSE_ROOT_LIVE_WOOD_C] += (s->value[C_TO_COARSEROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD];
	*/
	s->value[COARSE_ROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_log, "Live Coarse Biomass (Wcrl) = %g tC/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

	s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_log, "Dead Coarse Biomass (Wcrd) = %g tC/area\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	//test
	/*
	s->value[BRANCH_LIVE_WOOD_C] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C] -= s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD];
	*/
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_log, "Live Stem Branch Biomass (Wbbl) = %g tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);

	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	logger(g_log, "Dead Stem Branch Biomass (Wbbd) = %g tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];
	logger(g_log, "Total Carbon Biomass (Wtot) = %g tC/area\n", s->value[TOTAL_C]);

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>1e-4);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])-s->value[COARSE_ROOT_C]),>1e-4);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>1e-4);
}
