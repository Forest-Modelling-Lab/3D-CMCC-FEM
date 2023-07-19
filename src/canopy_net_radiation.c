/*
 * canopy_net_radiation.c
 *
 *  Created on: 12 lug 2016
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"
#include "canopy_radiation_sw_band.h"
#include "canopy_radiation_lw_band.h"
#include "canopy_net_radiation.h"

extern logger_t* g_debug_log;

void canopy_net_radiation(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**NET RADIATION ROUTINE**\n");

	/* compute net radiation */
	s->value[NET_RAD] = s->value[SW_RAD] + s->value[NET_LW_RAD];

	logger(g_debug_log, "Net SW Radiation for canopy = %g W/m2\n", s->value[SW_RAD]);
	logger(g_debug_log, "Net LW Radiation for canopy = %g W/m2\n", s->value[NET_LW_RAD]);
	logger(g_debug_log, "Net Radiation for canopy    = %g W/m2\n", s->value[NET_RAD]);
}
