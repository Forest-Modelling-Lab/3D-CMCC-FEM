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

void turnover( cell_t *const c, age_t *const a, species_t *const s, const int day, const int month, const int year )
{

#if 1 /* test */
	double juv_livewood_turnover_frac;       /* juvenile fraction of turnover */
	double mat_livewood_turnover_frac;       /* mature fraction of turnover */
	double effective_livewood_turnover;      /* effective livewood turnover */
	int t_age;                               /* age at which TURNOVER = (juvenile_livewood_turnover_frac/mature_livewood_turnover_frac)/2 */
	double n = 2.;

	/** age related function for turnover **/

	/* this function assumes that turnover rate increases with increasing tree age */
	/* note: assumptions */
	juv_livewood_turnover_frac = 0.1;    //fixme it should be moved to species.txt
	mat_livewood_turnover_frac = 0.7;    //fixme it should be moved to species.txt
	t_age                      = 10;     //fixme it should be moved to species.txt

	logger(g_debug_log, "\n**TURNOVER**\n");

	/* note: it recomputes variables in species.txt */
	effective_livewood_turnover = mat_livewood_turnover_frac + ( juv_livewood_turnover_frac - mat_livewood_turnover_frac ) *
			exp( -LN2 * pow( ( (double)a->value / t_age ), n ) );

	/* note: overwrites value to those in species.txt */
	s->value[LIVEWOOD_TURNOVER] = effective_livewood_turnover;

#endif

	logger(g_debug_log, "\n**TURNOVER**\n");

	/* test it accounts for fractions for previous year carbon biomass for daily turnover */
	/* following Krinner et al., 2005 turnover occurs every day of the year */
	/* daily fraction of biomass to remove */

	s->value[DAILY_LIVEWOOD_TURNOVER] = s->value[LIVEWOOD_TURNOVER] / (IS_LEAP_YEAR ( c->years[year].year ) ? 366 : 365);


	if ( ! day &&  ! month && s->counter[YOS] > 1 )
	{
		/* stem */
		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]   = s->value[YEARLY_C_TO_STEM]   * s->value[DAILY_LIVEWOOD_TURNOVER];

		/* coarse root */
		s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD]  = s->value[YEARLY_C_TO_CROOT]  * s->value[DAILY_LIVEWOOD_TURNOVER];

		/* branch */
		s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD] = s->value[YEARLY_C_TO_BRANCH] * s->value[DAILY_LIVEWOOD_TURNOVER];

		/* reset annual values once used */
		s->value[YEARLY_C_TO_STEM]   = 0.;
		s->value[YEARLY_C_TO_CROOT]  = 0.;
		s->value[YEARLY_C_TO_BRANCH] = 0.;
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

	/* Overall sapwood and heartwood */
	s->value[TOT_SAPWOOD_C]   = s->value[STEM_SAPWOOD_C]   + s->value[CROOT_SAPWOOD_C]   + s->value[BRANCH_SAPWOOD_C];
	s->value[TOT_HEARTWOOD_C] = s->value[STEM_HEARTWOOD_C] + s->value[CROOT_HEARTWOOD_C] + s->value[BRANCH_HEARTWOOD_C];

	logger(g_debug_log, "STEM_SAPWOOD_C     = %f tC/cell\n", s->value[STEM_SAPWOOD_C]);
	logger(g_debug_log, "STEM_HEARTWOOD_C   = %f tC/cell\n", s->value[STEM_HEARTWOOD_C]);
	logger(g_debug_log, "CROOT_SAPWOOD_C    = %f tC/cell\n", s->value[CROOT_SAPWOOD_C]);
	logger(g_debug_log, "CROOT_HEARTWOOD_C  = %f tC/cell\n", s->value[CROOT_HEARTWOOD_C]);
	logger(g_debug_log, "BRANCH_SAPWOOD_C   = %f tC/cell\n", s->value[BRANCH_SAPWOOD_C]);
	logger(g_debug_log, "BRANCH_HEARTWOOD_C = %f tC/cell\n", s->value[BRANCH_HEARTWOOD_C]);
	logger(g_debug_log, "TOT_SAPWOOD_C      = %f tC/cell\n", s->value[TOT_SAPWOOD_C]);
	logger(g_debug_log, "TOT_HEARTWOOD_C    = %f tC/cell\n", s->value[TOT_HEARTWOOD_C]);
}

