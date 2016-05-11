/*
 * heat_fluxes.c
 *
 *  Created on: 01/01/2016
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Latent_heat_flux (CELL *c, const MET_DATA *met, int month, int day)
{
	Log("\nLATENT_HEAT_ROUTINE\n");

	/*compute energy balance transpiration from canopy*/
	c->daily_c_evapo_watt = c->daily_c_evapo * met[month].d[day].lh_vap / 86400.0;
	c->daily_c_transp_watt = c->daily_c_transp * met[month].d[day].lh_vap / 86400.0;
	c->daily_c_evapotransp_watt = c->daily_c_evapo_watt + c->daily_c_transp_watt;
	Log("Latent heat canopy evapotranspiration = %f W/m^2\n", c->daily_c_evapotransp_watt);

	c->daily_latent_heat_flux = c->daily_c_evapotransp_watt + c->daily_soil_evaporation_watt;

	/*in case of snow formation*/
	if(c->prcp_snow != 0.0 && c->snow_subl != 0.0)
	{
		Log("implement negative heat fluxes!\n");
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		c->daily_latent_heat_flux += c->snow_subl * (met[month].d[day].lh_sub * 1000.0) / 86400.0;
		Log("Daily total latent heat flux with sublimation = %f W/m\n", c->daily_latent_heat_flux);
	}
	else
	{
		Log("Daily total latent heat flux = %f W/m\n", c->daily_latent_heat_flux);
	}

	c->monthly_latent_heat_flux += c->daily_latent_heat_flux;
	c->annual_latent_heat_flux += c->daily_latent_heat_flux;

	Log("\nSENSIBLE_HEAT_ROUTINE\n");

	c->daily_sensible_heat_flux = c->daily_canopy_sensible_heat_flux + c->daily_soil_sensible_heat_flux;
	Log("Daily sensible heat flux = %f W/m\n", c->daily_sensible_heat_flux);

	c->monthly_sensible_heat_flux += c->daily_latent_heat_flux;
	c->annual_sensible_heat_flux += c->daily_latent_heat_flux;

}
