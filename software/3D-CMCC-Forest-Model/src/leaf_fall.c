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
	static double foliage_to_remove;
	static double fine_root_to_remove;
	static double C_fraction_to_retransl = 0.1; /* fraction of C to re-translocate (see Bossell et al., 2006 and Campioli et al., 2013 */
	double previousLai, currentLai;
	double previousBiomass_lai, newBiomass_lai;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL DECIDUOUS **\n");

	logger(g_debug_log, "Leaf fall counter = %d\n", s->counter[LEAF_FALL_COUNTER]);
	logger(g_debug_log, "Leaf Carbon = %f\n", s->value[LEAF_C]);
	logger(g_debug_log, "Fine root Carbon  = %f\n", s->value[FROOT_C]);

	if(s->counter[LEAF_FALL_COUNTER] == 1)
	{
		logger(g_debug_log, "First day of Leaf fall\n");
		logger(g_debug_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

		/* note: assuming that fine roots for deciduous species progressively die together with leaves */
		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */

		/* assign LAI values at the beginning of the sigmoid shape */
		s->value[MAX_LAI_PROJ] = s->value[LAI_PROJ];

		/* assign senescence doy */
		s->counter[SENESCENCE_DAY_ONE] = c->doy;
	}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* following Campioli et al., 2013 and Bossel 1996 10% of leaf and fine root Carbon is daily re-translocated as reserve in the reserve pool */
		/* compute amount of leaf and fine root Carbon to re-translocate as reserve */

		/* retranslocating carbon and nitrogen */
		s->value[C_LEAF_TO_RESERVE]  = (s->value[LEAF_C] * C_fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		s->value[N_LEAF_TO_RESERVE]  = s->value[C_LEAF_TO_RESERVE]/s->value[CN_LEAVES];

		s->value[C_FROOT_TO_RESERVE] = (s->value[FROOT_C] * C_fraction_to_retransl) /s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		s->value[N_FROOT_TO_RESERVE] = s->value[C_FROOT_TO_RESERVE]/s->value[CN_FINE_ROOTS];

		previousLai = s->value[LAI_PROJ];

		/* sigmoid shape drives LAI reduction during leaf fall */
		currentLai = MAX(0,s->value[MAX_LAI_PROJ] / (1 + exp(-(s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAY_ONE] -
				c->doy)/(s->counter[DAY_FRAC_FOLIAGE_REMOVE] / (log(9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAY_ONE]) -
						log(.11111111111))))));
		logger(g_debug_log, "previousLai = %f\n", previousLai);
		logger(g_debug_log, "currentLai = %f\n", currentLai);

		/* check */
		CHECK_CONDITION(previousLai, <, currentLai);

		previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);

		newBiomass_lai      = (currentLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0));

		foliage_to_remove   = previousBiomass_lai - newBiomass_lai;


		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = (s->value[FROOT_C]*foliage_to_remove)/s->value[LEAF_C];
		logger(g_debug_log, "fineroot_to_remove = %f\n", fine_root_to_remove);

		/* update leaf falling */
		s->value[C_TO_LEAF]        = -foliage_to_remove ;
		s->value[C_TO_FINEROOT]    = -fine_root_to_remove;
		s->value[C_LEAF_TO_LITR]   = (foliage_to_remove - s->value[C_LEAF_TO_RESERVE]);
		s->value[C_FROOT_TO_LITR]  = (fine_root_to_remove - s->value[C_FROOT_TO_RESERVE]);
		s->value[N_TO_LEAF]        = -foliage_to_remove ;
		s->value[N_TO_FINEROOT]    = -fine_root_to_remove;
		s->value[N_LEAF_TO_LITR]   = (foliage_to_remove - s->value[N_LEAF_TO_RESERVE]);
		s->value[N_FROOT_TO_LITR]  = (fine_root_to_remove - s->value[N_FROOT_TO_RESERVE]);
	}
	else
	{
		/** LAST DAY OF LEAF FALL **/
		logger(g_debug_log, "Last day of leaf fall\n");
		s->value[C_TO_LEAF] = -s->value[LEAF_C];
		s->value[C_TO_FINEROOT] = - s->value[FROOT_C];

		/* assuming that the last day of leaf fall all the remaining carbon goes to reserve */
		s->value[C_LEAF_TO_RESERVE]      = s->value[LEAF_C];
		s->value[C_FROOT_TO_RESERVE]     = s->value[FROOT_C];
		s->value[C_LEAF_TO_LITR]         = 0.;
		s->value[C_FROOT_TO_LITR]        = 0.;
		s->value[N_LEAF_TO_RESERVE]      = s->value[LEAF_N];
		s->value[N_FROOT_TO_RESERVE]     = s->value[FROOT_N];
		s->value[N_LEAF_TO_LITR]         = 0.;
		s->value[N_FROOT_TO_LITR]        = 0.;
	}

	/* update litter pool */
	littering ( c, s );

}

