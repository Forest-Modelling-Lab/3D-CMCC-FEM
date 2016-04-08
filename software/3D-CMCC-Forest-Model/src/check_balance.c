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

void Check_carbon_balance (CELL *c)
{

	double carbon_in;
	double carbon_out;
	double carbon_stored;


	Log("\n*********CHECK CARBON BALANCE************\n");

	/* DAILY CHECK ON CARBON BALANCE */

	/* sum of sources */
	carbon_in = c->daily_gpp;

	/* sum of sinks */
	carbon_out = c->daily_maint_resp + c->daily_growth_resp;

	/* sum of current storage */
	carbon_stored = c->daily_leaf_carbon + c->daily_stem_carbon +
			c->daily_fine_root_carbon + c->daily_coarse_root_carbon +
			c->daily_branch_carbon + c->daily_reserve_carbon + c->daily_litter_carbon;

	c->carbon_balance = carbon_in - carbon_out -carbon_stored;

	if(c->years_count == 0 && c->doy == 1)
	{
		Log("NO CHECK CARBON BALANCE FOR THE FIRST DAY\n");
		Log("carbon balance (carbon_in - carbon_out - carbon_stored) = %f\n", c->carbon_balance);
	}
	else
	{
		if (fabs(c->old_carbon_balance - c->carbon_balance) > 1e-4 )
		{
			Log("in\n");
			Log("c->daily_gpp = %f\n", c->daily_gpp);
			Log("out\n");
			Log("c->daily_tot_aut_resp = %f\n",c->daily_aut_resp);
			Log("stored\n");
			Log("c->daily_leaf_carbon = %f\n", c->daily_leaf_carbon);
			Log("c->daily_stem_carbon = %f\n", c->daily_stem_carbon);
			Log("c->daily_fine_root_carbon = %f\n", c->daily_fine_root_carbon);
			Log("c->daily_coarse_root_carbon = %f\n", c->daily_coarse_root_carbon);
			Log("c->daily_branch_carbon = %f\n", c->daily_branch_carbon);
			Log("c->daily_reserve_carbon = %f\n", c->daily_reserve_carbon);
			Log("c->daily_litter_carbon = %f\n", c->daily_litter_carbon);
			Log("carbon in = %f\n", carbon_in);
			Log("carbon out = %f\n", carbon_out);
			Log("carbon stored = %f\n", carbon_stored);
			Log("differences in balance (old - current)= %f\n", c->old_carbon_balance - c->carbon_balance);
			Log("...FATAL ERROR IN carbon balance\n");
			Log("DOY CB = %d\n", c->doy);
			exit(1);
		}
		else
		{
			Log("...ok carbon balance\n");
		}
	}
	c->old_carbon_balance = c->carbon_balance;
}

void Check_water_balance (CELL *c)
{
	double water_in;
	double water_out;
	double water_stored;

	Log("\n*********CHECK WATER BALANCE************\n");
	/* DAILY CHECK ON WATER BALANCE */


	/*sum of sources (rain + snow)*/
	water_in = c->prcp_rain + c->prcp_snow + c->snow_melt;

	/* sum of sinks*/
	//comment: snow_subl is not considered here otherwise it could accounted twice (out and stored)
	//fixme check for daily_c_evapo
	water_out = c->daily_c_transp /* + c->daily_c_evapo */+ c->daily_c_int + c->daily_soil_evapo /*+ c->snow_subl*/ + c->out_flow;

	/* sum of current storage */
	//fixme check for daily_c_water_stored
	water_stored = (c->asw - c->old_asw) + /* c->daily_c_water_stored + */ c->prcp_snow;

	/* check balance */
	c->water_balance = water_in - water_out - water_stored;

	if(c->years_count == 0 && c->doy == 1)
	{
		Log("NO CHECK WATER BALANCE FOR THE FIRST DAY\n");
		Log("water balance (water_in - water_out - water_stored) = %f\n", c->water_balance);
	}
	else
	{
		if (fabs(c->old_water_balance - c->water_balance) > 1e-4 )
		{
			Log("in\n");
			Log("c->prcp_rain = %f\n", c->prcp_rain);
			Log("c->prcp_snow = %f\n", c->prcp_snow);
			Log("out\n");
			Log("c->daily_tot_c_transp = %f\n", c->daily_c_transp);
			Log("c->daily_tot_c_int = %f\n", c->daily_c_int);
			Log("c->daily_tot_c_evapo = %f\n", c->daily_c_evapo);
			Log("c->soil_evaporation = %f\n", c->daily_soil_evapo);
			Log("c->snow_subl = %f\n", c->snow_subl);
			Log("c->snow_melt = %f\n", c->snow_melt);
			Log("c->out_flow = %f\n", c->out_flow);
			Log("stored\n");
			Log("delta c->asw = %f\n", (c->asw - c->old_asw));
			Log("c->snow_pack = %f\n", c->snow_pack);
			Log("c->daily_tot_c_water_stored = %f\n", c->daily_c_water_stored);
			Log("c->asw = %f\n", c->asw);
			Log("c->old_asw = %f\n", c->old_asw);
			Log("water in = %f\n", water_in);
			Log("water out = %f\n", water_out);
			Log("water stored = %f\n", water_stored);
			Log("water balance = %f\n", c->water_balance);
			Log("old water balance = %f\n", c->old_water_balance);
			Log("differences in balance (old - current)= %f\n", c->old_water_balance - c->water_balance);
			Log("...FATAL ERROR IN water balance\n");
			Log("DOY = %d\n", c->doy);
			exit(1);
		}
		else
		{
			Log("...ok water balance\n");
		}
	}
	c->old_water_balance = c->water_balance;
}
