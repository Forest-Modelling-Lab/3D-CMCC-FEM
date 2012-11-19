/*
 * soilmodel.c
 *
 *  Created on: 16/nov/2012
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"

//extern  const char *szMonth[MONTHS];
//extern int MonthLength[];
//extern int DaysInMonth[];


void soil_model(CELL *c, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	Log("--SOIL MODEL ROUTINE--\n");

	Log("*********GET MONTHLY SOIL WATER BALACE************\n");
	/*Take off Evapotranspiration*/
	c->available_soil_water -= c->evapotranspiration ;

	Log("ASW at the END of month less Evapotraspiration = %g mm\n", c->available_soil_water);
	if ( c->available_soil_water < 0)
	{
		Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
		c->available_soil_water = site->minAsw;
		Log("ASW = %g\n", c->available_soil_water);
	}
	if ( c->available_soil_water > site->maxAsw)
	{
		Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
		c->available_soil_water = site->maxAsw;
		Log("Available soil water = %g\n", c->available_soil_water);
	}

	/* ok */
	return 1;
}