void leaf_fall_evergreen ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	static double foliage_to_remove;
	static double fine_root_to_remove;
	double yearly_leaf_fall_falling_C;
	double yearly_fine_root_turnover_C;
	static double C_fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL (turnover) EVERGREEN**\n");

	/************************************************************************************************************/

	if ( c->doy == 1 )
	{
		/* compute annual carbon leaf turnover */
		yearly_leaf_fall_falling_C  = s->value[LEAF_C] * s->value[LEAF_FINEROOT_TURNOVER];

		/* daily leaf fall */
		foliage_to_remove           = yearly_leaf_fall_falling_C / 365;

		/* compute carbon fine root turnover */
		yearly_fine_root_turnover_C = s->value[FROOT_C] * s->value[LEAF_FINEROOT_TURNOVER];

		/* daily fine root turnover */
		fine_root_to_remove         = yearly_fine_root_turnover_C / 365;

		logger(g_debug_log, "Annual leaf turnover = %g tC/cell/year\n", yearly_leaf_fall_falling_C);
		logger(g_debug_log, "Daily leaf turnover = %g tC/cell/year\n", foliage_to_remove);
		logger(g_debug_log, "Annual fine root turnover = %g tC/cell/year\n", yearly_fine_root_turnover_C);
		logger(g_debug_log, "Daily fine root turnover = %g tC/cell/year\n", fine_root_to_remove);
	}

	/*************************************************************************************************************/

	/* update carbon leaf pool */
	s->value[LEAF_C] -= foliage_to_remove;
	logger(g_debug_log, "LEAF_C = %f tC/cell/day\n", s->value[LEAF_C]);

	/* compute daily amount of carbon leaf and fine root to remove */
	s->value[C_LEAF_TO_LITR] = foliage_to_remove ;
	logger(g_debug_log, "Daily leaf turnover = %g tC/cell/day\n", s->value[C_LEAF_TO_LITR]);

	/* update carbon fine root pool */
	s->value[FROOT_C] -= fine_root_to_remove;
	logger(g_debug_log, "FINE_ROOT_C = %g tC/cell/day\n", s->value[FROOT_C]);

	s->value[C_FROOT_TO_LITR] = fine_root_to_remove;
	logger(g_debug_log, "Daily fine root turnover = %g tC/cell/day\n", s->value[C_FROOT_TO_LITR]);

	logger(g_debug_log, "Daily Carbon turnover to litter before retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR]);

	/* compute daily amount of carbon to re-translocate before remove leaf and fine root */
	s->value[C_LEAF_TO_RESERVE] = s->value[C_LEAF_TO_LITR] * C_fraction_to_retransl;
	logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %g tC/cell/day\n", s->value[C_LEAF_TO_RESERVE]);

	s->value[C_FROOT_TO_RESERVE] = s->value[C_FROOT_TO_LITR] * C_fraction_to_retransl;
	logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %g tC/cell/day\n", s->value[C_FROOT_TO_RESERVE]);

	/* update considering carbon retranslocation */
	s->value[C_LEAF_TO_LITR] -= s->value[C_LEAF_TO_RESERVE];
	logger(g_debug_log, "Daily leaf turnover after retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITR]);

	s->value[C_FROOT_TO_LITR] -= s->value[C_FROOT_TO_RESERVE];
	logger(g_debug_log, "Daily fine root turnover after retranslocation = %g tC/cell/day\n", s->value[C_FROOT_TO_LITR]);

	/**/
	s->value[C_TO_LEAF]        -= s->value[C_LEAF_TO_LITR];
	s->value[C_TO_FINEROOT]    -= s->value[C_FROOT_TO_LITR];

	/* update litter pool */
	littering ( c, s );


}

void littering ( cell_t *const c, species_t *const s )
{

	/* leaf and fine root daily litter production */

	/** carbon littering **/
	/* carbon litter transfer to carbon litter pool */
	s->value[C_TO_LITR]         = s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR];

	/* leaf litter carbon */
	s->value[C_LEAF_TO_LITR1C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITT_LAB_FRAC];
	s->value[C_LEAF_TO_LITR2C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITT_USCEL_FRAC];
	s->value[C_LEAF_TO_LITR3C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITT_SCEL_FRAC];
	s->value[C_LEAF_TO_LITR4C]  = s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITT_LIGN_FRAC];

	/* fine root litter carbon */
	s->value[C_FROOT_TO_LITR1C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITT_LAB_FRAC];
	s->value[C_FROOT_TO_LITR2C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITT_USCEL_FRAC];
	s->value[C_FROOT_TO_LITR3C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITT_SCEL_FRAC];
	s->value[C_FROOT_TO_LITR4C] = s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITT_LIGN_FRAC];

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
	s->value[N_LEAF_TO_LITR1N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITT_LAB_FRAC];
	s->value[N_LEAF_TO_LITR2N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITT_USCEL_FRAC];
	s->value[N_LEAF_TO_LITR3N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITT_SCEL_FRAC];
	s->value[N_LEAF_TO_LITR4N]  = s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITT_LIGN_FRAC];

	/* fine root litter carbon */
	s->value[N_FROOT_TO_LITR1N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITT_LAB_FRAC];
	s->value[N_FROOT_TO_LITR2N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITT_USCEL_FRAC];
	s->value[N_FROOT_TO_LITR3N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITT_SCEL_FRAC];
	s->value[N_FROOT_TO_LITR4N] = s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITT_LIGN_FRAC];

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




