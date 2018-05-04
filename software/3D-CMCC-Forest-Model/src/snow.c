/*
 * snow.c
 *
 *  Created on: 06/mar/2016
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "math.h"
#include "snow.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

void snow_melt_subl(cell_t *const c, meteo_daily_t *meteo_daily)
{
	/* FOLLOWING BIOME APPROACH */
	/* temperature and radiation snowmelt,
		from Joseph Coughlan PhD thesis, 1991 */

	static double snow_abs = 0.6;             /* absorptivity of snow */
	static double t_coeff = 0.65;             /* (kg/m2/deg C/d) temp. snowmelt coeff */
	double incident_rad;                      /* incident radiation(kJ/m2/d) */
	double t_melt, r_melt, r_sub;

	assert ( c );

	logger(g_debug_log, "**SNOW POOL**\n");

	t_melt = r_melt = r_sub = 0;
	t_melt = t_coeff * meteo_daily->tavg;

	/* canopy transmitted radiation: convert from W/m2 --> KJ/m2/d */
#if 0
	incident_rad = (c->sw_rad_abs_snow * meteo_daily->daylength_sec) * SNOW_ABS * 0.001;
#else
	incident_rad = ( c->net_rad_abs_snow * meteo_daily->daylength_sec) * SNOW_ABS * 0.001 ;
#endif

	/* temperature and radiation melt from snow pack */
	if ( meteo_daily->tavg > 0. )
	{
		if ( c->snow_pack > 0. )
		{
			/* Snow melts */

			r_melt             = incident_rad / meteo_daily->lh_fus;
			c->daily_snow_melt = t_melt + r_melt;

			if ( c->daily_snow_melt > c->snow_pack )
			{
				/* all snow pack melts */
				c->daily_snow_melt = c->snow_pack;
			}
			else
			{
				/*snow pack melts partially*/
			}
		}
	}
	else
	{
		r_sub = incident_rad / meteo_daily->lh_sub;

		if ( c->snow_pack > 0. )
		{
			/*snow sublimation*/
			if ( r_sub > c->snow_pack )
			{
				/* Snow sublimation */

				r_sub              = c->snow_pack;
				c->daily_snow_subl = r_sub;

				/*check for balance*/
				if ( c->daily_snow_subl > c->snow_pack )
				{
					c->daily_snow_subl = c->snow_pack;
				}
				else
				{
					/*snow pack sublimate partially*/
				}
			}
			else
			{
				c->daily_snow_subl = 0.;
			}
		}
	}

	/* following Lagergren et al., 2006 */
	/* impose zero value for tsoil in case of snow presence */
	//if( c->snow_pack ) meteo_daily->tsoil = 0.0;

	logger(g_debug_log, "*****************************************\n");

}
