/* heat_fluxes.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "heat_fluxes.h"
#include "constants.h"
#include "logger.h"


extern logger_t* g_log;

void Canopy_latent_heat_fluxes (species_t *const s, const meteo_t *const met, const int month, const int day)
{
	/* canopy canopy level latent heat fluxes (W/m2) */
	s->value[CANOPY_LATENT_HEAT] = s->value[CANOPY_EVAPO_TRANSP] * met[month].d[day].lh_vap / 86400;
	logger(g_log, "CANOPY LATENT HEAT = %g W/m2\n", s->value[CANOPY_LATENT_HEAT]);
}

void Canopy_sensible_heat_fluxes (species_t *const s, const meteo_t *const met, const int month, const int day)
{
	/* following MAESPA assume leaf-canopy temperature (°C) equals to average daily temperature (°C) */
	s->value[CANOPY_TEMPERATURE] = s->value[LEAF_TEMPERATURE] = met[month].d[day].tavg;


}



void Latent_heat_flux (cell_t *const c, const meteo_t *const met, const int month, const int day)
{
	logger(g_log, "\nLATENT_HEAT_ROUTINE\n");

	/*compute latent heat from canopy*/
	c->daily_c_evapo_watt = c->daily_c_evapo * met[month].d[day].lh_vap / 86400.0;
	c->daily_c_transp_watt = c->daily_c_transp * met[month].d[day].lh_vap / 86400.0;
	c->daily_c_evapotransp_watt = c->daily_c_evapo_watt + c->daily_c_transp_watt;
	logger(g_log, "Latent heat canopy evapotranspiration = %g W/m^2\n", c->daily_c_evapotransp_watt);
	logger(g_log, "Latent heat soil evaporation = %g W/m^2\n", c->daily_soil_evaporation_watt);

	/* adding soil latent heat flux to overall latent heat flux */
	c->daily_latent_heat_flux = c->daily_c_evapotransp_watt + c->daily_soil_evaporation_watt;

	/*in case of snow formation or dew fall*/
	if(c->prcp_snow != 0.0)
	{
		logger(g_log, "implement negative heat fluxes!\n");
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		c->daily_latent_heat_flux += c->snow_subl * (met[month].d[day].lh_sub * 1000.0) / 86400.0;
		logger(g_log, "Daily total latent heat flux with sublimation = %g W/m\n", c->daily_latent_heat_flux);
	}
	else
	{
		logger(g_log, "Daily total latent heat flux = %g W/m\n", c->daily_latent_heat_flux);
	}

	c->monthly_latent_heat_flux += c->daily_latent_heat_flux;
	c->annual_latent_heat_flux += c->daily_latent_heat_flux;

	logger(g_log, "\nSENSIBLE_HEAT_ROUTINE\n");

	c->daily_sensible_heat_flux = c->daily_canopy_sensible_heat_flux + c->daily_soil_sensible_heat_flux;
	logger(g_log, "Daily sensible heat flux = %g W/m\n", c->daily_sensible_heat_flux);

	c->monthly_sensible_heat_flux += c->daily_latent_heat_flux;
	c->annual_sensible_heat_flux += c->daily_latent_heat_flux;

}
