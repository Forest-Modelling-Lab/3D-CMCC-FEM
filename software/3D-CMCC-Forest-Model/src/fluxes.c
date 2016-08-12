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

extern logger_t* g_log;

void carbon_fluxes (species_t *const s)
{
	/* compute carbon balance between photosynthesis and autotrophic respiration */

	logger(g_log, "\nC-FLUXES\n");

	s->value[C_FLUX] = s->value[DAILY_GPP_gC] - fabs(s->value[TOTAL_AUT_RESP]);
	logger(g_log, "c-flux = %f gC m^2 day^-1\n", s->value[C_FLUX]);
//	logger(g_log, "c-flux = %f tDM ha^-1 day ^-1\n", ((s->value[C_FLUX] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell));

}

//too remove after made water_balance function
void Water_fluxes(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	logger(g_log, "\nW-FLUXES\n");
	//todo make it better
	c->daily_tot_w_flux = c->water_to_soil + meteo_daily->snow - c->water_to_atmosphere - c->out_flow;
	c->monthly_tot_w_flux += c->daily_tot_w_flux;
	c->annual_tot_w_flux += c->daily_tot_w_flux;

	logger(g_log, "Daily_w_flux = %f \n", c->daily_tot_w_flux);
}

void get_net_ecosystem_exchange(cell_t *const c)
{
//	int i;
//	for(i = 0; i< c->soils_count; i++)
//	{
//		c->daily_het_resp += c->soils[i].co2 * 1000.0;
//		c->monthly_het_resp += c->soils[i].co2 * 1000.0;
//		c->annual_het_resp += c->soils[i].co2 * 1000.0;
//	}
//	c->daily_r_eco = c->daily_aut_resp + c->daily_het_resp;
//	c->daily_nee = c->daily_gpp - c->daily_r_eco;
//
//	c->monthly_r_eco +=  c->daily_r_eco;
//	c->monthly_nee +=  c->daily_nee;
//
//	c->annual_r_eco +=  c->daily_r_eco;
//	c->annual_nee +=  c->daily_nee;
}

