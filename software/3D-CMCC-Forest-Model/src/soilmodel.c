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
void soil_model(MATRIX *const m, const YOS *const yos, const int years, const int month, const int day, const int years_of_simulation)
{
	int cell;
	int soil;

	// check parameters
	assert(m);


	for (cell = 0; cell < m->cells_count; cell++)
	{
		//[soil] represents the soil layer number
		//0 index if for the upper layer

		for (soil = 0; soil < m->cells[cell].soils_count; soil++)
		{
			m->cells[cell].soils[soil].variabile = 5;

			//prova
			if (soil == 0)
			{
				m->cells[cell].soils[soil].variabile = 5;
			}
			else
			{
				m->cells[cell].soils[soil].variabile = m->cells[cell].soils[soil-1].variabile + 1;
			}

			Log("soil layer = %d\n", soil);
		}
	}
}
