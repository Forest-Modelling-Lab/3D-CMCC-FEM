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

extern settings_t* g_settings;
extern logger_t* g_log;

void layer_self_pruning_thinning ( cell_t *const c )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	char mortality;

	species_t *s;


	logger(g_log, "*annual check for self pruning and self thinning (crowding competition)*\n");

	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
	{
		/* check if layer exceeds maximum layer coverage */
		if (c->t_layers[layer].layer_cover >= g_settings->max_layer_cover)
		{
			logger(g_log, "-layer cover exceeds max layer cover for layer %d\n", layer);

			//test
			/* first of all it checks if self-pruning is enough */
			/* SELF-PRUNING */
			if ( 0 == self_pruning ( c, layer ))
			{
				/* also self-thinning is necessary */
				/* SELF-THINNING */
				self_thinning ( c, layer );
			}

			//test if test above is ok remove
			/* SELF-THINNING */
			//self_thinning (c, layer);

			/* assign char */
			mortality = 'y';

			/* reset values for layer (they are recomputed below)*/
			c->t_layers[layer].layer_n_height_class = 0;
			c->t_layers[layer].layer_n_trees = 0;
			c->t_layers[layer].layer_density = 0;
			/* reset values for cell (they are recomputed below)*/
			c->cell_n_trees = 0;
			c->cell_cover = 0;

		}
		else
		{
			logger(g_log, "-no crowding competition happens for layer %d\n", layer);

			mortality = 'n';
		}
		/* check */
		CHECK_CONDITION(c->t_layers[layer].layer_cover , > g_settings->max_layer_cover);

		logger(g_log, "**************************************\n\n");

		/**************************************************************************************************/
		/* check if REcompute numbers of height classes, tree number and density after mortality within each layer */
		if ( mortality == 'y' )
		{
			/* REcompute numbers of height classes, tree number and density after mortality within each layer */
			logger(g_log, "REcompute numbers of height classes, tree number and density after mortality within each layer\n\n");

			for ( height = c->heights_count -1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* recompute number of height classes */
					c->t_layers[layer].layer_n_height_class += 1;
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
								c->t_layers[layer].layer_n_trees += s->counter[N_TREE];
							}
						}
					}
				}
			}

			/*recompute density for each layer */
			c->t_layers[layer].layer_density = c->t_layers[layer].layer_n_trees / g_settings->sizeCell;

			logger(g_log, "-layer = %d\n", layer);
			logger(g_log, "-height class(es) = %d layer \n", c->t_layers[layer].layer_n_height_class);
			logger(g_log, "-number of trees = %d layer\n", c->t_layers[layer].layer_n_trees);
			logger(g_log, "-density = %g layer\n", c->t_layers[layer].layer_density);
		}
		logger(g_log, "**************************************\n\n");

		/**************************************************************************************************/
		/* Recompute number of total trees and cell cover (cell level) */
		logger(g_log, "Recompute number of total trees and cell cover (cell level)\n");


		/* compute total number of trees per cell */
		c->cell_n_trees += c->t_layers[layer].layer_n_trees;

		/* assuming that plants tend to occupy the part of the cell not covered by the others */
		c->cell_cover += c->t_layers[layer].layer_cover;
	}

	logger(g_log, "-number of trees = %d cell\n", c->cell_n_trees);
	logger(g_log, "-cell cover = %g cell\n", c->cell_cover);
	logger(g_log, "**************************************\n\n");
}

