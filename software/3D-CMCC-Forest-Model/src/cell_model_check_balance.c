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

/* note: when model checks fluxes there's no need to use global variables */
/* note: when model checks for stocks and it uses at least one stock for balance or it
 *  simulates more than one cell it needs to use global variables */

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
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("incoming par = %g molPAR/m2/day\n", meteo_daily->incoming_par);
		error_log("\nout\n");
		error_log("c->par_refl = %g molPAR/m2/day\n",c->par_refl);
		error_log("c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		error_log("c->par_refl_snow = %g molPAR/m2/day\n",c->par_refl_snow);
		error_log("\nstore\n");
		error_log("c->apar = %g molPAR/m2/day\n", c->apar);
		error_log("c->par_for_soil = %g molPAR/m2/day\n", c->apar_soil);
		error_log("c->par_for_snow = %g molPAR/m2/day\n", c->apar_snow);
		error_log("\npar in = %g molPAR/m2/day\n", in);
		error_log("par out = %g molPAR/m2/day\n", out);
		error_log("par store = %g molPAR/m2/day\n", store);
		error_log("par balance = %g molPAR/m2/day\n",balance);
		error_log("...FATAL ERROR IN PAR radiative balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

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
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("incoming radiation = %g W/m2\n", meteo_daily->incoming_sw_downward_W);
		error_log("\nout\n");
		error_log("c->sw_rad_refl = %g W/m2\n",c->sw_rad_refl);
		error_log("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_refl_soil);
		error_log("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_refl_snow);
		error_log("\nstore\n");
		error_log("c->sw_rad_abs = %g W/m2\n", c->sw_rad_abs);
		error_log("c->net_sw_rad_for_soil = %g W/m2\n", c->sw_rad_abs_soil);
		error_log("\nradiation in = %g W/m2\n", in);
		error_log("radiation out = %g W/m2\n", out);
		error_log("net radiation store = %g W/m2\n", store);
		error_log("radiation balance = %g W/m2\n",balance);
		error_log("...FATAL ERROR IN Short Wave Radiation radiative balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

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
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("incoming PPFD = %g umol/m2/sec\n", meteo_daily->incoming_ppfd);
		error_log("\nout\n");
		error_log("c->ppfd_refl = %g umol/m2/sec\n",c->ppfd_refl);
		error_log("c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		error_log("\nstore\n");
		error_log("c->ppfd_abs = %g umol/m2/sec\n", c->ppfd_abs);
		error_log("c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_abs_soil);
		error_log("\nPPFD in = %g umol/m2/sec\n", in);
		error_log("PPFD out = %g umol/m2/sec\n", out);
		error_log("PPFD store = %g umol/m2/sec\n", store);
		error_log("PPFD balance = %g umol/m2/sec\n",balance);
		error_log("...FATAL ERROR IN PPFD radiative balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok PPFD radiative balance\n");
	}
	/* ok */
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

	balance = in - out - store;

	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		error_log("\nout\n");
		error_log("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_aut_resp);
		error_log("\nstore\n");
		error_log("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_npp);
		error_log("\ncarbon in = %g gC/m2/day\n", in);
		error_log("carbon out = %g gC/m2/day\n", out);
		error_log("carbon store = %g gC/m2/day\n", store);
		error_log("carbon_balance = %g gC/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' carbon flux balance (first) (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

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
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
		error_log("\nout\n");
		error_log("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_maint_resp + c->daily_growth_resp);
		error_log("\nstore\n");
		error_log("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_leaf_carbon);
		error_log("c->daily_stem_carbon = %g gC/m2/day\n", c->daily_stem_carbon);
		error_log("c->daily_fine_root_carbon = %g gC/m2/day\n", c->daily_froot_carbon);
		error_log("c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_croot_carbon);
		error_log("c->daily_branch_carbon = %g gC/m2/day\n", c->daily_branch_carbon);
		error_log("c->daily_reserve_carbon = %g gC/m2/day\n", c->daily_reserve_carbon);
		error_log("c->daily_litter_carbon = %g gC/m2/day\n", c->daily_litr_carbon);
		error_log("c->daily_soil_carbon = %g gC/m2/day\n", c->daily_soil_carbon);
		error_log("c->daily_fruit_carbon = %g gC/m2/day\n", c->daily_fruit_carbon);
		error_log("\ncarbon in = %g gC/m2/day\n", in);
		error_log("carbon out = %g gC/m2/day\n", out);
		error_log("carbon store = %g gC/m2/day\n", store);
		error_log("carbon_balance = %g gC/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' carbon flux balance (second) (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok carbon balance\n");
	}
	/* ok */
	return 1;
}

int check_cell_carbon_mass_balance(cell_t *const c)
{

	/* DAILY CHECK ON CELL LEVEL CARBON MASS BALANCE */
	/* check complete cell level carbon mass balance */

	/* sum of sources */
	c->carbon_in    = c->daily_gpp;

	/* sum of sinks */
	c->carbon_out   = c->daily_growth_resp + c->daily_maint_resp /* + c->daily_het_resp */;

	/* sum of current storage */
	c->carbon_store = c->leaf_carbon +
			c->froot_carbon   +
			c->croot_carbon   +
			c->stem_carbon    +
			c->branch_carbon  +
			c->reserve_carbon +
			c->fruit_carbon   +
			c->litr_carbon    +
			c->cwd_carbon     +
			c->soil_carbon    ;

	/* check carbon pool balance */
	c->carbon_balance = c->carbon_in - c->carbon_out - ( c->carbon_store - c->carbon_old_store );

	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( c->carbon_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin = %g gC/m2/day\n", c->carbon_in);
		error_log("daily_gpp_carbon = %g gC/m2\n", c->daily_gpp);
		error_log("\nout = %g gC/m2/day\n", c->carbon_out);
		error_log("daily_aut_resp = %g gC/m2/day\n", c->daily_aut_resp);
		error_log("\nstore\n");
		error_log("leaf_carbon = %g gC/m2/day\n", c->leaf_carbon);
		error_log("froot_carbon = %g gC/m2/day\n", c->froot_carbon);
		error_log("croot_carbon = %g gC/m2/day\n", c->croot_carbon);
		error_log("stem_carbon = %g gC/m2/day\n", c->stem_carbon);
		error_log("branch_carbon = %g gC/m2/day\n", c->branch_carbon);
		error_log("reserve_carbon = %g gC/m2/day\n", c->reserve_carbon);
		error_log("fruit_carbon = %g gC/m2/day\n",c->fruit_carbon);
		error_log("litr_carbon = %g gC/m2/day\n", c->litr_carbon);
		error_log("cwd_carbon = %g gC/m2/day\n", c->cwd_carbon);
		error_log("soil_carbon = %g gC/m2/day\n", c->soil_carbon);
		error_log("\ncarbon in = %g gC/m2/day\n", c->carbon_in);
		error_log("carbon out = %g gC/m2/day\n", c->carbon_out);
		error_log("delta carbon store = %g gC/m2/day\n", c->carbon_store - c->carbon_old_store);
		error_log("carbon_balance = %g gC/m2/day\n",c->carbon_balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' carbon mass balance (gC/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( c->carbon_balance ), > , eps);

		return 0;
	}
	else
	{
		c->carbon_old_store = c->carbon_store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance (gC/m2/day)\n");
	}
	/*******************************************************************************************************************/
	/* DAILY CHECK ON CELL LEVEL CARBON MASS BALANCE */
	/* check complete cell level carbon mass balance */

	/* sum of sources */
	c->carbontc_in    = c->daily_gpp_tC;

	/* sum of sinks */
	c->carbontc_out   = c->daily_aut_resp_tC /*+ c->daily_het_resp_tC*/;

	/* sum of current storage */
	c->carbontc_store = c->leaf_tC +
			c->froot_tC   +
			c->croot_tC   +
			c->stem_tC    +
			c->branch_tC  +
			c->reserve_tC +
			c->fruit_tC   +
			c->litr_tC    +
			c->cwd_tC     +
			c->soil_tC    ;

	/* check carbon pool balance */
	c->carbontc_balance = c->carbontc_in - c->carbontc_out - ( c->carbontc_store - c->carbontc_old_store );

	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( c->carbontc_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin = %g tC/sizecell/day\n", c->carbontc_in);
		error_log("daily_gpp_carbon = %g tC/sizecell\n", c->daily_gpp);
		error_log("\nout = %g tC/sizecell/day\n", c->carbontc_out);
		error_log("daily_aut_resp = %g tC/sizecell/day\n", c->daily_aut_resp);
		error_log("\nstore\n");
		error_log("leaf_tC = %g tC/sizecell/day\n", c->leaf_tC);
		error_log("froot_tC = %g tC/sizecell/day\n", c->froot_tC);
		error_log("croot_tC = %g tC/sizecell/day\n", c->croot_tC);
		error_log("stem_tC = %g tC/sizecell/day\n", c->stem_tC);
		error_log("branch_tC = %g tC/sizecell/day\n", c->branch_tC);
		error_log("reserve_tC = %g tC/sizecell/day\n", c->reserve_tC);
		error_log("fruit_tC = %g tC/sizecell/day\n",c->fruit_tC);
		error_log("litr_tC = %g tC/sizecell/day\n", c->litr_tC);
		error_log("cwd_tC = %g tC/sizecell/day\n", c->cwd_tC);
		error_log("soil_tC = %g tC/sizecell/day\n", c->soil_tC);
		error_log("\ncarbon in = %g tC/sizecell/day\n", c->carbontc_in);
		error_log("carbon out = %g tC/sizecell/day\n", c->carbontc_out);
		error_log("delta carbon store = %g tC/sizecell/day\n", c->carbontc_store - c->carbontc_old_store);
		error_log("carbon_balance = %g tC/sizecell/day\n",c->carbontc_balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' carbon mass balance (tC/sizecell/day)(exit)\n");
		CHECK_CONDITION(fabs( c->carbontc_balance ), > , eps);

		return 0;
	}
	else
	{
		c->carbontc_old_store = c->carbontc_store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance(tC/sizecell/day) \n");
	}
	/* ok */
	return 1;
}

int check_cell_water_flux_balance(cell_t *const c, const meteo_daily_t *const meteo_daily)
{

	logger(g_debug_log, "\n*********CHECK CELL SOIL WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	c->water_in      = meteo_daily->prcp;

	/* sum of sinks */
	c->water_out     = c->daily_c_transp + c->daily_c_evapo + c->daily_soil_evapo + c->daily_snow_subl + c->daily_out_flow;

	/* sum of current storage in cell pools */
	c->water_store   = ( c->asw + c->canopy_water_stored + c->snow_pack );

	/* check soil pool water balance */
	c->water_balance = c->water_in - c->water_out - ( c->water_store - c->water_old_store );

	logger(g_debug_log, "\nCELL SOIL POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( c->water_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOY = %d\n", c->dos);
		error_log("\nin\n");
		error_log("meteo_daily->prcp = %g\n", meteo_daily->prcp);
		error_log("\nout\n");
		error_log("c->daily_c_transp = %g\n", c->daily_c_transp);
		error_log("c->daily_c_evapo = %g\n", c->daily_c_evapo);
		error_log("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		error_log("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		error_log("c->out_flow = %g\n", c->daily_out_flow);
		error_log("\nstore (as a difference between old and current)\n");
		error_log("c->canopy_water_stored = %g\n", c->canopy_water_stored);
		error_log("c->asw = %g\n", c->asw);
		error_log("c->snow_pack = %g\n", c->snow_pack);
		error_log("soil water in = %g\n", c->water_in);
		error_log("soil water out = %g\n", c->water_out  );
		error_log("soil water store = %g\n", c->water_store);
		error_log("delta soil water store = %g\n", c->water_store - c->water_old_store);
		error_log("soil water balance = %g\n", c->water_balance);
		error_log("...FATAL ERROR IN 'Cell_model_daily' soil water balance (exit)\n");
		CHECK_CONDITION(fabs( c->water_balance ), > , eps);

		return 0;
	}
	else
	{
		c->water_old_store = c->water_store;
		logger(g_debug_log, "...ok 'Cell_model_daily' soil water balance\n");
	}

	logger(g_debug_log, "\n*********CHECK CELL SNOW WATER BALANCE************\n");
	/* DAILY CHECK ON SNOW POOL-SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account snow-soil-atmosphere fluxes */

	/* sum of sources (snow) */
	c->snow_in      = meteo_daily->snow;

	/* sum of sinks */
	c->snow_out     = c->daily_snow_melt + c->daily_snow_subl;

	/* sum of current storage in snow pack */
	c->snow_store   = c->snow_pack;

	/* check snow pool water balance */
	c->snow_balance = c->snow_in - c->snow_out - ( c->snow_store - c->snow_old_store);

	logger(g_debug_log, "\nCELL SNOW POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( c->snow_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("meteo_daily->snow = %g\n", meteo_daily->snow);
		error_log("\nout\n");
		error_log("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		error_log("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		error_log("\nstore (as a difference between old and current)\n");
		error_log("delta c->asw = %g\n", c->snow_pack);
		error_log("soil water in = %g\n", c->snow_in);
		error_log("soil water out = %g\n", c->snow_out);
		error_log("soil water store = %g\n", c->snow_store);
		error_log("soil water balance = %g\n", c->snow_balance);
		error_log("...FATAL ERROR IN 'Cell_model_daily' snow water balance (exit)\n");
		CHECK_CONDITION(fabs( c->snow_balance ), > , eps);

		return 0;
	}
	else
	{
		c->snow_old_store = c->snow_store;
		logger(g_debug_log, "...ok 'Cell_model_daily' snow water balance\n");
	}
	/* ok */
	return 1;
}
