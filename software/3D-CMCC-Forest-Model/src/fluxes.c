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
#include "types.h"
#include "constants.h"


void Carbon_fluxes (SPECIES *const s, CELL *const c, int height, int day, int month)
{
	int i;

	i = c->heights[height].z;
	//compute carbon balance between photosynthesis and autotrophic respiration
	//recompute GPP
	Log("\nC-FLUXES\n");

	s->value[C_FLUX] = s->value[DAILY_GPP_gC] - fabs(s->value[TOTAL_AUT_RESP]);
	Log("c-flux = %f gC m^2 day^-1\n", s->value[C_FLUX]);
//	Log("c-flux = %f tDM ha^-1 day ^-1\n", ((s->value[C_FLUX] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell));

	c->layer_daily_c_flux[i] = s->value[C_FLUX];
	c->daily_C_flux += s->value[C_FLUX];
	c->layer_monthly_c_flux[i] += s->value[C_FLUX];
	c->monthly_C_flux +=  s->value[C_FLUX];
	c->layer_annual_c_flux[i] += s->value[C_FLUX];
	c->annual_C_flux +=  s->value[C_FLUX];
}

//too remove after made water_balance function
void Water_fluxes (CELL *const c)
{
	Log("\nGET_W-FLUXES\n");
	//todo make it better
	c->daily_tot_w_flux = c->water_to_soil + c->prcp_snow - c->water_to_atmosphere - c->out_flow;
	c->monthly_tot_w_flux += c->daily_tot_w_flux;
	c->annual_tot_w_flux += c->daily_tot_w_flux;

	Log("Daily_w_flux = %f \n", c->daily_tot_w_flux);
}

void get_net_ecosystem_exchange(CELL *const c)
{
	int i;
	for(i = 0; i< c->soils_count; i++)
	{
		c->daily_het_resp += c->soils[i].co2 * 1000.0;
		c->monthly_het_resp += c->soils[i].co2 * 1000.0;
		c->annual_het_resp += c->soils[i].co2 * 1000.0;
	}
	c->daily_r_eco = c->daily_aut_resp + c->daily_het_resp;
	c->daily_nee = c->daily_gpp - c->daily_r_eco;

	c->monthly_r_eco +=  c->daily_r_eco;
	c->monthly_nee +=  c->daily_nee;

	c->annual_r_eco +=  c->daily_r_eco;
	c->annual_nee +=  c->daily_nee;
}

