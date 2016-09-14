/*management.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include "management.h"
#include "constants.h"
#include "settings.h"
#include "common.h"
#include "logger.h"
#include "forest_tree_class.h"
#include "remove_tree_class.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void forest_management (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int year)
{
	age_t *a;
	species_t *s;

	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* this function handles all other management functions */
	if ( ! string_compare_i (g_settings->management, "on") && year )
	{
		/* if year of simulation matches with thinning and class age doesn't matches with rotation age */
		if ( ( ! ( ( year+1 ) % (int)s->value[THINNING] ) ) && ( a->value != s->value[ROTATION] ) )
		{
			logger(g_log,"**FOREST MANAGEMENT**\n");

			thinning ( c, layer, height, dbh, age, species, year );
		}
		/* if class age matches with harvesting */
		if ( a->value == s->value[ROTATION] )
		{
			/* remove tree class */
			harvesting ( c, layer, height, dbh, age, species );

			/* replanting tree class */
			if( g_settings->replanted_n_tree )
			{
				if ( ! add_tree_class( c, height, dbh, age, species ) )
				{
					logger(g_log, "unable to add new height class! (exit)\n");
					exit(1);
				}
			}
		}
	}
}

/*****************************************************************************************************************************************/

void thinning (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int year)
{
	int trees_to_remove = 0;

	double IndWl,
	IndWs,
	IndWrf,
	IndWrc,
	IndWbb,
	IndWres,
	IndWslive,
	IndWsdead,
	IndWrclive,
	IndWrcdead,
	IndWbblive,
	IndWbbdead;

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

	logger(g_log, "** Management options: Thinning ** \n");

	logger(g_log, "basal area before thinning = %f m2/class cell\n", s->value[STAND_BASAL_AREA_m2]);
	logger(g_log, "trees before thinning = %d trees/cell\n", s->counter[N_TREE]);

	/* compute basal area to remain */
	stand_basal_area_to_remain = (g_settings->remainig_basal_area / 100.0 ) * s->value[STAND_BASAL_AREA_m2];
	logger(g_log, "basal area to remain = %f m2/class\n", stand_basal_area_to_remain);

	/* compute basal area to remove */
	stand_basal_area_to_remove = (1.0 - (g_settings->remainig_basal_area / 100.0)) * s->value[STAND_BASAL_AREA_m2];
	logger(g_log, "basal area to remove = %f\n", stand_basal_area_to_remove);

	/* compute integer number of trees to remove */
	trees_to_remove = ROUND((1.0 - (g_settings->remainig_basal_area / 100.0)) * s->counter[N_TREE]);
	logger(g_log, "trees_to_remove = %d\n", trees_to_remove);


	/* compute individual biomass before removing trees */
	IndWl = s->value[LEAF_C] / s->counter[N_TREE];
	IndWs = s->value[STEM_C] / s->counter[N_TREE];
	IndWrc = s->value[COARSE_ROOT_C] / s->counter[N_TREE];
	IndWrf = s->value[FINE_ROOT_C] / s->counter[N_TREE];
	IndWbb = s->value[BRANCH_C] / s->counter[N_TREE];
	IndWres = s->value[RESERVE_C] / s->counter[N_TREE];
	IndWslive = s->value[STEM_LIVE_WOOD_C] / s->counter[N_TREE];
	IndWsdead = s->value[STEM_DEAD_WOOD_C] / s->counter[N_TREE];
	IndWrclive = s->value[COARSE_ROOT_LIVE_WOOD_C] / s->counter[N_TREE];
	IndWrcdead = s->value[COARSE_ROOT_DEAD_WOOD_C] / s->counter[N_TREE];
	IndWbblive = s->value[BRANCH_LIVE_WOOD_C] / s->counter[N_TREE];
	IndWbbdead = s->value[BRANCH_DEAD_WOOD_C] / s->counter[N_TREE];

	/* remove trees */
	s->counter[N_TREE] -= trees_to_remove;
	logger(g_log, "Number of trees after management = %d trees/cell\n", s->counter[N_TREE]);

	/* check */
	CHECK_CONDITION(s->counter[N_TREE], < 0);

	/* recompute Biomass after removing trees */
	s->value[LEAF_C] = IndWl * s->counter[N_TREE];
	s->value[STEM_C] = IndWs * s->counter[N_TREE];
	s->value[COARSE_ROOT_C] = IndWrc * s->counter[N_TREE];
	s->value[FINE_ROOT_C] = IndWrf * s->counter[N_TREE];
	s->value[BRANCH_C] = IndWbb * s->counter[N_TREE];
	s->value[RESERVE_C] = IndWres * s->counter[N_TREE];
	s->value[STEM_LIVE_WOOD_C] = IndWslive * s->counter[N_TREE];
	s->value[STEM_DEAD_WOOD_C] = IndWsdead * s->counter[N_TREE];
	s->value[COARSE_ROOT_LIVE_WOOD_C] = IndWrclive * s->counter[N_TREE];
	s->value[COARSE_ROOT_DEAD_WOOD_C] = IndWrcdead * s->counter[N_TREE];
	s->value[BRANCH_LIVE_WOOD_C] = IndWbblive * s->counter[N_TREE];
	s->value[BRANCH_DEAD_WOOD_C] = IndWbbdead * s->counter[N_TREE];

	logger(g_log, "Main biomass pools after management:\nWl = %f\nWs = %f\nWrf = %f\nWrc = %f\nWrBB = %f\n Wres = %f\n",
			s->value[LEAF_C],
			s->value[STEM_C],
			s->value[FINE_ROOT_C],
			s->value[COARSE_ROOT_C],
			s->value[BRANCH_C],
			s->value[RESERVE_C]);

	/* Total class biomass at the end */
	s->value[TOTAL_W] = s->value[LEAF_C] + s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C] + s->value[STEM_C] + s->value[BRANCH_C] + s->value[RESERVE_C];
	logger(g_log, "Total Biomass = %f tC/ha\n", s->value[TOTAL_W]);

	/* update stand trees */
	c->cell_n_trees -= trees_to_remove;
	c->annual_dead_tree += trees_to_remove;

	/* adding coarse and fine root and leaf to litter pool */
	c->daily_litter_carbon_tC +=  (IndWrc * trees_to_remove) + (IndWrf * trees_to_remove) + (IndWl * trees_to_remove);

}

