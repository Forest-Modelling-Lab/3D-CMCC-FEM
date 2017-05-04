/* includes */
#include <CN-allocation.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "C-deciduous-partitioning.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "leaf_fall.h"
#include "biomass.h"
#include "dendometry.h"
#include "turnover.h"
#include "lai.h"
#include "mortality.h"
#include "aut_respiration.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;

/* Deciduous carbon allocation routine */
void daily_C_deciduous_partitioning (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species,
		const meteo_daily_t *const meteo_daily, const int day, const int month, const int year)
{

	double s0;
	double r0;
	double omega;
	double pS;
	double pR;
	double pL;
	double Light_trasm;
	static double reserve_to_leaf_budburst;
	static double reserve_to_froot_budburst;
	static double reserve_to_budburst;

	/* for check */
	double npp_to_alloc;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &d->ages[age];
	s = &a->species[species];

	s0    = s->value[S0CTEM];        /* parameter controlling allocation to stem (minimum ratio to stem pool */
	r0    = s->value[R0CTEM];        /* parameter controlling allocation to roots (minimum ratio to root pools */
	omega = s->value[OMEGA_CTEM]; /* controls the sensitivity of allocation to changes in water and light availability */

	//fixme it should takes into account above layers
	Light_trasm = exp(- s->value[K] * s->value[LAI_PROJ]);

	/* note: in Biome a constant proportion (50%) (Growth:storage parameter) of NPP that goes to the c-pools is allocated
	 * to each storage_pool, i.e. each carbon pools receive just a part of NPP (50%) the remaining remain as storage
	 * and used to maintain trees when NPP is < 0 */

	logger(g_debug_log, "\n**C-PARTITIONING**\n");
	logger(g_debug_log, "Carbon partitioning for deciduous\n");

	/* partitioning block using approach of:
	 *  Potter et al., 1993, Friendlingstein et al., 1999;
	 *  Schwalm & Ek, 2004; Arora and Boer 2005 */
	logger(g_debug_log, "*Partitioning ratios*\n");

	/* roots */
	pR = (r0 + (omega * ( 1. - s->value[F_SW]))) / (1. + (omega * (2. - Light_trasm - s->value[F_SW])));
	//logger(g_debug_log, "Roots CTEM ratio = %g %%\n", pR * 100);

	/* stem */
	pS = (s0 + (omega * ( 1. - Light_trasm))) / (1. + ( omega * (2. - Light_trasm - s->value[F_SW])));
	//logger(g_debug_log, "Stem CTEM ratio = %g %%\n", pS * 100. );

	/* reserve and leaves */
	pL = (1. - pS - pR);
	//logger(g_debug_log, "Reserve CTEM ratio = %g %%\n", pL * 100. );
	CHECK_CONDITION( fabs ( pR + pS + pL ), >, 1 + eps );

	/* fine root vs. coarse root ratio */
	s->value[FINE_COARSE_ROOT] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * ( 1. / s->value[STEM_LEAF]);

	if (s->counter[VEG_DAYS] == 1)
	{
		s->counter[BUD_BURST_COUNTER] = (int)s->value[BUD_BURST];
		logger(g_debug_log, "First day of budburst\n");
		logger(g_debug_log, "Days for bud burst = %g\n", s->value[BUD_BURST]);
	}
	if (s->counter[VEG_DAYS] > 1 && s->counter[VEG_DAYS] <= s->value[BUD_BURST])
	{
		s->counter[BUD_BURST_COUNTER] --;
		logger(g_debug_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
	}
	if (s->counter[VEG_DAYS] > s->value[BUD_BURST])
	{
		s->counter[BUD_BURST_COUNTER] = 0;
	}

	logger(g_debug_log, "PHENOLOGICAL PHASE = %d\n", s->phenology_phase);

	if ( g_settings->Prog_Aut_Resp )
	{
		/* assign NPP to local variables and remove the maintenance respiration */
		npp_to_alloc = s->value[GPP_tC] - s->value[TOTAL_MAINT_RESP_tC] ;
	}
	else
	{
		/* assign NPP to local variables and remove the prognostic respiration */
		npp_to_alloc = s->value[GPP_tC] * ( 1. - g_settings->Fixed_Aut_Resp_rate );
	}
	logger(g_debug_log, "npp_to_alloc = %g tC/sizecell/day\n", npp_to_alloc);

	/* note: none carbon pool is refilled if reserve is lower than minimum */
	/* reserves have priority before all other pools */

	switch (s->phenology_phase)
	{
	/************************************************************/
	case 1:
		logger(g_debug_log, "\n*BUDBURST*\n");
		logger(g_debug_log, "Bud burst phase using both reserve pools and npp\n");
		logger(g_debug_log, "LAI_PROJ = %g \n", s->value[LAI_PROJ]);
		logger(g_debug_log, "Allocating only into foliage and fine root\n");
		logger(g_debug_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);

		/* test "This has recently been confirmed by Dyckmans et al. (2000)
		who showed that only 44% of carbon in leaves came from
		carbon reserves in beech trees" (Barbaroux et al., 2003) */

		/* following Friedlingstein et al.,1998 and Krinner et al.,2005 during budburst model allocates to leaf and fine root */
		/* following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/

		/* compute amount of leaf carbon and relative growth respiration amount */
		reserve_to_leaf_budburst   = ( s->value[MAX_LEAF_C] + ( s->value[MAX_LEAF_C] * s->value[EFF_GRPERC] ) ) / (s->value[BUD_BURST] - 1.);

		/* compute amount of fine root carbon and relative growth respiration amount */
		reserve_to_froot_budburst  = ( s->value[MAX_FROOT_C] + ( s->value[MAX_FROOT_C] * s->value[EFF_GRPERC] ) ) / (s->value[BUD_BURST] - 1.);

		reserve_to_budburst        = reserve_to_leaf_budburst + reserve_to_froot_budburst;

		/* update carbon flux */
		s->value[C_TO_LEAF]        = reserve_to_leaf_budburst;
		s->value[C_TO_FROOT]       = reserve_to_froot_budburst;
		s->value[C_TO_RESERVE]     = npp_to_alloc - reserve_to_budburst;

		/********************************************************************************************************************************************/
		/* check for leaf C > max leaf C */
		if ( ( ( s->value[C_TO_LEAF] * ( 1. - s->value[EFF_GRPERC] ) ) + s->value[LEAF_C]) > s->value[MAX_LEAF_C])
		{
			double max_leafC;

			max_leafC = s->value[MAX_LEAF_C] - s->value[LEAF_C];

			s->value[C_TO_LEAF]     = max_leafC + ( max_leafC * s->value[EFF_GRPERC] );
			s->value[C_TO_RESERVE] += ( reserve_to_leaf_budburst - ( max_leafC + ( max_leafC * s->value[EFF_GRPERC] ) ) );
		}

		/* check for fine root C > max fine root C */
		if ( ( (s->value[C_TO_FROOT] * ( 1. - s->value[EFF_GRPERC] ) ) + s->value[FROOT_C] ) > s->value[MAX_FROOT_C])
		{
			double max_frootC;

			max_frootC = s->value[MAX_FROOT_C] - s->value[FROOT_C];

			s->value[C_TO_FROOT]    = max_frootC + ( max_frootC * s->value[EFF_GRPERC] );
			s->value[C_TO_RESERVE] += ( reserve_to_froot_budburst  - ( max_frootC + ( max_frootC * s->value[EFF_GRPERC] ) ) );
		}
		/********************************************************************************************************************************************/

		break;
		/**********************************************************************/
	case 2:
		logger(g_debug_log, "\n*NORMAL GROWTH*\n");
		logger(g_debug_log, "(LAI == PEAK LAI)\n");

		/* see Barbaroux et al., 2002, Scartazza et al., 2013 */

		if (npp_to_alloc > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
#if 1
				/* as in Bossel (1996), second priority is assigned to fruit */
				/* reproduction */
				if ( a->value > s->value[SEXAGE] )
				{
					logger(g_debug_log, "allocating into fruit pool\n");

					s->value[C_TO_FRUIT] = npp_to_alloc * s->value[FRUIT_PERC];
					npp_to_alloc        -= s->value[C_TO_FRUIT];
				}
				else
				{
					s->value[C_TO_FRUIT] = 0.;
				}
#endif
				logger(g_debug_log, "allocating into the three pools Ws(Ws+Wbb)+Wr(Wrc)+Wreserve\n");

				/* allocating into c pools */
				s->value[C_TO_RESERVE]     = (npp_to_alloc * pL);
				s->value[C_TO_CROOT]       = (npp_to_alloc * pR);
				s->value[C_TO_STEM]        = (npp_to_alloc * pS) * (1. - s->value[FRACBB]);
				s->value[C_TO_BRANCH]      = (npp_to_alloc * pS) * s->value[FRACBB];
			}
			/* it needs */
			else
			{
				logger(g_debug_log, "allocating into reserve\n");

				/* allocating into c pools */
				s->value[C_TO_RESERVE]     = npp_to_alloc;
			}
		}
		else
		{
			logger(g_debug_log, "Consuming reserve pool (negative NPP)\n");

			/* allocating into c pools */
			s->value[C_TO_RESERVE]     = npp_to_alloc;
		}

		break;
		/**********************************************************************/
	case 3:
		logger(g_debug_log, "\n*LEAF FALL*\n");
		logger(g_debug_log, "(DayLength < MINDAYLENGTH)\n");
		logger(g_debug_log, "allocating into W reserve pool\n");

		/* allocating into c pools */
		/* including retranslocated C */
		s->value[C_TO_RESERVE] = npp_to_alloc ;

		/* leaf fall */
		leaffall_deciduous( c, height, dbh, age, species );

		break;
		/**********************************************************************/
	case 0:
		logger(g_debug_log, "\n*DORMANT SEASON*\n");
		logger(g_debug_log, "consuming reserve pool\n");

		/* allocating into c pools */
		s->value[C_TO_RESERVE]     = npp_to_alloc;

		break;
		/**********************************************************************/
	}


	/* update live_total wood fraction based on age */
	live_total_wood_age ( a, s );
}




