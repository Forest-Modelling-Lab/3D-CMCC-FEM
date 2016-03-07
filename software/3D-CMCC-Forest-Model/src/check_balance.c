/*
 * check_balance.c
 *
 *  Created on: 05/mar/2016
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Check_water_balance (CELL *c)
{
	double water_in;
	double water_out;
	double water_stored;

	Log("\n*********CHECK WATER BALANCE************\n");

	//FIXME OVERALL FUNCTION DOESN'T WORK!!

	/* DAILY CHECK ON WATER BALANCE */
//	Log("prcp = %f\n", met[month].d[day].prcp);
	Log("c->daily_rain = %f\n", c->daily_rain);
	Log("c->daily_snow = %f\n", c->daily_snow);
//	Log("c->daily_tot_c_transp = %f\n", c->daily_tot_c_transp);
//	Log("c->daily_tot_c_int = %f\n", c->daily_tot_c_int);
//	Log("c->soil_evaporation = %f\n", c->soil_evaporation);
	Log("c->snow_subl = %f\n", c->snow_subl);
	Log("c->snow_melt = %f\n", c->snow_melt);
//	Log("c->runoff = %f\n", c->runoff);
	Log("c->available_soil_water = %f\n", c->available_soil_water);
	Log("c->snow_pack = %f\n", c->snow_pack);
	Log("c->daily_tot_c_water_stored = %f\n", c->daily_tot_c_water_stored);

	//test have a look of state update
	/*sum of sources (rain + snow)*/
	water_in = c->daily_rain + c->snow_melt;

	/*sum of sinks*/
	water_out = c->daily_tot_c_transp + c->daily_tot_c_int + c->soil_evaporation + c->snow_subl + c->runoff;

	/* sum of current storage */
	water_stored = c->available_soil_water + c->daily_tot_c_water_stored + c->snow_pack;


	/* check balance */
	c->water_balance = water_in - water_out - water_stored;

	if(c->years_count == 0 && c->doy == 1)
	{
		Log("NO CHECK WATER BALANCE FOR THE FIRST DAY\n");
		Log("water in (c->daily_rain + c->daily_snow) = %f\n", water_in);
		Log("water out (c->daily_tot_c_transp + c->daily_tot_c_int + c->soil_evaporation + c->snow_subl + c->runoff) = %f\n", water_out);
		Log("water stored (c->available_soil_water + c->daily_tot_c_water_stored) = %f\n", water_stored);
		Log("water balance (water_in - water_out - water_stored) = %f\n", c->water_balance);
		Log("old water balance = %f\n", c->old_water_balance);
	}
	else
	{
		if (fabs(c->old_water_balance - c->water_balance) > 1e-4 )
		{
			Log("...FATAL ERROR IN water balance\n");
			Log("water in (c->daily_rain + c->daily_snow) = %f\n", water_in);
			Log("water out (c->daily_tot_c_transp + c->daily_tot_c_int + c->soil_evaporation + c->snow_subl + c->runoff) = %f\n", water_out);
			Log("water stored (c->available_soil_water + c->daily_tot_c_water_stored) = %f\n", water_stored);
			Log("water balance (water_in - water_out - water_stored) = %f\n", c->water_balance);
			Log("old water balance = %f\n", c->old_water_balance);
			Log("differences in balance (old - current)= %f\n", c->old_water_balance - c->water_balance);
			Log("DOY = %d\n", c->doy);
			//ERROR(c->water_balance, "water balance");
		}
		else
		{
			Log("...ok water balance\n");
			Log("water in = %f\n", water_in);
			Log("water out = %f\n", water_out);
			Log("water stored = %f\n", water_stored);
			Log("water balance = %f\n", c->water_balance);
			Log("old water balance = %f\n", c->old_water_balance);
			Log("differences in balance (old - current)= %f\n", c->old_water_balance - c->water_balance);
			Log("asw = %f\n", c->available_soil_water);
		}
	}
	c->old_water_balance = c->water_balance;
}
