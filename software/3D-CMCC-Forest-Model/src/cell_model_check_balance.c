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
	store = c->daily_leaf_carbon   + c->daily_stem_carbon +
			c->daily_froot_carbon  + c->daily_croot_carbon +
			c->daily_branch_carbon + c->daily_reserve_carbon +
			c->daily_fruit_carbon  + c->daily_litrC +
			c->daily_soilC         + c->daily_cwdC;

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
		error_log("c->daily_leaf_carbon = %g gC/m2/day\n",        c->daily_leaf_carbon);
		error_log("c->daily_stem_carbon = %g gC/m2/day\n",        c->daily_stem_carbon);
		error_log("c->daily_fine_root_carbon = %g gC/m2/day\n",   c->daily_froot_carbon);
		error_log("c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_croot_carbon);
		error_log("c->daily_branch_carbon = %g gC/m2/day\n",      c->daily_branch_carbon);
		error_log("c->daily_reserve_carbon = %g gC/m2/day\n",     c->daily_reserve_carbon);
		error_log("c->daily_litrC = %g gC/m2/day\n",              c->daily_litrC);
		error_log("c->daily_cwdC = %g gC/m2/day\n",               c->daily_cwdC);
		error_log("c->daily_soilC = %g gC/m2/day\n",              c->daily_soilC);
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
	static double leaf;
	static double froot;
	static double croot;
	static double branch;
	static double reserve;
	static double stem;
	static double fruit;
	static double litr;
	static double cwd;
	static double soil;

	/* DAILY CHECK ON CELL LEVEL CARBON MASS BALANCE */
	/* check complete cell level carbon mass balance */

	/* sum of sources */
	c->cell_carbon_in    = c->daily_gpp;

	/* sum of sinks */
	c->cell_carbon_out   = c->daily_growth_resp +
			c->daily_maint_resp /* + c->daily_het_resp */;

	/* sum of current storage */
	c->cell_carbon_store = c->leaf_carbon +
			c->froot_carbon   +
			c->croot_carbon   +
			c->stem_carbon    +
			c->branch_carbon  +
			c->reserve_carbon +
			c->fruit_carbon   +
			c->litrC          +
			c->cwdC           +
			c->soilC          ;

	/* check carbon pool balance */
	c->cell_carbon_balance = c->cell_carbon_in - c->cell_carbon_out - ( c->cell_carbon_store - c->cell_carbon_old_store );

	logger(g_debug_log, "\nCELL LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( c->cell_carbon_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin\n");
		error_log("daily_gpp_carbon = %g gC/m2\n",       c->daily_gpp);
		error_log("\nout\n");
		error_log("daily_aut_resp = %g gC/m2/day\n",     c->daily_aut_resp);
		error_log("\nstore\n");
		error_log("leaf_carbon = %g gC/m2/day\n",        leaf - c->leaf_carbon);
		error_log("froot_carbon = %g gC/m2/day\n",       froot - c->froot_carbon);
		error_log("croot_carbon = %g gC/m2/day\n",       croot - c->croot_carbon);
		error_log("stem_carbon = %g gC/m2/day\n",        stem - c->stem_carbon);
		error_log("branch_carbon = %g gC/m2/day\n",      branch - c->branch_carbon);
		error_log("reserve_carbon = %g gC/m2/day\n",     reserve - c->reserve_carbon);
		error_log("fruit_carbon = %g gC/m2/day\n",       fruit - c->fruit_carbon);
		error_log("litr_carbon = %g gC/m2/day\n",        litr - c->litrC);
		error_log("cwd_carbon = %g gC/m2/day\n",         cwd - c->cwdC);
		error_log("soil_carbon = %g gC/m2/day\n",        soil - c->soilC);
		error_log("\ncarbon in = %g gC/m2/day\n",        c->cell_carbon_in);
		error_log("carbon out = %g gC/m2/day\n",         c->cell_carbon_out);
		error_log("delta carbon store = %g gC/m2/day\n", c->cell_carbon_store - c->cell_carbon_old_store);
		error_log("carbon_balance = %g gC/m2/day\n",     c->cell_carbon_balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' carbon mass balance (gC/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( c->cell_carbon_balance ), > , eps);

		return 0;
	}
	else
	{
		leaf = c->leaf_carbon;
		froot = c->froot_carbon;
		croot = c->croot_carbon;
		branch = c->branch_carbon;
		reserve = c->reserve_carbon;
		stem = c->stem_carbon;
		fruit = c->fruit_carbon;
		litr = c->litrC;
		cwd = c->cwdC;
		soil = c->soilC;

		c->cell_carbon_old_store = c->cell_carbon_store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance (gC/m2/day)\n");
	}
	/* ok */
	return 1;
}

