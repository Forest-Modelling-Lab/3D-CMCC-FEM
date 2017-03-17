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

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void carbon_allocation(species_t *const s)
{
	/* it allocates Daily assimilated Carbon for both deciduous and evergreen daily */
	logger(g_debug_log, "\n****CARBON ALLOCATION****\n");

	/*** update class level carbon pools ***/

	logger(g_debug_log, "C_TO_LEAF = %g tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_debug_log, "C_TO_FINEROOT = %g tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_debug_log, "C_TO_COARSEROOT = %g tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_debug_log, "C_TO_STEM = %g tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_debug_log, "C_TO_RESERVE = %g tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_debug_log, "C_TO_BRANCH = %g tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_debug_log, "C_TO_FRUIT = %g tC/cell/day\n", s->value[C_TO_FRUIT]);

	//fixme
	s->value[LEAF_C] += s->value[C_TO_LEAF] ;
	logger(g_debug_log, "Leaf Carbon (Wl) = %g tC/Cell\n", s->value[LEAF_C]);

	//fixme
	s->value[FINE_ROOT_C] += s->value[C_TO_FINEROOT];
	//s->value[FINE_ROOT_C] -= s->value[C_FINE_ROOT_TO_LITTER];
	logger(g_debug_log, "Fine Root Carbon (Wrf) = %g tC/Cell\n", s->value[FINE_ROOT_C]);

	s->value[RESERVE_C] += s->value[C_TO_RESERVE];
	s->value[RESERVE_C] += (s->value[C_LEAF_TO_RESERVE] + s->value[C_FINEROOT_TO_RESERVE]);;
	logger(g_debug_log, "Reserve Carbon (Wres) = %g tC/Cell\n", s->value[RESERVE_C]);

	s->value[STEM_C] += s->value[C_TO_STEM];
	logger(g_debug_log, "Stem Carbon (Ws) = %g tC/Cell\n", s->value[STEM_C]);

	s->value[BRANCH_C] += s->value[C_TO_BRANCH];
	logger(g_debug_log, "Branch and Bark Carbon (Wbb) = %g tC/Cell\n", s->value[BRANCH_C]);

	s->value[COARSE_ROOT_C] += s->value[C_TO_COARSEROOT];
	logger(g_debug_log, "Coarse Root Carbon (Wcr) = %g tC/Cell\n", s->value[COARSE_ROOT_C]);

	s->value[TOT_ROOT_C] = s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	logger(g_debug_log, "Total Root Carbon (Wtr) = %g tC/Cell\n", s->value[TOT_ROOT_C]);

	s->value[TOT_STEM_C] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
	logger(g_debug_log, "Total Stem Carbon (Wts)= %g tC/Cell\n", s->value[TOT_STEM_C]);

	s->value[FRUIT_C] += s->value[C_TO_FRUIT];
	logger(g_debug_log, "Fuit Carbon (Wfruit)= %g tC/Cell\n", s->value[FRUIT_C]);

	s->value[LITR_C] += s->value[C_TO_LITTER] + s->value[C_FRUIT_TO_LITTER];
	logger(g_debug_log, "Litter Carbon (Wlitter)= %g tC/Cell\n", s->value[LITR_C]);

	s->value[SOIL_C] += s->value[C_TO_SOIL];
	logger(g_debug_log, "Soil Carbon (WSoil)= %g tC/Cell\n", s->value[SOIL_C]);

	/***************************************************************************************/

	s->value[STEM_SAPWOOD_C] += s->value[C_TO_STEM];
	logger(g_debug_log, "Stem sapwood Carbon = %g tC/Cell\n", s->value[STEM_SAPWOOD_C]);

	s->value[COARSE_ROOT_SAPWOOD_C] += s->value[C_TO_COARSEROOT];
	logger(g_debug_log, "Coarse root sapwood Carbon = %g tC/Cell\n", s->value[COARSE_ROOT_SAPWOOD_C]);

	s->value[BRANCH_SAPWOOD_C] += s->value[C_TO_BRANCH];
	logger(g_debug_log, "Branch sapwood Carbon = %g tC/Cell\n", s->value[BRANCH_SAPWOOD_C]);

	/* live-dead wood biomass */
	/***************************************************************************************/

#if 0
	//test_new
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Carbon (Wsl) = %g tC/Cell\n", s->value[STEM_LIVE_WOOD_C]);
#endif

	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Stem Carbon (Wsd) = %g tC/Cell\n", s->value[STEM_DEAD_WOOD_C]);

	s->value[STEM_HEARTWOOD_C] = s->value[STEM_C] - s->value[STEM_SAPWOOD_C];
	logger(g_debug_log, "Stem heartwood Carbon = %g tC/Cell\n", s->value[STEM_HEARTWOOD_C]);

	/***************************************************************************************/

#if 0
	//test_new
	s->value[COARSE_ROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[COARSE_ROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Coarse Carbon (Wcrl) = %g tC/Cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
#endif
	s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Coarse Carbon (Wcrd) = %g tC/Cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	s->value[COARSE_ROOT_HEARTWOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_SAPWOOD_C];
	logger(g_debug_log, "Coarse root heartwood Carbon = %g tC/Cell\n", s->value[COARSE_ROOT_HEARTWOOD_C]);

	/***************************************************************************************/

#if 0
	//test_new
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Branch Carbon (Wbbl) = %g tC/Cell\n", s->value[BRANCH_LIVE_WOOD_C]);
#endif

	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	logger(g_debug_log, "Dead Stem Branch Carbon (Wbbd) = %g tC/Cell\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[BRANCH_HEARTWOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C];
	logger(g_debug_log, "Branch heartwood Carbon = %g tC/Cell\n", s->value[BRANCH_HEARTWOOD_C]);

	/***************************************************************************************/

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];
	logger(g_debug_log, "Total Carbon Biomass (Wtot) = %g tC/Cell\n", s->value[TOTAL_C]);

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>,eps);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])-s->value[COARSE_ROOT_C]),>,eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>,eps);

}

