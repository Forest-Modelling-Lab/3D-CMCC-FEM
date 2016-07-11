/*
 * canopy_temperature.c
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "light.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"

extern logger_t* g_log;

void canopy_temperature (species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int year)
{

	/* assign canopy temperature the first day if simulation */
	//note: special case  leaf/canopy temperature = daily  average temperature
	//fixme it should be forced to air temperature just the first day of simulation!
	if(s->counter[VEG_DAYS] == 1 && !day && !month)
	{
		s->value[CANOPY_TEMPERATURE] = met[month].d[day].tavg + TempAbs;
		logger(g_log, "Canopy Temperature = %g K\n", s->value[CANOPY_TEMPERATURE]);
	}
	else
	{


	}

}
