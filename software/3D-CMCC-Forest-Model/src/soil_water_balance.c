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
extern void Get_soil_water_balance (CELL *c, const MET_DATA *const met, int month, int day)
{
	Log("\n*********GET SOIL WATER BALACE************\n");
	Log("ASW before computation = %f mm\n", c->available_soil_water);
	Log("Rainfall = %f mm/m^2/day\n", met[month].d[day].rain);
	Log("water to soil from rainfall less interception = %f mm/m^2/day\n", c->water_to_soil);
	Log("snow_melt = %f mm/m^2/day\n", c->snow_melt);
	Log("Water to atmosphere due to ET = %f mm/m^2/day\n", c->water_to_atmosphere);


	/*compute balance*/
	c->available_soil_water += (met[month].d[day].rain + c->water_to_soil + c->snow_melt - c->water_to_atmosphere);
	Log("ASW after computation = %f\n", c->available_soil_water);

	/*check*/
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
		c->runoff = c->available_soil_water -c->max_asw;
		Log("Runoff = %f\n", c->runoff);
	}
	else
	{
		c->runoff = 0.0;
		Log("Runoff = %f\n", c->runoff);
	}






	c->daily_tot_w_flux = c->water_to_soil - (c->water_to_atmosphere - c->runoff);
	Log("Daily_w_flux = %f \n", c->daily_tot_w_flux);

	c->monthly_tot_w_flux += c->daily_tot_w_flux;
	c->annual_tot_w_flux += c->daily_tot_w_flux;


	c->swc= (c->available_soil_water * 100)/c->max_asw;
	Log("SWC = %g(%vol)\n", c->swc);


}
