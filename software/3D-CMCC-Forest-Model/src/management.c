/*management.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Choose_management (CELL *c, SPECIES *s, int years, int height)
{
	if (years == 0)
	{
		int Manag;
		printf("END OF FIRST YEAR RUN \n");
		//printf("INSERT VALUE FOR MANAGEMENT (T = timber; C = Coppice): ");
		//scanf ("%c",&Manag);
		//Log("Management routine choiced = %c \n", Manag);


		//Management
		if ( s->management == T)
		{
			Log("- Management type = TIMBER\n");
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
				Log("Case CLEARCUT choiced \n");

				//call function
				Clearcut_Timber_upon_request (s,  years, c->heights[height].z, c->annual_layer_number);

				break;

			case 2 :
				Log("Case ....... choiced \n");

				//call function

				break;

			case 3 :
				Log("Case .......  choiced \n");

				//call function

				break;

			case 4 :
				Log("Case .......  choiced \n");

				//call function

				break;

			}

		}
		else
		{
			Log("- Management type = COPPICE\n");
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
				Log("Case CLEARCUT choiced \n");

				//call function
				Clearcut_Coppice (s,  years, c->heights[height].z, c->annual_layer_number);

				break;

			case 2 :
				Log("Case ....... choiced \n");

				//call function

				break;

			case 3 :
				Log("Case .......  choiced \n");

				//call function

				break;

			case 4 :
				Log("Case .......  choiced \n");

				//call function

				break;
			}

		}

	}


}



void Management (SPECIES *const s, AGE * const a, int years)
{
	// ALESSIOR s->counter used instead of s->value
	if ( a->value >= s->value[MINAGEMANAG] /* && MINDBHMANAG*/)
	{
		// ALESSIOR s->counter used instead of s->value (int cast added)
		if ( years && !(years % (int)s->value[ROTATION]))
		{
			double IndWf,
			IndWs,
			IndWr,
			BiomRem;

			Log("**MANAGEMENT **\n");
			// ALESSIOR s->counter used instead of s->value
			Log("ROTATION = %d years\n",s->value[ROTATION]);
			//Only for Dominant Layer
			//Individual Biomass

			IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
			IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
			IndWr = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
			IndWr = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
			Log("Individual Biomass:\nWf = %f\nWs = %f\nWr = %f\n", IndWf, IndWs, IndWr);

			//Percentage of Cutted Trees for Managment
			//Roots should remains in ecosystem!!
			s->counter[CUT_TREES] = settings->harvested_tree * s->counter[N_TREE];
			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[CUT_TREES];
			Log("Management Cutted Trees = %d trees/ha\n", s->counter[CUT_TREES]);
			Log("Number of Trees  after Management = %d trees\n", s->counter[N_TREE]);
			//Recompute Biomass
			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWr * s->counter[N_TREE];
			s->value[BIOMASS_FINE_ROOT_tDM] = IndWr * s->counter[N_TREE];
			Log("Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\n",
					s->value[BIOMASS_FOLIAGE_tDM],
					s->value[BIOMASS_STEM_tDM],
					s->value[BIOMASS_FINE_ROOT_tDM],
					s->value[BIOMASS_COARSE_ROOT_tDM]);

			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
			Log("Total Biomass harvested from ecosystem = %f\n", BiomRem);
			// Total Biomass at the end
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
			Log("Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);
		}
		else
		{
			Log("NO ROTATION in Dominant layer\n");
			Log("NO MANAGEMENT in Dominant layer\n");
			s->counter[CUT_TREES] = 0;
		}
		Log("NO ROTATION in Dominant layer\n");
		Log("NO MANAGEMENT in Dominant layer\n");
		s->counter[CUT_TREES] = 0;
	}
	Log("NO ROTATION in Dominant layer\n");
	Log("NO MANAGEMENT in Dominant layer\n");
	s->counter[CUT_TREES] = 0;
}

