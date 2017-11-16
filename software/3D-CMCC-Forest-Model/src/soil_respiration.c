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

void soil_respiration_reichstein ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{
	double a;                     /* activation energy E0 parameter (K-1) */
	double b;                     /* activation energy E0 parameter (K-1) */
	double E0;                    /* activation-energy-type parameter of Lloyd and Taylor [1994] (K-1) */
	double water_scalar;
	double temp_scalar;
	double f;
	double T0;                    /* the lower temperature limit for the soil respiration (°C) */
	double Tref;                  /* reference temperature for respiration (°C) */
	double Tsoil;                 /* soil temperature (°C) */
	double daily_soil_resp_mol;   /* soil respiration, (umol m-2 s-1) */
	double Rref;                  /* reference soil respiration (at Tsoil = 18°C and non-limiting water) (umol m-2 s-1) */
	double a_lai;
	double b_lai;
	double Rswc;                  /* water content at half-maximal respiration */

	/* note: following Reichstein et al., (2003), Global Biogeochemical Cycles  */
	/* note: soil respiration includes both autotrophic and heterotrophic respiration */

	a     = 52.4;    /* see Reichstein et al., (2003) */
	b     = 285;     /* see Reichstein et al., (2003) */
	T0    = -46.02;  /* see Reichstein et al., (2003), Lloyd and Taylor (1994)  */
	Tref  = 20;      /* test 18 °C see Reichstein et al., (2003), test 15 °C see Hibbard et al., (2005) */
	a_lai = 0.6;     /* see Reichstein et al., (2003) */
	b_lai = 1.29;    /* see Reichstein et al., (2003) */
	Rswc  = 0.16;    /* see Reichstein et al., (2003) */

	/* Respiration references (based on LAI MAX) */
	Rref  = a_lai + b_lai * c->max_lai_proj;

	//todo: ask Carlo how change dynamically Rref!!!!

	Tsoil = meteo_daily->tsoil;

	/* compute activation energy */
	E0 = a + b * c->soil_moist_ratio;

	/* compute water scalar */
	water_scalar = c->soil_moist_ratio / ( Rswc + c->soil_moist_ratio );

	/* compute temperature scalar */
	f  = ( 1. / ( Tref - T0 ) ) - ( 1. / ( Tsoil - T0 ) );
	temp_scalar = exp( E0 * f );

	/* soil respiration in umol m-2 s-1 */
	daily_soil_resp_mol     = Rref * temp_scalar * water_scalar;

	/* soil respiration from umol m-2 sec-1 to gC m-2 d-1 */
	c->daily_soil_resp      = daily_soil_resp_mol *  GC_MOL  / 1e6 * 86400.;
	logger (g_debug_log, "c->daily_soil_resp = %g gC/m^2/day\n", c->daily_soil_resp);

	/* soil respiration flux from umol m-2 sec-1 to gCO2 m-2 day-1 */
	c->daily_soil_respCO2   = daily_soil_resp_mol * GCO2_MOL / 1e6 * 86400.;
	logger (g_debug_log, "c->daily_soil_respCO2 = %g gCO2/m^2/day\n", c->daily_soil_respCO2);

	/* monthly */
	c->monthly_soil_resp      += c->daily_soil_resp;
	c->monthly_soil_respCO2   += c->daily_soil_respCO2;

	/* annual */
	c->annual_soil_resp       += c->daily_soil_resp;
	c->annual_soil_respCO2    += c->daily_soil_respCO2;


}

void soil_respiration_biome ( cell_t *const c )
{
	/* daily */
	c->daily_soil_resp         = (c->daily_froot_aut_resp + c->daily_croot_aut_resp) + c->daily_het_resp;
	c->daily_soil_respCO2      = (c->daily_soil_resp / GC_MOL) * GCO2_MOL;

	/* monthly */
	c->monthly_soil_resp      += c->daily_soil_resp;
	c->monthly_soil_respCO2   += c->daily_soil_respCO2;

	/* annual */
	c->annual_soil_resp       += c->daily_soil_resp;
	c->annual_soil_respCO2    += c->daily_soil_respCO2;
}

void soil_respiration_canoak ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{

	double daily_soil_resp_mol;   /* soil respiration, umolC m-2 s-1 */
	double water_scalar;          /* soil water scalar */
	double temp_scalar;           /* soil temperature scalar */
	double min_psi, max_psi;

	/* note: water scalar from BIOME-BGC */
	/* calculate the rate constant scalar for soil water content.
	Uses the log relationship with water potential given in
	Andren, O., and K. Paustian, 1987. Barley straw decomposition in the field:
	a comparison of models. Ecology, 68(5):1190-1200.
	and supported by data in
	Orchard, V.A., and F.J. Cook, 1983. Relationship between soil respiration
	and soil moisture. Soil Biol. Biochem., 15(4):447-453.
	 */
	/* set the maximum and minimum values for water potential limits (MPa) */
	min_psi = -10.;
	max_psi = c->psi_sat;

	if (c->psi < min_psi)
	{
		/* no decomp below the minimum soil water potential */
		water_scalar = 0.;
	}
	else if (c->psi > max_psi)
	{
		/* this shouldn't ever happen, but just in case... */
		water_scalar = 1.;
	}
	else
	{
		water_scalar = log ( min_psi / c->psi ) / log ( min_psi / max_psi );
	}

#if 1
	water_scalar = c->soil_moist_ratio;
#endif

	/* note: water scalar from CANOAK */
	temp_scalar = exp( 51609. / 8.314 ) * ( ( 1. / TempAbs ) - 1. / ( meteo_daily->tsoil + TempAbs ) );


	/* based on CANOAK (Baldocchi et al.,) model (which I guess it assumes heterotrophic + root respiration) */
	/* After Hanson et al. 1993. Tree Physiol. 13, 1-15 */
	/* note: differently from CANOAK which assumes a constant soil wetness here we use a water_scalar variable */
	/* note: CANOAK doesn't account for the amount of litter or soil biomass */

	/* soil respiration in umol m-2 s-1 */
	daily_soil_resp_mol     = KL1_BASE * temp_scalar * water_scalar;

	/* soil respiration from umol m-2 sec-1 to gC m-2 d-1 */
	c->daily_soil_resp      = daily_soil_resp_mol *  GC_MOL  / 1e6 * 86400;
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




