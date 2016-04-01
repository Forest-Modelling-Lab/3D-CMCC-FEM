/*mortality.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Layer_cover_mortality (CELL *c, int height, int age, int species, double layer_cover, int tree_number)
{
	int i;
	//int oldNtree;
	int deadtree = 0;
	int oldNstump;
	int deadstump = 0;

	//the model makes die trees of the lower height class for that layer because
	//it passes through the function sort_by_height_desc the height classes starting from the lowest

	Log("\n\n*****LAYER_COVER_MORTALITY (CROWDING COMPETITION FUNCTION)*****\n");


	Log ("MORTALITY BASED ON HIGH CANOPY COVER layer %d, species %s height %f dbh %f !!!\n", c->heights[height].z, c->heights[height].ages[age].species[species].name,
			c->heights[height].value, c->heights[height].ages[age].species[species].value[AVDBH]);

	//Layer coverage mortality for timber
	//mortality occurs directly for timber
	if (c->heights[height].ages[age].species[species].management == T)
	{

		Log("Layer coverage mortality for timber \n");

		//compute average biomass
		c->heights[height].ages[age].species[species].value[AV_LEAF_MASS_KgC] = (c->heights[height].ages[age].species[species].value[LEAF_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_STEM_MASS_KgC] = (c->heights[height].ages[age].species[species].value[STEM_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_ROOT_MASS_KgC] = (c->heights[height].ages[age].species[species].value[TOT_ROOT_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_FINE_ROOT_MASS_KgC] = (c->heights[height].ages[age].species[species].value[FINE_ROOT_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_COARSE_ROOT_MASS_KgC] =(c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_RESERVE_MASS_KgC] = (c->heights[height].ages[age].species[species].value[RESERVE_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_BRANCH_MASS_KgC] = (c->heights[height].ages[age].species[species].value[BRANCH_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_LIVE_STEM_MASS_KgC] = (c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_DEAD_STEM_MASS_KgC] = (c->heights[height].ages[age].species[species].value[STEM_DEAD_WOOD_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (c->heights[height].ages[age].species[species].value[COARSE_ROOT_DEAD_WOOD_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_LIVE_BRANCH_MASS_KgC] = (c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;
		c->heights[height].ages[age].species[species].value[AV_DEAD_BRANCH_MASS_KgC] = (c->heights[height].ages[age].species[species].value[BRANCH_DEAD_WOOD_C]
											 /(double)c->heights[height].ages[age].species[species].counter[N_TREE])/1000.0;


		//check if cc passed to function is correct
		if (c->annual_layer_number == 1 && c->layer_cover_dominant >= settings->max_layer_cover)
		{
			Log("Layer cover in layer 0 passed to while = %f %% \n", c->layer_cover_dominant * 100);
		}
		if (c->annual_layer_number == 2 && (c->layer_cover_dominant >= settings->max_layer_cover || c->layer_cover_dominated >= settings->max_layer_cover))
		{
			if(c->layer_cover_dominant >= settings->max_layer_cover)
				Log("Layer cover in layer 1 passed to while= %f %%\n", c->layer_cover_dominant * 100);
			if(c->layer_cover_dominated >= settings->max_layer_cover)
				Log("Layer cover in layer 0 passed to while= %f %% \n", c->layer_cover_dominated * 100);
		}
		if (c->annual_layer_number > 2 && (c->layer_cover_dominant >= settings->max_layer_cover || c->layer_cover_dominated >= settings->max_layer_cover || c->layer_cover_subdominated >= settings->max_layer_cover))
		{
			if(c->layer_cover_dominant >= settings->max_layer_cover)
				Log("Layer cover in layer 2 passed to while= %f %%\n", c->layer_cover_dominant * 100);
			if(c->layer_cover_dominated >= settings->max_layer_cover)
				Log("Layer cover in layer 1 passed to while= %f %% \n", c->layer_cover_dominated * 100);
			if(c->layer_cover_subdominated >= settings->max_layer_cover)
				Log("Layer cover in layer 0 passed to while= %f %% \n", c->layer_cover_subdominated * 100);
		}


		/*
		while (layer_cover >= settings->max_layer_cover )
		{
			Log("layer cover prima while = %f\n", layer_cover);
			c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
			deadtree += 1;

			layer_cover = layer_cover - ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
			Log("layer cover dopo while = %f\n", layer_cover);
			c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]
		 * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
		}
		 */



		switch (c->annual_layer_number)
		{
		case 1:
			while ((c->layer_cover_dominant >= settings->max_layer_cover) && (c->heights[height].ages[age].species[species].counter[N_TREE] > 0))
			{
				//Log("layer cover prima while = %f\n", c->layer_cover_dominant);
				c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
				deadtree += 1;

				c->layer_cover_dominant -= ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
				//Log("layer cover dopo while = %f\n", c->layer_cover_dominant);
				c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]																				   * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
			}
			break;
		case 2:
			if (c->layer_cover_dominant >= settings->max_layer_cover)
			{
				while ((c->layer_cover_dominant >= settings->max_layer_cover) && (c->heights[height].ages[age].species[species].counter[N_TREE] > 0))
				{
					//Log("layer cover prima while = %f\n", c->layer_cover_dominant);
					c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
					deadtree += 1;

					c->layer_cover_dominant -= ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
					//Log("layer cover dopo while = %f\n", c->layer_cover_dominant);
					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]																					   * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
				}
			}
			else
			{
				while (c->layer_cover_dominated >= settings->max_layer_cover && c->heights[height].ages[age].species[species].counter[N_TREE] > 0)
				{
					//Log("layer cover prima while = %f\n", c->layer_cover_dominated);
					c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
					deadtree += 1;

					c->layer_cover_dominated -= ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
					//Log("layer cover dopo while = %f\n", c->layer_cover_dominated);
					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]																					   * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
				}
			}
			break;
		case 3:
			if (c->layer_cover_dominant >= settings->max_layer_cover )
			{
				while (c->layer_cover_dominant >= settings->max_layer_cover && c->heights[height].ages[age].species[species].counter[N_TREE] > 0)
				{
					//Log("layer cover prima while = %f\n", c->layer_cover_dominant);
					c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
					deadtree += 1;

					c->layer_cover_dominant -= ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
					//Log("layer cover dopo while = %f\n", c->layer_cover_dominant);
					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]																					   * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
				}
			}
			else if (c->layer_cover_dominated >= settings->max_layer_cover)
			{
				while (c->layer_cover_dominated >= settings->max_layer_cover && c->heights[height].ages[age].species[species].counter[N_TREE] > 0)
				{
					//Log("layer cover prima while = %f\n", c->layer_cover_dominated);
					c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
					deadtree += 1;

					c->layer_cover_dominated -= ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
					//Log("layer cover dopo while = %f\n", c->layer_cover_dominated);
					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]																					   * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
				}
			}
			else
			{
				while (c->layer_cover_subdominated >= settings->max_layer_cover && c->heights[height].ages[age].species[species].counter[N_TREE] > 0)
				{
					//Log("layer cover prima while = %f\n", c->layer_cover_subdominated);
					c->heights[height].ages[age].species[species].counter[N_TREE] -= 1;
					deadtree += 1;

					c->layer_cover_subdominated -= ((c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * 100.0) / settings->sizeCell)/100.0;
					//Log("layer cover dopo while = %f\n", c->layer_cover_subdominated);
					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]
					                                                                                                                               * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
				}
			}
			break;
		}

		if(c->heights[height].ages[age].species[species].counter[N_TREE] == 0)
		{
			//delete classes due to mortality
			Log("All trees of species %s dbh %g height %g age %d are died!!!!\n",
					c->heights[height].ages[age].species[species].name, c->heights[height].ages[age].species[species].value[AVDBH], c->heights[height].value, c->heights[height].ages[age].value);

			Log("reducing counter for died classes...........!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n");
			Log("heights_count = %d, ages_count = %d, species_count = %d \n", c->heights_count, c->heights[height].ages_count, c->heights[height].ages[age].species_count);
			//free (c->heights[height].ages[age].species[species].name);

			//todo it!!!
			//fixme in this case to BE CANCELLED is the last PROCESSED class BUT NOT NECESSARILY THE CLASS REALLY DIED
			if (c->heights[height].ages[age].species_count > 1)
				Log("Reducing species_count..\n");
				c->heights[height].ages[age].species_count --;
			//free (c->heights[height].ages[age].species);
			if(c->heights[height].ages_count > 1)
				Log("Reducing ages_count..\n");
				c->heights[height].ages_count --;
			//free (c->heights[height].ages);
			if (c->heights_count > 1)
				Log("Reducing heights_count..\n");
				c->heights_count --;


		}


		i = c->heights[height].z;

		c->layer_daily_dead_tree[i] += deadtree;
		c->layer_monthly_dead_tree[i] += deadtree;
		c->layer_annual_dead_tree[i] += deadtree;

		c->daily_dead_tree += deadtree;
		c->monthly_dead_tree += deadtree;
		c->annual_dead_tree += deadtree;


		c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]
		                                                                                                                               * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
		Log("Tree Removed for Crowding Competition from height class %f species %s dbh %f = %d trees\n", c->heights[height].value, c->heights[height].ages[age].species[species].name,
				c->heights[height].ages[age].species[species].value[AVDBH], deadtree);

		c->heights[height].ages[age].species[species].value[LEAF_C] -= (c->heights[height].ages[age].species[species].value[AV_LEAF_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[TOT_ROOT_C] -= (c->heights[height].ages[age].species[species].value[AV_ROOT_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[FINE_ROOT_C] -= (c->heights[height].ages[age].species[species].value[AV_FINE_ROOT_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[COARSE_ROOT_C] -= (c->heights[height].ages[age].species[species].value[AV_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[STEM_C] -= (c->heights[height].ages[age].species[species].value[AV_STEM_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[RESERVE_C] -= (c->heights[height].ages[age].species[species].value[AV_RESERVE_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[BRANCH_C] -= (c->heights[height].ages[age].species[species].value[AV_BRANCH_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C] -= (c->heights[height].ages[age].species[species].value[AV_LIVE_STEM_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[STEM_DEAD_WOOD_C] -= (c->heights[height].ages[age].species[species].value[AV_DEAD_STEM_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C] -= (c->heights[height].ages[age].species[species].value[AV_LIVE_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[COARSE_ROOT_DEAD_WOOD_C] -= (c->heights[height].ages[age].species[species].value[AV_DEAD_COARSE_ROOT_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C] -= (c->heights[height].ages[age].species[species].value[AV_LIVE_BRANCH_MASS_KgC]*1000.0*deadtree);
		c->heights[height].ages[age].species[species].value[BRANCH_DEAD_WOOD_C] -= (c->heights[height].ages[age].species[species].value[AV_DEAD_BRANCH_MASS_KgC]*1000.0*deadtree);

		Log("Number of Trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);




		//reset dead tree
		deadtree = 0;
	}
	//Layer coverage mortality for coppice
	//mortality occurs only for stools
	else
	{
		Log("Layer coverage mortality for coppice \n");

		//compute average biomass
		c->heights[height].ages[age].species[species].value[AV_STEM_MASS_KgDM] = c->heights[height].ages[age].species[species].value[BIOMASS_STEM_tDM]
														/ (double)c->heights[height].ages[age].species[species].counter[N_STUMP];
		//Log(" Av stump mass = %f tDM/tree\n", s->value[AV_STEM_MASS] );


		// levato
		//s->value[AV_ROOT_MASS] = s->value[AV_ROOT_MASS] = (s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_FINE_ROOT_tDM])/ (double)s->counter[N_TREE];
		//Log(" Av root mass = %f tDM/tree\n", s->value[AV_ROOT_MASS] );

		oldNstump = c->heights[height].ages[age].species[species].counter[N_STUMP];

		while (layer_cover >= settings->max_layer_cover )
		{
			c->heights[height].ages[age].species[species].counter[N_STUMP] -= 1;
			deadstump += 1;
			//layer_cover = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_STUMP] / settings->sizeCell;
			c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]
			                                                                                                                               * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;
		}
		oldNstump -= c->heights[height].ages[age].species[species].counter[N_STUMP];
		//s->value[BIOMASS_FOLIAGE] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);
		Log("Tot Root Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM] + c->heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM] );
		Log("Stump Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_tDM]);
		c->heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM] -= (c->heights[height].ages[age].species[species].value[AV_FINE_ROOT_MASS_KgDM] * 1000.0* deadstump);
		c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM] -= (c->heights[height].ages[age].species[species].value[AV_COARSE_ROOT_MASS_KgDM] * deadstump);
		c->heights[height].ages[age].species[species].value[BIOMASS_STEM_tDM] -= (c->heights[height].ages[age].species[species].value[AV_STEM_MASS_KgDM] * deadstump);
		Log("Tot Root Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM] + c->heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM] );
		Log("Stump Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_tDM]);
		Log("Number of Trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_STUMP]);
		Log("Tree Removed for Crowding Competition = %d trees\n", deadstump );
		Log("Canopy Cover in while = %f \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);

	}

}


/*Age mortality function from LPJ*/
//todo add to log results this function
void Age_Mortality (SPECIES *const s, AGE *const a)
{

	static int Dead_trees;




	//Age probability function
	s->value[AGEMORT] = (-(3 * log (0.001)) / (s->value[MAXAGE])) * pow (((double)a->value /s->value[MAXAGE]), 2);


	if ((s->counter[N_TREE] * s->value[AGEMORT]) > 1)
	{
		Log("**MORTALITY based on Tree Age (LPJ)**\n");
		Log("Age = %d years\n", a->value);
		Log("Age Mortality based on Tree Age (LPJ) = %f\n", s->value[AGEMORT]);
		Dead_trees = s->counter[N_TREE] * s->value[AGEMORT];
		Log("DEAD TREES = %d\n", Dead_trees);
		s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * Dead_trees * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
		s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * Dead_trees * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
		s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * Dead_trees * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
		//Log("Wf after dead = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE]);
		//Log("Wr after dead = %f tDM/ha\n", s->value[BIOMASS_ROOTS_TOT]);
		//Log("Ws after dead = %f tDM/ha\n", s->value[BIOMASS_STEM] );

		//----------------Number of trees after mortality---------------------

		s->counter[N_TREE] = s->counter[N_TREE] - Dead_trees;
		Log("Number of Trees  after age mortality = %d trees\n", s->counter[N_TREE]);

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
		Log("**NO-MORTALITY based on Tree Age (LPJ)**\n");
	}
	Log("**********************************\n");
}


//from LPJ Growth efficiency mortality
void Greff_Mortality (SPECIES *const s)
{
	static double greff;
	static double kmort1 = 0.02; //modified from original version
	static double kmort2 = 0.3;
	static double mortgreff;
	static int GreffDeadTrees;

	Log("**MORTALITY based on Growth Efficiency (LPJ)**\n");

	greff = (s->value[DEL_TOTAL_W] / (s->value[BIOMASS_FOLIAGE_tDM] * s->value[SLA_AVG]));
	//Log("greff from LPJ = %f\n", greff);
	//Log("DEL_TOTAL_W = %f\n", s->value[DEL_TOTAL_W]);
	//Log("WF= %f\n", s->value[BIOMASS_FOLIAGE]);

	mortgreff = kmort1 / (1 + kmort2 * greff);
	//Log("rate mort for greff from LPJ = %f\n", mortgreff);

	GreffDeadTrees = mortgreff * s->counter[N_TREE];

	Log("Dead trees for greff = %d\n", GreffDeadTrees);

	Log("Number of trees before greff mortality = %d trees/ha\n", s->counter[N_TREE]);

	s->counter[N_TREE] -= GreffDeadTrees;
	Log("Number of trees less greff mortality = %d trees/ha\n", s->counter[N_TREE]);

}




//in LPJ se CanCover supera 0.95 vengono uccisi tanti alberi finchè la CanCover non torna al massimo a 0.95


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

void Mortality (SPECIES *const s, int years)
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

		Log("MORTALITY based SELF-THINNING RULE\n");
		Log("Average Stem Mass > WSMax\n");
		Log("WS MAX = %f kgDM/tree\n",  s->value[WS_MAX]);
		Log("Average Stem Mass = %f kgDM stem /tree\n", s->value[AV_STEM_MASS_KgDM]);




		Log("Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);
		Log("Tree Stem Mass before Mortality Function = %f\n", s->value[BIOMASS_STEM_tDM]);

		n = (double)s->counter[N_TREE] / 1000;
		Log("n = %f\n", n);
		x1 = 1000 * s->value[MS] * s->value[BIOMASS_STEM_tDM] / (double)s->counter[N_TREE];
		Log("x1 = %f\n", x1);
		i = 0;
		while ( 1 )
		{
			i = i + 1;
			Log("i = %d\n", i);
			x2 = s->value[WSX1000] * pow (n, (1 - s->value[THINPOWER]));
			Log("X2 = %f\n", x2);
			fN = x2 - x1 * n - (1 - s->value[MS]) * s->value[BIOMASS_STEM_tDM];
			Log("fN = %f\n", fN);
			dfN = (1 - s->value[THINPOWER]) * x2 / n - x1;
			Log("dfN = %f\n", dfN);
			dN = -fN / dfN;
			Log("dN = %f\n", dN);
			n = n + dN;
			Log("n = %f\n", n);
			if ((fabs(dN) <= eps) || (i >= 5))
				break;
		}

		s->counter[DEAD_STEMS] = s->counter[N_TREE] - 1000 * n;
		Log("Dead Tree In Mortality Function = %d trees \n", s->counter[DEAD_STEMS]);

		//SERGIO CONTROL: if DEAD_STEMS < 0 set it to its minimum plausible value; that is 0
		if (s->counter[DEAD_STEMS] < 0)
		{
			s->counter[DEAD_STEMS]	 = 0;
		}
		//control
		if (s->counter[DEAD_STEMS] > s->counter[N_TREE])
		{
			Log("ERROR Number of Dead Trees > N Trees\n");
			Log("Dead Trees = %d\n", s->counter[DEAD_STEMS]);
			Log("Live Trees = %d\n", s->counter[N_TREE]);
		}
		else
		{
			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[DEAD_STEMS];
			Log("Number of Trees  after mortality = %d trees\n", s->counter[N_TREE]);
			s->value[BIOMASS_FOLIAGE_tDM] = s->value[BIOMASS_FOLIAGE_tDM] - s->value[MF] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_FOLIAGE_tDM] / s->counter[N_TREE]);
			s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_ROOTS_TOT_tDM] - s->value[MR] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_ROOTS_TOT_tDM] / s->counter[N_TREE]);
			s->value[BIOMASS_STEM_tDM] = s->value[BIOMASS_STEM_tDM] - s->value[MS] * s->counter[DEAD_STEMS] * (s->value[BIOMASS_STEM_tDM] / s->counter[N_TREE]);
			Log("Wf after dead = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE_tDM]);
			Log("Wr after dead = %f tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
			Log("Ws after dead = %f tDM/ha\n", s->value[BIOMASS_STEM_tDM]);
		}



		//----------------Number of trees after mortality---------------------




		//--------------------------------------------------------------------

		//deselected algorithm for 1Km^2 spatial resolution
		//s->value[WS_MAX] = s->value[WSX1000] * pow((1000 / (double)s->counter[N_TREE]), s->value[THINPOWER]);

		//modifified version for 1Km^2 spatial resolution
		s->value[WS_MAX] = s->value[WSX1000];

		Log("wSmax = %f KgDM/tree\n", s->value[WS_MAX]);
		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM] * 1000.0 / (double)s->counter[N_TREE];
		Log("AvStemMass after dead = %f Kg/tree\n", s->value[AV_STEM_MASS_KgDM]);
	}
	else
	{
		Log("NO MORTALITY based SELF-THINNING RULE\n");
		Log("Average Stem Mass < WSMax\n");
	}

	Log("**********************************\n");



}


void Stool_mortality (SPECIES *const s, int years)
{
	//to compute stools mortality in function on age only for coppices
	Log("***STOOLS MORTALITY*** \n");

}
