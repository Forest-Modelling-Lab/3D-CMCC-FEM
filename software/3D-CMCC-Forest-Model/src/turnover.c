/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "turnover.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

void turnover( cell_t *const c, species_t *const s, const int day, const int month, const int year )
{
#if 0
	int days_for_turnover;

	logger(g_debug_log, "\n**TURNOVER**\n");

	if ( c->doy ==  IS_LEAP_YEAR ( c->years[year].year ) ) days_for_turnover = 366;
	else days_for_turnover = 365;

	if ( ! day &&  ! month )
	{
		//test_new it accounts for fractions for previous year carbon biomass for turnover
		/* following Krinner et al., 2005 turnover occurs every day of the year */
		/* daily fraction of biomass to remove */
		s->value[DAILY_LIVEWOOD_TURNOVER]       = s->value[LIVEWOOD_TURNOVER] / (double)days_for_turnover;

		/* stem */
		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]   = s->value[STEM_SAPWOOD_C]    / s->value[DAILY_LIVEWOOD_TURNOVER];

		/* coarse root */
		s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD]  = s->value[CROOT_SAPWOOD_C]   / s->value[DAILY_LIVEWOOD_TURNOVER];

		/* branch */
		s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD] = s->value[BRANCH_SAPWOOD_C]  / s->value[DAILY_LIVEWOOD_TURNOVER];
	}

	/*******************************************************************************************************************/
	/* daily stem turnover live to dead wood*/
	logger(g_debug_log, "\n****Stem turnover****\n");

	s->value[STEM_SAPWOOD_C]   -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	s->value[STEM_HEARTWOOD_C] += s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	logger(g_debug_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %f tC/cell\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);

	/*******************************************************************************************************************/

	/* daily coarse root turnover live to dead wood */
	logger(g_debug_log, "\n****Coarse root turnover****\n");

	s->value[CROOT_SAPWOOD_C]   -= s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD];
	s->value[CROOT_HEARTWOOD_C] += s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD];
	logger(g_debug_log, "C_CROOT_LIVEWOOD_TO_DEADWOOD = %f tC/cell\n", s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD]);

	/*******************************************************************************************************************/

	/* daily branch turnover live to dead wood */
	logger(g_debug_log, "\n****Branch turnover****\n");

	s->value[BRANCH_SAPWOOD_C]   -= s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD];
	s->value[BRANCH_HEARTWOOD_C] += s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD];
	logger(g_debug_log, "C_BRANCH_LIVEWOOD_TO_DEADWOOD = %f tC/cell\n", s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD]);

	/*******************************************************************************************************************/
#else

	/* compute live-wood turnover rate (ratio) */
	s->value[DAILY_LIVEWOOD_TURNOVER] = (s->value[LIVEWOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL]);

	/*following BIOME_BGC turnover occurs only during growing season */
	if ( s->counter[VEG_UNVEG] == 1 )
	{
		/*******************************************************************************************************************/
		/* daily stem turnover live to dead wood*/

		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD] = (s->value[C_TO_STEM] * s->value[DAILY_LIVEWOOD_TURNOVER]);
		logger(g_debug_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %f tC/cell\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);

		/*******************************************************************************************************************/

		/* daily coarse root turnover live to dead wood */

		s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD] = (s->value[C_TO_CROOT] * s->value[DAILY_LIVEWOOD_TURNOVER]);
		logger(g_debug_log, "C_CROOT_LIVE_WOOD_TO_DEADWOOD = %f tC/cell\n", s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD]);

		/*******************************************************************************************************************/

		/* daily branch turnover live to dead wood */

		s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD] = (s->value[C_TO_BRANCH] * s->value[DAILY_LIVEWOOD_TURNOVER]);
		logger(g_debug_log, "C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD = %f tC/cell\n", s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD]);

		/*******************************************************************************************************************/
	}
	else
	{
		logger(g_debug_log,"No turnover\n");
	}

#endif
}

