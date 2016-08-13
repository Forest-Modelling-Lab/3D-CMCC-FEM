/*
 * snow.c
 *
 *  Created on: 06/mar/2016
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "snow.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void snow_melt_subl(cell_t *const c, meteo_daily_t *meteo_daily)
{
	//FOLLOWING BIOME APPROACH
	/* temperature and radiation snowmelt,
		from Joseph Coughlan PhD thesis, 1991 */

	static double snow_abs = 0.6;             /* absorptivity of snow */
	static double t_coeff = 0.65;             /* (kg/m2/deg C/d) temp. snowmelt coeff */
	double incident_rad;                      /* incident radiation(kJ/m2/d) */
	double t_melt, r_melt, r_sub;


	logger(g_log, "**SNOW POOL**\n");

	t_melt = r_melt = r_sub = 0;
	t_melt = t_coeff * meteo_daily->tavg;

	/* canopy transmitted radiation: convert from W/m2 --> KJ/m2/d */
	//fixme wrong incident radiation used!! it should be at the end of radiation functions
	incident_rad = (c->sw_rad_for_soil * meteo_daily->daylength * 3600) * snow_abs * 0.001;

	/* temperature and radiation melt from snow pack */
	if (meteo_daily->tavg > 0.0)
	{
		if (c->snow_pack > 0.0)
		{
			logger(g_log, "tavg = %f\n", meteo_daily->tavg);
			logger(g_log, "snow pack = %f cm\n", c->snow_pack);
			logger(g_log, "Snow melts!!\n");

			r_melt = incident_rad / meteo_daily->lh_fus;
			c->snow_melt = t_melt + r_melt;

			if (c->snow_melt > c->snow_pack)
			{
				/*all snow pack melts*/
				c->snow_melt = c->snow_pack;

				/*reset snow*/
				c->snow_pack = 0.0;
				logger(g_log, "ALL Snow melt!!\n");
				logger(g_log, "snow melt %f\n", c->snow_melt);
			}
			else
			{
				/*snow pack melts partially*/
				c->snow_pack -= c->snow_melt;
				logger(g_log, "snow_pack %f\n", c->snow_pack);
				logger(g_log, "a fraction of Snow melts!!\n");
			}
			c->snow_to_soil = c->snow_melt;
			logger(g_log, "snow to soil %f\n", c->snow_to_soil);

			/*check for balance*/
			logger(g_log, "Snow pack = %f (cm)\n", c->snow_pack);
		}
	}
	else
	{
		r_sub = incident_rad / meteo_daily->lh_sub;

		if (c->snow_pack > 0.0)
		{
			/*snow sublimation*/
			if (r_sub > c->snow_pack)
			{
				logger(g_log, "Snow sublimation!!\n");
				r_sub = c->snow_pack;
				c->snow_subl = r_sub;
				logger(g_log, "Snow sublimated = %f mm\n", c->snow_subl);

				/*check for balance*/
				if (c->snow_subl < c->snow_pack)
				{
					c->snow_pack -= c->snow_subl;
				}
				else
				{
					c->snow_subl = c->snow_pack;
					c->snow_pack = 0.0;
				}
			}
			else
			{
				c->snow_subl = 0.0;
			}
		}
		else
		{
			logger(g_log, "NO snow pack to sublimate\n");
		}
	}

	/* following Lagergren et al., 2006 */
	/* impose zero value for tsoil in case of snow presence */
	//todo move to soil when snow melt and subl will go to soil
	if(c->snow_pack != 0) meteo_daily->tsoil = 0.0;

	logger(g_log, "*****************************************\n");

}
