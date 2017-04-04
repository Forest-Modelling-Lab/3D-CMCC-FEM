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
	double in;
	double out;
	double store;
	double balance;
	static double old_store;
	static double old_leaf_carbon;
	static double old_froot_carbon;
	static double old_croot_carbon;
	static double old_stem_carbon;
	static double old_branch_carbon;
	static double old_reserve_carbon;
	static double old_fruit_carbon;
	static double old_litr_carbon;
	static double old_cwd_carbon;
	static double old_soil_carbon;

	double in_tC;
	double out_tC;
	double store_tC;
	double balance_tC;
	static double old_store_tC;
	static double old_leaf_tC;
	static double old_froot_tC;
	static double old_croot_tC;
	static double old_stem_tC;
	static double old_branch_tC;
	static double old_reserve_tC;
	static double old_fruit_tC;
	static double old_litr_tC;
	static double old_cwd_tC;
	static double old_soil_tC;

	/* DAILY CHECK ON CELL LEVEL CARBON MASS BALANCE */
	/* check complete cell level carbon mass balance */

	/* sum of sources */
	in = c->daily_gpp;

	/* sum of sinks */
	out = c->daily_growth_resp + c->daily_maint_resp /* + c->daily_het_resp */;

	/* sum of current storage */
	store = c->leaf_carbon    +
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
	balance = in - out - ( store - old_store );

	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin = %g gC/m2/day\n", in);
		error_log("daily_gpp_carbon = %g gC/m2\n", c->daily_gpp);
		error_log("\nout = %g gC/m2/day\n", out);
		error_log("daily_aut_resp = %g gC/m2/day\n", c->daily_aut_resp);
		error_log("\nstore\n");
		error_log("delta leaf_carbon = %g gC/m2/day\n", c->leaf_carbon - old_leaf_carbon );
		error_log("delta froot_carbon = %g gC/m2/day\n", c->froot_carbon - old_froot_carbon );
		error_log("delta croot_carbon = %g gC/m2/day\n", c->croot_carbon - old_croot_carbon );
		error_log("delta stem_carbon = %g gC/m2/day\n", c->stem_carbon - old_stem_carbon );
		error_log("delta branch_carbon = %g gC/m2/day\n", c->branch_carbon - old_branch_carbon);
		error_log("delta reserve_carbon = %g gC/m2/day\n", c->reserve_carbon - old_reserve_carbon);
		error_log("delta fruit_carbon = %g gC/m2/day\n",c->fruit_carbon - old_fruit_carbon);
		error_log("delta litr_carbon = %g gC/m2/day\n", c->litr_carbon - old_litr_carbon);
		error_log("delta cwd_carbon = %g gC/m2/day\n", c->cwd_carbon - old_cwd_carbon);
		error_log("delta soil_carbon = %g gC/m2/day\n", c->soil_carbon - old_soil_carbon);
		error_log("\ncarbon in = %g gC/m2/day\n", in);
		error_log("carbon out = %g gC/m2/day\n", out);
		error_log("delta carbon store = %g gC/m2/day\n", store - old_store);
		error_log("carbon_balance = %g gC/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' carbon mass balance (gC/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

		return 0;
	}
	else
	{
		old_leaf_carbon    = c->leaf_carbon;
		old_froot_carbon   = c->froot_carbon;
		old_croot_carbon   = c->croot_carbon;
		old_stem_carbon    = c->stem_carbon;
		old_branch_carbon  = c->branch_carbon;
		old_reserve_carbon = c->reserve_carbon ;
		old_fruit_carbon   = c->fruit_carbon ;
		old_litr_carbon    = c->litr_carbon ;
		old_cwd_carbon     = c->cwd_carbon;
		old_soil_carbon    = c->soil_carbon ;
		old_store = store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance (gC/m2/day)\n");
	}
	/*******************************************************************************************************************/
	/* DAILY CHECK ON CELL LEVEL CARBON MASS BALANCE */
	/* check complete cell level carbon mass balance */
