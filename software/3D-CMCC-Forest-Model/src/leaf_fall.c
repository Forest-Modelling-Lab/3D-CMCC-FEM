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

		//fixme senescenceDayOne MUST be in structure
		senescenceDayOne = c->doy;
		}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* following Campioli et al., 2013 and Bossel 1996 10% of leaf and fine root biomass is daily re-translocated as reserve in the reserve pool */
		/* compute amount of leaf and fine root biomass to re-translocate as reserve */


		s->value[C_LEAF_TO_RESERVE] = (s->value[LEAF_C] * fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);


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
		CHECK_CONDITION(previousLai, <, currentLai);

		previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);

		newBiomass_lai = (currentLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0));


		leaf_to_remove = previousBiomass_lai - newBiomass_lai;
		logger(g_debug_log, "leaf_to_remove = %f\n", leaf_to_remove);


		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = (s->value[FINE_ROOT_C]*leaf_to_remove)/s->value[LEAF_C];
		logger(g_debug_log, "fineroot_to_remove = %f\n", fine_root_to_remove);

		/* update leaf falling */
		s->value[LEAF_FALLING_C] = leaf_to_remove;


		s->value[C_TO_LEAF] = -leaf_to_remove ;
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = -fine_root_to_remove;
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", s->value[C_TO_FINEROOT]);
		s->value[C_TO_LITTER] = (leaf_to_remove - s->value[C_LEAF_TO_RESERVE]) + (fine_root_to_remove - s->value[C_FINEROOT_TO_RESERVE]);
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
	}
	else
	{
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
	}
}
void leaf_fall_evergreen ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	static double leaf_to_remove;
	static double fine_root_to_remove;
	double fraction_to_retransl = 0.1; /* fraction of C to re-translocate (see Bossell et al., 2006 and Campioli et al., 2013 */
	static int senescenceDayOne;
	//double previousLai, currentLai;
	//double previousBiomass_lai, newBiomass_lai;

	static double previous_leaf_C, current_leaf_C;
	static double annual_leaf_to_remove;
	static double annual_fineroot_to_remove;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL EVERGREEN **\n");

	logger(g_debug_log, "Leaf fall counter = %d\n", s->counter[LEAF_FALL_COUNTER]);
	logger(g_debug_log, "FIRST_DAY_LEAF_C = %f\n", s->value[FIRST_DAY_LEAF_C]);
	logger(g_debug_log, "FIRST_DAY_FINE_ROOT_C  = %f\n", s->value[FIRST_DAY_FINE_ROOT_C]);

	if(s->counter[LEAF_FALL_COUNTER] == 1)
	{
		logger(g_debug_log, "First day of Leaf fall\n");
		logger(g_debug_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

		/* note: assuming that fine roots for deciduous species progressively die together with leaves */
		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */

		/* assign leaf C values of the 1st of January at the beginning of the sigmoid shape */
		//fixme annual_leaf_to_remove MUST be in structure
		annual_leaf_to_remove = s->value[FIRST_DAY_LEAF_C] * s->value[LEAF_FINEROOT_TURNOVER];
		logger(g_debug_log, "annual_leaf_to_remove = %f\n", annual_leaf_to_remove );

		//fixme annual_fineroot_to_remove MUST be in structure
		annual_fineroot_to_remove = s->value[FIRST_DAY_FINE_ROOT_C] * s->value[LEAF_FINEROOT_TURNOVER];
		logger(g_debug_log, "annual_fineroot_to_remove = %f\n", annual_fineroot_to_remove );

		//fixme senescenceDayOne MUST be in structure
		senescenceDayOne = c->doy;
	}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* following Campioli et al., 2013 and Bossel 1996 10% of leaf and fine root biomass is daily re-translocated as reserve in the reserve pool */
		/* compute amount of leaf and fine root biomass to re-translocate as reserve */

		s->value[C_LEAF_TO_RESERVE] = ( annual_leaf_to_remove * fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);


		s->value[C_FINEROOT_TO_RESERVE]= ( annual_fineroot_to_remove * fraction_to_retransl) /s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);

		if ( s->counter[LEAF_FALL_COUNTER] == 1 )
		{	//fixme previous_leaf_C MUST be in structure
			previous_leaf_C = annual_leaf_to_remove;
		}
		else
		{
			//fixme previous_leaf_C MUST be in structure
			previous_leaf_C = current_leaf_C;
		}


		/* sigmoid shape drives LAI reduction during leaf fall */
		//fixme current_leaf_C MUST be in structure
		current_leaf_C = MAX ( 0 , annual_leaf_to_remove / (1 + exp ( - ( s->counter[DAY_FRAC_FOLIAGE_REMOVE] / 2.0 + senescenceDayOne -
				c->doy ) / ( s->counter[DAY_FRAC_FOLIAGE_REMOVE] / ( log ( 9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE] / 2.0 + senescenceDayOne ) -
						log (.11111111111) ) ) ) ) );
		logger(g_debug_log, "previous_leaf_C = %f\n", previous_leaf_C);
		logger(g_debug_log, "current_leaf_C = %f\n", current_leaf_C);

		/* check */
		CHECK_CONDITION(previous_leaf_C, <, current_leaf_C);

		leaf_to_remove = previous_leaf_C - current_leaf_C;
		logger(g_debug_log, "leaf_to_remove = %f\n", leaf_to_remove);


		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		//fixme
		fine_root_to_remove = (s->value[FINE_ROOT_C]*leaf_to_remove)/s->value[LEAF_C];
		logger(g_debug_log, "fineroot_to_remove = %f\n", fine_root_to_remove);

		/* update leaf falling */
		s->value[LEAF_FALLING_C] = leaf_to_remove;


		s->value[C_TO_LEAF] = -leaf_to_remove ;
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = -fine_root_to_remove;
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", s->value[C_TO_FINEROOT]);
		s->value[C_TO_LITTER] = (leaf_to_remove - s->value[C_LEAF_TO_RESERVE]) + (fine_root_to_remove - s->value[C_FINEROOT_TO_RESERVE]);
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
	}
