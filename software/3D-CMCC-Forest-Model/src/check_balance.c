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
	out = c->sw_rad_refl + c->sw_rad_for_soil_refl + c->sw_rad_for_snow_refl;

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
		printf("c->sw_rad_for_soil_refl = %g W/m2\n",c->sw_rad_for_soil_refl);
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

/******************************************************CLASS LEVEL BALANCE CLOSURE*****************************************************/

int check_tree_class_radiation_flux_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE RADIATIVE BALANCE */
	//fixme: for all the balance doesn't takes into account the amount of light previously removed in Radiation function

	/* PAR balance */
	/* sum of sources */
	in = s->value[PAR];

	/* sum of sinks */
	out = s->value[PAR_REFL] + s->value[TRANSM_PAR];

	/* sum of current storage */
	store = s->value[APAR_SUN] + s->value[APAR_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL PAR BALANCE\n");

	/* check for PAR balance closure*/
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("PAR in = %g\n", in);
		printf("PAR out = %g\n", out);
		printf("PAR store = %g\n", store);
		printf("PAR balance = %g\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL PAR balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok PAR balance at class level\n");
	}
	/****************************************************************************************************************/
	/* Net Short-Wave radiation balance */
	/* sum of sources */
	in      = s->value[SW_RAD];

	/* sum of sinks */
	/* it must take into account the overall transmitted NET_RAD (reflected is yet computed for net radiation) */
	out     = s->value[SW_RAD_REFL] + s->value[SW_RAD_TRANSM];

	/* sum of current storage */
	store   = s->value[SW_RAD_ABS_SUN] + s->value[SW_RAD_ABS_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL Short Wave BALANCE\n");

	/* check for NET_RAD balance closure*/
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("NET_SW_RAD in = %g\n", in);
		printf("NET_RAD out = %g\n", out);
		printf("NET_RAD store = %g\n", store);
		printf("NET_RAD balance = %g\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL NET_RAD balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok Short Wave radiation balance at class level\n");
	}
	/****************************************************************************************************************/
	/* PPFD balance */
	/* sum of sources */
	in      = s->value[PPFD];

	/* sum of sinks */
	out     = s->value[PPFD_REFL] +  s->value[PPFD_TRANSM];

	/* sum of current storage */
	store   = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL PPFD BALANCE\n");

	/* check for PPFD balance closure*/
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("PPFD in = %g\n", in);
		printf("PPFD out = %g\n", out);
		printf("PPFD store = %g\n", store);
		printf("PPFD balance = %g\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL PPFD balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok PPFD balance at class level\n");
	}

	return 1;
}

