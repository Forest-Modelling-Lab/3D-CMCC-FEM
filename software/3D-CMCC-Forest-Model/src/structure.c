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

extern settings_t* g_settings;
extern logger_t* g_log;

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];

void daily_forest_structure (cell_t *const c, const int day, const int month, const int year)
{
	int layer;
	int height;
	int age;
	int species;

	height_t *h;
	age_t *a;
	species_t *s;

	/* it defines the number of tree height classes in each canopy layer,
	 * the height class level cell coverage through the DBHDC_EFF function,
	 * the density of each layer and then the current tree mortality based on it
	 * on a daily basis
	 */

	logger(g_log, "\n\n***DAILY FOREST STRUCTURE***\n\n");

	/**************************************************************************************************/
	/* compute numbers of height classes within each layer */
	logger(g_log, "compute numbers of height classes within each layer\n\n");

	for ( layer = c->t_layers_count - 1; layer >= 0 ; --layer )
	{
		for ( height = c->heights_count -1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				c->t_layers[layer].layer_n_height_class += 1;
			}
		}
		logger(g_log, "-layer %d height class(es) = %d\n", layer, c->t_layers[layer].layer_n_height_class);
	}
	/* check */
	CHECK_CONDITION(c->t_layers[layer].layer_n_height_class, < 0);

	logger(g_log, "**************************************\n\n");

	/*************************************************************************************************/
	/* compute numbers of trees within each layer */
	logger(g_log, "compute numbers of trees within each layer\n\n");

	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						if( layer == c->heights[height].height_z )
						{
							c->t_layers[layer].layer_n_trees += c->heights[height].ages[age].species[species].counter[N_TREE];
						}
					}
				}
			}
		}
		logger(g_log, "-layer %d number of trees = %d\n", layer, c->t_layers[layer].layer_n_trees);
	}
	logger(g_log, "**************************************\n\n");

	/*************************************************************************************************/
	/* compute density within each layer */
	logger(g_log, "compute density within each layer\n\n");

	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		c->t_layers[layer].layer_density = c->t_layers[layer].layer_n_trees / g_settings->sizeCell;

		logger(g_log, "-layer %d density = %g\n", layer, c->t_layers[layer].layer_density);
	}
	logger(g_log, "**************************************\n\n");

	/*************************************************************************************************/
	/* compute effective dbh/crown diameter ratio within each class based on layer density (class level) */
	logger(g_log, "compute effective dbh/crown diameter ratio within each class based on layer density (class level)\n\n");

	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		logger(g_log, "----------------------------------\n");
		logger(g_log, "layer %d ", layer);

		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						h = &c->heights[height];
						a = &c->heights[height].ages[age];
						s = &c->heights[height].ages[age].species[species];

						logger(g_log,"height = %g age = %d species = %s\n", h->value, a->value, s->name);

						s->value[DBHDC_EFF] = ((s->value[DBHDCMIN] - s->value[DBHDCMAX]) / (s->value[DENMAX] - s->value[DENMIN]) *
								(c->t_layers[layer].layer_density - s->value[DENMIN]) + s->value[DBHDCMAX]);
						logger(g_log,"DBHDC effective = %g\n", s->value[DBHDC_EFF]);
					}
				}
			}
		}
	}

	/*************************************************************************************************/
	/* compute effective crown diameter and crown area and class cover using DBH-DC */
	logger(g_log, "compute effective crown diameter and crown area and class cover using DBH-DC\n\n");

	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		logger(g_log, "----------------------------------\n");
		logger(g_log, "layer %d ", layer);

		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						h = &c->heights[height];
						a = &c->heights[height].ages[age];
						s = &c->heights[height].ages[age].species[species];

						logger(g_log,"height = %g age = %d species = %s\n", h->value, a->value, s->name);

						s->value[CROWN_DIAMETER_DBHDC] = s->value[AVDBH] * s->value[DBHDC_EFF];
						logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC]);

						/* Crown Area using DBH-DC */
						s->value[CROWN_AREA_DBHDC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC], 2 );
						logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC]);

						/* Canopy Cover using DBH-DC */
						s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC] * s->counter[N_TREE] / g_settings->sizeCell;
						logger(g_log, "Canopy cover DBH-DC class level = %g %%\n", s->value[CANOPY_COVER_DBHDC] * 100.0);
					}
				}
			}
		}
	}
	logger(g_log, "**************************************\n\n");

	/*************************************************************************************************/
	/* compute layer canopy cover within each layer (layer level) */
	logger(g_log, "compute layer canopy cover within each layer (layer level)\n\n");

	//todo: control if with a drastic tree number reduction (e.g. management) there's a unrealistic strong variation in DBHDCeffective

	for (layer = c->t_layers_count - 1; layer >= 0; --layer)
	{
		for ( height = 0; height < c->heights_count ; ++height )
		{
			if( layer == c->heights[height].height_z )
			{
				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						s = &c->heights[height].ages[age].species[species];
						c->t_layers[layer].layer_cover += s->value[CANOPY_COVER_DBHDC];
					}
				}
			}
		}
		logger(g_log, "-Canopy cover DBH-DC layer (%d) level = %g %%\n", layer, c->t_layers[layer].layer_cover * 100.0);
	}
	logger(g_log, "**************************************\n");

	/**************************************************************************************************/
	/* compute daily overall cell cover (cell level) */
	logger(g_log, "daily overall cell cover (cell level))\n");

	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		/* number of trees per cell */
		c->cell_n_trees += c->t_layers[layer].layer_n_trees;

		/* note: overall cell cover can't exceed its area */
		c->cell_cover += c->t_layers[layer].layer_cover;

		if ( c->cell_cover > 1)
		{
			c->cell_cover = 1;
		}
	}
	logger(g_log, "-Number of trees cell level = %d trees/cell\n", c->cell_n_trees);
	logger(g_log, "-Canopy cover DBH-DC cell level = %g %%\n", c->cell_cover * 100.0);
	logger(g_log, "**************************************\n\n");

}
/*************************************************************************************************************************/
void monthly_forest_structure (cell_t* const c, const int day, const int month, const int year)
{
	int layer;
	int height;
	int age;
	int species;

	char mortality;                            /* mortality flag, = n (no mortality), = y (mortality) */

	species_t *s;

	/* monthly check for self pruning or self thinning (crowding competition) (layer level) */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month]) : (MonthLength[month] )) == c->doy )
	{
		logger(g_log, "\n***MONTHLY FOREST STRUCTURE (mortality)***\n");

		logger(g_log, "monthly check for self pruning or self thinning (crowding competition) (layer level)\n\n");

		for (layer = c->t_layers_count - 1; layer >= 0; layer --)
		{
			/* check if layer exceeds maximum layer coverage */
			if (c->t_layers[layer].layer_cover >= g_settings->max_layer_cover)
			{
				logger(g_log, "layer cover exceeds max layer cover for layer %d\n", layer);

				//test
				//			/* first of all it checks if self-pruning is enough */
				//			/* SELF-PRUNING */
				//			if ( !self_pruning ( c, layer ))
				//			{
				//				/* also self-thinning is necessary */
				//				/* SELF-THINNING */
				//				self_thinning (c, layer);
				//			}

				//test if test above is ok remove
				/* SELF-THINNING */
				self_thinning (c, layer);

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
				logger(g_log, "no crowding competition happens for layer %d\n", layer);

				mortality = 'n';
			}
			/* check */
			CHECK_CONDITION(c->t_layers[layer].layer_cover , > g_settings->max_layer_cover);
		}
		logger(g_log, "**************************************\n\n");

		/**************************************************************************************************/
		/* check if REcompute numbers of height classes, tree number and density after mortality within each layer */
		if ( mortality == 'y' )
		{
			/* REcompute numbers of height classes, tree number and density after mortality within each layer */
			logger(g_log, "REcompute numbers of height classes, tree number and density after mortality within each layer\n\n");

			for ( layer = c->t_layers_count - 1; layer >= 0 ; --layer )
			{
				for ( height = c->heights_count -1; height >= 0 ; --height )
				{
					if( layer == c->heights[height].height_z )
					{
						/* recompute number of height classes */
						c->t_layers[layer].layer_n_height_class += 1;
					}
					for ( age = 0; age < c->heights[height].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
						{
							s = &c->heights[height].ages[age].species[species];

							if( layer == c->heights[height].height_z )
							{
								/* recompute number of trees for each layer */
								c->t_layers[layer].layer_n_trees += s->counter[N_TREE];
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

			for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
			{
				/* compute total number of trees per cell */
				c->cell_n_trees += c->t_layers[layer].layer_n_trees;

				/* assuming that plants tend to occupy the part of the cell not covered by the others */
				c->cell_cover += c->t_layers[layer].layer_cover;
			}
			logger(g_log, "-number of trees = %d cell\n", c->cell_n_trees);
			logger(g_log, "-density = %g cell\n", c->cell_cover);
			logger(g_log, "**************************************\n\n");
		}
	}
}

/*************************************************************************************************************************/
void annual_forest_structure(cell_t* const c)
{
	int height;

	/* this function compute annually the number of forest layers
	 * comparing the tree height values of all tree height classes */

	logger(g_log, "\n***ANNUAL FOREST STRUCTURE***\n");

	/* compute number of annual layers */
	logger(g_log, "Number of forest layers\n");

	for ( height = c->heights_count - 1; height >= 0; --height )
	{
		/* first height class processed */
		if ( height == c->heights_count - 1 )
		{
			c->t_layers_count = 1;
		}
		/* other classes processed */
		else
		{
			if ( ( c->heights[height + 1].value - c->heights[height].value ) > g_settings->tree_layer_limit )
			{
				++c->t_layers_count;
			}
		}
	}
	logger(g_log, "Number of height classes = %d\n", c->heights_count);
	logger(g_log, "Number of forest layers = %d\n\n", c->t_layers_count);

	/* check */
	CHECK_CONDITION(+c->t_layers_count, < 1);
	CHECK_CONDITION(+c->t_layers_count, > c->heights_count);

	/*************************************************************************************/

	/* assign "z" values to height classes */
	logger(g_log, "Assign 'z' values\n");

	for ( height = c->heights_count - 1; height >= 0; --height )
	{
		/* first height class processed */
		if ( height == c->heights_count - 1 )
		{
			c->heights[height].height_z = c->t_layers_count - 1;
			logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].height_z);
		}
		/* other class processed */
		else
		{
			if ( ( c->heights[height + 1].value - c->heights[height].value ) > g_settings->tree_layer_limit )
			{
				c->heights[height].height_z = c->heights[height + 1].height_z - 1;
				logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].height_z);
			}
		}
	}

	logger(g_log,"******************************************************\n");
}

