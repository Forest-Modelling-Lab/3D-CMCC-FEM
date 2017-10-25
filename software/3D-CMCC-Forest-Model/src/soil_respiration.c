/*
 * soil_respiration.c
 *
 *  Created on: 25/set/2013
 *      Author: alessio
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_respiration.h"


extern logger_t* g_debug_log;

void soil_respiration( cell_t *const c, const meteo_daily_t *const meteo_daily )
{

	double soil_base_resp = 0.71; /* base rate of soil respiration, umol m-2 s-1 */
	double daily_soil_resp_mol;   /* soil respiration, umolCO2 m-2 s-1   */


	/* based on CANOAK (Baldocchi et al.,) model */
	/* After Hanson et al. 1993. Tree Physiol. 13, 1-15 */
	/* note: differently from CANOAK which assumes a constant soil wetness here we use soil moist ratio */

	//todo IS THIS SOIL RESPIRATION ADDED WITH ROOTS RESPIRATION?????

	/* soil respiration in umol m-2 s-1 */
	daily_soil_resp_mol     = soil_base_resp * exp( ( 51609. / 8.314 ) * ( ( 1. / TempAbs ) - 1. / ( meteo_daily->tsoil + TempAbs ) ) ) * c->soil_moist_ratio;

	/* soil respiration from umol m-2 sec-1 to gC m-2 d-1 */
	c->daily_soil_resp      = daily_soil_resp_mol *  GC_MOL / 1e6 * 86400;
	logger (g_debug_log, "c->daily_soil_resp = %g gC/m^2/day\n", c->daily_soil_resp);

	/* soil respiration flux from umol m-2 sec-1 to gCO2 m-2 day-1 */
	c->daily_soil_respCO2   = daily_soil_resp_mol * GCO2_MOL / 1e6 * 86400;
	logger (g_debug_log, "c->daily_soil_respCO2 = %g gCO2/m^2/day\n", c->daily_soil_respCO2);

	/* monthly */
	c->monthly_soil_resp      += c->daily_soil_resp;
	c->monthly_soil_respCO2   += c->daily_soil_respCO2;

	/* annual */
	c->annual_soil_resp       += c->daily_soil_resp;
	c->annual_soil_respCO2    += c->daily_soil_respCO2;
}
