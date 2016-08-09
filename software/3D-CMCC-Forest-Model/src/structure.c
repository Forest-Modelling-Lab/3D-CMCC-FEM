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

extern settings_t* g_settings;
extern logger_t* g_log;

void Annual_Forest_structure (cell_t *const c)
{
	/*determines number of tree layer in function of:
	-differences between tree height classes
	-vegetative or un-vegetative period
	to determine crowding competition */

	int height;
	//int layer;
	double current_height;
	double previous_height;


	logger(g_log, "****ANNUAL_FOREST_STRUCTURE_ROUTINE****\n");
	logger(g_log, "--NUMBER OF ANNUAL LAYERS--\n");

	if (g_settings->spatial == 'u')
	{
		/* sort by ascending tree heights */
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			current_height = c->heights[height].value;
			/* for the first height class processed */
			if (height == c->heights_count - 1 )
			{
				//ALESSIOC
				//c->annual_layer_number = 1;
				previous_height = current_height;
			}
			/* for the other height class processed */
			else
			{
				/* check if differences with previous height is bigger than a fixed value in setting */
				if ((previous_height -current_height ) > g_settings->tree_layer_limit)
				{
					/* increment annual tree layer number at cell level */
					c->annual_tree_layer_number += 1;
					//ALESSIOC
					c->t_layers_count += 1;

					previous_height = current_height;
				}
				else
				{
					previous_height = current_height;
				}

			}
		}

		logger(g_log, "ANNUAL TREE LAYERS NUMBER = %d\n", c->annual_tree_layer_number);
		CHECK_CONDITION(c->annual_tree_layer_number, > MAX_N_TREE_LAYER);
		//ALESSIOC
		logger(g_log, "ANNUAL TREE LAYERS NUMBER = %d\n", c->t_layers_count);
		CHECK_CONDITION(c->t_layers_count, > MAX_N_TREE_LAYER);

		//ALESSIOC NEW
		/* assign z value for each height class */
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			switch (c->t_layers_count)
			{
			case 1: /* one layer */
				c->heights[height].z = c->t_layers_count - 1;
				logger(g_log, "height %g, z %d\n", c->heights[height].value, c->heights[height].z);
				break;

			case 2: /* two layers */
				if (height == c->heights_count - 1 )
				{
					c->heights[height].z = c->t_layers_count - 1;
					logger(g_log, "height %g, z %d\n", c->heights[height].value, c->heights[height].z);
				}
				else
				{
					if ((c->heights[height+1].value - c->heights[height].value) > g_settings->tree_layer_limit)
					{
						c->heights[height].z = c->heights[height+1].z - 1;
						logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
					else
					{
						c->heights[height].z = c->heights[height+1].z;
						logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
				}
				break;

			case 3: /* three layers */
				if (height == c->heights_count - 1)
				{
					c->heights[height].z = c->t_layers_count - 1;
					logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
				}
				else
				{
					if ((c->heights[height+1].value - c->heights[height].value) > g_settings->tree_layer_limit)
					{
						c->heights[height].z = c->heights[height+1].z - 1;
						logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
					else
					{
						c->heights[height].z = c->heights[height+1].z;
						logger(g_log, "height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
				}
				break;
			}
		}
	}
	else
	{
		/* for spatial version only one layer */
		logger(g_log, "SPATIAL VERSION ONLY ONE LAYER\n");
		c->t_layers_count = 1;
		logger(g_log, "ANNUAL LAYERS NUMBER = %d\n", c->t_layers_count);
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			c->heights[height].z = 0;
		}
	}
}


void Daily_Forest_structure (cell_t *const c, const int day, const int month, const int year)
{
	int layer = 0;
	int height = 0;
	int age = 0;
	int species = 0;

	double layer_cover;
	int tree_number;

	double potential_maximum_crown_diameter,potential_minimum_crown_diameter;
	double potential_maximum_crown_area, potential_minimum_crown_area;
	double potential_maximum_density,potential_minimum_density;

	tree_layer_t *l;
	height_t *h;
	age_t *a;
	species_t *s;

	l = &c->t_layers[layer];
	h = &c->heights[height];
	a = &c->heights[height].ages[age];
	s = &c->heights[height].ages[age].species[species];

	/* it defines the number of tree height classes in each canopy layer,
	 * the height class level cell coverage through the DBHDC_EFF function,
	 * the density of each layer and then the current tree mortality based on it
	 * on a daily basis
	 */

	logger(g_log, "\n\n***FOREST_STRUCTURE***\n");

	//ALESSIOC NEW
	/**************************************************************************************************/
	/* compute numbers of height classes for each layer */
	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_asc);

		for (height = c->heights_count - 1; height >= 0; height--)
		{
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

			//fixme check if correct
			c->t_layers[c->heights[height].z].height_class = + 1;
		}
	}
	/*************************************************************************************************/
	/* compute numbers of trees for each layer */
	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_asc);

		for (height = c->heights_count - 1; height >= 0; height--)
		{
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

			for (age = h->ages_count - 1; age >= 0; age --)
			{
				for (species = a->species_count - 1; species >= 0; species --)
				{
					//fixme check if correct
					if(c->t_layers[c->heights[height].z].height_class == h->z)
					{
						l->n_trees += s->counter[N_TREE];
					}
				}
			}
		}
	}
	/*************************************************************************************************/
	/* compute density for each layer */
	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_asc);

		//fixme check if correct
		l->density = l->n_trees / g_settings->sizeCell;
	}
	/*************************************************************************************************/
	/* compute potential canopy cover for each class (class level) */
	//todo: control if with a drastic tree number reduction (e.g. management) there's a unrealistic strong variation in DBHDCeffective

	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

		for (age = h->ages_count - 1; age >= 0; age --)
		{
			for (species = a->species_count - 1; species >= 0; species -- )
			{
				logger(g_log,"\n\n**CANOPY HORIZONTAL STRUCTURE BASED ON EFFECTIVE STAND DBH (class level)\n");

				/* computing maximum and minimum potential crown diameter and crown area */
				/* assuming that at the beginning of simulation forest structure is at "equilibrium" */

				if (s->value[DBHDCMAX] != -9999 && s->value[DENMIN] != -9999)
				{
					/* case low density */
					logger(g_log, "\n-Low density\n");

					potential_maximum_crown_diameter = s->value[DBHDCMAX] * s->value[AVDBH];
					potential_maximum_crown_area = pow(((potential_maximum_crown_diameter)/2),2)*Pi;
					logger(g_log, "potential maximum crown area with DBHDCMAX = %g m^2\n", potential_maximum_crown_area);

					potential_minimum_density = g_settings->sizeCell / potential_maximum_crown_area;
					logger(g_log, "number of potential minimum trees with DBHDCMAX = %g\n", potential_minimum_density);

					/* case high density */
					logger(g_log, "\n-High density\n");

					potential_minimum_crown_diameter = s->value[DBHDCMIN] * s->value[AVDBH];
					potential_minimum_crown_area = pow(((potential_minimum_crown_diameter)/2),2)*Pi;
					logger(g_log, "potential minimum crown area with DBHDCMIN = %g m^2\n", potential_minimum_crown_area);

					potential_maximum_density = g_settings->sizeCell /potential_minimum_crown_area;
					logger(g_log, "number of potential maximum trees with DBHDCMIN = %g\n", potential_maximum_density);

					s->value[DENMAX] = potential_maximum_density/g_settings->sizeCell;
					logger(g_log, "potential density with dbhdcmax (high density) = %g (%g tree)\n", s->value[DENMAX], s->value[DENMAX] * g_settings->sizeCell);

					s->value[DENMIN] = potential_minimum_density/g_settings->sizeCell;
					logger(g_log, "potential density with dbhdcmin (low density) = %g (%g tree)\n", s->value[DENMIN], s->value[DENMIN] * g_settings->sizeCell);
				}
				/* in case no values from parameterization files are given */
				else
				{
					/*compute maximum crown area*/
					//	//TODO CHECK IF USE IT
					/*for references and variables see "Forest Mensuration" book 4th edition
					  B. Husch, T.W. Beers, J.A. Kershaw Jr.
					  edited by John Wiley & Sons, Inc
					  and Krajicek, et al., "Crown competition: a measure of density.
					  For. Sci. 7:36-42
					  Lhotka and Loewenstein 2008, Can J For Res
					*/
					potential_minimum_crown_area = ((100.0*Pi)/(4*g_settings->sizeCell)) * (9.7344 + (11.48612 * s->value[AVDBH] + (3.345241 *	pow(s->value[AVDBH], 2.0))));
					logger(g_log, "-MCA (Maximum Crown Area) = %g m^2\n", potential_minimum_crown_area);

					potential_minimum_crown_diameter = 2.0 * sqrt(potential_minimum_crown_area/Pi);
					logger(g_log, "-MCD (Maximum Crown Diameter) = %g m\n", potential_minimum_crown_diameter);

					/*recompute DBHDCmax and DENmin from MCA*/
					/*17 Oct 2013*/
					s->value[DBHDCMAX] = potential_minimum_crown_diameter / s->value[AVDBH];
					logger(g_log, "-recomputed DBHDCMAX = %g \n", s->value[DBHDCMAX]);

					s->value[DENMIN] = 1.0 / potential_minimum_crown_area;
					logger(g_log, "-recomputed DENMIN = %g tree/sizecell\n", s->value[DENMIN]);
				}

				logger(g_log, "DBHDCMAX = %g\n", s->value[DBHDCMAX]);
				logger(g_log, "DBHDCMIN = %g\n", s->value[DBHDCMIN]);
				logger(g_log, "DENMAX = %g\n", s->value[DENMAX]);
				logger(g_log, "DENMIN = %g\n", s->value[DENMIN]);
			}
		}
	}

	/*************************************************************************************************/
	/* check if layer density exceeds potential maximum and minimum density */
	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_asc);

		for (height = c->heights_count - 1; height >= 0; height--)
		{
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

			for (age = h->ages_count - 1; age >= 0; age --)
			{
				for (species = a->species_count - 1; species >= 0; species --)
				{
					//fixme check it, it considers denmax and denmin using species level values..
					if(l->density > s->value[DENMAX]) l->density = s->value[DENMAX];
					if(l->density < s->value[DENMIN]) l->density = s->value[DENMIN];
				}
			}
		}
	}

	/*************************************************************************************************/
	/* compute effective dbh/crown diameter ratio for each class based on layer density (class level) */

	for (height = c->heights_count - 1; height >= 0; height--)
	{
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

		for (age = h->ages_count - 1; age >= 0; age --)
		{
			for (species = a->species_count - 1; species >= 0; species --)
			{
				s->value[DBHDC_EFF] = ((s->value[DBHDCMIN] - s->value[DBHDCMAX]) / (s->value[DENMAX] - s->value[DENMIN]) *
						(l->density - s->value[DENMIN]) + s->value[DBHDCMAX]);

				/* check if DBHDCeffective exceeds maximum or minimum values */
				if (s->value[DBHDC_EFF] > s->value[DBHDCMAX])
				{
					logger(g_log, "DBHDC effective > DBHDCMAX!!!\n");
					s->value[DBHDC_EFF] = s->value[DBHDCMAX];
					logger(g_log, "DBHDC effective applied is DBHDCMAX = %g\n", s->value[DBHDC_EFF]);
				}
				if (s->value[DBHDC_EFF] < s->value[DBHDCMIN])
				{
					logger(g_log, "DBHDC effective < DBHDCMIN!!!\n");
					s->value[DBHDC_EFF] = s->value[DBHDCMIN];
					logger(g_log, "DBHDC effective applied is DBHDCMIN = %g\n", s->value[DBHDC_EFF]);
				}
			}
		}
	}

	//fixme USEFULL???
	//assuming no reduction in DBHDCeff to prevent reduction in DBHDCeff
	//test 16 MAY 2016 test check removing block that assumes no reduction in DBHDCeff
	//Answer: it causes an high increment in LAI values!!

	//	if(day == 0 && month == JANUARY && year == 0)
	//	{
	//		s->value[PREVIOUS_DBHDC_EFF] = s->value[DBHDC_EFF];
	//	}
	//	//test 13 May 2016 test why it considers just first day, month and year!!!!!!!
	//	//in this way seems that dbhdceff can just increase
	//	else
	//	{
	//		//test 13 MAY 2016 test check why model seems to use wrong DBHDCeff values
	//		logger(g_log, "DBHDC effective applied = %g\n", s->value[PREVIOUS_DBHDC_EFF]);
	//		logger(g_log, "DBHDC effective applied = %g\n", s->value[DBHDC_EFF]);
	//
	//		if (s->value[PREVIOUS_DBHDC_EFF] > s->value[DBHDC_EFF])
	//		{
	//			logger(g_log, "previous = %g\n eff = %g\n", s->value[PREVIOUS_DBHDC_EFF], s->value[DBHDC_EFF]);
	//			s->value[DBHDC_EFF] = s->value[PREVIOUS_DBHDC_EFF];
	//			logger(g_log, "previous = %g\n eff = %g\n", s->value[PREVIOUS_DBHDC_EFF], s->value[DBHDC_EFF]);
	//		}
	//		else
	//		{
	//			logger(g_log, "previous = %g\n eff = %g\n", s->value[PREVIOUS_DBHDC_EFF], s->value[DBHDC_EFF]);
	//			s->value[PREVIOUS_DBHDC_EFF] = s->value[DBHDC_EFF];
	//			logger(g_log, "previous = %g\n eff = %g\n", s->value[PREVIOUS_DBHDC_EFF], s->value[DBHDC_EFF]);
	//		}
	//	}

	//	//test 13 MAY 2016 test check why model seems to use wrong DBHDCeff values
	//	logger(g_log, "DBHDC effective applied = %g\n", s->value[PREVIOUS_DBHDC_EFF]);
	//	logger(g_log, "DBHDC effective applied = %g\n", s->value[DBHDC_EFF]);

	/*************************************************************************************************/
	/* compute effective crown diameter and crown area and class cover using DBH-DC */
	s->value[CROWN_DIAMETER_DBHDC_FUNC] = s->value[AVDBH] * s->value[DBHDC_EFF];
	logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);

	/* Crown Area using DBH-DC */
	s->value[CROWN_AREA_DBHDC_FUNC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC_FUNC], 2 );
	logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);

	/* Canopy Cover using DBH-DC */
	s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE] / g_settings->sizeCell;
	logger(g_log, "Canopy cover DBH-DC class related = %g\n", s->value[CANOPY_COVER_DBHDC]);


	/*************************************************************************************************/
	/* compute layer canopy cover for each layer (layer level) */
	//todo: control if with a drastic tree number reduction (e.g. management) there's a unrealistic strong variation in DBHDCeffective

	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_asc);

		for (height = c->heights_count - 1; height >= 0; height--)
		{
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

			for (age = h->ages_count - 1; age >= 0; age --)
			{
				for (species = a->species_count - 1; species >= 0; species --)
				{
					//fixme check if correct
					if(c->t_layers[h->z].height_class == h->z)
					{
						l->layer_cover += s->value[CANOPY_COVER_DBHDC];
					}
				}
			}
		}
	}

	/*************************************************************************************************/
	/* compute layer tree mortality due to canopy cover for each layer (layer level) */

	//test 13 MAY 2016 test
	//try to reduce step by step DBHDC up to minimum DBHDC value before call "Crowding_competition_mortality" function as a sort of self-thinning

	//the model makes die trees of the lower layer and height class for that layer because
	//it passes through the function sort_by_layer/height_desc the layer/height classes starting from the lowest

	qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_desc);

	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			for (age = h->ages_count - 1; age >= 0; age --)
			{
				for (species = a->species_count - 1; species >= 0; species -- )
				{
					if (l->layer_cover >= g_settings->max_layer_cover)
					{
						/* mortality */
						layer_cover = l->layer_cover;
						tree_number = l->n_trees;
						Crowding_competition_mortality (c, layer, height, age, species, layer_cover, tree_number);
						logger(g_log, "Recomputed Layer cover in layer 0 = %g %% \n", l->layer_cover * 100);
					}
				}
			}
		}
	}

	/**************************************************************************************************/
	/* REcompute numbers of height classes, tree number and density after mortality for each layer */

	for (layer = c->t_layers_count - 1; layer >= 0; layer --)
	{
		qsort (c->t_layers, c->t_layers_count, sizeof (tree_layer_t), sort_by_layers_asc);

		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

			for (age = c->heights[height].ages_count - 1; age >= 0; age --)
			{
				for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
				{
					/* number of height class for each layer */
					//fixme check if correct and necessary..
					//c->t_layers[c->heights[height].z].height_class = + 1;

					/* compute tree number for each layer */
					//fixme check if correct
					if(c->t_layers[c->heights[height].z].height_class == h->z)
					{
						l->n_trees += s->counter[N_TREE];
					}

					/* compute density for each layer */
					//fixme check if correct
					l->density = l->n_trees / g_settings->sizeCell;

					/* compute dbh/crown diameter ratio */
					s->value[DBHDC_EFF] = (( s->value[DBHDCMAX] - s->value[DBHDCMIN])/ (s->value[DENMAX] - s->value[DENMIN])* (l->density - s->value[DENMIN] ) + s->value[DBHDCMIN]);
					logger(g_log, "DBHDC effective to apply = %g\n", s->value[DBHDC_EFF]);


					/* check if layer density exceeds potential maximum and minimum density */
					if (s->value[DBHDC_EFF] > s->value[DBHDCMAX])
					{
						s->value[DBHDC_EFF] = s->value[DBHDCMAX];
						logger(g_log, "DBHDC effective to apply = %g\n", s->value[DBHDC_EFF]);
					}
					else if (s->value[DBHDC_EFF] < s->value[DBHDCMIN])
					{
						s->value[DBHDC_EFF] = s->value[DBHDCMIN];
						logger(g_log, "DBHDC effective to apply = %g\n", s->value[DBHDC_EFF]);
					}

					/* REcompute crown diameter using DBH-DC */
					s->value[CROWN_DIAMETER_DBHDC_FUNC] = s->value[AVDBH] * s->value[DBHDC_EFF];
					logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);

					/* REcompute crown are using DBH-DC */
					s->value[CROWN_AREA_DBHDC_FUNC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC_FUNC], 2 );
					logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);

					/* REcompute canopy cover using DBH-DC */
					s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE] / g_settings->sizeCell;
					logger(g_log, "-Canopy Cover from DBHDC function = %g \n", s->value[CANOPY_COVER_DBHDC]);

					/* REcompute layer cover using DBH-DC */
					l->layer_cover += s->value[CANOPY_COVER_DBHDC];
					logger(g_log, "-Layer Cover from DBHDC function = %g \n", l->layer_cover);
					logger(g_log, "*************************************************** \n");
				}
			}
		}
	}
}