/*****************************************************************************************************************************************/

void harvesting (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	/* at the moment it considers a complete harvesting for all classes (if considered) */
	logger(g_log, "** Management options: Harvesting ** \n");

	/* remove completely all trees */
	tree_class_remove (c, height, dbh, age, species );

}

/*****************************************************************************************************************************************/








void clearcut_timber_upon_request(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	int removed_tree;
	int layer_to_remove_tree;
	double IndWf,
	IndWs,
	IndWrf,
	IndWrc,
	IndWbb,
	IndWres;

	tree_layer_t *l;
	species_t *s;

	l = &c->tree_layers[layer];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
	IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
	IndWrc = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
	IndWrf = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
	IndWbb = s->value[BIOMASS_BRANCH_tDM] / s->counter[N_TREE];
	IndWres = s->value[RESERVE_tDM] / s->counter[N_TREE];

	/* CLEARCUT FOR TIMBER */
	logger(g_log, "CLEARCUT FOR TIMBER FUNCTION \n");
	printf ("Number of trees removed = ?\n");
	scanf ("%d", &removed_tree);

	if ( removed_tree != 0 )
	{
		if ( c->tree_layers_count > 1 )
		{
			printf("Layer from which of %d layer(s) remove trees = ?\n", c->tree_layers_count);

			scanf ("%d", &layer_to_remove_tree);
			printf ("layer choiced = %d \n", layer_to_remove_tree);

			if ( layer_to_remove_tree == 0 || layer_to_remove_tree > c->tree_layers_count )
			{
				puts("NO CHOICED VALUE CAN BE ACCEPTED");
			}

			//fixme continue..........depending on which layer is chosen

			//fixme continue..........
			//
			//			logger(g_log, "Number of trees removed = %d trees/ha \n", removed_tree);
			//
			//			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
			//			logger(g_log, "Number of mother trees after management = %d \n", s->counter[N_TREE] );
			//
			//			//Recompute Biomass
			//			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
			//			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
			//			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
			//			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
			//			logger(g_log, "Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\n",
			//					s->value[BIOMASS_FOLIAGE_tDM],
			//					s->value[BIOMASS_STEM_tDM],
			//					s->value[BIOMASS_FINE_ROOT_tDM],
			//					s->value[BIOMASS_COARSE_ROOT_tDM]);
			//
			//			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
			//			logger(g_log, "Total Biomass harvested from ecosystem = %f\n", BiomRem);
			//			// Total Biomass at the end
			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
			//			logger(g_log, "Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
			//
			//
			//			//DA QUI DEVE NASCERE UNA NUOVA CLASSE (se pollonifera) !!!!!!!
			//			//compute number of shoot produced after coppicing
			//			shoots_number = removed_tree * s->value[AV_SHOOT];
			//			logger(g_log, "Number of shoots produced after coppicing = %d shoots/ha \n", shoots_number);

		}
		else
		{

			//			logger(g_log, "Number of trees removed = %d trees/ha \n", removed_tree);
			//
			//			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
			//			logger(g_log, "Number of mother trees after management = %d \n", s->counter[N_TREE] );
			//
			//			//Recompute Biomass
			//			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
			//			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
			//			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
			//			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
			//			logger(g_log, "Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\n",
			//					s->value[BIOMASS_FOLIAGE_tDM],
			//					s->value[BIOMASS_STEM_tDM],
			//					s->value[BIOMASS_FINE_ROOT_tDM],
			//					s->value[BIOMASS_COARSE_ROOT_tDM]);
			//
			//			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
			//			logger(g_log, "Total Biomass harvested from ecosystem = %f\n", BiomRem);
			//			// Total Biomass at the end
			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
			//			logger(g_log, "Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
			//
			//
			//			//DA QUI DEVE NASCERE UNA NUOVA CLASSE (se pollonifera) !!!!!!!
			//			//compute number of shoot produced after coppicing
			//			shoots_number = removed_tree * s->value[AV_SHOOT];
			//			logger(g_log, "Number of shoots produced after coppicing = %d shoots/ha \n", shoots_number);
		}
	}
}

