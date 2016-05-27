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
#include "logger.h"

extern logger_t* g_log;

void Check_carbon_balance (CELL *c)
{

	double carbon_in;
	double carbon_out;
	double carbon_stored;


	logger(g_log, "\n*********CHECK CARBON BALANCE************\n");

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
		logger(g_log, "NO CHECK CARBON BALANCE FOR THE FIRST DAY\n");
		logger(g_log, "carbon balance (carbon_in - carbon_out - carbon_stored) = %f\n", c->carbon_balance);
	}
	else
	{
		if (fabs(c->old_carbon_balance - c->carbon_balance) > 1e-4 )
		{
			logger(g_log, "in\n");
			logger(g_log, "c->daily_gpp = %f\n", c->daily_gpp);
			logger(g_log, "out\n");
			logger(g_log, "c->daily_tot_aut_resp = %f\n",c->daily_aut_resp);
			logger(g_log, "stored\n");
			logger(g_log, "c->daily_leaf_carbon = %f\n", c->daily_leaf_carbon);
			logger(g_log, "c->daily_stem_carbon = %f\n", c->daily_stem_carbon);
			logger(g_log, "c->daily_fine_root_carbon = %f\n", c->daily_fine_root_carbon);
			logger(g_log, "c->daily_coarse_root_carbon = %f\n", c->daily_coarse_root_carbon);
			logger(g_log, "c->daily_branch_carbon = %f\n", c->daily_branch_carbon);
			logger(g_log, "c->daily_reserve_carbon = %f\n", c->daily_reserve_carbon);
			logger(g_log, "c->daily_litter_carbon = %f\n", c->daily_litter_carbon);
			logger(g_log, "carbon in = %f\n", carbon_in);
			logger(g_log, "carbon out = %f\n", carbon_out);
			logger(g_log, "carbon stored = %f\n", carbon_stored);
			logger(g_log, "differences in balance (old - current)= %f\n", c->old_carbon_balance - c->carbon_balance);
			logger(g_log, "...FATAL ERROR IN carbon balance\n");
			logger(g_log, "DOY CB = %d\n", c->doy);
			exit(1);
		}
		else
		{
			logger(g_log, "...ok carbon balance\n");
		}
	}
	c->old_carbon_balance = c->carbon_balance;
}

void Check_water_balance (CELL *c)
{
	double water_in;
	double water_out;
	double water_stored;

	logger(g_log, "\n*********CHECK WATER BALANCE************\n");
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
		logger(g_log, "NO CHECK WATER BALANCE FOR THE FIRST DAY\n");
		logger(g_log, "water balance (water_in - water_out - water_stored) = %f\n", c->water_balance);
	}
	else
	{
		if (fabs(c->old_water_balance - c->water_balance) > 1e-4 )
		{
			logger(g_log, "in\n");
			logger(g_log, "c->prcp_rain = %f\n", c->prcp_rain);
			logger(g_log, "c->prcp_snow = %f\n", c->prcp_snow);
			logger(g_log, "out\n");
			logger(g_log, "c->daily_tot_c_transp = %f\n", c->daily_c_transp);
			logger(g_log, "c->daily_tot_c_int = %f\n", c->daily_c_int);
			logger(g_log, "c->daily_tot_c_evapo = %f\n", c->daily_c_evapo);
			logger(g_log, "c->soil_evaporation = %f\n", c->daily_soil_evapo);
			logger(g_log, "c->snow_subl = %f\n", c->snow_subl);
			logger(g_log, "c->snow_melt = %f\n", c->snow_melt);
			logger(g_log, "c->out_flow = %f\n", c->out_flow);
			logger(g_log, "stored\n");
			logger(g_log, "delta c->asw = %f\n", (c->asw - c->old_asw));
			logger(g_log, "c->snow_pack = %f\n", c->snow_pack);
			logger(g_log, "c->daily_tot_c_water_stored = %f\n", c->daily_c_water_stored);
			logger(g_log, "c->asw = %f\n", c->asw);
			logger(g_log, "c->old_asw = %f\n", c->old_asw);
			logger(g_log, "water in = %f\n", water_in);
			logger(g_log, "water out = %f\n", water_out);
			logger(g_log, "water stored = %f\n", water_stored);
			logger(g_log, "water balance = %f\n", c->water_balance);
			logger(g_log, "old water balance = %f\n", c->old_water_balance);
			logger(g_log, "differences in balance (old - current)= %f\n", c->old_water_balance - c->water_balance);
			logger(g_log, "...FATAL ERROR IN water balance\n");
			logger(g_log, "DOY = %d\n", c->doy);
			exit(1);
		}
		else
		{
			logger(g_log, "...ok water balance\n");
		}
	}
	c->old_water_balance = c->water_balance;
}
