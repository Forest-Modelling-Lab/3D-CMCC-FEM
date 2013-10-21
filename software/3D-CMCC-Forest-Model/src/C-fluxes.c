/*
 * C-fluxes.c
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


void Get_C_fluxes (SPECIES *const s, CELL *const c, int height, int day, int month)
{
	int i;
	//compute carbon balance between photosynthesis and autotrophic respiration
	//recompute GPP
	Log("\nGET_C-Fluxes_ROUTINE\n");

	Log("respiration fraction = %g %%\n", (s->value[GPP_g_C]* 100)/s->value[TOTAL_AUT_RESP]);
	s->value[GPP_g_C] -= s->value[TOTAL_AUT_RESP];
	Log("C-fluxes = %g gC/m^2 day/month\n", s->value[GPP_g_C]);

	i = c->heights[height].z;

	c->daily_gpp[i] += s->value[GPP_g_C];
	c->monthly_gpp[i] += s->value[GPP_g_C];
	c->annual_gpp[i] += s->value[GPP_g_C];

	c->daily_tot_gpp += s->value[GPP_g_C];
	c->monthly_tot_gpp += s->value[GPP_g_C];
	c->annual_tot_gpp += s->value[GPP_g_C];

	Log("***************************** ANNUAL GPP *************************** \n");

	Log("*********************** CLASS LEVEL ANNUAL GPP ********************** \n");
	//class level
	s->value[YEARLY_POINT_GPP_G_C] += s->value[POINT_GPP_g_C];
	Log("-CLASS LEVEL\n");
	Log("-CLASS LEVEL Yearly GPP (absolute) = %g gC/m^2 yr\n", s->value[YEARLY_POINT_GPP_G_C]);

	Log("*********************** STAND LEVEL ANNUAL GPP ********************** \n");

	//cell level
	c->gpp += s->value[POINT_GPP_g_C];
	Log("-CELL LEVEL\n");
	Log("-CELL LEVEL Yearly GPP (absolute) = %g gC/m^2 yr\n", c->gpp);


}


