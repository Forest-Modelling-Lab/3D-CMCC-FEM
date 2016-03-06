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


void Check_water_balance (CELL *c, const MET_DATA *const met, int month, int day)
{
	double water_in;
	double water_out;
	double water_stored;

	Log("\n*********CHECK WATER BALANCE************\n");


	//FIXME OVERALL FUNCTION DOESN'T WORK!!

	/* DAILY CHECK ON WATER BALANCE */

	/*sum of sources (rain + snow)*/
	water_in = met[month].d[day].prcp;


	/*sum of sinks*/
	//fixme probably canopy_int = canopy_evapo will coauses problem fix it!!
	water_out = c->daily_tot_c_transp + c->daily_tot_c_int + c->soil_evaporation + c->snow_subl + c->runoff;

	/* sum of current storage */
	water_stored = c->available_soil_water;


	/* check balance */
	c->water_balance = water_in - water_out - water_stored;
	//c->water_balance = water_in - water_out - c->old_water_balance;

	if(c->years_count == 0 && c->doy == 1)
	{
		Log("NO CHECK WATER BALANCE FOR THE FIRST DAY\n");
		c->old_water_balance = c->water_balance;

		Log("water in = %f\n", water_in);
		Log("water out = %f\n", water_out);
		Log("water stored = %f\n", water_stored);
		Log("water balance = %f\n", c->water_balance);
		Log("old water balance = %f\n", c->old_water_balance);
	}
	else
	{
		if (fabs(c->old_water_balance - c->water_balance) > 1e-4 )
		{
			Log("...FATAL ERROR IN water balance\n");
			Log("water in = %f\n", water_in);
			Log("water out = %f\n", water_out);
			Log("water stored = %f\n", water_stored);
			Log("water balance = %f\n", c->water_balance);
			Log("old water balance = %f\n", c->old_water_balance);
			Log("differences in balance (old - current)= %f\n", c->old_water_balance - c->water_balance);
			ERROR(c->water_balance, "water balance");
		}
		else
		{
			Log("...ok water balance\n");
			Log("water balance = %f\n", c->water_balance);
			Log("old water balance = %f\n", c->old_water_balance);
			Log("asw = %f\n", c->available_soil_water);
			c->old_water_balance = c->water_balance;
		}
	}
}
