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

extern settings_t* g_settings;
extern logger_t* g_log;

int self_pruning ( cell_t *const c, const int layer )
{
	int height;
	int age;
	int species;

	double perc;
	double old_crown_area;

	tree_layer_t *l;
	height_t *h;
	age_t *a;
	species_t *s;

	l = &c->t_layers[layer];

	/* SELF-PRUNING function */
	/*it reduces dbhdc values for lower height class up to dbhdcmin otherwise
	 * mortality for self-thinning*/

	logger(g_log, "\n\n*****SELF-PRUNING FUNCTION for layer %d*****\n", layer);

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		h = &c->heights[height];

		/* first of all sort by descending height */
		/* model makes prune before lower height in each later */
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

		for ( age = 0; age < h->ages_count ; ++age )
		{
			a = &c->heights[height].ages[age];

			for ( species = 0; species < a->species_count; ++species )
			{
				s = &c->heights[height].ages[age].species[species];

				logger(g_log, "layer %d; layer cover %g, height %g\n", layer, l->layer_cover * 100, h->value);
				logger(g_log,"DBHDC effective before pruning = %g\n", s->value[DBHDC_EFF]);

				/* compute possible crown area for above canopy cover dbhdc */
				old_crown_area = s->value[CROWN_AREA_DBHDC];
				s->value[CROWN_AREA_DBHDC] = (s->value[CANOPY_COVER_DBHDC] * 100.0) / s->counter[N_TREE];

				/* until layer cover not reaches lower values than max_layer_cover or DBDCMIN */
				while( l->layer_cover > g_settings->max_layer_cover )
				{
					/* reduce DBHDC_EFF */
					/* note: 0.00005 is an arbitrary value */
					s->value[DBHDC_EFF] -= 0.00005;
					logger(g_log,"DBHDC effective during while = %g\n", s->value[DBHDC_EFF]);

					/* recompute crown diameter */
					s->value[CROWN_DIAMETER_DBHDC] = s->value[AVDBH] * s->value[DBHDC_EFF];
					logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC]);

					/* Crown Area using DBH-DC */
					s->value[CROWN_AREA_DBHDC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC], 2 );
					logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC]);

					/* Canopy Cover using DBH-DC */
					s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC] * s->counter[N_TREE] / g_settings->sizeCell;
					logger(g_log, "-Canopy cover DBH-DC class level = %g %%\n", s->value[CANOPY_COVER_DBHDC] * 100.0);

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
					logger(g_log, "-layer cover cell level = %g \n", l->layer_cover);

					/* reduce proportionally to the crown area reduction the amount of branch C pool */
					/* compute percentage in crown area reduction for self-pruning */
					perc = (s->value[CROWN_AREA_DBHDC] / old_crown_area) * 100.0;

					/* update branch C pool */
					s->value[BRANCH_C] *= perc;

					/* update branch N pool */
					s->value[BRANCH_N] *= perc;

					/* self-pruned branch to litter */
					s->value[C_TO_LITTER] += s->value[BRANCH_C] * (1 - perc);

					//test
					//update also leaf_C and fine_root_C???

				}

				logger(g_log, "-after pruning-\n");
				logger(g_log, "-DBHDC effective = %g\n", s->value[DBHDC_EFF]);
				logger(g_log, "-DBHDC MINIMUM = %g\n", s->value[DBHDCMIN]);
				logger(g_log, "-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC]);
				logger(g_log, "-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC]);
				logger(g_log, "-Canopy cover DBH-DC class level = %g %%\n", s->value[CANOPY_COVER_DBHDC] * 100.0);
				logger(g_log, "-layer cover cell level = %g\n", l->layer_cover);

				/* check if reduction in dbhdc satisfy max layer cover */
				if ( l->layer_cover < g_settings->max_layer_cover && s->value[DBHDC_EFF] > s->value[DBHDCMIN] )
				{
					//todo to be tested
					/* self-pruning was enough */
					logger(g_log, "self-pruning was enough");
					return 1;
				}
				else
				{
					//todo to be tested
					/* self-pruning was not enough */
					logger(g_log, "self-pruning was not enough");
					return 0;
				}
			}
		}
	}
}

