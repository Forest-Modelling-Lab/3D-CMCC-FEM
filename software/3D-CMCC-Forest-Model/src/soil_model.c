/*
 * soilmodel.c
 *
 *  Created on: 16/nov/2012
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "constants.h"
#include "logger.h"
#include "snow.h"
#include "soil_evaporation.h"
#include "soil_respiration.h"
#include "soil_water_balance.h"

extern logger_t* g_log;

int Soil_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	int soil_layer;
	//fixme move soil albedo into soil.txt file
	const double soil_albedo = 0.15;                                                      /* (ratio) soil albedo without snow (see MAESPA model) */
	double Light_refl_sw_rad_soil_frac;                                                   /* (ratio) fraction of Short Wave radiation reflected from the soil */

	/* shortcuts */
	cell_t *c;
	//soil_layer_s *s;
	meteo_daily_t *meteo_daily;

	/* assign shortcuts */
	c = &m->cells[cell];
	meteo_daily = &m->cells[cell].years[year].m[month].d[day];

	/* check parameters */
	assert(m);

	logger (g_log, "**\n*******SOIL_MODEL_DAILY*********\n");

	/* soil radiation */
	logger (g_log, "**SOIL RADIATION**\n");

	/* fraction of light reflected by the soil */
	Light_refl_sw_rad_soil_frac = soil_albedo;
	logger(g_log, "LightReflec_soil = %g %%\n", Light_refl_sw_rad_soil_frac * 100);
	logger(g_log, "******************************************************\n");

	/* compute values for soil layer when last height class in cell is processed */
	/* remove reflected part */
	logger(g_log,"*incoming light for soil*\n");
	logger(g_log,"incoming PAR for soil = %g molPAR/m^2/day\n", meteo_daily->par);
	logger(g_log,"incoming Short Wave radiation = %g W/m2\n", meteo_daily->sw_downward_W);
	logger(g_log,"incoming PPFD for soil = %g umol/m2/sec\n", meteo_daily->ppfd);
	c->par_refl_soil = meteo_daily->par * Light_refl_sw_rad_soil_frac;
	c->sw_rad_for_soil_refl = meteo_daily->sw_downward_W * Light_refl_sw_rad_soil_frac;
	c->ppfd_refl_soil = meteo_daily->ppfd * Light_refl_sw_rad_soil_frac;
	logger(g_log,"reflected light from the soil\n");
	logger(g_log,"par_refl_soil = %g molPAR/m^2/day\n", c->par_refl_soil);
	logger(g_log,"sw_rad_for_soil_refl = %g W/m2\n", c->sw_rad_for_soil_refl);
	logger(g_log,"ppfd_refl_soil = %g umol/m2/sec\n", c->ppfd_refl_soil);

	/* Par Short Wave radiation and PPFD for the soil */
	logger(g_log, "*incoming light for soil less reflected part*\n");
	logger(g_log, "PAR for soil = %g molPAR/m^2/day\n", meteo_daily->par);
	logger(g_log, "Short Wave radiation for soil = %g W/m2\n", meteo_daily->sw_downward_W);
	logger(g_log, "PPFD for soil = %g umol/m2/sec\n", meteo_daily->ppfd);

	/* compute absorbed part from soil */
	c->apar_soil = meteo_daily->par - c->par_refl_soil;
	c->sw_rad_abs_soil = meteo_daily->sw_downward_W - c->sw_rad_for_soil_refl;
	c->ppfd_abs_soil = meteo_daily->ppfd - c->ppfd_refl_soil;

	logger (g_log, "number of soil layers = %d\n", c->s_layers_count);

	//ALESSIOC-ALESSIOR still bugs in imported value for soil layer number
	/* loop on each cell layers starting from highest to lower */
	for ( soil_layer = c->s_layers_count -1 ; soil_layer >= 0; -- soil_layer )
	{
		logger (g_log, "soil_layer = %d\n", soil_layer);

		//ALESSIOC-ALESSIOR fixme to avoid errors due to worrong value of s_layer_count I putted in first layer
		if ( soil_layer == c->s_layers_count -1)
		{
			/* compute snow melt, snow sublimation */
			snow_melt_subl ( c, meteo_daily );

			/* compute soil evaporation */
			soil_evaporation ( c, meteo_daily );

			/* compute soil respiration */
			soil_respiration ( c );

			/* compute soil water balance */
			soil_water_balance ( c, meteo_daily );
		}
	}

	/* ok */
	return 1;


}
