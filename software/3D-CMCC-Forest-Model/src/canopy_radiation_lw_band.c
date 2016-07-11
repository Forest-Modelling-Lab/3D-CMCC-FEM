/*
 * canopy_radiation_lw_band.c
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "light.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"
#include "canopy_temperature.h"

extern logger_t* g_log;

void canopy_radiation_lw_band (species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species)
{
	double LW_emis_frac, LW_emis_frac_sun, LW_emis_frac_shade;                            /* (ratio) fraction of Long Wave radiation emissivity */
	double LW_abs_frac, LW_abs_frac_sun, LW_abs_frac_shade;                               /* (ratio) fraction of Long Wave radiation absorptivity */
	//const double soil_emis = 0.96;                                                        /* (ratio) soil emissivity */
	//const double snow_emis = 0.97;                                                        /* (ratio) snow emissivity */

	double leaf_cell_cover_eff;                                                           /* (ratio) fraction of square meter covered by leaf over the grid cell */

	double lw_out;

	logger(g_log, "\n**LONG WAVE BAND RADIATION ROUTINE**\n");

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
	logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/***********************************************************************************************************/

	/* call canopy temperature function*/
	canopy_temperature (s, c, met, day, month, year);

	/* LONG WAVE RADIATION FRACTIONS */
	/* CLM 4.0 APPROACH */
	/* fraction of long wave absorptivity through the canopy */
	LW_abs_frac = (exp(- s->value[LAI]));
	LW_abs_frac_sun = (exp(- s->value[LAI_SUN]));
	LW_abs_frac_shade = (exp(- s->value[LAI_SHADE]));

	/* fraction of long wave  emissivity by the canopy */
	LW_emis_frac = 1.0 - LW_abs_frac;
	LW_emis_frac_sun = 1.0 - LW_abs_frac_sun;
	LW_emis_frac_shade = 1.0 - LW_abs_frac_shade;

	logger(g_log, "LW_abs_frac (tot) = %g %%\n", LW_abs_frac);
	logger(g_log, "LW_abs_frac_sun = %g %%\n", LW_abs_frac_sun);
	logger(g_log, "LW_abs_frac_shade = %g %%\n", LW_abs_frac_shade);
	logger(g_log, "LW_emis_frac (tot) = %g %%\n", LW_emis_frac);
	logger(g_log, "LW_emis_frac_sun = %g %%\n", LW_emis_frac_sun);
	logger(g_log, "LW_emis_frac_shade = %g %%\n", LW_emis_frac_shade);

	/***********************************************************************************************************/

	//prova
	lw_out = (LW_emis_frac * SBC_W * pow(s->value[CANOPY_TEMPERATURE], 4.0) * leaf_cell_cover_eff);
	logger(g_log, "CANOPY_TEMPERATURE = %g K\n", s->value[CANOPY_TEMPERATURE]);
	logger(g_log, "prova = %g W/m2\n", lw_out);


}
