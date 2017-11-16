/*
 * het_respiration.c
 *
 *  Created on: 15 mar 2017
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aut_respiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

//extern settings_t* g_settings;
extern logger_t* g_debug_log;

void heterotrophic_respiration_reichstein(cell_t *const c)
{

	/* heterotrophic respiration computed as a differences soil_respiration - fine + coarse root respiration */
	c->daily_het_resp = c->daily_soil_resp - (c->daily_froot_aut_resp + c->daily_croot_aut_resp);
	logger (g_debug_log, "c->daily_het_resp = %g gC/m^2/day\n", c->daily_het_resp);

	/* monthly */
	c->monthly_het_resp += c->daily_het_resp;

	/* annual */
	c->annual_het_resp += c->daily_het_resp;

}

void heterotrophic_respiration_biome ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{
	double water_scalar;          /* soil water scalar */
	double temp_scalar;           /* soil temperature scalar */
	double rate_scalar;           /* final rate scalar as the product of the temperature and water scalars */
	double min_psi, max_psi;
	double TsoilK;
	double decomp_rate1;
	double decomp_rate2;
	double decomp_rate3;
	double decomp_rate4;
	double E0;                    /* activation-energy-type parameter of Lloyd and Taylor [1994] (K-1) */
	double T0;                    /* the lower temperature limit for the soil respiration (K) */
	double litr1C_lost;            /* daily litter carbon lost (gC/m2/day) */
	double litr2C_lost;            /* daily litter carbon lost (gC/m2/day) */
	double litr3C_lost;            /* daily litter carbon lost (gC/m2/day) */
	double litr4C_lost;            /* daily litter carbon lost (gC/m2/day) */

	E0 = 308.56;
	T0 = 227.13;

	/* calculate the rate constant scalar for soil temperature,
	assuming that the base rate constants are assigned for non-moisture
	limiting conditions at 25 C. The function used here is taken from
	Lloyd, J., and J.A. Taylor, 1994. On the temperature dependence of
	soil respiration. Functional Ecology, 8:315-323.
	This equation is a modification of their eqn. 11, changing the base
	temperature from 10 C to 25 C, since most of the microcosm studies
	used to get the base decomp rates were controlled at 25 C.
	note: this implies to change value value from 56.02 to 71.02 K */

	if ( meteo_daily->tsoil < -10. )
	{
		/* no decomposition processes for tsoil < -10.0 C */
		temp_scalar = 0.0;
	}
	else
	{
		TsoilK      = meteo_daily->tsoil + TempAbs;
		temp_scalar = exp ( E0 * ( ( 1. / 71.02 ) - ( 1. / ( TsoilK - T0 ) ) ) );
	}

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
		/* no respiration below the minimum soil water potential */
		water_scalar = 0.;
	}
	else if ( c->psi > max_psi )
	{
		/* this shouldn't ever happen, but just in case... */
		water_scalar = 1.;
	}
	else
	{
		water_scalar = log ( min_psi / c->psi ) / log ( min_psi / max_psi );
	}

#if 0
	water_scalar = c->soil_moist_ratio;
#endif

	/* calculate the final rate scalar as the product of the temperature and water scalars */
	rate_scalar     = temp_scalar * water_scalar;

	/* soil decomposition rate */
	decomp_rate1     = KL1_BASE * rate_scalar;
	decomp_rate2     = KL2_BASE * rate_scalar;
	decomp_rate3     = KL2_BASE * rate_scalar;
	decomp_rate4     = KL4_BASE * rate_scalar;

	/* compute carbon lost */
	litr1C_lost      = c->litr1C * decomp_rate1;
	litr2C_lost      = c->litr2C * decomp_rate2;
	litr3C_lost      = c->litr3C * decomp_rate3;
	litr4C_lost      = c->litr4C * decomp_rate4;

	/* remove from daily litter carbon flux carbon lost for decomposition */
	//c->daily_litrC -= litrC_lost;

	/* remove from litter carbon pool daily litter carbon fluxes */
	//c->litrC       -= litrC_lost;

	//note: a much simplified approach that use all litter (as in Reichestein et al., 2003) instead the labile fraction as in BIOME-BGC
	//fixme fixme fixme fixme fixme
	/* litter heterotrophic respiration gC/m2/day */
	c->daily_litr_het_resp    = (litr1C_lost * RFL1S1) + (litr2C_lost * RFL2S2) + (litr3C_lost * RFL2S2) + (litr1C_lost * RFL4S3);



	/* fraction of un-decomposed litter that goes to soil pool */
	//c->daily_soilC           += litrC_lost * ( 1. - RFL1S1 );

	/* add to soil carbon pool daily litter carbon fluxes */
	c->soilC                 += c->daily_soilC;

	logger (g_debug_log, "c->daily_litr_het_resp = %g gC/m^2/day\n", c->daily_litr_het_resp);

	/*******************************************************************************************************************************/

	/* total heterotrophic respiraton */

	/* daily */
	c->daily_het_resp        += c->daily_litr_het_resp;

	/* monthly */
	c->monthly_het_resp      += c->daily_litr_het_resp;

	/* annual */
	c->annual_het_resp       += c->daily_litr_het_resp;

}
