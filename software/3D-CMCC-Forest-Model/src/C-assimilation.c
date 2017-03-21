/*
 * C-assimilation.c
 *
 *  Created on: 14/ott/2013
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "C-assimilation.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;


void carbon_assimilation(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger (g_debug_log, "\n**C-ASSIMILATION**\n");

	/* NPP computation is based on ground surface area */
	s->value[NPP_gC] = s->value[DAILY_GPP_gC] - s->value[TOTAL_AUT_RESP];

	s->value[NPP_tC] = s->value[NPP_gC] / 1000000 * g_settings->sizeCell;
	logger(g_debug_log, "Daily NPP = %f gC/m^2/day\n", s->value[NPP_gC]);
	logger(g_debug_log, "Daily NPP = %f tC/area/day\n", s->value[NPP_tC]);

	/* class level */
	s->value[MONTHLY_NPP_gC] += s->value[NPP_gC];
	s->value[YEARLY_NPP_gC] += s->value[NPP_gC];
	s->value[MONTHLY_NPP_tC] += s->value[NPP_tC];
	s->value[YEARLY_NPP_tC] += s->value[NPP_tC];

	/* cell level */
	c->daily_npp_gC += s->value[NPP_gC];
	c->monthly_npp_gC += s->value[NPP_gC];
	c->annual_npp_gC += s->value[NPP_gC];
	c->daily_npp_tC += s->value[NPP_tC];
	c->monthly_npp_tC += s->value[NPP_tC];
	c->annual_npp_tC += s->value[NPP_tC];

}



