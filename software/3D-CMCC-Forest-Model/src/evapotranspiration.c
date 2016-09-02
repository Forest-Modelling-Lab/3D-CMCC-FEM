/* evapotranspiration.c */
#include "evapotranspiration.h"
#include "logger.h"

extern logger_t* g_log;

void evapotranspiration(cell_t* const c)
{
	logger(g_log, "\n**EVAPOTRANSPIRATION_ROUTINE**\n");

	//ALESSIOC TO ALESSIOR BUG WHEN NO TREES
	c->daily_et = c->daily_c_evapotransp + c->daily_soil_evapo;
	logger(g_log, "Daily EVAPOTRANSPIRATION = %f \n", c->daily_et);
	c->monthly_et += c->daily_et;
	logger(g_log, "Monthly EVAPOTRANSPIRATION = %f \n", c->monthly_et);
	c->annual_et += c->daily_et;
	logger(g_log, "Annual EVAPOTRANSPIRATION = %f \n", c->annual_et);

	/*compute water to atmosphere*/
	c->vapour_to_atmosphere = c->daily_et;
	logger(g_log, "vapour to atmosphere = %f \n", c->vapour_to_atmosphere);

}



