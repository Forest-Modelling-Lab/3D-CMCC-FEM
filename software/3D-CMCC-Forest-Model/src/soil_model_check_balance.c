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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("incoming par = %g molPAR/m2/day\n", meteo_daily->par);
		logger_all("\nout\n");
		logger_all("c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		logger_all("c->par_refl_snow = %g molPAR/m2/day\n",c->par_refl_snow);
		logger_all("\nstore\n");
		logger_all("c->par_for_soil = %g molPAR/m2/day\n", c->apar_soil);
		logger_all("c->par_for_snow = %g molPAR/m2/day\n", c->apar_snow);
		logger_all("\npar in = %g molPAR/m2/day\n", in);
		logger_all("par out = %g molPAR/m2/day\n", out);
		logger_all("par store = %g molPAR/m2/day\n", store);
		logger_all("par balance = %g molPAR/m2/day\n",balance);
		logger_all("...FATAL ERROR in 'Soil_model' PAR radiative balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("incoming radiation = %g W/m2\n", meteo_daily->sw_downward_W);
		logger_all("\nout\n");
		logger_all("c->sw_rad_refl_soil = %g W/m2\n",c->sw_rad_refl_soil);
		logger_all("c->sw_rad_refl_snow = %g W/m2\n",c->sw_rad_refl_snow);
		logger_all("\nstore\n");
		logger_all("c->net_sw_rad_for_soil = %g W/m2\n", c->sw_rad_abs_soil);
		logger_all("\nradiation in = %g W/m2\n", in);
		logger_all("radiation out = %g W/m2\n", out);
		logger_all("net radiation store = %g W/m2\n", store);
		logger_all("radiation balance = %g W/m2\n",balance);
		logger_all("...FATAL ERROR IN 'Soil_model_daily' Short Wave Radiation radiative balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("incoming PPFD = %g umol/m2/sec\n", meteo_daily->ppfd);
		logger_all("\nout\n");
		logger_all("c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		logger_all("c->ppfd_refl_snow = %g umol/m2/sec\n",c->ppfd_refl_snow);
		logger_all("\nstore\n");
		logger_all("c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_abs_soil);
		logger_all("\nPPFD in = %g umol/m2/sec\n", in);
		logger_all("PPFD out = %g umol/m2/sec\n", out);
		logger_all("PPFD store = %g umol/m2/sec\n", store);
		logger_all("PPFD balance = %g umol/m2/sec\n",balance);
		logger_all("...FATAL ERROR IN 'Soil_model_daily' PPFD radiative balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin\n");
		logger_all("c->daily_litrC_to_soilC = %g gC/m2/day\n", c->daily_soilC);
		logger_all("\nout\n");
		logger_all("c->daily_het_resp = %g gC/m2/day\n",c->daily_het_resp);
		logger_all("\nstore\n");
		logger_all("c->soil1C = %g gC/m2/day\n", c->soil1C);
		logger_all("c->soil2C = %g gC/m2/day\n", c->soil2C);
		logger_all("c->soil3C = %g gC/m2/day\n", c->soil3C);
		logger_all("c->soil4C = %g gC/m2/day\n", c->soil4C);
		logger_all("\ncarbon in = %g gC/m2/day\n", in);
		logger_all("carbon out = %g gC/m2/day\n", out);
		logger_all("carbon store = %g gC/m2/day\n", store);
		logger_all("carbon_balance = %g gC/m2/day\n",balance);
		logger_all("...FATAL ERROR IN 'Soil_model_daily' carbon balance (exit)\n");

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
		logger_all("DOS = %d\n", c->dos);
		logger_all("\nin\n");
		logger_all("meteo_daily->rain = %g\n", meteo_daily->rain);
		logger_all("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		logger_all("\nout\n");
		logger_all("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		logger_all("c->daily_out_flow = %g\n", c->daily_out_flow);
		logger_all("c->daily_c_transp = %g\n", c->daily_c_transp);
		logger_all("\nstore (as a difference between old and current)\n");
		logger_all("c->asw = %g\n", c->asw);
		logger_all("soil water in = %g\n", in);
		logger_all("soil water out = %g\n", out);
		logger_all("soil water store = %g\n", store);
		logger_all("delta soil water balance = %g\n", store - old_store );
		logger_all("...FATAL ERROR IN 'Soil_model_daily' soil water balance (exit)\n");

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
