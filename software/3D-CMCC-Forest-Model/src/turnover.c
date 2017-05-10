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
	int days_for_turnover;

#if 0
	double juvenile_livewood_turnover_frac;  /* juvenile fraction of turnover */
	double mature_livewood_turnover_frac;    /* mature fraction of turnover */
	int t_age;                               /* age at which TURNOVER = (juvenile_livewood_turnover_frac/mature_livewood_turnover_frac)/2 */
	double n = 2.;

	/* this function assumes that turonver increase with increasing tree age */
	/* assumptions */
	juvenile_livewood_turnover_frac = 0.1;
	mature_livewood_turnover_frac   = s->value[LIVEWOOD_TURNOVER];
	t_age = 5;

	/* note: it recomputes variables in species.txt */
	s->value[LIVEWOOD_TURNOVER] = mature_livewood_turnover_frac + ( juvenile_livewood_turnover_frac - mature_livewood_turnover_frac ) *
			exp( -ln2 * pow( ( a->value / t_age ), n ) );
#endif


	logger(g_debug_log, "\n**TURNOVER**\n");

	if ( c->doy ==  IS_LEAP_YEAR ( c->years[year].year ) ) days_for_turnover = 366;
	else days_for_turnover = 365;

	/* test it accounts for fractions for previous year carbon biomass for daily turnover */
	/* following Krinner et al., 2005 turnover occurs every day of the year */
	/* daily fraction of biomass to remove */
	s->value[DAILY_LIVEWOOD_TURNOVER] = s->value[LIVEWOOD_TURNOVER] / (double)days_for_turnover;

	if ( ! day &&  ! month && year )
	{
		/* stem */
		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]   = s->value[YEARLY_C_TO_STEM]    * s->value[DAILY_LIVEWOOD_TURNOVER];

		/* coarse root */
		s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD]  = s->value[YEARLY_C_TO_CROOT]   * s->value[DAILY_LIVEWOOD_TURNOVER];

		/* branch */
		s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD] = s->value[YEARLY_C_TO_BRANCH]  * s->value[DAILY_LIVEWOOD_TURNOVER];

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

