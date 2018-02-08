/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "turnover.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

void turnover( cell_t *const c, age_t *const a, species_t *const s, const int day, const int month, const int year )
{
	double effective_livewood_turnover;      /* effective livewood turnover */

#if 1

	double juv_livewood_turnover_frac;       /* juvenile fraction of turnover */
	double mat_livewood_turnover_frac;       /* mature fraction of turnover */
	int t_age;                               /* age at which TURNOVER = (juvenile_livewood_turnover_frac/mature_livewood_turnover_frac)/2 */
	double n = 2.;

	/** age related function for turnover **/

	/* assumption: this function assumes that turnover rate increases with increasing tree age */
	juv_livewood_turnover_frac = 0.1;                         //fixme it should be moved to species.txt
	mat_livewood_turnover_frac = s->value[LIVEWOOD_TURNOVER]; //fixme it should be moved to species.txt
	t_age                      = 30;                          //fixme it should be moved to species.txt

	/* note: it recomputes variables in species.txt */
	effective_livewood_turnover = mat_livewood_turnover_frac + ( juv_livewood_turnover_frac - mat_livewood_turnover_frac ) *
			exp( -LN2 * pow( ( (double)a->value / t_age ), n ) );

#else

	/* note: assumes fixed value in species.txt */
	effective_livewood_turnover = s->value[LIVEWOOD_TURNOVER];

#endif

	/**************************************************************************************************************************/

	/* test it accounts for fractions for previous year carbon biomass for daily turnover */
	/* following Krinner et al., 2005 turnover occurs every day of the year */
	/* daily fraction of biomass to remove */

	s->value[DAILY_LIVEWOOD_TURNOVER] = effective_livewood_turnover / ( IS_LEAP_YEAR ( c->years[year].year ) ? 366 : 365);


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

	/* check */
	CHECK_CONDITION (s->value[C_STEM_LIVEWOOD_TO_DEADWOOD],   <, 0.);
	CHECK_CONDITION (s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD],  <, 0.);
	CHECK_CONDITION (s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD], <, 0.);
	CHECK_CONDITION (s->value[C_STEM_LIVEWOOD_TO_DEADWOOD],   >, s->value[STEM_SAPWOOD_C]);
	CHECK_CONDITION (s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD],  >, s->value[CROOT_SAPWOOD_C]);
	CHECK_CONDITION (s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD], >, s->value[BRANCH_SAPWOOD_C]);

	/*******************************************************************************************************************/

	/* daily stem turnover live to dead wood*/

	s->value[STEM_SAPWOOD_C]   -= s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];
	s->value[STEM_HEARTWOOD_C] += s->value[C_STEM_LIVEWOOD_TO_DEADWOOD];

	/*******************************************************************************************************************/

	/* daily coarse root turnover live to dead wood */

	s->value[CROOT_SAPWOOD_C]   -= s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD];
	s->value[CROOT_HEARTWOOD_C] += s->value[C_CROOT_LIVEWOOD_TO_DEADWOOD];

	/*******************************************************************************************************************/

	/* daily branch turnover live to dead wood */

	s->value[BRANCH_SAPWOOD_C]   -= s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD];
	s->value[BRANCH_HEARTWOOD_C] += s->value[C_BRANCH_LIVEWOOD_TO_DEADWOOD];

	/*******************************************************************************************************************/

	/* Overall sapwood and heartwood stocks */

	s->value[SAPWOOD_C]   = s->value[STEM_SAPWOOD_C]   + s->value[CROOT_SAPWOOD_C]   + s->value[BRANCH_SAPWOOD_C];
	s->value[HEARTWOOD_C] = s->value[STEM_HEARTWOOD_C] + s->value[CROOT_HEARTWOOD_C] + s->value[BRANCH_HEARTWOOD_C];

	/*******************************************************************************************************************/

}

