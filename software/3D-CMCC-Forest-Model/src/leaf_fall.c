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


void leaf_fall_deciduous(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double foliage_to_remove;
	static double fine_root_to_remove;
	static double fraction_to_retransl = 0.1; /* fraction of C to re-translocate (see Bossell et al., 2006 and Campioli et al., 2013 */
	static int senescenceDayOne;
	double previousLai, currentLai;
	double previousBiomass_lai, newBiomass_lai;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL DECIDUOUS **\n");

	logger(g_debug_log, "Leaf fall counter = %d\n", s->counter[LEAF_FALL_COUNTER]);
	logger(g_debug_log, "Leaf biomass = %f\n", s->value[LEAF_C]);
	logger(g_debug_log, "Fine root biomass  = %f\n", s->value[FINE_ROOT_C]);

	if(s->counter[LEAF_FALL_COUNTER] == 1)
	{
		logger(g_debug_log, "First day of Leaf fall\n");
		logger(g_debug_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

		/* note: assuming that fine roots for deciduous species progressively die together with leaves */
		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */

		/* assign LAI values at the beginning of the sigmoid shape */
		s->value[MAX_LAI_PROJ] = s->value[LAI_PROJ];
		senescenceDayOne = c->doy;

#if 1
		/* move all C biomass to falling leaves pool */
		s->value[LEAF_FALLING_C] = s->value[LEAF_C];

		/* reset leaf C biomass pool */
		s->value[LEAF_C] = 0.;
#endif
	}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* following Campioli et al., 2013 and Bossel 1996 10% of leaf and fine root biomass is daily re-translocated as reserve in the reserve pool */
		/* compute amount of leaf and fine root biomass to re-translocate as reserve */


#if 1
		s->value[C_LEAF_TO_RESERVE] = (s->value[LEAF_FALLING_C] * fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
#else
		s->value[C_LEAF_TO_RESERVE] = (s->value[LEAF_C] * fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
#endif


		s->value[C_FINEROOT_TO_RESERVE]= (s->value[FINE_ROOT_C] * fraction_to_retransl) /s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);

		previousLai = s->value[LAI_PROJ];

		/* sigmoid shape drives LAI reduction during leaf fall */
		currentLai = MAX(0,s->value[MAX_LAI_PROJ] / (1 + exp(-(s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + senescenceDayOne -
				c->doy)/(s->counter[DAY_FRAC_FOLIAGE_REMOVE] / (log(9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + senescenceDayOne) -
						log(.11111111111))))));
		logger(g_debug_log, "previousLai = %f\n", previousLai);
		logger(g_debug_log, "currentLai = %f\n", currentLai);

		/* check */
		CHECK_CONDITION(previousLai, < currentLai);

		previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);

		newBiomass_lai = (currentLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0));
		foliage_to_remove = previousBiomass_lai - newBiomass_lai;
		logger(g_debug_log, "foliage_to_remove = %f\n", foliage_to_remove);


#if 1
		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = (s->value[FINE_ROOT_C]*foliage_to_remove)/s->value[LEAF_FALLING_C];
		logger(g_debug_log, "fineroot_to_remove = %f\n", fine_root_to_remove);
#else
		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = (s->value[FINE_ROOT_C]*foliage_to_remove)/s->value[LEAF_C];
		logger(g_debug_log, "fineroot_to_remove = %f\n", fine_root_to_remove);
#endif

		s->value[C_TO_LEAF] = -foliage_to_remove ;
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = -fine_root_to_remove;
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", s->value[C_TO_FINEROOT]);
		s->value[C_TO_LITTER] = (foliage_to_remove - s->value[C_LEAF_TO_RESERVE]) + (fine_root_to_remove - s->value[C_FINEROOT_TO_RESERVE]);
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
	}
	else
	{

#if 1
		logger(g_debug_log, "Last day of leaf fall\n");
		s->value[C_TO_LEAF] = -s->value[LEAF_FALLING_C];
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = - s->value[FINE_ROOT_C];
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", -s->value[C_TO_FINEROOT]);
		s->value[C_LEAF_TO_RESERVE] = s->value[LEAF_FALLING_C];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
		s->value[C_FINEROOT_TO_RESERVE] = s->value[FINE_ROOT_C];
		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);
		s->value[C_TO_LITTER] = 0.0;
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
#else
		logger(g_debug_log, "Last day of leaf fall\n");
		s->value[C_TO_LEAF] = -s->value[LEAF_C];
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = - s->value[FINE_ROOT_C];
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", -s->value[C_TO_FINEROOT]);
		s->value[C_LEAF_TO_RESERVE] = s->value[LEAF_C];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
		s->value[C_FINEROOT_TO_RESERVE] = s->value[FINE_ROOT_C];
		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);
		s->value[C_TO_LITTER] = 0.0;
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
#endif

	}
}

