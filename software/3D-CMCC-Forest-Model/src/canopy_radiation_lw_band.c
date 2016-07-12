/*
 * canopy_radiation_lw_band.c
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"
#include "canopy_temperature.h"
#include "canopy_radiation_lw_band.h"

extern logger_t* g_log;

//void canopy_lw_band_emit_trans_refl_radiation (cell_t *const c, species_t *const s, double Light_abs_frac, double Light_abs_frac_sun, double Light_abs_frac_shade, double Light_refl_par_frac, double Light_refl_sw_rad_canopy_frac)
//{
//	double leaf_cell_cover_eff;       /* effective fraction of leaf cover over the cell (ratio) */
//
//	/* note: This function works at class level computing absorbed transmitted and reflected PAR, NET RADIATION
//	 * and PPFD through different height * classes/layers considering at square meter takes into account coverage,
//	 * it means that a square meter grid cell * represents overall grid cell (see Duursma and Makela, 2007) */
//
//	/* it follows a little bit different rationale compared to BIOME-BGC approach
//	 * in BIOME_BGC:
//	 * apar = par * (1 - (exp(- K * LAI)));
//	 * apar_sun = par * (1 - (exp(- K * LAI_SUN)));
//	 * apar_shade = apar- apar_sun;
//	 *
//	 * in 3D-CMCC FEM:
//	 * apar_sun = par * (1 - (exp(- K * LAI_SUN)));
//	 * par_transm_sun  = par - apar_sun;
//	 * apar_shade = par_transm_sun * (1 - (exp(- K * LAI_SHADE)));
//	 * apar = apar_sun + apar_shade;
//	 *
//	 * then it consider that an amount of sunlit leaf are not completely outside the canopy
//	 * but there's an exponential decay of absorption also for sunlit foliage	 *
//	 */
//
//	/* compute effective canopy cover */
//	/* special case when LAI = < 1.0 */
//	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
//	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];
//
//	/* check for the special case in which is allowed to have more 100% of grid cell covered */
//	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
//
//
//}

