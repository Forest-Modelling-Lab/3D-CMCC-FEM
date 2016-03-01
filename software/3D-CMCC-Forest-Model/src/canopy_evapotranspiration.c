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


extern void Get_canopy_evapotranspiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, double vpd, int height, int age, int species)
{

	static int i;
	Log("\nGET_CANOPY_EVAPOTRANSPIRATION_ROUTINE\n");

	/*it computes canopy evaporation + canopy transpiration*/

		if (settings->time == 'd')
		{
			/*dominant layer*/
			if (c->heights[height].z == c->top_layer)
			{
				s->value[CANOPY_EVAPOTRANSPIRATION] = s->value[DAILY_TRANSP] + s->value[RAIN_INTERCEPTED];
				Log("Canopy evapotranspiration = %f mm\n", s->value[CANOPY_EVAPOTRANSPIRATION]);
				/*last height dominant class processed*/
				if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
				{
					/*control*/
					if (c->available_soil_water < c->daily_c_evapotransp[c->top_layer])
					{
						Log("ATTENTION DAILY EVAPOTRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
						c->daily_c_evapotransp[c->top_layer] = c->available_soil_water;
					}
				}
			}
			/*dominated*/
			else
			{
				/*dominated layer*/
				if (c->heights[height].z == c->top_layer-1)
				{
					s->value[CANOPY_EVAPOTRANSPIRATION] = s->value[DAILY_TRANSP] + s->value[RAIN_INTERCEPTED];
					Log("Canopy evapotranspiration = %f mm\n", s->value[CANOPY_EVAPOTRANSPIRATION]);
					/*last height dominant class processed*/
					if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
					{
						/*control*/
						if (c->available_soil_water < c->daily_c_evapotransp[c->top_layer-1])
						{
							Log("ATTENTION DAILY EVAPOTRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
							c->daily_c_evapotransp[c->top_layer-1] = c->available_soil_water;
						}
					}
				}
				/*subdominated layer*/
				else
				{
					s->value[CANOPY_EVAPOTRANSPIRATION] = s->value[DAILY_TRANSP] + s->value[RAIN_INTERCEPTED];
					Log("Canopy evapotranspiration = %f mm\n", s->value[CANOPY_EVAPOTRANSPIRATION]);
					/*last height dominant class processed*/
					if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
					{
						/*control*/
						if (c->available_soil_water < c->daily_c_evapotransp[c->top_layer-2])
						{
							Log("ATTENTION DAILY EVAPOTRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
							c->daily_c_evapotransp[c->top_layer-2] = c->available_soil_water;
						}
					}
				}
			}
		}
		/*monthly*/
		else
		{

		}

	i = c->heights[height].z;

	c->daily_c_evapotransp[i] += s->value[CANOPY_EVAPOTRANSPIRATION];
	c->monthly_c_evapotransp[i] += s->value[CANOPY_EVAPOTRANSPIRATION];
	c->annual_c_evapotransp[i] += s->value[CANOPY_EVAPOTRANSPIRATION];

	c->daily_tot_c_evapotransp += s->value[CANOPY_EVAPOTRANSPIRATION];
	c->monthly_tot_c_evapotransp += s->value[CANOPY_EVAPOTRANSPIRATION];
	c->annual_tot_c_evapotransp += s->value[CANOPY_EVAPOTRANSPIRATION];


	/*compute energy balance evapotranspiration from canopy*/
	c->daily_tot_c_evapotransp_watt = c->daily_tot_c_transp_watt + c->daily_tot_c_int_watt;
	Log("Energy for canopy evapotranspiration = %f W/m^2\n", c->daily_tot_c_evapotransp_watt);

}



