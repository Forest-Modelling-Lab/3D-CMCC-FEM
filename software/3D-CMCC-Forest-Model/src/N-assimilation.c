/*
 * N-assimilation.c
 *
 *  Created on: 28/ott/2013
 *      Author: alessio
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <N-assimilation.h>
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;

void nitrogen_assimilation(species_t *const s)
{
	logger(g_debug_log, "\n**POTENTIAL NITROGEN ASSIMILATION **\n");

	/* computing Nitrogen pools (tC/ha) */

	if ( ! s->value[LEAF_C] )
	{
		s->value[LEAF_N]            = 0.;
		s->value[LEAF_SUN_N]        = 0.;
		s->value[LEAF_SHADE_N]      = 0.;

	}
	else
	{
		s->value[LEAF_N]            = s->value[LEAF_C]            / s->value[CN_LEAVES];
		s->value[LEAF_SUN_N]        = s->value[LEAF_SUN_C]        / s->value[CN_LEAVES];
		s->value[LEAF_SHADE_N]      = s->value[LEAF_SHADE_C]      / s->value[CN_LEAVES];
	}

	if ( ! s->value[FROOT_C] )
	{
		s->value[FROOT_N]           = 0.;
	}
	else
	{
		s->value[FROOT_N]           = s->value[FROOT_C]           / s->value[CN_FINE_ROOTS];
	}
	//s->value[LEAF_FALLING_N]  = s->value[LEAF_FALLING_C]        / s->value[CN_FALLING_LEAVES];
	s->value[STEM_LIVEWOOD_N]   = s->value[STEM_LIVEWOOD_C]       / s->value[CN_LIVEWOOD];
	s->value[STEM_DEADWOOD_N]   = s->value[STEM_DEADWOOD_C]       / s->value[CN_DEADWOOD];
	s->value[STEM_N]            = s->value[STEM_LIVEWOOD_N]       + s->value[STEM_DEADWOOD_N];
	s->value[CROOT_LIVEWOOD_N]  = s->value[CROOT_LIVEWOOD_C]      / s->value[CN_LIVEWOOD];
	s->value[CROOT_DEADWOOD_N]  = s->value[CROOT_DEADWOOD_C]      / s->value[CN_DEADWOOD];
	s->value[CROOT_N]           = s->value[CROOT_LIVEWOOD_N]      + s->value[CROOT_DEADWOOD_N];
	s->value[BRANCH_LIVEWOOD_N] = s->value[BRANCH_LIVEWOOD_C]     / s->value[CN_LIVEWOOD];
	s->value[BRANCH_DEADWOOD_N] = s->value[BRANCH_DEADWOOD_C]     / s->value[CN_DEADWOOD];
	s->value[BRANCH_N]          = s->value[BRANCH_LIVEWOOD_N]     + s->value[BRANCH_DEADWOOD_N];

	/* note: special case reserve */
	s->value[RESERVE_N]        += s->value[N_TO_RESERVE];

}


void potential_nitrogen_assimilation ( cell_t *const c, species_t *const s )
{

	double n_to_leaf;
	double n_to_froot;
	double n_to_stem;
	double n_to_croot;
	double n_to_branch;
	double n_to_fruit;

	/* it allocates Daily assimilated Nitrogen for both deciduous and evergreen and compute Nitrogen demand */

	logger(g_debug_log, "\n**POTENTIAL NITROGEN ASSIMILATION **\n");

	/*** update class level carbon Nitrogen pools ***/

	/* leaf */
	s->value[N_TO_LEAF]   = s->value[C_TO_LEAF]   / s->value[CN_LEAVES];

	if ( s->value[N_TO_LEAF]   > 0. ) n_to_leaf   = s->value[N_TO_LEAF];
	else                              n_to_leaf   = 0.;

	/* fine root */
	s->value[N_TO_FROOT]  = s->value[C_TO_FROOT]  / s->value[CN_FINE_ROOTS];

	if ( s->value[N_TO_FROOT]  > 0. ) n_to_froot  = s->value[N_TO_FROOT];
	else                              n_to_froot  = 0.;

	/* fruit */
	s->value[N_TO_FRUIT]   = s->value[C_TO_FRUIT] / s->value[CN_LEAVES];//FIXME IT USES CN_LEAVES INSTEAD A CN_FRUITS

	if ( s->value[N_TO_FRUIT]  > 0. ) n_to_fruit  = s->value[N_TO_FRUIT];
	else                              n_to_fruit  = 0.;

	/* note: special case reserve */
	s->value[N_TO_RESERVE] = s->value[N_LEAF_TO_RESERVE] + s->value[N_FROOT_TO_RESERVE] + s->value[N_CROOT_TO_RESERVE] + s->value[N_BRANCH_TO_RESERVE];

	//note: if carbon transfer fluxes are positive than carbon and nitrogen to move are considered as "live tissues"
	//note: otherwise e.g. they need to be considered in their live and dead wood parts

	/* stem */
	if (s->value[C_TO_STEM] > 0.)
	{
		s->value[N_TO_STEM] = s->value[C_TO_STEM] / s->value[CN_LIVEWOOD];

		n_to_stem  = s->value[N_TO_STEM];

	}
	else
	{
		s->value[N_TO_STEM] = (s->value[C_TO_STEM] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] / s->value[CN_LIVEWOOD]) +
				(s->value[C_TO_STEM] * ( 1. - s->value[EFF_LIVE_TOTAL_WOOD_FRAC]) / s->value[CN_DEADWOOD]);

		n_to_stem   = 0.;
	}

	/* coarse root */
	if (s->value[C_TO_CROOT] > 0.)
	{
		s->value[N_TO_CROOT] = s->value[C_TO_CROOT] / s->value[CN_LIVEWOOD];

		n_to_croot   = s->value[N_TO_CROOT];

	}
	else
	{
		s->value[N_TO_CROOT] = (s->value[C_TO_CROOT] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] / s->value[CN_LIVEWOOD]) +
				(s->value[C_TO_CROOT] * ( 1. - s->value[EFF_LIVE_TOTAL_WOOD_FRAC]) / s->value[CN_DEADWOOD]);

		n_to_croot   = 0.;
	}

	/* branch */
	if (s->value[C_TO_BRANCH] > 0.)
	{
		s->value[N_TO_BRANCH] = s->value[C_TO_BRANCH] / s->value[CN_LIVEWOOD];

		n_to_branch   = s->value[N_TO_BRANCH];

	}
	else
	{
		s->value[N_TO_BRANCH] = (s->value[C_TO_BRANCH] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] / s->value[CN_LIVEWOOD]) +
				(s->value[C_TO_BRANCH] * ( 1. - s->value[EFF_LIVE_TOTAL_WOOD_FRAC]) / s->value[CN_DEADWOOD]);

		n_to_branch   = 0.;
	}


	/*****************************************************************************************************************************/

	/*** compute daily nitrogen demand ***/

	/* daily nitrogen demand */
	s->value[NPP_tN_DEMAND] = n_to_leaf + n_to_froot + n_to_stem + n_to_croot + n_to_branch + n_to_fruit /*+ n_to_reserve*/;

	/* tN/Cell/day -> gC/m2/day */
	s->value[NPP_gN_DEMAND] = s->value[NPP_tN_DEMAND] * 1e6 / g_settings->sizeCell;

	/* daily Nitrogen demand */
	s->value[TREE_N_DEMAND] = s->value[NPP_gN_DEMAND];

	//fixme
	if (s->value[NPP_gN_DEMAND] > c->soilN)
	{
		//todo back to partitioning-allocation routine and recompute both NPP in gC and NPP in gN based on the available soil nitrogen content
	}
}
