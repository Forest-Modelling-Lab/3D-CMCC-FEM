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
//of all physiological function for all forest/crop layers

//todo add day to arguments
void soil_model_daily(MATRIX *const m)
{
	int cell;
	int soil;

	// check parameters
	assert(m);


	for (cell = 0; cell < m->cells_count; cell++)
	{


		Log("--cells count = %d \n", m->cells_count);
		Log("--soils count = %d \n", m->cells[cell].soils_count);
		Log("x = %d\n",m->cells[cell].x);


		//FIXME bug IN SOILMODEL
		//[soil] represent the soil layer number
		//0 index if for the upper layer

		for (soil = 0; soil < m->cells[cell].soils_count; soil++)
		{
			Log("cazzo %d!!!!!\n", soil);
			m->cells[cell].soils[soil].variabile = 5;

			if (soil == 0)
			{
				m->cells[cell].soils[soil].variabile = 5;
			}
			else
			{
				m->cells[cell].soils[soil].variabile = m->cells[cell].soils[soil-1].variabile + 1;
			}

			Log("soil layer = %d\n", soil);
			Log("soil layer = %d , variabile = %g\n", soil, m->cells[cell].soils[soil].variabile);


		}






	}




		/*

		if (settings->soil_layer == 1 || m->cells[cell].soils_count == 1)
		{
			Log("The model is running with one soil layer\n");

			soil_layer = 0;

			Log("prova sull'array = %g\n", m->cells[cell].available_soil_water);

			//m->cells[cell].soils.layers[soil_layer].available_soil_water_layer = m->cells[cell].available_soil_water;
			//Log("prova sull'array = %g\n", m->cells[cell].soils[soil_layer].available_soil_water_layer);

			//todo move here all algorithms about soil

			//if the soil_model is upgraded and each cell has a own soil parameters
			 //example for variables related to the soil layers
			 //m->cells[cell].soils[soil].value[AVAILABLE_SOIL_WATER];
			 //example for variables not related to the soil layers
			 //m_>cells[cell].av_yearly_par_soil;

		}
		else
		{

			Log("The model is running with more than one soil layer\n");

			for (soil_layer = 0; soil_layer < settings->soil_layer soil_layer < m->cells[cell].soils_count; soil_layer++)
			{
				Log("Soil layers considered = %d\n", soil_layer);

				//m->cells[cell].soils[soil_layer].available_soil_water_layer = m->cells[cell].available_soil_water;
				//Log("prova sull'array = %g\n", m->cells[cell].soils[soil_layer].available_soil_water_layer);

				//todo move here all algorithms about soil
			}
		}

		Log("***************************************************\n\n");
	}*/
}