void Daily_check_for_veg_period (cell_t *const c, const meteo_daily_t *const meteo_daily, const int day, const int month)
{
	static int height;
	static int age;
	static int species;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	/* it computes the vegetative state for each species class,
	 * the number of days of leaf fall and
	 * the rate for leaves reduction (for deciduous species) */

	/*VEG_UNVEG = 1 for veg period, = 0 for Un-Veg period*/

	c->Veg_Counter = 0;

	logger(g_log, "\n\n\n****GET_DAILY_FOREST_STRUCTURE_ROUTINE for cell (%d, %d)****\n", c->x, c->y);

	//assign value for VEG_UNVEG (1 for veg, 0 for Unveg) and compute number of classes in veg period

	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{

				if (day == 0 && month == 0)
				{
					s->counter[LEAF_FALL_COUNTER] = 0;
				}

				/*PHENOLOGY = 0 FOR DECIDUOUS*/
				if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
				{
					logger(g_log, "-GET ANNUAL VEGETATIVE DAYS for species %s -\n", s->name);
					if (g_settings->spatial == 's')
					{
						//logger(g_log, "Spatial version \n");

						//veg period
						if (meteo_daily->ndvi_lai > 0.1)
						{
							s->counter[VEG_UNVEG] = 1;
							c->Veg_Counter += 1;
							logger(g_log, "%s is in veg period\n", s->name);
						}
						//unveg period
						else
						{
							s->counter[VEG_UNVEG] = 0;
							logger(g_log, "%s is in un-veg period\n", s->name);
						}
					}
					else
					{

						/*compute annual days of leaf fall*/
						s->counter[DAY_FRAC_FOLIAGE_REMOVE] = (int)(s->value[LEAF_FALL_FRAC_GROWING] * s->counter[DAY_VEG_FOR_LEAF_FALL]);
						logger(g_log, "Days of leaf fall for deciduous = %d day\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
						//monthly rate of foliage reduction

						//currently the model considers a linear reduction in leaf fall
						//it should be a negative sigmoid function
						//todo: create a sigmoid function
						s->value[FOLIAGE_REDUCTION_RATE] = 1.0 / (s->counter[DAY_FRAC_FOLIAGE_REMOVE] + 1);
						logger(g_log, "foliage reduction rate = %g,  = %g%%\n", s->value[FOLIAGE_REDUCTION_RATE], s->value[FOLIAGE_REDUCTION_RATE] * 100);


						//todo decidere se utlizzare growthend o mindaylenght
						//lo stesso approccio deve essere usato anche in Get_Veg_Days func
						//currently model can simulate only forests in boreal hemisphere
						if ((meteo_daily->thermic_sum >= s->value[GROWTHSTART] && month <= 6)
								|| (meteo_daily->daylength >= s->value[MINDAYLENGTH] && month >= 6 && c->north == 0))
						{
							s->counter[VEG_UNVEG] = 1;
							c->Veg_Counter += 1;
							//FIXME check if it works (daily)
							/* we consider that plants tend to occupy the part of the cell not covered by the others */
							c->cell_cover += s->value[CANOPY_COVER_DBHDC];
							if(c->cell_cover > 1.0)
							{
								c->cell_cover = 1.0;
							}
							logger(g_log, "%s is in veg period\n", s->name);
							logger(g_log, "cell_cover counter = %g\n", c->cell_cover);
						}
						else
						{
							//check for case 0 of allocation
							if (meteo_daily->daylength <= s->value[MINDAYLENGTH] && month >= 6 && c->north == 0 )
							{

								logger(g_log, "DAY_FRAC_FOLIAGE_REMOVE %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

								s->counter[LEAF_FALL_COUNTER] += 1;
								//check
								if(s->counter[LEAF_FALL_COUNTER] == 1)
								{
									//assign value of thermic sum
									s->value[THERMIC_SUM_FOR_END_VEG] = meteo_daily->thermic_sum;
									//logger(g_log, "thermic_sum END OF VEG = %g °C\n", s->value[THERMIC_SUM_FOR_END_VEG]);
								}

								//check
								if(s->counter[LEAF_FALL_COUNTER]  <= (int)s->counter[DAY_FRAC_FOLIAGE_REMOVE])
								{
									/*days of leaf fall*/
									s->counter[VEG_UNVEG] = 1;
									c->Veg_Counter += 1;
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
				}
				/*PHENOLOGY = 1 FOR EVERGREEN*/
				else
				{
					s->counter[VEG_UNVEG] = 1;
					logger(g_log, "Veg period = %d \n", s->counter[VEG_UNVEG]);
					c->Veg_Counter += 1;
					logger(g_log, "%s is in veg period\n", s->name);
				}
			}
		}
	}
	logger(g_log, "classes in veg period = %d\n", c->Veg_Counter);
}

void Daily_numbers_of_layers (cell_t *const c)
{
	static int height;
	static int age;
	static int species;
	static double current_height;
	static double previous_height;

	/* determines number of vegetative layer in function of:
	 *-differences between tree height classes
	 *-vegetative or un-vegetative period */


	//ALESSIOC
	//c->daily_layer_number = 0;

	logger(g_log, "\n--GET NUMBER OF DAILY LAYERS (Layer in Veg)--\n");


	qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc);

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				current_height = c->heights[height].value;

				if (c->heights_count > 1 )
				{
					if (height == c->heights_count -1 && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
					{
						//ALESSIOC
						//c->daily_layer_number += 1;
						previous_height = current_height;
					}
					if ((previous_height - current_height ) > g_settings->tree_layer_limit && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
					{
						//ALESSIOC
						//c->daily_layer_number += 1;
						previous_height = current_height;
					}
				}
				if (c->heights_count == 1  && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
				{
					//ALESSIOC
					//c->daily_layer_number = 1;
				}
			}
		}
	}
	//ALESSIOC
	//logger(g_log, "number of vegetative layers = %d\n", c->daily_layer_number);
}

void Daily_layer_cover(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	static int height;
	static int age;
	static int species;

	/* it daily computes the layer coverage based on the vegetative status of each species class*/

	//ALESSIOC
	//c->layer_cover_dominant = 0;
	//c->layer_cover_dominated = 0;
	//c->layer_cover_subdominated = 0;


	logger(g_log, "\nDAILY_FOREST_STRUCTURE_ROUTINE\n");

	logger(g_log, "Determine Effective Layer Cover \n");

	for (height = c->heights_count - 1; height >= 0; height -- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				if ( c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
				{
					/*
					if (c->daily_layer_number == 3)
					{
						switch (c->heights[height].z)
						{
						case 2:
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							break;
						case 1:
							c->layer_cover_dominated += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							break;
						case 0:
							c->layer_cover_subdominated  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							break;
						}
					}
					if (c->daily_layer_number == 2)
					{
						switch (c->heights[height].z)
						{
						case 1:
							//logger(g_log, "z = %d\n", c->heights[height].z);
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//logger(g_log, "Layer cover in layer 1 = %g %% \n", c->layer_cover_dominant * 100);
							break;
						case 0:
							//logger(g_log, "z = %d\n", c->heights[height].z);
							c->layer_cover_dominated  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//logger(g_log, "Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);
							break;
						}
					}
					else
					{
						//logger(g_log, "z = %d\n", c->heights[height].z);
						c->layer_cover_dominant  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						//logger(g_log, "Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);
					}
					 */
				}
				else
				{
					//logger(g_log, "Un-Vegetating layers\n");
					//logger(g_log, "z = %d\n", c->heights[height].z);
				}
			}
		}
	}

	//ALESSIOC
	/*
	logger(g_log, "Daily layer number = %d\n", c->daily_layer_number);

	if (c->daily_layer_number == 1)
	{
		logger(g_log, "Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);
	}
	if (c->daily_layer_number == 2)
	{
		logger(g_log, "Vegetated Layer cover in layer 1 = %g %%\n", c->layer_cover_dominant * 100);
		logger(g_log, "Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);
	}
	if (c->daily_layer_number > 2)
	{
		logger(g_log, "Vegetated Layer cover in layer 2 = %g %%\n", c->layer_cover_dominant * 100);
		logger(g_log, "Vegetated Layer cover in layer 1 = %g %% \n", c->layer_cover_dominated * 100);
		logger(g_log, "Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_subdominated * 100);
	}
	logger(g_log, "*************************************************** \n");
	 */

}

/*
void Get_top_layer (cell_t *const c, int heights_count, HEIGHT *heights)
{

	logger(g_log, " GET_TOP_LAYER_FUNCTION \n");
	int height;

	assert (heights);



	for ( height = heights_count - 1; height >= 0; height-- )
	{
		logger(g_log, "height dominance = %d \n", c->heights[height].dominance);
		if (c->heights[height].dominance == 1)
		{
			c->heights[height].top_layer = c->heights[height].z ;
			logger(g_log, "-Top layer and in Dominant Light is layer with z = %d\n", c->heights[height].top_layer);
			break;
		}
		if ( c->heights[height].top_layer == -1)
		{
			logger(g_log, "--NO TREES IN VEGETATIVE PERIOD!!!\n");
			logger(g_log, "**********************************************\n");
			break;
		}
	}
}
 */


void Daily_dominant_Light(cell_t *const c, int layer, int height, int age, int species)
{
	species_t *s;

	assert(height);

	s = &c->heights[height].ages[age].species[species];

	/* it computes which canopy layers is in dominant position for light */
	//ALESSIOC CHECK IT
	if (c->t_layers[layer].daily_n_layer != 0)
	{
		logger(g_log, "-Dominant Light Index Function-\n");

		//highest z value in veg period determines top_layer value
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			/* sort by ascending heights */
			qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

			for ( age = 0; age < c->heights[height].ages_count; age++ )
			{
				for ( species = 0; species < c->heights[height].ages[age].species_count; species++ )
				{
					if (s->counter[VEG_UNVEG]==1)
					{
						//ALESSIOC CHECK IT
						if (g_settings->spatial == 'u')
						{
							c->top_layer = c->heights[height].z;
						}
						else
						{
							c->top_layer = 0;
						}
					}
				}
			}
		}
		logger(g_log, "Daily/Monthly Dominant layer is z = %d\n", c->top_layer);
	}
}

/*
int Number_of_layers (cell_t *c)
{
	// ALESSIOR number_of_layers can be uninitialized
	int number_of_layers = 0;
	//compute number of layers
	if (c->dominant_veg_counter > 0 && c->dominated_veg_counter > 0 && c->subdominated_veg_counter > 0)
	{
		number_of_layers = 3;
		logger(g_log, "THREE LAYERS \n");
	}
	else if (c->dominant_veg_counter > 0 && c->dominated_veg_counter > 0 && c->subdominated_veg_counter == 0)
	{
		number_of_layers = 2;
		logger(g_log, "TWO LAYERS \n");
	}
	else if (c->dominant_veg_counter == 0 && c->dominated_veg_counter > 0 && c->subdominated_veg_counter == 0)
	{
		number_of_layers = 1;
		logger(g_log, "ONE LAYER \n");
	}
	return number_of_layers;

}
 */

void Daily_veg_counter(cell_t *const c, species_t *const s, const int height)
{
	//ALESSIOC
	/*
	switch (c->daily_layer_number)
	{
	case 3:
		if (c->heights[height].z == 2)
		{
			c->dominant_veg_counter += 1;
		}
		if (c->heights[height].z == 1)
		{
			c->dominated_veg_counter += 1;
		}
		else
		{
			c->subdominated_veg_counter += 1;
		}
		break;
	case 2:
		if (c->heights[height].z == 1)
		{
			c->dominant_veg_counter += 1;
		}
		else
		{
			c->dominated_veg_counter += 1;
		}
		break;
	case 1:
		c->dominant_veg_counter += 1;
		break;
	}
	 */
}






