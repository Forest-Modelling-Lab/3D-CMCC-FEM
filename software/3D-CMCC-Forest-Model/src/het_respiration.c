/*
 * het_respiration.c
 *
 *  Created on: 15 mar 2017
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aut_respiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

//extern settings_t* g_settings;
extern logger_t* g_debug_log;

void heterotrophic_respiration(cell_t *const c)
{

	/* heterotrophic respiration computed as a differences soil_respiration - fine + corase root respiration */
	c->daily_het_resp = c->daily_soil_resp - (c->daily_froot_aut_resp + c->daily_croot_aut_resp);
	logger (g_debug_log, "c->daily_het_resp = %g gC/m^2/day\n", c->daily_het_resp);

	/* monthly */
	c->monthly_het_resp += c->daily_het_resp;

	/* annual */
	c->annual_het_resp += c->daily_het_resp;

}
