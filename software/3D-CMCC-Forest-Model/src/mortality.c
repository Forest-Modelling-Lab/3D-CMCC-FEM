/*mortality.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "common.h"
#include "g-function.h"
#include "settings.h"
#include "logger.h"
#include "mortality.h"
#include "tree_model.h"
#include "remove_tree_class.h"
#include "biomass.h"
#include "canopy_cover.h"
#include "allometry.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void self_thinning_mortality (cell_t *const c, const int layer, const int year)
{
	int height;
	int dbh;
	int age;
	int species;
	int deadtree;
	int livetree;


	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	/* "First, large plants suppress small plants.
	 * The result is a “hierarchy of dominance and suppression” in which the smaller plants
	 * are at an accumulating disadvantage and finally die".
	 * Westoby, 1984, Advances in Ecological Research */

	/* NOTE: it mainly follows rationale of TREEDYN 3 "Crowding Competition mortality function",
	 * Bossel, 1996, Ecological Modelling (eq. 29) */

	/* This is a sort of self-thinning rules not based on biomass but based on canopy cover */

	/* the model makes die trees of the lower height class for that layer because
	it passes through the function sort_by_height_desc the height classes starting from the lowest */

	logger(g_debug_log, "\n\n*****SELF THINNING MORTALITY for layer %d*****\n", layer);

	for ( height = c->heights_count - 1; height >= 0; --height )
	{
		/* first of all sort by descending height */
		/* model makes die before lower height in each later */
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

		h = &c->heights[height];

		for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh)
		{
			d = &c->heights[height].dbhs[dbh];

			for ( age = 0; age < d->ages_count ; ++age )
			{
				a = &c->heights[height].dbhs[dbh].ages[age];

				for ( species = 0; species < a->species_count; ++species )
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					/* sort by species-specific light tolerance (from the most light demanding to the lowest */
					//test
					//int i;
					//for ( i = (int)s->value[LIGHT_TOL]; i > 0; --i)
					//{
					//fixme self thinning mortality should follows this rationale
					//fixme it shouldn't work properly in this way
					//}

					logger(g_debug_log, "MORTALITY BASED ON HIGH CANOPY COVER height %g species %s dbh %g !!!\n", h->value, s->name, d->value);

					/* initialize variables */
					livetree = s->counter[N_TREE];
					deadtree = 0;

					/* mortality */
					//FIXME PORCATA
					while ( c->tree_layers[layer].layer_cover >= g_settings->max_layer_cover )
					{

						/* remove one tree per run */
						++deadtree;
						--livetree;

						/* impose DBHDC_EFF = DBHDCMIN */
						s->value[DBHDC_EFF] = s->value[DBHDCMIN];

						/* update at cell level */
						++c->daily_dead_tree;

						/* update layer trees */
						--c->tree_layers[layer].layer_n_trees;

						c->tree_layers[layer].layer_cover  -= s->value[CANOPY_COVER_PROJ];

						if ( livetree > 0 )
						{
							/* recompute crown imposed DBHDC_EFF */
							crown_allometry        ( c, height, dbh, age, species );

							/* recompute canopy cover with imposed DBHDC_EFF */
							s->value[CANOPY_COVER_PROJ] = s->value[CROWN_AREA_PROJ] * livetree / g_settings->sizeCell;

							/* check for recompued canopy cover */
							c->tree_layers[layer].layer_cover += s->value[CANOPY_COVER_PROJ];
						}
//							else
//							{
//								//fixme
//								/* remove_tree_class */
//								logger(g_debug_log, "completely removed lower class, starting to remove from higher..\n");
//								if ( ! tree_class_remove(c, height, dbh, age, species) )
//								{
//									logger_error(g_debug_log, "unable to remove tree class");
//									exit(1);
//								}
//
//								/* mortality for the higher height class */
//								while (c->tree_layers[layer].layer_cover > g_settings->max_layer_cover &&
//										c->heights[height + 1].dbhs[dbh].ages[age].species[species].counter[N_TREE] >= 0)
//								{
//									--c->heights[height + 1].dbhs[dbh].ages[age].species[species].counter[N_TREE];
//									++c->heights[height + 1].dbhs[dbh].ages[age].species[species].counter[DEAD_TREE];
//
//									//todo check if correct (probably not)
//									dbhdc_function         ( c, layer, height + 1, dbh, age, species, year );
//									crown_allometry        ( c, height + 1, dbh, age, species );
//									canopy_cover_projected ( c, height + 1, dbh, age, species );
//
//									/* remove_tree_class */
//									if ( ! tree_class_remove(c, height, dbh, age, species) )
//									{
//										logger_error(g_debug_log, "unable to remove tree class");
//										exit(1);
//									}
//								}
//							}
					}

					//fixme this is not correct for multilayer
					/* remove dead C and N biomass */
					tree_biomass_remove ( c, height, dbh, age, species, deadtree );

					/* update live and dead tree (do not move above) */
					s->counter[DEAD_TREE] = deadtree;
					s->counter[N_TREE]    = livetree;
					c->n_trees -= deadtree;
				}
			}
		}
	}

	/* reset values for layer (they are recomputed below) */
	c->tree_layers[layer].layer_n_height_class = 0;
	c->tree_layers[layer].layer_n_trees        = 0;
	c->tree_layers[layer].layer_density        = 0;

	/* REcompute numbers of height classes, tree number and density after mortality within each layer */
	logger(g_debug_log, "REcompute numbers of height classes, tree number and density after mortality within each layer\n\n");


	for ( height = c->heights_count -1; height >= 0 ; --height )
	{
		if( layer == c->heights[height].height_z )
		{
			/* recompute number of height classes */
			c->tree_layers[layer].layer_n_height_class += 1;
		}
		for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh)
		{
			for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
			{
				for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					if( layer == c->heights[height].height_z )
					{
						/* recompute number of trees for each layer */
						c->tree_layers[layer].layer_n_trees += s->counter[N_TREE];
					}
				}
			}
		}
	}

	/*recompute density for each layer */
	c->tree_layers[layer].layer_density = c->tree_layers[layer].layer_n_trees / g_settings->sizeCell;

	logger(g_debug_log, "-layer = %d\n", layer);
	logger(g_debug_log, "-height class(es) = %d layer \n", c->tree_layers[layer].layer_n_height_class);
	logger(g_debug_log, "-number of trees = %d layer\n", c->tree_layers[layer].layer_n_trees);
	logger(g_debug_log, "-density = %g layer\n", c->tree_layers[layer].layer_density);
	logger(g_debug_log, "-Dead tree(s) = %d\n", deadtree);

	/* reset dead tree */
	deadtree = 0;

}

int growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	/* this function superimpose mortality for all trees in class when reserves
	 * go under zero assuming that reserve pool hasn't be refilled during the day
	 * and make the tree class die */

	species_t* s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	if( s->value[RESERVE_C] < 0 )
	{
		/* reset to zero n_trees */
		//ALESSIOC TO ALESSIOR IMPOSE N_TREE = 0 IS USEFUL?
		s->counter[DEAD_TREE] = s->counter[N_TREE];
		s->counter[N_TREE]    = 0;
		c->n_trees -= s->counter[DEAD_TREE];

		/* call remove_tree_class */
		if ( ! tree_class_remove(c, height, dbh, age, species) )
		{
			logger_error(g_debug_log, "unable to remove tree class");
			exit(1);
		}
		return 1;
	}
	return 0;
}

int annual_growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	/* this function superimpose mortality for all trees in class when reserves
	 * go under zero assuming that reserve pool hasn't be refilled during the year
	 * and make the tree class die */

	species_t* s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	if( s->value[RESERVE_C] < 0 )
	{
		/* reset to zero n_trees */
		//ALESSIOC TO ALESSIOR IMPOSE N_TREE = 0 IS USEFUL?
		s->counter[DEAD_TREE] = s->counter[N_TREE];
		s->counter[N_TREE]    = 0;
		c->n_trees -= s->counter[DEAD_TREE];

		/* call remove_tree_class */
		if ( ! tree_class_remove(c, height, dbh, age, species) )
		{
			logger_error(g_debug_log, "unable to remove tree class");
			exit(1);
		}
		return 1;
	}
	return 0;
}

