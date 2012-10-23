/*management.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math.h"
#include "types.h"



void Get_Management (SPECIES *const s, int years)
{
	if ( s->counter[TREE_AGE] >= s->counter[MINAGEMANAG] /* && MINDBHMANAG*/)
	{
		if ( years && !(years % s->counter[ROTATION]))
		{
			float IndWf,
			IndWs,
			IndWr,
			BiomRem;

			Log("**MANAGEMENT **\n");
			Log("ROTATION = %d years\n",s->counter[ROTATION]);
			//Only for Dominant Layer
			//Individual Biomass

			IndWf = s->value[BIOMASS_FOLIAGE_CTEM] / s->counter[N_TREE];
			IndWs = s->value[BIOMASS_STEM_CTEM] / s->counter[N_TREE];
			IndWr = s->value[BIOMASS_ROOTS_FINE_CTEM] / s->counter[N_TREE];
			IndWr = s->value[BIOMASS_ROOTS_COARSE_CTEM] / s->counter[N_TREE];
			Log("Individual Biomass:\nWf = %g\nWs = %g\nWr = %g\n", IndWf, IndWs, IndWr);

			//Numbers of Cutted Trees for Managment
			//Roots should remains in ecosystem!!
			s->counter[CUT_TREES] = site->cutTree;
			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[CUT_TREES];
			Log("Management Cutted Trees = %d trees/ha\n", s->counter[CUT_TREES]);
			Log("Number of Trees  after Management = %d trees\n", s->counter[N_TREE]);
			//Recompute Biomass
			s->value[BIOMASS_FOLIAGE_CTEM] = IndWf * s->counter[N_TREE];
			s->value[BIOMASS_STEM_CTEM] = IndWs * s->counter[N_TREE];
			s->value[BIOMASS_ROOTS_COARSE_CTEM] = IndWr * s->counter[N_TREE];
			s->value[BIOMASS_ROOTS_FINE_CTEM] = IndWr * s->counter[N_TREE];
			Log("Biomass after management:\nWf = %g\nWs = %g\nWrf = %g\nWrc = %g\n",
					s->value[BIOMASS_FOLIAGE_CTEM],
					s->value[BIOMASS_STEM_CTEM],
					s->value[BIOMASS_ROOTS_FINE_CTEM],
					s->value[BIOMASS_ROOTS_COARSE_CTEM]);

			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] /* ??? m->lpCell[index].Wr??*/);
			Log("Total Biomass harvested from ecosystem = %g\n", BiomRem);
			// Total Biomass at the end
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] + s->value[BIOMASS_STEM_CTEM];
			Log("Total 'live' Biomass = %g tDM/ha\n", s->value[TOTAL_W]);
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


void Clearcut_Timber (SPECIES *const s, int years, int z, int number_of_layers)
{
	int removed_tree;
	int layer_to_remove_tree;
	float IndWf,
	IndWs,
	IndWrf,
	IndWrc;
	float BiomRem;

	int shoots_number;    //number of shoots produced after coppicing

	IndWf = s->value[BIOMASS_FOLIAGE_CTEM] / s->counter[N_TREE];
	IndWs = s->value[BIOMASS_STEM_CTEM] / s->counter[N_TREE];
	IndWrc = s->value[BIOMASS_ROOTS_COARSE_CTEM] / s->counter[N_TREE];
	IndWrf = s->value[BIOMASS_ROOTS_FINE_CTEM] / s->counter[N_TREE];



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

		//in case of more than one layer
		if (layer_to_remove_tree == z)
		{
			Log("Number of trees removed = %d trees/ha \n", removed_tree);

			s->counter[N_TREE] = s->counter[N_TREE] - removed_tree;
			Log("Number of mother trees after management = %d \n", s->counter[N_TREE] );

			//Recompute Biomass
			s->value[BIOMASS_FOLIAGE_CTEM] = IndWf * s->counter[N_TREE];
			s->value[BIOMASS_STEM_CTEM] = IndWs * s->counter[N_TREE];
			s->value[BIOMASS_ROOTS_COARSE_CTEM] = IndWrc * s->counter[N_TREE];
			s->value[BIOMASS_ROOTS_FINE_CTEM] = IndWrf * s->counter[N_TREE];
			Log("Biomass after management:\nWf = %g\nWs = %g\nWrf = %g\nWrc = %g\n",
					s->value[BIOMASS_FOLIAGE_CTEM],
					s->value[BIOMASS_STEM_CTEM],
					s->value[BIOMASS_ROOTS_FINE_CTEM],
					s->value[BIOMASS_ROOTS_COARSE_CTEM]);

			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] /* ??? m->lpCell[index].Wr??*/);
			Log("Total Biomass harvested from ecosystem = %g\n", BiomRem);
			// Total Biomass at the end
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] + s->value[BIOMASS_STEM_CTEM];
			Log("Total 'live' Biomass = %g tDM/ha\n", s->value[TOTAL_W]);


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
	float IndWf,
	IndWs,
	IndWrf,
	IndWrc;
	float BiomRem;

	int shoots_number;



	IndWf = s->value[BIOMASS_FOLIAGE_CTEM] / s->counter[N_TREE];
	IndWs = s->value[BIOMASS_STEM_CTEM] / s->counter[N_TREE];
	IndWrc = s->value[BIOMASS_ROOTS_COARSE_CTEM] / s->counter[N_TREE];
	IndWrf = s->value[BIOMASS_ROOTS_FINE_CTEM] / s->counter[N_TREE];
	//Log("Individual Biomass:\nWf = %g\nWs = %g\nWr = %g\n", IndWf, IndWs, IndWr);


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
			s->value[BIOMASS_FOLIAGE_CTEM] = IndWf * s->counter[N_TREE];
			s->value[BIOMASS_STEM_CTEM] = IndWs * s->counter[N_TREE];
			s->value[BIOMASS_ROOTS_COARSE_CTEM] = IndWrc * s->counter[N_TREE];
			s->value[BIOMASS_ROOTS_FINE_CTEM] = IndWrf * s->counter[N_TREE];
			Log("Biomass after management:\nWf = %g\nWs = %g\nWrc = %g\n Wrf = %g\n",
					s->value[BIOMASS_FOLIAGE_CTEM],
					s->value[BIOMASS_STEM_CTEM],
					s->value[BIOMASS_ROOTS_COARSE_CTEM],
					s->value[BIOMASS_ROOTS_FINE_CTEM]);

			BiomRem = s->value[TOTAL_W] - (s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] /* ??? m->lpCell[index].Wr??*/);
			Log("Total Biomass harvested from ecosystem = %g\n", BiomRem);
			// Total Biomass at the end
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] + s->value[BIOMASS_STEM_CTEM];
			Log("Total 'live' Biomass = %g tDM/ha\n", s->value[TOTAL_W]);



			//DA QUI DEVE NASCERE UNA NUOVA CLASSE DI ETA' = 1 !!!!!!!
			//compute number of shoot produced after coppicing
			shoots_number = removed_tree * s->value[AV_SHOOT];
			Log ("Number of shoots produced after coppicing = %g shoots/ha \n", shoots_number);


		}
		else
		{
			Log("Layer uncutted \n");
		}
	}
}
