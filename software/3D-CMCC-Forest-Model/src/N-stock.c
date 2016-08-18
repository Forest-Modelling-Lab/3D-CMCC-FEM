/*
 * N-stock.c
 *
 *  Created on: 28/ott/2013
 *      Author: alessio
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "n-stock.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void nitrogen_stock(species_t *const s)
{
	logger(g_log, "\n**NITROGEN STOCK**\n");

	/* BIOME-BGC METHOD */

	/* computing Nitrogen pools from tons C/ha to gC/m^2 and then to gN */

	//test
	/* as in BIOME-BGC nitrogen content in falling leaves is lower */

	/* deciduous */
	if((s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.1) && s->counter[LEAF_FALL_COUNTER] == 1)
	{
		/* outside leaf fall*/
		if(!s->counter[LEAF_FALL_COUNTER])
		{
			s->value[LEAF_N] = (s->value[LEAF_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FALLING_LEAVES];
			logger(g_log, "Leaf falling nitrogen content = %f gN/m2\n", s->value[LEAF_N]);
		}
		/* during leaf fall */
		else
		{
			s->value[LEAF_N] = (s->value[LEAF_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FALLING_LEAVES];
			logger(g_log, "Leaf falling nitrogen content = %f gN/m2\n", s->value[LEAF_N]);
		}
	}
	/* evergreen */ //ALESSIOC todo todo todo
	else
	{

	}
	s->value[LEAF_N] = (s->value[LEAF_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LEAVES];
	logger(g_log, "Leaf nitrogen content = %f gN/m2\n", s->value[LEAF_N]);
	s->value[FINE_ROOT_N] = (s->value[FINE_ROOT_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FINE_ROOTS];
	logger(g_log, "Fine root nitrogen content = %f gN/m2\n", s->value[FINE_ROOT_N]);
	s->value[STEM_N] = (s->value[STEM_LIVE_WOOD_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live stem nitrogen content = %f gN/m2\n", s->value[STEM_N]);
	s->value[COARSE_ROOT_N] = (s->value[COARSE_ROOT_LIVE_WOOD_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live coarse root nitrogen content = %f gN/m2\n", s->value[COARSE_ROOT_N]);
	s->value[BRANCH_N] = (s->value[BRANCH_LIVE_WOOD_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live branch nitrogen content = %f gN/m2\n", s->value[BRANCH_N]);


}
