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




extern void Evapotranspiration (CELL *c)
{

	Log("\n**EVAPOTRANSPIRATION_ROUTINE**\n");

	c->daily_et = c->daily_c_evapotransp + c->daily_soil_evapo;
	Log("Daily EVAPOTRANSPIRATION = %f \n", c->daily_et);
	c->monthly_et += c->daily_et;
	Log("Monthly EVAPOTRANSPIRATION = %f \n", c->monthly_et);
	c->annual_et += c->daily_et;
	Log("Annual EVAPOTRANSPIRATION = %f \n", c->annual_et);

	/*compute water to atmosphere*/
	c->water_to_atmosphere = c->daily_et;
	Log("water to atmosphere = %f \n", c->water_to_atmosphere);

}