/* Age mortality function from LPJ-GUESS */
void age_mortality (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	int livetree = 0;
	int deadtree = 0;
	double age_mort;

	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	// ALESSIOR
	//if ( ! a ) return;
	s = &a->species[species];

	/* Age probability function */
	age_mort = ( - ( 3. * log ( 0.001 ) ) / (s->value[MAXAGE])) * pow (((double)a->value /s->value[MAXAGE]), 2.);

	livetree = s->counter[N_TREE];


	if ( ( livetree * age_mort ) > 1 )
	{
		logger(g_debug_log, "**MORTALITY based on Tree Age (LPJ)**\n");

		deadtree = (int)(livetree * age_mort);
		logger(g_debug_log, "dead trees = %d\n", deadtree);

		if ( livetree > deadtree)
		{
			/* update C and N biomass */
			tree_biomass_remove ( c, height, dbh, age, species, deadtree );
		}
		else
		{
			if ( ! tree_class_remove(c, height, dbh, age, species) )
			{
				logger_error(g_debug_log, "unable to remove tree class");
				exit(1);
			}
		}
	}
	else
	{
		deadtree = 0;
	}

	/* update at class level */
	s->counter[DEAD_TREE] += deadtree;
	s->counter[N_TREE]    -= deadtree;

	/* check */
	CHECK_CONDITION(s->counter[N_TREE],    <=, 0);
	CHECK_CONDITION(s->counter[DEAD_TREE], <, 0);

	/* update at cell level */
	c->daily_dead_tree   += deadtree;
	c->monthly_dead_tree += deadtree;
	c->annual_dead_tree  += deadtree;
	c->n_trees -= deadtree;

	/* check */
	CHECK_CONDITION(c->daily_dead_tree  , <, 0);
	CHECK_CONDITION(c->monthly_dead_tree, <, 0);
	CHECK_CONDITION(c->annual_dead_tree , <, 0);


	logger(g_debug_log, "**********************************\n");
}

void self_pruning ( cell_t *const c, const int height, const int dbh, const int age, const int species, const double old_layer_cover )
{
	double self_pruning_ratio;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n\n*****SELF PRUNING*****\n");

	/* reduce proportionally branch biomass to the crown area reduction */

	/* compute FRACTION in crown area reduction for self-pruning */
	self_pruning_ratio = g_settings->max_layer_cover / old_layer_cover;

	/* check for precision control */
	if (self_pruning_ratio > 1) self_pruning_ratio = 1.;

	/* remove biomass from branch */
	s->value[BRANCH_C_TO_REMOVE]  += ( s->value[BRANCH_C] * ( 1. - self_pruning_ratio ) );

	/* adding to BRANCH C transfer pools */
	s->value[C_BRANCH_TO_RESERVE] += s->value[BRANCH_C_TO_REMOVE] * C_FRAC_TO_RETRANSL;
	s->value[C_BRANCH_TO_CWD]     += s->value[BRANCH_C_TO_REMOVE] * ( 1. - C_FRAC_TO_RETRANSL );

	s->value[C_TO_RESERVE]        += s->value[C_BRANCH_TO_RESERVE];
	s->value[C_TO_CWD]            += s->value[C_BRANCH_TO_CWD];

	/* remove biomass from branch */
	s->value[BRANCH_N_TO_REMOVE]  += ( s->value[BRANCH_C] * ( 1. - self_pruning_ratio ) );

	/* adding to BRANCH C transfer pools */
	s->value[N_BRANCH_TO_RESERVE] += s->value[BRANCH_N_TO_REMOVE] * N_FRAC_TO_RETRANSL;
	s->value[N_BRANCH_TO_CWD]     += s->value[BRANCH_N_TO_REMOVE] * ( 1. - N_FRAC_TO_RETRANSL );

	s->value[N_TO_RESERVE]        += s->value[N_BRANCH_TO_RESERVE];
	s->value[N_TO_CWD]            += s->value[N_BRANCH_TO_CWD];

}


