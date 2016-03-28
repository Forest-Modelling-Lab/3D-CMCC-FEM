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
#include "types.h"
#include "constants.h"


void Carbon_assimilation (SPECIES *const s, CELL *const c, int years, int month, int day, int height)
{
	int i;

	Log ("\n**C-ASSIMILATION_ROUTINE**\n");

	/* NPP computation is based on ground surface area */
	s->value[NPP_gC] = s->value[DAILY_GPP_gC] - s->value[TOTAL_AUT_RESP];
	s->value[NPP_tC] = s->value[NPP_gC] / 1000000 * settings->sizeCell;
	s->value[NPP_tDM] = ((s->value[NPP_gC] * GC_GDM) / 1000000) * settings->sizeCell;

	Log("Daily NPP = %f gC/m^2 ground surface area/day\n", s->value[NPP_gC]);
	Log("Daily NPP = %f tC/area/day\n", s->value[NPP_tC]);
	Log("Daily NPP = %f tDM/area/day\n",  s->value[NPP_tDM]);

	i = c->heights[height].z;
	c->layer_daily_npp_tDM[i] += s->value[NPP_tDM];
	c->layer_daily_npp_gC[i] += s->value[NPP_gC];
	c->layer_monthly_npp_tDM[i] += s->value[NPP_tDM];
	c->layer_monthly_npp_gC[i] += s->value[NPP_gC];
	c->layer_annual_npp_tDM[i] += s->value[NPP_tDM];
	c->layer_annual_npp_gC[i] += s->value[NPP_gC];

	c->daily_npp_tDM += s->value[NPP_tDM];
	c->daily_npp_gC += s->value[NPP_gC];
	c->monthly_npp_tDM += s->value[NPP_tDM];
	c->monthly_npp_gC += s->value[NPP_gC];
	c->annual_npp_tDM += s->value[NPP_tDM];
	c->annual_npp_gC += s->value[NPP_gC];

	Log("*********************** CLASS LEVEL ANNUAL NPP ********************** \n");
	//class level
	s->value[YEARLY_NPP_tDM] += s->value[NPP_tDM];
	Log("-CLASS LEVEL Yearly NPP (per area covered) = %f tDM/sizecell yr\n", s->value[YEARLY_NPP_tDM]);

	Log("*********************** STAND LEVEL ANNUAL NPP ********************** \n");
	//cell level
	c->daily_npp_tDM += s->value[NPP_tDM];
	Log("-CELL LEVEL Yearly NPP (per area covered) = %f tDM/sizecell yr\n", c->daily_npp_tDM);

}



