/*canopy_interception.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

//compute fraction of rain intercepted by canopy

extern void Get_frac_canopy_interception (SPECIES *const s, const MET_DATA *const met, int month)
{

	//interception is a rate not a quantity


	//see also CLM model for rain interception
	/*
	Interception = 1 - exp (-0.5 * m->cells[cell].heights[height].ages[age].species[species].value[LAI])
	 */

	if (s->value[LAIMAXINTCPTN] <= 0)
	{
		s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
		Log("Frac Rain Interception = MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
	}
	else
	{
		if (settings->version == 's')
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
		}
		else
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
		}
		Log("Frac Rain Interception not use MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
	}
}
