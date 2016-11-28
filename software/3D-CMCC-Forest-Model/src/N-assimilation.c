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

	/* BIOME-BGC METHOD */

	/* computing Nitrogen pools from tons C/ha to gC/m^2 */

	s->value[LEAF_N] = s->value[LEAF_C] / s->value[CN_LEAVES];
	logger(g_debug_log, "Leaf nitrogen content = %g tN/area\n", s->value[LEAF_N]);

	s->value[LEAF_FALLING_N] = s->value[LEAF_FALLING_C] / s->value[CN_FALLING_LEAVES];
	logger(g_debug_log, "Leaf falling nitrogen content = %g tN/area\n", s->value[LEAF_FALLING_N]);

	s->value[FINE_ROOT_N] = s->value[FINE_ROOT_C] / s->value[CN_FINE_ROOTS];
	logger(g_debug_log, "Fine root nitrogen content = %g tN/area\n", s->value[FINE_ROOT_N]);

	s->value[STEM_N] = s->value[STEM_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live stem nitrogen content = %g tN/area\n", s->value[STEM_N]);

	s->value[COARSE_ROOT_N] = s->value[COARSE_ROOT_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live coarse root nitrogen content = %g tN/area\n", s->value[COARSE_ROOT_N]);

	s->value[BRANCH_N] = s->value[BRANCH_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live branch nitrogen content = %g tN/area\n", s->value[BRANCH_N]);

}