int self_pruning ( cell_t *const c, const int layer )
{
	int height;
	int dbh;
	int age;
	int species;

	double red_perc;
	double old_crown_area;

	tree_layer_t *l;
	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	l = &c->t_layers[layer];

	/* SELF-PRUNING function */
	/*it reduces dbhdc values for lower height class up to dbhdcmin otherwise
	 * mortality for self-thinning*/

	logger(g_log, "\n**SELF-PRUNING FUNCTION for layer %d**\n", layer);

	for ( height = c->heights_count - 1; height >= 0; --height )
	{
		h = &c->heights[height];

		/* first of all sort by descending height */
		/* model makes prune before lower height in each later */
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

		//ALESSIOC ALESSIOR CHECK THAT THE FOR STARTS WITH THE LOWEST DBH VALUE
		for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh)
		{
			d = &c->heights[height].dbhs[dbh];

			for ( age = 0; age < d->ages_count ; ++age )
			{
				a = &c->heights[height].dbhs[dbh].ages[age];

				for ( species = 0; species < a->species_count; ++species )
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					logger(g_log, "layer %d; layer cover %g, height %g\n", layer, l->layer_cover * 100, h->value);
					logger(g_log, "DBHDC effective before pruning = %g\n", s->value[DBHDC_EFF]);

					/* compute possible crown area for above canopy cover dbhdc */
					old_crown_area = s->value[CROWN_AREA_DBHDC];
					logger(g_log,"old crown area before pruning = %g m2/tree\n", old_crown_area);

					/* until layer cover not reaches lower values than max_layer_cover or DBDCMIN */
					while( l->layer_cover > g_settings->max_layer_cover )
					{
						/* reduce DBHDC_EFF */
						/* note: 0.00005 is an arbitrary value */
						s->value[DBHDC_EFF] -= 0.00005;
						//logger(g_log,"DBHDC effective during while = %g\n", s->value[DBHDC_EFF]);

						/* recompute crown diameter */
						s->value[CROWN_DIAMETER_DBHDC] = d->value * s->value[DBHDC_EFF];
						//logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC]);

						/* Crown Area using DBH-DC */
						s->value[CROWN_AREA_DBHDC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC], 2 );
						//logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC]);

						/* Canopy Cover using DBH-DC */
						s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC] * s->counter[N_TREE] / g_settings->sizeCell;
						//logger(g_log, "-Canopy cover DBH-DC class level = %g %%\n", s->value[CANOPY_COVER_DBHDC] * 100.0);

						/* recompute layer cover */
						if( c->t_layers[layer].layer_n_height_class == 1)
						{
							l->layer_cover = s->value[CANOPY_COVER_DBHDC];
						}
						//fixme is not correct for multilayer
						else
						{
							l->layer_cover += s->value[CANOPY_COVER_DBHDC];
						}
						//logger(g_log, "-layer cover cell level = %g \n", l->layer_cover);
					}
					/************************************************************************/

					/* reduce proportionally to the crown area reduction the amount of branch and leaf C pool */
					/* compute percentage in crown area reduction for self-pruning */
					red_perc = (s->value[CROWN_AREA_DBHDC] / old_crown_area);
					logger(g_log, "percentage of reduction in crown area = %g %%\n", (1 - red_perc) * 100 );

					/***************************************************************/
					/* update branch C pool */
					//fixme model highly overestimate reduction
					//because it removes indistinctly coarse branch and fine branch
					//fixme something should be retranslocated to reserve??
					//s->value[BRANCH_C] *= red_perc;

					/* update branch N pool */
					//s->value[BRANCH_N] *= red_perc;

					/***************************************************************/
					/* update coarse root C pool */
					//fixme something should be retranslocated to reserve??
					//s->value[COARSE_ROOT_C] *= red_perc;

					/* update coarse root N pool */
					//s->value[COARSE_ROOT_N] *= red_perc;
					;

					/***************************************************************/
					/* update leaf C pool */
					//fixme something should be retranslocated to reserve??
					//s->value[LEAF_C] *= perc;

					/* update leaf N pool */
					//s->value[LEAF_N] *= perc;

					/***************************************************************/
					/* self-pruned C biomass to litter */
					//s->value[LITTER_C] += (s->value[BRANCH_C] * (1 - red_perc)) +
					//		(s->value[COARSE_ROOT_C] * (1 - red_perc)) +
					//		(s->value[LEAF_C] * (1 - red_perc));

					/***************************************************************/

					/* summary after pruning */
					logger(g_log, "-after pruning-\n");
					logger(g_log, "-branch carbon = %g tC/cell\n", s->value[BRANCH_C]);
					logger(g_log, "-DBHDC effective = %g\n", s->value[DBHDC_EFF]);
					logger(g_log, "-DBHDC MINIMUM = %g\n", s->value[DBHDCMIN]);
					logger(g_log, "-DEN MINIMUM = %g\n", s->value[DENMIN]);
					logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC]);
					logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC]);
					logger(g_log, "-Canopy cover DBH-DC class level = %g %%\n", s->value[CANOPY_COVER_DBHDC] * 100.0);
					logger(g_log, "-layer cover cell level = %g\n", l->layer_cover);

					/* check if reduction in dbhdc satisfy max layer cover */
					if ( l->layer_cover < g_settings->max_layer_cover && s->value[DBHDC_EFF] > s->value[DBHDCMIN] )
					{
						//todo to be tested
						/* self-pruning was enough */
						logger(g_log, "self-pruning was enough\n");
						return 1;
					}
					else
					{
						//todo to be tested
						/* self-pruning was not enough */
						logger(g_log, "self-pruning was not enough\n");
						return 0;

					}
				}
			}
		}
	}
	//ALESSIOC PORCATA
	return 2;
}