void self_thinning(cell_t *const c, const int layer)
{
	int height;
	int age;
	int species;

	int deadtree = 0;
	//int oldNstump;
	//int deadstump = 0;

	height_t *h;
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

	logger(g_log, "\n\n*****SELF THINNING MORTALITY for layer %d*****\n", layer);

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		/* first of all sort by descending height */
		/* model makes die before lower height in each later */
		qsort (c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

		for ( age = 0; age < c->heights[height].ages_count ; ++age )
		{
			for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
			{
				h = &c->heights[height];
				a = &c->heights[height].ages[age];
				s = &c->heights[height].ages[age].species[species];

				logger(g_log, "MORTALITY BASED ON HIGH CANOPY COVER height %g species %s dbh %g !!!\n", h->value, s->name, s->value[AVDBH]);

				if (s->management == T)
				{
					/* compute average biomass */
					s->value[AV_LEAF_MASS_KgC] = (s->value[LEAF_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_STEM_MASS_KgC] = (s->value[STEM_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_ROOT_MASS_KgC] = (s->value[TOT_ROOT_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_FINE_ROOT_MASS_KgC] = (s->value[FINE_ROOT_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_COARSE_ROOT_MASS_KgC] =(s->value[COARSE_ROOT_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_RESERVE_MASS_KgC] = (s->value[RESERVE_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_BRANCH_MASS_KgC] = (s->value[BRANCH_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_LIVE_STEM_MASS_KgC] = (s->value[STEM_LIVE_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_DEAD_STEM_MASS_KgC] = (s->value[STEM_DEAD_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_LIVE_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_DEAD_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_LIVE_BRANCH_MASS_KgC] = (s->value[BRANCH_LIVE_WOOD_C] / (double)s->counter[N_TREE])/1000.0;
					s->value[AV_DEAD_BRANCH_MASS_KgC] = (s->value[BRANCH_DEAD_WOOD_C] / (double)s->counter[N_TREE])/1000.0;

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
						//CHECK_CONDITION(s->counter[N_TREE], > 0);

						if (s->counter[N_TREE] == 0)
						{
							/* mortality for the higher height class */
							while (c->t_layers[layer].layer_cover > g_settings->max_layer_cover && c->heights[height + 1].ages[age].species[species].counter[N_TREE] > 0)
							{
								--s->counter[N_TREE];
								++deadtree;

								c->heights[height + 1].ages[age].species[species].value[CANOPY_COVER_DBHDC] -=
										c->heights[height + 1].ages[age].species[species].value[CROWN_AREA_DBHDC] / g_settings->sizeCell;

								/* recompute layer level canopy cover */
								c->t_layers[layer].layer_cover -=
										c->heights[height + 1].ages[age].species[species].value[CROWN_AREA_DBHDC] / g_settings->sizeCell;

								/* check */
								CHECK_CONDITION(s->counter[N_TREE], <= 0);
							}
						}
//						logger(g_log, "Tree Removed %d Tree remaining %d from height class %g species %s dbh %g\n", deadtree, s->counter[N_TREE], h->value, s->name, s->value[AVDBH]);
//						logger(g_log, "layer %d layer cover = %g %%\n", layer, c->t_layers[layer].layer_cover);

						/* update class biomass */
						s->value[LEAF_C] -= (s->value[AV_LEAF_MASS_KgC]*1000.0*deadtree);
						s->value[FINE_ROOT_C] -= (s->value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree);
						s->value[COARSE_ROOT_C] -= (s->value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
						s->value[STEM_C] -= (s->value[AV_STEM_MASS_KgC]*1000.0*deadtree);
						s->value[RESERVE_C] -= (s->value[AV_RESERVE_MASS_KgC]*1000.0*deadtree);
						s->value[BRANCH_C] -= (s->value[AV_BRANCH_MASS_KgC]*1000.0*deadtree);
						s->value[STEM_LIVE_WOOD_C] -= (s->value[AV_LIVE_STEM_MASS_KgC]*1000.0*deadtree);
						s->value[STEM_DEAD_WOOD_C] -= (s->value[AV_DEAD_STEM_MASS_KgC]*1000.0*deadtree);
						s->value[COARSE_ROOT_LIVE_WOOD_C] -= (s->value[AV_LIVE_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
						s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[AV_DEAD_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
						s->value[BRANCH_LIVE_WOOD_C] -= (s->value[AV_LIVE_BRANCH_MASS_KgC]*1000.0*deadtree);
						s->value[BRANCH_DEAD_WOOD_C] -= (s->value[AV_DEAD_BRANCH_MASS_KgC]*1000.0*deadtree);

						s->value[LITTER_C] += (s->value[AV_LEAF_MASS_KgC]*1000.0*deadtree) +
								(s->value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree) +
								(s->value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree) +
								(s->value[AV_STEM_MASS_KgC]*1000.0*deadtree) +
								(s->value[AV_RESERVE_MASS_KgC]*1000.0*deadtree) +
								(s->value[AV_BRANCH_MASS_KgC]*1000.0*deadtree);

						//ALESSIOR how remove one class??
						//if(s->counter[N_TREE] == 0)
						//{
						//	//delete classes due to mortality
						//	logger(g_log, "All trees of species %s dbh %g height %g age %d are died!!!!\n",
						//			s->name, s->value[AVDBH], c->heights[height].value, c->heights[height].ages[age].value);

						//	logger(g_log, "reducing counter for died classes...........!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n");
						//	logger(g_log, "heights_count = %d, ages_count = %d, species_count = %d \n", c->heights_count, c->heights[height].ages_count, c->heights[height].ages[age].species_count);
						//	//free (s->name);

						//	//todo it!!!
						//	//fixme in this case to BE CANCELLED is the last PROCESSED class BUT NOT NECESSARILY THE CLASS REALLY DIED
						//	if (c->heights[height].ages[age].species_count > 1)
						//		logger(g_log, "Reducing species_count..\n");
						//		c->heights[height].ages[age].species_count --;
						//	//free (c->heights[height].ages[age].species);
						//	if(c->heights[height].ages_count > 1)
						//		logger(g_log, "Reducing ages_count..\n");
						//		c->heights[height].ages_count --;
						//	//free (c->heights[height].ages);
						//	if (c->heights_count > 1)
						//		logger(g_log, "Reducing heights_count..\n");
						//		c->heights_count --;
						//}

						/* update at cell level */
						c->cell_n_trees -= deadtree;
						c->daily_dead_tree += deadtree;
						c->monthly_dead_tree += deadtree;
						c->annual_dead_tree += deadtree;
					}
				}
				else
				{
//					//fixme to complete
//					while (c->t_layers[layer].layer_cover >= g_settings->max_layer_cover )
//					{
//						s->counter[N_STUMP] -= 1;
//						deadstump += 1;
//						//layer_cover = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_STUMP] / g_settings->sizeCell;
//						s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / g_settings->sizeCell;
//					}
//					oldNstump -= s->counter[N_STUMP];
				}
			}
		}

		logger(g_log, "LEAF_C removed =%g tC\n",(s->value[AV_LEAF_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "FINE_ROOT_C removed =%g tC\n",(s->value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "COARSE_ROOT_C removed =%g tC\n",(s->value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "STEM_C removed =%g tC\n",(s->value[AV_STEM_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "RESERVE_C removed =%g tC\n",(s->value[AV_RESERVE_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "BRANCH_C removed =%g tC\n",(s->value[AV_BRANCH_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "STEM_LIVE_WOOD_C removed =%g tC\n",(s->value[AV_LIVE_STEM_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "STEM_DEAD_WOOD_C removed =%g tC\n",(s->value[AV_DEAD_STEM_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "COARSE_ROOT_LIVE_WOOD_C removed =%g tC\n",(s->value[AV_LIVE_COARSE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "COARSE_ROOT_DEAD_WOOD_C removed =%g tC\n",(s->value[AV_DEAD_COARSE_ROOT_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "BRANCH_LIVE_WOOD_C removed =%g tC\n",(s->value[AV_LIVE_BRANCH_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "BRANCH_DEAD_WOOD_C removed =%g tC\n",(s->value[AV_DEAD_BRANCH_MASS_KgC]*1000.0*deadtree));
		logger(g_log, "Tree remaining for height class %g age %d species %s = %d\n", h->value, a->value, s->name, s->counter[N_TREE]);
	}

	/* reset dead tree */
	deadtree = 0;

}

/*Self-thinnig mortality function from 3PG*/
//----------------------------------------------------------------------------//
//                                                                            //
//                             GetMortality                                   //
//This function determines the number of stems to remove to ensure the
//self-thinning rule is satisfied. It applies the Newton-Rhapson method
//to solve for N to an accuracy of 1 stem or less. To change this,
//change the value of "accuracy".
//This was the old mortality function:
//getMortality = oldN - 1000 * (wSx1000 * oldN / oldW / 1000) ^ (1 / thinPower)
//which has been superceded by the following ..
//                                                                            //
//----------------------------------------------------------------------------//

void self_thinning_mortality_3PG (species_t *const s, int years)
// TreeNumber = m->lpCell[index].InitialNTree, Ws = Ws
// TreeNumber = m->lpCell[index].NTree, Ws = Ws
{

	int i;
	double fN, dfN;
	double dN, n, x1, x2;


	//deselected algorithm for 1Km^2 spatial resolution
	/*m->cells[cell].heights[height].ages[age].species[species].value[WS_MAX] = m->cells[cell].heights[height].ages[age].species[species].value[WSX1000] *
        pow((1000 / (double)m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]),
                m->cells[cell].heights[height].ages[age].species[species].value[THINPOWER]);
	 */

	//modifified version for 1Km^2 spatial resolution
	s->value[WS_MAX] = s->value[WSX1000];



	if ( s->value[AV_STEM_MASS_KgDM] > s->value[WS_MAX])
	{

		logger(g_log, "MORTALITY based SELF-THINNING RULE\n");
		logger(g_log, "Average Stem Mass > WSMax\n");
		logger(g_log, "WS MAX = %g kgDM/tree\n",  s->value[WS_MAX]);
		logger(g_log, "Average Stem Mass = %g kgDM stem /tree\n", s->value[AV_STEM_MASS_KgDM]);




		logger(g_log, "Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);
		logger(g_log, "Tree Stem Mass before Mortality Function = %g\n", s->value[BIOMASS_STEM_tDM]);

		n = (double)s->counter[N_TREE] / 1000;
		logger(g_log, "n = %g\n", n);
		x1 = 1000 * s->value[MS] * s->value[BIOMASS_STEM_tDM] / (double)s->counter[N_TREE];
		logger(g_log, "x1 = %g\n", x1);
		i = 0;
		while ( 1 )
		{
			i = i + 1;
			logger(g_log, "i = %d\n", i);
			x2 = s->value[WSX1000] * pow (n, (1 - s->value[THINPOWER]));
			logger(g_log, "X2 = %g\n", x2);
			fN = x2 - x1 * n - (1 - s->value[MS]) * s->value[BIOMASS_STEM_tDM];
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


/*Age mortality function from LPJ*/
void Age_Mortality (age_t *const a, species_t *const s)
{

	static int Dead_trees;

	/* Age probability function */
	s->value[AGEMORT] = (-(3 * log (0.001)) / (s->value[MAXAGE])) * pow (((double)a->value /s->value[MAXAGE]), 2);


	if ((s->counter[N_TREE] * s->value[AGEMORT]) > 1)
	{
		logger(g_log, "**MORTALITY based on Tree Age (LPJ)**\n");
		logger(g_log, "Age = %d years\n", a->value);
		logger(g_log, "Age Mortality based on Tree Age (LPJ) = %g\n", s->value[AGEMORT]);
		Dead_trees = (int)(s->counter[N_TREE] * s->value[AGEMORT]);
		logger(g_log, "DEAD TREES = %d\n", Dead_trees);
		s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * Dead_trees * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
		s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * Dead_trees * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
		s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * Dead_trees * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
		//logger(g_log, "Wf after dead = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE]);
		//logger(g_log, "Wr after dead = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT]);
		//logger(g_log, "Ws after dead = %g tDM/ha\n", s->value[BIOMASS_STEM] );

		//----------------Number of trees after mortality---------------------

		s->counter[N_TREE] = s->counter[N_TREE] - Dead_trees;
		logger(g_log, "Number of Trees  after age mortality = %d trees\n", s->counter[N_TREE]);

		if ( !s->counter[DEAD_STEMS])
		{
			s->counter[DEAD_STEMS] = Dead_trees;
		}
		else
		{
			s->counter[DEAD_STEMS] += Dead_trees;
		}
	}
	else
	{
		logger(g_log, "**NO-MORTALITY based on Tree Age (LPJ)**\n");
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
