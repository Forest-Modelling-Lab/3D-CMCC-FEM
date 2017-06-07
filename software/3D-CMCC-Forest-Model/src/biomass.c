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
#if 0
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

	max_live_total_ratio = s->value[LIVE_TOTAL_WOOD];

	//fixme values should be included in species.txt
	min_live_total_ratio = 0.03;

	t1 = max_live_total_ratio - min_live_total_ratio;
	t2 = max_age - min_age;

	s->value[EFF_LIVE_TOTAL_WOOD_FRAC] = (t1/t2)*(max_age - a->value) + min_live_total_ratio;
#else
	s->value[EFF_LIVE_TOTAL_WOOD_FRAC] = s->value[LIVE_TOTAL_WOOD];
#endif
	logger(g_debug_log, "Effective live:total wood fraction based on stand age = %g\n", s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
}


void abg_bgb_biomass(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double old_agb;       //fixme this should be moved to matrix since classes can change
	static double old_bgb;       //fixme this should be moved to matrix since classes can change
	static double tree_old_agb;  //fixme this should be moved to matrix since classes can change
	static double tree_old_bgb;  //fixme this should be moved to matrix since classes can change

	species_t *s;

	//ALESSIOR
	/*
	if ( ! c->heights ) return;
	if ( ! c->heights[height].dbhs ) return;
	if ( ! c->heights[height].dbhs[dbh].ages ) return;
	if ( ! c->heights[height].dbhs[dbh].ages[age].species ) return;
	 */
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "**AGB & BGB**\n");

	old_agb = s->value[AGB];
	old_bgb = s->value[BGB];
	tree_old_agb = s->value[TREE_AGB];
	tree_old_bgb = s->value[TREE_BGB];

	s->value[AGB]            = s->value[LEAF_C]  + s->value[STEM_C] + s->value[BRANCH_C] +s->value[FRUIT_C];
	logger(g_debug_log, "AGB              = %f tC/cell\n", s->value[AGB]);

	s->value[BGB]            = s->value[FROOT_C] + s->value[CROOT_C];
	logger(g_debug_log, "BGB              = %f tC/cell\n", s->value[BGB]);

	s->value[DELTA_AGB]      = s->value[AGB] - old_agb;
	logger(g_debug_log, "DELTA_AGB        = %f tC/cell/year\n", s->value[DELTA_AGB]);

	s->value[DELTA_BGB]      = s->value[BGB] - old_bgb;
	logger(g_debug_log, "DELTA_BGB        = %f tC/cell/year\n", s->value[DELTA_BGB]);

	s->value[TREE_AGB]       = s->value[AGB] / (double)s->counter[N_TREE];
	logger(g_debug_log, "Yearly Class AGB = %f tC/tree\n", s->value[TREE_AGB]);

	s->value[TREE_BGB]       = s->value[BGB] / (double)s->counter[N_TREE];
	logger(g_debug_log, "Yearly Class BGB = %f tC/tree\n", s->value[TREE_BGB]);

	s->value[DELTA_TREE_AGB] = s->value[TREE_AGB] - tree_old_agb;
	logger(g_debug_log, "DELTA_TREE_AGB   = %f tC/tree/year\n", s->value[DELTA_TREE_AGB]);

	s->value[DELTA_TREE_BGB] = s->value[TREE_BGB] - tree_old_bgb;
	logger(g_debug_log, "DELTA_TREE_BGB   = %f tC/tree/year\n", s->value[DELTA_TREE_BGB]);
}