/* Self-thinnig mortality function from 3PG */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                     Self-thinnig mortality (3-PG)                           */
/*This function determines the number of stems to remove to ensure the         */
/*self-thinning rule is satisfied. It applies the Newton-Rhapson method        */
/*to solve for N to an accuracy of 1 stem or less. To change this,             */
/*change the value of "accuracy".                                              */
/*This was the old mortality function:                                         */
/*getMortality = oldN - 1000 * (wSx1000 * oldN / oldW / 1000) ^ (1 / thinPower)*/
/*which has been superceded by the following ..                                */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/* NOT USED */
//void self_thinning_mortality_3PG (species_t *const s, int years)
//{
//
//	int i;
//	double fN, dfN;
//	double dN, n, x1, x2;
//
//
//	s->value[WS_MAX] = s->value[WSX1000] * pow ( ( 1000 / (double) s->counter[N_TREE] ), s->value[THINPOWER] );
//
//	//ALESSIOC ??????????????
//	//modifified version for 1Km^2 spatial resolution
//	s->value[WS_MAX] = s->value[WSX1000];
//
//	if ( s->value[AV_STEM_MASS_KgDM] > s->value[WS_MAX])
//	{
//		logger(g_log, "*Self-thinnig mortality (3-PG)*\n");
//		logger(g_log, "Average Stem Mass > WSMax\n");
//		logger(g_log, "WS MAX = %g kgC/tree\n", s->value[WS_MAX]);
//		logger(g_log, "Average Stem Mass = %g kgC stem/tree\n", s->value[AV_STEM_MASS_KgDM]);
//		logger(g_log, "Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);
//
//		n = (double)s->counter[N_TREE] / 1000;
//		logger(g_log, "n = %g\n", n);
//
//		x1 = 1000 * s->value[MS] * s->value[STEM_C] / (double)s->counter[N_TREE];
//		logger(g_log, "x1 = %g\n", x1);
//		i = 0;
//		while ( 1 )
//		{
//			i = i + 1;
//			logger(g_log, "i = %d\n", i);
//			x2 = s->value[WSX1000] * pow (n, (1 - s->value[THINPOWER]));
//			logger(g_log, "X2 = %g\n", x2);
//			fN = x2 - x1 * n - (1 - s->value[MS]) * s->value[STEM_C];
//			logger(g_log, "fN = %g\n", fN);
//			dfN = (1 - s->value[THINPOWER]) * x2 / n - x1;
//			logger(g_log, "dfN = %g\n", dfN);
//			dN = -fN / dfN;
//			logger(g_log, "dN = %g\n", dN);
//			n = n + dN;
//			logger(g_log, "n = %g\n", n);
//			if ((fabs(dN) <= eps) || (i >= 5))
//				break;
//		}
//
//		s->counter[DEAD_STEMS] = (int)(s->counter[N_TREE] - 1000 * n);
//		logger(g_log, "Dead Tree In Mortality Function = %d trees \n", s->counter[DEAD_STEMS]);
//
//		//SERGIO CONTROL: if DEAD_STEMS < 0 set it to its minimum plausible value; that is 0
//		if (s->counter[DEAD_STEMS] < 0)
//		{
//			s->counter[DEAD_STEMS]	 = 0;
//		}
//		//control
//		if (s->counter[DEAD_STEMS] > s->counter[N_TREE])
//		{
//			logger(g_log, "ERROR Number of Dead Trees > N Trees\n");
//			logger(g_log, "Dead Trees = %d\n", s->counter[DEAD_STEMS]);
//			logger(g_log, "Live Trees = %d\n", s->counter[N_TREE]);
//		}
//		else
//		{
//			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[DEAD_STEMS];
//			logger(g_log, "Number of Trees  after mortality = %d trees\n", s->counter[N_TREE]);
//			s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
//			s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
//			s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
//			logger(g_log, "Wf after dead = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_tDM]);
//			logger(g_log, "Wr after dead = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
//			logger(g_log, "Ws after dead = %g tDM/ha\n", s->value[BIOMASS_STEM_tDM]);
//		}
//
//
//
//		//----------------Number of trees after mortality---------------------
//
//
//
//
//		//--------------------------------------------------------------------
//
//		//deselected algorithm for 1Km^2 spatial resolution
//		//s->value[WS_MAX] = s->value[WSX1000] * pow((1000 / (double)s->counter[N_TREE]), s->value[THINPOWER]);
//
//		//modifified version for 1Km^2 spatial resolution
//		s->value[WS_MAX] = s->value[WSX1000];
//
//		logger(g_log, "wSmax = %g KgDM/tree\n", s->value[WS_MAX]);
//		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM] * 1000.0 / (double)s->counter[N_TREE];
//		logger(g_log, "AvStemMass after dead = %g Kg/tree\n", s->value[AV_STEM_MASS_KgDM]);
//	}
//	else
//	{
//		logger(g_log, "NO MORTALITY based SELF-THINNING RULE\n");
//		logger(g_log, "Average Stem Mass < WSMax\n");
//	}
//
//	logger(g_log, "**********************************\n");
//
//
//
//}


