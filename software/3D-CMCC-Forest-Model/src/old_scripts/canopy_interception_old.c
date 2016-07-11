/*
 * canopy_interception.c
 *
 *  Created on: 06/nov/2013
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;


void Canopy_interception  (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height)
{
	double PotEvap;
	double gamma;
	double sat;
	double cell_coverage;

	/*following Priestley and Taylor,1972; Gerten et al., 2004*/
	/**********************************************************/

	logger(g_log, "\nCANOPY_INTERCEPTION-EVAPORATION_ROUTINE\n");

	if(s->value[CANOPY_COVER_DBHDC] > 1.0)
	{
		cell_coverage = 1.0;
	}
	else
	{
		cell_coverage = s->value[CANOPY_COVER_DBHDC];
	}

	gamma = 65.05+met[month].d[day].tday*0.064;
	sat = ((2.503e6 * exp((17.268*met[month].d[day].tday)/(237.3+met[month].d[day].tday))))/
			pow((237.3+met[month].d[day].tday),2);

	logger(g_log, "Rainfall = %f mm\n", met[month].d[day].prcp);


	/*compute potential  and actual evaporation for each layer*/
	PotEvap = (sat / (sat + gamma)/ met[month].d[day].lh_vap)* s->value[NET_SW_RAD_ABS] * 86400;
	logger(g_log, "Potential evaporation = %f mm/m^2 day\n",PotEvap);

	/*compute interception rate */
	if (settings->spatial == 's')
	{
		s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , met[month].d[day].ndvi_lai / s->value[LAIMAXINTCPTN]);
	}
	else
	{
		s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
	}
	logger(g_log, "remaining rainfall on canopy from the day(s) before = %f\n", s->value[CANOPY_WATER_STORED]);

	/*no snow but rain and canopy dry*/
	if (met[month].d[day].tavg > 0.0 && c->prcp_rain > 0.0 && s->value[CANOPY_WATER_STORED] == 0.0)
	{
		logger(g_log, "Fraction of rain intercepted = %f %\n", s->value[FRAC_RAIN_INTERC]*100);
		/*dominant layer*/
		if (c->heights[height].z == c->top_layer)
		{
			s->value[RAIN_INTERCEPTED] = ((c->prcp_rain * cell_coverage) * s->value[FRAC_RAIN_INTERC]);
			logger(g_log, "Canopy interception = %f mm/m^2\n", s->value[RAIN_INTERCEPTED]);
			//fixme do the same thing for canopy transpiration!!!!
			/*last height dominant class processed*/
			if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
			{
				c->layer_daily_c_int[c->top_layer] += s->value[RAIN_INTERCEPTED];

				//fixme remove form here and use in soil water balance routine
				/*control*/
				if (c->prcp_rain > c->layer_daily_c_int[c->top_layer])
				{
					c->water_to_soil = (c->prcp_rain - c->layer_daily_c_int[c->top_layer]);
				}
				else
				{
					c->water_to_soil = 0;
				}
				logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
			}

		}
		/*dominated*/
		else
		{
			/*no interception occurs if canopy is still wet from the day(s) before*/
			if(s->value[CANOPY_WATER_STORED] != 0.0)
			{
				logger(g_log, "canopy water of the day(s) before = %f\n", s->value[CANOPY_WATER_STORED]);
			}
			/*if canopy is dry recompute*/
			else
			{
				/*dominated layer*/
				if (c->heights[height].z == c->top_layer-1)
				{
					s->value[RAIN_INTERCEPTED] = ((c->water_to_soil * cell_coverage) * s->value[FRAC_RAIN_INTERC]);
					logger(g_log, "Canopy interception based on coverage = %f mm\n", s->value[RAIN_INTERCEPTED]);

					c->layer_daily_c_int[c->top_layer-1] += s->value[RAIN_INTERCEPTED];
					//logger(g_log, "intercepted water from dominated layer = %f mm \n", c->daily_c_int[c->top_layer-1]);


					/*last height dominated class processed*/
					if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
					{
						/*control*/
						if (c->water_to_soil > c->layer_daily_c_int[c->top_layer-1])
						{
							c->water_to_soil -= c->layer_daily_c_int[c->top_layer-1];
						}
						else
						{
							c->water_to_soil = 0;
						}
						logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
					}
				}
				/*subdominated layer*/
				else
				{
					s->value[RAIN_INTERCEPTED] = ((c->water_to_soil * cell_coverage) * s->value[FRAC_RAIN_INTERC]);
					logger(g_log, "Canopy interception based on coverage = %f mm\n", s->value[RAIN_INTERCEPTED]);

					c->layer_daily_c_int[c->top_layer-2] += s->value[RAIN_INTERCEPTED];
					logger(g_log, "intercepted water from subdominated layer = %f mm \n", c->layer_daily_c_int[c->top_layer-2]);
					/*last height subdominated class processed*/
					if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
					{
						/*control*/
						if (c->water_to_soil > c->layer_daily_c_int[c->top_layer-2])
						{
							c->water_to_soil -= c->layer_daily_c_int[c->top_layer-2];
						}
						else
						{
							c->water_to_soil = 0;
						}
						logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
					}
				}
			}
		}
		logger(g_log, "PotEvap = %f mmkg/m2/day\n", PotEvap );
		if(PotEvap <= 0)
		{
			PotEvap = 0.0;
			logger(g_log, "Negative Potential Evaporation\n");
			s->value[FRAC_DAYTIME_EVAPO] = 0.0;
			s->value[CANOPY_EVAPORATION] = 0.0;
			s->value[CANOPY_WATER_STORED] = s->value[RAIN_INTERCEPTED];
		}
		else
		{
			//test it seems doesn't work
			//			s->value[FRAC_DAYTIME_EVAPO] = Minimum (s->value[RAIN_INTERCEPTED]/(PotEvap*EVAPOCOEFF), 1.0);
			//			logger(g_log, "FRAC_DAYTIME_WET_CANOPY = %f\n", s->value[FRAC_DAYTIME_EVAPO]);
			//			/* just a part of rain evaporates */
			//			if(s->value[FRAC_DAYTIME_EVAPO] == 1.0)
			//			{
			//				s->value[CANOPY_EVAPORATION] = PotEvap * EVAPOCOEFF/* * s->value[FRAC_DAYTIME_WET_CANOPY]*/;
			//				logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
			//				s->value[CANOPY_WATER_STORED] = s->value[RAIN_INTERCEPTED] - s->value[CANOPY_EVAPORATION];
			//				logger(g_log, "remaining rainfall on canopy = %f\n", s->value[CANOPY_WATER_STORED]);
			//
			//			}
			//			/* all rain evaporates */
			//			else
			//			{
			//				s->value[CANOPY_EVAPORATION] = s->value[RAIN_INTERCEPTED];
			//				s->value[RAIN_INTERCEPTED] = 0.0;
			//				s->value[CANOPY_WATER_STORED] = 0.0;
			//			}
			/* all rain evaporates */
			if (PotEvap > s->value[RAIN_INTERCEPTED])
			{
				s->value[CANOPY_EVAPORATION] = s->value[RAIN_INTERCEPTED];
				s->value[CANOPY_WATER_STORED] = 0.0;
			}
			else
			{
				s->value[CANOPY_EVAPORATION] = PotEvap;
				s->value[CANOPY_WATER_STORED] = s->value[RAIN_INTERCEPTED] - s->value[CANOPY_EVAPORATION];
			}
		}
		logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
		logger(g_log, "remaining rainfall on canopy = %f\n", s->value[CANOPY_WATER_STORED]);
	}
	/*no snow but rain but no interception occurs cause canopy still wet since the day(s) before*/
	else if (met[month].d[day].tavg > 0.0 && c->prcp_rain > 0.0 && s->value[CANOPY_WATER_STORED] > 0.0)
	{
		logger(g_log, "Fraction of rain intercepted = %f %\n", s->value[FRAC_RAIN_INTERC]*100);
		/*check if canopy is not completely wet*/
		s->value[RAIN_INTERCEPTED] = 0.0;
		if(PotEvap <= 0)
		{
			PotEvap = 0.0;
			logger(g_log, "Negative Potential Evaporation\n");
			s->value[FRAC_DAYTIME_EVAPO] = 0.0;
		}
		else
		{
			//			s->value[FRAC_DAYTIME_EVAPO] = Minimum (s->value[CANOPY_WATER_STORED]/(PotEvap*EVAPOCOEFF), 1.0);
			//			logger(g_log, "FRAC_DAYTIME_WET_CANOPY = %f\n", s->value[FRAC_DAYTIME_EVAPO]);
			//			/*all rain evaporates*/
			//			if(s->value[FRAC_DAYTIME_EVAPO] == 1.0)
			//			{
			//				s->value[CANOPY_EVAPORATION] = s->value[CANOPY_WATER_STORED];
			//				s->value[RAIN_INTERCEPTED] = 0.0;
			//				s->value[CANOPY_WATER_STORED] = 0.0;
			//			}
			//			/*just a part of rain evaporates*/
			//			else
			//			{
			//				s->value[CANOPY_EVAPORATION] = PotEvap * EVAPOCOEFF/* * s->value[FRAC_DAYTIME_WET_CANOPY]*/;
			//				logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
			//
			//				s->value[CANOPY_WATER_STORED] -= s->value[CANOPY_EVAPORATION];
			//				logger(g_log, "remaining rainfall on canopy = %f\n", s->value[CANOPY_WATER_STORED]);
			//			}
			/* all rain evaporates */
			if (PotEvap > s->value[RAIN_INTERCEPTED])
			{
				s->value[CANOPY_EVAPORATION] = s->value[RAIN_INTERCEPTED];
				s->value[CANOPY_WATER_STORED] = 0.0;
			}
			else
			{
				s->value[CANOPY_EVAPORATION] = s->value[RAIN_INTERCEPTED] - PotEvap;
				s->value[CANOPY_WATER_STORED] = s->value[RAIN_INTERCEPTED] - s->value[CANOPY_EVAPORATION];
			}
			logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
			logger(g_log, "remaining rainfall on canopy = %f\n", s->value[CANOPY_WATER_STORED]);
		}
		c->water_to_soil = c->prcp_rain ;
		logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
	}
	/*no snow no rain but canopy wet from the day(s) before*/
	else if (met[month].d[day].tavg > 0.0 && c->prcp_rain == 0.0 && s->value[CANOPY_WATER_STORED] > 0.0)
	{
		if(PotEvap <= 0)
		{
			s->value[FRAC_DAYTIME_EVAPO] = 0.0;
		}
		else
		{
			//			s->value[FRAC_DAYTIME_EVAPO] = Minimum (s->value[CANOPY_WATER_STORED]/(PotEvap*EVAPOCOEFF), 1.0);
			//			logger(g_log, "FRAC_DAYTIME_WET_CANOPY = %f\n", s->value[FRAC_DAYTIME_EVAPO]);
			//			/*all rain evaporates*/
			//			if(s->value[FRAC_DAYTIME_EVAPO] == 1.0)
			//			{
			//				s->value[CANOPY_EVAPORATION] = s->value[CANOPY_WATER_STORED];
			//				s->value[RAIN_INTERCEPTED] = 0.0;
			//				s->value[CANOPY_WATER_STORED] = 0.0;
			//			}
			//			/*just a part of rain evaporates*/
			//			else
			//			{
			//				s->value[CANOPY_EVAPORATION] = PotEvap * EVAPOCOEFF * s->value[FRAC_DAYTIME_EVAPO];
			//				logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
			//
			//				s->value[CANOPY_WATER_STORED] -= s->value[CANOPY_EVAPORATION];
			//				logger(g_log, "remaining rainfall on canopy = %f\n", s->value[CANOPY_WATER_STORED]);
			//			}
			/* all rain evaporates */
			if (PotEvap > s->value[RAIN_INTERCEPTED])
			{
				s->value[CANOPY_EVAPORATION] = s->value[RAIN_INTERCEPTED];
				s->value[CANOPY_WATER_STORED] = 0.0;
			}
			else
			{
				s->value[CANOPY_EVAPORATION] = s->value[RAIN_INTERCEPTED] - PotEvap;
				s->value[CANOPY_WATER_STORED] = s->value[RAIN_INTERCEPTED] - s->value[CANOPY_EVAPORATION];
			}
			logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
			logger(g_log, "remaining rainfall on canopy = %f\n", s->value[CANOPY_WATER_STORED]);
		}
		c->water_to_soil = c->prcp_rain ;
		logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
	}
	else if (met[month].d[day].tavg > 0.0 && c->prcp_rain == 0.0 && s->value[CANOPY_WATER_STORED] == 0.0)
	{
		s->value[RAIN_INTERCEPTED] = 0.0;
		logger(g_log, "Canopy_interception = %f mmkg/m2/day\n", s->value[RAIN_INTERCEPTED]);
		s->value[CANOPY_EVAPORATION] = 0;
		logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
		s->value[CANOPY_WATER_STORED] = 0.0;
		logger(g_log, "Canopy_water_stored = %f mmkg/m2/day\n", s->value[CANOPY_WATER_STORED]);
		c->layer_daily_c_int[c->heights[height].z] = 0.0;
		c->water_to_soil = c->prcp_rain ;
		logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
	}
	else
	{
		/*snow*/
		s->value[FRAC_DAYTIME_EVAPO] = 0.0;
		s->value[RAIN_INTERCEPTED] = 0.0;
		logger(g_log, "Canopy_interception = %f mmkg/m2/day\n", s->value[RAIN_INTERCEPTED]);
		s->value[CANOPY_EVAPORATION] = 0;
		logger(g_log, "Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
		s->value[CANOPY_WATER_STORED] = 0.0;
		logger(g_log, "Canopy_water_stored = %f mmkg/m2/day\n", s->value[CANOPY_WATER_STORED]);
		c->layer_daily_c_int[c->heights[height].z] = 0.0;
		c->water_to_soil = c->prcp_snow ;
		logger(g_log, "water to soil = %f mm\n", c->water_to_soil);
	}

	/* upscale to cell coverage level */
	s->value[CANOPY_EVAPORATION] *= cell_coverage;

	//test
	/*compute fraction of the day in which canopy can transpire if wet*/
	s->value[FRAC_DAYTIME_TRANSP] = 1.0 - s->value[FRAC_DAYTIME_EVAPO];

	//fixme it may be a problem in multispecies multilayered runs
	//c->daily_tot_c_water_stored += s->value[CANOPY_WATER_STORED];
	c->daily_c_int += s->value[RAIN_INTERCEPTED];
	c->daily_c_evapo += s->value[CANOPY_EVAPORATION];
	c->daily_c_water_stored += s->value[CANOPY_WATER_STORED];
	logger(g_log, "Canopy evaporation = %f mm/m2 day\n", c->daily_c_evapo);

	//fixme it still uses bad data
	/*compute a energy balance evaporation for rain intercepted from canopy and then evaporated*/
	c->daily_c_int_watt = c->daily_c_evapo * met[month].d[day].lh_vap / 86400.0;
	logger(g_log, "Latent heat canopy interception/evaporation = %f W/m^2\n", c->daily_c_int_watt);



}
