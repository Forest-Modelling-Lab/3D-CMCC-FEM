/*
 * renovation.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;


void renovation(cell_t *const c, const int layer, const int height, const int age, const int species)
{

	tree_layer_t *l;
	height_t *h;
	age_t *a;
	species_t *s;

	l = &c->t_layers[layer];
	h = &c->heights[height];
	a = &c->heights[height].ages[age];
	s = &c->heights[height].ages[age].species[species];

	/* compute light availability for seeds of dominant layer */

	if (l->layer_z == 0)
	{
		//logger(g_log, "Average Yearly Par at Soil Level for renovation = %f molPAR/m^2 month\n", m->cells[cell].av_yearly_par_soil);
		//convert molPAR/m^2 month into W/m^2 hour
		//3600 = seconds in a hour
		logger(g_log, "Average Yearly Par at Soil Level for renovation = %f W/m^2 hour\n", c->av_yearly_par_soil);
		c->av_yearly_par_soil = (c->av_yearly_par_soil / ( MOLPAR_MJ * 365 * c->av_yearly_daylength * 3600 )) * W_MJ;
		logger(g_log, "Average Yearly Par at Soil Level for renovation from previous year = %f W/m^2 hour\n", c->av_yearly_par_soil);


		//ERRORE PÈERCHÈ IN QUESTO CASO PRENDEREBBE IL VALORE "LIGHT_TOL" DELLA SPECIE A z == 0
		if (s->value[LIGHT_TOL] <= 2)
		{
			//Shade Tolerant
			logger(g_log, "%s is Shade Tolerant\n", s->name);
			logger(g_log, "Minimum Par at Soil level for Establishment = %f W/m^2 hour\n", s->value[MINPAREST]);
			if (c->av_yearly_par_soil < s->value[MINPAREST])
			{
				logger(g_log, "NO light available for establishment of SHADE TOLLERANT TREES Under Dominant Canopy !!\n");
			}
			else
			{
				logger(g_log, "Light available for establishment of SHADE TOLLERANT TREES Under Dominant Canopy !!\n");
			}
		}
		else
		{
			//Shade Intolerant
			logger(g_log, "%s is Shade Intolerant\n", s->name);
			logger(g_log, "Minimum Par at Soil level for Establishment = %f W/m^2 hour\n", s->value[MINPAREST]);
			if (c->av_yearly_par_soil < s->value[MINPAREST])
			{
				logger(g_log, "NO light available for establishment of SHADE INTOLLERANT TREES Under Dominant Canopy !!\n");
			}
			else
			{
				logger(g_log, "Light available for establishment of SHADE INTOLLERANT TREES Under Dominant Canopy !!\n");
			}
		}
	}
}
