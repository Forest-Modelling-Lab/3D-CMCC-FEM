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

void nitrogen_assimilation(species_t *const s)
{
	logger(g_debug_log, "\n**NITROGEN STOCK**\n");

	/* computing Nitrogen pools from tonsC/ha to gC/m-2 */

	/* leaf */
	s->value[LEAF_N] = s->value[LEAF_C] / s->value[CN_LEAVES];
	logger(g_debug_log, "Leaf nitrogen content = %g tN/area\n", s->value[LEAF_N]);

	//s->value[LEAF_FALLING_N] = s->value[LEAF_FALLING_C] / s->value[CN_FALLING_LEAVES];
	//logger(g_debug_log, "Leaf falling nitrogen content = %g tN/area\n", s->value[LEAF_FALLING_N]);

	/* fine root */
	s->value[FROOT_N] = s->value[FROOT_C] / s->value[CN_FINE_ROOTS];
	logger(g_debug_log, "Fine root nitrogen content = %g tN/area\n", s->value[FROOT_N]);

	/* stem */
	s->value[STEM_LIVE_WOOD_N] = s->value[STEM_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live stem nitrogen content = %g tN/area\n", s->value[STEM_LIVE_WOOD_N]);

	s->value[STEM_DEAD_WOOD_N] = s->value[STEM_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	logger(g_debug_log, "Dead stem nitrogen content = %g tN/area\n", s->value[STEM_DEAD_WOOD_N]);

	s->value[STEM_N] = s->value[STEM_LIVE_WOOD_N] + s->value[STEM_DEAD_WOOD_N];
	logger(g_debug_log, "Stem nitrogen content = %g tN/area\n", s->value[STEM_N]);

	/* coarse root */
	s->value[COARSE_ROOT_LIVE_WOOD_N] = s->value[CROOT_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live coarse root nitrogen content = %g tN/area\n", s->value[COARSE_ROOT_LIVE_WOOD_N]);

	s->value[COARSE_ROOT_DEAD_WOOD_N] = s->value[CROOT_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	logger(g_debug_log, "Dead coarse root nitrogen content = %g tN/area\n", s->value[COARSE_ROOT_DEAD_WOOD_N]);

	s->value[CROOT_N] = s->value[CROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_N];
	logger(g_debug_log, "Coarse root nitrogen content = %g tN/area\n", s->value[CROOT_N]);

	/* branch */
	s->value[BRANCH_LIVE_WOOD_N] = s->value[BRANCH_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live branch nitrogen content = %g tN/area\n", s->value[BRANCH_LIVE_WOOD_N]);

	s->value[BRANCH_DEAD_WOOD_N] = s->value[BRANCH_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	logger(g_debug_log, "Dead branch nitrogen content = %g tN/area\n", s->value[BRANCH_DEAD_WOOD_N]);

	s->value[BRANCH_N] = s->value[BRANCH_LIVE_WOOD_N] + s->value[BRANCH_DEAD_WOOD_N];
	logger(g_debug_log, "Branch nitrogen content = %g tN/area\n", s->value[BRANCH_N]);

}
