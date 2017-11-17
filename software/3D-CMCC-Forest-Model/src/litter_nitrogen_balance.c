/*
 * litter_nitrogen_balance.c
 *
 *  Created on: 17 nov 2017
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_model.h"

extern logger_t* g_debug_log;

void litter_nitrogen_balance (cell_t *const c)
{

	logger(g_debug_log, "\n**LITTER NITROGEN BALANCE**\n");


	/* nitrogen fluxes out of labile litter pool */
	c->litr1N -= c->daily_litr1N_to_soil1N;
	/* nitrogen fluxes in of labile litter pool */
	c->litr1N += c->daily_litr1N;

	/* nitrogen fluxes out of cellulose litter pool */
	c->litr2N -= c->daily_litr2N_to_soil2N;
	/* nitrogen fluxes in of cellulose litter pool */
	c->litr2N += c->daily_litr2N;

	/* nitrogen fluxes out from shielded to unshielded cellulose pools */
	c->litr3N -= c->daily_litr3N_to_litr2N;
	/* nitrogen fluxes in from shielded to unshielded cellulose pools */
	c->litr3N += c->daily_litr3N;

	/* nitrogen fluxes out of lignin litter pool */
	c->litr4N -= c->daily_litr4N_to_soil3N;
	/* nitrogen fluxes in of lignin litter pool */
	c->litr4N += c->daily_litr4N;

	/* total */
	c->litrN  = c->litr1N + c->litr2N + c->litr3N + c->litr4N;
}
