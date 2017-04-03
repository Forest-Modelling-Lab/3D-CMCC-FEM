/*
 * cell_model_check_balance.c
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

int check_cell_radiation_flux_balance (cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log,"\n*********CHECK CELL RADIATIVE BALANCE************\n");
	/* DAILY CHECK ON RADIATIVE BALANCE */

	/* PAR RADIATIVE BALANCE */

	/* sum of sources */
	in = meteo_daily->incoming_par;

	/* sum of sinks */
	out = c->par_refl + c->par_refl_soil + c->par_refl_snow;

	/* sum of current storage */
	store = c->apar + c->apar_soil + c->apar_snow ;

	balance = in - out -store;

	logger(g_debug_log, "\nCELL RADIATIVE BALANCE (PAR)\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("incoming par = %g molPAR/m2/day\n", meteo_daily->incoming_par);
		printf("\nout\n");
		printf("c->par_refl = %g molPAR/m2/day\n",c->par_refl);
		printf("c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		printf("c->par_refl_snow = %g molPAR/m2/day\n",c->par_refl_snow);
		printf("\nstore\n");
		printf("c->apar = %g molPAR/m2/day\n", c->apar);
		printf("c->par_for_soil = %g molPAR/m2/day\n", c->apar_soil);
		printf("c->par_for_snow = %g molPAR/m2/day\n", c->apar_snow);
		printf("\npar in = %g molPAR/m2/day\n", in);
		printf("par out = %g molPAR/m2/day\n", out);
		printf("par store = %g molPAR/m2/day\n", store);
		printf("par balance = %g molPAR/m2/day\n",balance);
		printf("...FATAL ERROR IN PAR radiative balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok PAR radiative balance\n");
	}

	/*******************************************************************************************************************************************/

	/* SHORT WAVE RADIATION RADIATIVE BALANCE */

	/* sum of sources */
	in = meteo_daily->incoming_sw_downward_W;

	/* sum of sinks */
	out = c->sw_rad_refl + c->sw_rad_refl_soil + c->sw_rad_refl_snow;

	/* sum of current storage */
	store = c->sw_rad_abs + c->sw_rad_abs_soil + c->sw_rad_abs_snow;

	balance = in - out -store;

	logger(g_debug_log, "\nCELL RADIATIVE BALANCE (Short Wave Radiation)\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("incoming radiation = %g W/m2\n", meteo_daily->incoming_sw_downward_W);
		printf("\nout\n");
		printf("c->sw_rad_refl = %g W/m2\n",c->sw_rad_refl);
		printf("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_refl_soil);
		printf("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_refl_snow);
		printf("\nstore\n");
		printf("c->sw_rad_abs = %g W/m2\n", c->sw_rad_abs);
		printf("c->net_sw_rad_for_soil = %g W/m2\n", c->sw_rad_abs_soil);
		printf("\nradiation in = %g W/m2\n", in);
		printf("radiation out = %g W/m2\n", out);
		printf("net radiation store = %g W/m2\n", store);
		printf("radiation balance = %g W/m2\n",balance);
		printf("...FATAL ERROR IN Short Wave Radiation radiative balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok Short Wave Radiation radiative balance\n");
	}

	/*******************************************************************************************************************************************/

	/* PPFD RADIATIVE BALANCE */
	/* sum of sources */
	in = meteo_daily->incoming_ppfd;

	/* sum of sinks */
	out = c->ppfd_refl + c->ppfd_refl_soil + c->ppfd_refl_snow;

	/* sum of current storage */
	store = c->ppfd_abs + c->ppfd_abs_soil + c->ppfd_abs_snow;

	balance = in - out -store;

	logger(g_debug_log, "\nCELL RADIATIVE BALANCE (PPFD)\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("incoming PPFD = %g umol/m2/sec\n", meteo_daily->incoming_ppfd);
		printf("\nout\n");
		printf("c->ppfd_refl = %g umol/m2/sec\n",c->ppfd_refl);
		printf("c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		printf("\nstore\n");
		printf("c->ppfd_abs = %g umol/m2/sec\n", c->ppfd_abs);
		printf("c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_abs_soil);
		printf("\nPPFD in = %g umol/m2/sec\n", in);
		printf("PPFD out = %g umol/m2/sec\n", out);
		printf("PPFD store = %g umol/m2/sec\n", store);
		printf("PPFD balance = %g umol/m2/sec\n",balance);
		printf("...FATAL ERROR IN PPFD radiative balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok PPFD radiative balance\n");
	}

	/*******************************************************************************************************************************************/
	return 1;
}

int check_cell_carbon_flux_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK CELL CARBON BALANCE************\n");

	/* check complete cell level carbon flux balance */

	/* sum of carbon sources */
	in = c->daily_gpp;

	/* sum of carbon sinks */
	out = c->daily_aut_resp;

	/* sum of current carbon storage */
	store = c->daily_npp;

	balance = in - out -store;

	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		printf("\nout\n");
		printf("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_aut_resp);
		printf("\nstore\n");
		printf("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_npp);
		printf("\ncarbon in = %g gC/m2/day\n", in);
		printf("carbon out = %g gC/m2/day\n", out);
		printf("carbon store = %g gC/m2/day\n", store);
		printf("carbon_balance = %g gC/m2/day\n",balance);
		printf("...FATAL ERROR IN carbon balance (exit)\n");
		printf("DOY = %d\n", c->doy);
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}

	/***************************************************************************************************************/

	/* sum of carbon sources */
	in = c->daily_gpp;

	/* sum of carbon sinks */
	out = c->daily_maint_resp + c->daily_growth_resp;

	/* sum of current carbon storage */
	store = c->daily_leaf_carbon + c->daily_stem_carbon +
			c->daily_froot_carbon + c->daily_croot_carbon +
			c->daily_branch_carbon + c->daily_reserve_carbon +
			c->daily_litr_carbon + c->daily_soil_carbon +
			+ c->daily_fruit_carbon;

	balance = in - out - store;

	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		printf("\nout\n");
		printf("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_maint_resp + c->daily_growth_resp);
		printf("\nstore\n");
		printf("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_leaf_carbon);
		printf("c->daily_stem_carbon = %g gC/m2/day\n", c->daily_stem_carbon);
		printf("c->daily_fine_root_carbon = %g gC/m2/day\n", c->daily_froot_carbon);
		printf("c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_croot_carbon);
		printf("c->daily_branch_carbon = %g gC/m2/day\n", c->daily_branch_carbon);
		printf("c->daily_reserve_carbon = %g gC/m2/day\n", c->daily_reserve_carbon);
		printf("c->daily_litter_carbon = %g gC/m2/day\n", c->daily_litr_carbon);
		printf("c->daily_soil_carbon = %g gC/m2/day\n", c->daily_soil_carbon);
		printf("c->daily_fruit_carbon = %g gC/m2/day\n", c->daily_fruit_carbon);
		printf("\ncarbon in = %g gC/m2/day\n", in);
		printf("carbon out = %g gC/m2/day\n", out);
		printf("carbon store = %g gC/m2/day\n", store);
		printf("carbon_balance = %g gC/m2/day\n",balance);
		printf("...FATAL ERROR IN carbon balance (exit)\n");
		printf("DOY = %d\n", c->doy);
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok carbon balance\n");
	}

	return 1;
}

int check_cell_carbon_mass_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;
	static double old_store;

	/* DAILY CHECK ON CELL LEVEL CARBON MASS BALANCE */
	/* check complete cell level carbon mass balance */

	/* sum of sources */
	in = c->daily_gpp_tC;

	/* sum of sinks */
	out = c->daily_aut_resp_tC + c->daily_litr_carbon;

	/* sum of current storage */
	store = c->leaf_tC    +
			c->froot_tC   +
			c->croot_tC   +
			c->stem_tC    +
			c->branch_tC  +
			c->reserve_tC +
			c->fruit_tC   ;

	/* check carbon pool balance */
	balance = in - out - (store - old_store);

	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		printf("DOS = %d\n", c->dos);
		printf("\nin = %g tC/sizecell/day\n", in);
		printf("daily_gpp_tC = %g tC/sizecell\n", c->daily_gpp_tC);
		printf("\nout = %g tC/sizecell/day\n", out);
		printf("daily_aut_resp_tC = %g tC/sizecell/day\n", c->daily_aut_resp_tC);
		printf("daily_litr_carbon = %g tC/sizecell/day\n", c->daily_litr_carbon);
		printf("\nold_store = %g tC/sizecell\n", old_store);
		printf("store = %g tC/sizecell\n", store);
		printf("store - old_tore = %g tC/sizecell\n", store - old_store);
		printf("leaf_tC = %g tC/cell/day\n", c->leaf_tC );
		printf("froot_tC = %g tC/cell/day\n", c->froot_tC );
		printf("croot_tC = %g tC/cell/day\n",c->croot_tC );
		printf("stem_tC = %g tC/cell/day\n",c->stem_tC );
		printf("branch_tC = %g tC/cell/day\n",c->branch_tC );
		printf("reserve_tC = %g tC/cell/day\n",c->reserve_tC );
		printf("fruit_tC = %g tC/cell/day\n",c->fruit_tC );
		printf("\nbalance = %g tC/sizecell\n", balance);
		printf("...FATAL ERROR AT CELL LEVEL carbon mass balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok carbon mass balance at cell level\n");
	}
	/*******************************************************************************************************************/
	return 1;
}

int check_cell_water_flux_balance(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK CELL SOIL WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	in = meteo_daily->rain + c->snow_melt;

	/* sum of sinks */
	out = c->daily_c_transp + c->daily_c_rain_int + c->daily_soil_evapo + c->out_flow;

	/* sum of current storage in soil */
	store = (c->asw - c->old_asw);

	/* check soil pool water balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCELL SOIL POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("meteo_daily->rain = %g\n", meteo_daily->rain);
		printf("c->snow_melt = %g\n", c->snow_melt);
		printf("\nout\n");
		printf("c->daily_c_transp = %g\n", c->daily_c_transp);
		printf("c->daily_c_int = %g\n", c->daily_c_rain_int);
		printf("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		printf("c->out_flow = %g\n", c->out_flow);
		printf("\nstore (as a difference between old and current)\n");
		printf("c->daily_c_water_stored = %g\n", c->daily_c_water_stored);
		printf("delta c->asw = %g\n", (c->asw - c->old_asw));
		printf("c->asw = %g\n", c->asw);
		printf("c->old_asw = %g\n", c->old_asw);
		printf("soil water in = %g\n", in);
		printf("soil water out = %g\n", out);
		printf("soil water store = %g\n", store);
		printf("soil water balance = %g\n", balance);
		printf("...FATAL ERROR IN soil water balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok soil water balance\n");
	}

	logger(g_debug_log, "\n*********CHECK CELL SNOW WATER BALANCE************\n");
	/* DAILY CHECK ON SNOW POOL-SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account snow-soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	in = meteo_daily->snow;

	/* sum of sinks */
	out = c->snow_melt + c->snow_subl;

	/* sum of current storage in snow pack */
	store = c->snow_pack - c->old_snow_pack;

	/* check snow pool water balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCELL SNOW POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("meteo_daily->snow = %g\n", meteo_daily->snow);
		printf("\nout\n");
		printf("c->snow_subl = %g\n", c->snow_subl);
		printf("c->snow_melt = %g\n", c->snow_melt);
		printf("\nstore (as a difference between old and current)\n");
		printf("delta c->asw = %g\n", (c->snow_pack - c->old_snow_pack));
		printf("soil water in = %g\n", in);
		printf("soil water out = %g\n", out);
		printf("soil water store = %g\n", store);
		printf("soil water balance = %g\n", balance);
		printf("...FATAL ERROR IN snow water balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok snow water balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	return 1;
}
