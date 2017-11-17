/*
 * litter_carbon_balance.c
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
#include "litter_model.h"

extern logger_t* g_debug_log;

void litter_carbon_balance (cell_t *const c)
{

	logger(g_debug_log, "\n**LITTER CARBON BALANCE**\n");

	//fixme add froot litter

	/* carbon fluxes balance of labile litter pool */
	c->daily_litr1C       = ( c->daily_leaf_to_litr1C + c->daily_froot_to_litr1C)
			- ( c-> daily_litr1_het_resp + c->daily_litr1C_to_soil1C );

	/* carbon mass of labile litter pool */
	c->litr1C            += c->daily_litr1C;

	/* carbon fluxes balance of cellulose litter pool */
	c->daily_litr2C       = ( c->daily_leaf_to_litr2C + c->daily_litr3C_to_litr2C + c->daily_froot_to_litr2C)
			- ( c-> daily_litr2_het_resp + c->daily_litr2C_to_soil2C );

	/* carbon mass of cellulose litter pool */
	c->litr2C            += c->daily_litr2C;

	/* carbon fluxes balance of unshielded cellulose pools */
	c->daily_litr3C       = ( c->daily_leaf_to_litr3C + c->daily_froot_to_litr3C)
			- c->daily_litr3C_to_litr2C;

	/* carbon mass of unshielded cellulose pools */
	c->litr3C            += c->daily_litr3C;

	/* carbon fluxes balance of lignin litter pool */
	c->daily_litr4C       = ( c->daily_leaf_to_litr4C + c->daily_froot_to_litr4C)
			- ( c-> daily_litr4_het_resp + c->daily_litr4C_to_soil3C );

	/* carbon mass of lignin litter pool */
	c->litr4C            += c->daily_litr4C;

	/* total */
	c->daily_to_litrC        = c->daily_litr1C + c->daily_litr2C + c->daily_litr3C + c->daily_litr4C;
	c->litrC                 = c->litr1C + c->litr2C + c->litr3C + c->litr4C;


	/* move from litter to soil pools (this need to be done here to close litter balance) */
	c->daily_soil1C      += c->daily_litr1C_to_soil1C;
	c->daily_soil2C      += c->daily_litr2C_to_soil2C;
	c->daily_soil3C      += c->daily_litr4C_to_soil3C;
	c->daily_soilC       += c->daily_soil1C + c->daily_soil2C + c->daily_soil3C;
	c->soilC             += c->daily_soilC;

}

