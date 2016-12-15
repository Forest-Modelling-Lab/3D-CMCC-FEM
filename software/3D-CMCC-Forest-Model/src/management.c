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

	/* sort class in ascending way by heights */
	qsort ( c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc );

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

						/* reset years_for_thinning */
						years_for_thinning = 0;

						/* check that all mandatory variables are filled */
						CHECK_CONDITION (g_settings->replanted_n_tree, <, 0);
						CHECK_CONDITION (g_settings->replanted_height, <, 1.3);
						CHECK_CONDITION (g_settings->replanted_avdbh, <, 0);
						CHECK_CONDITION (g_settings->replanted_age, <, 0);

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
	if (s->value[THINNING_REGIME] == 0)
	{

	}
	else
	{

	}

	logger(g_debug_log, "** Management options: Thinning ** \n");

	logger(g_debug_log, "basal area before thinning = %f m2/class cell\n", s->value[STAND_BASAL_AREA_m2]);
	logger(g_debug_log, "trees before thinning = %d trees/cell\n", s->counter[N_TREE]);

	/* compute basal area to remain */
	stand_basal_area_to_remain = (1.0 - (s->value[THINNING_INTENSITY] / 100.0 ) ) * s->value[STAND_BASAL_AREA_m2];
	logger(g_debug_log, "basal area to remain = %f m2/class\n", stand_basal_area_to_remain);

	/* compute basal area to remove */
	stand_basal_area_to_remove = (s->value[THINNING_INTENSITY] / 100.0) * s->value[STAND_BASAL_AREA_m2];
	logger(g_debug_log, "basal area to remove = %f\n", stand_basal_area_to_remove);

	/* compute integer number of trees to remove */
	trees_to_remove = ROUND((s->value[THINNING_INTENSITY] / 100.0) * s->counter[N_TREE]);
	logger(g_debug_log, "trees_to_remove = %d\n", trees_to_remove);

	/* update C and N biomass */
	tree_biomass_remove ( s, trees_to_remove );

	/* remove trees */
	s->counter[N_TREE] -= trees_to_remove;
	logger(g_debug_log, "Number of trees after management = %d trees/cell\n", s->counter[N_TREE]);

	/* check */
	CHECK_CONDITION(s->counter[N_TREE], <, 0);

	/*********************************************************************************************************************************************************************/

	/* adding coarse and fine root and leaf to litter pool */
	c->daily_litter_carbon_tC += s->value[C_TO_LITTER] ;

	/* Total class C at the end */
	s->value[TOTAL_C] = s->value[LEAF_C] + s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C] + s->value[STEM_C] + s->value[BRANCH_C] + s->value[RESERVE_C];
	logger(g_debug_log, "Total Biomass = %f tC/ha\n", s->value[TOTAL_C]);

	/* update stand trees */
	c->cell_n_trees -= trees_to_remove;
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
	tree_biomass_remove ( s, s->counter[N_TREE] );

	/* remove completely all trees */
	tree_class_remove (c, height, dbh, age, species );
}

/*****************************************************************************************************************************************/







