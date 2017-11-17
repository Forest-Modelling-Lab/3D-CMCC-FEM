/*
 * litter_model_check_balance.c
 *
 *  Created on: 03 apr 2017
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "check_balance.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;

int check_litter_carbon_flux_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK LITTER CARBON BALANCE************\n");

	/* check complete litter and cwd level carbon flux balance */

	/* sum of carbon sources */
	in      = c->daily_leaf_to_litrC /* + c->daily_froot_to_litrC */;

	/* sum of carbon sinks */
	out     = c->daily_litr_het_resp + c->daily_litr_to_soilC;

	/* sum of current carbon storage */
	store   = c->daily_litr1C +
			c->daily_litr2C   +
			c->daily_litr3C   +
			c->daily_litr4C   /*+
			c->daily_froot_litr1C  +
			c->daily_froot_litr2C  +
			c->daily_froot_litr3C  +
			c->daily_froot_litr4C  +
			c->daily_stem_cwdC     +
			c->daily_croot_cwdC    +
			c->daily_branch_cwdC   +
			c->daily_reserve_cwdC  +
			c->daily_fruit_cwdC    */;

	balance = in - out - store;

	if ( ( fabs( balance ) > eps ) && ( c->dos > 1) )
	{
		error_log("\nLITTER CARBON FLUX BALANCE\n");

		error_log("DOY                   = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_leaf_to_litrC= %f gC/m2/day\n", c->daily_leaf_to_litrC);
//		error_log("c->daily_cwdC         = %f gC/m2/day\n", c->daily_cwdC);
		error_log("\nout\n");
		error_log("c->daily_litr_to_soilC= %f gC/m2/day\n", c->daily_litr_to_soilC);
		error_log("c->daily_litr_het_resp= %f gC/m2/day\n", c->daily_litr_het_resp);
		error_log("\nstore\n");
		error_log("c->daily_litr1C       = %f gC/m2/day\n", c->daily_litr1C);
		error_log("c->daily_litr2C       = %f gC/m2/day\n", c->daily_litr2C);
		error_log("c->daily_litr3C       = %f gC/m2/day\n", c->daily_litr3C);
		error_log("c->daily_litr4C       = %f gC/m2/day\n", c->daily_litr4C);
//		error_log("c->daily_froot_litr1C = %f gC/m2/day\n", c->daily_froot_litr1C);
//		error_log("c->daily_froot_litr2C = %f gC/m2/day\n", c->daily_froot_litr2C);
//		error_log("c->daily_leaf_litrC   = %f gC/m2/day\n", c->daily_froot_litr3C);
//		error_log("c->daily_froot_litr4C = %f gC/m2/day\n", c->daily_froot_litr4C);
//		error_log("c->daily_stem_cwdC    = %f gC/m2/day\n", c->daily_stem_cwdC);
//		error_log("c->daily_croot_cwdC   = %f gC/m2/day\n", c->daily_croot_cwdC);
//		error_log("c->daily_reserve_cwdC = %f gC/m2/day\n", c->daily_reserve_cwdC);
//		error_log("c->daily_branch_cwdC  = %f gC/m2/day\n", c->daily_branch_cwdC);
//		error_log("c->daily_fruit_cwdC   = %f gC/m2/day\n", c->daily_fruit_cwdC);

		error_log("\nbalance\n");
		error_log("carbon in             = %f gC/m2/day\n", in);
		error_log("carbon out            = %f gC/m2/day\n", out);
		error_log("carbon store          = %f gC/m2/day\n", store);
		error_log("carbon_balance        = %f gC/m2/day\n", balance);
		error_log("...FATAL ERROR in 'Litter_ model_daily' carbon balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Litter_ model_daily' carbon balance\n");
	}
	/* ok */
	return 1;
}

