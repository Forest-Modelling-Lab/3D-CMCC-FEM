/* structure.c */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include "matrix.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "g-function.h"
#include "mortality.h"
#include "structure.h"
#include "g-function.h"
#include "allometry.h"
#include "canopy_cover.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;
//extern int MonthLength [];
//extern int MonthLength_Leap [];

/* */
int alloc_struct(void** t, int* count, int* avail, unsigned int size) {
	void *no_leak;

	if ( *avail ) {
		--*avail;
		++*count;
	} else {
		no_leak = realloc(*t, ++*count*size);
		if ( ! no_leak ) {
			--*count;
			return 0;
		}
		*t = no_leak;
	}
	return 1;
}

int layer_add(cell_t* const c)
{
	int ret;
	static tree_layer_t t_layer = { 0 };

	assert(c);

	ret = alloc_struct((void **)&c->tree_layers, &c->tree_layers_count, &c->t_layers_avail, sizeof(tree_layer_t));
	if ( ret )
	{
		c->tree_layers[c->tree_layers_count-1] = t_layer;
	}

	return ret;
}
/*************************************************************************************************************************/
int annual_forest_structure(cell_t* const c, const int year)
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;
	int zeta_count = 0;

	species_t *s;

	/* this function compute annually:
	 * -the number of forest layers comparing the tree height values of all tree height classes
	 * -layer density
	 * -DBHDC_EFF based on overall layer cover
	 * -crown diameter
	 * -crown area
	 * -class cover
	 * -layer cover
	 * -cell cover
	 * */

	logger(g_debug_log, "\n***ANNUAL FOREST STRUCTURE***\n");



	assert( ! c->tree_layers_count );

	for ( height = 0; height < c->heights_count; ++height )
	{
		assert( ! c->heights[height].height_z );
	}

	/*********************************************************************************************************/

	/* compute number of annual layers */
	/* note: it starts from the lowest height values up to the highest */

	/* add 1 layer by default */
	if ( ! layer_add(c) ) return 0;

	logger(g_debug_log, "*compute height_z*\n");

	/* note: it must starts from the lowest tree height class */
	qsort(c->heights, c->heights_count, sizeof(height_t), sort_by_heights_asc);

	logger(g_debug_log, "*compute height_z*\n");

	/* compute zeta counter */
	if (c->heights_count > 1)
	{
		for ( height = 0; height < c->heights_count-1; ++height )
		{
			logger(g_debug_log, "*value %g*\n\n", c->heights[height].value);

			// ALESSIOR TO ALESSIOC...this give error
			// on +1 YOU MUST remove -1 from count!
			if ( (c->heights[height+1].value - c->heights[height].value) > g_settings->tree_layer_limit )
			{
				++zeta_count;

				/* compute layer number and alloc memory for each one */
				if ( ! layer_add(c) ) return 0;
			}
		}
	}
	logger(g_debug_log, "*zeta_count %d*\n\n", zeta_count);
	logger(g_debug_log, "*c->t_layers_count %d*\n\n", c->tree_layers_count);

	/*****************************************************************************************/

	/* assign zeta for each height class */
	for ( height = c->heights_count -1 ; height >= 0; --height )
	{
		// ALESSIOR...on height == 0 you can't go to previous values (height-1)
		//if ( (c->heights[height].value - c->heights[height-1].value) > g_settings->tree_layer_limit )
		if ( height && (c->heights[height].value - c->heights[height-1].value) > g_settings->tree_layer_limit )
		{
			c->heights[height].height_z = zeta_count;
			--zeta_count;
		}
		else
		{
			c->heights[height].height_z = zeta_count;
		}
		logger(g_debug_log, "*value %g z = %d*\n\n", c->heights[height].value, c->heights[height].height_z);
	}

	logger(g_debug_log, "-Number of height classes = %d per cell\n", c->heights_count);
	logger(g_debug_log, "-Number of layers = %d per cell\n\n", c->tree_layers_count);
	logger(g_debug_log,"***********************************\n");

	/* check */
	CHECK_CONDITION(c->tree_layers_count, <, 1);
	CHECK_CONDITION(c->tree_layers_count, >, c->heights_count);

	/*************************************************************************************/
	/* compute numbers of height classes within each layer */
	logger(g_debug_log, "*compute numbers of height classes within each layer*\n\n");

	for ( layer = c->tree_layers_count - 1; layer >= 0 ; --layer )
	{
		for ( height = c->heights_count -1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				++c->tree_layers[layer].layer_n_height_class;
			}
		}
		logger(g_debug_log, "-layer %d height class(es) = %d\n", layer, c->tree_layers[layer].layer_n_height_class);

		/* check */
		CHECK_CONDITION(c->tree_layers[layer].layer_n_height_class, <, ZERO);
	}
	logger(g_debug_log, "**************************************\n\n");

	/*************************************************************************************/
	/* compute numbers of trees within each layer */
	logger(g_debug_log, "*compute numbers of trees within each layer*\n\n");

	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
				{
					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							if( layer == c->heights[height].height_z )
							{
								c->tree_layers[layer].layer_n_trees += c->heights[height].dbhs[dbh].ages[age].species[species].counter[N_TREE];
							}
						}
					}
				}
			}
		}
		logger(g_debug_log, "-layer %d number of trees = %d\n", layer, c->tree_layers[layer].layer_n_trees);
	}
	logger(g_debug_log, "**************************************\n\n");

	/*************************************************************************************/
	/* compute density within each layer */
	logger(g_debug_log, "*compute density within each layer*\n\n");

	for (layer = c->tree_layers_count - 1; layer >= 0; layer --)
	{
		c->tree_layers[layer].layer_density = c->tree_layers[layer].layer_n_trees / g_settings->sizeCell;

		logger(g_debug_log, "-layer %d density = %g layer\n", layer, c->tree_layers[layer].layer_density);
	}
	logger(g_debug_log, "**************************************\n\n");

	/*************************************************************************************/

	/* compute effective dbh/crown diameter ratio within each class based on layer density (class level) */
	logger(g_debug_log, "compute effective dbh/crown diameter ratio within each class based on layer density (class level)\n\n");

	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		logger(g_debug_log, "----------------------------------\n");

		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
				{
					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							dbhdc_function (c, layer, height, dbh, age, species, year);
						}
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n\n");

	/*************************************************************************************/

	/* Compute Crown and Canopy allometry */
	logger(g_debug_log, "*Compute Crown and Canopy allometry*\n\n");

	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
				{
					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							crown_allometry ( c, height, dbh, age, species );
							canopy_cover    ( c, height, dbh, age, species );
						}
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n\n");

	/*************************************************************************************/

	/* compute layer canopy cover within each layer (layer level) */
	logger(g_debug_log, "*compute layer canopy cover within each layer (layer level)*\n\n");

	//todo: control if with a drastic tree number reduction (e.g. management) there's a unrealistic strong variation in DBHDCeffective
	for (layer = c->tree_layers_count - 1; layer >= 0; --layer)
	{
		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
				{
					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s = &c->heights[height].dbhs[dbh].ages[age].species[species];
							c->tree_layers[layer].layer_cover += s->value[CANOPY_COVER_PROJ];
							logger(g_debug_log, "CANOPY_COVER_PROJ = %f\n", s->value[CANOPY_COVER_PROJ]);
							logger(g_debug_log, "layer_cover       = %f\n", c->tree_layers[layer].layer_cover);
						}
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n");

	/*************************************************************************************/

	/* check if layer cover exceeds maximum layer cover */
	logger(g_debug_log, "*check if layer cover exceeds maximum layer cover*\n\n");

	for (layer = c->tree_layers_count - 1; layer >= 0; --layer)
	{
		if ( c->tree_layers[layer].layer_cover > g_settings->max_layer_cover )
		{
			/* note: 04 Oct 2016 */
			/* call of this function is due to the assumption that:
			  -overall layer canopy cover cannot exceeds its maximum
			  -DBHDC_EFF cannot be < DBHDCMIN
			 */

			/* start to reduce DBHDC_EFF from the lowest height class */
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

			for ( height = 0; height < c->heights_count ; ++height )
			{
				if( layer == c->heights[height].height_z )
				{
					for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
					{
						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								double old_layer_cover;
								old_layer_cover = c->tree_layers[layer].layer_cover;

								s = &c->heights[height].dbhs[dbh].ages[age].species[species];

								while ( c->tree_layers[layer].layer_cover >= g_settings->max_layer_cover )
								{
									/** self-thinning **/
									if ( s->value[DBHDC_EFF] <= s->value[DBHDCMIN] )
									{
										self_thinning_mortality ( c, layer, year );
									}
									/** self-pruning **/
									else
									{
										c->tree_layers[layer].layer_cover -= s->value[CANOPY_COVER_PROJ];

										s->value[DBHDC_EFF] -= 0.001;

										crown_allometry ( c, height, dbh, age, species );
										canopy_cover    ( c, height, dbh, age, species );

										/* check for recompued canopy cover */
										c->tree_layers[layer].layer_cover += s->value[CANOPY_COVER_PROJ];

										/* self pruning function */
										self_pruning ( c, height, dbh, age, species, old_layer_cover );

										/** self-thinning **/
										if ( s->value[DBHDC_EFF] <= s->value[DBHDCMIN] )
										{
											self_thinning_mortality ( c, layer, year );
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n");
	logger(g_debug_log, "**************************************\n");
	/*************************************************************************************/

	/* compute total number of trees */
	logger(g_debug_log, "*compute total number of trees*\n\n");

	c->n_trees = 0;
	for (layer = c->tree_layers_count - 1; layer >= 0; --layer)
	{
		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
				{
					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							c->n_trees += s->counter[N_TREE];
						}
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n");
	logger(g_debug_log, "**************************************\n");
	/*************************************************************************************/
	/* compute daily overall cell cover (cell level) */
	logger(g_debug_log, "*daily overall cell cover (cell level))*\n");

	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		/* note: overall cell cover can't exceed its area */
		c->cell_cover += c->tree_layers[layer].layer_cover;

		if ( c->cell_cover > 1)
		{
			//fixme
			c->cell_cover = 1;
			//puts("attention canopy cover at cell level exceeds 1!!!\n");
		}
	}

	/* compute overall bare soil cover */
	c->bare_soil_cover = 1. - c->cell_cover;

	logger(g_debug_log, "-Number of trees cell level = %d trees/cell\n", c->n_trees);
	logger(g_debug_log, "-Canopy cover at cell level = %g %%\n", c->cell_cover * 100.0);
	logger(g_debug_log, "-Bare soil cover at cell level = %g %%\n", c->bare_soil_cover * 100.0);
	logger(g_debug_log, "**************************************\n");

	return 1;
}
/*************************************************************************************************************************/
int monthly_forest_structure (cell_t* const c)
{

	//logger(g_debug_log, "\n***MONTHLY FOREST STRUCTURE***\n");

	return 1;
}

int daily_forest_structure ( cell_t *const c, const meteo_daily_t *const meteo_daily )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	int layer_height_class_counter;

	tree_layer_t *l;
	height_t *h;
	species_t *s;

	/* This function computes at class level and at daily scale :
	 * -Canopy Cover Projected (based on daily LAI)
	 * -Canopy Cover Exposed (based on daily LAI)
	 * (being LAI variables at daily scale Canopy Cover must be updated)
	 */


	logger(g_debug_log, "\n***DAILY FOREST STRUCTURE***\n");

	/*******************************COMPUTE VERTICAL COMPETITION*******************************/

	logger(g_debug_log, "\n***VERTICAL COMPUTATION***\n");

	/* compute average layer tree height */
	for (layer = c->tree_layers_count - 1; layer >= 0; --layer)
	{
		l = &c->tree_layers[layer];
		l->layer_avg_tree_height = 0.;
		layer_height_class_counter = 0;

		for ( height = 0; height < c->heights_count ; ++height )
		{
			if ( layer == c->heights[height].height_z )
			{
				h = &c->heights[height];
				/* cumulate class height values */
				l->layer_avg_tree_height += h->value;

				/* increment counter */
				++layer_height_class_counter;
			}
		}

		/* to avoid division by zero */
		if ( layer_height_class_counter > 0 )
		{
			/* compute mean */
			l->layer_avg_tree_height /= l->layer_n_height_class;
		}

		logger(g_debug_log, "\n-Layer %d avg tree height = %g\n", layer, l->layer_tree_height_modifier);
	}

	/*****************************************************************************/
	/* compute TREE HEIGHT VERTICAL MODIFIER */
	for (layer = c->tree_layers_count - 1; layer >= 0; --layer)
	{
		l = &c->tree_layers[layer];

		/* only if more than one layer is present */
		//ALESSIOR
		// bad counter specified ?
		//if ( c->tree_layers_count > 1 )
		if ( layer > 1 )
		{
			/* following Wallace et al., 1991, Cannell and Grace 1993 */
			/* note: it considers differences in average values of tree height among all over the layers */

			/* upper layer */
			l->layer_tree_height_modifier = 0.5 * ( 1. + pow ( 2. , ( - c->tree_layers[layer].layer_avg_tree_height / c->tree_layers[layer-1].layer_avg_tree_height ) ) -
					( pow ( 2. , ( - c->tree_layers[layer-1].layer_avg_tree_height / c->tree_layers[layer].layer_avg_tree_height ) ) ) );
			logger(g_debug_log, "\n-Layer %d light vertical modifier = %g\n", layer, l->layer_tree_height_modifier);

			/* lower layer */
			c->tree_layers[layer-1].layer_tree_height_modifier = 1. - l->layer_tree_height_modifier;
			logger(g_debug_log, "\n-Layer %d light vertical modifier = %g\n", layer - 1, c->tree_layers[layer-1].layer_tree_height_modifier);

		}
		else
		{
			/* no vertical competition */
			l->layer_tree_height_modifier = 1.;
			logger(g_debug_log, "\n-Layer %d light vertical modifier = %g\n", layer, l->layer_tree_height_modifier);
		}
	}

	/******************************COMPUTE HORIZONTAL COMPETITION******************************/

	logger(g_debug_log, "\n***HORIZONTAL COMPUTATION***\n");

	/* compute fraction of class cover (exposed and projected) */
	for (layer = c->tree_layers_count - 1; layer >= 0; --layer)
	{
		l = &c->tree_layers[layer];

		for ( height = 0; height < c->heights_count ; ++height )
		{
			h = &c->heights[height];

			for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh )
			{
				for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
				{
					for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
					{
						int i;
						int odd;
						double diff_cover;
						double max_cover;
						double min_cover;
						double temp_diff_cover;
						double temp_can_cover;
						double cum_temp_cover = 0.;
						int daylength = ROUND(meteo_daily->daylength);

						s = &c->heights[height].dbhs[dbh].ages[age].species[species];

						/*****************************************************************************************/

						/* compute daily canopy projected cover */
						s->value[DAILY_CANOPY_COVER_PROJ] = s->value[CANOPY_COVER_PROJ];


						/* canopy cannot absorbs more than 100% of incoming flux (e.g. rain) */
						if( s->value[DAILY_CANOPY_COVER_PROJ] > 1. )
						{
							s->value[DAILY_CANOPY_COVER_PROJ] = 1.;
						}
						logger(g_debug_log, "%s height class canopy projected cover = %g %%\n", s->name, s->value[DAILY_CANOPY_COVER_PROJ] * 100.);

						/*****************************************************************************************/

						/* compute daily canopy exposed cover */
						s->value[DAILY_CANOPY_COVER_EXP] = s->value[CANOPY_COVER_EXP];

						/* canopy cannot absorb more than 100% of incoming flux (e.g. rain) */
						if( s->value[DAILY_CANOPY_COVER_EXP] > 1. )
						{
							s->value[DAILY_CANOPY_COVER_EXP] = 1.;
						}

						/* integrate with layer TREE HEIGHT MODIFIER */
						if ( layer == c->heights[height].height_z )
						{
							s->value[DAILY_CANOPY_COVER_EXP] *= l->layer_tree_height_modifier ;
						}

						/* canopy cannot absorb more than 100% of incoming flux (e.g. light) */
						if( s->value[DAILY_CANOPY_COVER_EXP] > 1. )
						{
							s->value[DAILY_CANOPY_COVER_EXP] = 1.;
						}

						/** now integrating all over the daylength **/
						max_cover       = s->value[DAILY_CANOPY_COVER_EXP];
						min_cover       = s->value[DAILY_CANOPY_COVER_PROJ];
						diff_cover      = max_cover - min_cover;
						temp_diff_cover = diff_cover / ( meteo_daily->daylength / 2. );

						/* check if odd */
						if(daylength %2 != 0) odd = 0; /* ok is a odd number */
						else odd = 1;                  /* in NOT an odd number */

						/* morning to nadir */
						for ( i = 0; i < daylength ; ++i)
						{
							if ( ! odd )
							{
								if ( i == 0 )
								{
									temp_can_cover  = max_cover;
								}
								else if ( i <= ( daylength  / 2. ) )
								{
									/* morning to nadir */
									temp_can_cover -= ( temp_diff_cover );
								}
								else
								{
									/* from nadir to evening */
									temp_can_cover += ( temp_diff_cover );
								}
							}
							else
							{
								if ( i == 0 )
								{
									temp_can_cover  = max_cover;
								}
								else if ( i <= ( daylength  / 2. ) -1 )
								{
									/* morning to nadir */
									temp_can_cover -= ( temp_diff_cover );
								}
								else if (i <= ( daylength  / 2. ))
								{
									/* nothing to do */
								}
								else
								{
									/* from nadir to evening */
									temp_can_cover += ( temp_diff_cover );
								}
							}
							/* cumulate */
							cum_temp_cover += temp_can_cover;
						}

						/** compute integrating all over the daylength corresponding canopy intercepting cover **/
						s->value[DAILY_CANOPY_COVER_EXP] = cum_temp_cover / daylength;

						/* check */
						CHECK_CONDITION ( s->value[DAILY_CANOPY_COVER_EXP], > , 1 )
					}
				}
			}
		}
	}
	return 1;
}


