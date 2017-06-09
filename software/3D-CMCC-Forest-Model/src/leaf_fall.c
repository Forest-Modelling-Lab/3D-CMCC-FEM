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
#include "littering.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void leaffall_deciduous ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	double previousLai, currentLai;
	double previousLeafC, currentLeafC;
	double previousLeafN, currentLeafN;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL DECIDUOUS **\n");

	if( s->counter[LEAF_FALL_COUNTER] == 1 )
	{
		logger(g_debug_log, "First day of Leaf fall\n");

		/* note: assuming that fine roots and fruit for deciduous species progressively die together with leaves */
		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */

		/* assign Maximum LAI values at the beginning of the sigmoid shape */
		s->value[MAX_LAI_LEAFFALL_PROJ] = s->value[LAI_PROJ];

		/* assign senescence doy */
		s->counter[SENESCENCE_DAY_ONE] = c->doy;
	}

	if( s->counter[LEAF_FALL_COUNTER] < s->counter[DAYS_LEAFFALL] )
	{
		/* load previous LAI */
		previousLai = s->value[LAI_PROJ];

		/* sigmoid shape drives LAI reduction during leaf fall */
		currentLai  = MAX(0,s->value[MAX_LAI_LEAFFALL_PROJ] / (1 + exp(-(s->counter[DAYS_LEAFFALL] / 2. + s->counter[SENESCENCE_DAY_ONE] - c->doy)
				/(s->counter[DAYS_LEAFFALL] / (log(9. * s->counter[DAYS_LEAFFALL] / 2. + s->counter[SENESCENCE_DAY_ONE]) -
						log(.11111111111))))));

		/* check */
		CHECK_CONDITION(previousLai, <, currentLai);

		/* determine Leaf Area Index from leaf carbon */
		previousLeafC = previousLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1e3);
		currentLeafC  = currentLai  * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1e3);

		previousLeafN = previousLeafC / s->value[CN_LEAVES];
		currentLeafN  = currentLeafC  / s->value[CN_LEAVES];

		/* update leaf carbon */
		s->value[LEAF_C_TO_REMOVE]   = previousLeafC - currentLeafC;
		s->value[LEAF_N_TO_REMOVE]   = previousLeafN - currentLeafN;

		/* update fine root carbon */
		s->value[FROOT_C_TO_REMOVE]  = ( s->value[FROOT_C] * s->value[LEAF_C_TO_REMOVE]) / s->value[LEAF_C];
		s->value[FROOT_N_TO_REMOVE]  = ( s->value[FROOT_N] * s->value[LEAF_N_TO_REMOVE]) / s->value[LEAF_N];

		/* update fruit carbon */
		s->value[FRUIT_C_TO_REMOVE]  = ( s->value[FRUIT_C] * s->value[LEAF_C_TO_REMOVE]) / s->value[LEAF_C];
		s->value[FRUIT_N_TO_REMOVE]  = ( s->value[FRUIT_N] * s->value[LEAF_N_TO_REMOVE]) / s->value[LEAF_N];

		/* reconcile leaf and fine root */
		leaffall ( s );

	}
	else
	{
		/* last day of litterfall, special case to guarantee that pools go to 0.0 */

		logger(g_debug_log, "Last day of leaf fall\n");

		//assumption: last day of leaf fall all carbon goes to litter and cwd with no retranslocation
		s->value[C_LEAF_TO_LITR]    += s->value[LEAF_C];
		s->value[C_FROOT_TO_LITR]   += s->value[FROOT_C];
		s->value[C_FRUIT_TO_CWD]    += s->value[FRUIT_C];
		s->value[C_LEAF_TO_RESERVE]  = 0.;
		s->value[C_FROOT_TO_RESERVE] = 0.;

		/* adding to main C transfer pools */
		s->value[C_TO_RESERVE]      += 0.; /*leave it as so */
		s->value[C_TO_LITR]          = (s->value[LEAF_C] + s->value[FROOT_C]);
		s->value[C_TO_CWD]           = s->value[FRUIT_C];

		//assumption: last day of leaf fall all nitrogen goes to litter with no retranslocation
		s->value[N_LEAF_TO_LITR]    += s->value[LEAF_N];
		s->value[N_FROOT_TO_LITR]   += s->value[FROOT_N];
		s->value[N_FRUIT_TO_CWD]    += s->value[FRUIT_N];
		s->value[N_LEAF_TO_RESERVE]  = 0.;
		s->value[N_FROOT_TO_RESERVE] = 0.;

		/* balancing leaf_C in and out */
		s->value[N_TO_LEAF]          = - s->value[LEAF_N];
		s->value[N_TO_FROOT]         = - s->value[FROOT_N];
		s->value[N_TO_FRUIT]         = - s->value[FRUIT_N];

		/* adding to main C transfer pools */
		s->value[N_TO_RESERVE]      += 0.; /* leave it as so */
		s->value[N_TO_LITR]          = (s->value[LEAF_N] + s->value[FROOT_N]);
		s->value[N_TO_CWD]           = s->value[FRUIT_N];
	}
}

