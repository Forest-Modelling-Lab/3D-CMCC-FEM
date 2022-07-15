/* evapotranspiration.c */
#include "evapotranspiration.h"
#include "logger.h"

extern logger_t* g_debug_log;

void evapotranspiration(cell_t* const c)
{
	logger(g_debug_log, "\n**EVAPOTRANSPIRATION_ROUTINE**\n");

	//FIXME BUG WHEN NO TREES
	c->daily_et = c->daily_canopy_et + c->daily_soil_evapo;
	logger(g_debug_log, "Daily EVAPOTRANSPIRATION = %f \n", c->daily_et);
	c->monthly_et += c->daily_et;
	logger(g_debug_log, "Monthly EVAPOTRANSPIRATION = %f \n", c->monthly_et);
	c->annual_et += c->daily_et;
	logger(g_debug_log, "Annual EVAPOTRANSPIRATION = %f \n", c->annual_et);
}



