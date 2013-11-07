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
	static int veg_dominant_counter;
	static int veg_dominated_counter;
	static int veg_subdominant_counter;

	/*compute fraction of rain intercepted*/
	if (s->value[LAIMAXINTCPTN] <= 0)
	{
		s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
		Log("Frac Rain Interception = MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
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
		Log("Frac Rain Interception not use MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
	}


	if (settings->time == 'd' && s->counter[VEG_UNVEG] == 1)
	{
		if (met[month].d[day].tavg > 0)
		{
			if (c->heights[height].z == c->top_layer)
			{
				s->value[RAIN_INTERCEPTED] = (met[month].d[day].rain * s->value[FRAC_RAIN_INTERC]) * s->value[CANOPY_COVER_DBHDC];
				c->daily_canopy_interception[c->top_layer] += s->value[RAIN_INTERCEPTED];
				/*last height dominant class processed*/
				if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
				{
					/*compute effective amount of water intercepted*/
					c->daily_canopy_interception[c->top_layer] *= met[month].d[day].rain * c->gapcover[c->top_layer];
					Log("intercepted water from top layer = %g mm \n", c->daily_canopy_interception[c->top_layer]);
				}
			}
		}
	}
	/*no interception if tavg < 0 (snow), or outside growing season*/
	else
	{
		s->value[RAIN_INTERCEPTED] = 0.0;
	}
	if (settings->time == 'm' && s->counter[VEG_UNVEG] == 1)
	{
		if (met[month].tavg > 0)
		{
			s->value[RAIN_INTERCEPTED] = met[month].rain * s->value[FRAC_RAIN_INTERC]* s->value[CANOPY_COVER_DBHDC];
		}
	}
	/*no interception if tavg < 0 (snow), or outside growing season*/
	else
	{
		s->value[RAIN_INTERCEPTED] = 0.0;
	}

}
