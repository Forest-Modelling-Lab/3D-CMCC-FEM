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
	double old_water_balance;
	double water_in;
	double water_out;
	double water_stored;
	double water_balance;
	Log("\n*********GET SOIL WATER BALACE************\n");

	/* DAILY CHECK ON WATER BALANCE */

	/*sum of sources*/
	water_in = met[month].d[day].rain;
	Log("water in = %f\n", water_in);

	/*sum of sinks*/
	//fixme probablty canopy_int = canopy_evapo coauses problem fix it!!
	water_out = c->daily_tot_c_transp + c->soil_evaporation + c->snow_subl + c->runoff ;
	Log("water out = %f\n", water_out);

	/* sum of current storage */
	water_stored = c->available_soil_water + c->snow_pack + c->daily_tot_c_int;
	Log("water stored = %f\n", water_stored);

	/* check balance */
	water_balance = water_in - water_out - water_stored;
	Log("water balance = %f\n", water_balance);


//	if(month != 0 && day != 0)
//	{
//		if (fabs(old_water_balance - water_balance) > 1e-4 )
//		{
//			ERROR(water_balance, "water balance");
//		}
//		else
//		{
//			/*update balance*/
//			c->available_soil_water += (met[month].d[day].rain + c->water_to_soil + c->snow_melt - c->water_to_atmosphere);
//			Log("ASW after computation = %f\n", c->available_soil_water);
//		}
//	}
//	else
//	{
//		old_water_balance = 1.0;
//	}
//	old_water_balance = water_balance;

	/*update balance*/
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
