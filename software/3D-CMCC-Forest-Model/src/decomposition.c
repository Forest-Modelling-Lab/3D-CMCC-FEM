/*
 * decomposition.c
 *
 *  Created on: 13 mar 2017
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"


//extern settings_t* g_settings;
//extern logger_t* g_debug_log;

void decomposition (cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double soil_tempK;                           /* soil temperature (Kelvin) */
	double minpsi, maxpsi;                       /* minimum and maximum soil water potential limits (MPa) */

	double cn_l1,cn_l2,cn_l4,cn_s1,cn_s2,cn_s3,cn_s4;
	double rfl1s1, rfl2s2,rfl4s3,rfs1s2,rfs2s3,rfs3s4;
	double kl1_base,kl2_base,kl4_base,ks1_base,ks2_base,ks3_base,ks4_base,kfrag_base;
	double kl1,kl2,kl4,ks1,ks2,ks3,ks4,kfrag;

	/** following BIOME-BGC decomp.c file **/
	/* calculate the rate constant scalar for soil temperature,
	assuming that the base rate constants are assigned for non-moisture
	limiting conditions at 25 C. The function used here is taken from
	Lloyd, J., and J.A. Taylor, 1994. On the temperature dependence of
	soil respiration. Functional Ecology, 8:315-323.
	This equation is a modification of their eqn. 11, changing the base
	temperature from 10 C to 25 C, since most of the microcosm studies
	used to get the base decomp rates were controlled at 25 C. */

	/* check for soil temperature */
	if ( meteo_daily->tsoil < -10.0 )
	{
		/* no decomposition processes for tsoil < -10.0 C */
		c->tsoil_scalar= 0.0;
	}
	else
	{
		soil_tempK = meteo_daily->tsoil + 273.15;
		c->tsoil_scalar = exp(308.56*((1.0/71.02)-(1.0/(soil_tempK-227.13))));
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
	minpsi = -10.0;
	maxpsi = c->psi_sat;

	/* check for soil water */
	if (c->psi < minpsi)
	{
		/* no decomposition below the minimum soil water potential */
		c->wsoil_scalar = 0.0;
	}
	else if (c->psi > maxpsi)
	{
		/* this shouldn't ever happen, but just in case... */
		c->wsoil_scalar = 1.0;
	}
	else
	{
		c->wsoil_scalar = log(minpsi/c->psi)/log(minpsi/maxpsi);
	}

	/* calculate the final rate scalar as the product of the temperature and water scalars */
	c->rate_scalar = c->tsoil_scalar * c->wsoil_scalar;

	/* calculate compartment C:N ratios */
	//TODO
	/*
	if (ns->litr1n > 0.0) cn_l1 = cs->litr1c/ns->litr1n;
	if (ns->litr2n > 0.0) cn_l2 = cs->litr2c/ns->litr2n;
	if (ns->litr4n > 0.0) cn_l4 = cs->litr4c/ns->litr4n;
	*/
	cn_s1 = SOIL1_CN;
	cn_s2 = SOIL2_CN;
	cn_s3 = SOIL3_CN;
	cn_s4 = SOIL4_CN;

	/* respiration fractions for fluxes between compartments */
	rfl1s1 = RFL1S1;
	rfl2s2 = RFL2S2;
	rfl4s3 = RFL4S3;
	rfs1s2 = RFS1S2;
	rfs2s3 = RFS2S3;
	rfs3s4 = RFS3S4;

	/* calculate the corrected rate constants from the rate scalar and their
	base values. All rate constants are (1/day) */
	kl1_base = KL1_BASE;                             /* labile litter pool */
	kl2_base = KL2_BASE;                             /* cellulose litter pool */
	kl4_base = KL4_BASE;                             /* lignin litter pool */
	ks1_base = KS1_BASE;                             /* fast microbial recycling pool */
	ks2_base = KS2_BASE;                             /* medium microbial recycling pool */
	ks3_base = KS3_BASE;                             /* slow microbial recycling pool */
	ks4_base = KS4_BASE;                             /* recalcitrant SOM (humus) pool */
	kfrag_base = KFRAG_BASE;                         /* physical fragmentation of coarse woody debris */
	kl1 = kl1_base * c->rate_scalar;
	kl2 = kl2_base * c->rate_scalar;
	kl4 = kl4_base * c->rate_scalar;
	ks1 = ks1_base * c->rate_scalar;
	ks2 = ks2_base * c->rate_scalar;
	ks3 = ks3_base * c->rate_scalar;
	ks4 = ks4_base * c->rate_scalar;
	kfrag = kfrag_base * c->rate_scalar;

	/* calculate the flux from CWD to litter lignin and cellulose
	compartments, due to physical fragmentation */
	//TODO
	/*
	cwdc_loss = kfrag * cs->cwdc;
	cf->cwdc_to_litr2c = cwdc_loss * epc->deadwood_fucel;
	cf->cwdc_to_litr3c = cwdc_loss * epc->deadwood_fscel;
	cf->cwdc_to_litr4c = cwdc_loss * epc->deadwood_flig;
	nf->cwdn_to_litr2n = cf->cwdc_to_litr2c/epc->deadwood_cn;
	nf->cwdn_to_litr3n = cf->cwdc_to_litr3c/epc->deadwood_cn;
	nf->cwdn_to_litr4n = cf->cwdc_to_litr4c/epc->deadwood_cn;
	*/



}
