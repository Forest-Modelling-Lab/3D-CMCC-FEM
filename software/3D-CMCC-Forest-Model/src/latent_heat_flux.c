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

void Latent_heat_flux (CELL *c, const MET_DATA *met, int month, int day)
{
	double tairK;
	double tsoilK;

	tairK = met[month].d[day].tavg + TempAbs;
	tsoilK = met[month].d[day].tsoil + TempAbs;

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

	/*following Maespa model of Duursma et al., */
	if(c->daily_bl_cond!= 0)
	{
		c->daily_sensible_heat_flux = (CP * met[month].d[day].rho_air * c->daily_bl_cond * (tairK - tsoilK)) / 86400.0;
	}
	else
	{
		c->daily_sensible_heat_flux = (CP * met[month].d[day].rho_air * (tairK - tsoilK)) / 86400.0;
	}
	Log("Daily sensible heat flux = %f W/m\n", c->daily_sensible_heat_flux);

	c->monthly_sensible_heat_flux += c->daily_latent_heat_flux;
	c->annual_sensible_heat_flux += c->daily_latent_heat_flux;

}
