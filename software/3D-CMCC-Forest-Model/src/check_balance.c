/*
 * check_balance.c
 *
 *  Created on: 05/mar/2016
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "check_balance.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void Check_carbon_balance(cell_t *const c)
{

	double carbon_in;
	double carbon_out;
	double carbon_stored;


	logger(g_log, "\n*********CHECK CARBON BALANCE************\n");

	/* DAILY CHECK ON CARBON BALANCE */
	/* note: all biomass increments pools are in gC/m2/day */

	/* sum of sources */
	carbon_in = c->daily_gpp;

	/* sum of sinks */
	carbon_out = c->daily_maint_resp + c->daily_growth_resp;

	/* sum of current storage */
	carbon_stored = c->daily_leaf_carbon + c->daily_stem_carbon +
			c->daily_fine_root_carbon + c->daily_coarse_root_carbon +
			c->daily_branch_carbon + c->daily_reserve_carbon +
			c->daily_litter_carbon;

	c->carbon_balance = carbon_in - carbon_out -carbon_stored;

	if (fabs(c->carbon_balance) > 1e-4 )
	{
		logger(g_log, "\nCELL CARBON POOL BALANCE\n");
		logger(g_log, "\nin\n");
		logger(g_log, "c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		logger(g_log, "\nout\n");
		logger(g_log, "c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_maint_resp + c->daily_growth_resp);
		logger(g_log, "\nstored\n");
		logger(g_log, "c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_leaf_carbon);
		logger(g_log, "c->daily_stem_carbon = %g gC/m2/day\n", c->daily_stem_carbon);
		logger(g_log, "c->daily_fine_root_carbon = %g gC/m2/day\n", c->daily_fine_root_carbon);
		logger(g_log, "c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_coarse_root_carbon);
		logger(g_log, "c->daily_branch_carbon = %g gC/m2/day\n", c->daily_branch_carbon);
		logger(g_log, "c->daily_reserve_carbon = %g gC/m2/day\n", c->daily_reserve_carbon);
		logger(g_log, "c->daily_litter_carbon = %g gC/m2/day\n", c->daily_litter_carbon);
		logger(g_log, "\ncarbon in = %g gC/m2/day\n", carbon_in);
		logger(g_log, "carbon out = %g gC/m2/day\n", carbon_out);
		logger(g_log, "carbon stored = %g gC/m2/day\n", carbon_stored);
		logger(g_log, "carbon_balance = %g gC/m2/day\n",c->carbon_balance);
		logger(g_log, "...FATAL ERROR IN carbon balance (exit)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		exit(1);
	}
	else
	{
		logger(g_log, "...ok carbon balance\n");
	}
}

void Check_soil_water_balance(cell_t *const c)
{
	double soil_water_pool_in;
	double soil_water_pool_out;
	double soil_water_pool_stored;

	double daily_frac_transp;
	double daily_frac_evapo;
	double daily_frac_soil_evapo;

	logger(g_log, "\n*********CHECK WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	soil_water_pool_in = c->prcp_rain + c->prcp_snow + c->snow_melt;

	/* sum of sinks */
	//comment: snow_subl is not considered here otherwise it could accounted twice (out and stored)
	soil_water_pool_out = c->daily_c_transp + /* c->daily_c_evapo + */ c->daily_soil_evapo /*+ c->snow_subl*/ + c->out_flow;

	/* sum of current storage in soil */
	//fixme check for daily_c_water_stored
	soil_water_pool_stored = (c->asw - c->old_asw) /*+ c->daily_c_water_stored */ + c->prcp_snow;

	/* check soil pool water balance */
	c->soil_pool_water_balance = soil_water_pool_in - soil_water_pool_out - soil_water_pool_stored;

	//	/*******************************************************************************************************************/
	//	/* DAILY CHECK ON CANOPY POOL-ATMOSPHERE WATER BALANCE */
	//
	//	/* sum of sources (intercepted rain + snow) */
	//	canopy_water_pool_in = c->daily_c_int;
	//
	//	/* sum of sinks */
	//	canopy_water_pool_out = c->daily_c_evapo;
	//
	//	/* sum of current storage in canopy */
	//	canopy_water_pool_stored = c->daily_c_water_stored - c->old_daily_c_water_stored;
	//
	//	/* check canopy pool water balance */
	//	c->canopy_pool_water_balance = canopy_water_pool_in - canopy_water_pool_out - canopy_water_pool_stored;
	//
	//	/*******************************************************************************************************************/

	/* check fractions */
	if(c->daily_et != 0)
	{
		daily_frac_transp = (100 * c->daily_c_transp)/c->daily_et;
		daily_frac_evapo = (100 * c->daily_c_evapo)/c->daily_et;
		daily_frac_soil_evapo = (100 * c->daily_soil_evapo)/c->daily_et;
		logger(g_log, "daily_frac_transp = %g %%\n", daily_frac_transp);
		logger(g_log, "daily_frac_evapo = %g %%\n", daily_frac_evapo);
		logger(g_log, "daily_frac_soil_evapo = %g %%\n", daily_frac_soil_evapo);
	}

	/* check for soil water pool water balance */
	if (fabs(c->soil_pool_water_balance) > 1e-4 )
	{
		logger(g_log, "\nCELL SOIL POOL WATER BALANCE\n");
		logger(g_log, "\nin\n");
		logger(g_log, "c->prcp_rain = %g\n", c->prcp_rain);
		logger(g_log, "c->prcp_snow = %g\n", c->prcp_snow);
		logger(g_log, "\nout\n");
		logger(g_log, "c->daily_tot_c_transp = %g\n", c->daily_c_transp);
		logger(g_log, "c->soil_evaporation = %g\n", c->daily_soil_evapo);
		logger(g_log, "c->snow_subl = %g\n", c->snow_subl);
		logger(g_log, "c->snow_melt = %g\n", c->snow_melt);
		logger(g_log, "c->out_flow = %g\n", c->out_flow);
		logger(g_log, "\nstored (as a difference between old and current)\n");
		logger(g_log, "delta c->asw = %g\n", (c->asw - c->old_asw));
		logger(g_log, "c->snow_pack = %g\n", c->snow_pack);
		logger(g_log, "c->asw = %g\n", c->asw);
		logger(g_log, "c->old_asw = %g\n", c->old_asw);
		logger(g_log, "soil water in = %g\n", soil_water_pool_in);
		logger(g_log, "soil water out = %g\n", soil_water_pool_out);
		logger(g_log, "soil water stored = %g\n", soil_water_pool_stored);
		logger(g_log, "soil water balance = %g\n", c->soil_pool_water_balance);
		logger(g_log, "...FATAL ERROR IN soil water balance (exit)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		exit(1);
	}
	else
	{
		logger(g_log, "...ok soil water balance\n");
	}
}

void Check_class_carbon_balance(cell_t *const c, species_t* const s)
{
	double leaf_pool_in;
	double leaf_pool_out;
	double leaf_pool_storage;
	double leaf_pool_balance;


	double carbon_pool_in;
	double carbon_pool_out;
	double carbon_pool_stored;
	double carbon_pool_balance;

	/* DAILY CHECK ON CLASS LEVEL CARBON BALANCE */

	/* check leaf balance */

	/* sum of sources */
	leaf_pool_in = s->value[C_TO_LEAF];

	/* sum of sinks */
	leaf_pool_out = s->value[C_TO_LITTER];

	/* sum of current storage */
	leaf_pool_storage = s->value[LEAF_C] - s->value[OLD_LEAF_C];

	/* check leaf pool balance */
	leaf_pool_balance = leaf_pool_in - leaf_pool_out - leaf_pool_storage;

	/* check for leaf balance closure */
	//fixme both function should be the same
	/* deciduous */
	if(s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		/*
		if (fabs(leaf_pool_balance)> 1e-4)
		{
			logger(g_log, "\nCLASS LEVEL LEAF CARBON BALANCE\n");
			logger(g_log, "leaf in = %g \n", leaf_pool_in);
			logger(g_log, "leaf out = %g \n", leaf_pool_out);
			logger(g_log, "leaf stored = %g \n", leaf_pool_storage);
			logger(g_log, "leaf balance = %g \n", leaf_pool_balance);
			logger(g_log, "C_TO_LEAF = %g \n", s->value[C_TO_LEAF]);
			logger(g_log, "C_TO_LITTER = %g \n", s->value[C_TO_LITTER]);
			logger(g_log, "LEAF_C = %g \n", s->value[LEAF_C]);
			logger(g_log, "OLD_LEAF_C = %g \n", s->value[OLD_LEAF_C]);
			logger(g_log, "...FATAL ERROR IN CELL LEVEL leaf balance (exit)\n");
			exit(1);
		}
		else
		{
			logger(g_log, "...ok leaf carbon balance at class level\n");
		}
		*/
	}
	/* evergreen */
	else
	{
		if (fabs(leaf_pool_balance)> 1e-4)
		{
			logger(g_log, "\nCLASS LEVEL LEAF CARBON BALANCE\n");
			logger(g_log, "leaf in = %g \n", leaf_pool_in);
			logger(g_log, "leaf out = %g \n", leaf_pool_out);
			logger(g_log, "leaf stored = %g \n", leaf_pool_storage);
			logger(g_log, "leaf balance = %g \n", leaf_pool_balance);
			logger(g_log, "C_TO_LEAF = %g \n", s->value[C_TO_LEAF]);
			logger(g_log, "C_TO_LITTER = %g \n", s->value[C_TO_LITTER]);
			logger(g_log, "LEAF_C = %g \n", s->value[LEAF_C]);
			logger(g_log, "OLD_LEAF_C = %g \n", s->value[OLD_LEAF_C]);
			logger(g_log, "...FATAL ERROR IN CELL LEVEL leaf balance (exit)\n");
			exit(1);
		}
		else
		{
			logger(g_log, "...ok leaf carbon balance at class level\n");
		}
	}
	/*******************************************************************************************************************/

	/* check complete tree level carbon balance */

	/* sum of sources */
	carbon_pool_in = s->value[DAILY_GPP_gC];

	/* sum of sinks */
	carbon_pool_out = s->value[TOTAL_MAINT_RESP] + s->value[TOTAL_GROWTH_RESP] + s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell;

	/* sum of current storage */
	//fixme both function should be the same
	/* deciduous */
	if(s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		carbon_pool_stored = s->value[C_TO_LEAF] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_STEM] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_FINEROOT] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_COARSEROOT] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_BRANCH] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_RESERVE] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_FRUIT] * 1000000.0 / g_settings->sizeCell;
	}
	/* evergreen */
	else
	{
		carbon_pool_stored = (s->value[LEAF_C] - s->value[OLD_LEAF_C]) * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_STEM] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_FINEROOT] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_COARSEROOT] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_BRANCH] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_RESERVE] * 1000000.0 / g_settings->sizeCell
				+ s->value[C_TO_FRUIT] * 1000000.0 / g_settings->sizeCell;
	}

	/* check carbon pool balance */
	carbon_pool_balance = carbon_pool_in - carbon_pool_out - carbon_pool_stored;

	/*******************************************************************************************************************/
	/* check for carbon balance closure */
	if (fabs(carbon_pool_balance)> 1e-4)
	{
		logger(g_log, "\nCLASS LEVEL CARBON BALANCE\n");
		logger(g_log, "carbon in = %g gC/m2\n", carbon_pool_in);
		logger(g_log, "carbon out = %g gC/m2\n", carbon_pool_out);
		logger(g_log, "carbon stored = %g gC/m2\n", carbon_pool_stored);
		logger(g_log, "carbon balance = %g gC/m2\n", carbon_pool_balance);
		logger(g_log, "C_TO_LEAF = %g gC/m2\n", s->value[C_TO_LEAF]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_FINEROOT = %g gC/m2\n", s->value[C_TO_FINEROOT]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_COARSEROOT = %g gC/m2\n", s->value[C_TO_COARSEROOT]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_STEM = %g gC/m2\n", s->value[C_TO_STEM]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_RESERVE = %g gC/m2\n", s->value[C_TO_RESERVE]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_BRANCH = %g gC/m2\n", s->value[C_TO_BRANCH]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_FRUIT = %g gC/m2\n", s->value[C_TO_FRUIT]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_LITTER = %g gC/m2\n\n", s->value[C_TO_LITTER]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "...FATAL ERROR IN CELL LEVEL carbon balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok carbon balance at class level\n");
	}

	s->value[OLD_LEAF_C] = s->value[LEAF_C];

	/*******************************************************************************************************************/
}

