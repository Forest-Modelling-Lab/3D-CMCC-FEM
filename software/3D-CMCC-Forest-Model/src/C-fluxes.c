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

	i = c->heights[height].z;
	//compute carbon balance between photosynthesis and autotrophic respiration
	//recompute GPP
	Log("\nGET_C-Fluxes_ROUTINE\n");

	s->value[C_FLUX] = s->value[GPP_g_C] - fabs(s->value[TOTAL_AUT_RESP]);
	Log("c-flux = %f gC m^2 day^-1\n", s->value[C_FLUX]);
	Log("c-flux = %f tDM ha^-1 day ^-1\n", ((s->value[C_FLUX] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell));

	c->daily_c_flux[i] = s->value[C_FLUX];
	c->daily_tot_c_flux += s->value[C_FLUX];
	c->daily_c_flux_tDM[i] += ((s->value[C_FLUX] * GC_GDM) / 1000000) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);;

}


