/*
 * wue.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Water_Use_Efficiency (SPECIES *s)
{
	/*WATER USE EFFICIENCY*/
	//fixme it doensn't have sense
	s->value[WUE] = 100 * ( s->value[YEARLY_NPP_tDM] / s->counter[VEG_DAYS]) / (s->value[MONTHLY_EVAPOTRANSPIRATION] / s->counter[VEG_DAYS]);
	//logger(g_log, "Average Water use efficiency = %f\n", m->cells[cell].heights[height].ages[age].species[species].value[WUE]);
}
