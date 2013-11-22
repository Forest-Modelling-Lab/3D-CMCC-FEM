/*
 * soil_water_balance.c
 *
 *  Created on: 12/nov/2012
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

//fixme  maybe it can be moved to soil_model.c
extern void Get_soil_water_balance (CELL *c,  const MET_DATA *const met, int month, int day)
{

	Log("*********GET %c SOIL WATER BALACE************\n", settings->time);
	/*compute water to atmosphere*/
	c->water_to_atmosphere = c->daily_evapotranspiration;
	Log("Water lost to atmosphere = %f \n", c->water_to_atmosphere);



	/*compute water to soil pool*/
	c->water_to_soil = met[month].d[day].rain - c->daily_tot_c_int;
	Log("Water to soil = %f \n", c->water_to_soil);

	/*Take off Evapotranspiration*/
	c->available_soil_water -= c->daily_evapotranspiration ;
	Log("ASW at the END of day/month less Evapotraspiration = %f mm\n", c->available_soil_water);

	if ( c->available_soil_water < c->max_asw * site->min_frac_maxasw)
	{
		Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
		c->available_soil_water = c->max_asw * site->min_frac_maxasw;
		Log("ASW = %f\n", c->available_soil_water);
	}
	if ( c->available_soil_water > c->max_asw)
	{
		Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
		c->available_soil_water = c->max_asw;
		Log("Available soil water = %f\n", c->available_soil_water);
	}
}
