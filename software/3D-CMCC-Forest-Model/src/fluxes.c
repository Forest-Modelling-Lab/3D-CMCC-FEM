/*
 * C-fluxes.c
 *
 *  Created on: 14/ott/2013
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fluxes.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

void carbon_fluxes (species_t *const s)
{
	/* compute carbon balance between photosynthesis and autotrophic respiration */

	logger(g_debug_log, "\n**C-FLUXES**\n");

	s->value[C_FLUX] = s->value[GPP_gC] - fabs(s->value[TOTAL_AUT_RESP]);
	logger(g_debug_log, "c-flux = %f gC/m2/day\n", s->value[C_FLUX]);
}

//too remove after made water_balance function
void water_fluxes(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
//	logger(g_debug_log, "\nW-FLUXES\n");
//	//todo make it better
//	c->daily_tot_w_flux = c->water_to_soil + meteo_daily->snow - c->water_to_atmosphere - c->out_flow;
//	c->monthly_tot_w_flux += c->daily_tot_w_flux;
//	c->annual_tot_w_flux += c->daily_tot_w_flux;
//	logger(g_debug_log, "Daily_w_flux = %f \n", c->daily_tot_w_flux);
}

void net_ecosystem_exchange(cell_t *const c)
{

}

