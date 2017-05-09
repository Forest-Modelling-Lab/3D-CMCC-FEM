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
#include "mortality.h"
#include "remove_tree_class.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void carbon_allocation( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* it allocates daily assimilated carbon for both deciduous and evergreen daily
	 * and removes the respired and dead parts */

	logger(g_debug_log, "\n**CARBON ALLOCATION**\n");

	/*** removing growth respiration and dead pools from carbon flux pools ***/
#if 1 //test
	s->value[C_TO_LEAF]    -= ((s->value[LEAF_GROWTH_RESP]   / 1e6 * g_settings->sizeCell) + s->value[C_LEAF_TO_LITR]  + s->value[C_LEAF_TO_RESERVE]);
	s->value[C_TO_FROOT]   -= ((s->value[FROOT_GROWTH_RESP]  / 1e6 * g_settings->sizeCell) + s->value[C_FROOT_TO_LITR] + s->value[C_FROOT_TO_RESERVE]);
	s->value[C_TO_STEM]    -= ((s->value[STEM_GROWTH_RESP]   / 1e6 * g_settings->sizeCell) + s->value[C_STEM_TO_CWD]);
	s->value[C_TO_CROOT]   -= ((s->value[CROOT_GROWTH_RESP]  / 1e6 * g_settings->sizeCell) + s->value[C_CROOT_TO_CWD]);
	s->value[C_TO_BRANCH]  -= ((s->value[BRANCH_GROWTH_RESP] / 1e6 * g_settings->sizeCell) + s->value[C_BRANCH_TO_CWD]);
	s->value[C_TO_FRUIT]   -=   s->value[C_FRUIT_TO_CWD];
	s->value[C_TO_RESERVE] -=  (s->value[C_RESERVE_TO_CWD]);
	s->value[C_TO_LITR]    +=   s->value[C_LEAF_TO_LITR] +
			s->value[C_FROOT_TO_LITR];
	s->value[C_TO_CWD]     +=   s->value[C_BRANCH_TO_CWD];
#else
	//if using test
	s->value[C_TO_LEAF]    -= ((s->value[LEAF_GROWTH_RESP]   / 1e6 * g_settings->sizeCell) + s->value[C_LEAF_TO_LITR]  + s->value[C_LEAF_TO_RESERVE]);
	s->value[C_TO_FROOT]   -= ((s->value[FROOT_GROWTH_RESP]  / 1e6 * g_settings->sizeCell) + s->value[C_FROOT_TO_LITR] + s->value[C_FROOT_TO_RESERVE]);
	s->value[C_TO_STEM]    -= ((s->value[STEM_GROWTH_RESP]   / 1e6 * g_settings->sizeCell));
	s->value[C_TO_CROOT]   -= ((s->value[CROOT_GROWTH_RESP]  / 1e6 * g_settings->sizeCell));
	s->value[C_TO_BRANCH]  -= ((s->value[BRANCH_GROWTH_RESP] / 1e6 * g_settings->sizeCell));
	s->value[C_TO_FRUIT]   -=   s->value[C_FRUIT_TO_CWD];
	s->value[C_TO_RESERVE] -=  (s->value[C_RESERVE_TO_CWD]);
	s->value[C_TO_LITR]    +=   s->value[C_LEAF_TO_LITR] +
			s->value[C_FROOT_TO_LITR];
	s->value[C_TO_CWD]     +=   s->value[C_BRANCH_TO_CWD];