void average_tree_pools(species_t *const s)
{
	logger(g_debug_log, "\n*AVERAGE TREE POOLS*\n");
	logger(g_debug_log,  "N_TREE = %d\n", s->counter[N_TREE]);

	/* note: it takes into account previous biomass to remove if function is called twice in a day (e.g.

	/* compute tree average C pools */
	s->value[TREE_LEAF_C]                = (s->value[LEAF_C]             / (double)s->counter[N_TREE]);
	s->value[TREE_STEM_C]                = (s->value[STEM_C]             / (double)s->counter[N_TREE]);
	s->value[TREE_FROOT_C]               = (s->value[FROOT_C]            / (double)s->counter[N_TREE]);
	s->value[TREE_CROOT_C]               = (s->value[CROOT_C]            / (double)s->counter[N_TREE]);
	s->value[TREE_RESERVE_C]             = (s->value[RESERVE_C]          / (double)s->counter[N_TREE]);
	s->value[TREE_BRANCH_C]              = (s->value[BRANCH_C]           / (double)s->counter[N_TREE]);
	s->value[TREE_FRUIT_C]               = (s->value[FRUIT_C]            / (double)s->counter[N_TREE]);
	s->value[TREE_STEM_SAPWOOD_C]        = (s->value[STEM_SAPWOOD_C]     / (double)s->counter[N_TREE]);
	s->value[TREE_STEM_HEARTWOOD_C]      = (s->value[STEM_HEARTWOOD_C]   / (double)s->counter[N_TREE]);
	s->value[TREE_CROOT_SAPWOOD_C]       = (s->value[CROOT_SAPWOOD_C]    / (double)s->counter[N_TREE]);
	s->value[TREE_CROOT_HEARTWOOD_C]     = (s->value[CROOT_HEARTWOOD_C]  / (double)s->counter[N_TREE]);
	s->value[TREE_BRANCH_SAPWOOD_C]      = (s->value[BRANCH_SAPWOOD_C]   / (double)s->counter[N_TREE]);
	s->value[TREE_BRANCH_HEARTWOOD_C]    = (s->value[BRANCH_HEARTWOOD_C] / (double)s->counter[N_TREE]);
	s->value[TREE_SAPWOOD_C]             = (s->value[SAPWOOD_C]          / (double)s->counter[N_TREE]);
	s->value[TREE_HEARTWOOD_C]           = (s->value[HEARTWOOD_C]        / (double)s->counter[N_TREE]);
	s->value[TREE_STEM_LIVEWOOD_C]       = (s->value[STEM_LIVEWOOD_C]    / (double)s->counter[N_TREE]);
	s->value[TREE_STEM_DEADWOOD_C]       = (s->value[STEM_DEADWOOD_C]    / (double)s->counter[N_TREE]);
	s->value[TREE_CROOT_LIVEWOOD_C]      = (s->value[CROOT_LIVEWOOD_C]   / (double)s->counter[N_TREE]);
	s->value[TREE_CROOT_DEADWOOD_C]      = (s->value[CROOT_DEADWOOD_C]   / (double)s->counter[N_TREE]);
	s->value[TREE_BRANCH_LIVEWOOD_C]     = (s->value[BRANCH_LIVEWOOD_C]  / (double)s->counter[N_TREE]);
	s->value[TREE_BRANCH_DEADWOOD_C]     = (s->value[BRANCH_DEADWOOD_C]  / (double)s->counter[N_TREE]);
	s->value[TREE_LIVEWOOD_C]            = (s->value[LIVEWOOD_C]         / (double)s->counter[N_TREE]);
	s->value[TREE_DEADWOOD_C]            = (s->value[DEADWOOD_C]         / (double)s->counter[N_TREE]);

	/* compute tree average N pools */
	s->value[TREE_LEAF_N]                = (s->value[LEAF_N]             / (double)s->counter[N_TREE]);
	s->value[TREE_STEM_N]                = (s->value[STEM_N]             / (double)s->counter[N_TREE]);
	s->value[TREE_FROOT_N]               = (s->value[FROOT_N]            / (double)s->counter[N_TREE]);
	s->value[TREE_CROOT_N]               = (s->value[CROOT_N]            / (double)s->counter[N_TREE]);
	s->value[TREE_BRANCH_N]              = (s->value[BRANCH_N]           / (double)s->counter[N_TREE]);
	s->value[TREE_RESERVE_N]             = (s->value[RESERVE_N]          / (double)s->counter[N_TREE]);
	s->value[TREE_FRUIT_N]               = (s->value[FRUIT_N]            / (double)s->counter[N_TREE]);

	logger(g_debug_log, "TREE_LEAF_C    = %f tC/tree\n", s->value[TREE_LEAF_C]);
	logger(g_debug_log, "TREE_STEM_C    = %f tC/tree\n", s->value[TREE_STEM_C]);
	logger(g_debug_log, "TREE_FROOT_C   = %f tC/tree\n", s->value[TREE_FROOT_C]);
	logger(g_debug_log, "TREE_CROOT_C   = %f tC/tree\n", s->value[TREE_CROOT_C]);
	logger(g_debug_log, "TREE_RESERVE_C = %f tC/tree\n", s->value[TREE_RESERVE_C]);
	logger(g_debug_log, "TREE_BRANCH_C  = %f tC/tree\n", s->value[TREE_BRANCH_C]);
	logger(g_debug_log, "TREE_FRUIT_C   = %f tC/tree\n", s->value[TREE_FRUIT_C]);


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
	else s->value[FRACBB] = s->value[FRACBB1] + ( s->value[FRACBB0] - s->value[FRACBB1] )* exp( -LN2 * ( (double)a->value / s->value[TBB] ) );
}

