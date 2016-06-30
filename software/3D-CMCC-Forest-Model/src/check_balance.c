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

	double in;
	double out;
	double store;

	logger(g_log, "\n*********CHECK CARBON BALANCE************\n");

	/* DAILY CHECK ON CARBON BALANCE */
	/* note: all biomass increments pools are in gC/m2/day */

	/* sum of sources */
	in = c->daily_gpp;

	/* sum of sinks */
	out = c->daily_maint_resp + c->daily_growth_resp;

	/* sum of current storage */
	store = c->daily_leaf_carbon + c->daily_stem_carbon +
			c->daily_fine_root_carbon + c->daily_coarse_root_carbon +
			c->daily_branch_carbon + c->daily_reserve_carbon +
			c->daily_litter_carbon + c->daily_fruit_carbon;

	c->carbon_balance = in - out -store;

	if (fabs(c->carbon_balance) > 1e-8 )
	{
		logger(g_log, "\nCELL CARBON POOL BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin\n");
		logger(g_log, "c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		logger(g_log, "\nout\n");
		logger(g_log, "c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_maint_resp + c->daily_growth_resp);
		logger(g_log, "\nstore\n");
		logger(g_log, "c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_leaf_carbon);
		logger(g_log, "c->daily_stem_carbon = %g gC/m2/day\n", c->daily_stem_carbon);
		logger(g_log, "c->daily_fine_root_carbon = %g gC/m2/day\n", c->daily_fine_root_carbon);
		logger(g_log, "c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_coarse_root_carbon);
		logger(g_log, "c->daily_branch_carbon = %g gC/m2/day\n", c->daily_branch_carbon);
		logger(g_log, "c->daily_reserve_carbon = %g gC/m2/day\n", c->daily_reserve_carbon);
		logger(g_log, "c->daily_litter_carbon = %g gC/m2/day\n", c->daily_litter_carbon);
		logger(g_log, "c->daily_fruit_carbon = %g gC/m2/day\n", c->daily_fruit_carbon);
		logger(g_log, "\ncarbon in = %g gC/m2/day\n", in);
		logger(g_log, "carbon out = %g gC/m2/day\n", out);
		logger(g_log, "carbon store = %g gC/m2/day\n", store);
		logger(g_log, "carbon_balance = %g gC/m2/day\n",c->carbon_balance);
		logger(g_log, "...FATAL ERROR IN carbon balance (exit)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		exit(1);
	}
	else
	{
		logger(g_log, "...ok carbon balance\n");
	}
	//todo it should also include non only on daily movement of carbon but also the biomass stocks

}

void Check_soil_water_balance(cell_t *const c)
{
	double in;
	double out;
	double store;

	double daily_frac_transp;
	double daily_frac_evapo;
	double daily_frac_soil_evapo;

	logger(g_log, "\n*********CHECK WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	in = c->prcp_rain + c->prcp_snow + c->snow_melt;

	/* sum of sinks */
	//comment: snow_subl is not considered here otherwise it could accounted twice (out and store)
	out = c->daily_c_transp + /*c->daily_c_evapo + */ c->daily_soil_evapo /*+ c->snow_subl*/ + c->out_flow;

	/* sum of current storage in soil */
	//fixme check for daily_c_water_store
	store = (c->asw - c->old_asw) + /*(c->daily_c_water_stored - c->old_daily_c_water_stored)*/ + c->prcp_snow;

	/* check soil pool water balance */
	c->soil_pool_water_balance = in - out - store;

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
	//	canopy_water_pool_store = c->daily_c_water_store - c->old_daily_c_water_store;
	//
	//	/* check canopy pool water balance */
	//	c->canopy_pool_water_balance = canopy_water_pool_in - canopy_water_pool_out - canopy_water_pool_store;
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
	if (fabs(c->soil_pool_water_balance) > 1e-8 )
	{
		logger(g_log, "\nCELL SOIL POOL WATER BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin\n");
		logger(g_log, "c->prcp_rain = %g\n", c->prcp_rain);
		logger(g_log, "c->prcp_snow = %g\n", c->prcp_snow);
		logger(g_log, "\nout\n");
		logger(g_log, "c->daily_tot_c_transp = %g\n", c->daily_c_transp);
		logger(g_log, "c->daily_c_evapo = %g\n", c->daily_c_evapo);
		logger(g_log, "c->soil_evaporation = %g\n", c->daily_soil_evapo);
		logger(g_log, "c->snow_subl = %g\n", c->snow_subl);
		logger(g_log, "c->snow_melt = %g\n", c->snow_melt);
		logger(g_log, "c->out_flow = %g\n", c->out_flow);
		logger(g_log, "\nstore (as a difference between old and current)\n");
		logger(g_log, "c->daily_c_water_stored = %g\n", c->daily_c_water_stored);
		logger(g_log, "delta c->asw = %g\n", (c->asw - c->old_asw));
		logger(g_log, "c->snow_pack = %g\n", c->snow_pack);
		logger(g_log, "c->asw = %g\n", c->asw);
		logger(g_log, "c->old_asw = %g\n", c->old_asw);
		logger(g_log, "soil water in = %g\n", in);
		logger(g_log, "soil water out = %g\n", out);
		logger(g_log, "soil water store = %g\n", store);
		logger(g_log, "soil water balance = %g\n", c->soil_pool_water_balance);
		logger(g_log, "...FATAL ERROR IN soil water balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok soil water balance\n");
	}
}

void Check_class_carbon_balance(cell_t *const c, species_t* const s)
{
	double in;
	double out;
	double store;
	double balance;

	/* DAILY CHECK ON CLASS LEVEL CARBON BALANCE */
	/* check complete tree level carbon balance */

	/* sum of sources */
	in = s->value[DAILY_GPP_gC];

	/* sum of sinks */
	out = s->value[TOTAL_MAINT_RESP] + s->value[TOTAL_GROWTH_RESP] + s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell;

	/* sum of current storage */
	store = s->value[C_TO_LEAF] * 1000000.0 / g_settings->sizeCell
			+ s->value[C_TO_STEM] * 1000000.0 / g_settings->sizeCell
			+ s->value[C_TO_FINEROOT] * 1000000.0 / g_settings->sizeCell
			+ s->value[C_TO_COARSEROOT] * 1000000.0 / g_settings->sizeCell
			+ s->value[C_TO_BRANCH] * 1000000.0 / g_settings->sizeCell
			+ s->value[C_TO_RESERVE] * 1000000.0 / g_settings->sizeCell
			+ s->value[C_TO_FRUIT] * 1000000.0 / g_settings->sizeCell;


	/* check carbon pool balance */
	balance = in - out - store;

	/*******************************************************************************************************************/
	/* check for carbon balance closure */
	if (fabs(balance)> 1e-8)
	{
		logger(g_log, "\nCLASS LEVEL CARBON BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin = %g gC/m2\n", in);
		logger(g_log, "DAILY_GPP_gC = %g gC/m2\n", s->value[DAILY_GPP_gC]);
		logger(g_log, "\nout = %g gC/m2\n", out);
		logger(g_log, "TOTAL_MAINT_RESP = %g gC/m2\n", s->value[TOTAL_MAINT_RESP]);
		logger(g_log, "TOTAL_GROWTH_RESP = %g gC/m2\n", s->value[TOTAL_GROWTH_RESP]);
		logger(g_log, "C_TO_LITTER = %g gC/m2\n", s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell);
		logger(g_log, "\nstore = %g gC/m2\n", store);
		logger(g_log, "C_TO_LEAF = %g gC/m2\n", s->value[C_TO_LEAF]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_FINEROOT = %g gC/m2\n", s->value[C_TO_FINEROOT]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_COARSEROOT = %g gC/m2\n", s->value[C_TO_COARSEROOT]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_STEM = %g gC/m2\n", s->value[C_TO_STEM]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_RESERVE = %g gC/m2\n", s->value[C_TO_RESERVE]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_BRANCH = %g gC/m2\n", s->value[C_TO_BRANCH]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "C_TO_FRUIT = %g gC/m2\n", s->value[C_TO_FRUIT]* 1000000.0 / g_settings->sizeCell);
		logger(g_log, "\nbalance = %g gC/m2\n", balance);
		logger(g_log, "...FATAL ERROR IN CELL LEVEL carbon balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok carbon balance at class level\n");
	}
 	/*******************************************************************************************************************/
}

void Check_class_water_balance(cell_t *const c, species_t* const s)
{
	double in;
	double out;
	double store;
	double balance;


	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE WATER BALANCE */

	/* sum of sources */
	in = s->value[CANOPY_INT];

	/* sum of sinks */
	out = s->value[CANOPY_EVAPO];

	/* sum of current storage */
	store = s->value[CANOPY_WATER] - s->value[OLD_CANOPY_WATER];

	/* check canopy water pool balance */
	balance = in - out - store;

	/*******************************************************************************************************************/
	/* check for canopy water pool water balance closure (during growing season) */
	if (fabs(balance)> 1e-8 && s->counter[VEG_UNVEG] == 1)
	{
		logger(g_log, "\nCLASS LEVEL WATER BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "canopy water in = %g\n", in);
		logger(g_log, "canopy water out = %g\n", out);
		logger(g_log, "canopy water store = %g\n", store);
		logger(g_log, "canopy water balance = %g\n", balance);
		logger(g_log, "...FATAL ERROR IN CELL LEVEL canopy water balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok canopy water balance at class level\n");
	}
}



//void leaf_balance (cell_t *const c, species_t* const s)
//{
//	int first_balance;
//	double in, out, store, balance;
//	//static double old_in_out;
//	static double old_balance;
//
//	if(!c->years_count && c->doy == 1)first_balance = 1;
//	else first_balance = 0;
//
//	in = s->value[C_TO_LEAF];
//
//	out = s->value[C_LEAF_TO_LITTER];
//
//	store = s->value[LEAF_C];
//
//	balance = in - out - store;
//
//	if(!first_balance)
//	{
//		if(fabs(old_balance - balance)>1e-4)
//		{
//			logger(g_log, "\n\nfatal error in leaf balance\n");
//			logger(g_log, "\nin = %g gC/m2\n", in);
//			logger(g_log, "out = %g gC/m2\n", out);
//			//logger(g_log, "old_in_out = %g gC/m2\n", old_in_out);
//			logger(g_log, "store = %g gC/m2\n", store);
//			logger(g_log, "balance = %g gC/m2\n", balance);
//			logger(g_log, "old balance = %g gC/m2\n", old_balance);
//			logger(g_log, "Difference (previous - current) = %g\n",old_balance-balance);
//			exit(1);
//		}
//		else
//		{
//			logger(g_log, "...ok canopy water balance at class level\n");
//		}
//	}
//	else
//	{
//		logger(g_log, "\nin = %g gC/m2\n", in);
//		logger(g_log, "out = %g gC/m2\n", out);
//		//logger(g_log, "old_in_out = %g gC/m2\n", old_in_out);
//		logger(g_log, "store = %g gC/m2\n", store);
//		logger(g_log, "balance = %g gC/m2\n", balance);
//		logger(g_log, "old balance = %g gC/m2\n", old_balance);
//		logger(g_log, "Difference (previous - current) = %g\n",old_balance-balance);
//	}
//	old_balance = balance;
//
//}

//void New_Check_carbon_balance (cell_t *const c, species_t* const s)
//{
//	int first_balance;
//	double in, out, store, balance;
//	static double old_balance;
//
//	if(!c->years_count && c->doy == 1)first_balance = 1;
//	else first_balance = 0;
//
//	//note: it should be called within the c-partitioning allocation routine to avoid a double accounting of c_to_x
//
//	/* sum of sources gC/m2/day */
//	in = s->value[DAILY_GPP_gC];
//
//	/* sum of sinks gC/m2/day */
//	out = s->value[TOTAL_GROWTH_RESP] + s->value[TOTAL_MAINT_RESP] +
//			((s->value[LITTER_C] + s->value[C_TO_LITTER]) * 1000000.0 / g_settings->sizeCell);
//
//	/* sum of current storage tC/area*/
//	store = ((s->value[LEAF_C] + s->value[C_TO_LEAF]) +
//			(s->value[FINE_ROOT_C] + s->value[C_TO_FINEROOT]) +
//			(s->value[COARSE_ROOT_C] + s->value[C_TO_COARSEROOT]) +
//			(s->value[STEM_C] + s->value[C_TO_STEM]) +
//			(s->value[BRANCH_C] + s->value[C_TO_BRANCH]) +
//			(s->value[RESERVE_C] + s->value[C_TO_RESERVE] + s->value[C_LEAF_TO_RESERVE] + s->value[C_FINEROOT_TO_RESERVE]) +
//			(s->value[FRUIT_C] + s->value[C_TO_FRUIT]));
//
//	/* convert current storage in gC/m2/day */
//	store *= 1000000.0 / g_settings->sizeCell;
//
//	/* calculate current balance */
//	balance = in - out - store;
//
//	if(!first_balance)
//	{
//		if(fabs(old_balance - balance) > 1e-8)
//		{
//			logger(g_log, "\nCLASS LEVEL CARBON BALANCE\n");
//			logger(g_log, "DOY = %d\n", c->doy);
//			logger(g_log, "\nin = %g gC/m2\n", in);
//			logger(g_log, "DAILY_GPP_gC = %g gC/m2\n", s->value[DAILY_GPP_gC]);
//			logger(g_log, "\nout = %g gC/m2\n", out);
//			logger(g_log, "TOTAL_MAINT_RESP = %g gC/m2\n", s->value[TOTAL_MAINT_RESP]);
//			logger(g_log, "TOTAL_GROWTH_RESP = %g gC/m2\n", s->value[TOTAL_GROWTH_RESP]);
//			logger(g_log, "C_TO_LITTER = %g gC/m2\n", s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "LITTER_C = %g gC/m2\n", s->value[LITTER_C] * 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "\nstore = %g gC/m2\n", store);
//			logger(g_log, "C_TO_LEAF = %g gC/m2\n", s->value[C_TO_LEAF]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "LEAF_C = %g gC/m2\n", s->value[LEAF_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_TO_FINEROOT = %g gC/m2\n", s->value[C_TO_FINEROOT]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "FINE_ROOT_C = %g gC/m2\n", s->value[FINE_ROOT_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_TO_COARSEROOT = %g gC/m2\n", s->value[C_TO_COARSEROOT]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "COARSE_ROOT_C = %g gC/m2\n", s->value[COARSE_ROOT_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_TO_STEM = %g gC/m2\n", s->value[C_TO_STEM]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "STEM_C = %g gC/m2\n", s->value[STEM_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_TO_RESERVE = %g gC/m2\n", s->value[C_TO_RESERVE]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_LEAF_TO_RESERVE = %g gC/m2\n", s->value[C_LEAF_TO_RESERVE]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_FINEROOT_TO_RESERVE = %g gC/m2\n", s->value[C_FINEROOT_TO_RESERVE]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "RESERVE_C = %g gC/m2\n", s->value[RESERVE_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_TO_BRANCH = %g gC/m2\n", s->value[C_TO_BRANCH]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "BRANCH_C = %g gC/m2\n", s->value[BRANCH_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "C_TO_FRUIT = %g gC/m2\n", s->value[C_TO_FRUIT]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "FRUIT_C = %g gC/m2\n", s->value[FRUIT_C]* 1000000.0 / g_settings->sizeCell);
//			logger(g_log, "\ncarbon balance = %g\n", balance);
//			logger(g_log, "old carbon balance = %g\n", old_balance);
//			logger(g_log, "Difference (previous - current) = %g\n",old_balance-balance);
//			logger(g_log, "...FATAL ERROR IN CLASS LEVEL in carbon balance (exit)\n");
//			exit(1);
//		}
//	}
//
//	/* assign current value to old for next check */
//	old_balance = balance;
//
//}
