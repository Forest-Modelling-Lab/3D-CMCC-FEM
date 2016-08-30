/* heat_fluxes.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "heat_fluxes.h"
#include "constants.h"
#include "logger.h"


extern logger_t* g_log;

void Canopy_latent_heat_fluxes (species_t *const s, const meteo_daily_t *const meteo_daily)
{
	logger(g_log, "\n**CANOPY_LATENT_HEAT**\n");

	/* canopy canopy level latent heat fluxes (W/m2) */
	s->value[CANOPY_LATENT_HEAT] = s->value[CANOPY_EVAPO_TRANSP] * meteo_daily->lh_vap / 86400;
	logger(g_log, "CANOPY LATENT HEAT FLUX = %g W/m2\n", s->value[CANOPY_LATENT_HEAT]);

	s->value[MONTHLY_CANOPY_LATENT_HEAT] += s->value[CANOPY_LATENT_HEAT];
	s->value[YEARLY_CANOPY_LATENT_HEAT] += s->value[CANOPY_LATENT_HEAT];
}

void Canopy_sensible_heat_fluxes(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	logger(g_log, "\n**CANOPY SENSIBLE HEAT**\n");
}

void latent_heat_flux (cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	logger(g_log, "\n**CELL LATENT HEAT**\n");

	/*compute latent heat from canopy*/
	c->daily_c_evapo_watt = c->daily_c_evapo * meteo_daily->lh_vap / 86400.0;
	c->daily_c_transp_watt = c->daily_c_transp * meteo_daily->lh_vap / 86400.0;
	c->daily_c_evapotransp_watt = c->daily_c_evapo_watt + c->daily_c_transp_watt;
	c->daily_c_latent_heat_flux = c->daily_c_evapotransp_watt;
	logger(g_log, "Latent heat canopy evapotranspiration = %g W/m^2\n", c->daily_c_latent_heat_flux);
	logger(g_log, "Latent heat soil evaporation = %g W/m^2\n", c->daily_soil_evaporation_watt);

	/* adding soil latent heat flux to overall latent heat flux */
	c->daily_latent_heat_flux = c->daily_c_evapotransp_watt + c->daily_soil_evaporation_watt;

	/*in case of snow formation or dew fall*/
	if(meteo_daily->snow != 0.0)
	{
		logger(g_log, "implement negative heat fluxes!\n");
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		c->daily_latent_heat_flux += c->snow_subl * (meteo_daily->lh_sub * 1000.0) / 86400.0;
		logger(g_log, "Daily total latent heat flux with sublimation = %g W/m\n", c->daily_latent_heat_flux);
	}
	else
	{
		logger(g_log, "Daily total latent heat flux = %g W/m\n", c->daily_latent_heat_flux);
	}

	c->monthly_latent_heat_flux += c->daily_latent_heat_flux;
	c->annual_latent_heat_flux += c->daily_latent_heat_flux;

}

void sensible_heat_flux (cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	logger(g_log, "\n**CELL SENSIBLE HEAT**\n");

	c->daily_sensible_heat_flux = c->daily_c_sensible_heat_flux + c->daily_soil_sensible_heat_flux;
	logger(g_log, "Daily sensible heat flux = %g W/m\n", c->daily_sensible_heat_flux);

	c->monthly_sensible_heat_flux += c->daily_latent_heat_flux;
	c->annual_sensible_heat_flux += c->daily_latent_heat_flux;
}