int check_tree_class_carbon_flux_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CARBON FLUX BALANCE */
	/* check complete tree level carbon flux balance */

	/* sum of sources */
	in      = s->value[GPP];

	/* sum of sinks */
	out     = s->value[TOTAL_AUT_RESP];

	/* sum of current storage */
	store   = s->value[NPP];

	/* check carbon flux balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL CARBON FLUX BALANCE (first)\n");

	/* check for carbon flux balance closure */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin = %g gC/m2/day\n", in);
		printf("GPP = %g gC/m2/day\n", s->value[GPP]);
		printf("\nout = %g gC/m2/day\n", out);
		printf("TOTAL_AUT_RESP = %g gC/m2/day\n", s->value[TOTAL_AUT_RESP]);
		printf("\nstore = %g gC/m2/day\n", store);
		printf("NPP = %g gC/m2/day\n", s->value[NPP]);
		printf("\nbalance = %g gC/m2/day\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL carbon flux balance (first) (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok carbon flux balance (first) at class level\n");
	}

	/* DAILY CHECK ON CLASS LEVEL CARBON BALANCE */
	/* check complete tree level carbon balance */

	/* sum of sources */
	in      = s->value[GPP_tC];

	/* sum of sinks */
	out     = s->value[TOTAL_MAINT_RESP_tC] + s->value[TOTAL_GROWTH_RESP_tC] +
			( s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR] + s->value[C_FRUIT_TO_LITR]);

	/* sum of current storage */
	store   = s->value[C_TO_LEAF]  +
			s->value[C_TO_STEM]    +
			s->value[C_TO_FROOT]   +
			s->value[C_TO_CROOT]   +
			s->value[C_TO_BRANCH]  +
			s->value[C_TO_RESERVE] +
			s->value[C_TO_FRUIT]   ;

	/* check carbon flux balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL CARBON FLUX BALANCE (second)\n");

	/* check for carbon flux balance closure */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin = %g tC/sizecell/day\n", in);
		printf("GPP_gC = %g tC/sizecell/day\n", s->value[GPP_tC]);
		printf("\nout = %g tC/sizecell/day\n", out);
		printf("TOTAL_MAINT_RESP = %g tC/sizecell/day\n", s->value[TOTAL_MAINT_RESP_tC]);
		printf("TOTAL_GROWTH_RESP = %g tC/sizecell/day\n", s->value[TOTAL_GROWTH_RESP_tC]);
		printf("C_LEAF_TO_LITR = %g tC/sizecell/day\n", s->value[C_LEAF_TO_LITR]);
		printf("C_FROOT_TO_LITR = %g tC/sizecell/day\n", s->value[C_FROOT_TO_LITR]);
		printf("\nstore = %g tC/sizecell/day\n", store);
		printf("C_TO_LEAF = %g tC/sizecell/day\n", s->value[C_TO_LEAF]);
		printf("C_TO_FROOT = %g tC/sizecell/day\n", s->value[C_TO_FROOT]);
		printf("C_TO_CROOT = %g tC/sizecell/day\n", s->value[C_TO_CROOT]);
		printf("C_TO_STEM = %g tC/sizecell/day\n", s->value[C_TO_STEM]);
		printf("C_TO_RESERVE = %g tC/sizecell/day\n", s->value[C_TO_RESERVE]);
		printf("C_TO_BRANCH = %g tC/sizecell/day\n", s->value[C_TO_BRANCH]);
		printf("C_TO_FRUIT = %g tC/sizecell/day\n", s->value[C_TO_FRUIT]);
		printf("\nbalance = %g tC/sizecell/day\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL carbon flux balance (second) (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok carbon flux balance (second) at class level\n");
	}

	/*******************************************************************************************************************/
	return 1;
}

