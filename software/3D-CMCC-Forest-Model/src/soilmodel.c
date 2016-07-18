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
#include "matrix.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

//this function should be used only for function not directly related to the forest layers, it just run at the end
//of all physiological function for all forest/crop layers
void soil_model(matrix_t *const m, const yos_t *const yos, const int years, const int month, const int day, const int years_of_simulation)
{
	int cell;

	// check parameters
	assert(m);


	for (cell = 0; cell < m->cells_count; cell++)
	{
		//[soil] represents the soil layer number
		//0 index if for the upper layer
	}
}
