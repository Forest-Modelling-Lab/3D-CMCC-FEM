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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("incoming par = %g molPAR/m2/day\n", meteo_daily->incoming_par);
		logger_all("\nout\n");
		logger_all("c->par_refl = %g molPAR/m2/day\n",c->par_refl);
		logger_all("c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		logger_all("c->par_refl_snow = %g molPAR/m2/day\n",c->par_refl_snow);
		logger_all("\nstore\n");
		logger_all("c->apar = %g molPAR/m2/day\n", c->apar);
		logger_all("c->par_for_soil = %g molPAR/m2/day\n", c->apar_soil);
		logger_all("c->par_for_snow = %g molPAR/m2/day\n", c->apar_snow);
		logger_all("\npar in = %g molPAR/m2/day\n", in);
		logger_all("par out = %g molPAR/m2/day\n", out);
		logger_all("par store = %g molPAR/m2/day\n", store);
		logger_all("par balance = %g molPAR/m2/day\n",balance);
		logger_all("...FATAL ERROR IN PAR radiative balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("incoming radiation = %g W/m2\n", meteo_daily->incoming_sw_downward_W);
		logger_all("\nout\n");
		logger_all("c->sw_rad_refl = %g W/m2\n",c->sw_rad_refl);
		logger_all("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_refl_soil);
		logger_all("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_refl_snow);
		logger_all("\nstore\n");
		logger_all("c->sw_rad_abs = %g W/m2\n", c->sw_rad_abs);
		logger_all("c->net_sw_rad_for_soil = %g W/m2\n", c->sw_rad_abs_soil);
		logger_all("\nradiation in = %g W/m2\n", in);
		logger_all("radiation out = %g W/m2\n", out);
		logger_all("net radiation store = %g W/m2\n", store);
		logger_all("radiation balance = %g W/m2\n",balance);
		logger_all("...FATAL ERROR IN Short Wave Radiation radiative balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("incoming PPFD = %g umol/m2/sec\n", meteo_daily->incoming_ppfd);
		logger_all("\nout\n");
		logger_all("c->ppfd_refl = %g umol/m2/sec\n",c->ppfd_refl);
		logger_all("c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		logger_all("\nstore\n");
		logger_all("c->ppfd_abs = %g umol/m2/sec\n", c->ppfd_abs);
		logger_all("c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_abs_soil);
		logger_all("\nPPFD in = %g umol/m2/sec\n", in);
		logger_all("PPFD out = %g umol/m2/sec\n", out);
		logger_all("PPFD store = %g umol/m2/sec\n", store);
		logger_all("PPFD balance = %g umol/m2/sec\n",balance);
		logger_all("...FATAL ERROR IN PPFD radiative balance (exit)\n");

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
	in      = c->daily_gpp;

	/* sum of carbon sinks */
	out     = c->daily_aut_resp;

	/* sum of current carbon storage */
	store   = c->daily_npp;

	balance = in - out -store;

	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		logger_all("\nout\n");
		logger_all("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_aut_resp);
		logger_all("\nstore\n");
		logger_all("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_npp);
		logger_all("\ncarbon in = %g gC/m2/day\n", in);
		logger_all("carbon out = %g gC/m2/day\n", out);
		logger_all("carbon store = %g gC/m2/day\n", store);
		logger_all("carbon_balance = %g gC/m2/day\n",balance);
		logger_all("...FATAL ERROR in 'Cell_model_daily' carbon balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Cell_model_daily' carbon balance\n");
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
			+ c->daily_fruit_carbon + c->daily_cwd_carbon;

	balance = in - out - store;

	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		logger_all("\nout\n");
		logger_all("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_maint_resp + c->daily_growth_resp);
		logger_all("\nstore\n");
		logger_all("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_leaf_carbon);
		logger_all("c->daily_stem_carbon = %g gC/m2/day\n", c->daily_stem_carbon);
		logger_all("c->daily_fine_root_carbon = %g gC/m2/day\n", c->daily_froot_carbon);
		logger_all("c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_croot_carbon);
		logger_all("c->daily_branch_carbon = %g gC/m2/day\n", c->daily_branch_carbon);
		logger_all("c->daily_reserve_carbon = %g gC/m2/day\n", c->daily_reserve_carbon);
		logger_all("c->daily_litter_carbon = %g gC/m2/day\n", c->daily_litr_carbon);
		logger_all("c->daily_soil_carbon = %g gC/m2/day\n", c->daily_soil_carbon);
		logger_all("c->daily_fruit_carbon = %g gC/m2/day\n", c->daily_fruit_carbon);
		logger_all("\ncarbon in = %g gC/m2/day\n", in);
		logger_all("carbon out = %g gC/m2/day\n", out);
		logger_all("carbon store = %g gC/m2/day\n", store);
		logger_all("carbon_balance = %g gC/m2/day\n",balance);
		logger_all("...FATAL ERROR in 'Cell_model_daily' carbon balance (exit)\n");

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
	out = c->daily_aut_resp_tC /*+ c->daily_het_resp_tC*/;

	/* sum of current storage */
	store = c->leaf_tC    +
			c->froot_tC   +
			c->croot_tC   +
			c->stem_tC    +
			c->branch_tC  +
			c->reserve_tC +
			c->fruit_tC   +
			c->litr_carbon+
			c->cwd_carbon +
			c->soil_carbon;

	/* check carbon pool balance */
	balance = in - out - (store - old_store);

	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		logger_all("DOS = %d\n", c->dos);
		logger_all("\nin = %g tC/sizecell/day\n", in);
		logger_all("daily_gpp_tC = %g tC/sizecell\n", c->daily_gpp_tC);
		logger_all("\nout = %g tC/sizecell/day\n", out);
		logger_all("daily_aut_resp_tC = %g tC/sizecell/day\n", c->daily_aut_resp_tC);
		logger_all("daily_litr_carbon = %g tC/sizecell/day\n", c->daily_litr_carbon);
		logger_all("\nold_store = %g tC/sizecell\n", old_store);
		logger_all("store = %g tC/sizecell\n", store);
		logger_all("store - old_tore = %g tC/sizecell\n", store - old_store);
		logger_all("leaf_tC = %g tC/cell/day\n", c->leaf_tC );
		logger_all("froot_tC = %g tC/cell/day\n", c->froot_tC );
		logger_all("croot_tC = %g tC/cell/day\n",c->croot_tC );
		logger_all("stem_tC = %g tC/cell/day\n",c->stem_tC );
		logger_all("branch_tC = %g tC/cell/day\n",c->branch_tC );
		logger_all("reserve_tC = %g tC/cell/day\n",c->reserve_tC );
		logger_all("fruit_tC = %g tC/cell/day\n",c->fruit_tC );
		logger_all("\nbalance = %g tC/sizecell\n", balance);
		logger_all("...FATAL ERROR in 'Cell_model_daily' carbon mass balance (exit)\n");

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance \n");
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
	static double old_store;

	double in_snow;
	double out_snow;
	double store_snow;
	double balance_snow;
	static double old_store_snow;

	logger(g_debug_log, "\n*********CHECK CELL SOIL WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	in      = meteo_daily->prcp;

	/* sum of sinks */
	out     = c->daily_c_transp + c->daily_c_evapo + c->daily_soil_evapo + c->daily_snow_subl + c->daily_out_flow;

	/* sum of current storage in cell pools */
	store   = ( c->asw + c->canopy_water_stored + c->snow_pack );

	/* check soil pool water balance */
	balance = in - out - ( store - old_store );

	logger(g_debug_log, "\nCELL SOIL POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		logger_all("DOY = %d\n", c->dos);
		logger_all("\nin\n");
		logger_all("meteo_daily->prcp = %g\n", meteo_daily->prcp);
		logger_all("\nout\n");
		logger_all("c->daily_c_transp = %g\n", c->daily_c_transp);
		logger_all("c->daily_c_evapo = %g\n", c->daily_c_evapo);
		logger_all("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		logger_all("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		logger_all("c->out_flow = %g\n", c->daily_out_flow);
		logger_all("\nstore (as a difference between old and current)\n");
		logger_all("c->canopy_water_stored = %g\n", c->canopy_water_stored);
		logger_all("c->asw = %g\n", c->asw);
		logger_all("c->snow_pack = %g\n", c->snow_pack);
		logger_all("soil water in = %g\n", in);
		logger_all("soil water out = %g\n", out);
		logger_all("soil water store = %g\n", store);
		logger_all("delta soil water store = %g\n", store - old_store);
		logger_all("soil water balance = %g\n", balance);
		logger_all("...FATAL ERROR IN 'Cell_model_daily' soil water balance (exit)\n");

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok 'Cell_model_daily' soil water balance\n");
	}

	logger(g_debug_log, "\n*********CHECK CELL SNOW WATER BALANCE************\n");
	/* DAILY CHECK ON SNOW POOL-SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account snow-soil-atmosphere fluxes */

	/* sum of sources (snow) */
	in_snow      = meteo_daily->snow;

	/* sum of sinks */
	out_snow     = c->daily_snow_melt + c->daily_snow_subl;

	/* sum of current storage in snow pack */
	store_snow   = c->snow_pack;

	/* check snow pool water balance */
	balance_snow = in_snow - out_snow - ( store_snow - old_store_snow);

	logger(g_debug_log, "\nCELL SNOW POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("meteo_daily->snow = %g\n", meteo_daily->snow);
		logger_all("\nout\n");
		logger_all("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		logger_all("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		logger_all("\nstore (as a difference between old and current)\n");
		logger_all("delta c->asw = %g\n", c->snow_pack);
		logger_all("soil water in = %g\n", in_snow);
		logger_all("soil water out = %g\n", out_snow);
		logger_all("soil water store = %g\n", store_snow);
		logger_all("soil water balance = %g\n", balance_snow);
		logger_all("...FATAL ERROR IN 'Cell_model_daily' snow water balance (exit)\n");

		return 0;
	}
	else
	{
		old_store_snow = store_snow;
		logger(g_debug_log, "...ok 'Cell_model_daily' snow water balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	return 1;
}
