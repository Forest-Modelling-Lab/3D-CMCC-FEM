/*
 * evapotranspiration.c
 *
 *  Created on: 06/nov/2013
 *      Author: alessio
 */




/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"




extern void Get_evapotranspiration (SPECIES *const s, CELL *c, const MET_DATA *const met, int month, int day, int height)
{
	Log("\nGET_EVAPOTRANSPIRATION_ROUTINE\n");

	c->daily_evapotranspiration = c->daily_tot_c_evapotransp + c->soil_evaporation;
	Log("Daily total EVAPOTRANSPIRATION = %g \n", c->daily_evapotranspiration);
}



