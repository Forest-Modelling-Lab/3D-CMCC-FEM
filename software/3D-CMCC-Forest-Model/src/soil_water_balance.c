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

void soil_water_balance(cell_t *c, const meteo_daily_t *const meteo_daily)
{
	logger(g_debug_log, "\n**SOIL-SNOW WATER BALANCE**\n");

	/* update balance */

	/* soil water balance */
	c->asw += ( ( meteo_daily->rain + c->daily_snow_melt ) - ( c->daily_canopy_transp + c->daily_soil_evapo ) );
	logger(g_debug_log, "asw = %g\n", c->asw);
	logger(g_debug_log, "rain = %g\n", meteo_daily->rain);
	logger(g_debug_log, "daily_snow_melt = %g\n", c->daily_snow_melt);
	logger(g_debug_log, "daily_canopy_transp = %g\n", c->daily_canopy_transp);
	logger(g_debug_log, "daily_soil_evapo = %g\n", c->daily_soil_evapo);



	/* snow pack balance */
	c->snow_pack += meteo_daily->snow - (c->daily_snow_melt + c->daily_snow_subl);
	logger(g_debug_log, "snow_pack = %g\n", c->snow_pack);

	if ( c->asw > c->max_asw_fc)
	{
		c->daily_out_flow = c->asw - c->max_asw_fc;
		c->asw            = c->max_asw_fc;

		logger(g_debug_log, "daily_out_flow = %g\n", c->daily_out_flow);
		logger(g_debug_log, "asw = %g\n", c->asw);
		logger(g_debug_log, "max_asw_fc = %g\n", c->max_asw_fc);

		c->monthly_out_flow += c->daily_out_flow;
		c->annual_out_flow  += c->daily_out_flow;
	}

	/* from BIOME-BGC */
	/* the following special case prevents evaporation under very
	dry conditions from causing a negative soilwater content */
	/* negative soilwater */
	if ( c->asw < ZERO )
	{
		logger(g_debug_log,"#WARNING negative values for soil water!\n");
		/* add back the evaporation and transpiration fluxes, and
		set these fluxes to 0.0 */
		c->asw                  += c->daily_soil_evapo;
		c->asw                  += c->daily_canopy_transp;
		c->daily_soil_evapo      = 0.;
		c->daily_canopy_transp   = 0.;
	}

	/* soil moist ratio (%vol.) */
	c->soil_moist_ratio = c->asw / c->max_asw_fc;
	logger(g_debug_log, "Soil moisture = %g %\n", c->soil_moist_ratio );

	CHECK_CONDITION(c->asw,       <, ZERO);
	CHECK_CONDITION(c->snow_pack, <, ZERO);

}
