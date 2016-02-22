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

	c->daily_tot_et = c->daily_tot_c_evapotransp + c->soil_evaporation;
	Log("Daily total EVAPOTRANSPIRATION = %f \n", c->daily_tot_et);
	if (met[month].d[day].n_days == 1)
	{
		c->monthly_tot_et = 0;
	}
	c->monthly_tot_et += c->daily_tot_et;
	Log("Monthly total EVAPOTRANSPIRATION = %f \n", c->monthly_tot_et);
	//fixme add annual_tot_et

}