void potential_max_min_canopy_cover (cell_t *const c)
{
	int height;
	int age;
	int species;

	double pot_max_crown_diameter,pot_min_crown_diameter;
	double pot_max_crown_area, pot_min_crown_area;
	double pot_max_density,pot_min_density;

	height_t *h;
	age_t *a;
	species_t *s;

	logger(g_log,"\nPotential_max_min_canopy_cover Function\n");

	for ( height = 0; height < c->heights_count; height++ )
	{
		h = &c->heights[height];

		for ( age = 0; age < h->ages_count; age++ )
		{
			a = &c->heights[height].ages[age];

			for ( species = 0; species < a->species_count; species ++)
			{
				s = &c->heights[height].ages[age].species[species];

				if (s->value[DBHDCMAX] != -9999 && s->value[DENMIN] != -9999)
				{
					logger(g_log,"using DBHDCMAX and DBHDCMIN\n");
					/* case low density */
					//logger(g_log, "-in case of low density\n");

					pot_max_crown_diameter = s->value[DBHDCMAX] * s->value[AVDBH];
					pot_max_crown_area = pow(((pot_max_crown_diameter)/2),2)*Pi;
					logger(g_log, "potential maximum crown area with DBHDCMAX = %g m^2\n", pot_max_crown_area);

					pot_min_density = g_settings->sizeCell / pot_max_crown_area;
					logger(g_log, "number of potential minimum trees with DBHDCMAX = %g\n", pot_min_density);

					/* case high density */
					//logger(g_log, "-in case of high density\n");

					pot_min_crown_diameter = s->value[DBHDCMIN] * s->value[AVDBH];
					pot_min_crown_area = pow(((pot_min_crown_diameter)/2),2)*Pi;
					logger(g_log, "potential minimum crown area with DBHDCMIN = %g m^2\n", pot_min_crown_area);

					pot_max_density = g_settings->sizeCell /pot_min_crown_area;
					logger(g_log, "number of potential maximum trees with DBHDCMIN = %g\n", pot_max_density);

					s->value[DENMAX] = pot_max_density/g_settings->sizeCell;
					logger(g_log, "potential density with dbhdcmax (high density) = %g (%g tree)\n", s->value[DENMAX], s->value[DENMAX] * g_settings->sizeCell);

					s->value[DENMIN] = pot_min_density/g_settings->sizeCell;
					logger(g_log, "potential density with dbhdcmin (low density) = %g (%g tree)\n", s->value[DENMIN], s->value[DENMIN] * g_settings->sizeCell);

					/* check */
					CHECK_CONDITION(pot_max_crown_diameter, < pot_min_crown_diameter);
					CHECK_CONDITION(pot_max_crown_area, < pot_min_crown_area);
					CHECK_CONDITION(pot_max_density, < pot_min_density);
				}
				/* in case no values from parameterization files are given */
				else
				{
					/*compute maximum crown area*/
					//	//TODO CHECK IF USE IT
					/*for references and variables see "Forest Mensuration" book 4th edition
					 *B. Husch, T.W. Beers, J.A. Kershaw Jr.
					 *edited by John Wiley & Sons, Inc
					 *and Krajicek, et al., "Crown competition: a measure of density.
					 *For. Sci. 7:36-42
					 *Lhotka and Loewenstein 2008, Can J For Res
					 */
					logger(g_log,"without using DBHDCMAX and DBHDCMIN\n");

					pot_min_crown_area = ((100.0*Pi)/(4*g_settings->sizeCell)) * (9.7344 + (11.48612 * s->value[AVDBH] + (3.345241 *	pow(s->value[AVDBH], 2.0))));
					logger(g_log, "potential_minimum_crown_area = %g m^2\n", pot_min_crown_area);

					pot_min_crown_diameter = 2.0 * sqrt(pot_min_crown_area/Pi);
					logger(g_log, "potential_minimum_crown_diameter= %g m\n", pot_min_crown_diameter);

					/*recompute DBHDCmax and DENmin from MCA*/
					/*17 Oct 2013*/
					s->value[DBHDCMAX] = pot_min_crown_diameter / s->value[AVDBH];
					logger(g_log, "-recomputed DBHDCMAX = %g \n", s->value[DBHDCMAX]);

					s->value[DENMIN] = 1.0 / pot_min_crown_area;
					logger(g_log, "-recomputed DENMIN = %g tree/sizecell\n", s->value[DENMIN]);
				}

				logger(g_log, "DBHDCMAX = %g\n", s->value[DBHDCMAX]);
				logger(g_log, "DBHDCMIN = %g\n", s->value[DBHDCMIN]);
				logger(g_log, "DENMAX = %g\n", s->value[DENMAX]);
				logger(g_log, "DENMIN = %g\n", s->value[DENMIN]);

				CHECK_CONDITION(s->value[DENMAX], < s->value[DENMIN]);
				CHECK_CONDITION(s->value[DBHDCMAX], < s->value[DBHDCMIN]);
			}
		}
	}
}