void leaffall_evergreen ( cell_t *const c, const int height, const int dbh, const int age, const int species, const int year )
{
	int days_for_leaffall;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	if ( IS_LEAP_YEAR ( c->years[year].year ) ) days_for_leaffall = 366;
	else days_for_leaffall = 365;

	logger(g_debug_log, "\n**LEAF FALL EVERGREEN**\n");

	/************************************************************************************************************/

	if ( c->doy == 1 )
	{
		/* daily leaf fall turnover rate */
		s->value[LEAF_C_TO_REMOVE]    = (s->value[LEAF_C]  * s->value[LEAF_FROOT_TURNOVER]) / days_for_leaffall;
		s->value[LEAF_N_TO_REMOVE]    = (s->value[LEAF_N]  * s->value[LEAF_FROOT_TURNOVER]) / days_for_leaffall;

		/* daily fine root turnover rate */
		s->value[FROOT_C_TO_REMOVE]   = (s->value[FROOT_C] * s->value[LEAF_FROOT_TURNOVER]) / days_for_leaffall;
		s->value[FROOT_N_TO_REMOVE]   = (s->value[FROOT_N] * s->value[LEAF_FROOT_TURNOVER]) / days_for_leaffall;

		if ( s->counter[YOS] > s->value[CONES_LIFE_SPAN] )
		{
			/* daily fruit turnover rate */
			s->value[FRUIT_C_TO_REMOVE]   = (s->value[FRUIT_C] * (1. / s->value[CONES_LIFE_SPAN])) / days_for_leaffall;
			s->value[FRUIT_N_TO_REMOVE]   = (s->value[FRUIT_N] * (1. / s->value[CONES_LIFE_SPAN])) / days_for_leaffall;
		}
	}

	/*************************************************************************************************************/

	/* reconcile leaf and fine root */
	leaffall ( s );
}

