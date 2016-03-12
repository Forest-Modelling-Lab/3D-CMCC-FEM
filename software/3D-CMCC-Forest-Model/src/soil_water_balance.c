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
	Log("\nGET SOIL WATER BALACE\n");
	c->old_available_soil_water = c->asw;

	/*update balance*/
	if(met[month].d[day].tavg>0.0)
	{
		c->asw = (c->asw + c->daily_prcp + c->snow_melt)
				- (c->daily_tot_c_transp + c->daily_tot_c_int + c->soil_evaporation + c->runoff);
	}
	else
	{
		c->asw = c->asw - c->soil_evaporation;
	}
	Log("ASW = %f mm\n", c->asw);
	Log("snow pack = %f mm\n", c->snow_pack);

	/*check*/
	if (c->asw < (c->max_asw * site->min_frac_maxasw))
	{
		//TEST REMOVED ANY REFILL OF WATER SOIL
		Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
		//c->asw = c->max_asw * site->min_frac_maxasw;
		Log("ASW = %f\n", c->asw);
	}
	if ( c->asw > c->max_asw)
	{
		c->runoff = c->asw -c->max_asw;
		Log("Runoff = %f\n", c->runoff);
		Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
		c->asw = c->max_asw;
		Log("Available soil water = %f\n", c->asw);
	}
	c->swc= (c->asw * 100)/c->max_asw;
	Log("SWC = %g(%vol)\n", c->swc);
}