int check_cell_nitrogen_flux_balance(cell_t *const c)
{
	//todo as for carbon
//	double in;
//	double out;
//	double store;
//	double balance;
//
//	logger(g_debug_log, "\n*********CHECK CELL CARBON BALANCE************\n");
//
//	/* check complete cell level carbon flux balance */
//
//	/* sum of carbon sources */
//	in      = c->daily_gpp;
//
//	/* sum of carbon sinks */
//	out     = c->daily_aut_resp;
//
//	/* sum of current carbon storage */
//	store   = c->daily_npp;
//
//	balance = in - out - store;
//
//	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");
//
//	if ( fabs( balance ) > eps )
//	{
//		error_log("DOY = %d\n", c->doy);
//		error_log("\nin\n");
//		error_log("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
//		error_log("\nout\n");
//		error_log("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_aut_resp);
//		error_log("\nstore\n");
//		error_log("c->daily_leaf_carbon = %g gC/m2/day\n", c->daily_npp);
//		error_log("\ncarbon in = %g gC/m2/day\n", in);
//		error_log("carbon out = %g gC/m2/day\n", out);
//		error_log("carbon store = %g gC/m2/day\n", store);
//		error_log("carbon_balance = %g gC/m2/day\n",balance);
//		error_log("...FATAL ERROR in 'Cell_model_daily' carbon flux balance (first) (exit)\n");
//		CHECK_CONDITION(fabs( balance ), > , eps);
//
//		return 0;
//	}
//	else
//	{
//		logger(g_debug_log, "...ok 'Cell_model_daily' carbon balance\n");
//	}
//
//	/***************************************************************************************************************/
//
//	/* sum of carbon sources */
//	in = c->daily_gpp;
//
//	/* sum of carbon sinks */
//	out = c->daily_maint_resp + c->daily_growth_resp;
//
//	/* sum of current carbon storage */
//	store = c->daily_leaf_carbon   + c->daily_stem_carbon +
//			c->daily_froot_carbon  + c->daily_croot_carbon +
//			c->daily_branch_carbon + c->daily_reserve_carbon +
//			c->daily_fruit_carbon  + c->daily_litrC +
//			c->daily_soilC         + c->daily_cwdC;
//
//	balance = in - out - store;
//
//	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");
//
//	if ( fabs( balance ) > eps )
//	{
//		error_log("DOY = %d\n", c->doy);
//		error_log("\nin\n");
//		error_log("c->daily_gpp = %g gC/m2/day\n", c->daily_gpp);
//		error_log("\nout\n");
//		error_log("c->daily_tot_aut_resp = %g gC/m2/day\n",c->daily_maint_resp + c->daily_growth_resp);
//		error_log("\nstore\n");
//		error_log("c->daily_leaf_carbon = %g gC/m2/day\n",        c->daily_leaf_carbon);
//		error_log("c->daily_stem_carbon = %g gC/m2/day\n",        c->daily_stem_carbon);
//		error_log("c->daily_fine_root_carbon = %g gC/m2/day\n",   c->daily_froot_carbon);
//		error_log("c->daily_coarse_root_carbon = %g gC/m2/day\n", c->daily_croot_carbon);
//		error_log("c->daily_branch_carbon = %g gC/m2/day\n",      c->daily_branch_carbon);
//		error_log("c->daily_reserve_carbon = %g gC/m2/day\n",     c->daily_reserve_carbon);
//		error_log("c->daily_litrC = %g gC/m2/day\n",              c->daily_litrC);
//		error_log("c->daily_cwdC = %g gC/m2/day\n",               c->daily_cwdC);
//		error_log("c->daily_soilC = %g gC/m2/day\n",              c->daily_soilC);
//		error_log("\ncarbon in = %g gC/m2/day\n", in);
//		error_log("carbon out = %g gC/m2/day\n", out);
//		error_log("carbon store = %g gC/m2/day\n", store);
//		error_log("carbon_balance = %g gC/m2/day\n",balance);
//		error_log("...FATAL ERROR in 'Cell_model_daily' carbon flux balance (second) (exit)\n");
//		CHECK_CONDITION(fabs( balance ), > , eps);
//
//		return 0;
//	}
//	else
//	{
//		logger(g_debug_log, "...ok carbon balance\n");
//	}
	/* ok */
	return 1;
}

