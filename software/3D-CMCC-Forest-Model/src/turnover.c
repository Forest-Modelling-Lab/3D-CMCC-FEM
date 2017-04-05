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

void turnover(species_t *const s)
{
	logger(g_debug_log, "\n**TURNOVER**\n");

#if 0
	//test_new
	/* following Krinner et al., 2005 turnover occurs every day of the year */
	s->value[DAILY_LIVE_WOOD_TURNOVER] = s->value[LIVE_WOOD_TURNOVER]/365.0;

	/*******************************************************************************************************************/
	/* daily stem turnover live to dead wood*/
	logger(g_debug_log, "\n****Stem turnover****\n");

	s->value[STEM_SAPWOOD_C] -= (s->value[STEM_SAPWOOD_C] * s->value[DAILY_LIVE_WOOD_TURNOVER]);
	logger(g_debug_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);

	/*******************************************************************************************************************/

	/* daily coarse root turnover live to dead wood */
	logger(g_debug_log, "\n****Coarse root turnover****\n");

	s->value[COARSE_ROOT_SAPWOOD_C] -= (s->value[COARSE_ROOT_SAPWOOD_C] * s->value[DAILY_LIVE_WOOD_TURNOVER]);
	logger(g_debug_log, "C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD]);

	/*******************************************************************************************************************/

	/* daily branch turnover live to dead wood */
	logger(g_debug_log, "\n****Branch turnover****\n");
	s->value[BRANCH_SAPWOOD_C] -= (s->value[BRANCH_SAPWOOD_C] * s->value[DAILY_LIVE_WOOD_TURNOVER]);
	logger(g_debug_log, "C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD = %g tC/cell\n", s->value[BRANCH_SAPWOOD_C]);

	/*******************************************************************************************************************/

	/* daily fruit turnover */
	if( s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0 )
	{
		s->value[C_FRUIT_TO_LITTER] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
		logger(g_debug_log, "C_FRUIT_TO_LITTER = %g tC/cell\n", s->value[C_FRUIT_TO_LITTER]);
	}
#else

	/* compute live-wood turnover rate (ratio) */
	s->value[DAILY_LIVE_WOOD_TURNOVER] = (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL]);

	/*following BIOME_BGC turnover occurs only during growing season */
	if ( s->counter[VEG_UNVEG] == 1 )
	{
		/*******************************************************************************************************************/
		/* daily stem turnover live to dead wood*/
		logger(g_debug_log, "\n****Stem turnover****\n");

		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD] = (s->value[C_TO_STEM] * s->value[DAILY_LIVE_WOOD_TURNOVER]);
		logger(g_debug_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);

		/*******************************************************************************************************************/

		/* daily coarse root turnover live to dead wood */
		logger(g_debug_log, "\n****Coarse root turnover****\n");

		s->value[C_CROOT_LIVE_WOOD_TO_DEADWOOD] = (s->value[C_TO_CROOT] * s->value[DAILY_LIVE_WOOD_TURNOVER]);
		logger(g_debug_log, "C_CROOT_LIVE_WOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_CROOT_LIVE_WOOD_TO_DEADWOOD]);

		/*******************************************************************************************************************/

		/* daily branch turnover live to dead wood */
		logger(g_debug_log, "\n****Branch turnover****\n");

		s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD] = (s->value[C_TO_BRANCH] * s->value[DAILY_LIVE_WOOD_TURNOVER]);
		logger(g_debug_log, "C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD = %g tC/cell\n", s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD]);

		/*******************************************************************************************************************/
	}
	else
	{
		logger(g_debug_log,"No turnover\n");
	}

#endif
}

