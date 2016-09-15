/*
 * soil_radiation_sw_band.c
 *
 *  Created on: 10/set/2016
 *      Author: alessio-cmcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "math.h"
#include "common.h"
#include "soil_radiation_sw_band.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void soil_radiation_sw_band ( cell_t *const c, meteo_daily_t *meteo_daily )
{
	//fixme move soil and snow albedo into soil.txt file
	const double soil_albedo = 0.15;                                                      /* (ratio) soil albedo without snow (see MAESPA model) */
	const double snow_albedo = 0.65;                                                      /* (ratio) snow albedo as an average between freshly fallen snow and 0.4 for melting snow */
	double Light_refl_sw_rad_soil_frac = 0.;                                                   /* (ratio) fraction of Short Wave radiation reflected from the soil */
	double Light_refl_sw_rad_snow_frac = 0.;                                                   /* (ratio) fraction of Short Wave radiation reflected from the snow */

	/* check parameters */
	assert ( c );

	/* radiation for soil */
	logger (g_log, "**SOIL RADIATION**\n");

	/* fraction of light reflected */

	if ( !c->snow_pack )
	{
		/* fraction of light reflected by the soil */
		Light_refl_sw_rad_soil_frac = soil_albedo;
	}
	else
	{
		/* fraction of light reflected by the snow */
		Light_refl_sw_rad_snow_frac = snow_albedo;
	}
	logger(g_log, "LightReflec_soil = %g %%\n", Light_refl_sw_rad_soil_frac * 100);
	logger(g_log, "LightReflec_snow = %g %%\n", Light_refl_sw_rad_snow_frac * 100);
	logger(g_log, "******************************************************\n");

	/* compute values for soil layer when last height class in cell is processed */
	/* remove reflected part */
	logger(g_log,"*incoming light for soil*\n");
	logger(g_log,"incoming PAR for soil = %g molPAR/m^2/day\n", meteo_daily->par);
	logger(g_log,"incoming Short Wave radiation = %g W/m2\n", meteo_daily->sw_downward_W);
	logger(g_log,"incoming PPFD for soil = %g umol/m2/sec\n", meteo_daily->ppfd);

	if ( ! c->snow_pack )
	{
		c->par_refl_soil = meteo_daily->par * Light_refl_sw_rad_soil_frac;
		c->sw_rad_for_soil_refl = meteo_daily->sw_downward_W * Light_refl_sw_rad_soil_frac;
		c->ppfd_refl_soil = meteo_daily->ppfd * Light_refl_sw_rad_soil_frac;
		logger(g_log,"reflected light from the soil\n");
		logger(g_log,"par_refl_soil = %g molPAR/m^2/day\n", c->par_refl_soil);
		logger(g_log,"sw_rad_for_soil_refl = %g W/m2\n", c->sw_rad_for_soil_refl);
		logger(g_log,"ppfd_refl_soil = %g umol/m2/sec\n", c->ppfd_refl_soil);
	}
	else
	{
		c->par_refl_snow = meteo_daily->par * Light_refl_sw_rad_snow_frac;
		c->sw_rad_for_snow_refl = meteo_daily->sw_downward_W * Light_refl_sw_rad_snow_frac;
		c->ppfd_refl_snow = meteo_daily->ppfd * Light_refl_sw_rad_snow_frac;
		logger(g_log,"reflected light from the snow\n");
		logger(g_log,"par_refl_snow = %g molPAR/m^2/day\n", c->par_refl_snow);
		logger(g_log,"sw_rad_for_snow_refl = %g W/m2\n", c->sw_rad_for_snow_refl);
		logger(g_log,"ppfd_refl_snow = %g umol/m2/sec\n", c->ppfd_refl_snow);
	}

	/* Par Short Wave radiation and PPFD for the soil */
	logger(g_log, "*incoming light for soil less reflected part*\n");
	logger(g_log, "PAR for soil = %g molPAR/m^2/day\n", meteo_daily->par);
	logger(g_log, "Short Wave radiation for soil = %g W/m2\n", meteo_daily->sw_downward_W);
	logger(g_log, "PPFD for soil = %g umol/m2/sec\n", meteo_daily->ppfd);

	if ( ! c->snow_pack )
	{
		/* compute absorbed part from soil */
		c->apar_soil = meteo_daily->par - c->par_refl_soil;
		c->sw_rad_abs_soil = meteo_daily->sw_downward_W - c->sw_rad_for_soil_refl;
		c->ppfd_abs_soil = meteo_daily->ppfd - c->ppfd_refl_soil;
	}
	else
	{
		/* compute absorbed part from snow */
		c->apar_snow = meteo_daily->par - c->par_refl_snow;
		c->sw_rad_abs_snow = meteo_daily->sw_downward_W - c->sw_rad_for_snow_refl;
		c->ppfd_abs_snow = meteo_daily->ppfd - c->ppfd_refl_snow;
	}
}
