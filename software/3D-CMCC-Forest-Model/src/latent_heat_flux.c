/*
 * canopy_evaporation.c
 *
 *  Created on: 07/nov/2013
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Latent_heat_flux (CELL *c)
{
	Log("\nLATENT_HEAT_ROUTINE\n");

	c->daily_latent_heat_flux = c->daily_c_evapotransp_watt + c->daily_soil_evaporation_watt;

	/*in case of snow formation*/
	if(c->prcp_snow != 0.0 && c->snow_subl != 0.0)
	{
		Log("implement negative heat fluxes!\n");
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		c->daily_latent_heat_flux += c->snow_subl * (c->lh_sub * 1000.0) / 86400.0;
		Log("Daily total latent heat flux with sublimation = %f W/m\n", c->daily_latent_heat_flux);
	}
	else
	{
		Log("Daily total latent heat flux = %f W/m\n", c->daily_latent_heat_flux);
	}

	c->monthly_latent_heat_flux += c->daily_latent_heat_flux;
	c->annual_latent_heat_flux += c->daily_latent_heat_flux;
}