int check_tree_class_carbon_mass_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;
	static double old_store;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CARBON MASS BALANCE */
	/* check complete tree level carbon mass balance */

	/* sum of sources */
	in      = s->value[GPP_tC];

	/* sum of sinks */
	out     = s->value[TOTAL_MAINT_RESP_tC] +
			s->value[TOTAL_GROWTH_RESP_tC] +
			s->value[C_TO_LITR];

	/* sum of current storage */
	store   = s->value[LEAF_C]  +
			s->value[FROOT_C]   +
			s->value[CROOT_C]   +
			s->value[STEM_C]    +
			s->value[BRANCH_C]  +
			s->value[RESERVE_C] +
			s->value[FRUIT_C]   ;

	/* check carbon pool balance */
	balance = in - out - (store - old_store);

	logger(g_debug_log, "\nCLASS LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		printf("DOS = %d\n", c->dos);
		printf("\nin = %g tC/sizecell/day\n", in);
		printf("GPP = %g tC/sizecell\n", s->value[GPP_tC]);
		printf("\nout = %g tC/sizecell/day\n", out);
		printf("TOTAL_MAINT_RESP_tC = %g tC/sizecell/day\n", s->value[TOTAL_MAINT_RESP_tC]);
		printf("TOTAL_GROWTH_RESP_tC = %g tC/sizecell/day\n", s->value[TOTAL_GROWTH_RESP_tC]);
		printf("LITR_C = %g tC/sizecell/day\n", s->value[LITR_C]);
		printf("\nold_store = %g tC/sizecell\n", old_store);
		printf("store = %g tC/sizecell\n", store);
		printf("store - old_tore = %g tC/sizecell\n", store - old_store);
		printf("LEAF_C = %g tC/cell/day\n", s->value[LEAF_C]);
		printf("FROOT_C = %g tC/cell/day\n", s->value[FROOT_C]);
		printf("CROOT_C = %g tC/cell/day\n", s->value[CROOT_C]);
		printf("STEM_C = %g tC/cell/day\n", s->value[STEM_C]);
		printf("BRANCH_C = %g tC/cell/day\n", s->value[BRANCH_C]);
		printf("RESERVE_C = %g tC/cell/day\n", s->value[RESERVE_C]);
		printf("FRUIT_C = %g tC/cell/day\n", s->value[FRUIT_C]);
		printf("\nbalance = %g tC/sizecell\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL carbon mass balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok carbon mass balance at class level\n");
	}

	/*******************************************************************************************************************/
	return 1;
}
int check_tree_class_nitrogen_flux_balance (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL NITROGEN BALANCE */
	/* check complete tree level nitrogen balance */

	/* sum of sources */
	in      = s->value[NPP_tN];

	/* sum of sinks */
	out     =  s->value[N_LEAF_TO_LITR] + s->value[N_FROOT_TO_LITR]/* + s->value[N_FRUIT_TO_LITR]*/;

	/* sum of current storage */
	store   = s->value[N_TO_LEAF] +
			s->value[N_TO_STEM]   +
			s->value[N_TO_FROOT]  +
			s->value[N_TO_CROOT]  +
			s->value[N_TO_BRANCH] +
			s->value[N_TO_RESERVE];

	/* check nitrogen flux balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL NITROGEN FLUX BALANCE\n");

	/* check for nitrogen flux balance closure */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin = %g tN/sizecell/day\n", in);
		printf("NPP_tN = %g tN/sizecell/day\n", s->value[NPP_tN]);
		printf("\nout = %g tN/sizecell/day\n", out);
		printf("N_LEAF_TO_LITR = %g tN/sizecell/day\n", s->value[N_LEAF_TO_LITR]);
		printf("N_FROOT_TO_LITR = %g tN/sizecell/day\n", s->value[N_FROOT_TO_LITR]);
		printf("\nstore = %g tN/sizecell/day\n", store);
		printf("N_TO_LEAF = %g tN/sizecell/day\n", s->value[N_TO_LEAF]);
		printf("N_TO_FROOT = %g tN/sizecell/day\n", s->value[N_TO_FROOT]);
		printf("N_TO_CROOT = %g tN/sizecell/day\n", s->value[N_TO_CROOT]);
		printf("N_TO_STEM = %g tN/sizecell/day\n", s->value[N_TO_STEM]);
		printf("N_TO_RESERVE = %g tN/sizecell/day\n", s->value[N_TO_RESERVE]);
		printf("N_TO_BRANCH = %g tN/sizecell/day\n", s->value[N_TO_BRANCH]);
		printf("N_TO_FRUIT = %g tN/sizecell/day\n", s->value[N_TO_FRUIT]);
		printf("\nbalance = %g tN/sizecell/day\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL nitrogen flux balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok carbon flux nitrogen at class level\n");
	}

	/*******************************************************************************************************************/
	return 1;
}

int check_tree_class_water_flux_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE WATER BALANCE */

	/* sum of sources */
	in      = s->value[CANOPY_INT];

	/* sum of sinks */
	out     = s->value[CANOPY_EVAPO];

	/* sum of current storage */
	store   = s->value[CANOPY_WATER] - s->value[OLD_CANOPY_WATER];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL WATER BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("canopy water in = %g\n", in);
		printf("canopy water out = %g\n", out);
		printf("canopy water store = %g\n", store);
		printf("canopy water balance = %g\n", balance);
		printf("...FATAL ERROR AT CELL LEVEL canopy water balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok canopy water balance at class level\n");
	}

	return 1;
}
