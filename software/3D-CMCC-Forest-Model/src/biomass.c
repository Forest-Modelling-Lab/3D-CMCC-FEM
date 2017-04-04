/* biomass.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "biomass.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;

void live_total_wood_age(const age_t *const a, species_t *const s)
{
	/* this function update based on current tree age the amount of live:total wood ratio
	 * based on the assumption that the live wood decrease linearly increasing age */
	/* e.g. for Fagus sylvatica  base on simulation for Hesse site (age 30) and Collelongo site (age 160)*/
	// fixme values should be included in species.txt
	double max_live_total_ratio;
	double min_live_total_ratio;
	double t1;
	double t2;

	int min_age;
	int max_age;

	logger(g_debug_log, "\n*live:total wood ratio based on tree age*\n");

	max_age = (int)s->value[MAXAGE];

	//fixme values should be included in species.txt
	/* age at which live:total wood ratio is maximum */
	min_age = 25;
	//logger(g_debug_log, "min age = %d\n", min_age);

	max_live_total_ratio = s->value[LIVE_TOTAL_WOOD]; /* for min_age = 1 */

	//fixme values should be included in species.txt
	min_live_total_ratio = 0.03; /* for max_age = 160 */

	t1 = max_live_total_ratio - min_live_total_ratio;
	t2 = max_age - min_age;

	s->value[EFF_LIVE_TOTAL_WOOD_FRAC] = (t1/t2)*(max_age - a->value) + min_live_total_ratio;
	//logger(g_debug_log, "Effective live:total wood fraction based on stand age = %g\n", s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
}


void annual_tree_increment(cell_t *const c, const int height, const int dbh, const int age, const int species, const int year)
{
	double prev_vol;
	double single_tree_prev_vol;

	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	// ALESSIOR
	//if ( ! a ) return;
	s = &a->species[species];

	/* in m^3/cell/yr */
	/* CAI = Volume t1 - Volume t0 */
	/* MAI = Volume t1 / Age t1 */

	/* CURRENT ANNUAL INCREMENT-CAI */

	if ( !year )
	{
		logger(g_debug_log, "***CAI & MAI***\n");

		/* STAND VOLUME-(STEM VOLUME) */
		/* assign previous volume to temporary variables */
		prev_vol = s->value[VOLUME];
		logger(g_debug_log, "-previous stand volume = %g m^3/cell\n", prev_vol );

		single_tree_prev_vol = s->value[TREE_VOLUME];
		logger(g_debug_log, "-previous single tree volume = %.8f m^3/tree\n", single_tree_prev_vol );

		/* compute current stand level volume */
		s->value[VOLUME] = ( s->value[STEM_C] * GC_GDM ) / s->value[MASS_DENSITY];
		logger(g_debug_log, "-current stand volume = %g m^3/cell\n", s->value[VOLUME] );

		/* compute current stand level volume */
		s->value[TREE_VOLUME] = s->value[VOLUME] / (int) s->counter[N_TREE];
		logger(g_debug_log, "-current single tree volume = %.8f m^3/tree\n", s->value[TREE_VOLUME] );

		/* CAI-Current Annual Increment */
		s->value[CAI] = s->value[VOLUME] - prev_vol;
		logger(g_debug_log, "CAI-Current Annual Increment = %g m^3/cell/yr\n", s->value[CAI]);

		/* MAI-Mean Annual Increment */
		s->value[MAI] = s->value[VOLUME] / (double)a->value;
		logger(g_debug_log, "MAI-Mean Annual Increment = %g m^3/cell/yr \n", s->value[MAI]);

		/* check */
		CHECK_CONDITION(s->value[TREE_VOLUME], <, single_tree_prev_vol - eps);
	}
}

void abg_bgb_biomass(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;

	// ALESSIOR
	/*
	if ( ! c->heights ) return;
	if ( ! c->heights[height].dbhs ) return;
	if ( ! c->heights[height].dbhs[dbh].ages ) return;
	if ( ! c->heights[height].dbhs[dbh].ages[age].species ) return;
	 */

	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "**AGB & BGB**\n");
	logger(g_debug_log, "-for Class\n");
	s->value[CLASS_AGB] = s->value[TOT_STEM_C] + s->value[LEAF_C];
	logger(g_debug_log, "Yearly Class AGB = %g tC/cell year\n", s->value[CLASS_AGB]);
	s->value[CLASS_BGB] = s->value[TOT_ROOT_C];
	logger(g_debug_log, "Yearly Class BGB = %g tC/cell year\n", s->value[CLASS_BGB]);
}

