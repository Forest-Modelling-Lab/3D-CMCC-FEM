/*management.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include <new_forest_tree_class.h>
#include "g-function.h"
#include "management.h"
#include "constants.h"
#include "settings.h"
#include "common.h"
#include "logger.h"
#include "remove_tree_class.h"
#include "biomass.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

int forest_management (cell_t *const c, const int day, const int month, const int year)
{
	//int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_for_thinning;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	// sort by above or below ?
	// ALESSIOR TO ALESSIOC FIXME
	qsort ( c->heights, c->heights_count, sizeof (height_t),
		
			(THINNING_REGIME_ABOVE == g_settings->thinning_regime) ? sort_by_heights_asc : sort_by_heights_desc
	);

	//fixme to include once THINNING_REGIME is included
	//	for ( layer = c->tree_layers_count - 1 ; layer >= 0; --layer )
	//	{
	/* loop on each heights starting from highest to lower */
	for ( height = c->heights_count -1 ; height >= 0; --height )
	{
		/* assign shortcut */
		h = &c->heights[height];

		/* loop on each dbh starting from highest to lower */
		for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh )
		{
			/* assign shortcut */
			d = &h->dbhs[dbh];

			/* loop on each age class */
			for ( age = d->ages_count - 1 ; age >= 0 ; --age )
			{
				/* assign shortcut */
				a = &h->dbhs[dbh].ages[age];

				/* loop on each species class */
				for ( species = 0; species < a->species_count; ++species )
				{
					/* assign shortcut */
					s = &a->species[species];

					if ( g_settings->management && ((T == s->management) || (C == s->management)) ) {
						/* this function handles all other management functions */

						/* check at the beginning of simulation */
						if( !year )
						{
							CHECK_CONDITION ( c->years[year].year, >, g_settings->year_start_management );
							CHECK_CONDITION ( (g_settings->year_start_management - g_settings->year_start), >, s->value[THINNING] );
						}

						/***** THINNING *****/
						//note : +1 since it works at the 1st of January of the subsequent year
						if ( ( c->years[year].year == g_settings->year_start_management + 1 ) || ( s->value[THINNING] == years_for_thinning + 1 ) )
						{
							logger(g_debug_log,"**FOREST MANAGEMENT**\n");
							logger(g_debug_log,"**THINNING**\n");

							thinning ( c, height, dbh, age, species, year );

							/* reset counter */
							years_for_thinning = 0;
							return 0;
						}

						/* increment counter */
						++years_for_thinning;

						/* check */
						CHECK_CONDITION( years_for_thinning, >, s->value[ROTATION] );

						/***** HARVESTING *****/
						/* if class age matches with harvesting */
						//note : +1 since it works at the 1st of January of the subsequent year

						if ( ( a->value + 1 ) == s->value[ROTATION] )
						{
							logger(g_debug_log,"**FOREST MANAGEMENT**\n");
							logger(g_debug_log,"**HARVESTING**\n");

							/* remove tree class */
							harvesting ( c, height, dbh, age, species );

							/* note: RESET c->dos */
							c->dos = 0.;

							/* reset years_for_thinning */
							years_for_thinning = 0;

							/* check that all mandatory variables are filled */
							CHECK_CONDITION (g_settings->replanted_n_tree, <, ZERO);
							CHECK_CONDITION (g_settings->replanted_height, <, 1.3);
							CHECK_CONDITION (g_settings->replanted_avdbh,  <, ZERO);
							CHECK_CONDITION (g_settings->replanted_age,    <, ZERO);

							/* re-planting tree class */
							if( g_settings->replanted_n_tree )
							{
								if ( ! add_tree_class_for_replanting( c , day, month, year ) )
								{
									logger_error(g_debug_log, "unable to add new replanted class! (exit)\n");
									exit(1);
								}
							}
							return 1;
						}
						else
						{
							return 0;
						}
					}
				}
			}
		}
	}
	return 0;
	//	}
}

/*****************************************************************************************************************************************/

void thinning (cell_t *const c, const int height, const int dbh, const int age, const int species, const int year)
{
	int trees_to_remove = 0;
	double stand_basal_area_to_remain;
	double stand_basal_area_to_remove;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];


	/* thinning function based on basal area */

	//TODO
	if (THINNING_REGIME_ABOVE == s->value[THINNING_REGIME] )
	{

	}
	else
	{

	}

	logger(g_debug_log, "** Management options: Thinning ** \n");

	logger(g_debug_log, "basal area before thinning = %f m2/class cell\n", s->value[STAND_BASAL_AREA_m2]);
	logger(g_debug_log, "trees before thinning = %d trees/cell\n", s->counter[N_TREE]);

	/* compute basal area to remain */
	stand_basal_area_to_remain = (1.0 - (s->value[THINNING_INTENSITY] / 100. ) ) * s->value[STAND_BASAL_AREA_m2];
	logger(g_debug_log, "basal area to remain = %f m2/class\n", stand_basal_area_to_remain);

	/* compute basal area to remove */
	stand_basal_area_to_remove = (s->value[THINNING_INTENSITY] / 100. ) * s->value[STAND_BASAL_AREA_m2];
	logger(g_debug_log, "basal area to remove = %f\n", stand_basal_area_to_remove);

	/* compute integer number of trees to remove */
	trees_to_remove = ROUND((s->value[THINNING_INTENSITY] / 100. ) * s->counter[N_TREE]);
	logger(g_debug_log, "trees_to_remove = %d\n", trees_to_remove);

	if ( trees_to_remove < s->counter[N_TREE] )
	{
		/* update C and N biomass */
		tree_biomass_remove ( c, height, dbh, age, species, trees_to_remove );

		/* remove trees */
		s->counter[N_TREE] -= trees_to_remove;
		logger(g_debug_log, "Number of trees after management = %d trees/cell\n", s->counter[N_TREE]);
	}
	else
	{
		/* update C and N biomass */
		tree_biomass_remove ( c, height, dbh, age, species, s->counter[N_TREE] );

		/* remove completely all trees */
		tree_class_remove (c, height, dbh, age, species );
	}

	/* check */
	CHECK_CONDITION(s->counter[N_TREE], <, ZERO );

	/*********************************************************************************************************************************************************************/

	/* adding coarse and fine root and leaf to litter pool */
	//fixme
	//c->daily_litr_carbon   += include biomass  ;

	/* Total class C at the end */
	s->value[TOTAL_C]   = s->value[LEAF_C] + s->value[CROOT_C] + s->value[FROOT_C] + s->value[STEM_C] + s->value[BRANCH_C] + s->value[RESERVE_C];
	logger(g_debug_log, "Total Biomass = %f tC/ha\n", s->value[TOTAL_C]);

	/* update stand trees */
	c->n_trees -= trees_to_remove;
	c->annual_dead_tree += trees_to_remove;

}

/*****************************************************************************************************************************************/

void harvesting (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* at the moment it considers a complete harvesting for all classes (if considered) */
	logger(g_debug_log, "\n\n\n\n\n** Management options: Harvesting ** \n\n\n\n\n");

	/* update C and N biomass */
	tree_biomass_remove ( c, height, dbh, age, species, s->counter[N_TREE] );

	/* remove completely all trees */
	tree_class_remove (c, height, dbh, age, species );
}

/*****************************************************************************************************************************************/



