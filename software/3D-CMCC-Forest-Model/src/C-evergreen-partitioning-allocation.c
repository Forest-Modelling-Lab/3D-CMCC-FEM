/*
 * C-evergreen-partitioning-allocation.c
 *
 *  Created on: 20/nov/2013
 *      Author: alessio
 */
/* includes */
#include <CN-allocation.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "lai.h"
#include "leaf_fall.h"
#include "turnover.h"
#include "dendometry.h"
#include "biomass.h"
#include "check_balance.h"
#include "mortality.h"
#include "aut_respiration.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

extern int MonthLength [];
extern int MonthLength_Leap [];

/* Evergreen carbon allocation routine */
void daily_C_evergreen_partitioning_allocation(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species,
		const meteo_daily_t *const meteo_daily, const int day, const int month, const int year)
{
	double s0;
	double r0;
	double omega;
	double pS;
	double pR;
	double pL;

	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	double Light_trasm;

	/* for check */
	double npp_to_alloc;

	//height_t *h;
	age_t *a;
	species_t *s;

	//h = &c->heights[height];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &a->species[species];

	s0    = s->value[S0CTEM];        /* parameter controlling allocation to stem (minimum ratio to stem pool) */
	r0    = s->value[R0CTEM];        /* parameter controlling allocation to roots (minimum ratio to root pools) */
	omega = s->value[OMEGA_CTEM]; /* controls the sensitivity of allocation to changes in water and light availability */

	//fixme it should takes into account above layers
	Light_trasm = exp(- s->value[K] * s->value[LAI_PROJ]);

	/* Marconi: here the allocation of biomass reserve is divided in fineroot and leaves following the
	 * allocation ratio parameter between them. That because
	 * in evergreen we don't have bud burst phenology phase, and indeed there are two phenology phases;
	 * the former in which carbon is allocated in fineroot and foliage, the latter in
	 * every pool except foliage*/

	logger(g_debug_log, "\n**C-PARTITIONING-ALLOCATION**\n");
	logger(g_debug_log, "Carbon partitioning for evergreen\n");

	/* partitioning block using approach of Potter et al., 1993, Schwalm & Ek, 2004; Arora and Boer 2005 */
	logger(g_debug_log, "*Partitioning ratios*\n");

	/* roots */
	pR = (r0 + (omega * ( 1.0 - s->value[F_SW]))) / (1. + (omega * (2. - Light_trasm - s->value[F_SW])));
	logger(g_debug_log, "Roots CTEM ratio = %g %%\n", pR * 100.);

	/* stem */
	pS = (s0 + (omega * ( 1.0 - Light_trasm))) / (1. + ( omega * (2. - Light_trasm - s->value[F_SW])));
	logger(g_debug_log, "Stem CTEM ratio = %g %%\n", pS * 100.);

	/* reserve and leaves */
	pL = (1. - pS - pR);
	logger(g_debug_log, "Reserve CTEM ratio = %g %%\n", pL * 100.);
	CHECK_CONDITION( fabs ( pR + pS + pL ), >, 1 + eps );

	logger(g_debug_log, "\nCarbon allocation for evergreen\n");
	logger(g_debug_log, "PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
	logger(g_debug_log, "LAI_PROJ = %g\n", s->value[LAI_PROJ]);
	logger(g_debug_log, "PEAK_LAI_PROJ = %g \n", s->value[PEAK_LAI_PROJ]);

	/* assign NPP to local variables */
	npp_to_alloc = s->value[GPP_tC] - s->value[TOTAL_MAINT_RESP_tC] ;

	/* note: none carbon pool is refilled if reserve is lower than minimum */
	/* reserves have priority before all other pools */

	switch ( s->phenology_phase )
	{
	/************************************************************/
	case 1:
		/*
		 * this phenological phase happens when:
		 * - month < 6 for north hemisphere
		 * - LAI < PEAK_LAI
		 * */

		/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
		 * and Barbaroux et al., 2002, the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
		 * the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
		 * sharing the daily remaining amount (taking into account respiration costs)of NSC */

		/*note: every day of the year leaf and fine root turnover happens, that's why every day
		 * an amount of leaf and fine root C is re-translocated to the reserve pool*/

		/* partitioning */
		if (npp_to_alloc > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
				logger(g_debug_log, "Allocating only into foliage and fine root pools (positive NPP)\n");
				s->value[C_TO_LEAF]    = npp_to_alloc * (1. - s->value[FROOT_LEAF_FRAC]);
				s->value[C_TO_FROOT]   = npp_to_alloc * s->value[FROOT_LEAF_FRAC];
			}
			/* it needs */
			else
			{
				logger(g_debug_log, "Allocating only into reserve pool (low reserves, positive NPP)\n");
				s->value[C_TO_RESERVE] = npp_to_alloc;
			}
		}
		else
		{
			logger(g_debug_log, "Consuming reserve pool (negative NPP)\n");
			s->value[C_TO_RESERVE]     = npp_to_alloc;
		}
		/**********************************************************************/
		break;
	case 2:
		/*
		 * this phenological phase happens when:
		 * - month > 6 for north hemisphere
		 * - LAI > PEAK_LAI
		 */

		/* partitioning */
		if (npp_to_alloc > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
				logger(g_debug_log, "Allocating only into Coarse root, Reserve, Stem and Branch pools (positive NPP)\n");

				/* reproduction */
				if ( g_settings->regeneration && ( a->value > s->value[SEXAGE] ) )
				{
					logger(g_debug_log, "allocating into fruit pool\n");

					s->value[C_TO_FRUIT] = npp_to_alloc * s->value[FRUIT_PERC];
					npp_to_alloc -= s->value[C_TO_FRUIT];
				}
				s->value[C_TO_CROOT]     = (npp_to_alloc * pR);
				s->value[C_TO_RESERVE]   = (npp_to_alloc * pL);
				s->value[C_TO_TOT_STEM]  = (npp_to_alloc * pS);
				s->value[C_TO_STEM]      = (npp_to_alloc * pS) * (1. - s->value[FRACBB]);
				s->value[C_TO_BRANCH]    = (npp_to_alloc * pS) * s->value[FRACBB];
			}
			/* it needs */
			else
			{
				logger(g_debug_log, "Allocating only into reserve pool (low reserves, positive NPP)\n");
				s->value[C_TO_RESERVE]   = npp_to_alloc;
			}
		}
		else
		{
			logger(g_debug_log, "Consuming reserve pool (negative NPP)\n");
			s->value[C_TO_RESERVE]       = npp_to_alloc;
		}
		/**********************************************************************/
		break;
	}

	/* check for daily growth efficiency mortality */
	//note: since forest structure works on annual scale it must be turned off
	//daily_growth_efficiency_mortality ( c, height, dbh, age, species );

	/* update live_total wood fraction based on age */
	live_total_wood_age ( a, s );

	/* growth respiration */
	growth_respiration ( c, layer, height, dbh, age, species );

	/* leaf and fine root fall */
	leaf_fall_evergreen ( c, height, dbh, age, species );

	/* allocate daily carbon */
	carbon_allocation ( c, s );

	/* update Leaf Area Index */
	daily_lai ( s );

	/* allocate daily nitrogen */
	nitrogen_allocation ( c, s );

	/* compute single tree biomass pools */
	average_tree_pools ( s );

	/* to avoid "jumps" of dbh it has computed once monthly */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		/* old */
		dendrometry_old ( c, layer, height, dbh, age, species);
	}
