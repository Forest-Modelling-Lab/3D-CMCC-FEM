/*
 * Soil_model_daily_check_balance.c
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

int check_soil_radiation_flux_balance (cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log,"\n*********CHECK SOIL RADIATIVE BALANCE************\n");
	/* DAILY CHECK ON RADIATIVE BALANCE */

	/* PAR RADIATIVE BALANCE */

	/* sum of sources */
	in      = meteo_daily->par;

	/* sum of sinks */
	out     = c->par_refl_soil + c->par_refl_snow;

	/* sum of current storage */
	store   = c->apar_soil + c->apar_snow ;

	balance = in - out -store;

	logger(g_debug_log, "\nSOIL RADIATIVE BALANCE (PAR)\n");

	if ( fabs( balance ) > eps )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("incoming par = %g molPAR/m2/day\n", meteo_daily->par);
		error_log("\nout\n");
		error_log("c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		error_log("c->par_refl_snow = %g molPAR/m2/day\n",c->par_refl_snow);
		error_log("\nstore\n");
		error_log("c->par_for_soil = %g molPAR/m2/day\n", c->apar_soil);
		error_log("c->par_for_snow = %g molPAR/m2/day\n", c->apar_snow);
		error_log("\npar in = %g molPAR/m2/day\n", in);
		error_log("par out = %g molPAR/m2/day\n", out);
		error_log("par store = %g molPAR/m2/day\n", store);
		error_log("par balance = %g molPAR/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Soil_model' PAR radiative balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Soil_model_daily' PAR radiative balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	/*******************************************************************************************************************************************/

	/* SHORT WAVE RADIATION RADIATIVE BALANCE */

	/* sum of sources */
	in      =  meteo_daily->sw_downward_W;

	/* sum of sinks */
	out     = c->sw_rad_refl_soil + c->sw_rad_refl_snow;

	/* sum of current storage */
	store   = c->sw_rad_abs_soil + c->sw_rad_abs_snow;

	balance = in - out - store;

	logger(g_debug_log, "\nSOIL RADIATIVE BALANCE (Short Wave Radiation)\n");

	if ( fabs( balance ) > eps )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("incoming radiation = %g W/m2\n", meteo_daily->sw_downward_W);
		error_log("\nout\n");
		error_log("c->sw_rad_refl_soil = %g W/m2\n",c->sw_rad_refl_soil);
		error_log("c->sw_rad_refl_snow = %g W/m2\n",c->sw_rad_refl_snow);
		error_log("\nstore\n");
		error_log("c->net_sw_rad_for_soil = %g W/m2\n", c->sw_rad_abs_soil);
		error_log("\nradiation in = %g W/m2\n", in);
		error_log("radiation out = %g W/m2\n", out);
		error_log("net radiation store = %g W/m2\n", store);
		error_log("radiation balance = %g W/m2\n",balance);
		error_log("...FATAL ERROR IN 'Soil_model_daily' Short Wave Radiation radiative balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Soil_model_daily' Short Wave Radiation radiative balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	/*******************************************************************************************************************************************/

	/* PPFD RADIATIVE BALANCE */
	/* sum of sources */
	in      = meteo_daily->ppfd;

	/* sum of sinks */
	out     = c->ppfd_refl_soil + c->ppfd_refl_snow;

	/* sum of current storage */
	store   = c->ppfd_abs_soil + c->ppfd_abs_snow;

	balance = in - out -store;

	logger(g_debug_log, "\nSOIL RADIATIVE BALANCE (PPFD)\n");

	if ( fabs( balance ) > eps )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("incoming PPFD = %g umol/m2/sec\n", meteo_daily->ppfd);
		error_log("\nout\n");
		error_log("c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		error_log("c->ppfd_refl_snow = %g umol/m2/sec\n",c->ppfd_refl_snow);
		error_log("\nstore\n");
		error_log("c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_abs_soil);
		error_log("\nPPFD in = %g umol/m2/sec\n", in);
		error_log("PPFD out = %g umol/m2/sec\n", out);
		error_log("PPFD store = %g umol/m2/sec\n", store);
		error_log("PPFD balance = %g umol/m2/sec\n",balance);
		error_log("...FATAL ERROR IN 'Soil_model_daily' PPFD radiative balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Soil_model_daily' PPFD radiative balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	/*******************************************************************************************************************************************/
	return 1;
}

int check_soil_carbon_flux_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK SOIL CARBON BALANCE************\n");

	/* check complete soil level carbon flux balance */

	/* sum of carbon sources */
	in      = c->daily_soilC;

	/* sum of carbon sinks */
	out     = c->daily_het_resp;

	/* sum of current carbon storage */
	store   = c->soil1C +
			c->soil2C +
			c->soil3C +
			c->soil4C ;

	balance = in - out -store;

	logger(g_debug_log, "\nSOIL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_litrC_to_soilC = %g gC/m2/day\n", c->daily_soilC);
		error_log("\nout\n");
		error_log("c->daily_het_resp = %g gC/m2/day\n",c->daily_het_resp);
		error_log("\nstore\n");
		error_log("c->soil1C = %g gC/m2/day\n", c->soil1C);
		error_log("c->soil2C = %g gC/m2/day\n", c->soil2C);
		error_log("c->soil3C = %g gC/m2/day\n", c->soil3C);
		error_log("c->soil4C = %g gC/m2/day\n", c->soil4C);
		error_log("\ncarbon in = %g gC/m2/day\n", in);
		error_log("carbon out = %g gC/m2/day\n", out);
		error_log("carbon store = %g gC/m2/day\n", store);
		error_log("carbon_balance = %g gC/m2/day\n",balance);
		error_log("...FATAL ERROR IN 'Soil_model_daily' carbon balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Soil_model_daily' carbon balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	return 1;
}

int check_soil_water_flux_balance(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;
	static double old_store;

	logger(g_debug_log, "\n*********CHECK SOIL WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow melt) */
	in      = meteo_daily->rain + c->daily_snow_melt;

	/* sum of sinks */
	out     = c->daily_soil_evapo + c->daily_out_flow + c->daily_c_transp;

	/* sum of current storage in soil */
	store   = c->asw;

	/* check soil pool water balance */
	balance = in - out - ( store - old_store );

	logger(g_debug_log, "\nSOIL POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin\n");
		error_log("meteo_daily->rain = %g\n", meteo_daily->rain);
		error_log("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		error_log("\nout\n");
		error_log("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		error_log("c->daily_out_flow = %g\n", c->daily_out_flow);
		error_log("c->daily_c_transp = %g\n", c->daily_c_transp);
		error_log("\nstore (as a difference between old and current)\n");
		error_log("c->asw = %g\n", c->asw);
		error_log("soil water in = %g\n", in);
		error_log("soil water out = %g\n", out);
		error_log("soil water store = %g\n", store);
		error_log("delta soil water balance = %g\n", store - old_store );
		error_log("...FATAL ERROR IN 'Soil_model_daily' soil water balance (exit)\n");

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok 'Soil_model_daily' soil water balance\n");
	}
	logger(g_debug_log,"*****************************************************\n");

	return 1;
}
