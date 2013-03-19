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
#include "constants.h"



//this function should be used only for function not directly related to the forest layers, it just run at the end
//of all physiological function for all forest layers


void soil_model(MATRIX *const m, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	int cell;
	int soil_layer;

	// check parameters
	assert(m && yos);

	for ( cell = 0; cell < m->cells_count; cell++)
	{

		Log("--SOIL MODEL ROUTINE--\n");

		Log("*********GET MONTHLY SOIL WATER BALACE************\n");

		if (settings->soil_layer == 1 /*m->cells[cell].soils_count == 1*/)
		{
			Log("The model is running with one soil layer\n");

			soil_layer = 0;

			Log("prova sull'array = %g\n", m->cells[cell].available_soil_water);

			//m->cells[cell].soils.layers[soil_layer].available_soil_water_layer = m->cells[cell].available_soil_water;
			//Log("prova sull'array = %g\n", m->cells[cell].soils[soil_layer].available_soil_water_layer);

			//todo move here all algorithms about soil

			//if the soil_model is upgraded and each cell has a own soil parameters
			/*
			 //example for variables related to the soil layers
			 m->cells[cell].soils[soil].value[AVAILABLE_SOIL_WATER];
			 //example for variables not related to the soil layers
			 m_>cells[cell].av_yearly_par_soil;
			 */
		}
		else
		{

			Log("The model is running with more than one soil layer\n");

			for (soil_layer = 0; soil_layer < settings->soil_layer /*soil_layer < m->cells[cell].soils_count*/; soil_layer++)
			{
				Log("Soil layers considered = %d\n", soil_layer);

				//m->cells[cell].soils[soil_layer].available_soil_water_layer = m->cells[cell].available_soil_water;
				//Log("prova sull'array = %g\n", m->cells[cell].soils[soil_layer].available_soil_water_layer);

				//todo move here all algorithms about soil
			}
		}
		Log("***************************************************\n\n");
	}
}
