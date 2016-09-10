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
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_radiation_sw_band.h"
#include "snow.h"
#include "soil_evaporation.h"
#include "soil_respiration.h"
#include "soil_water_balance.h"
#include "settings.h"

extern logger_t* g_log;
extern settings_t* g_settings;

int Soil_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	int soil_layer;

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
	logger (g_log, "number of soil layers = %d\n", c->soil_layers_count);

	CHECK_CONDITION( c->soil_layers_count, != g_settings->number_of_soil_layer );

	/* loop on each cell layers starting from highest to lower */
	for ( soil_layer = c->soil_layers_count -1 ; soil_layer >= 0; -- soil_layer )
	{
		logger (g_log, "soil_layer = %d\n", soil_layer);

		/* run on only for the highest soil layer */
		if ( soil_layer == c->soil_layers_count -1)
		{
			/* soil radiative balance */
			soil_radiation_sw_band ( c, meteo_daily );

			if ( c->snow_pack )
			{
				/* compute snow melt, snow sublimation */
				snow_melt_subl ( c, meteo_daily );
			}
			else
			{
				/* compute soil evaporation */
				soil_evaporation ( c, meteo_daily );
			}

		}

		/* compute soil respiration (not yet implemented) */
		soil_respiration ( c );

		/* compute soil water balance */
		soil_water_balance ( c, meteo_daily );
	}

	/* ok */
	return 1;
}