void Clearcut_Timber_without_request (SPECIES *s, CELL *c, int years)
{
	int removed_tree = 0.0;
	double IndWf,
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

	double stand_basal_area_to_remove;
	//double

	//CLEARCUT FOR TIMBER (Taglio raso)
	Log("CLEARCUT FOR TIMBER FUNCTION \n");

	IndWf = s->value[LEAF_C] / s->counter[N_TREE];
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


//	stand_basal_area_to_remove = settings->basal_area_remove * s->value[STAND_BASAL_AREA];
//	removed_tree = round(stand_basal_area_to_remove / s->value[BASAL_AREA]);
//	(double)removed_tree = stand_basal_area_to_remove / s->value[BASAL_AREA];
//	removed_tree = (int)floor(removed_tree+0.5);






	removed_tree = s->counter[N_TREE] * (settings->harvested_tree / 100.0 );
	Log("removed tree = %d\n", removed_tree);


	Log("Number of trees removed = %d trees/ha \n", removed_tree);

	s->counter[N_TREE] -= removed_tree;
	Log("Number of trees after management = %d \n", s->counter[N_TREE]);

	//Recompute Biomass
	s->value[LEAF_C] = IndWf * s->counter[N_TREE];
	s->value[STEM_C] = IndWs * s->counter[N_TREE];
	s->value[COARSE_ROOT_C] = IndWrc * s->counter[N_TREE];
	s->value[FINE_ROOT_C] = IndWrf * s->counter[N_TREE];
	s->value[BRANCH_C] = IndWbb * s->counter[N_TREE];
	s->value[RESERVE_C] = IndWres * s->counter[N_TREE];
	Log("Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\nWrBB = %f\n Wres = %f\n",
			s->value[LEAF_C],
			s->value[STEM_C],
			s->value[FINE_ROOT_C],
			s->value[COARSE_ROOT_C],
			s->value[BRANCH_C],
			s->value[RESERVE_C]);

	// Total Biomass at the end
	s->value[TOTAL_W] = s->value[LEAF_C] + s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C] + s->value[STEM_C] + s->value[BRANCH_C] + s->value[RESERVE_C];
	Log("Total Biomass = %f tC/ha\n", s->value[TOTAL_W]);

	/* update stand trees */
	c->n_tree -= removed_tree;
	c->annual_dead_tree += removed_tree;

	/* adding coarse and fine root and leaf to litter pool */
	c->daily_litter_carbon_tC +=  (IndWrc * removed_tree) + (IndWrf * removed_tree) + (IndWf * removed_tree);

}

