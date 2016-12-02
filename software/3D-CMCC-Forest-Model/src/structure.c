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
	double temp_crown_area;
	double temp_crown_radius;
	double temp_crown_diameter;
	double previous_dbhdc_eff;
	double max_dbhdc_incr = 0.05;                   /* fraction of maximum dbhdc increment */

	height_t *h;
	dbh_t *d;
	age_t *a;
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
		if ( (c->heights[height].value - c->heights[height-1].value) > g_settings->tree_layer_limit )
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
		CHECK_CONDITION(c->tree_layers[layer].layer_n_height_class, <, 0);
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
							h = &c->heights[height];
							d = &c->heights[height].dbhs[dbh];
							a = &c->heights[height].dbhs[dbh].ages[age];
							s = &c->heights[height].dbhs[dbh].ages[age].species[species];

							logger(g_debug_log,"-layer = %d layer density = %g height = %g dbh = %g age = %d species = %s\n", layer,c->tree_layers[layer].layer_density, h->value, d->value, a->value, s->name);

							/************************************************************************************************************************/
							/* note: 04 Oct 2016 still USEFULL ?????*/
							/* compute potential maximum and minimum density for DBHDC function */
							//potential_max_min_density ( c );
							/* compute effective dbh/crown diameter */
							/*
							s->value[DBHDC_EFF] = ((s->value[DBHDCMIN] - s->value[DBHDCMAX]) / (s->value[DENMAX] - s->value[DENMIN]) *
									(c->tree_layers[layer].layer_density - s->value[DENMIN]) + s->value[DBHDCMAX]);
							logger(g_debug_log,"-DENMAX = %g\n", s->value[DENMAX]);
							logger(g_debug_log,"-DENMIN = %g\n", s->value[DENMIN]);
							logger(g_debug_log,"-DBHDCMAX = %g\n", s->value[DBHDCMAX]);
							logger(g_debug_log,"-DBHDCMIN = %g\n", s->value[DBHDCMIN]);
							logger(g_debug_log,"-DBHDC effective = %g\n", s->value[DBHDC_EFF]);
							 */

							/************************************************************************************************************************/
							/* note: 04 Oct 2016 */
							/* compute potential maximum and minimum density for DBHDC function */
							/* new DBHDC function */
							/* this function in mainly based on the assumptions that trees tend to occupy */
							/* all space they can, if they cannot then fixed values constrain their crown */
							/* see also: Lhotka and Loewenstein, 1997; Lhotka and Loewenstein, 2008 */

							/* previous dbhdc eff */
							previous_dbhdc_eff = s->value[DBHDC_EFF];

							temp_crown_area = (g_settings->sizeCell * g_settings->max_layer_cover) / (c->tree_layers[layer].layer_density * g_settings->sizeCell);

							temp_crown_radius = sqrt(temp_crown_area / Pi);

							temp_crown_diameter = temp_crown_radius * 2.;

							s->value[DBHDC_EFF] = temp_crown_diameter / d->value;
							logger(g_debug_log,"-DBHDC effective = %g\n", s->value[DBHDC_EFF]);

							/* check if current dbhdc_eff grows to much (case when there's thinning) */
							/* this is checked to avoid unrealistic crown area increment */

							//note max_dbhdc_incr corresponds to an arbitrary increment of n value
							if ( ( year ) && ( s->value[DBHDC_EFF] > ( previous_dbhdc_eff + (previous_dbhdc_eff * max_dbhdc_incr ) ) ) )
							{
								s->value[DBHDC_EFF] = previous_dbhdc_eff + ( previous_dbhdc_eff * max_dbhdc_incr );
							}


							/************************************************************************************************************************/
							/* check */
							if (s->value[DBHDC_EFF] > s->value[DBHDCMAX])
							{
								logger(g_debug_log,"-DBHDC effective (%g) > DBHDCMAX (%g) \n", s->value[DBHDC_EFF] , s->value[DBHDCMAX]);
								s->value[DBHDC_EFF] = s->value[DBHDCMAX];
								logger(g_debug_log,"-DBHDC effective = %g\n", s->value[DBHDC_EFF]);
							}
							if (s->value[DBHDC_EFF] < s->value[DBHDCMIN])
							{
								logger(g_debug_log,"-DBHDC effective (%g) > DBHDCMIN (%g) \n", s->value[DBHDC_EFF] , s->value[DBHDCMIN]);
								s->value[DBHDC_EFF] = s->value[DBHDCMIN];
								logger(g_debug_log,"-DBHDC effective = %g\n", s->value[DBHDC_EFF]);
							}
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
							logger(g_debug_log, "----------------------------------\n");
							crown_allometry ( c, height, dbh, age, species );
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
						}
					}
				}
			}
		}
		if( !layer )
		{
			logger(g_debug_log, "-Canopy cover DBH-DC layer (%d) level = %g%%\n", layer, c->tree_layers[layer].layer_cover * 100.0);
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
								while ( c->tree_layers[layer].layer_cover > g_settings->max_layer_cover &&  s->value[DBHDC_EFF] > s->value[DBHDCMIN] )
								{

									/* reduce DBHDC_EFF */
									s->value[DBHDC_EFF] -= 0.001;

									/* remove current class cover for layer cover */
									c->tree_layers[layer].layer_cover -= s->value[CANOPY_COVER_PROJ];

									/* recall crown allometry */
									crown_allometry ( c, height, dbh, age, species );

									/* recompute layer cover with current DBHDC_EFF */
									c->tree_layers[layer].layer_cover += s->value[CANOPY_COVER_PROJ];

									/* check if self-pruning was enough */
									if ( c->tree_layers[layer].layer_cover > g_settings->max_layer_cover && s->value[DBHDC_EFF] <= s->value[DBHDCMIN] )
									{
										/* self-pruning was not enough */
										self_thinning_mortality (c, layer);
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
	/* compute daily overall cell cover (cell level) */
	logger(g_debug_log, "*daily overall cell cover (cell level))*\n");

	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		/* note: overall cell cover can't exceed its area */
		c->cell_cover += c->tree_layers[layer].layer_cover;

		if ( c->cell_cover > 1)
		{
			c->cell_cover = 1;
		}
	}
	logger(g_debug_log, "-Number of trees cell level = %d trees/cell\n", c->cell_n_trees);
	logger(g_debug_log, "-Canopy cover DBH-DC cell level = %g %%\n", c->cell_cover * 100.0);
	logger(g_debug_log, "**************************************\n");

	return 1;
}
/*************************************************************************************************************************/
int monthly_forest_structure (cell_t* const c)
{

	//logger(g_debug_log, "\n***MONTHLY FOREST STRUCTURE***\n");

	return 1;
}

int daily_forest_structure (cell_t *const c)
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
		if ( c->tree_layers_count > 1 )
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
						s = &c->heights[height].dbhs[dbh].ages[age].species[species];

						/*****************************************************************************************/

						/* compute daily canopy projected cover */
						s->value[DAILY_CANOPY_COVER_PROJ] = s->value[CANOPY_COVER_PROJ];


						/* canopy cannot absorb more than 100% of incoming flux (e.g. rain) */
						if( s->value[DAILY_CANOPY_COVER_PROJ] > 1. )
						{
							s->value[DAILY_CANOPY_COVER_PROJ] = 1.;
						}
						logger(g_debug_log, "%s height class canopy projected cover = %g %%\n", s->name, s->value[DAILY_CANOPY_COVER_PROJ]*100.0);

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
						logger(g_debug_log, "%s height class canopy exposed cover = %g %%\n", s->name, s->value[CANOPY_COVER_EXP]*100.0);

						/*****************************************************************************************/
					}
				}
			}
		}
	}
	return 1;
}

