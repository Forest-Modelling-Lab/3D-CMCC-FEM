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
	double LW_transm_canopy_frac, LW_transm_canopy_frac_sun, LW_transm_canopy_frac_shade;       /* (ratio) fraction of Long Wave radiation transmittivity */
	double LW_abs_canopy_frac;
	//double LW_abs_canopy_frac_sun, LW_abs_canopy_frac_shade;         /* (ratio) fraction of Long Wave radiation absrptivity */
	//double LW_refl_canopy_frac, LW_refl_canopy_frac_sun, LW_refl_canopy_frac_shade;         /* (ratio) fraction of Long Wave radiation reflectivity */
	double leaf_cell_cover_eff;                                                           /* (ratio) fraction of square meter covered by leaf over the grid cell */

	double TsoilK;


	logger(g_log, "\n**LONG WAVE BAND RADIATION ROUTINE**\n");

	logger(g_log, "-ATMOSPHERE DOWNWARD LONGWAVE RADIATION = %g  (W/m2)\n", met[month].d[day].atm_lw_downward_W);

	TsoilK = met[month].d[day].tsoil + TempAbs;


	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
	logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/***********************************************************************************************************/

	/* LONG WAVE RADIATION FRACTIONS */
	/* fraction of long wave emissivity  by the canopy CLM 4.5 (eq. 4.20)  */
	/* for Dai et al., 2004 it represents also the absorbed */
	LW_emis_canopy_frac = (1. - exp(- s->value[LAI]));
	logger(g_log, "LW_emis_canopy_frac = %g %%\n", LW_emis_canopy_frac);

	/* fraction of long wave emissivity  by the canopy (see Gouttevin et al., 2015) */
	LW_abs_canopy_frac = (1. - exp(- s->value[LAI] * s->value[K]));
	logger(g_log, "LW_emis_canopy_frac = %g %%\n", LW_emis_canopy_frac);

	/* fraction of long wave transmissivity  by the canopy */
	LW_transm_canopy_frac = (1. - LW_emis_canopy_frac);
	logger(g_log, "LW_transm_canopy_frac = %g %%\n", LW_transm_canopy_frac);
	/***********************************************************************************************************/

	/* compute canopy upward and net long wave radiation */
	/* upward long wave fluxes to atmosphere */
	s->value[LW_RAD_EMIT] = (LW_emis_canopy_frac * SBC_W * pow(s->value[CANOPY_TEMP_K], 4.)) * leaf_cell_cover_eff;
	logger(g_log, "canopy emitted long wave fluxes to atmosphere = %g (W/m2)\n", s->value[LW_RAD_EMIT]);

	/* soil long wave emitted (general formula) */
	c->soil_long_wave_emitted = EMSOIL * SBC_W * pow(TsoilK, 4.);
	/***********************************************************************************************************/

	//fixme to be fixed for multilayered canopies
	/* net canopy long wave radiation CLM 4.5 (eq. 4.18) */
	s->value[NET_LW_RAD] =
			(2. - LW_emis_canopy_frac * (1.0 - EMSOIL)) *
			s->value[LW_RAD_EMIT] -
			(LW_emis_canopy_frac * c->soil_long_wave_emitted) -
			(LW_emis_canopy_frac * ((1. + (1. - EMSOIL) * (1 - LW_emis_canopy_frac)) * met[month].d[day].atm_lw_downward_W));
	logger(g_log, "net canopy long wave fluxes (CLM 4.5) = %g (W/m2)\n", s->value[NET_LW_RAD]);

	/* compute canopy downward long wave radiation (below the canopy) CLM 4.5 (eq. 4.16) */
	//fixme CLM uses for TcanopyK_old Tn+1
	//note: equation has been modified for canopy coverage
	s->value[LW_RAD_TRANSM] = (((1. - LW_emis_canopy_frac) * met[month].d[day].atm_lw_downward_W) +
			s->value[LW_RAD_EMIT]  + (4. * LW_emis_canopy_frac * SBC_W * pow (s->value[CANOPY_TEMP_K], 3.) *
			(s->value[CANOPY_TEMP_K_OLD] - s->value[CANOPY_TEMP_K])) * leaf_cell_cover_eff) +
			(met[month].d[day].atm_lw_downward_W * (1. - leaf_cell_cover_eff));
	logger(g_log, "long wave fluxes below the canopy (CLM 4.5) = %g (W/m2)\n", s->value[LW_RAD_TRANSM]);

	/* compute soil net long wave radiation CLM 4.5 (eq. 4.17) */
	c->net_lw_rad_for_soil = c->soil_long_wave_emitted - (EMSOIL * s->value[LW_RAD_TRANSM]) - (EMSOIL * met[month].d[day].atm_lw_downward_W);
	logger(g_log, "net soil long wave fluxes (CLM 4.5) = %g (W/m2)\n", c->net_lw_rad_for_soil);

	/***********************************************************************************************************/

	/* following Gouttevin et al., 2015 (including emissivity for leaf and soil)*/
