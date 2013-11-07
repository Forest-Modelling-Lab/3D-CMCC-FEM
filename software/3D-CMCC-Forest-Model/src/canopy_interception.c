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


extern void Get_canopy_interception  (SPECIES *const s, CELL *c, const MET_DATA *const met, int month, int day, int height)
{

	Log("\nGET_CANOPY_INTERCEPTION_ROUTINE\n");

	/*compute fraction of rain intercepted if in growing season*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		if (s->value[LAIMAXINTCPTN] <= 0)
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
		}
		else
		{
			if (settings->spatial == 's')
			{
				if(settings->time == 'm')
				{
					s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
				}
				else
				{
					s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , met[month].d[day].ndvi_lai / s->value[LAIMAXINTCPTN]);
				}
			}
			else
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1.0 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
			}
		}
	}

	/*daily*/
	if (settings->time == 'd' )
	{
		if (met[month].d[day].tavg > 0.0 && met[month].d[day].rain > 0.0)
		{
			if (s->counter[VEG_UNVEG] == 1)
			{
				/*dominant layer*/
				if (c->heights[height].z == c->top_layer)
				{
					s->value[RAIN_INTERCEPTED] = ((met[month].d[day].rain * s->value[CANOPY_COVER_DBHDC]) * s->value[FRAC_RAIN_INTERC]);
					Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
					c->daily_c_int[c->top_layer] += s->value[RAIN_INTERCEPTED];
					Log("intercepted water from dominant layer = %g mm \n", c->daily_c_int[c->top_layer]);
					//fixme do the same thing for canopy transpiration!!!!
					/*last height dominant class processed*/
					if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
					{
						/*control*/
						if (met[month].d[day].rain > c->daily_c_int[c->top_layer])
						{
							c->water_to_soil = (met[month].d[day].rain - c->daily_c_int[c->top_layer]);
						}
						else
						{
							c->water_to_soil = 0;
						}
						Log("water to soil = %g mm\n", c->water_to_soil);
					}
				}
				/*dominated*/
				else
				{
					/*dominated layer*/
					if (c->heights[height].z == c->top_layer-1)
					{
						s->value[RAIN_INTERCEPTED] = ((c->water_to_soil * s->value[CANOPY_COVER_DBHDC]) * s->value[FRAC_RAIN_INTERC]);
						Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
						c->daily_c_int[c->top_layer-1] += s->value[RAIN_INTERCEPTED];
						Log("intercepted water from dominated layer = %g mm \n", c->daily_c_int[c->top_layer-1]);
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
							Log("water to soil = %g mm\n", c->water_to_soil);
						}
					}
					/*subdominated layer*/
					else
					{
						s->value[RAIN_INTERCEPTED] = ((c->water_to_soil * s->value[CANOPY_COVER_DBHDC]) * s->value[FRAC_RAIN_INTERC]);
						Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
						c->daily_c_int[c->top_layer-2] += s->value[RAIN_INTERCEPTED];
						Log("intercepted water from subdominated layer = %g mm \n", c->daily_c_int[c->top_layer-2]);
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
							Log("water to soil = %g mm\n", c->water_to_soil);
						}
					}
				}
			}
			else
			{
					s->value[RAIN_INTERCEPTED] = 0.0;
					Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
					c->daily_c_int[c->heights[height].z] = 0.0;
					Log("Intercepted water from dominant layer = %g mm \n", c->daily_c_int[c->heights[height].z]);
					c->water_to_soil = met[month].d[day].rain ;
					Log("water to soil = %g mm\n", c->water_to_soil);

			}
		}
		else
		{
			s->value[RAIN_INTERCEPTED] = 0.0;
			Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
			c->daily_c_int[c->heights[height].z] = 0.0;
			Log("intercepted water from dominant layer = %g mm \n", c->daily_c_int[c->heights[height].z]);
			c->water_to_soil = met[month].d[day].rain;
			Log("water to soil = %g mm\n", c->water_to_soil);
		}
	}
	/*no interception if tavg < 0 (snow), or outside growing season*/

	//fixme do it as in daily version!!!
	/*monthly*/
	if (settings->time == 'm' )
	{
		if (met[month].tavg > 0.0 && met[month].rain > 0.0)
		{
			if (s->counter[VEG_UNVEG] == 1)
			{
				if (c->heights[height].z == c->top_layer)
				{
					s->value[RAIN_INTERCEPTED] = (met[month].rain * s->value[FRAC_RAIN_INTERC]) * s->value[CANOPY_COVER_DBHDC];
					Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
					c->daily_c_int[c->top_layer] += s->value[RAIN_INTERCEPTED];
					//fixme do the same thing for canopy transpiration!!!!
					/*last height dominant class processed*/
					if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
					{
						/*compute effective amount of water intercepted*/
						c->daily_c_int[c->top_layer] *= met[month].rain * c->gapcover[c->top_layer];
						Log("intercepted water from top layer = %g mm \n", c->daily_c_int[c->top_layer]);
					}
				}
			}
			else
			{
				s->value[RAIN_INTERCEPTED] = 0.0;
				Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
				c->daily_c_int[c->top_layer] = 0.0;
				Log("Intercepted water from top layer = %g mm \n", c->daily_c_int[c->top_layer]);
			}
		}
		else
		{
			s->value[RAIN_INTERCEPTED] = 0.0;
			Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
			c->daily_c_int[c->top_layer] = 0.0;
			Log("intercepted water from top layer = %g mm \n", c->daily_c_int[c->top_layer]);
		}
	}
}
