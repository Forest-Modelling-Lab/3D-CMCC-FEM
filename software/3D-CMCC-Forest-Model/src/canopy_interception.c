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


extern void Get_canopy_interception  (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height)
{
	double PotEvap;
	double gamma;
	double sat;

	Log("\nGET_CANOPY_INTERCEPTION-EVAPORATION_ROUTINE\n");

	gamma = 65.05+met[month].d[day].tday*0.064;
	sat = ((2.503e6 * exp((17.268*met[month].d[day].tday)/(237.3+met[month].d[day].tday))))/
			pow((237.3+met[month].d[day].tday),2);


	/*compute fraction of rain intercepted if in growing season*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		if (met[month].d[day].tavg > 0.0 && met[month].d[day].rain > 0.0)
		{
			/*dominant layer*/
			if (c->heights[height].z == c->top_layer)
			{
				/*rainfall intercepted of the day(s) not evaporated before*/
				if(s->value[RAIN_INTERCEPTED] != 0.0)
				{
					Log("intercepted rainfall of the day(s) before = %f\n", s->value[RAIN_INTERCEPTED]);
				}
				/*if canopy is dry recompute*/
				else
				{
					if (s->value[LAIMAXINTCPTN] <= 0)
					{
						s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
					}
					else
					{
						if (settings->spatial == 's')
						{
							s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , met[month].d[day].ndvi_lai / s->value[LAIMAXINTCPTN]);
						}
						else
						{
							s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
						}
					}
					Log("Fraction of rain intercepted = %f %\n", s->value[FRAC_RAIN_INTERC]*100);
					s->value[RAIN_INTERCEPTED] = ((met[month].d[day].rain * s->value[CANOPY_COVER_DBHDC]) * s->value[FRAC_RAIN_INTERC]);
					Log("Canopy interception = %f mm\n", s->value[RAIN_INTERCEPTED]);
					//Log("intercepted water from dominant layer = %f mm \n", c->daily_c_int[c->top_layer]);
					//fixme do the same thing for canopy transpiration!!!!
					/*last height dominant class processed*/
					if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
					{
						c->daily_c_int[c->top_layer] += s->value[RAIN_INTERCEPTED];

						//fixme remove form here and use in soil water balance routine
						/*control*/
						if (met[month].d[day].rain > c->daily_c_int[c->top_layer])
						{
							c->water_to_soil = (met[month].d[day].rain - c->daily_c_int[c->top_layer]);
						}
						else
						{
							c->water_to_soil = 0;
						}
						Log("water to soil = %f mm\n", c->water_to_soil);
					}
				}
			}
			/*dominated*/
			else
			{
				/*rainfall intercepted of the day(s) before not evporated*/
				if(s->value[RAIN_INTERCEPTED] != 0.0)
				{
					Log("intercepted rainfall of the day(s) before = %f\n", s->value[RAIN_INTERCEPTED]);
				}
				/*if canopy is dry recompute*/
				else
				{
					/*dominated layer*/
					if (c->heights[height].z == c->top_layer-1)
					{
						s->value[RAIN_INTERCEPTED] = ((c->water_to_soil * s->value[CANOPY_COVER_DBHDC]) * s->value[FRAC_RAIN_INTERC]);
						Log("Canopy interception = %f mm\n", s->value[RAIN_INTERCEPTED]);

						c->daily_c_int[c->top_layer-1] += s->value[RAIN_INTERCEPTED];
						//Log("intercepted water from dominated layer = %f mm \n", c->daily_c_int[c->top_layer-1]);


						/*last height dominated class processed*/
						if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
						{
							/*control*/
							if (c->water_to_soil > c->daily_c_int[c->top_layer-1])
							{
								c->water_to_soil -= c->daily_c_int[c->top_layer-1];
							}
							else
							{
								c->water_to_soil = 0;
							}
							Log("water to soil = %f mm\n", c->water_to_soil);
						}
					}
					/*subdominated layer*/
					else
					{
						s->value[RAIN_INTERCEPTED] = ((c->water_to_soil * s->value[CANOPY_COVER_DBHDC]) * s->value[FRAC_RAIN_INTERC]);
						Log("Canopy interception = %f mm\n", s->value[RAIN_INTERCEPTED]);

						c->daily_c_int[c->top_layer-2] += s->value[RAIN_INTERCEPTED];
						Log("intercepted water from subdominated layer = %f mm \n", c->daily_c_int[c->top_layer-2]);
						/*last height subdominated class processed*/
						if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
						{
							/*control*/
							if (c->water_to_soil > c->daily_c_int[c->top_layer-2])
							{
								c->water_to_soil -= c->daily_c_int[c->top_layer-2];
							}
							else
							{
								c->water_to_soil = 0;
							}
							Log("water to soil = %f mm\n", c->water_to_soil);
						}
					}
				}
			}

			/*compute total daily interception*/
			//c->daily_tot_c_int += c->daily_c_int[c->heights[height].z];
			//Log("Daily total canopy interception = %f \n", c->daily_tot_c_int);

			/*following Gerten et al., 2004*/
			/*compute potential  and actual evaporation for each layer*/
			PotEvap = (sat / (sat + gamma)/ c->lh_vap)* s->value[NET_RAD_ABS] * 86400;
			Log("PotEvap = %f mmkg/m2/day\n", PotEvap );
			if(PotEvap < 0)
			{
				PotEvap = 0.0;
				Log("Negative Potential Evaporation\n");
				s->value[FRAC_DAYTIME_WET_CANOPY] = 0.0;
			}
			else
			{
				s->value[FRAC_DAYTIME_WET_CANOPY] = Minimum (s->value[RAIN_INTERCEPTED]/(PotEvap*EVAPOCOEFF), 1);
			}
			Log("FRAC_DAYTIME_WET_CANOPY = %f\n", s->value[FRAC_DAYTIME_WET_CANOPY]);
			s->value[CANOPY_EVAPORATION] = PotEvap * EVAPOCOEFF * s->value[FRAC_DAYTIME_WET_CANOPY];
			Log("Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);


			/*check if intercepted rainfall exceeds daily evaporation*/
			if(s->value[CANOPY_EVAPORATION] < s->value[RAIN_INTERCEPTED])
			{
				Log("RAIN INTERCEPTED EXCEEDS CANOPY EVAPORATION\n");
				s->value[RAIN_INTERCEPTED] -= s->value[CANOPY_EVAPORATION];
				Log("remaining rainfall on canopy = %f\n", s->value[RAIN_INTERCEPTED]);
			}
			else
			{
				s->value[RAIN_INTERCEPTED]= 0;
				Log("remaining rainfall on canopy = %f\n", s->value[RAIN_INTERCEPTED]);
			}
		}
		else if (met[month].d[day].tavg > 0.0 && met[month].d[day].rain > 0.0 && s->value[RAIN_INTERCEPTED] > 0.0)
		{
			/*compute potential  and actual evaporation for each layer for wet canopy*/
			PotEvap = (sat / (sat + gamma)/ c->lh_vap)* s->value[NET_RAD_ABS] * 86400;
			Log("PotEvap = %f mmkg/m2/day\n", PotEvap );
			if(PotEvap < 0)
			{
				s->value[FRAC_DAYTIME_WET_CANOPY] = 0.0;
			}
			else
			{
				s->value[FRAC_DAYTIME_WET_CANOPY] = Minimum (s->value[RAIN_INTERCEPTED]/(PotEvap*EVAPOCOEFF), 1);
			}
			Log("FRAC_DAYTIME_WET_CANOPY = %f\n", s->value[FRAC_DAYTIME_WET_CANOPY]);
			s->value[CANOPY_EVAPORATION] = PotEvap * EVAPOCOEFF * s->value[FRAC_DAYTIME_WET_CANOPY];
			Log("Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);

			s->value[RAIN_INTERCEPTED] -= s->value[CANOPY_EVAPORATION];
			Log("Remaining water over the canopy = %f\n", s->value[RAIN_INTERCEPTED]);
			c->water_to_soil = met[month].d[day].rain ;
			Log("water to soil = %f mm\n", c->water_to_soil);
		}
		/*if there's till a canopy wet from the day(s) before bit without daily rainfall*/
		else if (met[month].d[day].tavg > 0.0 && met[month].d[day].rain == 0.0 && s->value[RAIN_INTERCEPTED] > 0.0)
		{
			/*compute potential  and actual evaporation for each layer for wet canopy*/
			PotEvap = (sat / (sat + gamma)/ c->lh_vap)* s->value[NET_RAD_ABS] * 86400;
			Log("PotEvap = %f mmkg/m2/day\n", PotEvap );
			if(PotEvap < 0)
			{
				s->value[FRAC_DAYTIME_WET_CANOPY] = 0.0;
			}
			else
			{
				s->value[FRAC_DAYTIME_WET_CANOPY] = Minimum (s->value[RAIN_INTERCEPTED]/(PotEvap*EVAPOCOEFF), 1);
			}
			Log("FRAC_DAYTIME_WET_CANOPY = %f\n", s->value[FRAC_DAYTIME_WET_CANOPY]);
			s->value[CANOPY_EVAPORATION] = PotEvap * EVAPOCOEFF * s->value[FRAC_DAYTIME_WET_CANOPY];
			Log("Canopy_evaporation = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);

			s->value[RAIN_INTERCEPTED] -= s->value[CANOPY_EVAPORATION];
			Log("Remaining water over the canopy = %f\n", s->value[RAIN_INTERCEPTED]);
			c->water_to_soil = met[month].d[day].rain ;
			Log("water to soil = %f mm\n", c->water_to_soil);
		}
		else
		{
			s->value[RAIN_INTERCEPTED] = 0.0;
			Log("NO RAIN TO INTERCEPT\n");
			s->value[CANOPY_EVAPORATION] = 0;
			Log("Canopy_evaporation for dominant layer = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
			c->daily_c_int[c->heights[height].z] = 0.0;
			c->water_to_soil = met[month].d[day].rain ;
			Log("water to soil = %f mm\n", c->water_to_soil);
		}

		c->daily_tot_c_int += s->value[CANOPY_EVAPORATION];



		//fixme it unreasonable that all rainfall intercepted evaporates simultaneously
		/* calculate the time required to evaporate all the canopy water */
		//evap_dayl = met[month].d[day].rain/e;
		//Log("evap_dayl = %f\n", evap_dayl);


		//fixme it still uses bad data
		/*compute a energy balance evaporation for rain intercepted from canopy and then evaporated*/
		c->daily_tot_c_int_watt = c->daily_tot_c_int * c->lh_vap / 86400;
		Log("Latent heat canopy interception/evaporation = %f W/m^2\n", c->daily_tot_c_int_watt);
	}
	else
	{
		s->value[RAIN_INTERCEPTED] = 0.0;
		Log("NO RAIN TO INTERCEPT\n");
		s->value[CANOPY_EVAPORATION] = 0;
		Log("Canopy_evaporation for dominant layer = %f mmkg/m2/day\n", s->value[CANOPY_EVAPORATION]);
		c->water_to_soil = met[month].d[day].rain ;
		Log("water to soil = %f mm\n", c->water_to_soil);
	}



}