void canopy_radiation_lw_band(species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int year, const int height, const int age, const int species)
{
	double LW_emis_canopy_frac, LW_emis_canopy_frac_sun, LW_emis_canopy_frac_shade;       /* (ratio) fraction of Long Wave radiation emissivity */
	const double LW_emis_soil_frac = 0.96;                                                /* (ratio) soil emissivity */
	//const double snow_emis = 0.97;                                                      /* (ratio) snow emissivity */

//	/* incoming LW from atmosphere */
//	double LW_atm_canopy_out; /* incoming lw and then out to atmosphere */
//	double LW_atm_trasm_canopy_to_soil; /* incoming lw transmitted through the canopy */
//	double LW_atm_trasm_canopy_to_soil_refl_soil; /* incoming lw transmitted through the canopy and reflected by soil */
//	double LW_atm_trasm_canopy_to_soil_refl_soil_trasm_canopy_to_atm; /* incoming lw transmitted through the canopy and reflected by soil and transmitted through canopy to atmosphere*/
//
//	/* lw emitted by the canopy to amsphere */
//	double LW_canopy_to_atm; /* emitted lw from canopy to atmosphere */
//
//	/* lw emitted by the canopy to the soil */
//	double LW_canopy_to_soil; /* emitted lw from canopy to soil */
//
//	/* lw emitted by the canopy */
//	double LW_canopy_temp_to_soil; /* increase-decrease lw due to increase-decrease canopy temp and emitted to soil */
//	double LW_canopy_temp_to_soil_refl_soil; /* increase-decrease lw due to increase-decrease canopy temp and emitted to soil and reflected by the soil */
//	double LW_canopy_temp_to_soil_refl_soil_trasm_canopy_to_atm; /* increase-decrease lw due to increase-decrease canopy temp and emitted to soil and reflected by the soil transmitted by the canopy to amosphere */
//
//	/* lw emitted by the soil */
//	double LW_canopy_soil_out;
//	double LW_soil_trasm_canopy_to_atm; /* lw emitted by the soil transmitted by the canopy to the atmosphere */

	/* global */
	double LW_canopy_out_atm;
	double LW_canopy_out_soil;
	double LW_soil_out_atm;

	double leaf_cell_cover_eff;                                                           /* (ratio) fraction of square meter covered by leaf over the grid cell */

	double TcanopyK;
	double TcanopyK_old;
	double TsoilK;

	double lw_out_canopy;
	double lw_out_soil;
	double lw_out;
	double net_lw;


	logger(g_log, "\n**LONG WAVE BAND RADIATION ROUTINE**\n");

	TsoilK = met[month].d[day].tsoil + TempAbs;


	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
	logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/***********************************************************************************************************/

	/* call canopy temperature function*/
	TcanopyK = canopy_temperature (s, c, met, day, month, year);

	/* LONG WAVE RADIATION FRACTIONS */
	/* CLM 4.5 APPROACH */
	/* fraction of long wave  emissivity by the canopy */
	LW_emis_canopy_frac = 1.0 - (exp(- s->value[LAI]));
	LW_emis_canopy_frac_sun = 1.0 - (exp(- s->value[LAI_SUN]));
	LW_emis_canopy_frac_shade = 1.0 - (exp(- s->value[LAI_SHADE]));

	logger(g_log, "LW_emis_frac (tot) = %g %%\n", LW_emis_canopy_frac);
	logger(g_log, "LW_emis_frac_sun = %g %%\n", LW_emis_canopy_frac_sun);
	logger(g_log, "LW_emis_frac_shade = %g %%\n", LW_emis_canopy_frac_shade);

	/***********************************************************************************************************/
	/* from clm 4.5 */
	/* compute LW coming from atmosphere and then out from the canopy to atmosphere */
//	LW_canopy_out_atm =
//			((1. - LW_emis_soil_frac) * (1. - LW_emis_canopy_frac) * (1. - LW_emis_canopy_frac) * met[month].d[day].lw_downward_W) +
//			(LW_emis_canopy_frac * SBC_W * pow(TcanopyK, 4.)) +
//			(LW_emis_canopy_frac * (1. - LW_emis_soil_frac) * (1. - LW_emis_canopy_frac) * SBC_W * pow (TcanopyK, 4.)) +
//			(4. * LW_emis_canopy_frac * SBC_W * pow(TcanopyK, 3.) * (TcanopyK_old - TcanopyK)) +
//			(4. * LW_emis_canopy_frac * (1. - LW_emis_soil_frac) * (1. - LW_emis_canopy_frac) * SBC_W * pow(TcanopyK, 3.) * (TcanopyK_old - TcanopyK)) +
//			(LW_emis_soil_frac * (1. - LW_emis_canopy_frac) * SBC_W * pow(TsoilK, 4.));
//
//	/* compute LW coming from atmosphere and then out from the canopy to below canopy */
//	LW_canopy_out_soil =
//			((1. - LW_emis_canopy_frac) * met[month].d[day].lw_downward_W) +
//			(LW_emis_canopy_frac * SBC_W * pow(TcanopyK, 4.)) +
//			(4. * LW_emis_canopy_frac * SBC_W * pow(TcanopyK, 3.) * (TcanopyK_old - TcanopyK));
//
//	/* compute LW out from the soil */
//	LW_soil_out_atm =
//			((1. - LW_emis_soil_frac) * met[month].d[day].lw_downward_W) +
//			(LW_emis_soil_frac * SBC_W * pow(TsoilK, 4.));

	/* emitted lw radiation from canopy */
	s->value[LW_RAD_EMIT] = (LW_emis_canopy_frac * SBC_W * pow(TcanopyK, 4.0));

	/* reflected lw radiation from canopy */
	s->value[LW_RAD_REFL] = (1. - LW_emis_canopy_frac) * met[month].d[day].lw_downward_W;

	/* net long wave radiation for canopy */
	s->value[NET_LW_RAD] = met[month].d[day].lw_downward_W - (s->value[LW_RAD_EMIT] + s->value[LW_RAD_REFL]);

	/* upward long-wave radiation from canopy */
	c->long_wave_radiation_upward_W = s->value[LW_RAD_EMIT] + s->value[LW_RAD_REFL];/* * leaf_cell_cover_eff*/
	logger(g_log, "Long Wave outgoing from canopy = %g W/m2\n", c->long_wave_radiation_upward_W);

//	lw_out_soil = LW_emis_soil_frac * SBC_W * pow(TsoilK, 4.0) /* * (1.0 -leaf_cell_cover_eff)*/;
//	logger(g_log, "lw soil out = %g W/m2\n", lw_out_soil);
//
//	lw_out = lw_out_canopy + lw_out_soil;
//	logger(g_log, "lw out = %g W/m2\n", lw_out);
//
//	net_lw = met[month].d[day].lw_downward_W - lw_out;
//	logger(g_log, "lw net_lw = %g W/m2\n", net_lw);

}
