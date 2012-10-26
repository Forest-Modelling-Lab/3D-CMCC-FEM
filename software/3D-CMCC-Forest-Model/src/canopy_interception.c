/*canopy_interception.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

//compute fraction of rain intercpted by canopy

float Get_canopy_interception (SPECIES *const s, const MET_DATA *const met, int month)
{
	float Interception;
	if (s->value[LAIMAXINTCPTN] <= 0)
	{
		Interception = s->value[MAXINTCPTN];
		Log("Rain Interception = MAXINTCPTN\n");
		Log("Rain Interception = %g \n", Interception);
	}
	else
	{
		if (settings->version == 's')
		{
			Interception = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
		}
		else
		{
			Interception = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
		}
		Log("Rain Interception not use MAXINTCPTN\n");
		Log("Rain Interception = %g \n", Interception);
	}

	return Interception;
}