//
//void clearcut_timber_upon_request(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
//{
//	int removed_tree;
//	int layer_to_remove_tree;
//	double IndWf,
//	IndWs,
//	IndWrf,
//	IndWrc,
//	IndWbb,
//	IndWres;
//
//	tree_layer_t *l;
//	species_t *s;
//
//	l = &c->tree_layers[layer];
//	s = &c->heights[height].dbhs[dbh].ages[age].species[species];
//
//	IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
//	IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
//	IndWrc = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
//	IndWrf = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
//	IndWbb = s->value[BIOMASS_BRANCH_tDM] / s->counter[N_TREE];
//	IndWres = s->value[RESERVE_tDM] / s->counter[N_TREE];
//
//	/* CLEARCUT FOR TIMBER */
//	logger(g_debug_log, "CLEARCUT FOR TIMBER FUNCTION \n");
//	printf ("Number of trees removed = ?\n");
//	scanf ("%d", &removed_tree);
//
//	if ( removed_tree != 0 )
//	{
//		if ( c->tree_layers_count > 1 )
//		{
//			printf("Layer from which of %d layer(s) remove trees = ?\n", c->tree_layers_count);
//
//			scanf ("%d", &layer_to_remove_tree);
//			printf ("layer choiced = %d \n", layer_to_remove_tree);
//
//			if ( layer_to_remove_tree == 0 || layer_to_remove_tree > c->tree_layers_count )
//			{
//				puts("NO CHOICED VALUE CAN BE ACCEPTED");
//			}
//
//			//fixme continue..........depending on which layer is chosen
//
//			//fixme continue..........
//			//
//			//			logger(g_debug_log, "Number of trees removed = %d trees/ha \n", removed_tree);
//			//
//			//			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
//			//			logger(g_debug_log, "Number of mother trees after management = %d \n", s->counter[N_TREE] );
//			//
//			//			//Recompute Biomass
//			//			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
//			//			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
//			//			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
//			//			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
//			//			logger(g_debug_log, "Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\n",
//			//					s->value[BIOMASS_FOLIAGE_tDM],
//			//					s->value[BIOMASS_STEM_tDM],
//			//					s->value[BIOMASS_FINE_ROOT_tDM],
//			//					s->value[BIOMASS_COARSE_ROOT_tDM]);
//			//
//			//			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
//			//			logger(g_debug_log, "Total Biomass harvested from ecosystem = %f\n", BiomRem);
//			//			// Total Biomass at the end
//			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
//			//			logger(g_debug_log, "Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
//			//
//			//
//			//			//DA QUI DEVE NASCERE UNA NUOVA CLASSE (se pollonifera) !!!!!!!
//			//			//compute number of shoot produced after coppicing
//			//			shoots_number = removed_tree * s->value[AV_SHOOT];
//			//			logger(g_debug_log, "Number of shoots produced after coppicing = %d shoots/ha \n", shoots_number);
//
//		}
//		else
//		{
//
//			//			logger(g_debug_log, "Number of trees removed = %d trees/ha \n", removed_tree);
//			//
//			//			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
//			//			logger(g_debug_log, "Number of mother trees after management = %d \n", s->counter[N_TREE] );
//			//
//			//			//Recompute Biomass
//			//			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
//			//			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
//			//			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
//			//			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
//			//			logger(g_debug_log, "Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\n",
//			//					s->value[BIOMASS_FOLIAGE_tDM],
//			//					s->value[BIOMASS_STEM_tDM],
//			//					s->value[BIOMASS_FINE_ROOT_tDM],
//			//					s->value[BIOMASS_COARSE_ROOT_tDM]);
//			//
//			//			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
//			//			logger(g_debug_log, "Total Biomass harvested from ecosystem = %f\n", BiomRem);
//			//			// Total Biomass at the end
//			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
//			//			logger(g_debug_log, "Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
//			//
//			//
//			//			//DA QUI DEVE NASCERE UNA NUOVA CLASSE (se pollonifera) !!!!!!!
//			//			//compute number of shoot produced after coppicing
//			//			shoots_number = removed_tree * s->value[AV_SHOOT];
//			//			logger(g_debug_log, "Number of shoots produced after coppicing = %d shoots/ha \n", shoots_number);
//		}
//	}
//}
//
//void clearcut_coppice(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
//{
//	int removed_tree;
//	int layer_to_remove_tree;
//	double IndWf,
//	IndWs,
//	IndWrf,
//	IndWrc;
//
//	species_t *s;
//
//	s = &c->heights[height].dbhs[dbh].ages[age].species[species];
//
//	IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
//	IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
//	IndWrc = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
//	IndWrf = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
//	//logger(g_debug_log, "Individual Biomass:\nWf = %f\nWs = %f\nWr = %f\n", IndWf, IndWs, IndWr);
//
//	/* CLEARCUT FOR COPPICE */
//	logger(g_debug_log, "CLEARCUT FOR COPPICE FUNCTION \n");
//	logger(g_debug_log, "Layer modelled z = %d \n", layer);
//	logger(g_debug_log, "Numbers of layers = %d \n", c->tree_layers_count);
//	logger(g_debug_log, "Number of stools = %d \n", s->counter[N_STUMP]);
//
//	printf ("Number of trees removed = ?\n");
//	scanf ("%d", &removed_tree);
//
//	if ( removed_tree != 0 )
//	{
//		if ( c->tree_layers_count > 1 )
//		{
//			printf("Layer from which of %d layer(s) remove trees = ?\n", c->tree_layers_count);
//
//			scanf ("%d", &layer_to_remove_tree);
//			printf ("layer choiced = %d \n", layer_to_remove_tree);
//
//			if ( layer_to_remove_tree == 0 || layer_to_remove_tree > c->tree_layers_count )
//			{
//				puts("NO CHOICED VALUE CAN BE ACCEPTED");
//			}
//
//			//fixme continue..........depending on which layer is chosen
//
//			//fixme continue..........
//			//			logger(g_debug_log, "Number of trees removed = %d trees/ha \n", removed_tree);
//			//
//			//			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
//			//			logger(g_debug_log, "Number of trees after management = %d \n", s->counter[N_TREE] );
//			//			logger(g_debug_log, "Number of stools = %d \n", s->counter[N_STUMP]);
//			//
//			//			//Recompute Biomass
//			//			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
//			//			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
//			//			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
//			//			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
//			//			logger(g_debug_log, "Biomass after management:\nWf = %f\nWs = %f\nWrc = %f\n Wrf = %f\n",
//			//					s->value[BIOMASS_FOLIAGE_tDM],
//			//					s->value[BIOMASS_STEM_tDM],
//			//					s->value[BIOMASS_COARSE_ROOT_tDM],
//			//					s->value[BIOMASS_FINE_ROOT_tDM]);
//			//
//			//			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
//			//			logger(g_debug_log, "Total Biomass harvested from ecosystem = %f\n", BiomRem);
//			//			// Total Biomass at the end
//			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
//			//			logger(g_debug_log, "Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
//			//
//			//
//			//
//			//			//DA QUI DEVE NASCERE UNA NUOVA CLASSE DI ETA' = 1 !!!!!!!
//			//			//compute number of shoot produced after coppicing
//			//			shoots_number = removed_tree * s->value[AV_SHOOT];
//			//			logger(g_debug_log, "Number of shoots produced after coppicing = %f shoots/ha \n", shoots_number);
//			//
//			//
//			//		}
//			//		else
//			//		{
//			//			logger(g_debug_log, "Layer uncutted \n");
//			//		}
//		}
//	}
//}
//
//void choose_management(species_t *const s, const int years)
//{
//	if ( !years )
//	{
//		int Manag;
//		printf("END OF FIRST YEAR RUN \n");
//		//printf("INSERT VALUE FOR MANAGEMENT (T = timber; C = Coppice): ");
//		//scanf ("%c",&Manag);
//		//logger(g_debug_log, "Management routine choiced = %c \n", Manag);
//
//		/* Management */
//		if ( s->management == T )
//		{
//			logger(g_debug_log, "- Management type = TIMBER\n");
//			printf("SELECT TYPE OF MANAGEMENT: \n"
//					"-CLEARCUT = 1 \n"
//					"-........ = 2 \n"
//					"-........ = 3 \n"
//					"-........ = 4 \n"
//					"-........ = 5 \n");
//
//			scanf ("%d",&Manag);
//
//			switch ( Manag )
//			{
//			case 1 :
//				logger(g_debug_log, "Case CLEARCUT choiced \n");
//
//				//Clearcut_Timber_upon_request (s,  years, c->heights[height].z, c->annual_layer_number);
//
//				break;
//
//			case 2 :
//				logger(g_debug_log, "Case ....... choiced \n");
//
//				//fixme call function
//
//				break;
//
//			case 3 :
//				logger(g_debug_log, "Case .......  choiced \n");
//
//				//fixme call function
//
//				break;
//
//			case 4 :
//				logger(g_debug_log, "Case .......  choiced \n");
//
//				//fixme call function
//
//				break;
//
//			}
//
//		}
//		else
//		{
//			logger(g_debug_log, "- Management type = COPPICE\n");
//			printf("SELECT TYPE OF MANAGEMENT: \n"
//					"-CLEARCUT = 1 \n"
//					"-........ = 2 \n"
//					"-........ = 3 \n"
//					"-........ = 4 \n"
//					"-........ = 5 \n");
//
//			scanf ("%d",&Manag);
//
//			switch (Manag)
//			{
//			case 1 :
//				logger(g_debug_log, "Case CLEARCUT choiced \n");
//
//				//Clearcut_Coppice (s, years, c->heights[height].z, c->annual_layer_number);
//
//				break;
//
//			case 2 :
//				logger(g_debug_log, "Case ....... choiced \n");
//
//				//fixme call function
//
//				break;
//
//			case 3 :
//				logger(g_debug_log, "Case .......  choiced \n");
//
//				//fixme call function
//
//				break;
//
//			case 4 :
//				logger(g_debug_log, "Case .......  choiced \n");
//
//				//fixme call function
//
//				break;
//			}
//		}
//	}
//}