void clearcut_coppice(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	int removed_tree;
	int layer_to_remove_tree;
	double IndWf,
	IndWs,
	IndWrf,
	IndWrc;

	species_t *s;

	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
	IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
	IndWrc = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
	IndWrf = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
	//logger(g_log, "Individual Biomass:\nWf = %f\nWs = %f\nWr = %f\n", IndWf, IndWs, IndWr);

	/* CLEARCUT FOR COPPICE */
	logger(g_log, "CLEARCUT FOR COPPICE FUNCTION \n");
	logger(g_log, "Layer modelled z = %d \n", layer);
	logger(g_log, "Numbers of layers = %d \n", c->tree_layers_count);
	logger(g_log, "Number of stools = %d \n", s->counter[N_STUMP]);

	printf ("Number of trees removed = ?\n");
	scanf ("%d", &removed_tree);

	if ( removed_tree != 0 )
	{
		if ( c->tree_layers_count > 1 )
		{
			printf("Layer from which of %d layer(s) remove trees = ?\n", c->tree_layers_count);

			scanf ("%d", &layer_to_remove_tree);
			printf ("layer choiced = %d \n", layer_to_remove_tree);

			if ( layer_to_remove_tree == 0 || layer_to_remove_tree > c->tree_layers_count )
			{
				puts("NO CHOICED VALUE CAN BE ACCEPTED");
			}

			//fixme continue..........depending on which layer is chosen

			//fixme continue..........
			//			logger(g_log, "Number of trees removed = %d trees/ha \n", removed_tree);
			//
			//			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
			//			logger(g_log, "Number of trees after management = %d \n", s->counter[N_TREE] );
			//			logger(g_log, "Number of stools = %d \n", s->counter[N_STUMP]);
			//
			//			//Recompute Biomass
			//			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
			//			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
			//			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
			//			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
			//			logger(g_log, "Biomass after management:\nWf = %f\nWs = %f\nWrc = %f\n Wrf = %f\n",
			//					s->value[BIOMASS_FOLIAGE_tDM],
			//					s->value[BIOMASS_STEM_tDM],
			//					s->value[BIOMASS_COARSE_ROOT_tDM],
			//					s->value[BIOMASS_FINE_ROOT_tDM]);
			//
			//			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
			//			logger(g_log, "Total Biomass harvested from ecosystem = %f\n", BiomRem);
			//			// Total Biomass at the end
			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
			//			logger(g_log, "Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
			//
			//
			//
			//			//DA QUI DEVE NASCERE UNA NUOVA CLASSE DI ETA' = 1 !!!!!!!
			//			//compute number of shoot produced after coppicing
			//			shoots_number = removed_tree * s->value[AV_SHOOT];
			//			logger(g_log, "Number of shoots produced after coppicing = %f shoots/ha \n", shoots_number);
			//
			//
			//		}
			//		else
			//		{
			//			logger(g_log, "Layer uncutted \n");
			//		}
		}
	}
}

