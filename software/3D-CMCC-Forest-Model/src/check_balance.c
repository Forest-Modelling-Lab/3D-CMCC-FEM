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

void Check_radiation_balance (cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_log,"\n*********CHECK CELL RADIATIVE BALANCE************\n");
	/* DAILY CHECK ON RADIATIVE BALANCE */

	/* PAR RADIATIVE BALANCE */

	/* sum of sources */
	in = meteo_daily->par;

	/* sum of sinks */
	out = c->par_refl + c->par_refl_soil;

	/* sum of current storage */
	store = c->apar + c->par_for_soil;

	balance = in - out -store;

	//FIXME
	/*
	if (fabs(balance) > 1e-8 )
	{
		logger(g_log, "\nCELL RADIATIVE BALANCE (PAR)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin\n");
		logger(g_log, "incoming par = %g molPAR/m2/day\n", meteo_daily->par);
		logger(g_log, "\nout\n");
		logger(g_log, "c->par_refl = %g molPAR/m2/day\n",c->par_refl);
		logger(g_log, "c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		logger(g_log, "\nstore\n");
		logger(g_log, "c->apar = %g molPAR/m2/day\n", c->apar);
		logger(g_log, "c->par_for_soil = %g molPAR/m2/day\n", c->par_for_soil);
		logger(g_log, "\npar in = %g molPAR/m2/day\n", in);
		logger(g_log, "par out = %g molPAR/m2/day\n", out);
		logger(g_log, "par store = %g molPAR/m2/day\n", store);
		logger(g_log, "par balance = %g molPAR/m2/day\n",balance);
		logger(g_log, "...FATAL ERROR IN PAR radiative balance (exit)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		exit(1);
	}
	else
	{
		logger(g_log, "...ok PAR radiative balance\n");
	}
	*/

	/*******************************************************************************************************************************************/

	/* NET SHORT WAVE RADIATION RADIATIVE BALANCE */

	/* sum of sources */
	in = meteo_daily->sw_downward_W;

	/* sum of sinks */
	out = c->sw_rad_refl + c->sw_rad_for_soil_refl;

	/* sum of current storage */
	store = c->net_sw_rad_abs + c->net_sw_rad_for_soil;

	balance = in - out -store;

	//FIXME
	/*
	if (fabs(balance) > 1e-8 )
	{
		logger(g_log, "\nCELL RADIATIVE BALANCE (Net Radiation)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin\n");
		logger(g_log, "incoming radiation = %g W/m2\n", meteo_daily->sw_downward_W);
		logger(g_log, "\nout\n");
		logger(g_log, "c->sw_rad_refl = %g W/m2\n",c->sw_rad_refl);
		logger(g_log, "c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_for_soil_refl);
		logger(g_log, "\nstore\n");
		logger(g_log, "c->net_sw_rad_abs = %g W/m2\n", c->net_sw_rad_abs);
		logger(g_log, "c->net_sw_rad_for_soil = %g W/m2\n", c->net_sw_rad_for_soil);
		logger(g_log, "radiation in = %g W/m2\n", in);
		logger(g_log, "radiation out = %g W/m2\n", out);
		logger(g_log, "net radiation store = %g W/m2\n", store);
		logger(g_log, "radiation balance = %g W/m2\n",balance);
		logger(g_log, "...FATAL ERROR IN Net Radiation radiative balance (exit)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		exit(1);
	}
	else
	{
		logger(g_log, "...ok Net Radiation radiative balance\n");
	}
	*/
	/*******************************************************************************************************************************************/

	/* PPFD RADIATIVE BALANCE */
	/* sum of sources */
	in = meteo_daily->ppfd;

	/* sum of sinks */
	out = c->ppfd_refl + c->par_refl_soil;

	/* sum of current storage */
	store = c->ppfd_abs + c->ppfd_for_soil;

	balance = in - out -store;

	//FIXME
	/*
	if (fabs(balance) > 1e-8 )
	{
		logger(g_log, "\nCELL RADIATIVE BALANCE (PPFD)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin\n");
		logger(g_log, "incoming PPFD = %g umol/m2/sec\n", meteo_daily->sw_downward_W * RAD2PAR * EPAR);
		logger(g_log, "\nout\n");
		logger(g_log, "c->ppfd_refl = %g umol/m2/sec\n",c->ppfd_refl);
		logger(g_log, "c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		logger(g_log, "\nstore\n");
		logger(g_log, "c->ppfd_abs = %g umol/m2/sec\n", c->ppfd_abs);
		logger(g_log, "c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_for_soil);
		logger(g_log, "PPFD in = %g umol/m2/sec\n", in);
		logger(g_log, "PPFD out = %g umol/m2/sec\n", out);
		logger(g_log, "PPFD store = %g umol/m2/sec\n", store);
		logger(g_log, "PPFD balance = %g umol/m2/sec\n",balance);
		logger(g_log, "...FATAL ERROR IN PPFD radiative balance (exit)\n");
		logger(g_log, "DOY = %d\n", c->doy);
		exit(1);
	}
	else
	{
		logger(g_log, "...ok PPFD radiative balance\n");
	}
	*/
	/*******************************************************************************************************************************************/
}

