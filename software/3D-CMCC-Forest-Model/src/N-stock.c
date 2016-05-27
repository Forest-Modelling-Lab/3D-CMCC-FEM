/*
 * N-stock.c
 *
 *  Created on: 28/ott/2013
 *      Author: alessio
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

//FOLLOWING BIOME-BGC

void Nitrogen_stock (SPECIES *s)
{
	logger(g_log, "\n**NITROGEN STOCK**\n");

	/*computing Nitrogen content from tons DM/ha to gC/m^2 and then as in BIOME to gN*/
	s->value[LEAF_NITROGEN] = (s->value[LEAF_C] * 1000000.0 /settings->sizeCell) / s->value[CN_LEAVES];
	logger(g_log, "Foliage nitrogen content = %f gN/m^2\n", s->value[LEAF_NITROGEN]);
	s->value[FINE_ROOT_NITROGEN] = (s->value[FINE_ROOT_C] * 1000000.0 /settings->sizeCell) / s->value[CN_FINE_ROOTS];
	logger(g_log, "Fine root nitrogen content = %f gN/m^2\n", s->value[FINE_ROOT_NITROGEN]);
	s->value[STEM_NITROGEN] = (s->value[STEM_LIVE_WOOD_C] * 1000000.0 /settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live stem nitrogen content = %f gN/m^2\n", s->value[STEM_NITROGEN]);
	s->value[COARSE_ROOT_NITROGEN] = (s->value[COARSE_ROOT_LIVE_WOOD_C] * 1000000.0 /settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live coarse root nitrogen content = %f gN/cell\n", s->value[COARSE_ROOT_NITROGEN]);
	s->value[BRANCH_NITROGEN] = (s->value[BRANCH_LIVE_WOOD_C] * 1000000.0 /settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live branch nitrogen content = %f gN/cell\n", s->value[BRANCH_NITROGEN]);
}