void prephenology (cell_t *const c, const meteo_daily_t *const meteo_daily, const int day, const int month)
{
	int height;
	int dbh;
	int age;
	int species;

	species_t *s;

	/* it computes the vegetative state for each species class,
	 * the number of days of leaf fall and
	 * the rate for leaves reduction (for deciduous species) */

	/*VEG_UNVEG = 1 for veg period, = 0 for Un-Veg period*/


	logger(g_debug_log, "*prephenology*\n");
	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh)
		{
			for (age = c->heights[height].dbhs[dbh].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; species++)
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					/* FOR DECIDUOUS */
					if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
					{
						/* compute days for leaf fall based on the annual number of veg days */
						s->counter[DAY_FRAC_FOLIAGE_REMOVE] = (int)(s->value[LEAF_FALL_FRAC_GROWING] * s->counter[DAY_VEG_FOR_LEAF_FALL]);
						logger(g_debug_log, "-days of leaf fall for %s = %d day\n", c->heights[height].dbhs[dbh].ages[age].species[species].name, s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

						//currently model can simulate only forests in boreal hemisphere
						if ((meteo_daily->thermic_sum >= s->value[GROWTHSTART] && month <= 6) ||
								(meteo_daily->daylength >= s->value[MINDAYLENGTH] && month >= 6 && c->north == 0))
						{
							s->counter[VEG_UNVEG] = 1;
							logger(g_debug_log, "-%s is in veg period\n", s->name);
						}
						else
						{
							if (meteo_daily->daylength <= s->value[MINDAYLENGTH] && month >= 6 && c->north == 0 )
							{
								s->counter[LEAF_FALL_COUNTER] += 1;

								if(s->counter[LEAF_FALL_COUNTER]  <= (int)s->counter[DAY_FRAC_FOLIAGE_REMOVE])
								{
									/*days of leaf fall*/
									s->counter[VEG_UNVEG] = 1;
								}
								else
								{
									/*outside days of leaf fall*/
									s->counter[VEG_UNVEG] = 0;
								}

							}
							else
							{
								s->counter[VEG_UNVEG] = 0;
								logger(g_debug_log, "-%s is in un-veg period\n", s->name);
							}
						}
					}
					/* FOR EVERGREEN */
					else
					{
						s->counter[VEG_UNVEG] = 1;
						logger(g_debug_log, "-%s is in veg period\n", s->name);
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n");
}


/*note: not used */
//void potential_max_min_density ( cell_t *const c )
//{
//
//	int height;
//	int dbh;
//	int age;
//	int species;
//
//	double low_dbh = 2;       /* minimum dbh (cm) */
//	double high_dbh = 100;    /* maximum dbh (cm) */
//
//	double min_crown_radius;  /* minimum crown radius (m) */
//	double min_crown_diameter;
//	double min_crown_area;
//
//	double max_crown_radius;
//	double max_crown_diameter;
//	double max_crown_area;
//	double trees_number;
//
//	height_t *h;
//	dbh_t *d;
//	age_t *a;
//	species_t *s;
//
//	for ( height = 0; height < c->heights_count; ++height )
//	{
//		h = &c->heights[height];
//
//		for ( dbh = 0; dbh < h->dbhs_count; ++dbh )
//		{
//			d = &c->heights[height].dbhs[dbh];
//
//			for ( age = 0; age < d->ages_count; ++age )
//			{
//				a = &c->heights[height].dbhs[dbh].ages[age];
//
//				for ( species = 0; species < a->species_count; ++species )
//				{
//					s = &c->heights[height].dbhs[dbh].ages[age].species[species];
//
//					/* compute maximum density */
//
//					/* compute minimum crown radius for maximum possible density */
//					/* note using minimum dbh with DBHDC_MIN */
//					min_crown_diameter = low_dbh * s->value[DBHDCMIN];
//
//					min_crown_radius = min_crown_diameter / 2.;
//
//					min_crown_area = pow(min_crown_radius, 2)* Pi;
//
//					trees_number = g_settings->sizeCell / min_crown_area;
//
//					s->value[DENMAX] = trees_number / g_settings->sizeCell;
//
//					/* compute minimum density */
//
//					/* compute maximum crown radius for minimum possible density */
//					/* note using maximum dbh with DBHDC_MAX */
//					max_crown_diameter = high_dbh * s->value[DBHDCMAX];
//
//					max_crown_radius = max_crown_diameter / 2.;
//
//					max_crown_area = pow(max_crown_radius, 2)* Pi;
//
//					trees_number = g_settings->sizeCell / max_crown_area;
//
//					s->value[DENMIN] = trees_number / g_settings->sizeCell;
//				}
//			}
//		}
//	}
//}
/***************************************************************************************************/
//
//void potential_max_min_canopy_cover (cell_t *const c)
//{
//	int height;
//	int dbh;
//	int age;
//	int species;
//
//	double pot_max_crown_diameter,pot_min_crown_diameter;
//	double pot_max_crown_area, pot_min_crown_area;
//	double pot_max_density,pot_min_density;
//
//	height_t *h;
//	dbh_t *d;
//	age_t *a;
//	species_t *s;
//
//	logger(g_debug_log,"\n*Potential_max_min_canopy_cover Function*\n");
//
//	for ( height = 0; height < c->heights_count; height++ )
//	{
//		h = &c->heights[height];
//
//		for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh)
//		{
//			d = &c->heights[height].dbhs[dbh];
//
//			for ( age = 0; age < d->ages_count; age++ )
//			{
//				a = &c->heights[height].dbhs[dbh].ages[age];
//
//				for ( species = 0; species < a->species_count; species ++)
//				{
//					s = &c->heights[height].dbhs[dbh].ages[age].species[species];
//
//					if (s->value[DBHDCMAX] != -9999 && s->value[DENMIN] != -9999)
//					{
//						logger(g_debug_log,"using DBHDCMAX and DBHDCMIN\n");
//
//						/* case low density */
//						logger(g_debug_log, "-in case of low density\n");
//
//						pot_max_crown_diameter = s->value[DBHDCMAX] * d->value;
//						pot_max_crown_area = ( Pi / 4) * pow (pot_max_crown_diameter, 2 );
//						logger(g_debug_log, "potential maximum crown area with DBHDCMAX = %g m^2\n", pot_max_crown_area);
//
//						pot_min_density = g_settings->sizeCell / pot_max_crown_area;
//						logger(g_debug_log, "number of potential minimum trees with DBHDCMAX = %g\n", pot_min_density);
//
//						/* case high density */
//						logger(g_debug_log, "-in case of high density\n");
//
//						pot_min_crown_diameter = s->value[DBHDCMIN] * d->value;
//						pot_min_crown_area = ( Pi / 4) * pow (pot_min_crown_diameter, 2 );;
//						logger(g_debug_log, "potential minimum crown area with DBHDCMIN = %g m^2\n", pot_min_crown_area);
//
//						pot_max_density = g_settings->sizeCell /pot_min_crown_area;
//						logger(g_debug_log, "number of potential maximum trees with DBHDCMIN = %g\n", pot_max_density);
//
//						s->value[DENMAX] = pot_max_density/g_settings->sizeCell;
//						logger(g_debug_log, "potential density with dbhdcmax (high density) = %g (%g tree)\n", s->value[DENMAX], s->value[DENMAX] * g_settings->sizeCell);
//
//						s->value[DENMIN] = pot_min_density/g_settings->sizeCell;
//						logger(g_debug_log, "potential density with dbhdcmin (low density) = %g (%g tree)\n", s->value[DENMIN], s->value[DENMIN] * g_settings->sizeCell);
//
//						/* check */
//						CHECK_CONDITION(pot_max_crown_diameter, <, pot_min_crown_diameter);
//						CHECK_CONDITION(pot_max_crown_area, <, pot_min_crown_area);
//						CHECK_CONDITION(pot_max_density, <, pot_min_density);
//					}
//					/* in case no values from parameterization files are given */
//					else
//					{
//						/* compute maximum crown area */
//						/*for references and variables see "Forest Mensuration" book 4th edition
//						 *B. Husch, T.W. Beers, J.A. Kershaw Jr.
//						 *edited by John Wiley & Sons, Inc
//						 *and Krajicek, et al., "Crown competition: a measure of density.
//						 *For. Sci. 7:36-42
//						 *Lhotka and Loewenstein 2008, Can J For Res
//						 */
//						logger(g_debug_log,"without using DBHDCMAX and DENMIN\n");
//
//						pot_max_crown_area = ((100.0*Pi)/(4*g_settings->sizeCell)) * (9.7344 + (11.48612 *
//								d->value + (3.345241 * pow(d->value, 2.0))));
//						logger(g_debug_log, "pot_max_crown_area = %g m^2\n", pot_max_crown_area);
//
//						pot_max_crown_diameter = 2.0 * sqrt(pot_max_crown_area/Pi);
//						logger(g_debug_log, "pot_max_crown_diameter = %g m\n", pot_max_crown_diameter);
//
//						/* recompute DBHDCMAX and DENMIN from MCA */
//						/*17 Oct 2013*/
//						s->value[DBHDCMAX] = pot_max_crown_diameter / d->value;
//						logger(g_debug_log, "-recomputed DBHDCMAX = %g \n", s->value[DBHDCMAX]);
//
//						s->value[DENMIN] = 1.0 / pot_max_crown_diameter;
//						logger(g_debug_log, "-recomputed DENMIN = %g tree/sizecell (%d trees)\n", s->value[DENMIN], s->value[DENMIN] * g_settings->sizeCell);
//					}
//
//					logger(g_debug_log, "DBHDCMAX = %g\n", s->value[DBHDCMAX]);
//					logger(g_debug_log, "DBHDCMIN = %g\n", s->value[DBHDCMIN]);
//					logger(g_debug_log, "DENMAX = %g\n", s->value[DENMAX]);
//					logger(g_debug_log, "DENMIN = %g\n", s->value[DENMIN]);
//
//					CHECK_CONDITION(s->value[DENMAX], <, s->value[DENMIN]);
//					CHECK_CONDITION(s->value[DBHDCMAX], <, s->value[DBHDCMIN]);
//				}
//			}
//		}
//	}
//}

