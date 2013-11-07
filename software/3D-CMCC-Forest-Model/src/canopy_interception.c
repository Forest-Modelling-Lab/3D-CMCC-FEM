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

	//CANOPY INTERCEPTION
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
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
			}
			else
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].d[day].ndvi_lai / s->value[LAIMAXINTCPTN]);
			}
		}
		else
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
		}
		Log("Frac Rain Interception not use MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
	}


	if (settings->time == 'd' && s->counter[VEG_UNVEG == 1])
	{
		if (met[month].d[day].tavg > 0)
		{

		}
	}
	/*no interception if tavg < 0 (snow), or outside growing season*/
	else
	{

	}
	if (settings->time == 'm' && s->counter[VEG_UNVEG == 1])
	{
		if (met[month].tavg > 0)
		{

		}
	}
	/*no interception if tavg < 0 (snow), or outside growing season*/
	else
	{

	}

}