void Clearcut_Timber_upon_request (SPECIES *const s, int years, int z, int number_of_layers)
{
	int removed_tree;
	int layer_to_remove_tree;
	double IndWf,
	IndWs,
	IndWrf,
	IndWrc,
	IndWbb,
	IndWres;
	double BiomRem;

	int shoots_number;    //number of shoots produced after coppicing

	IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
	IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
	IndWrc = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
	IndWrf = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
	IndWbb = s->value[BIOMASS_BRANCH_tDM] / s->counter[N_TREE];
	IndWres = s->value[RESERVE_tDM] / s->counter[N_TREE];



	//CLEARCUT FOR TIMBER (Taglio raso)
	Log("CLEARCUT FOR TIMBER FUNCTION \n");
	Log("Numbers of layers = %d \n", number_of_layers);

	Log("Layer modelled z = %d \n", z);

	printf ("Number of trees removed = ?\n");
	scanf ("%d", &removed_tree);

	if (removed_tree != 0)
	{
		if (number_of_layers >= 3)
		{

			while ( 1 )
			{
				puts( "Layer from which remove trees = ?\n"
						"-2 = dominant\n"
						"-1 = dominated\n"
						"-0 = sub-dominated");

				scanf ("%d", &layer_to_remove_tree);
				printf ("layer choiced = %d \n", layer_to_remove_tree);

				if ( layer_to_remove_tree < 0 || layer_to_remove_tree > 2 )
				{
					puts("NO CHOICED VALUE CAN BE ACCEPTED");
				}
				else
				{
					break;
				}
			}
		}
		else if (number_of_layers == 2)
		{
			while ( 1 )  {
				puts( "Layer from which remove trees =\n"
						"-1 = dominant\n"
						"-0 = dominated");

				scanf ("%d", &layer_to_remove_tree);
				printf ("layer choiced = %d \n", layer_to_remove_tree);

				if ( layer_to_remove_tree < 0 || layer_to_remove_tree > 1 )
				{
					puts("NO CHOICED VALUE CAN BE ACCEPTED");
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			layer_to_remove_tree = 0;
			Log ("Layer from which remove trees = %d \n", layer_to_remove_tree);
		}

		//in case of more than one layer
		if (layer_to_remove_tree == z)
		{
			Log("Number of trees removed = %d trees/ha \n", removed_tree);

			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
			Log("Number of mother trees after management = %d \n", s->counter[N_TREE] );

			//Recompute Biomass
			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
			Log("Biomass after management:\nWf = %f\nWs = %f\nWrf = %f\nWrc = %f\n",
					s->value[BIOMASS_FOLIAGE_tDM],
					s->value[BIOMASS_STEM_tDM],
					s->value[BIOMASS_FINE_ROOT_tDM],
					s->value[BIOMASS_COARSE_ROOT_tDM]);

			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
			Log("Total Biomass harvested from ecosystem = %f\n", BiomRem);
			// Total Biomass at the end
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
			Log("Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);


			//DA QUI DEVE NASCERE UNA NUOVA CLASSE  !!!!!!!
			//compute number of shoot produced after coppicing
			shoots_number = removed_tree * s->value[AV_SHOOT];
			Log ("Number of shoots produced after coppicing = %d shoots/ha \n", shoots_number);



		}
		else
		{
			Log("Layer uncutted \n");
		}
	}
}

void Clearcut_Coppice (SPECIES *const s, int years, int z, int number_of_layers)
{
	int removed_tree;
	int layer_to_remove_tree;
	double IndWf,
	IndWs,
	IndWrf,
	IndWrc;
	double BiomRem;

	int shoots_number;



	IndWf = s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE];
	IndWs = s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE];
	IndWrc = s->value[BIOMASS_COARSE_ROOT_tDM] / s->counter[N_TREE];
	IndWrf = s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[N_TREE];
	//Log("Individual Biomass:\nWf = %f\nWs = %f\nWr = %f\n", IndWf, IndWs, IndWr);


	//CLEARCUT FOR TIMBER
	Log("CLEARCUT FOR COPPICE FUNCTION \n");
	Log("Layer modelled z = %d \n", z);
	Log("Numbers of layers = %d \n", number_of_layers);
	Log("Number of stools = %d \n", s->counter[N_STUMP]);

	printf ("Number of trees removed = ?\n");
	scanf ("%d", &removed_tree);

	if ( removed_tree != 0)
	{
		if (number_of_layers >= 3)
		{

			while ( 1 )  {
				puts( "Layer from which remove trees = ?\n"
						"-2 = dominant\n"
						"-1 = dominated\n"
						"-0 = sub-dominated");

				scanf ("%d", &layer_to_remove_tree);
				printf ("layer choiced = %d \n", layer_to_remove_tree);

				if ( layer_to_remove_tree < 0 || layer_to_remove_tree > 2 )
				{
					puts("NO CHOICED VALUE CAN BE ACCEPTED");
				}
				else
				{
					break;
				}
			}
		}
		else if (number_of_layers == 2)
		{
			while ( 1 )  {
				puts( "Layer from which remove trees =\n"
						"-1 = dominant\n"
						"-0 = dominated");

				scanf ("%d", &layer_to_remove_tree);
				printf ("layer choiced = %d \n", layer_to_remove_tree);

				if ( layer_to_remove_tree < 0 || layer_to_remove_tree > 1 )
				{
					puts("NO CHOICED VALUE CAN BE ACCEPTED");
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			layer_to_remove_tree = 0;
			Log ("Layer from which remove trees = %d \n", layer_to_remove_tree);
		}

		if (layer_to_remove_tree == z)
		{
			Log("Number of trees removed = %d trees/ha \n", removed_tree);

			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
			Log("Number of trees after management = %d \n", s->counter[N_TREE] );
			Log("Number of stools = %d \n", s->counter[N_STUMP]);

			//Recompute Biomass
			s->value[BIOMASS_FOLIAGE_tDM] = IndWf * s->counter[N_TREE];
			s->value[BIOMASS_STEM_tDM] = IndWs * s->counter[N_TREE];
			s->value[BIOMASS_COARSE_ROOT_tDM] = IndWrc * s->counter[N_TREE];
			s->value[BIOMASS_FINE_ROOT_tDM] = IndWrf * s->counter[N_TREE];
			Log("Biomass after management:\nWf = %f\nWs = %f\nWrc = %f\n Wrf = %f\n",
					s->value[BIOMASS_FOLIAGE_tDM],
					s->value[BIOMASS_STEM_tDM],
					s->value[BIOMASS_COARSE_ROOT_tDM],
					s->value[BIOMASS_FINE_ROOT_tDM]);

			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] /* ??? m->lpCell[index].Wr??*/);
			Log("Total Biomass harvested from ecosystem = %f\n", BiomRem);
			// Total Biomass at the end
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_STEM_tDM];
			Log("Total 'live' Biomass = %f tDM/ha\n", s->value[TOTAL_W]);



			//DA QUI DEVE NASCERE UNA NUOVA CLASSE DI ETA' = 1 !!!!!!!
			//compute number of shoot produced after coppicing
			shoots_number = removed_tree * s->value[AV_SHOOT];
			Log ("Number of shoots produced after coppicing = %f shoots/ha \n", shoots_number);


		}
		else
		{
			Log("Layer uncutted \n");
		}
	}
}