#endif

	/*** update class level carbon mass pools ***/
	s->value[LEAF_C]     += s->value[C_TO_LEAF];
	s->value[FROOT_C]    += s->value[C_TO_FROOT];
	s->value[STEM_C]     += s->value[C_TO_STEM];
	s->value[CROOT_C]    += s->value[C_TO_CROOT];
	s->value[BRANCH_C]   += s->value[C_TO_BRANCH];
	s->value[RESERVE_C]  += s->value[C_TO_RESERVE];
	s->value[FRUIT_C]    += s->value[C_TO_FRUIT];
	s->value[LITR_C]     += s->value[C_TO_LITR];
	s->value[CWD_C]      += s->value[C_TO_CWD];

	/* check */
	CHECK_CONDITION ( s->value[LEAF_C],     < , ZERO );
	CHECK_CONDITION ( s->value[FROOT_C],    < , ZERO );
	CHECK_CONDITION ( s->value[STEM_C],     < , ZERO );
	CHECK_CONDITION ( s->value[BRANCH_C],   < , ZERO );
	CHECK_CONDITION ( s->value[CROOT_C],    < , ZERO );
	CHECK_CONDITION ( s->value[FRUIT_C],    < , ZERO );
	CHECK_CONDITION ( s->value[LITR_C],     < , ZERO );
	CHECK_CONDITION ( s->value[CWD_C],      < , ZERO );

	/* single tree average tree pools */
	average_tree_pools ( s );


	/*** update cell level carbon fluxes (gC/m2/day)***/
	c->daily_leaf_carbon        += (s->value[C_TO_LEAF]    * 1e6 / g_settings->sizeCell);
	c->daily_froot_carbon       += (s->value[C_TO_FROOT]   * 1e6 / g_settings->sizeCell);
	c->daily_stem_carbon        += (s->value[C_TO_STEM]    * 1e6 / g_settings->sizeCell);
	c->daily_croot_carbon       += (s->value[C_TO_CROOT]   * 1e6 / g_settings->sizeCell);
	c->daily_branch_carbon      += (s->value[C_TO_BRANCH]  * 1e6 / g_settings->sizeCell);
	c->daily_reserve_carbon     += (s->value[C_TO_RESERVE] * 1e6 / g_settings->sizeCell);
	c->daily_fruit_carbon       += (s->value[C_TO_FRUIT]   * 1e6 / g_settings->sizeCell);
	//computed in littering.c
	//c->daily_litrC              += (s->value[C_TO_LITR]    * 1e6 / g_settings->sizeCell);
	//c->daily_cwdC               += (s->value[C_TO_CWD]     * 1e6 / g_settings->sizeCell);

	/*** update cell level carbon pools (gC/m2)***/
	c->leaf_carbon              += (s->value[C_TO_LEAF]    * 1e6 / g_settings->sizeCell);
	c->froot_carbon             += (s->value[C_TO_FROOT]   * 1e6 / g_settings->sizeCell);
	c->stem_carbon              += (s->value[C_TO_STEM]    * 1e6 / g_settings->sizeCell);
	c->branch_carbon            += (s->value[C_TO_BRANCH]  * 1e6 / g_settings->sizeCell);
	c->croot_carbon             += (s->value[C_TO_CROOT]   * 1e6 / g_settings->sizeCell);
	c->reserve_carbon           += (s->value[C_TO_RESERVE] * 1e6 / g_settings->sizeCell);
	c->fruit_carbon             += (s->value[C_TO_FRUIT]   * 1e6 / g_settings->sizeCell);
	//computed in littering.c
	//c->litrC                    += (s->value[C_TO_LITR]    * 1e6 / g_settings->sizeCell);
	//c->cwdC                     += (s->value[C_TO_CWD]     * 1e6 / g_settings->sizeCell);

	/* check */
	CHECK_CONDITION ( c->leaf_carbon,    < , ZERO );
	CHECK_CONDITION ( c->froot_carbon,   < , ZERO );
	CHECK_CONDITION ( c->stem_carbon,    < , ZERO );
	CHECK_CONDITION ( c->branch_carbon,  < , ZERO );
	CHECK_CONDITION ( c->croot_carbon,   < , ZERO );
	CHECK_CONDITION ( c->fruit_carbon,   < , ZERO );
	CHECK_CONDITION ( c->litrC,          < , ZERO );
	CHECK_CONDITION ( c->cwdC,           < , ZERO );
	/***************************************************************************************/

	/* sapwood and heartwood */

	s->value[STEM_SAPWOOD_C]   += s->value[C_TO_STEM];
	s->value[CROOT_SAPWOOD_C]  += s->value[C_TO_CROOT];
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
	s->value[BRANCH_LIVE_WOOD_C] = s->value[BRANCH_C] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
#endif
	s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	s->value[BRANCH_HEARTWOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C];

	/***************************************************************************************/

	s->value[TOTAL_C] = s->value[LEAF_C] +
			s->value[FROOT_C]            +
			s->value[STEM_C]             +
			s->value[BRANCH_C]           +
			s->value[CROOT_C]            +
			s->value[FRUIT_C]            +
			s->value[RESERVE_C]          ;

	/* check for closure */
	CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C]   + s->value[STEM_DEAD_WOOD_C])  -s->value[STEM_C]),  >,eps);
	CHECK_CONDITION(fabs((s->value[CROOT_LIVE_WOOD_C]  + s->value[CROOT_DEAD_WOOD_C]) -s->value[CROOT_C]), >,eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])-s->value[BRANCH_C]),>,eps);
}

/********************************************************************************************************************************************/