//
//	/* sum of sources */
//	in_tC    = c->daily_gpp_tC;
//
//	/* sum of sinks */
//	out_tC   = c->daily_aut_resp_tC /*+ c->daily_het_resp_tC*/;
//
//	/* sum of current storage */
//	store_tC = c->leaf_tC +
//			c->froot_tC   +
//			c->croot_tC   +
//			c->stem_tC    +
//			c->branch_tC  +
//			c->reserve_tC +
//			c->fruit_tC   +
//			c->litr_tC    +
//			c->cwd_tC     +
//			c->soil_tC    ;
//
//	/* check carbon pool balance */
//	balance_tC = in_tC - out_tC - ( store_tC - old_store_tC );
//
//	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");
//
//	/* check for carbon mass balance closure */
//	if ( ( fabs( balance_tC ) > eps ) && ( c->dos > 1 ) )
//	{
//		error_log("DOS = %d\n", c->dos);
//		error_log("\nin = %g tC/sizecell/day\n", in_tC);
//		error_log("daily_gpp_carbon = %g tC/sizecell\n", c->daily_gpp);
//		error_log("\nout = %g tC/sizecell/day\n", out_tC);
//		error_log("daily_aut_resp = %g tC/sizecell/day\n", c->daily_aut_resp);
//		error_log("\nstore\n");
//		error_log("delta leaf_tC = %g tC/sizecell/day\n", c->leaf_tC - old_leaf_tC );
//		error_log("delta froot_tC = %g tC/sizecell/day\n", c->froot_tC - old_froot_tC );
//		error_log("delta croot_tC = %g tC/sizecell/day\n", c->croot_tC - old_croot_tC );
//		error_log("delta stem_tC = %g tC/sizecell/day\n", c->stem_tC - old_stem_tC );
//		error_log("delta branch_tC = %g tC/sizecell/day\n", c->branch_tC - old_branch_tC);
//		error_log("delta reserve_tC = %g tC/sizecell/day\n", c->reserve_tC - old_reserve_tC);
//		error_log("delta fruit_tC = %g tC/sizecell/day\n",c->fruit_tC - old_fruit_tC);
//		error_log("delta litr_tC = %g tC/sizecell/day\n", c->litr_tC - old_litr_tC);
//		error_log("delta cwd_tC = %g tC/sizecell/day\n", c->cwd_tC - old_cwd_tC);
//		error_log("delta soil_tC = %g tC/sizecell/day\n", c->soil_tC - old_soil_tC);
//		error_log("\ncarbon in = %g tC/sizecell/day\n", in_tC);
//		error_log("carbon out = %g tC/sizecell/day\n", out_tC);
//		error_log("delta carbon store = %g tC/sizecell/day\n", store_tC - old_store_tC);
//		error_log("carbon_balance = %g tC/sizecell/day\n",balance_tC);
//		error_log("...FATAL ERROR in 'Cell_model_daily' carbon mass balance (tC/sizecell/day)(exit)\n");
//		CHECK_CONDITION(fabs( balance_tC ), > , eps);
//
//		return 0;
//	}
//	else
//	{
//		old_leaf_tC    = c->leaf_tC;
//		old_froot_tC   = c->froot_tC;
//		old_croot_tC   = c->croot_tC;
//		old_stem_tC    = c->stem_tC;
//		old_branch_tC  = c->branch_tC;
//		old_reserve_tC = c->reserve_tC ;
//		old_fruit_tC   = c->fruit_tC ;
//		old_litr_tC    = c->litr_tC ;
//		old_cwd_tC     = c->cwd_tC;
//		old_soil_tC    = c->soil_tC ;
//		old_store_tC   = store_tC;
//		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance(tC/sizecell/day) \n");
//	}
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
		error_log("soil water in = %g\n", in);
		error_log("soil water out = %g\n", out);
		error_log("soil water store = %g\n", store);
		error_log("delta soil water store = %g\n", store - old_store);
		error_log("soil water balance = %g\n", balance);
		error_log("...FATAL ERROR IN 'Cell_model_daily' soil water balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

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
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("meteo_daily->snow = %g\n", meteo_daily->snow);
		error_log("\nout\n");
		error_log("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		error_log("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		error_log("\nstore (as a difference between old and current)\n");
		error_log("delta c->asw = %g\n", c->snow_pack);
		error_log("soil water in = %g\n", in_snow);
		error_log("soil water out = %g\n", out_snow);
		error_log("soil water store = %g\n", store_snow);
		error_log("soil water balance = %g\n", balance_snow);
		error_log("...FATAL ERROR IN 'Cell_model_daily' snow water balance (exit)\n");
		CHECK_CONDITION(fabs( balance ), > , eps);

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