//	else
//	{
//		logger(g_debug_log, "Last day of leaf fall\n");
//		s->value[C_TO_LEAF] = -s->value[LEAF_C];
//		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
//		s->value[C_TO_FINEROOT] = - s->value[FINE_ROOT_C];
//		logger(g_debug_log, "C_TO_FINEROOT = %f\n", -s->value[C_TO_FINEROOT]);
//		s->value[C_LEAF_TO_RESERVE] = s->value[LEAF_C];
//		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
//		s->value[C_FINEROOT_TO_RESERVE] = s->value[FINE_ROOT_C];
//		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);
//		s->value[C_TO_LITTER] = 0.0;
//		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
//	}
}

#if 0
void leaf_fall_evergreen ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	double yearly_leaf_fall_falling_C;
	double yearly_fine_root_turnover_C;
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL (turnover) EVERGREEN**\n");

	/************************************************************************************************************/

	if ( c->doy == 1 )
	{
		/* compute annual leaf turnover */
		yearly_leaf_fall_falling_C = s->value[LEAF_C] * s->value[LEAF_FINEROOT_TURNOVER];
		logger(g_debug_log, "Annual leaf turnover = %g tC/cell/year\n", yearly_leaf_fall_falling_C);

		/* daily leaf fall */
		s->value[LEAF_FALLING_C] = yearly_leaf_fall_falling_C / 365;
		logger(g_debug_log, "Daily leaf turnover = %g tC/cell/year\n", s->value[LEAF_FALLING_C]);

		/* compute fine root turnover */
		yearly_fine_root_turnover_C = s->value[FINE_ROOT_C] * s->value[LEAF_FINEROOT_TURNOVER];
		logger(g_debug_log, "Annual fine root turnover = %g tC/cell/year\n", yearly_fine_root_turnover_C);

		/* daily fine root turnover */
		s->value[FINE_ROOT_TURNOVER_C] = yearly_fine_root_turnover_C / 365;
		logger(g_debug_log, "Daily fine root turnover = %g tC/cell/year\n", s->value[FINE_ROOT_TURNOVER_C]);
	}

	/*************************************************************************************************************/

	/* update biomass leaf pool */
	s->value[LEAF_C] -= s->value[LEAF_FALLING_C];
	logger(g_debug_log, "LEAF_C = %f tC/cell/day\n", s->value[LEAF_C]);

	/* compute daily amount of leaf and fine root to remove */
	s->value[C_LEAF_TO_LITTER] = s->value[LEAF_FALLING_C] ;
	logger(g_debug_log, "Daily leaf turnover = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	/* update biomass fine root pool */
	s->value[FINE_ROOT_C] -= s->value[FINE_ROOT_TURNOVER_C];
	logger(g_debug_log, "FINE_ROOT_C = %g tC/cell/day\n", s->value[FINE_ROOT_C]);

	s->value[C_FINE_ROOT_TO_LITTER] = s->value[FINE_ROOT_TURNOVER_C];
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

	/* considering that both leaf and fine root contribute to the litter pool */
	s->value[C_TO_LITTER] = (s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);
	logger(g_debug_log, "biomass to litter after retranslocation = %g tC/cell/day\n", s->value[C_TO_LITTER]);

}
#endif


