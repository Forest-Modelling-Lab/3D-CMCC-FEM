#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mpfit.h"
#include "common.h"
#include "leaf_fall.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;


void leaf_fall_deciduous ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	static double leaf_to_remove;
	static double fine_root_to_remove;
	static double fraction_to_retransl = 0.1;           /* fraction of C to re-translocate (see Bossell et al., 2006 and Campioli et al., 2013 */
	double previousLai, currentLai;
	double previousBiomass_lai, newBiomass_lai;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL DECIDUOUS **\n");

	if(s->counter[LEAF_FALL_COUNTER] == 1)
	{
		logger(g_debug_log, "First day of Leaf fall\n");

		/* note: assuming that fine roots for deciduous species progressively die together with leaves */
		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */

		/* assign LAI values at the beginning of the sigmoid shape */
		s->value[MAX_LAI_PROJ] = s->value[LAI_PROJ];

		/* assign senescence doy */
		s->counter[SENESCENCE_DAY_ONE] = c->doy;
		previousLai = s->value[LAI_PROJ];

		/* sigmoid shape drives LAI reduction during leaf fall */
		currentLai = MAX(0,s->value[MAX_LAI_PROJ] / (1 + exp(-(s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAY_ONE] - c->doy)
				/(s->counter[DAY_FRAC_FOLIAGE_REMOVE] / (log(9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAY_ONE]) -
						log(.11111111111))))));

		/* check */
		CHECK_CONDITION(previousLai, <, currentLai);

		/* update leaf carbon */
		previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);
		newBiomass_lai      = currentLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);
		leaf_to_remove      = previousBiomass_lai - newBiomass_lai;

		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = ( s->value[FROOT_C] * leaf_to_remove) / s->value[LEAF_C];
	}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* reconcile leaf and fine root */
		leaf_fall ( s, leaf_to_remove, fine_root_to_remove, fraction_to_retransl);

		/* update litter pool */
		littering ( c, s );
	}
}

void leaf_fall_evergreen ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	static double leaf_to_remove;
	static double fine_root_to_remove;
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL (turnover) EVERGREEN**\n");

	/************************************************************************************************************/

	if ( c->doy == 1 )
	{
		/* daily leaf fall turnover rate */
		leaf_to_remove              = (s->value[LEAF_C] * s->value[LEAF_FROOT_TURNOVER]) / 365;

		/* daily fine root turnover rate */
		fine_root_to_remove         = (s->value[FROOT_C] * s->value[LEAF_FROOT_TURNOVER]) / 365;
	}

	/*************************************************************************************************************/

	/* reconcile leaf and fine root */
	leaf_fall ( s, leaf_to_remove, fine_root_to_remove, fraction_to_retransl);

	/* update litter pool */
	littering ( c, s );

}

void leaf_fall (species_t *const s, double leaf_to_remove, double fine_root_to_remove, double fraction_to_retransl)
{

	/* compute fluxes of carbon leaf and fine root pool */
	s->value[C_LEAF_TO_LITR]     = leaf_to_remove * ( 1. - fraction_to_retransl);
	s->value[C_FROOT_TO_LITR]    = fine_root_to_remove * ( 1. - fraction_to_retransl);
	s->value[C_LEAF_TO_RESERVE]  = leaf_to_remove * fraction_to_retransl;
	s->value[C_FROOT_TO_RESERVE] = fine_root_to_remove * fraction_to_retransl;

	/* for evergreen to balance leaf_C in and out */
	s->value[C_TO_LEAF]         -= s->value[C_LEAF_TO_LITR];
	s->value[C_TO_FROOT]        -= s->value[C_FROOT_TO_LITR];

	/* update carbon leaf and fine root pools */
	s->value[LEAF_C]            -= leaf_to_remove;
	s->value[FROOT_C]           -= fine_root_to_remove;


	//todo Nitrogen fluxes


	/* check */
	CHECK_CONDITION(s->value[LEAF_C], <, 0.);
	CHECK_CONDITION(s->value[FROOT_C], <, 0.);
}

