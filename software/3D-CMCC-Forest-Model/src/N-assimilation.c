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

#if 0
	/* as in BIOME-BGC nitrogen content in falling leaves is lower */

	/* deciduous */
	if((s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2) && s->counter[LEAF_FALL_COUNTER] == 1)
	{
		/* outside leaf fall*/
		if(!s->counter[LEAF_FALL_COUNTER])
		{
			s->value[LEAF_N] = (s->value[LEAF_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FALLING_LEAVES];
			logger(g_debug_log, "Leaf falling nitrogen content = %f gN/m2\n", s->value[LEAF_N]);
		}
		/* during leaf fall */
		else
		{
			//ALESSIOC todo todo todo considering a reduction in leaf nitrogen for falling leaves
			//currently I don't know how to tell to the model the amount of falling leaves
			//should be something like that using "LEAF_FALLING_C"
			//s->value[LEAF_N] = (s->value[LEAF_FALLING_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FALLING_LEAVES];
			logger(g_debug_log, "Leaf falling nitrogen content = %f gN/m2\n", s->value[LEAF_N]);
		}
	}
	/* evergreen */
	else
	{
		//ALESSIOC todo todo todo considering a reduction in leaf nitrogen for falling leaves
		//currently I don't know how to tell to the model the amount of falling leaves
		//should be something like that using "LEAF_FALLING_C"
		//s->value[LEAF_N] = (s->value[LEAF_FALLING_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FALLING_LEAVES];
	}
#endif

	s->value[LEAF_N] = s->value[LEAF_C] / s->value[CN_LEAVES];
	logger(g_debug_log, "Leaf nitrogen content = %g tN/area\n", s->value[LEAF_N]);

	s->value[FINE_ROOT_N] = s->value[FINE_ROOT_C] / s->value[CN_FINE_ROOTS];
	logger(g_debug_log, "Fine root nitrogen content = %g tN/area\n", s->value[FINE_ROOT_N]);

	s->value[STEM_N] = s->value[STEM_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live stem nitrogen content = %g tN/area\n", s->value[STEM_N]);

	s->value[COARSE_ROOT_N] = s->value[COARSE_ROOT_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live coarse root nitrogen content = %g tN/area\n", s->value[COARSE_ROOT_N]);

	s->value[BRANCH_N] = s->value[BRANCH_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "Live branch nitrogen content = %g tN/area\n", s->value[BRANCH_N]);

}
