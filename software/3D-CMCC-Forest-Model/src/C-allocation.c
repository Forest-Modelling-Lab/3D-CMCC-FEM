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

void C_allocation (species_t *const s, cell_t *const c, const int day, const int month, const int years, const int height, const int age, const int species)
{
	/* it allocates for both deciduous and evergreen daily carbon biomass */
	logger(g_log, "\n****BIOMASS POOLS UPDATE****\n");

		/* update class level carbon biomass pools */
		s->value[LEAF_C] += s->value[C_TO_LEAF];
		//s->value[LEAF_C] -= s->value[C_LEAF_TO_LITTER];
		logger(g_log, "Leaf Biomass (Wf) = %g tC/area\n", s->value[LEAF_C]);

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
		logger(g_log, "Coarse Root Biomass (Wrc) = %g tC/area\n", s->value[COARSE_ROOT_C]);

		s->value[TOT_ROOT_C] =  s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
		logger(g_log, "Total Root Biomass (Wr TOT) = %g tC/area\n", s->value[TOT_ROOT_C]);

		s->value[TOT_STEM_C] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
		logger(g_log, "Total Stem Biomass (Wts)= %g tC/area\n", s->value[TOT_STEM_C]);

		s->value[FRUIT_C] += s->value[C_TO_FRUIT];
		logger(g_log, "Fuit Biomass (Wfruit)= %g tC/area\n", s->value[FRUIT_C]);

		s->value[LITTER_C] += s->value[C_TO_LITTER] + s->value[C_FRUIT_TO_LITTER];
		logger(g_log, "Litter Biomass (Wlitter)= %g tC/area\n", s->value[LITTER_C]);

		/* live-dead wood biomass */
		s->value[STEM_LIVE_WOOD_C] += (s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
		s->value[STEM_LIVE_WOOD_C] -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
		logger(g_log, "Live Stem Biomass (Ws) = %g tC/area\n", s->value[STEM_LIVE_WOOD_C]);

		s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
		logger(g_log, "Dead Stem Biomass (Ws) = %g tC/area\n", s->value[STEM_DEAD_WOOD_C]);

		s->value[COARSE_ROOT_LIVE_WOOD_C] += (s->value[C_TO_COARSEROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
		s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD];
		logger(g_log, "Live Coarse Biomass (Ws) = %g tC/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

		s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
		logger(g_log, "Dead Coarse Biomass (Ws) = %g tC/area\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

		s->value[BRANCH_LIVE_WOOD_C] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
		s->value[BRANCH_LIVE_WOOD_C] -= s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD];
		logger(g_log, "Live Stem Branch Biomass (Ws) = %g tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);

		s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
		logger(g_log, "Dead Stem Branch Biomass (Ws) = %g tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);

		s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];
		logger(g_log, "Total Carbon Biomass (W) = %g tC/area\n", s->value[TOTAL_C]);

		/* check for closure */
		CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>1e-4);
		CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])-s->value[COARSE_ROOT_C]),>1e-4);
		CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>1e-4);
}