void leaf_fall_evergreen (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double daily_leaf_fineroot_turnover_rate;
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL EVERGREEN**\n");

	/* compute rates */
	/* compute leaf and fine root turnover rate (ratio) */
	daily_leaf_fineroot_turnover_rate = s->value[LEAF_FINEROOT_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL];

	if ( c->doy == 1 )
	{
		/* compute annual amount of CN leaf falling (for evergreen) */
		CN_leaf_falling ( c, height, dbh, age, species );
	}

	/* compute daily amount of leaf and fine root to remove */
	s->value[C_LEAF_TO_LITTER] = (s->value[LEAF_FALLING_C] * daily_leaf_fineroot_turnover_rate);
	logger(g_debug_log, "Daily leaf turnover = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	s->value[C_FINE_ROOT_TO_LITTER] = (s->value[FINE_ROOT_C] * daily_leaf_fineroot_turnover_rate);
	logger(g_debug_log, "Daily fine root turnover = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_LITTER]);

	logger(g_debug_log, "Daily biomass turnover to litter before retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);

	/* compute daily amount of C to re-translocate before remove leaf and fine root */
	s->value[C_LEAF_TO_RESERVE] = s->value[C_LEAF_TO_LITTER] * fraction_to_retransl;
	logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %g tC/cell/day\n", s->value[C_LEAF_TO_RESERVE]);

	s->value[C_FINEROOT_TO_RESERVE] = s->value[C_FINE_ROOT_TO_LITTER] * fraction_to_retransl;
	logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %g tC/cell/day\n", s->value[C_FINEROOT_TO_RESERVE]);

	/* update considering retranslocation */
	s->value[C_LEAF_TO_LITTER] -= s->value[C_LEAF_TO_RESERVE];
	logger(g_debug_log, "Daily leaf turnover after retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	s->value[C_FINE_ROOT_TO_LITTER] -= s->value[C_FINEROOT_TO_RESERVE];
	logger(g_debug_log, "Daily fine root turnover after retranslocation = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_LITTER]);

	/**/
	s->value[C_TO_LEAF] -= s->value[C_LEAF_TO_LITTER];
	s->value[C_TO_FINEROOT] -= s->value[C_FINE_ROOT_TO_LITTER];

	s->value[LEAF_FALLING_C] -= s->value[C_LEAF_TO_LITTER];
	s->value[FINE_ROOT_C] -= s->value[C_FINE_ROOT_TO_LITTER];

	/* considering that both leaf and fine root contribute to the litter pool */
	s->value[C_TO_LITTER] = (s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);
	logger(g_debug_log, "biomass to litter after retranslocation = %g tC/cell/day\n", s->value[C_TO_LITTER]);

}

void CN_leaf_falling (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double daily_leaf_fineroot_turnover_rate;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* compute rates */
	/* compute leaf and fine root turnover rate (ratio) */
	daily_leaf_fineroot_turnover_rate = s->value[LEAF_FINEROOT_TURNOVER];
	logger(g_debug_log, "Daily leaf fine root turnover rate = %g (ratio)\n", daily_leaf_fineroot_turnover_rate);

	if ( s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2 )
	{
		/* compute leaf falling Carbon biomass pool */
		s->value[LEAF_FALLING_C] = s->value[LEAF_C] * daily_leaf_fineroot_turnover_rate;
		logger(g_debug_log, "LEAF_FALLING_C = %g tC/cell/day\n", s->value[LEAF_FALLING_C]);

		/* update biomass leaf pool */
		s->value[LEAF_C] -= s->value[LEAF_FALLING_C];
 	}
	else
	{
		s->value[LEAF_FALLING_C] = 0.;
	}


}


