/*
 * litter_model.c
 *
 *  Created on: 22 mar 2017
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
#include "settings.h"
#include "decomposition.h"
#include "check_balance.h"
#include "meteo.h"

extern settings_t* g_settings;

int Litter_model(matrix_t *const m, const int cell, const int half_hour, const int hour, const int day, const int month, const int year)
{
	/* shortcuts */
	cell_t *c;
	meteo_daily_t *meteo_daily;
	
	assert(m);
	
	meteo_daily = NULL;

	/* assign shortcuts */
	c = &m->cells[cell];

#if 1
	meteo_daily = &m->cells[cell].years[year].daily[month].d[day];
	assert(meteo_daily);
	decomposition ( c, meteo_daily );
#else
	if ( DAILY == g_settings->time )
	{
		meteo_daily = &m->cells[cell].years[year].daily[month].d[day];
		assert(meteo_daily);
		decomposition ( c, meteo_daily );
	}
	else if ( HOURLY == g_settings->time )
	{
		//meteo_daily = &m->cells[cell].years[year].hourly[month].d[day].h[hour];		
		assert(1);
	}
	else if ( HALFHOURLY == g_settings->time )
	{
		//meteo_daily = m->cells[cell].years[year].halfhourly[month].d[day].h[hour].hh[half_hour];		
		assert(1);
	}
#endif
	/* check parameters */
	assert(meteo_daily);

	/* decomposition */
	decomposition ( c, meteo_daily );

	/*******************************************************************************************************/

	/* CHECK FOR BALANCE CLOSURE */

	/* CHECK FOR CARBON FLUX BALANCE CLOSURE */
	/* 1 */ if ( ! check_litter_carbon_flux_balance    ( c ) ) return 0;

	/* CHECK FOR CARBON MASS BALANCE CLOSURE */
	/* 2 */ if ( ! check_litter_carbon_mass_balance    ( c ) ) return 0;

	/* CHECK FOR NITROGEN FLUX BALANCE CLOSURE */
	/* 1 */ if ( ! check_litter_nitrogen_flux_balance  ( c ) ) return 0;

	/* CHECK FOR NITROGEN MASS BALANCE CLOSURE */
	/* 2 */ if ( ! check_litter_nitrogen_mass_balance  ( c ) ) return 0;

	/*******************************************************************************************************/

	/* ok */
	return 1;
}
