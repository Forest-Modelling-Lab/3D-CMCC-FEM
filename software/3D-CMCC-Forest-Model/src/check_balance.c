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
  /* DAILY CHECK ON CARBON BALANCE */

  /* sum of sources */
  carbon_in = c->daily_gpp;

  /* sum of sinks */
  carbon_out = c->daily_leaf_maint_resp + c->daily_stem_maint_resp+
      c->daily_fine_root_maint_resp + c->daily_branch_maint_resp+
      c->daily_coarse_root_maint_resp + c->daily_leaf_growth_resp+
      c->daily_stem_growth_resp + c->daily_fine_root_growth_resp+
      c->daily_branch_growth_resp + c->daily_coarse_root_growth_resp;

  /* sum of current storage */
  //test check if during leaf fall leaf_carbon becomes negative
  carbon_stored = c->daily_leaf_carbon + c->daily_stem_carbon +
	c->daily_fine_root_carbon + c->daily_coarse_root_carbon +
	c->daily_branch_carbon + c->daily_reserve_carbon;

  c->carbon_balance = carbon_in - carbon_out - carbon_stored;

  //test
  //a first attempt to
  if(c->years_count == 0 && c->doy == 1)
    {
      Log("NO CHECK CARBON BALANCE FOR THE FIRST DAY\n");
      Log("carbon balance (carbon_in - carbon_out - carbon_stored) = %f\n", c->carbon_balance);
    }
  else
    {
      if (fabs(c->old_carbon_balance - c->carbon_balance) > 1e-4 )
	{
	  Log("...FATAL ERROR IN carbon balance\n");
	  Log("DOY = %d\n", c->doy);
	  //ERROR(c->carbon_balance, "carbon balance");
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

  //FIXME OVERALL FUNCTION DOESN'T WORK!!

  /* DAILY CHECK ON WATER BALANCE */
  Log("c->prcp_rain = %f\n", c->prcp_rain);
  Log("c->prcp_snow = %f\n", c->prcp_snow);
  Log("c->daily_tot_c_transp = %f\n", c->daily_tot_c_transp);
  Log("c->daily_tot_c_int = %f\n", c->daily_tot_c_int);
  Log("c->daily_tot_c_evapo = %f\n", c->daily_tot_c_evapo);
  Log("c->soil_evaporation = %f\n", c->soil_evaporation);
  Log("c->snow_subl = %f\n", c->snow_subl);
  Log("c->snow_melt = %f\n", c->snow_melt);
  Log("c->out_flow = %f\n", c->out_flow);
  Log("delta c->asw = %f\n", (c->asw - c->old_asw));
  Log("c->snow_pack = %f\n", c->snow_pack);
  Log("c->daily_tot_c_water_stored = %f\n", c->daily_tot_c_water_stored);
  Log("c->asw = %f\n", c->asw);

  /*sum of sources (rain + snow)*/
  water_in = c->prcp_rain + c->prcp_snow + c->snow_melt;

  /*sum of sinks*/
  //comment: snow_subl is not considered here otherwise it could accounted twice (out and stored)
  water_out = c->daily_tot_c_transp + c->daily_tot_c_evapo + c->daily_tot_c_int + c->soil_evaporation /*+ c->snow_subl*/ + c->out_flow;

  /* sum of current storage */
  water_stored = (c->asw - c->old_asw) + c->daily_tot_c_water_stored + c->prcp_snow;

  /* check balance */
  c->water_balance = water_in - water_out - water_stored;

  Log("water in = %f\n", water_in);
  Log("water out = %f\n", water_out);
  Log("water stored = %f\n", water_stored);
  Log("water balance = %f\n", c->water_balance);
  Log("old water balance = %f\n", c->old_water_balance);
  Log("differences in balance (old - current)= %f\n", c->old_water_balance - c->water_balance);


  if(c->years_count == 0 && c->doy == 1)
    {
      Log("NO CHECK WATER BALANCE FOR THE FIRST DAY\n");
      Log("water balance (water_in - water_out - water_stored) = %f\n", c->water_balance);
    }
  else
    {
      if (fabs(c->old_water_balance - c->water_balance) > 1e-4 )
	{
	  Log("...FATAL ERROR IN water balance\n");
	  Log("DOY = %d\n", c->doy);
	  ERROR(c->water_balance, "water balance");
	}
      else
	{
	  Log("...ok water balance\n");
	}
    }
  c->old_water_balance = c->water_balance;
}