//	/*original version*/
//	s->value[LW_RAD_ABS] = LW_abs_canopy_frac * (met[month].d[day].atm_lw_downward_W + lw_soil_emit - (2. * s->value[LW_RAD_EMIT]));
//	//logger(g_log, "abs canopy long wave fluxes (Gouttevin)= %g (W/m2)\n", s->value[LW_RAD_ABS]);
//	/*modified version*/
//	s->value[LW_RAD_ABS] = LW_abs_canopy_frac *(met[month].d[day].atm_lw_downward_W + lw_soil_emit - (2. * s->value[LW_RAD_EMIT])) * leaf_cell_cover_eff;
//	logger(g_log, "abs canopy long wave fluxes (Gouttevin-modified)= %g (W/m2)\n", s->value[LW_RAD_ABS]);


	/* from CLM 4.5 */
	/* compute LW coming from atmosphere and then out from the canopy to atmosphere */
//	double lw_atm_trasm_canopy_to_soil;
//	double lw_atm_trasm_canopy_to_soil_reflected_soil;
//	double lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm;
//	double lw_atm_temp;
//	double lw_canopy_emit_to_atm;
//	double lw_canopy_emit_to_soil;
//	double lw_canopy_emit_to_soil_reflected_to_soil;
//	double lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm;
//	double lw_canopy_emit_to_atm_delta;
//	double lw_canopy_emit_to_soil_delta;
//	double lw_canopy_emit_to_soil_delta_refl_soil;
//	double lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm;
//	double lw_soil_emit_to_canopy;
//	double lw_soil_emit_to_canopy_trasm_canopy_to_atm;
//	double lw_total_upward_to_atm;
//	double net_lw_canopy;
	//fixme it can be used ONLY FOR ONE LAYER CANOPY
