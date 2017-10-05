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
#include "soil_nitrogen_balance.h"
#include "settings.h"
#include "check_balance.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;

int Soil_model(matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	int soil_layer;

	/* shortcuts */
	cell_t *c;
	//soil_layer_s *s;
	meteo_daily_t *meteo_daily;

	/* check parameters */
	assert(m);

	/* assign shortcuts */
	c = &m->cells[cell];

	if ( DAILY == g_settings->time )
	{
		meteo_daily  = &METEO_DAILY(m->cells[cell].years[year].m)[month].d[day];
		//meteo_daily = &m->cells[cell].years[year].m[month].d[day];
	
		logger (g_debug_log, "**\n*******SOIL_MODEL_DAILY*********\n");
		logger (g_debug_log, "number of soil layers = %d\n", c->soil_layers_count);

		CHECK_CONDITION( c->soil_layers_count, !=, g_settings->number_of_soil_layer );

		/* loop on each cell layers starting from highest to lower */
		for ( soil_layer = c->soil_layers_count -1 ; soil_layer >= 0; -- soil_layer )
		{
			logger (g_debug_log, "soil_layer = %d\n", soil_layer);

			/* run on only for the highest soil layer */
			if ( soil_layer == c->soil_layers_count -1)
			{
				/* add Nitrogen to top soil pool from atmospheric deposition */
				c->daily_soilN += meteo_daily->Ndeposition;

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

			/* compute soil water balance */
			soil_water_balance ( c, meteo_daily );

			/* compute soil nitrogen balance */
			//soil_nitrogen_balance();

			/* compute soil respiration (not yet implemented) */
			soil_respiration ( c );
		}

		/*******************************************************************************************************/

		/* CHECK FOR BALANCE CLOSURE */

		/* CHECK FOR RADIATIVE FLUX BALANCE CLOSURE */
		/* 1 */ if ( ! check_soil_radiation_flux_balance ( c, meteo_daily ) ) return 0;

		/* CHECK FOR CARBON FLUX BALANCE CLOSURE */
		/* 2 */ //fixme if ( ! check_soil_carbon_flux_balance    ( c ) ) return 0;

		/* CHECK FOR CARBON MASS BALANCE CLOSURE */
		/* 3 */ //fixme if ( ! check_soil_carbon_mass_balance    ( c ) ) return 0;

		/* CHECK FOR WATER FLUX BALANCE CLOSURE */
		/* 4 */ if ( ! check_soil_water_flux_balance     ( c, meteo_daily ) ) return 0;

		/*******************************************************************************************************/
	}
	else if ( HOURLY == g_settings->time )
	{
		// TODO;
	}
	else if ( HALFHOURLY == g_settings->time )
	{
		// TODO
	}

	/* ok */
	return 1;
}