void littering ( cell_t *const c, species_t *const s )
{

	/* leaf and fine root daily litter production */

	/** carbon littering **/
	/* carbon litter transfer to carbon litter pool */
	s->value[C_TO_LITR]         = s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR];

	/* leaf litter carbon */
	s->value[C_LEAF_TO_LITR1C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_LAB_FRAC];
	s->value[C_LEAF_TO_LITR2C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[C_LEAF_TO_LITR3C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[C_LEAF_TO_LITR4C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_LIGN_FRAC];

	/* fine root litter carbon */
	s->value[C_FROOT_TO_LITR1C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_LAB_FRAC];
	s->value[C_FROOT_TO_LITR2C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[C_FROOT_TO_LITR3C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[C_FROOT_TO_LITR4C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_LIGN_FRAC];

	/* cell level litter carbon */
	c->daily_leaf_litrC        += s->value[C_LEAF_TO_LITR];
	c->daily_froot_litrC       += s->value[C_FROOT_TO_LITR];
	c->daily_leaf_litr1C       += s->value[C_LEAF_TO_LITR1C];
	c->daily_leaf_litr2C       += s->value[C_LEAF_TO_LITR2C];
	c->daily_leaf_litr3C       += s->value[C_LEAF_TO_LITR3C];
	c->daily_leaf_litr4C       += s->value[C_LEAF_TO_LITR4C];
	c->daily_froot_litr1C      += s->value[C_FROOT_TO_LITR1C];
	c->daily_froot_litr2C      += s->value[C_FROOT_TO_LITR2C];
	c->daily_froot_litr3C      += s->value[C_FROOT_TO_LITR3C];
	c->daily_froot_litr4C      += s->value[C_FROOT_TO_LITR4C];
	c->daily_litrC             += (c->daily_leaf_litrC + c->daily_froot_litrC);

	/** nitrogen littering **/
	/* nitrogen litter transfer to nitrogen litter pool */
	/* convert carbon amount to nitrogen amount */
	s->value[N_LEAF_TO_LITR]    = s->value[C_LEAF_TO_LITR] / s->value[CN_FALLING_LEAVES];
	s->value[N_FROOT_TO_LITR]   = s->value[N_FROOT_TO_LITR] / s->value[CN_FINE_ROOTS];
	s->value[N_TO_LITR]         = s->value[N_LEAF_TO_LITR] + s->value[N_FROOT_TO_LITR];

	/* leaf litter nitrogen */
	s->value[N_LEAF_TO_LITR1N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_LAB_FRAC];
	s->value[N_LEAF_TO_LITR2N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[N_LEAF_TO_LITR3N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[N_LEAF_TO_LITR4N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_LIGN_FRAC];

	/* fine root litter carbon */
	s->value[N_FROOT_TO_LITR1N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_LAB_FRAC];
	s->value[N_FROOT_TO_LITR2N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[N_FROOT_TO_LITR3N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[N_FROOT_TO_LITR4N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_LIGN_FRAC];

	/* cell level litter nitrogen */
	c->daily_leaf_litrN        += s->value[N_LEAF_TO_LITR];
	c->daily_froot_litrN       += s->value[N_FROOT_TO_LITR];
	c->daily_leaf_litr1N       += s->value[N_LEAF_TO_LITR1N];
	c->daily_leaf_litr2N       += s->value[N_LEAF_TO_LITR2N];
	c->daily_leaf_litr3N       += s->value[N_LEAF_TO_LITR3N];
	c->daily_leaf_litr4N       += s->value[N_LEAF_TO_LITR4N];
	c->daily_froot_litr1N      += s->value[N_FROOT_TO_LITR1N];
	c->daily_froot_litr2N      += s->value[N_FROOT_TO_LITR2N];
	c->daily_froot_litr3N      += s->value[N_FROOT_TO_LITR3N];
	c->daily_froot_litr4N      += s->value[N_FROOT_TO_LITR4N];
	c->daily_litrN             += (c->daily_leaf_litrN + c->daily_froot_litrN);
}