void Check_carbon_balance(cell_t *const c)
{

	double in;
	double out;
	double store;
	double balance;

	logger(g_log, "\n*********CHECK CELL CARBON BALANCE************\n");

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

	balance = in - out -store;

	if (fabs(balance) > 1e-8 )
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
		logger(g_log, "carbon_balance = %g gC/m2/day\n",balance);
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

void Check_soil_water_balance(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;

	double daily_frac_transp;
	double daily_frac_evapo;
	double daily_frac_soil_evapo;

	logger(g_log, "\n*********CHECK CELL WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	in = meteo_daily->rain + meteo_daily->snow + c->snow_melt;

	/* sum of sinks */
	//comment: snow_subl is not considered here otherwise it could accounted twice (out and store)
	out = c->daily_c_transp + /*c->daily_c_evapo + */ c->daily_soil_evapo /*+ c->snow_subl*/ + c->out_flow;

	/* sum of current storage in soil */
	//fixme check for daily_c_water_store
	//fixme change snow with snowpack - old_snowpack
	store = (c->asw - c->old_asw) + /*(c->daily_c_water_stored - c->old_daily_c_water_stored)*/ + meteo_daily->snow;

	/* check soil pool water balance */
	balance = in - out - store;

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
	if (fabs(balance) > 1e-8 )
	{
		logger(g_log, "\nCELL SOIL POOL WATER BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "\nin\n");
		logger(g_log, "meteo_daily->rain = %g\n", meteo_daily->rain);
		logger(g_log, "meteo_daily->snow = %g\n", meteo_daily->snow);
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
		logger(g_log, "soil water balance = %g\n", balance);
		logger(g_log, "...FATAL ERROR IN soil water balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok soil water balance\n");
	}
}

/******************************************************CLASS LEVEL BALANCE CLOSURE*****************************************************/

void Check_class_radiation_balance(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE RADIATIVE BALANCE */
	//fixme: for all the balance doesn't takes into account the amount of light previously removed in Radiation function

	/* PAR balance */
	/* sum of sources */
	in = s->value[PAR];

	/* sum of sinks */
	out = /*s->value[PAR_REFL] + */ s->value[TRANSM_PAR];

	/* sum of current storage */
	store = s->value[APAR_SUN] + s->value[APAR_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	/* check for PAR balance closure*/
	if (fabs(balance)> 1e-8  && s->counter[VEG_UNVEG] == 1)
	{
		logger(g_log, "\nCLASS LEVEL PAR BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "PAR in = %g\n", in);
		logger(g_log, "PAR out = %g\n", out);
		logger(g_log, "PAR store = %g\n", store);
		logger(g_log, "PAR balance = %g\n", balance);
		logger(g_log, "...FATAL ERROR AT CELL LEVEL PAR balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok PAR balance at class level\n");
	}
	/****************************************************************************************************************/
	/* Net Short-Wave radiation balance */
	/* sum of sources */
	in = /*s->value[SW_RAD_REFL] + */s->value[NET_SW_RAD];

	/* sum of sinks */
	/* it must take into account the overall transmitted NET_RAD (reflected is yet computed for net radiation) */
	out = s->value[NET_SW_RAD_TRANSM];

	/* sum of current storage */
	store = s->value[NET_SW_RAD_ABS_SUN] + s->value[NET_SW_RAD_ABS_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	/* check for NET_RAD balance closure*/
	if (fabs(balance)> 1e-8  && s->counter[VEG_UNVEG] == 1)
	{
		logger(g_log, "\nCLASS LEVEL Net Short Wave BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "NET_SW_RAD in = %g\n", in);
		logger(g_log, "NET_RAD out = %g\n", out);
		logger(g_log, "NET_RAD store = %g\n", store);
		logger(g_log, "NET_RAD balance = %g\n", balance);
		logger(g_log, "...FATAL ERROR AT CELL LEVEL NET_RAD balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok Net Short Wave radiation balance at class level\n");
	}
	/****************************************************************************************************************/
	/* PPFD balance */
	/* sum of sources */
	in = s->value[PPFD];

	/* sum of sinks */
	out = /*s->value[PPFD_REFL] + */ s->value[PPFD_TRANSM];

	/* sum of current storage */
	store = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	/* check for PPFD balance closure*/
	if (fabs(balance)> 1e-8  && s->counter[VEG_UNVEG] == 1)
	{
		logger(g_log, "\nCLASS LEVEL PPFD BALANCE\n");
		logger(g_log, "DOY = %d\n", c->doy);
		logger(g_log, "PPFD in = %g\n", in);
		logger(g_log, "PPFD out = %g\n", out);
		logger(g_log, "PPFD store = %g\n", store);
		logger(g_log, "PPFD balance = %g\n", balance);
		logger(g_log, "...FATAL ERROR AT CELL LEVEL PPFD balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok PPFD balance at class level\n");
	}
}
void Check_class_carbon_balance(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

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
		logger(g_log, "...FATAL ERROR AT CELL LEVEL carbon balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok carbon balance at class level\n");
	}
	/*******************************************************************************************************************/
}

void Check_class_water_balance(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

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
		logger(g_log, "...FATAL ERROR AT CELL LEVEL canopy water balance (exit)\n");
		exit(1);
	}
	else
	{
		logger(g_log, "...ok canopy water balance at class level\n");
	}
}