void self_thinning ( cell_t *const c, const int layer )
{
	int height;
	int dbh;
	int age;
	int species;

	int deadtree = 0;
	//int oldNstump;
	//int deadstump = 0;

	height_t *h;
	dbh_t *d;
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

	logger(g_log, "\n\n*****SELF THINNING MORTALITY for layer %d*****\n", layer);

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		/* first of all sort by descending height */
		/* model makes die before lower height in each later */
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

		h = &c->heights[height];

		//ALESSIOC ALESSIOR CHECK THAT THE FOR STARTS WITH THE LOWEST DBH VALUE
		for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh)
		{
			d = &c->heights[height].dbhs[dbh];

			for ( age = 0; age < d->ages_count ; ++age )
			{
				for ( species = 0; species < d->ages[age].species_count; ++species )
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					logger(g_log, "MORTALITY BASED ON HIGH CANOPY COVER height %g species %s dbh %g !!!\n", h->value, s->name, d->value);

					if (s->management == T)
					{
						/* compute average biomass */
						s->value[AV_STEM_MASS_KgC] = (s->value[STEM_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LEAF_MASS_KgC] = (s->value[LEAF_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_ROOT_MASS_KgC] = (s->value[TOT_ROOT_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_FINE_ROOT_MASS_KgC] = (s->value[FINE_ROOT_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_COARSE_ROOT_MASS_KgC] =(s->value[COARSE_ROOT_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_RESERVE_MASS_KgC] = (s->value[RESERVE_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_BRANCH_MASS_KgC] = (s->value[BRANCH_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LIVE_STEM_MASS_KgC] = (s->value[STEM_LIVE_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_DEAD_STEM_MASS_KgC] = (s->value[STEM_DEAD_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_LIVE_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_DEAD_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LIVE_BRANCH_MASS_KgC] = (s->value[BRANCH_LIVE_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_DEAD_BRANCH_MASS_KgC] = (s->value[BRANCH_DEAD_WOOD_C] / (double)s->counter[N_TREE])*1000.0;

						/* mortality */
						while (c->t_layers[layer].layer_cover > g_settings->max_layer_cover && s->counter[N_TREE] > 0)
						{
							--s->counter[N_TREE];
							++deadtree;

							/* recompute class level canopy cover */
							s->value[CANOPY_COVER_DBHDC] -= (s->value[CROWN_AREA_DBHDC] / g_settings->sizeCell);

							/* recompute layer level canopy cover */
							c->t_layers[layer].layer_cover -= (s->value[CROWN_AREA_DBHDC] / g_settings->sizeCell);

							//fixme to remove once implemented function below
							CHECK_CONDITION(s->counter[N_TREE], > 0);

							if (s->counter[N_TREE] == 0)
							{
								/* mortality for the higher height class */
								while (c->t_layers[layer].layer_cover > g_settings->max_layer_cover && c->heights[height + 1].dbhs[dbh].ages[age].species[species].counter[N_TREE] > 0)
								{
									--c->heights[height + 1].dbhs[dbh].ages[age].species[species].counter[N_TREE];
									++deadtree;

									//fixme not correct
									c->heights[height + 1].dbhs[dbh].ages[age].species[species].value[CANOPY_COVER_DBHDC] -=
											(c->heights[height + 1].dbhs[dbh].ages[age].species[species].value[CROWN_AREA_DBHDC] / g_settings->sizeCell);

									/* recompute layer level canopy cover */
									c->t_layers[layer].layer_cover -=
											(c->heights[height + 1].dbhs[dbh].ages[age].species[species].value[CROWN_AREA_DBHDC] / g_settings->sizeCell);

								}
							}

							/* check */
							CHECK_CONDITION(s->counter[N_TREE], <= 0);

							/* update at cell level */
							c->daily_dead_tree += deadtree;
							c->monthly_dead_tree += deadtree;
							c->annual_dead_tree += deadtree;
						}

						/* update class biomass */
						s->value[STEM_C] -= (s->value[AV_STEM_MASS_KgC]/1000.0*deadtree);
						s->value[LEAF_C] -= (s->value[AV_LEAF_MASS_KgC]/1000.0*deadtree);
						s->value[FINE_ROOT_C] -= (s->value[AV_FINE_ROOT_MASS_KgC]/1000.0*deadtree);
						s->value[COARSE_ROOT_C] -= (s->value[AV_COARSE_ROOT_MASS_KgC]/1000.0*deadtree);
						s->value[RESERVE_C] -= (s->value[AV_RESERVE_MASS_KgC]/1000.0*deadtree);
						s->value[BRANCH_C] -= (s->value[AV_BRANCH_MASS_KgC]/1000.0*deadtree);
						s->value[STEM_LIVE_WOOD_C] -= (s->value[AV_LIVE_STEM_MASS_KgC]/1000.0*deadtree);
						s->value[STEM_DEAD_WOOD_C] -= (s->value[AV_DEAD_STEM_MASS_KgC]/1000.0*deadtree);
						s->value[COARSE_ROOT_LIVE_WOOD_C] -= (s->value[AV_LIVE_COARSE_ROOT_MASS_KgC]/1000.0*deadtree);
						s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[AV_DEAD_COARSE_ROOT_MASS_KgC]/1000.0*deadtree);
						s->value[BRANCH_LIVE_WOOD_C] -= (s->value[AV_LIVE_BRANCH_MASS_KgC]/1000.0*deadtree);
						s->value[BRANCH_DEAD_WOOD_C] -= (s->value[AV_DEAD_BRANCH_MASS_KgC]/1000.0*deadtree);

						s->value[LITTER_C] += (s->value[AV_LEAF_MASS_KgC]/1000.0*deadtree) +
								(s->value[AV_FINE_ROOT_MASS_KgC]/1000.0*deadtree) +
								(s->value[AV_COARSE_ROOT_MASS_KgC]/1000.0*deadtree) +
								(s->value[AV_STEM_MASS_KgC]/1000.0*deadtree) +
								(s->value[AV_RESERVE_MASS_KgC]/1000.0*deadtree) +
								(s->value[AV_BRANCH_MASS_KgC]/1000.0*deadtree);

						/* compute average biomass */
						s->value[AV_LEAF_MASS_KgC] = (s->value[LEAF_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_STEM_MASS_KgC] = (s->value[STEM_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_ROOT_MASS_KgC] = (s->value[TOT_ROOT_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_FINE_ROOT_MASS_KgC] = (s->value[FINE_ROOT_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_COARSE_ROOT_MASS_KgC] =(s->value[COARSE_ROOT_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_RESERVE_MASS_KgC] = (s->value[RESERVE_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_BRANCH_MASS_KgC] = (s->value[BRANCH_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LIVE_STEM_MASS_KgC] = (s->value[STEM_LIVE_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_DEAD_STEM_MASS_KgC] = (s->value[STEM_DEAD_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_LIVE_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_DEAD_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_LIVE_BRANCH_MASS_KgC] = (s->value[BRANCH_LIVE_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
						s->value[AV_DEAD_BRANCH_MASS_KgC] = (s->value[BRANCH_DEAD_WOOD_C] / (double)s->counter[N_TREE])*1000.0;
					}

					logger(g_log, "LEAF_C removed =%g tC\n",(s->value[AV_LEAF_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "FINE_ROOT_C removed =%g tC\n",(s->value[AV_FINE_ROOT_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "COARSE_ROOT_C removed =%g tC\n",(s->value[AV_COARSE_ROOT_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "STEM_C removed =%g tC\n",(s->value[AV_STEM_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "RESERVE_C removed =%g tC\n",(s->value[AV_RESERVE_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "BRANCH_C removed =%g tC\n",(s->value[AV_BRANCH_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "STEM_LIVE_WOOD_C removed =%g tC\n",(s->value[AV_LIVE_STEM_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "STEM_DEAD_WOOD_C removed =%g tC\n",(s->value[AV_DEAD_STEM_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "COARSE_ROOT_LIVE_WOOD_C removed =%g tC\n",(s->value[AV_LIVE_COARSE_ROOT_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "COARSE_ROOT_DEAD_WOOD_C removed =%g tC\n",(s->value[AV_DEAD_COARSE_ROOT_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "BRANCH_LIVE_WOOD_C removed =%g tC\n",(s->value[AV_LIVE_BRANCH_MASS_KgC]/1000.0*deadtree));
					logger(g_log, "BRANCH_DEAD_WOOD_C removed =%g tC\n",(s->value[AV_DEAD_BRANCH_MASS_KgC]/1000.0*deadtree));
				}
			}
		}
	}

	/* reset values for layer (they are recomputed below)*/
	c->t_layers[layer].layer_n_height_class = 0;
	c->t_layers[layer].layer_n_trees = 0;
	c->t_layers[layer].layer_density = 0;
	/* reset values for cell (they are recomputed below)*/
	c->cell_n_trees = 0;
	c->cell_cover = 0;

	/* REcompute numbers of height classes, tree number and density after mortality within each layer */
	logger(g_log, "REcompute numbers of height classes, tree number and density after mortality within each layer\n\n");


	for ( height = c->heights_count -1; height >= 0 ; --height )
	{
		if( layer == c->heights[height].height_z )
		{
			/* recompute number of height classes */
			c->t_layers[layer].layer_n_height_class += 1;
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
						c->t_layers[layer].layer_n_trees += s->counter[N_TREE];
					}
				}
			}
		}
	}

	/*recompute density for each layer */
	c->t_layers[layer].layer_density = c->t_layers[layer].layer_n_trees / g_settings->sizeCell;

	logger(g_log, "-layer = %d\n", layer);
	logger(g_log, "-height class(es) = %d layer \n", c->t_layers[layer].layer_n_height_class);
	logger(g_log, "-number of trees = %d layer\n", c->t_layers[layer].layer_n_trees);
	logger(g_log, "-density = %g layer\n", c->t_layers[layer].layer_density);
	logger(g_log, "-Dead tree(s) = %d\n", deadtree);

	/* reset dead tree */
	deadtree = 0;

}

void daily_growth_efficiency_mortality ( species_t *const s )
{

	/* this function superimpose mortality  for all tree class when reserves
	 * go under zero assuming that reserve pool hasn't be refilled during the day
	 * and make trees class die */

	/* call remove_tree_class */
	//remove_tree_class ();

	//todo it shouldn't be a "check_condition" but should kill tree class without stop the code execution
	/* check */
	CHECK_CONDITION(s->value[RESERVE_C], < 0.0);

}

void annual_growth_efficiency_mortality ( species_t *const s )
{

	//todo it shouldn't be a "check_condition" but should kill tree class without stop the code execution
	/* check */
	CHECK_CONDITION(s->value[RESERVE_C], < 0.0);

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

void self_thinning_mortality_3PG (species_t *const s, int years)
{

	int i;
	double fN, dfN;
	double dN, n, x1, x2;


	s->value[WS_MAX] = s->value[WSX1000] * pow ( ( 1000 / (double) s->counter[N_TREE] ), s->value[THINPOWER] );

	//ALESSIOC ??????????????
	//modifified version for 1Km^2 spatial resolution
	s->value[WS_MAX] = s->value[WSX1000];

	if ( s->value[AV_STEM_MASS_KgDM] > s->value[WS_MAX])
	{
		logger(g_log, "*Self-thinnig mortality (3-PG)*\n");
		logger(g_log, "Average Stem Mass > WSMax\n");
		logger(g_log, "WS MAX = %g kgC/tree\n", s->value[WS_MAX]);
		logger(g_log, "Average Stem Mass = %g kgC stem/tree\n", s->value[AV_STEM_MASS_KgDM]);
		logger(g_log, "Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);

		n = (double)s->counter[N_TREE] / 1000;
		logger(g_log, "n = %g\n", n);

		x1 = 1000 * s->value[MS] * s->value[STEM_C] / (double)s->counter[N_TREE];
		logger(g_log, "x1 = %g\n", x1);
		i = 0;
		while ( 1 )
		{
			i = i + 1;
			logger(g_log, "i = %d\n", i);
			x2 = s->value[WSX1000] * pow (n, (1 - s->value[THINPOWER]));
			logger(g_log, "X2 = %g\n", x2);
			fN = x2 - x1 * n - (1 - s->value[MS]) * s->value[STEM_C];
			logger(g_log, "fN = %g\n", fN);
			dfN = (1 - s->value[THINPOWER]) * x2 / n - x1;
			logger(g_log, "dfN = %g\n", dfN);
			dN = -fN / dfN;
			logger(g_log, "dN = %g\n", dN);
			n = n + dN;
			logger(g_log, "n = %g\n", n);
			if ((fabs(dN) <= eps) || (i >= 5))
				break;
		}

		s->counter[DEAD_STEMS] = (int)(s->counter[N_TREE] - 1000 * n);
		logger(g_log, "Dead Tree In Mortality Function = %d trees \n", s->counter[DEAD_STEMS]);

		//SERGIO CONTROL: if DEAD_STEMS < 0 set it to its minimum plausible value; that is 0
		if (s->counter[DEAD_STEMS] < 0)
		{
			s->counter[DEAD_STEMS]	 = 0;
		}
		//control
		if (s->counter[DEAD_STEMS] > s->counter[N_TREE])
		{
			logger(g_log, "ERROR Number of Dead Trees > N Trees\n");
			logger(g_log, "Dead Trees = %d\n", s->counter[DEAD_STEMS]);
			logger(g_log, "Live Trees = %d\n", s->counter[N_TREE]);
		}
		else
		{
			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[DEAD_STEMS];
			logger(g_log, "Number of Trees  after mortality = %d trees\n", s->counter[N_TREE]);
			s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
			s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
			s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
			logger(g_log, "Wf after dead = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_tDM]);
			logger(g_log, "Wr after dead = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
			logger(g_log, "Ws after dead = %g tDM/ha\n", s->value[BIOMASS_STEM_tDM]);
		}



		//----------------Number of trees after mortality---------------------




		//--------------------------------------------------------------------

		//deselected algorithm for 1Km^2 spatial resolution
		//s->value[WS_MAX] = s->value[WSX1000] * pow((1000 / (double)s->counter[N_TREE]), s->value[THINPOWER]);

		//modifified version for 1Km^2 spatial resolution
		s->value[WS_MAX] = s->value[WSX1000];

		logger(g_log, "wSmax = %g KgDM/tree\n", s->value[WS_MAX]);
		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM] * 1000.0 / (double)s->counter[N_TREE];
		logger(g_log, "AvStemMass after dead = %g Kg/tree\n", s->value[AV_STEM_MASS_KgDM]);
	}
	else
	{
		logger(g_log, "NO MORTALITY based SELF-THINNING RULE\n");
		logger(g_log, "Average Stem Mass < WSMax\n");
	}

	logger(g_log, "**********************************\n");



}

void update_biomass_after_mortality ()
{
	//fixme to do
}


/* Age mortality function from LPJ-GUESS */
void age_mortality (age_t *const a, species_t *const s)
{
	int dead_trees;

	/* Age probability function */
	s->value[AGEMORT] = (-(3 * log (0.001)) / (s->value[MAXAGE])) * pow (((double)a->value /s->value[MAXAGE]), 2);
	logger(g_log, "Max Age = %g years\n", s->value[MAXAGE]);
	logger(g_log, "Age factor (LPJ-GUESS) = %g\n", s->value[AGEMORT]);

	if ( ( s->counter[N_TREE] * s->value[AGEMORT] ) > 1 )
	{
		logger(g_log, "**MORTALITY based on Tree Age (LPJ)**\n");
		logger(g_log, "Age = %d years\n", a->value);
		logger(g_log, "Age factor (LPJ-GUESS) = %g\n", s->value[AGEMORT]);

		/* casting to int dead_trees */
		dead_trees = (int)(s->counter[N_TREE] * s->value[AGEMORT]);
		logger(g_log, "dead trees = %d\n", dead_trees);

		/* update biomass pools */
		s->value[LEAF_C] -= ( ( s->value[LEAF_C] / (double)s->counter[N_TREE] ) * dead_trees );
		s->value[STEM_C] -= ( ( s->value[STEM_C] / (double)s->counter[N_TREE] ) * dead_trees );
		s->value[COARSE_ROOT_C] -= ( ( s->value[COARSE_ROOT_C] / (double)s->counter[N_TREE] ) * dead_trees );
		s->value[FINE_ROOT_C] -= ( ( s->value[FINE_ROOT_C] / (double)s->counter[N_TREE] ) * dead_trees );
		s->value[BRANCH_C] -= ( ( s->value[BRANCH_C] / (double)s->counter[N_TREE] ) * dead_trees );
		//FIXME UPDATE ALSO LIVE AND DEAD POOLS

		/* update current number of trees */
		s->counter[N_TREE] -= dead_trees;
		logger(g_log, "Number of Trees  after age mortality = %d trees\n", s->counter[N_TREE]);

		/* assign to global variable */
		s->counter[DEAD_STEMS] = dead_trees;

	}
	else
	{
		logger(g_log, "**NO-MORTALITY based on Tree Age (LPJ)**\n");

		s->counter[DEAD_STEMS] = 0;
	}
	logger(g_log, "**********************************\n");
}


//from LPJ Growth efficiency mortality
void Greff_Mortality (species_t *const s)
{
	static double greff;
	static double kmort1 = 0.02; //modified from original version
	static double kmort2 = 0.3;
	static double mortgreff;
	static int GreffDeadTrees;

	logger(g_log, "**MORTALITY based on Growth Efficiency (LPJ)**\n");

	greff = (s->value[DEL_TOTAL_W] / (s->value[BIOMASS_FOLIAGE_tDM] * s->value[SLA_AVG]));
	//logger(g_log, "greff from LPJ = %g\n", greff);
	//logger(g_log, "DEL_TOTAL_W = %g\n", s->value[DEL_TOTAL_W]);
	//logger(g_log, "WF= %g\n", s->value[BIOMASS_FOLIAGE]);

	mortgreff = kmort1 / (1 + kmort2 * greff);
	//logger(g_log, "rate mort for greff from LPJ = %g\n", mortgreff);

	GreffDeadTrees = (int)(mortgreff * s->counter[N_TREE]);

	logger(g_log, "Dead trees for greff = %d\n", GreffDeadTrees);

	logger(g_log, "Number of trees before greff mortality = %d trees/ha\n", s->counter[N_TREE]);

	s->counter[N_TREE] -= GreffDeadTrees;
	logger(g_log, "Number of trees less greff mortality = %d trees/ha\n", s->counter[N_TREE]);

}


void Stool_mortality(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	//	species_t *s;
	//	s = &c->heights[height].ages[age].species[species];

	//to compute stools mortality in function on age only for coppices
	logger(g_log, "***STOOLS MORTALITY*** \n");
}