void leaffall (species_t *const s)
{
	/*** carbon leaf_fall ***/
	/* compute fluxes of carbon leaf and fine root pool */
	s->value[C_LEAF_TO_RESERVE]  += s->value[LEAF_C_TO_REMOVE]   * C_FRAC_TO_RETRANSL;
	s->value[C_FROOT_TO_RESERVE] += s->value[FROOT_C_TO_REMOVE]  * C_FRAC_TO_RETRANSL;
	s->value[C_LEAF_TO_LITR]     += s->value[LEAF_C_TO_REMOVE]   * ( 1. - C_FRAC_TO_RETRANSL );
	s->value[C_FROOT_TO_LITR]    += s->value[FROOT_C_TO_REMOVE]  * ( 1. - C_FRAC_TO_RETRANSL );
	s->value[C_FRUIT_TO_CWD]     += s->value[FRUIT_C_TO_REMOVE];

	logger(g_debug_log, "C_LEAF_TO_RESERVE  = %f\n", s->value[C_LEAF_TO_RESERVE]);
	logger(g_debug_log, "C_FROOT_TO_RESERVE = %f\n", s->value[C_FROOT_TO_RESERVE]);
	logger(g_debug_log, "C_LEAF_TO_LITR     = %f\n", s->value[C_LEAF_TO_LITR]);
	logger(g_debug_log, "C_FROOT_TO_LITR    = %f\n", s->value[C_FROOT_TO_LITR]);
	logger(g_debug_log, "C_FRUIT_TO_CWD     = %f\n", s->value[C_FRUIT_TO_CWD]);

	/* carbon litter transfer fluxes to carbon litter pool and reserves */
	s->value[C_TO_RESERVE]      += (s->value[C_LEAF_TO_RESERVE] + s->value[C_FROOT_TO_RESERVE]);
	s->value[C_TO_LITR]         += (s->value[C_LEAF_TO_LITR]    + s->value[C_FROOT_TO_LITR] );
	s->value[C_TO_CWD]          += s->value[C_FRUIT_TO_CWD];

	logger(g_debug_log, "C_TO_RESERVE       = %f\n", s->value[C_TO_RESERVE]);
	logger(g_debug_log, "C_TO_LITR          = %f\n", s->value[C_TO_LITR]);
	logger(g_debug_log, "C_TO_CWD           = %f\n", s->value[C_TO_CWD]);

	/*** nitrogen leaf_fall ***/
	/* compute fluxes of nitrogen leaf and fine root pool */
	/* assumption: retranslocation happens as for C using falling leaves parameter: */
	/* see Bossel, 1996 Ecological Modelling */
	/* see Dezi et al., 2010 GCB */

	//fixme to include
	s->value[N_LEAF_TO_RESERVE]  = 0. /* s->value[LEAF_N_TO_REMOVE]   * (1. - (s->value[CN_LEAVES]     / s->value[CN_FALLING_LEAVES]))*/;
	s->value[N_LEAF_TO_LITR]     = s->value[LEAF_N_TO_REMOVE] /*  * (s->value[CN_LEAVES]     / s->value[CN_FALLING_LEAVES])*/;
	s->value[N_FROOT_TO_RESERVE] = 0. /* s->value[FROOT_N_TO_REMOVE]  * N_FRAC_TO_RETRANSL*/;
	s->value[N_FROOT_TO_LITR]    = s->value[FROOT_N_TO_REMOVE]/*  * (1 - N_FRAC_TO_RETRANSL)*/;
	s->value[N_FRUIT_TO_CWD]     = s->value[FRUIT_N_TO_REMOVE];

	logger(g_debug_log, "N_LEAF_TO_RESERVE  = %f\n", s->value[N_LEAF_TO_RESERVE]);
	logger(g_debug_log, "N_FROOT_TO_RESERVE = %f\n", s->value[N_FROOT_TO_RESERVE]);
	logger(g_debug_log, "N_LEAF_TO_LITR     = %f\n", s->value[N_LEAF_TO_LITR]);
	logger(g_debug_log, "N_FROOT_TO_LITR    = %f\n", s->value[N_FROOT_TO_LITR]);
	logger(g_debug_log, "N_FRUIT_TO_CWD     = %f\n", s->value[N_FRUIT_TO_CWD]);

	/* nitrogen litter transfer fluxes to nitrogen litter pool and reserves */
	s->value[N_TO_RESERVE]      += (s->value[N_LEAF_TO_RESERVE] + s->value[N_FROOT_TO_RESERVE]);
	s->value[N_TO_LITR]         += (s->value[N_LEAF_TO_LITR]    + s->value[N_FROOT_TO_LITR]);
	s->value[N_TO_CWD]          += s->value[N_FRUIT_TO_CWD];

	logger(g_debug_log, "N_TO_RESERVE       = %f\n", s->value[N_TO_RESERVE]);
	logger(g_debug_log, "N_TO_LITR          = %f\n", s->value[N_TO_LITR]);
	logger(g_debug_log, "N_TO_CWD           = %f\n", s->value[N_TO_CWD]);

	/* check */
	if ( s->value[LEAF_C] < s->value[LEAF_C_TO_REMOVE] )
	{
		s->value[LEAF_C]            = 0.;
		s->value[LEAF_C_TO_REMOVE]  = 0.;
	}
	if ( s->value[FROOT_C] < s->value[FROOT_C_TO_REMOVE] )
	{
		s->value[FROOT_C]           = 0.;
		s->value[FROOT_C_TO_REMOVE] = 0.;
	}
	if ( s->value[FRUIT_C] < s->value[FRUIT_C_TO_REMOVE] )
	{
		s->value[FRUIT_C]           = 0.;
		s->value[FRUIT_C_TO_REMOVE] = 0.;
	}

	/* check */
	if ( s->value[LEAF_N] < s->value[LEAF_N_TO_REMOVE] )
	{
		s->value[LEAF_N]            = 0.;
		s->value[LEAF_N_TO_REMOVE]  = 0.;
	}
	if ( s->value[FROOT_N] < s->value[FROOT_N_TO_REMOVE] )
	{
		s->value[FROOT_N]           = 0.;
		s->value[FROOT_N_TO_REMOVE] = 0.;
	}
	if ( s->value[FRUIT_N] < s->value[FRUIT_N_TO_REMOVE] )
	{
		s->value[FRUIT_N]           = 0.;
		s->value[FRUIT_N_TO_REMOVE] = 0.;
	}

	/* check */
	CHECK_CONDITION(s->value[LEAF_C],  <, s->value[LEAF_C_TO_REMOVE]);
	CHECK_CONDITION(s->value[FROOT_C], <, s->value[FROOT_C_TO_REMOVE]);
	CHECK_CONDITION(s->value[FRUIT_C], <, s->value[FRUIT_C_TO_REMOVE]);
	CHECK_CONDITION(s->value[LEAF_N],  <, s->value[LEAF_N_TO_REMOVE]);
	CHECK_CONDITION(s->value[FROOT_N], <, s->value[FROOT_N_TO_REMOVE]);
	CHECK_CONDITION(s->value[FRUIT_N], <, s->value[FRUIT_N_TO_REMOVE]);
}







