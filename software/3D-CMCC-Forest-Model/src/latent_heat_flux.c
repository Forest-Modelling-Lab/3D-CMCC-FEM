/*
 * canopy_evaporation.c
 *
 *  Created on: 07/nov/2013
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

extern void Get_latent_heat_flux (SPECIES *const s, CELL *c, const MET_DATA *const met, int month, int day, int height)
{

	double conversion_factor; //factor to convert mm/day of ET to W/m^2

	Log("\nGET_LATENT_HEAT_ROUTINE\n");


	c->daily_tot_latent_heat_flux = c->daily_tot_c_evapotransp_watt + c->daily_soil_evaporation_watt;

	/*in case of snow formation*/
	if(c->daily_snow != 0.0)
	{
		//fixme it doesn't seems to have sense this conversion factor
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		c->daily_tot_latent_heat_flux += c->snow_subl * (c->lh_sub * 1000) / 86400;
		Log("Daily total latent heat flux with sublimation = %f W/m^2\n", c->daily_tot_latent_heat_flux);
	}
	else
	{
		Log("PROVA LATENT HEAT = %f\n", c->daily_tot_latent_heat_flux);
	}
}