void Check_class_water_balance(species_t* s)
{
	double canopy_water_pool_in;
	double canopy_water_pool_out;
	double canopy_water_pool_stored;
	double canopy_water_pool_balance;


	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE CONTINUUM WATER BALANCE */

	/* sum of sources */
	canopy_water_pool_in = s->value[CANOPY_INT];

	/* sum of sinks */
	canopy_water_pool_out = s->value[CANOPY_EVAPO];

	/* sum of current storage */
	canopy_water_pool_stored = s->value[CANOPY_WATER] - s->value[OLD_CANOPY_WATER];

	/* check canopy water pool balance */
	canopy_water_pool_balance = canopy_water_pool_in - canopy_water_pool_out - canopy_water_pool_stored;

	/*******************************************************************************************************************/
	/* check for canopy water pool water balance closure (during growing season) */
	if (fabs(canopy_water_pool_balance)> 1e-4 && s->counter[VEG_UNVEG] == 1)
	{
		logger(g_log, "\nCLASS LEVEL WATER BALANCE\n");
		logger(g_log, "canopy water in = %g\n", canopy_water_pool_in);
		logger(g_log, "canopy water out = %g\n", canopy_water_pool_out);
		logger(g_log, "canopy water stored = %g\n", canopy_water_pool_stored);
		logger(g_log, "canopy water balance = %g\n", canopy_water_pool_balance);
		logger(g_log, "...FATAL ERROR IN CELL LEVEL canopy water balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok canopy water balance at class level\n");
	}
}