void tree_biomass_remove (cell_t *const c, const int height, const int dbh, const int age, const int species, const int tree_remove)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* compute single tree average biomass */
	average_tree_pools ( s );

	logger(g_debug_log, "\n*TREE BIOMASS REMOVE*\n");

	/* update class carbon pools */

	/* carbon to litter fluxes */
	s->value[C_LEAF_TO_LITR]          += (s->value[TREE_LEAF_C]    * tree_remove);

	s->value[C_FROOT_TO_LITR]         += (s->value[TREE_FROOT_C]   * tree_remove);

	/* overall litter */
	s->value[C_TO_LITR]               += s->value[C_LEAF_TO_LITR] +
			s->value[C_FROOT_TO_LITR];

	/* carbon to cwd fluxes */
	s->value[C_STEM_TO_CWD]           += (s->value[TREE_STEM_C]    * tree_remove);

	s->value[C_CROOT_TO_CWD]          += (s->value[TREE_CROOT_C]   * tree_remove);

	s->value[C_BRANCH_TO_CWD]         += (s->value[TREE_BRANCH_C]  * tree_remove);

	s->value[C_RESERVE_TO_CWD]        += (s->value[TREE_RESERVE_C] * tree_remove);

	s->value[C_FRUIT_TO_CWD]          += (s->value[TREE_FRUIT_C]   * tree_remove);

	/* overall cwd */
	s->value[C_TO_CWD]                += (s->value[C_STEM_TO_CWD] +
			s->value[C_CROOT_TO_CWD]  +
			s->value[C_BRANCH_TO_CWD] +
			s->value[C_RESERVE_TO_CWD]+
			s->value[C_FRUIT_TO_CWD]) ;

	/* sapwood and heartwood */
	s->value[C_STEM_SAPWOOD_TO_CWD]     += (s->value[TREE_STEM_SAPWOOD_C]   * tree_remove);

	s->value[C_CROOT_SAPWOOD_TO_CWD]    += (s->value[TREE_CROOT_SAPWOOD_C]  * tree_remove);

	s->value[C_BRANCH_SAPWOOD_TO_CWD]   += (s->value[TREE_BRANCH_SAPWOOD_C] * tree_remove);

	s->value[C_STEM_HEARTWOOD_TO_CWD]   += (s->value[TREE_STEM_HEARTWOOD_C]   * tree_remove);

	s->value[C_CROOT_HEARTWOOD_TO_CWD]  += (s->value[TREE_CROOT_HEARTWOOD_C]  * tree_remove);

	s->value[C_BRANCH_HEARTWOOD_TO_CWD] += (s->value[TREE_BRANCH_HEARTWOOD_C] * tree_remove);

	logger(g_debug_log, "Carbon biomass to remove for trees = %d\n", tree_remove);
	logger(g_debug_log, "C_LEAF_TO_LITR            = %f tC/cell\n", s->value[C_LEAF_TO_LITR]);
	logger(g_debug_log, "C_FROOT_TO_LITR           = %f tC/cell\n", s->value[C_FROOT_TO_LITR]);
	logger(g_debug_log, "C_TO_LITR                 = %f tC/cell\n", s->value[C_TO_LITR]);
	logger(g_debug_log, "C_STEM_TO_CWD             = %f tC/cell\n", s->value[C_STEM_TO_CWD]);
	logger(g_debug_log, "C_CROOT_TO_CWD            = %f tC/cell\n", s->value[C_CROOT_TO_CWD]);
	logger(g_debug_log, "C_BRANCH_TO_CWD           = %f tC/cell\n", s->value[C_BRANCH_TO_CWD]);
	logger(g_debug_log, "C_BRANCH_TO_CWD           = %f tC/cell\n", s->value[C_BRANCH_TO_CWD]);
	logger(g_debug_log, "C_FRUIT_TO_CWD            = %f tC/cell\n", s->value[C_FRUIT_TO_CWD]);
	logger(g_debug_log, "C_TO_CWD                  = %f tC/cell\n", s->value[C_TO_CWD]);
	logger(g_debug_log, "C_STEM_SAPWOOD_TO_CWD     = %f tC/cell\n", s->value[C_STEM_SAPWOOD_TO_CWD]);
	logger(g_debug_log, "C_CROOT_SAPWOOD_TO_CWD    = %f tC/cell\n", s->value[C_CROOT_SAPWOOD_TO_CWD]);
	logger(g_debug_log, "C_BRANCH_SAPWOOD_TO_CWD   = %f tC/cell\n", s->value[C_TO_CWD]);
	logger(g_debug_log, "C_STEM_HEARTWOOD_TO_CWD   = %f tC/cell\n", s->value[C_STEM_HEARTWOOD_TO_CWD]);
	logger(g_debug_log, "C_CROOT_HEARTWOOD_TO_CWD  = %f tC/cell\n", s->value[C_CROOT_HEARTWOOD_TO_CWD]);
	logger(g_debug_log, "C_BRANCH_HEARTWOOD_TO_CWD = %f tC/cell\n", s->value[C_TO_CWD]);

	/******************************************************************************************/

	/* note: special case for turnover when mortality and thinning management happen */
	/* removing biomass to NOT consider in turnover of the subsequent year */
	s->value[YEARLY_C_TO_STEM]   -= ((s->value[YEARLY_C_TO_STEM]   / s->counter[N_TREE]) * tree_remove);
	s->value[YEARLY_C_TO_CROOT]  -= ((s->value[YEARLY_C_TO_CROOT]  / s->counter[N_TREE]) * tree_remove);
	s->value[YEARLY_C_TO_BRANCH] -= ((s->value[YEARLY_C_TO_BRANCH] / s->counter[N_TREE]) * tree_remove);

	/******************************************************************************************/

	/* nitrogen to litter pool */
	s->value[N_LEAF_TO_LITR]          += (s->value[TREE_LEAF_N]       * tree_remove);

	s->value[N_FROOT_TO_LITR]         += (s->value[TREE_FROOT_N]      * tree_remove);

	/* overall litter */
	s->value[N_TO_LITR]               += s->value[N_LEAF_TO_LITR] +
			s->value[N_FROOT_TO_LITR];

	s->value[LITR_N]                  += s->value[N_TO_LITR];

	/* nitrogen to cwd fluxes */
	s->value[N_STEM_TO_CWD]           += (s->value[TREE_STEM_N]       * tree_remove);

	s->value[N_CROOT_TO_CWD]          += (s->value[TREE_CROOT_N]      * tree_remove);

	s->value[N_BRANCH_TO_CWD]         += (s->value[TREE_BRANCH_N]     * tree_remove);

	s->value[N_BRANCH_TO_CWD]         += (s->value[TREE_RESERVE_N]    * tree_remove);

	s->value[N_FRUIT_TO_CWD]          += (s->value[TREE_FRUIT_N]      * tree_remove);

	s->value[N_RESERVE_TO_CWD]        += (s->value[TREE_RESERVE_N]    * tree_remove);

	/* overall cwd */
	s->value[N_TO_CWD]                += (s->value[N_STEM_TO_CWD] +
			s->value[N_CROOT_TO_CWD]  +
			s->value[N_BRANCH_TO_CWD] +
			s->value[N_BRANCH_TO_CWD] +
			s->value[N_FRUIT_TO_CWD]) ;

	logger(g_debug_log, "Nitrogen biomass to remove\n");
	logger(g_debug_log, "N_LEAF_TO_LITR   = %f tN/cell\n", s->value[N_LEAF_TO_LITR]);
	logger(g_debug_log, "N_FROOT_TO_LITR  = %f tN/cell\n", s->value[N_FROOT_TO_LITR]);
	logger(g_debug_log, "N_TO_LITR        = %f tN/cell\n", s->value[N_TO_LITR]);
	logger(g_debug_log, "N_STEM_TO_CWD    = %f tN/cell\n", s->value[N_STEM_TO_CWD]);
	logger(g_debug_log, "N_CROOT_TO_CWD   = %f tN/cell\n", s->value[N_CROOT_TO_CWD]);
	logger(g_debug_log, "N_BRANCH_TO_CWD  = %f tN/cell\n", s->value[N_BRANCH_TO_CWD]);
	logger(g_debug_log, "N_BRANCH_TO_CWD  = %f tN/cell\n", s->value[N_BRANCH_TO_CWD]);
	logger(g_debug_log, "N_FRUIT_TO_CWD   = %f tN/cell\n", s->value[N_FRUIT_TO_CWD]);
	logger(g_debug_log, "N_TO_CWD         = %f tN/cell\n", s->value[N_TO_CWD]);

}
