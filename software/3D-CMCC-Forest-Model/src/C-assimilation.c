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
extern logger_t* g_log;


void Carbon_assimilation(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	logger (g_log, "\n**C-ASSIMILATION_ROUTINE**\n");

	/* NPP computation is based on ground surface area */
	s->value[NPP_gC] = s->value[DAILY_GPP_gC] - s->value[TOTAL_AUT_RESP];
	s->value[NPP_tC] = s->value[NPP_gC] / 1000000 * g_settings->sizeCell;
	s->value[NPP_tDM] = ((s->value[NPP_gC] * GC_GDM) / 1000000) * g_settings->sizeCell;

	logger(g_log, "Daily NPP = %f gC/m^2 ground surface area/day\n", s->value[NPP_gC]);
	logger(g_log, "Daily NPP = %f tC/area/day\n", s->value[NPP_tC]);
	logger(g_log, "Daily NPP = %f tDM/area/day\n",  s->value[NPP_tDM]);

	c->daily_npp_tDM += s->value[NPP_tDM];
	c->daily_npp_gC += s->value[NPP_gC];
	c->monthly_npp_tDM += s->value[NPP_tDM];
	c->monthly_npp_gC += s->value[NPP_gC];
	c->annual_npp_tDM += s->value[NPP_tDM];
	c->annual_npp_gC += s->value[NPP_gC];

	logger(g_log, "*********************** CLASS LEVEL ANNUAL NPP ********************** \n");
	//class level
	s->value[YEARLY_NPP_tDM] += s->value[NPP_tDM];
	logger(g_log, "-CLASS LEVEL Yearly NPP (per area covered) = %f tDM/sizecell yr\n", s->value[YEARLY_NPP_tDM]);

	logger(g_log, "*********************** STAND LEVEL ANNUAL NPP ********************** \n");
	//cell level
	c->daily_npp_tDM += s->value[NPP_tDM];
	logger(g_log, "-CELL LEVEL Yearly NPP (per area covered) = %f tDM/sizecell yr\n", c->daily_npp_tDM);

}



