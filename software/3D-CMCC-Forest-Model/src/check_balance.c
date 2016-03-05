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


void check_water_balance (CELL *c, const MET_DATA *const met, int month, int day)
{
	double water_in;
	double water_out;
	double water_stored;

	Log("\n*********CHECK SOIL WATER BALANCE************\n");


	//FIXME OVERALL FUNCTION DOESN'T WORK!!

	/* DAILY CHECK ON WATER BALANCE */

	/*sum of sources*/
	water_in = met[month].d[day].rain;
	Log("water in = %f\n", water_in);

	/*sum of sinks*/
	//fixme probably canopy_int = canopy_evapo coauses problem fix it!!
	water_out = c->daily_tot_c_transp + c->daily_tot_c_int + c->soil_evaporation + c->snow_subl + c->runoff ;
	Log("water out = %f\n", water_out);

	/* sum of current storage */
	water_stored = c->available_soil_water + c->snow_pack ;
	Log("water stored = %f\n", water_stored);

	/* check balance */
	c->water_balance = water_stored - water_in - water_out;
	Log("water balance = %f\n", c->water_balance);

	Log("old water balance = %f\n", c->old_water_balance);

	if(month != 0 && day != 0)
	{
		if (fabs(c->water_balance - c->old_water_balance) > 1e-4 )
		{
			ERROR(c->water_balance, "water balance");
		}
		else
		{
			Log("...ok water balance\n");
		}
	}
	else
	{
		c->old_water_balance = 1.0;
	}
	c->old_water_balance = c->water_balance;
}
