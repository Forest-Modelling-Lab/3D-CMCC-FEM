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
#include "soil_water_balance.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

//fixme  maybe it can be moved to soil_model.c
void soil_water_balance(cell_t *c, const meteo_daily_t *const meteo_daily)
{
	logger(g_debug_log, "\n**SOIL-SNOW WATER BALANCE**\n");

	/* water pools */
	c->old_daily_c_water_stored = c->daily_c_water_stored;
	c->old_snow_pack = c->snow_pack;
	c->old_asw = c->asw;

	/*update balance*/

	/* soil water balance */
	c->asw = c->old_asw + (meteo_daily->rain + c->snow_melt) - (c->daily_c_transp + c->daily_soil_evapo + c->daily_c_rain_int);

	/* snow pack balance */
	c->snow_pack = c->old_snow_pack + meteo_daily->snow - (c->snow_melt + c->snow_subl);

	logger(g_debug_log, "asw = %g mm\n", c->asw);
	logger(g_debug_log, "snow pack = %g mm\n", c->snow_pack);

	if ( c->asw > c->max_asw_fc)
	{
		logger(g_debug_log," asw %g > max_asw_fc %g\n", c->asw, c->max_asw_fc);
		c->out_flow = c->asw - c->max_asw_fc;
		logger(g_debug_log, "out_flow = %g\n", c->out_flow);
		logger(g_debug_log, "ATTENTION asw exceeds max_asw_fc!! \n");
		c->asw = c->max_asw_fc;
		logger(g_debug_log, "Available soil water = %g\n", c->asw);
	}
	/* otherwise, no outflow */
	else
	{
		c->out_flow = 0.0;
	}
	/* calculates the outflow flux from the difference between soilwater
	and maximum soilwater */
	/* water in excess of saturation to outflow */
	//todo test it
	//	if (c->asw > c->max_asw_sat)
	//	{
	//		logger(g_debug_log," asw %g > max_asw_sat %g\n", c->asw, c->max_asw_sat);
	//		c->out_flow = c->asw - c->max_asw_sat;
	//		logger(g_debug_log, "out_flow = %g\n", c->out_flow);
	//		logger(g_debug_log, "ATTENTION asw exceeds max_asw_sat!! \n");
	//		c->asw -= c->out_flow;
	//		logger(g_debug_log, "c->out_flow = %g\n", c->out_flow);
	//	}
	//	/* slow drainage from saturation to field capacity */
	//	else if (c->asw > c->max_asw_fc)
	//	{
	//		logger(g_debug_log," asw %g > max_asw_fc %g\n", c->asw, c->max_asw_fc);
	//		c->out_flow = c->asw - c->max_asw_fc;
	//		logger(g_debug_log, "out_flow = %g\n", c->out_flow);
	//		logger(g_debug_log, "ATTENTION asw exceeds max_asw_fc!! \n");
	//		c->asw = c->max_asw_fc;
	//		logger(g_debug_log, "Available soil water = %g\n", c->asw);
	//	}
	//	/* otherwise, no outflow */
	//	else
	//	{
	//		c->out_flow = 0.0;
	//	}

	/* from BIOME-BGC */
	/* the following special case prevents evaporation under very
	dry conditions from causing a negative soilwater content */
	/* negative soilwater */
	if (c->asw < 0.0)
	{
		logger(g_debug_log,"WARNING negative values for soil water!!!!!!!!!\n");
		/* add back the evaporation and transpiration fluxes, and
		set these fluxes to 0.0 */
		c->asw             += c->daily_soil_evapo;
		c->asw             += c->daily_c_transp;
		c->daily_soil_evapo = 0.0;
		c->daily_c_transp   = 0.0;
	}

	CHECK_CONDITION(c->asw, <, 0.0);
	CHECK_CONDITION(c->snow_pack, <, 0.0);

	//fixme
	//	c->swc = (c->asw * 100)/c->max_asw_fc;
	//	logger(g_debug_log, "asw = %g\n", c->asw);
	//	logger(g_debug_log, "max_asw_fc = %g\n", c->max_asw_fc);
	//	logger(g_debug_log, "SWC = %g(%%vol)\n", c->swc);
	//
	//	/* check */
	//	CHECK_CONDITION (c->swc, >, 100.1);
	//	CHECK_CONDITION (c->swc, <, 0);

}
