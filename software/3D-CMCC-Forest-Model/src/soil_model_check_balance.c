/*
 * soil_model_check_balance.c
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
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("incoming par = %g molPAR/m2/day\n", meteo_daily->par);
		printf("\nout\n");
		printf("c->par_refl_soil = %g molPAR/m2/day\n",c->par_refl_soil);
		printf("c->par_refl_snow = %g molPAR/m2/day\n",c->par_refl_snow);
		printf("\nstore\n");
		printf("c->par_for_soil = %g molPAR/m2/day\n", c->apar_soil);
		printf("c->par_for_snow = %g molPAR/m2/day\n", c->apar_snow);
		printf("\npar in = %g molPAR/m2/day\n", in);
		printf("par out = %g molPAR/m2/day\n", out);
		printf("par store = %g molPAR/m2/day\n", store);
		printf("par balance = %g molPAR/m2/day\n",balance);
		printf("...FATAL ERROR SOIL PAR radiative balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok SOIL PAR radiative balance\n");
	}

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
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("incoming radiation = %g W/m2\n", meteo_daily->sw_downward_W);
		printf("\nout\n");
		printf("c->sw_rad_refl_soil = %g W/m2\n",c->sw_rad_refl_soil);
		printf("c->sw_rad_refl_snow = %g W/m2\n",c->sw_rad_refl_snow);
		printf("\nstore\n");
		printf("c->net_sw_rad_for_soil = %g W/m2\n", c->sw_rad_abs_soil);
		printf("\nradiation in = %g W/m2\n", in);
		printf("radiation out = %g W/m2\n", out);
		printf("net radiation store = %g W/m2\n", store);
		printf("radiation balance = %g W/m2\n",balance);
		printf("...FATAL ERROR IN SOIL Short Wave Radiation radiative balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok SOIL Short Wave Radiation radiative balance\n");
	}

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
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("incoming PPFD = %g umol/m2/sec\n", meteo_daily->ppfd);
		printf("\nout\n");
		printf("c->ppfd_refl_soil = %g umol/m2/sec\n",c->ppfd_refl_soil);
		printf("c->ppfd_refl_snow = %g umol/m2/sec\n",c->ppfd_refl_snow);
		printf("\nstore\n");
		printf("c->ppfd_for_soil = %g umol/m2/sec\n", c->ppfd_abs_soil);
		printf("\nPPFD in = %g umol/m2/sec\n", in);
		printf("PPFD out = %g umol/m2/sec\n", out);
		printf("PPFD store = %g umol/m2/sec\n", store);
		printf("PPFD balance = %g umol/m2/sec\n",balance);
		printf("...FATAL ERROR IN SOIL PPFD radiative balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok SOIL PPFD radiative balance\n");
	}


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
	in = c->daily_soilC;

	/* sum of carbon sinks */
	out = c->daily_het_resp;

	/* sum of current carbon storage */
	store = c->soil1C +
			c->soil2C +
			c->soil3C +
			c->soil4C ;

	balance = in - out -store;

	logger(g_debug_log, "\nSOIL CARBON FLUX BALANCE\n");

	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("c->daily_litrC_to_soilC = %g gC/m2/day\n", c->daily_soilC);
		printf("\nout\n");
		printf("c->daily_het_resp = %g gC/m2/day\n",c->daily_het_resp);
		printf("\nstore\n");
		printf("c->soil1C = %g gC/m2/day\n", c->soil1C);
		printf("c->soil2C = %g gC/m2/day\n", c->soil2C);
		printf("c->soil3C = %g gC/m2/day\n", c->soil3C);
		printf("c->soil4C = %g gC/m2/day\n", c->soil4C);
		printf("\ncarbon in = %g gC/m2/day\n", in);
		printf("carbon out = %g gC/m2/day\n", out);
		printf("carbon store = %g gC/m2/day\n", store);
		printf("carbon_balance = %g gC/m2/day\n",balance);
		printf("...FATAL ERROR IN SOIL carbon balance (exit)\n");
		printf("DOY = %d\n", c->doy);
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok SOIL carbon balance\n");
	}

	return 1;
}

int check_soil_water_flux_balance(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK SOIL SOIL WATER BALANCE************\n");
	/* DAILY CHECK ON SOIL POOL-ATMOSPHERE WATER BALANCE */
	/* it takes into account soil-atmosphere fluxes */

	/* sum of sources (rain + snow) */
	in = meteo_daily->rain + c->daily_snow_melt;

	/* sum of sinks */
	out = c->daily_soil_evapo + c->daily_out_flow;

	/* sum of current storage in soil */
	store = c->asw;

	/* check soil pool water balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCELL SOIL POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( balance ) > eps ) && (c->dos > 1) )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin\n");
		printf("meteo_daily->rain = %g\n", meteo_daily->rain);
		printf("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		printf("\nout\n");
		printf("c->daily_c_transp = %g\n", c->daily_c_transp);
		printf("c->daily_c_int = %g\n", c->daily_c_rain_int);
		printf("c->soil_evaporation = %g\n", c->daily_soil_evapo);
		printf("c->daily_out_flow = %g\n", c->daily_out_flow);
		printf("\nstore (as a difference between old and current)\n");
		printf("c->daily_c_water_stored = %g\n", c->daily_c_water_stored);
		printf("c->asw = %g\n", c->asw);
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
	out = c->daily_snow_melt + c->daily_snow_subl;

	/* sum of current storage in snow pack */
	store = c->snow_pack;

	/* check snow pool water balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCELL SNOW POOL WATER BALANCE\n");

	/* check for soil water pool water balance */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		printf("DOY = %d\n", c->dos);
		printf("\nin\n");
		printf("meteo_daily->snow = %g\n", meteo_daily->snow);
		printf("\nout\n");
		printf("c->daily_snow_subl = %g\n", c->daily_snow_subl);
		printf("c->daily_snow_melt = %g\n", c->daily_snow_melt);
		printf("\nstore (as a difference between old and current)\n");
		printf("delta c->asw = %g\n", c->snow_pack);
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
