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
#include "het_respiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

//extern settings_t* g_settings;
extern logger_t* g_debug_log;

double heterotrophic_respiration_biome ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{
	double water_scalar;          /* soil water scalar */
	double temp_scalar;           /* soil temperature scalar */
	double rate_scalar;           /* final rate scalar as the product of the temperature and water scalars */
	double min_psi, max_psi;
	double TsoilK;
	double E0;                    /* activation-energy-type parameter of Lloyd and Taylor [1994] (K-1) */
	double T0;                    /* the lower temperature limit for the soil respiration (K) */


	/************************************************************************************************/

	/*** decompostion ***/

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

	return(rate_scalar);
}

void litter_heterotrophic_respiration_biome ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{
	double rate_scalar;
	double litt_decomp_rate1;     /* labile litter decomposition rate */
	double litt_decomp_rate2;     /* cellulose litter decomposition rate */
	double litt_decomp_rate4;     /* lignin litter decomposition rate */
	double pot_litr1C_loss;       /* daily labile litter carbon loss (gC/m2/day) */
	double pot_litr2C_loss;       /* daily cellulose litter carbon loss (gC/m2/day) */
	double pot_litr4C_loss;       /* daily lignin litter carbon loss (gC/m2/day) */
	double cn_litr1;              /* CN ratio of labile litter */
	double cn_litr2;              /* CN ratio of cellulose litter */
	double cn_litr4;              /* CN ratio of lignin litter */
	double ratio;

	rate_scalar = heterotrophic_respiration_biome ( c, meteo_daily );

	/* litter decomposition rate */
	litt_decomp_rate1     = KL1_BASE * rate_scalar;
	litt_decomp_rate2     = KL2_BASE * rate_scalar;
	litt_decomp_rate4     = KL4_BASE * rate_scalar;

	/* compute potential carbon loss */
	/* calculate the non-nitrogen limited fluxes between litter and	soil compartments. These will be ammended for N limitation if it turns
		out the potential gross immobilization is greater than potential gross	mineralization. */

	/* 1. labile litter to fast microbial recycling pool */
	if ( c->litr1C > 0. )
	{
		pot_litr1C_loss      = c->litr1C * litt_decomp_rate1;
	}

	/* 2. cellulose litter to medium microbial recycling pool */
	if ( c->litr2C > 0. )
	{
		pot_litr2C_loss      = c->litr2C * litt_decomp_rate2;
	}

	/* 3. lignin litter to slow microbial recycling pool */
	if ( c->litr4C > 0. )
	{
		pot_litr4C_loss      = c->litr4C * litt_decomp_rate4;
	}

	/* calculate C:N ratios */
	if (c->litr1N > 0.) cn_litr1 = c->litr1C / c->litr1N;
	if (c->litr2N > 0.) cn_litr2 = c->litr2C / c->litr2N;
	if (c->litr4N > 0.) cn_litr4 = c->litr4C / c->litr4N;

	/* calculate the non-nitrogen limited fluxes between litter and
	soil compartments. These will be ammended for N limitation if it turns
	out the potential gross immobilization is greater than potential gross
	mineralization. */

	/* 1 labile litter fluxes */
	if ( c->litr1C > 0. )
	{
		/* carbon */
		c->daily_litr1_het_resp   = pot_litr1C_loss * RFL1S1;
		c->daily_litr1C_to_soil1C = pot_litr1C_loss * ( 1. - RFL1S1 );
		/* nitrogen */
		if ( c->litr1N > 0. ) c->daily_litr1N_to_soil1N = pot_litr1C_loss * cn_litr1;
		else c->daily_litr1N_to_soil1N = 0.;
	}

	/* cellulose litter fluxes */
	if ( c->litr2C > 0. )
	{
		/* carbon */
		c->daily_litr2_het_resp   = pot_litr2C_loss * RFL2S2;
		c->daily_litr2C_to_soil2C = pot_litr2C_loss * ( 1. - RFL2S2 );
		/* nitrogen */
		if ( c->litr2N > 0. ) c->daily_litr2N_to_soil2N = pot_litr2C_loss * cn_litr2;
		else c->daily_litr2N_to_soil2N = 0.;
	}

	/* release of shielded cellulose litter, tied to the decay rate of lignin litter */
	if ( c->litr3C > 0. )
	{
		//fixme fixme fixme fixme miss competition (see biome)
		c->daily_litr3C_to_litr2C = litt_decomp_rate4 * c->litr3C;
	}

	/* lignin litter fluxes */
	if ( c->litr4C > 0. )
	{
		/* carbon */
		c->daily_litr4_het_resp   = pot_litr4C_loss * RFL4S3;
		c->daily_litr4C_to_soil3C = pot_litr4C_loss * ( 1. - RFL4S3 );
		/* nitrogen */
		if ( c->litr4N > 0. ) c->daily_litr4N_to_soil3N = pot_litr4C_loss * cn_litr4;
		else c->daily_litr4N_to_soil3N = 0.;
	}

	/* summarize litter heterotrophic respiration */
	c->daily_litr_het_resp = c->daily_litr1_het_resp + c->daily_litr2_het_resp + c->daily_litr4_het_resp;
	logger (g_debug_log, "c->daily_litr_het_resp = %g gC/m^2/day\n", c->daily_litr_het_resp);

	/* summarize carbon litter to soil */
	c->daily_litr_to_soilC = c->daily_litr1C_to_soil1C + c->daily_litr2C_to_soil2C + c->daily_litr4C_to_soil3C;
	logger (g_debug_log, "c->daily_litr_to_soilC = %g gC/m^2/day\n", c->daily_litr_to_soilC);

	/* summarize nitrogen litter to soil */
	c->daily_litr_to_soilN = c->daily_litr1N_to_soil1N + c->daily_litr2N_to_soil2N + c->daily_litr4N_to_soil3N;
	logger (g_debug_log, "c->daily_litr_to_soilN = %g gN/m^2/day\n", c->daily_litr_to_soilN);


	/* call heterotrophic_respiration */
	heterotrophic_respiration ( c );

	/*******************************************************************************************************************************/
}

void soil_heterotrophic_respiration_biome ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{
//	double rate_scalar;
//	double soil_decomp_rate1;     /* fast microbial recycling pool soil decomposition rate */
//	double soil_decomp_rate2;     /* fast microbial recycling pool soil decomposition rate */
//	double soil_decomp_rate3;     /* slow microbial recycling pool soil decomposition rate */
//	double soil_decomp_rate4;     /* recalcitrant SOM (humus) pool soil decomposition rate */

//	rate_scalar = heterotrophic_respiration_biome ( c, meteo_daily );

	/* soil decomposition rate */
//	soil_decomp_rate1     = KS1_BASE * rate_scalar;
//	soil_decomp_rate2     = KS2_BASE * rate_scalar;
//	soil_decomp_rate3     = KS3_BASE * rate_scalar;
//	soil_decomp_rate4     = KS4_BASE * rate_scalar;

	//heterotrophic_respiration ( c );
}

void heterotrophic_respiration ( cell_t *const c )
{

	/*******************************************************************************************************************************/

	/* total heterotrophic respiraton */

	/* daily */
	c->daily_het_resp        += c->daily_litr_het_resp;

	/* monthly */
	c->monthly_het_resp      += c->daily_litr_het_resp;

	/* annual */
	c->annual_het_resp       += c->daily_litr_het_resp;

	/*******************************************************************************************************************************/

}


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
