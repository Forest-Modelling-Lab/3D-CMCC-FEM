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

	/* computing Nitrogen content from tons C/ha to gC/m^2 and then to gN */
	s->value[LEAF_NITROGEN] = (s->value[LEAF_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LEAVES];
	logger(g_log, "Leaf nitrogen content = %f gN/m2\n", s->value[LEAF_NITROGEN]);
	s->value[FINE_ROOT_NITROGEN] = (s->value[FINE_ROOT_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_FINE_ROOTS];
	logger(g_log, "Fine root nitrogen content = %f gN/m2\n", s->value[FINE_ROOT_NITROGEN]);
	s->value[STEM_NITROGEN] = (s->value[STEM_LIVE_WOOD_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live stem nitrogen content = %f gN/m2\n", s->value[STEM_NITROGEN]);
	s->value[COARSE_ROOT_NITROGEN] = (s->value[COARSE_ROOT_LIVE_WOOD_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live coarse root nitrogen content = %f gN/m2\n", s->value[COARSE_ROOT_NITROGEN]);
	s->value[BRANCH_NITROGEN] = (s->value[BRANCH_LIVE_WOOD_C] * 1000000.0 /g_settings->sizeCell) / s->value[CN_LIVE_WOODS];
	logger(g_log, "Live branch nitrogen content = %f gN/m2\n", s->value[BRANCH_NITROGEN]);
}
