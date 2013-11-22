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
	//compute carbon balance between photosynthesis and autotrophic respiration
	//recompute GPP
	Log("\nGET_C-Fluxes_ROUTINE\n");

	c->daily_c_flux = s->value[GPP_g_C] - s->value[TOTAL_AUT_RESP];
	Log("c-flux = double\n", c->daily_c_flux);


}


