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

void carbon_allocation( cell_t *const c, species_t *const s)
{
	/* it allocates daily assimilated carbon for both deciduous and evergreen daily */

	logger(g_debug_log, "\n**CARBON ALLOCATION**\n");

	/*** removing growth respiration from carbon flux pools ***/
	s->value[C_TO_LEAF]  -= (s->value[LEAF_GROWTH_RESP]   / 1e6 * g_settings->sizeCell);
	s->value[C_TO_FROOT] -= (s->value[FROOT_GROWTH_RESP]  / 1e6 * g_settings->sizeCell);
	s->value[C_TO_STEM]  -= (s->value[STEM_GROWTH_RESP]   / 1e6 * g_settings->sizeCell);
	s->value[C_TO_CROOT] -= (s->value[CROOT_GROWTH_RESP]  / 1e6 * g_settings->sizeCell);
	s->value[C_TO_BRANCH]-= (s->value[BRANCH_GROWTH_RESP] / 1e6 * g_settings->sizeCell);

	/*** update class level carbon mass pools ***/
	s->value[LEAF_C]     += s->value[C_TO_LEAF];
	s->value[FROOT_C]    += s->value[C_TO_FROOT];
	s->value[STEM_C]     += s->value[C_TO_STEM];
	s->value[BRANCH_C]   += s->value[C_TO_BRANCH];
	s->value[CROOT_C]    += s->value[C_TO_CROOT];
	s->value[RESERVE_C]  += s->value[C_TO_RESERVE];
	s->value[FRUIT_C]    += s->value[C_TO_FRUIT];
	s->value[TOT_ROOT_C] += s->value[C_TO_CROOT] + s->value[C_TO_FROOT];
	s->value[TOT_STEM_C] += s->value[C_TO_STEM]  + s->value[C_TO_BRANCH];
	s->value[LITR_C]     += s->value[C_TO_LITR]  + s->value[C_FRUIT_TO_LITR];

	/*** update cell level carbon mass pools ***/
	c->leaf_tC           += s->value[C_TO_LEAF];
	c->froot_tC          += s->value[C_TO_FROOT];
	c->stem_tC           += s->value[C_TO_STEM];
	c->branch_tC         += s->value[C_TO_BRANCH];
	c->croot_tC          += s->value[C_TO_CROOT];
	c->reserve_tC        += s->value[C_TO_RESERVE];
	c->fruit_tC          += s->value[C_TO_FRUIT];

	/*** update cell level carbon mass pools ***/
	c->leaf_carbon       += s->value[C_TO_LEAF]    * 1e6 / g_settings->sizeCell;
	c->froot_carbon      += s->value[C_TO_FROOT]   * 1e6 / g_settings->sizeCell;
	c->stem_carbon       += s->value[C_TO_STEM]    * 1e6 / g_settings->sizeCell;
	c->branch_carbon     += s->value[C_TO_BRANCH]  * 1e6 / g_settings->sizeCell;
	c->croot_carbon      += s->value[C_TO_CROOT]   * 1e6 / g_settings->sizeCell;
	c->reserve_carbon    += s->value[C_TO_RESERVE] * 1e6 / g_settings->sizeCell;
	c->fruit_carbon      += s->value[C_TO_FRUIT]   * 1e6 / g_settings->sizeCell;


#if 0
	/* check for possible overexceeding carbon flux */
	if ( s->value[LEAF_C] > s->value[MAX_LEAF_C] )
	{
		double exceeding_leaf_C;
		double ex_leaf_to_reserve;

		/* retranslocate biomass from leaf to reserve */
		exceeding_leaf_C   = s->value[LEAF_C] - s->value[MAX_LEAF_C];
		ex_leaf_to_reserve = (exceeding_leaf_C + (exceeding_leaf_C * s->value[EFF_GRPERC]));

		/* retranslocate biomass from leaf to reserve considering carbon lost for growth respiration */
		s->value[C_LEAF_TO_RESERVE]   += ex_leaf_to_reserve;
		s->value[RESERVE_C]           += ex_leaf_to_reserve;

		/* recompute leaf growth respiration */
		//s->value[LEAF_GROWTH_RESP]    -= (exceeding_leaf_C * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];

		/* recompute total growth respiration */
		//s->value[TOTAL_GROWTH_RESP]   -= (exceeding_leaf_C * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];
		//s->value[TOTAL_GROWTH_RESP_tC] = s->value[TOTAL_GROWTH_RESP] / 1e6 * g_settings->sizeCell ;

		/* fix leaf carbon to correct value */
		s->value[LEAF_C] = s->value[MAX_LEAF_C];

	}
	if ( s->value[FROOT_C] > s->value[MAX_FROOT_C] )
	{
		double exceeding_froot_C;
		double ex_froot_to_reserve;

		/* compute exceeding carbon to fine root  */
		exceeding_froot_C   = s->value[FROOT_C] - s->value[MAX_FROOT_C];
		ex_froot_to_reserve = (exceeding_froot_C + (exceeding_froot_C * s->value[EFF_GRPERC]));

		/* since fine root and leaf go in parallel */
		s->value[C_FROOT_TO_RESERVE]  += ex_froot_to_reserve;
		s->value[RESERVE_C]           += ex_froot_to_reserve;

		/* recompute fine root growth respiration */
		//s->value[FROOT_GROWTH_RESP]   -= (exceeding_froot_C * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];

		/* recompute total growth respiration */
		//s->value[TOTAL_GROWTH_RESP]   -= (exceeding_froot_C * 1e6 / g_settings->sizeCell) * s->value[EFF_GRPERC];
		//s->value[TOTAL_GROWTH_RESP_tC] = s->value[TOTAL_GROWTH_RESP] / 1e6 * g_settings->sizeCell ;

		/* fix fine root carbon to correct value */
		s->value[FROOT_C] = s->value[MAX_FROOT_C];

	}
#endif

	/***************************************************************************************/

	/* sapwood and heartwood*/

	s->value[STEM_SAPWOOD_C] += s->value[C_TO_STEM];
	s->value[CROOT_SAPWOOD_C] += s->value[C_TO_CROOT];
	s->value[BRANCH_SAPWOOD_C] += s->value[C_TO_BRANCH];

#if 0
	//test_new if not using the allometric equations for the sapwood amount
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	s->value[STEM_HEARTWOOD_C] = s->value[STEM_C] - s->value[STEM_SAPWOOD_C];

	/***************************************************************************************/

#if 0
	//test_new if not using the allometric equations for the sapwood amount
	s->value[CROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[CROOT_LIVE_WOOD_C] = s->value[CROOT_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[CROOT_DEAD_WOOD_C] = s->value[CROOT_C] - s->value[CROOT_LIVE_WOOD_C];
	s->value[CROOT_HEARTWOOD_C] = s->value[CROOT_C] - s->value[CROOT_SAPWOOD_C];

	/***************************************************************************************/

#if 0
	//test_new if not using the allometric equations for the sapwood amount
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	s->value[BRANCH_HEARTWOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C];

	/***************************************************************************************/

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>,eps);
	CHECK_CONDITION(fabs((s->value[CROOT_LIVE_WOOD_C] + s->value[CROOT_DEAD_WOOD_C])-s->value[CROOT_C]),>,eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>,eps);

}

/********************************************************************************************************************************************/

void nitrogen_allocation ( cell_t *const c, species_t *const s )
{
	/* it allocates Daily assimilated Nitrogen for both deciduous and evergreen and compute Nitrogen demand */

	logger(g_debug_log, "\n**NITROGEN ALLOCATION**\n");

	/*** update class level carbon Nitrogen pools ***/

	s->value[N_TO_LEAF]   = s->value[C_TO_LEAF]   / s->value[CN_LEAVES];
	s->value[N_TO_FROOT]  = s->value[C_TO_FROOT]  / s->value[CN_FINE_ROOTS];
	s->value[N_TO_STEM]   = s->value[C_TO_STEM]   / s->value[CN_LIVE_WOODS];
	s->value[N_TO_BRANCH] = s->value[C_TO_BRANCH] / s->value[CN_LIVE_WOODS];
	s->value[N_TO_CROOT]  = s->value[C_TO_CROOT]  / s->value[CN_LIVE_WOODS];

	/*****************************************************************************************************************************/

	/*** compute daily nitrogen demand ***/

	/* daily nitrogen demand */
	s->value[NPP_tN] = s->value[N_TO_LEAF] + s->value[N_TO_FROOT] + s->value[N_TO_STEM] + s->value[N_TO_BRANCH] + s->value[N_TO_CROOT];

	/* tN/Cell/day -> gC/m2/day */
	s->value[NPP_gN] = s->value[NPP_tN] * 1e6 / g_settings->sizeCell;

	/* daily Nitrogen demand */
	s->value[TREE_N_DEMAND] = s->value[NPP_gN];

	//fixme
	if (s->value[TREE_N_DEMAND] > c->soilN)
	{
		//todo back to partitioning-allocation routine and recompute both NPP in gC and NPP in gN based on the available soil nitrogen content
	}

#if 0
	s->value[LEAF_N]     += s->value[N_TO_LEAF];
	s->value[FROOT_N]    += s->value[N_TO_FROOT];
	//s->value[RESERVE_N]  += s->value[N_TO_RESERVE]; //fixme
	s->value[STEM_N]     += s->value[C_TO_STEM];
	s->value[BRANCH_N]   += s->value[C_TO_BRANCH];
	s->value[CROOT_N]    += s->value[C_TO_CROOT];
	//s->value[FRUIT_N]    += s->value[N_TO_FRUIT]; //fixme
	s->value[TOT_ROOT_N] += s->value[CROOT_N] + s->value[FROOT_N];
	s->value[TOT_STEM_N] += s->value[N_TO_STEM] + s->value[N_TO_BRANCH];
	s->value[LITR_N]     += s->value[N_TO_LITR] /*+ s->value[N_FRUIT_TO_LITR]*/; //fixme


	logger(g_debug_log, "N_TO_LEAF = %g tN/cell/day\n", s->value[N_TO_LEAF]);
	logger(g_debug_log, "N_TO_FROOT = %g tN/cell/day\n", s->value[N_TO_FROOT]);
	logger(g_debug_log, "N_TO_NROOT = %g tN/cell/day\n", s->value[N_TO_NROOT]);
	logger(g_debug_log, "N_TO_STEM = %g tN/cell/day\n", s->value[N_TO_STEM]);
	logger(g_debug_log, "N_TO_RESERVE = %g tN/cell/day\n", s->value[N_TO_RESERVE]);
	logger(g_debug_log, "N_TO_BRANCH = %g tN/cell/day\n", s->value[N_TO_BRANCH]);
	logger(g_debug_log, "N_TO_FRUIT = %g tN/cell/day\n", s->value[N_TO_FRUIT]);
	logger(g_debug_log, "Leaf Nitrogen (Wl) = %g tN/Cell\n", s->value[LEAF_N]);
	logger(g_debug_log, "Fine Root Nitrogen (Wrf) = %g tN/Cell\n", s->value[FROOT_N]);
	logger(g_debug_log, "Reserve Nitrogen (Wres) = %g tN/Cell\n", s->value[RESERVE_N]);
	logger(g_debug_log, "Stem Nitrogen (Ws) = %g tN/Cell\n", s->value[STEM_N]);
	logger(g_debug_log, "Branch and Bark Nitrogen (Wbb) = %g tN/Cell\n", s->value[BRANCH_N]);
	logger(g_debug_log, "Coarse Root Nitrogen (Wcr) = %g tN/Cell\n", s->value[CROOT_N]);
	logger(g_debug_log, "Total Root Nitrogen (Wtr) = %g tN/Cell\n", s->value[TOT_ROOT_N]);
	logger(g_debug_log, "Total Stem Nitrogen (Wts)= %g tN/Cell\n", s->value[TOT_STEM_N]);
	logger(g_debug_log, "Fruit Nitrogen (Wfruit)= %g tN/Cell\n", s->value[FRUIT_N]);
	logger(g_debug_log, "Litter Nitrogen (Wlitter)= %g tN/Cell\n", s->value[LITR_N]);

	/***************************************************************************************/

	/* sapwood and heartwood*/

	s->value[STEM_SAPWOOD_C] += s->value[C_TO_STEM];
	s->value[CROOT_SAPWOOD_C] += s->value[C_TO_CROOT];
	s->value[BRANCH_SAPWOOD_C] += s->value[C_TO_BRANCH];


#if 0
	//test_new if not using the allometric equations for the sapwood amount
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[STEM_LIVE_WOOD_C] = s->value[STEM_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
	s->value[STEM_HEARTWOOD_C] = s->value[STEM_C] - s->value[STEM_SAPWOOD_C];

	/***************************************************************************************/

#if 0
	//test_new if not using the allometric equations for the sapwood amount
	s->value[CROOT_LIVE_WOOD_C] = s->value[COARSE_ROOT_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[CROOT_LIVE_WOOD_C] = s->value[CROOT_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[CROOT_DEAD_WOOD_C] = s->value[CROOT_C] - s->value[CROOT_LIVE_WOOD_C];
	s->value[CROOT_HEARTWOOD_C] = s->value[CROOT_C] - s->value[CROOT_SAPWOOD_C];

	/***************************************************************************************/

#if 0
	//test_new if not using the allometric equations for the sapwood amount
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_SAPWOOD_C] * s->value[LIVE_TOTAL_WOOD_FRAC];
#else
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C]* s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	s->value[BRANCH_HEARTWOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C];

	/***************************************************************************************/

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + s->value[FRUIT_C] + s->value[RESERVE_C];

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])-s->value[STEM_C]),>,eps);
	CHECK_CONDITION(fabs((s->value[CROOT_LIVE_WOOD_C] + s->value[CROOT_DEAD_WOOD_C])-s->value[CROOT_C]),>,eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>,eps);
#endif




}