/********************************************************************************************************************************************/

void nitrogen_allocation (species_t *const s)
{
	/* it allocates Daily assimilated Nitrogen for both deciduous and evergreen and compute Nitrogen demand */

	logger(g_debug_log, "\n****NITROGEN POOLS UPDATE****\n");

	/*** update class level carbon Nitrogen pools ***/

	s->value[N_TO_LEAF] = s->value[C_TO_LEAF] / s->value[CN_LEAVES];
	//s->value[LEAF_N] += s->value[N_TO_LEAF];
	//s->value[LEAF_C] -= s->value[C_LEAF_TO_LITTER];


	s->value[N_TO_FINEROOT] = s->value[C_TO_FINEROOT] / s->value[CN_FINE_ROOTS];
	//s->value[FINE_ROOT_N] += s->value[N_TO_FINEROOT];
	//s->value[FINE_ROOT_C] -= s->value[C_FINE_ROOT_TO_LITTER];

	s->value[N_TO_STEM] = s->value[C_TO_STEM] / s->value[CN_LIVE_WOODS];
	//s->value[STEM_N] += s->value[N_TO_STEM];

	s->value[N_TO_BRANCH] = s->value[C_TO_BRANCH] / s->value[CN_LIVE_WOODS];
	//s->value[BRANCH_N] += s->value[N_TO_BRANCH];

	s->value[N_TO_COARSEROOT] = s->value[C_TO_COARSEROOT] / s->value[CN_LIVE_WOODS];
	//s->value[COARSE_ROOT_N] += s->value[N_TO_COARSEROOT];


	logger(g_debug_log, "N_TO_LEAF = %g tN/cell/day\n", s->value[N_TO_LEAF]);
	logger(g_debug_log, "N_TO_FINEROOT = %g tN/cell/day\n", s->value[N_TO_FINEROOT]);
	logger(g_debug_log, "N_TO_COARSEROOT = %g tN/cell/day\n", s->value[N_TO_COARSEROOT]);
	logger(g_debug_log, "N_TO_STEM = %g tN/cell/day\n", s->value[N_TO_STEM]);
	logger(g_debug_log, "N_TO_BRANCH = %g tN/cell/day\n", s->value[N_TO_BRANCH]);

	/*****************************************************************************************************************************/

	/*** compute daily nitrogen demand ***/

	/* daily nitrogen demand */
	s->value[NPP_tN] = s->value[N_TO_LEAF] + s->value[N_TO_FINEROOT] + s->value[N_TO_STEM] + s->value[N_TO_BRANCH] + s->value[N_TO_COARSEROOT];
	logger(g_debug_log, "Daily Nitrogen demand = %g tN/Cell/day\n", s->value[NPP_tN]);

	/* tN/Cell/day -> gC/m2/day */
	s->value[NPP_gN] = s->value[NPP_tN] / g_settings->sizeCell * 1000000;
	logger(g_debug_log, "Daily Nitrogen demand = %g gN/m2/day\n", s->value[NPP_gN]);

#if 0
	s->value[TOT_ROOT_N] = s->value[COARSE_ROOT_N] + s->value[FINE_ROOT_N];
	logger(g_debug_log, "Total Root Nitrogen (Wtr) = %g tN/Cell\n", s->value[TOT_ROOT_N]);

	s->value[TOT_STEM_N] += s->value[C_TO_STEM] + s->value[C_TO_BRANCH];
	logger(g_debug_log, "Total Stem Nitrogen (Wts)= %g tN/Cell\n", s->value[TOT_STEM_N]);

	s->value[FRUIT_N] += s->value[C_TO_FRUIT];
	logger(g_debug_log, "Fuit Nitrogen (Wfruit)= %g tN/Cell\n", s->value[FRUIT_N]);

	s->value[LITTER_N] += s->value[C_TO_LITTER] + s->value[C_FRUIT_TO_LITTER];
	logger(g_debug_log, "Litter Nitrogen (Wlitter)= %g tN/Cell\n", s->value[LITTER_N]);


	/* live-dead wood Nitrogen */
	//test
	/*
	s->value[STEM_LIVE_WOOD_N] += (s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_N] -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	*/
	s->value[STEM_LIVE_WOOD_N] = s->value[STEM_N] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Nitrogen (Wsl) = %g tN/Cell\n", s->value[STEM_LIVE_WOOD_N]);

	s->value[STEM_DEAD_WOOD_N] = s->value[STEM_N] - s->value[STEM_LIVE_WOOD_N];
	logger(g_debug_log, "Dead Stem Nitrogen (Wsd) = %g tN/Cell\n", s->value[STEM_DEAD_WOOD_N]);

	//test
	/*
	s->value[COARSE_ROOT_LIVE_WOOD_N] += (s->value[C_TO_NOARSEROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_N] -= s->value[C_NOARSEROOT_LIVE_WOOD_TO_DEADWOOD];
	*/
	s->value[COARSE_ROOT_LIVE_WOOD_N] = s->value[COARSE_ROOT_N] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Coarse Nitrogen (Wcrl) = %g tN/Cell\n", s->value[COARSE_ROOT_LIVE_WOOD_N]);

	s->value[COARSE_ROOT_DEAD_WOOD_N] = s->value[COARSE_ROOT_N] - s->value[COARSE_ROOT_LIVE_WOOD_N];
	logger(g_debug_log, "Dead Coarse Nitrogen (Wcrd) = %g tN/Cell\n", s->value[COARSE_ROOT_DEAD_WOOD_N]);

	//test
	/*
	s->value[BRANCH_LIVE_WOOD_N] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_N] -= s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD];
	*/
	s->value[BRANCH_LIVE_WOOD_N] = s->value[BRANCH_N]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	logger(g_debug_log, "Live Stem Branch Nitrogen (Wbbl) = %g tN/Cell\n", s->value[BRANCH_LIVE_WOOD_N]);

	s->value[BRANCH_DEAD_WOOD_N] = s->value[BRANCH_N] - s->value[BRANCH_LIVE_WOOD_N];
	logger(g_debug_log, "Dead Stem Branch Nitrogen (Wbbd) = %g tN/Cell\n", s->value[BRANCH_DEAD_WOOD_N]);

	s->value[TOTAL_N] = s->value[LEAF_N] +s->value[STEM_N] + s->value[BRANCH_N] + s->value[TOT_ROOT_N] + s->value[FRUIT_N] + s->value[RESERVE_N];
	logger(g_debug_log, "Total Carbon Nitrogen (Wtot) = %g tN/Cell\n", s->value[TOTAL_N]);

	/* check for closure */
	CHECK_NONDITION(fabs((s->value[STEM_LIVE_WOOD_N] + s->value[STEM_DEAD_WOOD_N])-s->value[STEM_N]),>,eps);
	CHECK_NONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_N] + s->value[COARSE_ROOT_DEAD_WOOD_N])-s->value[COARSE_ROOT_N]),>,eps);
	CHECK_NONDITION(fabs((s->value[BRANCH_LIVE_WOOD_N] + s->value[BRANCH_DEAD_WOOD_N])-s->value[BRANCH_N]),>,eps);
#endif

}
