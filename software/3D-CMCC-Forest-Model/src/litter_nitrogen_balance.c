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
	/*********************************************************************************************************/

	/* update mass of deadwood */
	c->deadwood_N              -= (c->daily_deadwood_to_litr2N + c->daily_deadwood_to_litr3N + c->daily_deadwood_to_litr4N);

	/*********************************************************************************************************/

	/* nitrogen fluxes balance of labile litter pool */
	c->daily_to_litr1N          = ( c->daily_leaf_to_litr1N + c->daily_froot_to_litr1N )
			- ( c->daily_litr1N_to_soil1N );

	/* nitrogen mass of labile litter pool */
	c->litr1N                  += c->daily_to_litr1N;
	/*********************************************************************************************************/

	/* nitrogen fluxes balance of cellulose litter pool */
	c->daily_to_litr2N          = ( c->daily_leaf_to_litr2N + c->daily_litr3N_to_litr2N + c->daily_deadwood_to_litr2N + c->daily_froot_to_litr2N)
			- ( c->daily_litr2N_to_soil2N );

	/* nitrogen mass of cellulose litter pool */
	c->litr2N                  += c->daily_to_litr2N;
	/*********************************************************************************************************/

	/* nitrogen fluxes balance of unshielded cellulose pools */
	c->daily_to_litr3N          = ( c->daily_leaf_to_litr3N + c->daily_froot_to_litr3N + c->daily_deadwood_to_litr3N )
			- c->daily_litr3N_to_litr2N;

	/* nitrogen mass of unshielded cellulose pools */
	c->litr3N                  += c->daily_to_litr3N;
	/*********************************************************************************************************/

	/* nitrogen fluxes balance of lignin litter pool */
	c->daily_to_litr4N          = ( c->daily_leaf_to_litr4N + c->daily_froot_to_litr4N + c->daily_deadwood_to_litr4N )
			- ( c->daily_litr4N_to_soil3N );

	/* nitrogen mass of lignin litter pool */
	c->litr4N                  += c->daily_to_litr4N;
	/*********************************************************************************************************/

	/* total fluxes  */
	c->daily_leaf_to_litrN      = c->daily_leaf_to_litr1N  + c->daily_leaf_to_litr2N     + c->daily_leaf_to_litr3N     + c->daily_leaf_to_litr4N;
	c->daily_froot_to_litrN     = c->daily_froot_to_litr1N + c->daily_froot_to_litr2N    + c->daily_froot_to_litr3N    + c->daily_froot_to_litr4N;
	c->daily_deadwood_to_litrN  =                            c->daily_deadwood_to_litr2N + c->daily_deadwood_to_litr3N + c->daily_deadwood_to_litr4N;
	c->daily_to_litrN           = c->daily_to_litr1N       + c->daily_to_litr2N          + c->daily_to_litr3N          + c->daily_to_litr4N;

	/* total mass */
	c->litrN                    = c->litr1N + c->litr2N + c->litr3N + c->litr4N;

	/* move from litter to soil pools (this need to be done here to close litter balance) */
	c->daily_to_soil1N          = c->daily_litr1N_to_soil1N;
	c->daily_to_soil2N          = c->daily_litr2N_to_soil2N;
	c->daily_to_soil3N          = c->daily_litr4N_to_soil3N;
	c->daily_to_soilN           = c->daily_to_soil1N + c->daily_to_soil2N + c->daily_to_soil3N;
	c->soilN                    = c->daily_to_soilN;
}
