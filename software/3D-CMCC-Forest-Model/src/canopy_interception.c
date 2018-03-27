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


void canopy_interception(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, meteo_daily_t *const meteo_daily)
{
	static int cell_height_class_counter;
	static int layer_height_class_counter;

	static double temp_int_rain;
	static double temp_int_snow;

	tree_layer_t *l;
	species_t *s;

	l = &c->tree_layers[layer];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* it computes canopy rain interception with a slightly different rationale used for canopy radiation absorption */
	/* note: it is still used cause rainfall is considered always vertical from zenith angle */

	/* CANOPY INTERCEPTION */

	/* first height class in the cell is processed */
	if( ! layer_height_class_counter && ! cell_height_class_counter )
	{
		/* reset temporary values when the first height class in layer is processed */
		temp_int_rain = 0.;
		temp_int_snow = 0.;

		/* assign meteo variables to cell variables */
		/* assign incoming rain */
		//c->rain = meteo_daily->rain;
		/* assign incoming snow */
		//c->snow = meteo_daily->snow;
	}

	/*****************************************************************************************************************/
	/* counters */
	layer_height_class_counter ++;
	cell_height_class_counter++;

	/*************************************************************************/
	/* shared functions among all class/layers */

	//note: not accumulation of rain or snow happens if canopy is wet or snowed */
	if( ( meteo_daily->prcp > 0.) && ( s->value[LAI_PROJ] > 0. ) && ( ! s->value[CANOPY_WATER] ) && ( ! s->value[CANOPY_SNOW] ) )
	{
		/* for rain */
		if( meteo_daily->rain )
		{
			/* compute maximum water storage (interception) (mm/m2 area covered) */

#if 0
			/* compute interception for dry canopy (Lawrence et al., 2006) */

			s->value[CANOPY_INT]      = s->value[INT_COEFF] * meteo_daily->rain * ( 1. - exp(-0.5 * s->value[LAI_PROJ])) * s->value[DAILY_CANOPY_COVER_PROJ];
#else
			//test 01 june 2017
			/* following Jiao et al., 2016, Water Eq. [8] pg. 9 */
			{
				double Int_max_rain;    /* maximum intercepted rain (mm/m2) */

				Int_max_rain = 0.284 + 0.092 * s->value[LAI_PROJ] * ( 1. - exp ( -0.231 * meteo_daily->rain ) );

				s->value[CANOPY_INT_RAIN] = MIN( Int_max_rain , meteo_daily->rain );
			}

#endif
			/* update pool */
			s->value[CANOPY_WATER]    = s->value[CANOPY_INT_RAIN];

			/* make zero snow */
			s->value[CANOPY_INT_SNOW] = 0.;

			CHECK_CONDITION( s->value[CANOPY_INT_RAIN], > , meteo_daily->rain );
		}
		/* for snow */
		else
		{
			/* following Dewire (PhD thesis) and Pomeroy et al., 1998., Hedstrom & Pomeroy, 1998 */

			double Int_max_snow;      /* maximum intercepted snow (mm/m2)*/

			Int_max_snow              = 4.4 * s->value[LAI_PROJ];

			//test
#if 0
			s->value[CANOPY_INT_SNOW] = 0.;
#else
			s->value[CANOPY_INT_SNOW] = s->value[CANOPY_SNOW] + 0.7 * ( Int_max_snow - s->value[CANOPY_SNOW] ) * (1. - exp( - ( meteo_daily->snow / Int_max_snow ) ) ) * s->value[DAILY_CANOPY_COVER_PROJ];
#endif

			/* update pool */
			//fixme for now assuming no snow accumulation
			s->value[CANOPY_SNOW]     = s->value[CANOPY_INT_SNOW];

			/* make zero rain */
			s->value[CANOPY_INT_RAIN] = 0.;

			CHECK_CONDITION(s->value[CANOPY_INT_SNOW], > , meteo_daily->snow);
		}
	}

	/**********************************************************************************************************/

	/* update temporary rain and snow */
	temp_int_rain += s->value[CANOPY_INT_RAIN];
	temp_int_snow += s->value[CANOPY_INT_SNOW];

	/**********************************************************************************************************/

	/* when matches the last height class in the layer is processed */
	if ( l->layer_n_height_class == layer_height_class_counter )
	{
		/* compute interceptable rain for lower layers */
		c->daily_canopy_rain_int += s->value[CANOPY_INT_RAIN];
		meteo_daily->rain        -= temp_int_rain;

		/* compute interceptable snow for lower layers */
		c->daily_canopy_snow_int += s->value[CANOPY_INT_SNOW];
		meteo_daily->snow        -= temp_int_snow;

		/* reset temporary values when the last height class in layer is processed */
		temp_int_rain = 0.;
		temp_int_snow = 0.;

		/* reset counter */
		layer_height_class_counter = 0;
	}

	/**********************************************************************************************************/
	/* when it matches the last height class in the cell is processed */
	if ( c->heights_count == cell_height_class_counter )
	{
		/* last height class in cell processed */

		/* reset counter */
		cell_height_class_counter = 0;
	}
	/*****************************************************************************************************************/

	/* cumulate */
	s->value[MONTHLY_CANOPY_INT] += (s->value[CANOPY_INT_RAIN] + s->value[CANOPY_INT_SNOW]);
	s->value[YEARLY_CANOPY_INT]  += (s->value[CANOPY_INT_RAIN] + s->value[CANOPY_INT_SNOW]);
}
