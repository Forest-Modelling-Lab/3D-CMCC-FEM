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
#include "types.h"


extern void Get_renovation (CELL *c, HEIGHT *h, SPECIES *s)
{

	//compute light availability for seeds of dominant layer

	if (h->z == 0)
	{
		//Log("Average Yearly Par at Soil Level for renovation = %g molPAR/m^2 month\n", m->cells[cell].av_yearly_par_soil);
		//convert molPAR/m^2 month into W/m^2 hour
		//3600 = seconds in a hour
		Log("Average Yearly Par at Soil Level for renovation = %g W/m^2 hour\n", c->av_yearly_par_soil);
		c->av_yearly_par_soil = (c->av_yearly_par_soil / ( MOLPAR_MJ * 365 * c->av_yearly_daylength * 3600 )) * W_MJ;
		Log("Average Yearly Par at Soil Level for renovation from previous year = %g W/m^2 hour\n", c->av_yearly_par_soil);


		//ERRORE PÈERCHÈ IN QUESTO CASO PRENDEREBBE IL VALORE "LIGHT_TOL" DELLA SPECIE A z == 0
		if (s->value[LIGHT_TOL] <= 2)
		{
			//Shade Tolerant
			Log("%s is Shade Tolerant\n", s->name);
			Log("Minimum Par at Soil level for Establishment = %g W/m^2 hour\n", s->value[MINPAREST]);
			if (c->av_yearly_par_soil < s->value[MINPAREST])
			{
				Log("NO light available for establishment of SHADE TOLLERANT TREES Under Dominant Canopy !!\n");
			}
			else
			{
				Log("Light available for establishment of SHADE TOLLERANT TREES Under Dominant Canopy !!\n");
			}
		}
		else
		{
			//Shade Intolerant
			Log("%s is Shade Intolerant\n", s->name);
			Log("Minimum Par at Soil level for Establishment = %g W/m^2 hour\n", s->value[MINPAREST]);
			if (c->av_yearly_par_soil < s->value[MINPAREST])
			{
				Log("NO light available for establishment of SHADE INTOLLERANT TREES Under Dominant Canopy !!\n");
			}
			else
			{
				Log("Light available for establishment of SHADE INTOLLERANT TREES Under Dominant Canopy !!\n");
			}
		}
	}
}