void nitrogen_allocation ( cell_t *const c, species_t *const s )
{
	double n_to_leaf;
	double n_to_froot;
	double n_to_stem;
	double n_to_croot;
	double n_to_branch;
	double n_to_fruit;
	double n_to_reserve;

	/* it allocates Daily assimilated Nitrogen for both deciduous and evergreen and compute Nitrogen demand */

	logger(g_debug_log, "\n**NITROGEN ALLOCATION**\n");

	/*** update class level carbon Nitrogen pools ***/


	/* leaf */
	s->value[N_TO_LEAF]   = s->value[C_TO_LEAF]   / s->value[CN_LEAVES];

	if ( s->value[N_TO_LEAF]   > 0. ) n_to_leaf   = s->value[N_TO_LEAF];
	else n_to_leaf     = 0.;

	/* fine root */
	s->value[N_TO_FROOT]  = s->value[C_TO_FROOT]  / s->value[CN_FINE_ROOTS];

	if ( s->value[N_TO_FROOT]  > 0. ) n_to_froot  = s->value[N_TO_FROOT];
	else n_to_froot    = 0.;

	/* reserve */
	s->value[N_TO_RESERVE] = 0. /* s->value[N_LEAF_TO_RESERVE] */;//FIXME

	if ( s->value[N_TO_RESERVE]  > 0. ) n_to_reserve  = s->value[N_TO_RESERVE];
	else n_to_reserve  = 0.;

	/* fruit */
	s->value[N_TO_FRUIT]   = s->value[C_TO_FRUIT] / s->value[CN_LEAVES];//FIXME IT USES CN_LEAVES INSTEAD A CN_FRUITS

	if ( s->value[N_TO_FRUIT]  > 0. ) n_to_fruit  = s->value[N_TO_FRUIT];
	else n_to_fruit    = 0.;

	//note: if carbon transfer fluxes are positive than carbon and nitrogen to move are considered as "live tissues"
	//note: otherwise e.g. they need to be considered in their live and dead wood parts

	/* stem */
	if (s->value[C_TO_STEM] > 0.)
	{
		s->value[N_TO_STEM] = s->value[C_TO_STEM] / s->value[CN_LIVE_WOODS];

		n_to_stem  = s->value[N_TO_STEM];

	}
	else
	{
		s->value[N_TO_STEM] = (s->value[C_TO_STEM] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] / s->value[CN_LIVE_WOODS]) +
				(s->value[C_TO_STEM] * ( 1. - s->value[EFF_LIVE_TOTAL_WOOD_FRAC]) / s->value[CN_DEAD_WOODS]);

		n_to_stem   = 0.;
	}

	/* coarse root */
	if (s->value[C_TO_CROOT] > 0.)
	{
		s->value[N_TO_CROOT] = s->value[C_TO_CROOT] / s->value[CN_LIVE_WOODS];

		n_to_croot   = s->value[N_TO_CROOT];

	}
	else
	{
		s->value[N_TO_CROOT] = (s->value[C_TO_CROOT] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] / s->value[CN_LIVE_WOODS]) +
				(s->value[C_TO_CROOT] * ( 1. - s->value[EFF_LIVE_TOTAL_WOOD_FRAC]) / s->value[CN_DEAD_WOODS]);

		n_to_croot   = 0.;
	}

	/* branch */
	if (s->value[C_TO_BRANCH] > 0.)
	{
		s->value[N_TO_BRANCH] = s->value[C_TO_BRANCH] / s->value[CN_LIVE_WOODS];

		n_to_branch   = s->value[N_TO_BRANCH];

	}
	else
	{
		s->value[N_TO_BRANCH] = (s->value[C_TO_BRANCH] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] / s->value[CN_LIVE_WOODS]) +
				(s->value[C_TO_BRANCH] * ( 1. - s->value[EFF_LIVE_TOTAL_WOOD_FRAC]) / s->value[CN_DEAD_WOODS]);

		n_to_branch   = 0.;
	}

	/*****************************************************************************************************************************/

	/*** compute daily nitrogen demand ***/

	/* daily nitrogen demand */
	s->value[NPP_tN] = n_to_leaf + n_to_froot + n_to_stem + n_to_croot + n_to_branch + n_to_fruit + n_to_reserve;

	/* tN/Cell/day -> gC/m2/day */
	s->value[NPP_gN] = s->value[NPP_tN] * 1e6 / g_settings->sizeCell;

	/* daily Nitrogen demand */
	s->value[TREE_N_DEMAND] = s->value[NPP_gN];

	//fixme
	if (s->value[TREE_N_DEMAND] > c->soilN)
	{
		//todo back to partitioning-allocation routine and recompute both NPP in gC and NPP in gN based on the available soil nitrogen content
	}

	logger(g_debug_log, "N_TO_LEAF    = %f tN/cell/day\n", s->value[N_TO_LEAF]);
	logger(g_debug_log, "N_TO_FROOT   = %f tN/cell/day\n", s->value[N_TO_FROOT]);
	logger(g_debug_log, "N_TO_CROOT   = %f tN/cell/day\n", s->value[N_TO_CROOT]);
	logger(g_debug_log, "N_TO_STEM    = %f tN/cell/day\n", s->value[N_TO_STEM]);
	logger(g_debug_log, "N_TO_RESERVE = %f tN/cell/day\n", s->value[N_TO_RESERVE]);
	logger(g_debug_log, "N_TO_BRANCH  = %f tN/cell/day\n", s->value[N_TO_BRANCH]);
	logger(g_debug_log, "N_TO_FRUIT   = %f tN/cell/day\n", s->value[N_TO_FRUIT]);



}
