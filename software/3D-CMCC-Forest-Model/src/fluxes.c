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

void carbon_fluxes (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* compute carbon balance between photosynthesis and autotrophic respiration */

	logger(g_debug_log, "\n**C-FLUXES**\n");

	s->value[C_FLUX] = s->value[GPP] - s->value[TOTAL_AUT_RESP];
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

void ecosystem_respiration (cell_t *const c)
{
	c->daily_r_eco    = c->daily_aut_resp + c->daily_soil_resp;
	logger(g_debug_log, "daily_r_eco = %f gC/m2/day\n", c->daily_r_eco);

	c->monthly_r_eco += c->daily_r_eco;
	logger(g_debug_log, "monthly_r_eco = %f gC/m2/month\n", c->monthly_r_eco);

	c->annual_r_eco  += c->daily_r_eco;
	logger(g_debug_log, "annual_r_eco = %f gC/m2/year\n", c->annual_r_eco);

}

void net_ecosystem_productivity(cell_t *const c)
{
	c->daily_nep     = c->daily_npp - c->daily_soil_resp;
	logger(g_debug_log, "daily_nep = %f gC/m2/day\n", c->daily_nep);

	c->monthly_nep  += c->daily_nep;
	logger(g_debug_log, "monthly_nep = %f gC/m2/monthly\n", c->monthly_nep);

	c->annual_nep   += c->daily_nep;
	logger(g_debug_log, "annual_nep = %f gC/m2/annual\n", c->annual_nep);
}

void net_ecosystem_exchange(cell_t *const c)
{
	c->daily_nee     = c->daily_gpp - c->daily_r_eco;
	logger(g_debug_log, "daily_nee = %f gC/m2/day\n", c->daily_nee);

	c->monthly_nee  += c->daily_nee;
	logger(g_debug_log, "monthly_nee = %f gC/m2/monthly\n", c->monthly_nee);

	c->annual_nee   += c->daily_nee;
	logger(g_debug_log, "annual_nee = %f gC/m2/annual\n", c->annual_nee);
}

