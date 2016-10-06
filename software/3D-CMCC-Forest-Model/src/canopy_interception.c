/*
 * canopy_interception.c
 *
 *  Created on: 19/ago/2016
 *      Author: alessio-cmcc
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "canopy_evapotranspiration.h"
#include "constants.h"
#include "logger.h"
#include "Penman_Monteith.h"
#include "heat_fluxes.h"

extern logger_t* g_debug_log;

void canopy_interception(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	static int cell_height_class_counter;
	static int layer_height_class_counter;

	static double temp_int_rain;
	static double temp_int_snow;

	double leaf_cell_cover_eff;

	tree_layer_t *l;
	species_t *s;

	l = &c->tree_layers[layer];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* it computes canopy rain interception with the same rationale
	 * used for canopy radiation absorption */

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if( s->value[LAI] < 1.0 ) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if( leaf_cell_cover_eff > 1.0 ) leaf_cell_cover_eff = 1.0;

	logger(g_debug_log, "\n*CANOPY INTERCEPTION*\n");

	/* first height class in the cell is processed */
	if( !layer_height_class_counter && !cell_height_class_counter )
	{
		/* reset temporary values when the first height class in layer is processed */
		temp_int_rain = 0.0;
		temp_int_snow = 0.0;

		/* assign meteo variables to cell variables */
		/* assign incoming rain */
		//fixme alessioc it could use the same rationale of sw radiation usinf meteo_daily
		c->rain = meteo_daily->rain;

		/* assign incoming snow */
		c->snow = meteo_daily->snow;
	}

	/*****************************************************************************************************************/
	/* counters */
	layer_height_class_counter ++;
	cell_height_class_counter++;

	/*************************************************************************/
	/* shared functions among all class/layers */
	/* compute interception for dry canopy (Lawrence et al., 2006) */
	if( meteo_daily->prcp > 0.0 && s->value[LAI] > 0.0 && s->value[CANOPY_WATER] == 0.0 )
	{
		/* for rain */
		if( meteo_daily->rain != 0.0 )
		{
			logger(g_debug_log, "rain = %g mm/m2/day\n", meteo_daily->rain);

			s->value[CANOPY_INT] = s->value[INT_COEFF] * meteo_daily->rain * (1.0 - exp(-0.5 * s->value[LAI])) * leaf_cell_cover_eff;
			logger(g_debug_log, "CANOPY_INT = %g mm/m2/day\n", s->value[CANOPY_INT]);

			s->value[CANOPY_WATER] = s->value[CANOPY_INT];

			CHECK_CONDITION(s->value[CANOPY_INT], > meteo_daily->rain);
		}
		/* for snow */
		else
		{
			/* following Dewire (PhD thesis) and Pomeroy et al., 1998., Hedstrom & Pomeroy, 1998 */
			double Int_max_snow;                           /* maximum intercepted snow (mm)*/

			logger(g_debug_log, "snow = %g mm/m2/day\n", meteo_daily->snow);

			Int_max_snow = 4.4 * s->value[LAI];
			s->value[CANOPY_INT_SNOW] = s->value[CANOPY_SNOW] + 0.7 * ( Int_max_snow - s->value[CANOPY_SNOW] ) *
				(1 - exp( - ( meteo_daily->snow /Int_max_snow ) ) ) * leaf_cell_cover_eff;

			//fixme for now assuming no snow interception
			s->value[CANOPY_INT_SNOW] = 0.0;
			logger(g_debug_log, "CANOPY_INT_SNOW = %g mm/m2/day\n", s->value[CANOPY_INT_SNOW]);
		}
	}

	/**********************************************************************************************************/
	/* update temporary rain and snow */
	temp_int_rain += s->value[CANOPY_INT];
	temp_int_snow += s->value[CANOPY_INT_SNOW];

	/**********************************************************************************************************/

	/* when matches the last height class in the layer is processed */
	if ( l->layer_n_height_class == layer_height_class_counter )
	{
		logger(g_debug_log,"last height class in layer processed\n");
		logger(g_debug_log,"update rain/snow interception values for lower layer\n");

		/* compute interceptable rain for lower layers */
		c->daily_c_rain_int += s->value[CANOPY_INT];
		c->rain -= temp_int_rain;

		/* compute interceptable snow for lower layers */
		c->daily_c_snow_int += s->value[CANOPY_INT_SNOW];
		c->snow -= temp_int_snow;

		/* reset temporary values when the last height class in layer is processed */
		temp_int_rain = 0.0;
		temp_int_snow = 0.0;

		/* reset counter */
		layer_height_class_counter = 0;
	}

	/**********************************************************************************************************/
	/* when matches the last height class in the cell is processed */
	//fixme fixme fixme fixme fixme sometimes it doesn't go in  caused by the a jump in "cell_height_class_counter"
	//as it is now is used just for print data but it should be fixed
	if ( c->heights_count == cell_height_class_counter )
	{
		logger(g_debug_log,"\n***********************************\n");
		logger(g_debug_log,"last height class in cell processed\n");
		logger(g_debug_log, "rain to soil = %g mm\n", c->rain);
		logger(g_debug_log, "snow to soil = %g cm\n", c->snow);

		/* reset counter */
		cell_height_class_counter = 0;
	}
	/*****************************************************************************************************************/
}
