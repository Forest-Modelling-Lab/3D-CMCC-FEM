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

	Log("*********GET %c SOIL WATER BALACE************\n", settings->time);
	/*compute water to atmosphere*/
	c->water_to_atmosphere = c->daily_tot_et;
	Log("Water to atmosphere = %f \n", c->water_to_atmosphere);



	/*compute water that goes into the soil pool*/
	if(c->daily_snow !=0)
	{
		c->water_to_soil = met[month].d[day].rain - c->daily_tot_et;
		Log("Water to soil = %f \n", c->water_to_soil);
	}
	else
	{
		c->water_to_soil -=  c->daily_tot_et;
		Log("Water to soil = %f \n", c->water_to_soil);
	}

	c->daily_tot_w_flux = c->water_to_soil - (c->water_to_atmosphere - c->runoff);
	Log("Daily_w_flux = %f \n", c->daily_tot_w_flux);

	c->monthly_tot_w_flux += c->daily_tot_w_flux;
	c->annual_tot_w_flux += c->daily_tot_w_flux;

	/*Take off Evapotranspiration*/
	c->available_soil_water -= c->daily_tot_et ;
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
	c->swc= (c->available_soil_water * 100)/c->max_asw;
	Log("SWC = %g(%vol)\n", c->swc);


}
