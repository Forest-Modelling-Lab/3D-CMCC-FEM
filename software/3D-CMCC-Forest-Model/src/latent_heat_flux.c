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

	/*upward energy fluxes*/
	//for consistency with eddy data use tday
	if(met[month].d[day].tday > 0.0) /*positive values of latent heat flux*/
	{
		if (met[month].d[day].tday <= 5)
		{
			conversion_factor = 28.94;
		}
		else if (met[month].d[day].tday <= 15)
		{
			conversion_factor = 28.66;
		}
		else if (met[month].d[day].tday <= 25)
		{
			conversion_factor = 28.35;
		}
		else if (met[month].d[day].tday <= 35)
		{
			conversion_factor = 28.00;
		}
		else
		{
			conversion_factor = 27.63;
		}
		if(c->snow_pack != 0)
		{
			//todo add function for latent heat flux from snow melt
		}

		//fixme
		/*just an attempt to compute latent heat flux*/
		//fixme ERROR MODEL GETS daily_tot_C_TRANSP!! not all evaporated fluxes
		c->daily_tot_latent_heat_flux = conversion_factor * c->daily_tot_c_transp;
		Log("Daily total evapotransp = %f W/m^2\n", c->daily_tot_c_transp);
		Log("Daily total latent heat flux = %f W/m^2\n", c->daily_tot_latent_heat_flux);
	}
	else /*negative values of latent heat flux*/
	{
		/*downward energy fluxes*/
		/*in case of snow formation*/
		if(c->daily_snow != 0.0)
		{
			//fixme it doesn't seems to have sense this conversion factor
			/*
				conversion_factor = 28.94;
				c->daily_tot_latent_heat_flux = (conversion_factor * c->daily_snow) * (-1);
				Log("Daily total latent heat flux with rain/snow != 0 = %f W/m^2\n", c->daily_tot_latent_heat_flux);
			 */
		}
		else
		{
			c->daily_tot_latent_heat_flux = 0;
			Log("Daily total latent heat flux = %f W/m^2\n", c->daily_tot_latent_heat_flux);
		}
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		//fixme check if miss something
		c->daily_tot_latent_heat_flux += 28.94 * c->snow_subl;
		Log("Daily total latent heat flux with sublimation = %f W/m^2\n", c->daily_tot_latent_heat_flux);
	}

	Log("PROVA LATENT HEAT = %f\n", c->daily_tot_c_evapotransp_watt + c->daily_soil_evaporation_watt);
}