int check_litter_carbon_mass_balance(cell_t *const c)
{
	/* DAILY CHECK ON LITTER LEVEL CARBON MASS BALANCE */
	/* check complete litter level carbon mass balance */

	/* sum of sources */
	c->litr_carbon_in    = c->daily_leaf_to_litrC /* + c->daily_froot_to_litrC */;

	/* sum of sinks */
	c->litr_carbon_out   = c->daily_litr_het_resp + c->daily_litr_to_soilC;

	/* sum of current storage */
	c->litr_carbon_store =
			c->leaf_litr1C +
			c->leaf_litr2C  +
			c->leaf_litr3C  +
			c->leaf_litr4C /* +
			c->froot_litr1C +
			c->froot_litr2C +
			c->froot_litr3C +
			c->froot_litr4C +
			c->cwdC*/;

	/* check carbon pool balance */
	c->litr_carbon_balance = c->litr_carbon_in - c->litr_carbon_out - ( c->litr_carbon_store - c->litr_carbon_old_store );

	/* check for carbon mass balance closure */
	if ( ( fabs( c->litr_carbon_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("\nLITTER LEVEL CARBON MASS BALANCE\n");

		error_log("DOS                = %d\n", c->dos);
		error_log("\nin\n");
		error_log("in                 = %f gC/m2/day\n", c->litr_carbon_in);
		error_log("daily_leaf_to_litrC= %f gC/m2\n",     c->daily_leaf_to_litrC);
//		error_log("daily_cwdC         = %f gC/m2\n",     c->daily_cwdC);
		error_log("\nout\n");
		error_log("out                = %f gC/m2/day\n", c->litr_carbon_out);
		error_log("daily_het_resp     = %f gC/m2/day\n", c->daily_litr_het_resp);
		error_log("daily_soilC        = %f gC/m2/day\n", c->daily_soilC);
		error_log("\nstore\n");
		error_log("leaf_litr1C        = %f gC/m2/day\n", c->leaf_litr1C);
		error_log("leaf_litr2C        = %f gC/m2/day\n", c->leaf_litr2C);
		error_log("leaf_litr3C        = %f gC/m2/day\n", c->leaf_litr3C);
		error_log("leaf_litr4C        = %f gC/m2/day\n", c->leaf_litr4C);
//		error_log("froot_litr1C       = %f gC/m2/day\n", c->froot_litr1C);
//		error_log("froot_litr2C       = %f gC/m2/day\n", c->froot_litr2C);
//		error_log("froot_litr3C       = %f gC/m2/day\n", c->froot_litr3C);
//		error_log("froot_litr4C       = %f gC/m2/day\n", c->froot_litr4C);
//		error_log("cwdC               = %f gC/m2/day\n", c->cwdC);

		error_log("\nbalance\n");
		error_log("carbon in          = %f gC/m2/day\n", c->litr_carbon_in);
		error_log("carbon out         = %f gC/m2/day\n", c->litr_carbon_out);
		error_log("delta carbon store = %f gC/m2/day\n", c->litr_carbon_store - c->litr_carbon_old_store);
		error_log("carbon_balance     = %f gC/m2/day\n", c->litr_carbon_balance);
		error_log("...FATAL ERROR in 'Litter_model_daily' carbon mass balance (gC/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( c->litr_carbon_balance ), > , eps);

		return 0;
	}
	else
	{
		c->litr_carbon_old_store = c->litr_carbon_store;
		logger(g_debug_log, "...ok in 'Litter_model_daily' carbon mass balance (gC/m2/day)\n");
	}
	/* ok */
	return 1;

}

/******************************************************************************************/
/******************************************************************************************/

int check_litter_nitrogen_flux_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK LITTER NITROGEN BALANCE************\n");

	/* check complete litter level nitrogen flux balance */

	/* sum of nitrogen sources */
	in      = c->daily_to_litrN /*+ c->daily_cwdN*//* fixme include Ndepostiion daily */;

	/* sum of nitrogen sinks */
	out     = c->daily_soilN /* fixme include N plants demand */;

	/* sum of current nitrogen storage */
	store   = c->daily_leaf_to_litr1N +
			c->daily_leaf_to_litr2N   +
			c->daily_leaf_to_litr3N   +
			c->daily_leaf_to_litr4N   +
			c->daily_froot_to_litr1N  +
			c->daily_froot_to_litr2N  +
			c->daily_froot_to_litr3N  +
			c->daily_froot_to_litr4N  /*+
			c->daily_stem_cwdN     +
			c->daily_croot_cwdN    +
			c->daily_branch_cwdN   +
			c->daily_reserve_cwdN  +
			c->daily_fruit_cwdN    */;

	balance = in - out - store;

	if ( ( fabs( balance ) > eps ) && ( c->dos > 1) )
	{
		error_log("\nLITTER NITROGEN FLUX BALANCE\n");

		error_log("DOY                   = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_litrC        = %f gC/m2/day\n", c->daily_to_litrC);
//		error_log("c->daily_cwdN         = %f gC/m2/day\n", c->daily_cwdN);
		error_log("\nout\n");
		error_log("c->daily_soilN        = %f gC/m2/day\n", c->daily_soilN);
		error_log("\nstore\n");
		error_log("c->daily_leaf_litr1N  = %f gN/m2/day\n", c->daily_leaf_to_litr1N);
		error_log("c->daily_leaf_litr2N  = %f gN/m2/day\n", c->daily_leaf_to_litr2N);
		error_log("c->daily_leaf_litr3N  = %f gN/m2/day\n", c->daily_leaf_to_litr3N);
		error_log("c->daily_leaf_litr4N  = %f gN/m2/day\n", c->daily_leaf_to_litr4N);
		error_log("c->daily_froot_litr1N = %f gN/m2/day\n", c->daily_froot_to_litr1N);
		error_log("c->daily_froot_litr2N = %f gN/m2/day\n", c->daily_froot_to_litr2N);
		error_log("c->daily_leaf_litrN   = %f gN/m2/day\n", c->daily_froot_to_litr3N);
		error_log("c->daily_froot_litr4N = %f gN/m2/day\n", c->daily_froot_to_litr4N);
//		error_log("c->daily_stem_cwdN    = %f gN/m2/day\n", c->daily_stem_cwdN);
//		error_log("c->daily_croot_cwdN   = %f gN/m2/day\n", c->daily_croot_cwdN);
//		error_log("c->daily_reserve_cwdN = %f gN/m2/day\n", c->daily_reserve_cwdN);
//		error_log("c->daily_branch_cwdN  = %f gN/m2/day\n", c->daily_branch_cwdN);
//		error_log("c->daily_fruit_litrN  = %f gN/m2/day\n", c->daily_fruit_cwdN);
		error_log("\nnitrogen in         = %f gN/m2/day\n", in);
		error_log("nitrogen out          = %f gN/m2/day\n", out);
		error_log("nitrogen store        = %f gN/m2/day\n", store);
		error_log("nitrogen_balance      = %f gN/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Litter_ model_daily' nitrogen balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Litter_ model_daily' nitrogen balance\n");
	}
	/* ok */
	return 1;
}

int check_litter_nitrogen_mass_balance(cell_t *const c)
{
	/* DAILY CHECK ON LITTER LEVEL NITROGEN MASS BALANCE */
	/* check complete litter level nitrogen mass balance */

	/* sum of sources */
	c->litr_nitrogen_in    = c->daily_to_litrN /*+ c->daily_cwdN*//* fixme include Ndepostiion daily */;

	//fixme
	/* sum of sinks */
	c->litr_nitrogen_out   = c->daily_soilN/* fixme include N plants demand */;

	//todo include cwd nitrogen pool
	/* sum of current storage */
	c->litr_nitrogen_store = c->leaf_litr1N +
			c->leaf_litr2N  +
			c->leaf_litr3N  +
			c->leaf_litr4N  +
			c->froot_litr1N +
			c->froot_litr2N +
			c->froot_litr3N +
			c->froot_litr4N /*+
			c->cwdN         */;

	/* check nitrogen pool balance */
	c->litr_nitrogen_balance = c->litr_nitrogen_in - c->litr_nitrogen_out - ( c->litr_nitrogen_store - c->litr_nitrogen_old_store );

	/* check for nitrogen mass balance closure */
	if ( ( fabs( c->litr_nitrogen_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("\nLITTER LEVEL NITROGEN MASS BALANCE\n");

		error_log("DOS                  = %d\n", c->dos);
		error_log("\nin                 = %f gN/m2/day\n", c->litr_nitrogen_in);
		error_log("daily_litrC          = %f gN/m2\n", c->daily_to_litrN);
//		error_log("daily_cwdN           = %f gN/m2\n", c->daily_cwdN);
		error_log("\nout                = %f gN/m2/day\n", c->litr_nitrogen_out);
		error_log("daily_soilN          = %f gN/m2/day\n");
		error_log("\nstore\n");
		error_log("leaf_litr1N          = %f gN/m2/day\n", c->leaf_litr1N);
		error_log("leaf_litr2N          = %f gN/m2/day\n", c->leaf_litr2N);
		error_log("leaf_litr3N          = %f gN/m2/day\n", c->leaf_litr3N);
		error_log("leaf_litr4N          = %f gN/m2/day\n", c->leaf_litr4N);
		error_log("froot_litr1N         = %f gN/m2/day\n", c->froot_litr1N);
		error_log("froot_litr2N         = %f gN/m2/day\n", c->froot_litr2N);
		error_log("froot_litr3N         = %f gN/m2/day\n", c->froot_litr3N);
		error_log("froot_litr4N         = %f gN/m2/day\n", c->froot_litr4N);
//		error_log("cwdN                 = %f gN/m2/day\n", c->cwdN);
		error_log("\nnitrogen in        = %f gN/m2/day\n", c->litr_nitrogen_in);
		error_log("nitrogen out         = %f gN/m2/day\n", c->litr_nitrogen_out);
		error_log("delta nitrogen store = %f gN/m2/day\n", c->litr_nitrogen_store - c->litr_nitrogen_old_store);
		error_log("nitrogen             = %f gN/m2/day\n", c->litr_nitrogen_balance);
		error_log("...FATAL ERROR in 'Litter_model_daily' nitrogen mass balance (gN/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( c->litr_nitrogen_balance ), > , eps);

		return 0;
	}
	else
	{
		c->litr_nitrogen_old_store = c->litr_nitrogen_store;
		logger(g_debug_log, "...ok in 'Litter_model_daily' nitrogen mass balance (gN/m2/day)\n");
	}
	/* ok */
	return 1;
}