void daily_check_for_veg_period (cell_t *const c, const meteo_daily_t *const meteo_daily, const int day, const int month)
{
	int height;
	int age;
	int species;

	species_t *s;

	/* it computes the vegetative state for each species class,
	 * the number of days of leaf fall and
	 * the rate for leaves reduction (for deciduous species) */

	/*VEG_UNVEG = 1 for veg period, = 0 for Un-Veg period*/


	logger(g_log, "\n****DAILY_FOREST_VEG_PERIOD****\n");
	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				s = &c->heights[height].ages[age].species[species];

				/* FOR DECIDUOUS */
				if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
				{
					/* compute days for leaf fall based on the annual number of veg days */
					s->counter[DAY_FRAC_FOLIAGE_REMOVE] = (int)(s->value[LEAF_FALL_FRAC_GROWING] * s->counter[DAY_VEG_FOR_LEAF_FALL]);
					logger(g_log, "Days of leaf fall for deciduous = %d day\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

					//currently model can simulate only forests in boreal hemisphere
					if ((meteo_daily->thermic_sum >= s->value[GROWTHSTART] && month <= 6) ||
							(meteo_daily->daylength >= s->value[MINDAYLENGTH] && month >= 6 && c->north == 0))
					{
						s->counter[VEG_UNVEG] = 1;
						logger(g_log, "%s is in veg period\n", s->name);
					}
					else
					{
						//check for case 0 of allocation
						if (meteo_daily->daylength <= s->value[MINDAYLENGTH] && month >= 6 && c->north == 0 )
						{

							logger(g_log, "DAY_FRAC_FOLIAGE_REMOVE %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

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
							logger(g_log, "%s is in un-veg period\n", s->name);
						}
					}
				}
				/* FOR EVERGREEN */
				else
				{
					s->counter[VEG_UNVEG] = 1;
					logger(g_log, "Veg period = %d \n", s->counter[VEG_UNVEG]);
					logger(g_log, "%s is in veg period\n", s->name);
				}
			}
		}
	}
	logger(g_log, "classes in veg period = %d\n", c->Veg_Counter);
}