void average_tree_pools(species_t *const s)
{
	/* compute tree average C biomass */
	s->value[AV_LEAF_MASS_KgC]             = (s->value[LEAF_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_STEM_MASS_KgC]             = (s->value[STEM_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_ROOT_MASS_KgC]             = (s->value[TOT_ROOT_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_FROOT_MASS_KgC]            = (s->value[FROOT_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_CROOT_MASS_KgC]            = (s->value[CROOT_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_RESERVE_MASS_KgC]          = (s->value[RESERVE_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_BRANCH_MASS_KgC]           = (s->value[BRANCH_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_FRUIT_MASS_KgC]            = (s->value[FRUIT_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_LIVE_STEM_MASS_KgC]        = (s->value[STEM_LIVE_WOOD_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_DEAD_STEM_MASS_KgC]        = (s->value[STEM_DEAD_WOOD_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_LIVE_CROOT_MASS_KgC]       = (s->value[CROOT_LIVE_WOOD_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_DEAD_CROOT_MASS_KgC]       = (s->value[CROOT_DEAD_WOOD_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_LIVE_BRANCH_MASS_KgC]      = (s->value[BRANCH_LIVE_WOOD_C]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_DEAD_BRANCH_MASS_KgC]      = (s->value[BRANCH_DEAD_WOOD_C]/(double)s->counter[N_TREE]) * 1e3;

	/* compute tree average N biomass */
	s->value[AV_LEAF_MASS_KgN]             = (s->value[LEAF_N]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_STEM_MASS_KgN]             = (s->value[STEM_N]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_FROOT_MASS_KgN]            = (s->value[FROOT_N]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_CROOT_MASS_KgN]            = (s->value[CROOT_N]/(double)s->counter[N_TREE]) * 1e3;
	s->value[AV_BRANCH_MASS_KgN]           = (s->value[BRANCH_N]/(double)s->counter[N_TREE]) * 1e3;

}


void tree_branch_and_bark (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	// ALESSIOR
	//if ( ! a ) return;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* compute branch and bark fractions */

	if ( !s->value[FRACBB0] ) s->value[FRACBB0] = s->value[FRACBB1];
	else s->value[FRACBB] = s->value[FRACBB1] + ( s->value[FRACBB0]- s->value[FRACBB1] )* exp( -ln2 * ( a->value / s->value[TBB] ) );
}

void tree_biomass_remove (cell_t *const c, species_t *const s, const int tree_remove)
{
	average_tree_pools ( s );

	/* update class carbon pools */

	s->value[LEAF_C]                  -= (s->value[AV_LEAF_MASS_KgC]             / 1e3 * tree_remove);
	s->value[FROOT_C]                 -= (s->value[AV_FROOT_MASS_KgC]            / 1e3 * tree_remove);
	s->value[STEM_C]                  -= (s->value[AV_STEM_MASS_KgC]             / 1e3 * tree_remove);
	s->value[CROOT_C]                 -= (s->value[AV_CROOT_MASS_KgC]            / 1e3 * tree_remove);
	s->value[RESERVE_C]               -= (s->value[AV_RESERVE_MASS_KgC]          / 1e3 * tree_remove);
	s->value[BRANCH_C]                -= (s->value[AV_BRANCH_MASS_KgC]           / 1e3 * tree_remove);
	s->value[FRUIT_C]                 -= (s->value[AV_FRUIT_MASS_KgC]            / 1e3 * tree_remove);
	s->value[STEM_SAPWOOD_C]          -= (s->value[AV_STEM_SAPWOOD_MASS_KgC]     / 1e3 * tree_remove);
	s->value[STEM_HEARTWOOD_C]        -= (s->value[AV_STEM_HEARTWOOD_MASS_KgC]   / 1e3 * tree_remove);
	s->value[STEM_LIVE_WOOD_C]        -= (s->value[AV_LIVE_STEM_MASS_KgC]        / 1e3 * tree_remove);
	s->value[STEM_DEAD_WOOD_C]        -= (s->value[AV_DEAD_STEM_MASS_KgC]        / 1e3 * tree_remove);
	s->value[CROOT_SAPWOOD_C]         -= (s->value[AV_CROOT_SAPWOOD_MASS_KgC]    / 1e3 * tree_remove);
	s->value[CROOT_HEARTWOOD_C]       -= (s->value[AV_CROOT_HEARTWOOD_MASS_KgC]  / 1e3 * tree_remove);
	s->value[CROOT_LIVE_WOOD_C]       -= (s->value[AV_LIVE_CROOT_MASS_KgC]       / 1e3 * tree_remove);
	s->value[CROOT_DEAD_WOOD_C]       -= (s->value[AV_DEAD_CROOT_MASS_KgC]       / 1e3 * tree_remove);
	s->value[BRANCH_SAPWOOD_C]        -= (s->value[AV_BRANCH_SAPWOOD_MASS_KgC]   / 1e3 * tree_remove);
	s->value[BRANCH_HEARTWOOD_C]      -= (s->value[AV_BRANCH_HEARTWOOD_MASS_KgC] / 1e3 * tree_remove);
	s->value[BRANCH_LIVE_WOOD_C]      -= (s->value[AV_LIVE_BRANCH_MASS_KgC]      / 1e3 * tree_remove);
	s->value[BRANCH_DEAD_WOOD_C]      -= (s->value[AV_DEAD_BRANCH_MASS_KgC]      / 1e3 * tree_remove);

	/* update class nitrogen pools */
	s->value[STEM_N]                  -= (s->value[AV_STEM_MASS_KgN]             / 1e3 * tree_remove);
	s->value[LEAF_N]                  -= (s->value[AV_LEAF_MASS_KgN]             / 1e3 * tree_remove);
	s->value[FROOT_N]                 -= (s->value[AV_FROOT_MASS_KgN]            / 1e3 * tree_remove);
	s->value[CROOT_N]                 -= (s->value[AV_CROOT_MASS_KgN]            / 1e3 * tree_remove);
	s->value[BRANCH_N]                -= (s->value[AV_BRANCH_MASS_KgN]           / 1e3 * tree_remove);

	/* carbon to litter pool */
	s->value[C_TO_LITR]               += (s->value[AV_LEAF_MASS_KgC]             / 1e3 * tree_remove) +
			(s->value[AV_FROOT_MASS_KgC]                                         / 1e3 * tree_remove);

	/* carbon to cwd pool */
	s->value[C_TO_CWD]                += (s->value[AV_CROOT_MASS_KgC]            / 1e3 * tree_remove) +
			(s->value[AV_STEM_MASS_KgC]                                          / 1e3 * tree_remove) +
			(s->value[AV_RESERVE_MASS_KgC]                                       / 1e3 * tree_remove) +
			(s->value[AV_BRANCH_MASS_KgC]                                        / 1e3 * tree_remove);

	/* nitrogen to litter pool */
	s->value[N_TO_LITTER]             += (s->value[AV_LEAF_MASS_KgN]             / 1e3 * tree_remove) +
			(s->value[AV_FROOT_MASS_KgN]                                         / 1e3 * tree_remove);;

	/* nitrogen to cwd pool */
	s->value[N_TO_CWD]                += (s->value[AV_CROOT_MASS_KgN]            / 1e3 * tree_remove) +
			(s->value[AV_STEM_MASS_KgN]                                          / 1e3 * tree_remove) +
			(s->value[AV_RESERVE_MASS_KgN]                                       / 1e3 * tree_remove) +
			(s->value[AV_BRANCH_MASS_KgN]                                        / 1e3 * tree_remove);

	/*** carbon ***/

	/*** update at cell level ***/
	c->leaf_carbon                    -= (s->value[AV_LEAF_MASS_KgC]             * 1e3 * tree_remove);
	c->stem_carbon                    -= (s->value[AV_STEM_MASS_KgC]             * 1e3 * tree_remove);
	c->froot_carbon                   -= (s->value[AV_FROOT_MASS_KgC]            * 1e3 * tree_remove);
	c->croot_carbon                   -= (s->value[AV_CROOT_MASS_KgC]            * 1e3 * tree_remove);
	c->branch_carbon                  -= (s->value[AV_BRANCH_MASS_KgC]           * 1e3 * tree_remove);
	c->reserve_carbon                 -= (s->value[AV_RESERVE_MASS_KgC]          * 1e3 * tree_remove);
	c->fruit_carbon                   -= (s->value[AV_FRUIT_MASS_KgC]            * 1e3 * tree_remove);
	c->litr_carbon                    += ((s->value[AV_LEAF_MASS_KgC]            * 1e3 * tree_remove))+
			((s->value[AV_FROOT_MASS_KgC]                                        * 1e3 * tree_remove))+
			((s->value[AV_FRUIT_MASS_KgC]                                        * 1e3 * tree_remove));
	c->cwd_carbon                     += ((s->value[AV_STEM_MASS_KgC]            * 1e3 * tree_remove))+
			((s->value[AV_CROOT_MASS_KgC]                                        * 1e3 * tree_remove))+
			((s->value[AV_BRANCH_MASS_KgC]                                       * 1e3 * tree_remove));

	/*** update at cell level ***/
	c->leaf_tC                        -= (s->value[AV_LEAF_MASS_KgC]             / 1e3 * tree_remove);
	c->stem_tC                        -= (s->value[AV_STEM_MASS_KgC]             / 1e3 * tree_remove);
	c->froot_tC                       -= (s->value[AV_FROOT_MASS_KgC]            / 1e3 * tree_remove);
	c->croot_tC                       -= (s->value[AV_CROOT_MASS_KgC]            / 1e3 * tree_remove);
	c->branch_tC                      -= (s->value[AV_BRANCH_MASS_KgC]           / 1e3 * tree_remove);
	c->reserve_tC                     -= (s->value[AV_RESERVE_MASS_KgC]          / 1e3 * tree_remove);
	c->fruit_tC                       -= (s->value[AV_FRUIT_MASS_KgC]            / 1e3 * tree_remove);
	c->litr_tC                        += ((s->value[AV_LEAF_MASS_KgC]            / 1e3 * tree_remove))+
			((s->value[AV_FROOT_MASS_KgC]                                        / 1e3 * tree_remove))+
			((s->value[AV_FRUIT_MASS_KgC]                                        / 1e3 * tree_remove));
	c->cwd_tC                         += ((s->value[AV_STEM_MASS_KgC]            / 1e3 * tree_remove))+
			((s->value[AV_CROOT_MASS_KgC]                                        / 1e3 * tree_remove))+
			((s->value[AV_BRANCH_MASS_KgC]                                       / 1e3 * tree_remove));


	/*** nitrogen ***/

	/*** update at cell level ***/
	c->leaf_nitrogen                  -= (s->value[AV_LEAF_MASS_KgN]             * 1e3 * tree_remove);
	c->stem_nitrogen                  -= (s->value[AV_STEM_MASS_KgN]             * 1e3 * tree_remove);
	c->froot_nitrogen                 -= (s->value[AV_FROOT_MASS_KgN]            * 1e3 * tree_remove);
	c->croot_nitrogen                 -= (s->value[AV_CROOT_MASS_KgN]            * 1e3 * tree_remove);
	c->branch_nitrogen                -= (s->value[AV_BRANCH_MASS_KgN]           * 1e3 * tree_remove);
	c->reserve_nitrogen               -= (s->value[AV_RESERVE_MASS_KgN]          * 1e3 * tree_remove);
	c->fruit_nitrogen                 -= (s->value[AV_FRUIT_MASS_KgN]            * 1e3 * tree_remove);
	c->litr_nitrogen                  += ((s->value[AV_LEAF_MASS_KgN]            * 1e3 * tree_remove))+
			((s->value[AV_FROOT_MASS_KgN]                                        * 1e3 * tree_remove));
	c->cwd_nitrogen                   += ((s->value[AV_STEM_MASS_KgN]            * 1e3 * tree_remove))+
			((s->value[AV_CROOT_MASS_KgN]                                        * 1e3 * tree_remove))+
			((s->value[AV_BRANCH_MASS_KgN]                                       * 1e3 * tree_remove));

	/*** update at cell level ***/
	c->leaf_tN                        -= (s->value[AV_LEAF_MASS_KgN]             / 1e3 * tree_remove);
	c->stem_tN                        -= (s->value[AV_STEM_MASS_KgN]             / 1e3 * tree_remove);
	c->froot_tN                       -= (s->value[AV_FROOT_MASS_KgN]            / 1e3 * tree_remove);
	c->croot_tN                       -= (s->value[AV_CROOT_MASS_KgN]            / 1e3 * tree_remove);
	c->branch_tN                      -= (s->value[AV_BRANCH_MASS_KgN]           / 1e3 * tree_remove);
	c->reserve_tN                     -= (s->value[AV_RESERVE_MASS_KgN]          / 1e3 * tree_remove);
	c->fruit_tN                       -= (s->value[AV_FRUIT_MASS_KgN]            / 1e3 * tree_remove);
	c->litr_tN                        += ((s->value[AV_LEAF_MASS_KgN]            / 1e3 * tree_remove))+
			((s->value[AV_FROOT_MASS_KgN]                                        / 1e3 * tree_remove));
	c->cwd_tN                         += ((s->value[AV_STEM_MASS_KgN]            / 1e3 * tree_remove))+
			((s->value[AV_CROOT_MASS_KgN]                                        / 1e3 * tree_remove))+
			((s->value[AV_BRANCH_MASS_KgN]                                       / 1e3 * tree_remove));


}
