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




extern void Get_evapotranspiration (CELL *c)
{

	Log("\nGET_EVAPOTRANSPIRATION_ROUTINE\n");


	c->daily_tot_et = c->daily_tot_c_evapotransp + c->soil_evaporation;
	Log("Daily total EVAPOTRANSPIRATION = %f \n", c->daily_tot_et);

	/*compute water to atmosphere*/
	c->water_to_atmosphere = c->daily_tot_et;
	Log("water to atmosphere = %f \n", c->water_to_atmosphere);
	c->monthly_tot_et += c->daily_tot_et;
	Log("Monthly total EVAPOTRANSPIRATION = %f \n", c->monthly_tot_et);
	//fixme add annual_tot_et

}