void choose_management(species_t *const s, const int years)
{
	if ( !years )
	{
		int Manag;
		printf("END OF FIRST YEAR RUN \n");
		//printf("INSERT VALUE FOR MANAGEMENT (T = timber; C = Coppice): ");
		//scanf ("%c",&Manag);
		//logger(g_log, "Management routine choiced = %c \n", Manag);

		/* Management */
		if ( s->management == T )
		{
			logger(g_log, "- Management type = TIMBER\n");
			printf("SELECT TYPE OF MANAGEMENT: \n"
					"-CLEARCUT = 1 \n"
					"-........ = 2 \n"
					"-........ = 3 \n"
					"-........ = 4 \n"
					"-........ = 5 \n");

			scanf ("%d",&Manag);

			switch ( Manag )
			{
			case 1 :
				logger(g_log, "Case CLEARCUT choiced \n");

				//Clearcut_Timber_upon_request (s,  years, c->heights[height].z, c->annual_layer_number);

				break;

			case 2 :
				logger(g_log, "Case ....... choiced \n");

				//fixme call function

				break;

			case 3 :
				logger(g_log, "Case .......  choiced \n");

				//fixme call function

				break;

			case 4 :
				logger(g_log, "Case .......  choiced \n");

				//fixme call function

				break;

			}

		}
		else
		{
			logger(g_log, "- Management type = COPPICE\n");
			printf("SELECT TYPE OF MANAGEMENT: \n"
					"-CLEARCUT = 1 \n"
					"-........ = 2 \n"
					"-........ = 3 \n"
					"-........ = 4 \n"
					"-........ = 5 \n");

			scanf ("%d",&Manag);

			switch (Manag)
			{
			case 1 :
				logger(g_log, "Case CLEARCUT choiced \n");

				//Clearcut_Coppice (s, years, c->heights[height].z, c->annual_layer_number);

				break;

			case 2 :
				logger(g_log, "Case ....... choiced \n");

				//fixme call function

				break;

			case 3 :
				logger(g_log, "Case .......  choiced \n");

				//fixme call function

				break;

			case 4 :
				logger(g_log, "Case .......  choiced \n");

				//fixme call function

				break;
			}
		}
	}
}




