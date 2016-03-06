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
#include "types.h"
#include "constants.h"

void Check_prcp (CELL *c, MET_DATA *met, int month, int day)
{

	//FOLLOWING BIOME APPROACH
	/* temperature and radiation snowmelt,
	from Joseph Coughlan PhD thesis, 1991 */

	static double snow_abs = 0.6; // absorptivity of snow
	static double t_coeff = 0.65; // (kg/m2/deg C/d) temp. snowmelt coeff
	double incident_rad;  //incident radiation (kJ/m2/d) incident radiation
	double t_melt, r_melt, r_sub;


	Log("-CHECK PRECIPITATION-\n");

	c->snow_subl = 0.0;
	c->snow_melt = 0.0;
	c->daily_snow = 0.0;
	c->daily_rain = 0.0;



	t_melt = r_melt = r_sub = 0;
	t_melt = t_coeff * met[month].d[day].tavg;

	/* canopy transmitted radiation: convert from W/m2 --> KJ/m2/d */
	//incident_rad = c->net_radiation_for_soil * snow_abs * 0.001;

	/* canopy transmitted radiation: convert from MJ/m2/d  --> KJ/m2/d */
	incident_rad = met[month].d[day].solar_rad * (met[month].d[day].solar_rad / 24.0) * snow_abs * 1000;


	/* temperature and radiation melt from snowpack */
	if (met[month].d[day].tavg > 0.0)
	{
		c->daily_rain = met[month].d[day].prcp;

		if (c->snow_pack > 0.0)
		{
			Log("tavg = %f\n", met[month].d[day].tavg);
			Log("snow pack = %f cm\n", c->snow_pack);
			Log("Snow melt!!\n");
			r_melt = incident_rad / c->lh_fus;
			c->snow_melt = t_melt + r_melt;
			//Log("snow_melt %f\n", c->snow_melt);


			if (c->snow_melt > c->snow_pack)
			{
				/*all snow pack melts*/
				c->snow_melt = c->snow_pack;

				/*reset snow*/
				c->snow_pack = 0.0;
				Log("ALL Snow melt!!\n");
				Log("snow melt %f\n", c->snow_melt);
			}
			else
			{
				/*snow pack melts partially*/
				c->snow_pack -= c->snow_melt;
				Log("snow_pack %f\n", c->snow_pack);
				Log("A FRACTION OF Snow melt!!\n");
			}
			c->snow_to_soil=c->snow_melt;
			Log("snow to soil %f\n", c->snow_to_soil);


			/*check for balance*/
			Log("Snow pack = %f (cm)\n", c->snow_pack);
		}
	}
	else
	{
		if(met[month].d[day].prcp > 0.0)
		{
			Log("tavg = %f\n", met[month].d[day].tavg);
			Log("rain becomes snow\n");

			c->daily_snow = met[month].d[day].prcp;
			Log("Daily snow = %f cm\n", c->daily_snow);

			c->snow_pack += c->daily_snow;
			Log("snow pack  + daily snow= %f cm\n", c->snow_pack);
		}
		else
		{
			c->daily_snow = 0.0;
			Log("NO rain NO snow\n");
			Log("snow pack = %f cm\n", c->snow_pack);
		}

		r_sub = incident_rad / c->lh_sub;

		if (c->snow_pack > 0.0)
		{
			/*snow sublimation*/
			if (r_sub > c->snow_pack)
			{
				Log("Snow sublimation!!\n");
				r_sub = c->snow_pack;
				c->snow_subl = r_sub;
				Log("Snow sublimated = %f mm\n", c->snow_subl);
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
			Log("NO snow pack to sublimate\n");
		}
	}

	Log("*****************************************\n");
}
