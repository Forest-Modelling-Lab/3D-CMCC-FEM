/*
 * water_balance.c
 *
 *  Created on: 05/mar/2016
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void water_downward_balance (CELL *c, const MET_DATA *const met, int month, int day)
{
	Log("\nWATER BALANCE\n");


	/* rain vs. snow, and canopy interception */
	if (met[month].d[day].tavg > 0.0)             /* rain */
	{
		if (c->daily_tot_c_int < met[month].d[day].prcp)
		{
			c->precip_soil = met[month].d[day].prcp - c->daily_tot_c_int;
		}
		else                          /* canopy limits interception */
		{
			c->precip_soil = 0.0;
		}
	}
	else                              /* snow */
	{
		c->precip_soil = met[month].d[day].prcp;     /* no interception */
	}
}