int check_cell_nitrogen_mass_balance(cell_t *const c, const meteo_annual_t *const meteo_annual)
{
	static double leaf;
	static double froot;
	static double croot;
	static double branch;
	static double reserve;
	static double stem;
	static double fruit;
	static double litr;
	static double cwd;
	static double soil;

	/* DAILY CHECK ON CELL LEVEL NITROGEN MASS BALANCE */
	/* check complete cell level nitrogen mass balance */

	/* sum of sources */
	c->cell_nitrogen_in    = meteo_annual->Ndep + c->Nfix;

	/* sum of sinks */
	c->cell_nitrogen_out   = c->Nleach + c->Nvol;

	/* sum of current storage */
	c->cell_nitrogen_store = c->leaf_nitrogen +
			c->froot_nitrogen   +
			c->croot_nitrogen   +
			c->stem_nitrogen    +
			c->branch_nitrogen  +
			c->reserve_nitrogen +
			c->fruit_nitrogen   +
			c->litrN          +
			c->cwdN           +
			c->soilN          ;

	/* check nitrogen pool balance */
	c->cell_nitrogen_balance = c->cell_nitrogen_in - c->cell_nitrogen_out - ( c->cell_nitrogen_store - c->cell_nitrogen_old_store );

	logger(g_debug_log, "\nCELL LEVEL NITROGEN MASS BALANCE\n");

	/* check for nitrogen mass balance closure */
	if ( ( fabs( c->cell_nitrogen_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin\n");
		error_log("Ndep = %g gN/m2\n",                     meteo_annual->Ndep);
		error_log("c->Nfix = %g gN/m2\n",                  c->Nfix);
		error_log("\nout\n");
		error_log("Nleach = %g gN/m2/day\n",               c->Nleach);
		error_log("Nvol = %g gN/m2/day\n",                 c->Nvol);
		error_log("\nstore\n");
		error_log("leaf_nitrogen = %g gN/m2/day\n",        leaf - c->leaf_nitrogen);
		error_log("froot_nitrogen = %g gN/m2/day\n",       froot - c->froot_nitrogen);
		error_log("croot_nitrogen = %g gN/m2/day\n",       croot - c->croot_nitrogen);
		error_log("stem_nitrogen = %g gN/m2/day\n",        stem - c->stem_nitrogen);
		error_log("branch_nitrogen = %g gN/m2/day\n",      branch - c->branch_nitrogen);
		error_log("reserve_nitrogen = %g gN/m2/day\n",     reserve - c->reserve_nitrogen);
		error_log("fruit_nitrogen = %g gN/m2/day\n",       fruit - c->fruit_nitrogen);
		error_log("litr_nitrogen = %g gN/m2/day\n",        litr - c->litrN);
		error_log("cwd_nitrogen = %g gN/m2/day\n",         cwd - c->cwdN);
		error_log("soil_nitrogen = %g gN/m2/day\n",        soil - c->soilN);
		error_log("\nnitrogen in = %g gN/m2/day\n",        c->cell_nitrogen_in);
		error_log("nitrogen out = %g gN/m2/day\n",         c->cell_nitrogen_out);
		error_log("delta nitrogen store = %g gN/m2/day\n", c->cell_nitrogen_store - c->cell_nitrogen_old_store);
		error_log("nitrogen_balance = %g gN/m2/day\n",     c->cell_nitrogen_balance);
		error_log("...FATAL ERROR in 'Cell_model_daily' nitrogen mass balance (gN/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( c->cell_nitrogen_balance ), > , eps);

		return 0;
	}
	else
	{
		leaf = c->leaf_nitrogen;
		froot = c->froot_nitrogen;
		croot = c->croot_nitrogen;
		branch = c->branch_nitrogen;
		reserve = c->reserve_nitrogen;
		stem = c->stem_nitrogen;
		fruit = c->fruit_nitrogen;
		litr = c->litrC;
		cwd = c->cwdC;
		soil = c->soilC;

		c->cell_nitrogen_old_store = c->cell_nitrogen_store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' nitrogen mass balance (gC/m2/day)\n");
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
	c->cell_water_in      = meteo_daily->prcp;

	/* sum of sinks */
	c->cell_water_out     = c->daily_canopy_transp + c->daily_canopy_evapo + c->daily_soil_evapo + c->daily_snow_subl + c->daily_out_flow;

	/* sum of current storage in cell pools */
	c->cell_water_store   = ( c->asw + c->canopy_water_stored + c->snow_pack );

	/* check soil pool water balance */
	c->cell_water_balance = c->cell_water_in - c->cell_water_out - ( c->cell_water_store - c->cell_water_old_store );

	logger(g_debug_log, "\nCELL SOIL POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( c->cell_water_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOY = %d\n", c->dos);
		error_log("\nin\n");
		error_log("meteo_daily->prcp = %g\n", meteo_daily->prcp);
		error_log("\nout\n");
		error_log("c->daily_canopy_transp = %g\n", c->daily_canopy_transp);
		error_log("c->daily_canopy_evapo = %g\n", c->daily_canopy_evapo);
		error_log("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		error_log("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		error_log("c->out_flow = %g\n", c->daily_out_flow);
		error_log("\nstore (as a difference between old and current)\n");
		error_log("c->canopy_water_stored = %g\n", c->canopy_water_stored);
		error_log("c->asw = %g\n", c->asw);
		error_log("c->snow_pack = %g\n", c->snow_pack);
		error_log("soil water in = %g\n", c->cell_water_in);
		error_log("soil water out = %g\n", c->cell_water_out  );
		error_log("soil water store = %g\n", c->cell_water_store);
		error_log("delta soil water store = %g\n", c->cell_water_store - c->cell_water_old_store);
		error_log("soil water balance = %g\n", c->cell_water_balance);
		error_log("...FATAL ERROR IN 'Cell_model_daily' soil water balance (exit)\n");
		CHECK_CONDITION(fabs( c->cell_water_balance ), > , eps);

		return 0;
	}
	else
	{
		c->cell_water_old_store = c->cell_water_store;
		logger(g_debug_log, "...ok 'Cell_model_daily' soil water balance\n");
	}

	logger(g_debug_log, "\n*********CHECK CELL SNOW WATER BALANCE************\n");
	/* DAILY CHECK ON SNOW POOL-SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account snow-soil-atmosphere fluxes */

	/* sum of sources (snow) */
	c->cell_snow_in      = meteo_daily->snow;

	/* sum of sinks */
	c->cell_snow_out     = c->daily_snow_melt + c->daily_snow_subl;

	/* sum of current storage in snow pack */
	c->cell_snow_store   = c->snow_pack;

	/* check snow pool water balance */
	c->cell_snow_balance = c->cell_snow_in - c->cell_snow_out - ( c->cell_snow_store - c->cell_snow_old_store);

	logger(g_debug_log, "\nCELL SNOW POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( c->cell_snow_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("meteo_daily->snow = %g\n", meteo_daily->snow);
		error_log("\nout\n");
		error_log("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		error_log("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		error_log("\nstore (as a difference between old and current)\n");
		error_log("delta c->asw = %g\n", c->snow_pack);
		error_log("soil water in = %g\n", c->cell_snow_in);
		error_log("soil water out = %g\n", c->cell_snow_out);
		error_log("soil water store = %g\n", c->cell_snow_store);
		error_log("soil water balance = %g\n", c->cell_snow_balance);
		error_log("...FATAL ERROR IN 'Cell_model_daily' snow water balance (exit)\n");
		CHECK_CONDITION(fabs( c->cell_snow_balance ), > , eps);

		return 0;
	}
	else
	{
		c->cell_snow_old_store = c->cell_snow_store;
		logger(g_debug_log, "...ok 'Cell_model_daily' snow water balance\n");
	}
	/* ok */
	return 1;
}