#if 0
	dendrometry ( c, layer, height, dbh, age, species, meteo_daily, month, year );
#endif

	/* turnover */
	turnover ( s );

	/* update class level month carbon biomass increment in tC/month/cell */
	s->value[M_C_TO_STEM]       += s->value[C_TO_STEM];
	s->value[M_C_TO_LEAF]       += s->value[C_TO_LEAF];
	s->value[M_C_TO_FROOT]      += s->value[C_TO_FROOT];
	s->value[M_C_TO_CROOT]      += s->value[C_TO_CROOT];
	s->value[M_C_TO_RESERVE]    += s->value[C_TO_RESERVE];
	s->value[M_C_TO_ROOT]       += s->value[C_TO_ROOT];
	s->value[M_C_TO_BRANCH]     += s->value[C_TO_BRANCH];
	s->value[M_C_TO_FRUIT]      += s->value[C_TO_FRUIT];
	s->value[M_C_TO_TOT_STEM]   += s->value[C_TO_TOT_STEM];

	/* update class level annual carbon biomass increment in tC/year/cell */
	s->value[Y_C_TO_STEM]         += s->value[C_TO_STEM];
	s->value[Y_C_TO_LEAF]         += s->value[C_TO_LEAF];
	s->value[Y_C_TO_FROOT]        += s->value[C_TO_FROOT];
	s->value[Y_C_TO_CROOT]        += s->value[C_TO_CROOT];
	s->value[Y_C_TO_RESERVE]      += s->value[C_TO_RESERVE];
	s->value[Y_C_TO_ROOT]         += s->value[C_TO_ROOT];
	s->value[Y_C_TO_BRANCH]       += s->value[C_TO_BRANCH];
	s->value[Y_C_TO_FRUIT]        += s->value[C_TO_FRUIT];
	s->value[Y_C_TO_TOT_STEM]     += s->value[C_TO_TOT_STEM];

	/* update cell level carbon biomass in gC/m2/day */
	c->daily_leaf_carbon           += s->value[C_TO_LEAF]    * 1e6 / g_settings->sizeCell ;
	c->daily_stem_carbon           += s->value[C_TO_STEM]    * 1e6 / g_settings->sizeCell ;
	c->daily_froot_carbon          += s->value[C_TO_FROOT]   * 1e6 / g_settings->sizeCell ;
	c->daily_croot_carbon          += s->value[C_TO_CROOT]   * 1e6 / g_settings->sizeCell ;
	c->daily_branch_carbon         += s->value[C_TO_BRANCH]  * 1e6 / g_settings->sizeCell ;
	c->daily_reserve_carbon        += s->value[C_TO_RESERVE] * 1e6 / g_settings->sizeCell ;
	c->daily_root_carbon           += s->value[C_TO_ROOT]    * 1e6 / g_settings->sizeCell ;
	c->daily_fruit_carbon          += s->value[C_TO_FRUIT]   * 1e6 / g_settings->sizeCell ;
	c->daily_litr_carbon           += (s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR]  + s->value[C_FRUIT_TO_LITR]) * 1e6 / g_settings->sizeCell ;
	c->daily_soil_carbon           += 0. ;


	/* update cell level carbon biomass in tC/cell/day */
	c->daily_leaf_carbon_tC        += s->value[C_TO_LEAF];
	c->daily_stem_carbon_tC        += s->value[C_TO_STEM];
	c->daily_froot_carbon_tC       += s->value[C_TO_FROOT];
	c->daily_croot_carbon_tC       += s->value[C_TO_CROOT];
	c->daily_branch_carbon_tC      += s->value[C_TO_BRANCH];
	c->daily_reserve_carbon_tC     += s->value[C_TO_RESERVE];
	c->daily_root_carbon_tC        += s->value[C_TO_ROOT];
	c->daily_fruit_carbon_tC       += s->value[C_TO_FRUIT];
	c->daily_litter_carbon_tC      += s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR] + s->value[C_TO_FRUIT];
	c->daily_soil_carbon_tC        += 0.;
	logger(g_debug_log, "******************************\n");
}

/**/