//
//	/*** atmospheric long wave ***/
//	/* atmospheric long wave radiation transmitted through the canopy */
//	lw_atm_trasm_canopy_to_soil = met[month].d[day].atm_lw_downward_W * LW_transm_canopy_frac;
//	logger(g_log, "lw_atm_trasm_canopy_to_soil = %g (W/m2)\n", lw_atm_trasm_canopy_to_soil);
//
//	/* taking into account canopy coverage */
//	lw_atm_temp = (lw_atm_trasm_canopy_to_soil * leaf_cell_cover_eff) + (met[month].d[day].atm_lw_downward_W * (1. - leaf_cell_cover_eff));
//	logger(g_log, "lw_atm_temp = %g (W/m2)\n", lw_atm_temp);
//
//	/* atmospheric long wave radiation transmitted through the canopy and reflected by the soil */
//	lw_atm_trasm_canopy_to_soil_reflected_soil = lw_atm_temp * (1. - EMSOIL);
//	logger(g_log, "lw_atm_trasm_canopy_to_soil_reflected_soil = %g (W/m2)\n", lw_atm_trasm_canopy_to_soil_reflected_soil);
//
//	/* atmospheric long wave radiation transmitted through the canopy and reflected by the soil and transmitted through the canopy to atmosphere */
//	lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm = (lw_atm_trasm_canopy_to_soil_reflected_soil * LW_transm_canopy_frac * leaf_cell_cover_eff) +
//			((lw_atm_trasm_canopy_to_soil_reflected_soil) * (1. - leaf_cell_cover_eff));
//	logger(g_log, "lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm = %g (W/m2)\n", lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm);
//
//	/*** canopy  long wave ***/
//	/* canopy long wave emitted to atmosphere */
//	lw_canopy_emit_to_atm = (LW_emis_canopy_frac * SBC_W * pow(s->value[CANOPY_TEMP_K], 4.) * leaf_cell_cover_eff);
//	logger(g_log, "lw_canopy_emit_to_atm = %g (W/m2)\n", lw_canopy_emit_to_atm);
//
//	/* canopy long wave radiation emitted to soil */
//	lw_canopy_emit_to_soil = (LW_emis_canopy_frac * SBC_W * pow(s->value[CANOPY_TEMP_K], 4.) * leaf_cell_cover_eff);
//	logger(g_log, "lw_canopy_emit_to_soil = %g (W/m2)\n", lw_canopy_emit_to_soil);
//
//	/* canopy long wave radiation emitted by the canopy and reflected by the soil */
//	lw_canopy_emit_to_soil_reflected_to_soil = lw_canopy_emit_to_soil * (1. - EMSOIL);
//	logger(g_log, "lw_canopy_emit_to_soil_reflected_to_soil = %g (W/m2)\n", lw_canopy_emit_to_soil_reflected_to_soil);
//
//	/* canopy long wave radiation emitted by the canopy and reflected by the soil transmitted through the canopy to the atmosphere */
//	lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm = (lw_canopy_emit_to_soil_reflected_to_soil * LW_emis_canopy_frac * leaf_cell_cover_eff) +
//			((lw_canopy_emit_to_soil_reflected_to_soil ) * (1. - leaf_cell_cover_eff));
//	logger(g_log, "lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm = %g (W/m2)\n", lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm);
//
//	/* increase/decrease long wave emitted by the canopy due to an increment/decrement in canopy temperature to atmosphere */
//	//fixme CLM uses for TcanopyK_old Tn+1
//	lw_canopy_emit_to_atm_delta =  4. * LW_emis_canopy_frac * SBC_W * pow(s->value[CANOPY_TEMP_K_OLD], 3) * (s->value[CANOPY_TEMP_K_OLD] - s->value[CANOPY_TEMP_K]) * leaf_cell_cover_eff;
//	logger(g_log, "lw_canopy_emit_to_atm_delta = %g (W/m2)\n", lw_canopy_emit_to_atm_delta);
//
//	/* canopy increase/decrease long wave emitted due to an increment/decrement in canopy temperature to soil */
//	lw_canopy_emit_to_soil_delta =  (4. * LW_emis_canopy_frac * SBC_W * pow(s->value[CANOPY_TEMP_K_OLD], 3) * (s->value[CANOPY_TEMP_K_OLD] - s->value[CANOPY_TEMP_K])) * leaf_cell_cover_eff;
//	logger(g_log, "lw_canopy_emit_to_soil_delta = %g (W/m2)\n", lw_canopy_emit_to_soil_delta);
//
//	/* canopy increase/decrease long wave emitted due to an increment/decrement in canopy temperature to soil and reflect by the soil */
//	lw_canopy_emit_to_soil_delta_refl_soil =  lw_canopy_emit_to_soil_delta * ( 1. - EMSOIL);
//	logger(g_log, "lw_canopy_emit_to_soil_delta_refl_soil = %g (W/m2)\n", lw_canopy_emit_to_soil_delta_refl_soil);
//
//	/* canopy increase/decrease long wave emitted due to an increment/decrement in canopy temperature to soil and reflect by the soil transmitted though the canopy to atmosphere */
//	lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm = (lw_canopy_emit_to_soil_delta_refl_soil * LW_emis_canopy_frac * leaf_cell_cover_eff) +
//			((lw_canopy_emit_to_soil_delta_refl_soil ) * (1. - leaf_cell_cover_eff));
//	logger(g_log, "lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm = %g (W/m2)\n", lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm);
//
//	/* soil long wave emitted to canopy (or in absence of canopy to atmosphere)*/
//	lw_soil_emit_to_canopy = EMSOIL * SBC_W * pow(TsoilK, 4.);
//	logger(g_log, "lw_soil_emit_to_canopy = %g (W/m2)\n", lw_soil_emit_to_canopy);
//
//	/* soil long wave emitted to canopy and (if present) transmitted to the atmosphere */
//	lw_soil_emit_to_canopy_trasm_canopy_to_atm = (lw_soil_emit_to_canopy * LW_emis_canopy_frac * leaf_cell_cover_eff) +
//			((lw_soil_emit_to_canopy ) * (1. - leaf_cell_cover_eff));
//	logger(g_log, "lw_soil_emit_to_canopy_trasm_canopy_to_atm = %g (W/m2)\n", lw_soil_emit_to_canopy_trasm_canopy_to_atm);
//
//	/* total long wave upward to atmosphere */
//	logger(g_log, "**single upward component**\n"
//			"lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm = %g (W/m2)\n"
//			"lw_canopy_emit_to_atm = %g\n"
//			"lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm = %g\n"
//			"lw_canopy_emit_to_atm_delta = %g\n"
//			"lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm = %g\n"
//			"lw_soil_emit_to_canopy_trasm_canopy_to_atm = %g\n",
//			lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm,
//			lw_canopy_emit_to_atm,
//			lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm,
//			lw_canopy_emit_to_atm_delta,
//			lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm,
//			lw_soil_emit_to_canopy_trasm_canopy_to_atm);
//
//	lw_total_upward_to_atm =
//	/*1*/		lw_atm_trasm_canopy_to_soil_reflected_soil_transm_canopy_to_atm +
//	/*2*/		lw_canopy_emit_to_atm +
//	/*3*/		lw_canopy_emit_to_soil_reflected_to_soil_transm_canopy_to_atm +
//	/*4*/		lw_canopy_emit_to_atm_delta +
//	/*5*/		lw_canopy_emit_to_soil_delta_refl_soil_transm_canopy_to_atm +
//	/*6*/		lw_soil_emit_to_canopy_trasm_canopy_to_atm;
//	logger(g_log, "lw_total_upward_to_atm = %g (W/m2)\n", lw_total_upward_to_atm);
//
//	net_lw_canopy = met[month].d[day].atm_lw_downward_W - lw_total_upward_to_atm;
//	logger(g_log, "net_lw_canopy = %g (W/m2)\n", net_lw_canopy);

}
