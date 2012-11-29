/*
 * soil_water_balance.c
 *
 *  Created on: 12/nov/2012
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

extern void Get_soil_water_balance (CELL *const c)
{

	Log("*********GET MONTHLY SOIL WATER BALACE************\n");
	/*Take off Evapotranspiration*/
	c->available_soil_water -= c->evapotranspiration ;

	Log("ASW at the END of month less Evapotraspiration = %g mm\n", c->available_soil_water);
	if ( c->available_soil_water < 0)
	{
		Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
		c->available_soil_water = c->max_asw * site->init_frac_maxasw;
		Log("ASW = %g\n", c->available_soil_water);
	}
	if ( c->available_soil_water > c->max_asw)
	{
		Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
		c->available_soil_water = c->max_asw;
		Log("Available soil water = %g\n", c->available_soil_water);
	}
}
