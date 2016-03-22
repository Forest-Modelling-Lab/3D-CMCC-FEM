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
void Soil_water_balance (CELL *c, const MET_DATA *const met, int month, int day)
{
	Log("\n**SOIL WATER BALACE**\n");
	c->old_asw = c->asw;

	Log("in\n");
	Log("c->prcp_rain = %f\n", c->prcp_rain);
	Log("c->prcp_snow = %f\n", c->prcp_snow);
	Log("out\n");
	Log("c->daily_c_transp = %f\n", c->daily_c_transp);
	Log("c->daily_c_int = %f\n", c->daily_c_int);
	Log("c->daily_c_evapo = %f\n", c->daily_c_evapo);
	Log("c->soil_evaporation = %f\n", c->daily_soil_evapo);
	Log("c->snow_subl = %f\n", c->snow_subl);
	Log("c->snow_melt = %f\n", c->snow_melt);
	Log("c->out_flow = %f\n", c->out_flow);
	Log("stored\n");
	Log("delta c->asw = %f\n", (c->asw - c->old_asw));
	Log("c->snow_pack = %f\n", c->snow_pack);
	Log("c->daily_c_water_stored = %f\n", c->daily_c_water_stored);
	Log("c->asw = %f\n", c->asw);
	Log("c->old_asw = %f\n", c->old_asw);

	/*update balance*/
	if(met[month].d[day].tavg>0.0)
	{
		c->asw = (c->asw + c->prcp_rain + c->snow_melt) -
				(c->daily_c_transp + c->daily_c_int + c->daily_soil_evapo + c->daily_c_evapo + c->out_flow);
	}
	else
	{
		c->asw = c->asw - (c->daily_c_transp + c->daily_c_int + c->daily_soil_evapo + c->daily_c_evapo + c->out_flow);
	}
	Log("ASW = %f mm\n", c->asw);
	Log("snow pack = %f mm\n", c->snow_pack);

	/*check*/
	if (c->asw < 0.0 /*(c->max_asw * site->min_frac_maxasw)*/)
	{
		//TEST TO REMOVE ANY REFILL OF WATER SOIL
		Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
		exit(1);
		//CHECK_CONDITION(c->asw, < 0.0)
		c->asw = c->max_asw * site->min_frac_maxasw;
		Log("ASW = %f\n", c->asw);
	}
	if ( c->asw > c->max_asw)
	{
		c->out_flow = c->asw -c->max_asw;
		Log("out_flow = %f\n", c->out_flow);
		Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
		c->asw = c->max_asw;
		Log("Available soil water = %f\n", c->asw);
	}
	c->swc= (c->asw * 100)/c->max_asw;
	Log("SWC = %g(%vol)\n", c->swc);
}
