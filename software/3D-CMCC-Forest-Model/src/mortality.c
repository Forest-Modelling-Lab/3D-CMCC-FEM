/*mortality.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_layer_cover_mortality (CELL *c, int height, int age, int species, double layer_cover, int tree_number)
{
	int i;
	//int oldNtree;
	int deadtree;
	int oldNstump;
	int deadstump;

	//the model makes die trees of the lower height class for that layer because
	//it passes through the function sort_by_height_desc the height classes starting from the lowest

	Log("*****GET_LAYER_COVER_MORTALITY*****\n");


	Log ("MORTALITY BASED ON HIGH CANOPY COVER layer %d, species %s height %f dbh %f !!!\n", c->heights[height].z, c->heights[height].ages[age].species[species].name,
			c->heights[height].value, c->heights[height].ages[age].species[species].value[AVDBH]);

	//Log ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CONTROL ROOT BIOMASS!!!\n");


	//Layer coverage mortality for timber
	//mortality occurs directly for timber
	if (c->heights[height].ages[age].species[species].management == T)
	{

		Log("Layer coverage mortality for timber \n");

		//compute average biomass
		c->heights[height].ages[age].species[species].value[AV_STEM_MASS] = c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]
		                                                                                                                        / (double)c->heights[height].ages[age].species[species].counter[N_TREE];
		c->heights[height].ages[age].species[species].value[AV_FINE_ROOT_MASS] = c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]
		                                                                                                                             / (double)c->heights[height].ages[age].species[species].counter[N_TREE];
		c->heights[height].ages[age].species[species].value[AV_COARSE_ROOT_MASS] = c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]
		                                                                                                                               / (double)c->heights[height].ages[age].species[species].counter[N_TREE];
		c->heights[height].ages[age].species[species].value[AV_RESERVE_BIOMASS] = c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]
		                                                                                                                              / (double)c->heights[height].ages[age].species[species].counter[N_TREE];
		//Log(" Av stem mass = %f tDM/tree\n", s->value[AV_STEM_MASS] );

		//Log("Tot Root Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]+ c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] );
		//Log("Stem Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] );



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
			while (c->layer_cover_dominant >= settings->max_layer_cover)
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
				while (c->layer_cover_dominant >= settings->max_layer_cover)
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
				while (c->layer_cover_dominated >= settings->max_layer_cover)
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
			if (c->layer_cover_dominant >= settings->max_layer_cover)
			{
				while (c->layer_cover_dominant >= settings->max_layer_cover)
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
				while (c->layer_cover_dominated >= settings->max_layer_cover)
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
				while (c->layer_cover_subdominated >= settings->max_layer_cover)
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


		i = c->heights[height].z;

		c->daily_dead_tree[i] += deadtree;
		c->monthly_dead_tree[i] += deadtree;
		c->annual_dead_tree[i] += deadtree;

		c->daily_tot_dead_tree += deadtree;
		c->monthly_tot_dead_tree += deadtree;
		c->annual_tot_dead_tree += deadtree;


		c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = (c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]
		                                                                                                                               * c->heights[height].ages[age].species[species].counter[N_TREE]) / settings->sizeCell;

		c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_FINE_ROOT_MASS] * deadtree);
		c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_COARSE_ROOT_MASS] * deadtree);
		c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_STEM_MASS] * deadtree);
		c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_RESERVE_BIOMASS] * deadtree);
		Log("Tot Root Biomass after reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]
		                                                                                                            + c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stem Biomass after reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] );
		Log("Number of Trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);
		Log("Tree Removed for Crowding Competition from height class %f species %s dbh %f = %d trees\n", c->heights[height].value, c->heights[height].ages[age].species[species].name,
				c->heights[height].ages[age].species[species].value[AVDBH], deadtree);



		//reset dead tree
		deadtree = 0;
	}
	//Layer coverage mortality for coppice
	//mortality occurs only for stools
	else
	{
		Log("Layer coverage mortality for coppice \n");

		//compute average biomass
		c->heights[height].ages[age].species[species].value[AV_STEM_MASS] = c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]
																			/ (double)c->heights[height].ages[age].species[species].counter[N_STUMP];
		//Log(" Av stump mass = %f tDM/tree\n", s->value[AV_STEM_MASS] );


		// levato
		//s->value[AV_ROOT_MASS] = s->value[AV_ROOT_MASS] = (s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM])/ (double)s->counter[N_TREE];
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
		//s->value[BIOMASS_FOLIAGE_CTEM] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);
		Log("Tot Root Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] + c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stump Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] );
		c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_ROOT_MASS] * deadstump);
		c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_ROOT_MASS] * deadstump);
		c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] -= (c->heights[height].ages[age].species[species].value[AV_STEM_MASS] * deadstump);
		Log("Tot Root Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] + c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stump Biomass before reduction = %f tDM/tree\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] );
		Log("Number of Trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_STUMP]);
		Log("Tree Removed for Crowding Competition = %d trees\n", deadstump );
		Log("Canopy Cover in while = %f \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);

	}

}


/*Age mortality function from LPJ*/
//todo add to log results this function
void Get_Age_Mortality (SPECIES *const s, AGE *const a)
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
		s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MF] * Dead_trees * (s->value[BIOMASS_FOLIAGE_CTEM] / s->counter[N_TREE]);
		s->value[BIOMASS_ROOTS_TOT_CTEM] = s->value[BIOMASS_ROOTS_TOT_CTEM] - s->value[MR] * Dead_trees * (s->value[BIOMASS_ROOTS_TOT_CTEM] / s->counter[N_TREE]);
		s->value[BIOMASS_STEM_CTEM] = s->value[BIOMASS_STEM_CTEM] - s->value[MS] * Dead_trees * (s->value[BIOMASS_STEM_CTEM] / s->counter[N_TREE]);
		//Log("Wf after dead = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);
		//Log("Wr after dead = %f tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
		//Log("Ws after dead = %f tDM/ha\n", s->value[BIOMASS_STEM_CTEM] );

		//----------------Number of trees after mortality---------------------

		s->counter[N_TREE] = s->counter[N_TREE] - Dead_trees;
		Log("Number of Trees  after age mortality = %d trees\n", s->counter[N_TREE]);

		if ( !s->counter[DEL_STEMS])
		{
			s->counter[DEL_STEMS] = Dead_trees;
		}
		else
		{
			s->counter[DEL_STEMS] += Dead_trees;
		}
	}
	else
	{
		Log("**NO-MORTALITY based on Tree Age (LPJ)**\n");
	}
	Log("**********************************\n");
}


//from LPJ Growth efficiency mortality
void Get_Greff_Mortality (SPECIES *const s)
{
	Log("**MORTALITY based on Growth Efficiency (LPJ)**\n");
	static double greff;
	static double kmort1 = 0.02; //modified from original version
	static double kmort2 = 0.3;
	static double mortgreff;
	static int GreffDeadTrees;

	greff = (s->value[DEL_TOTAL_W] / (s->value[BIOMASS_FOLIAGE_CTEM] * s->value[SLA]));
	//Log("greff from LPJ = %f\n", greff);
	//Log("DEL_TOTAL_W = %f\n", s->value[DEL_TOTAL_W]);
	//Log("WF= %f\n", s->value[BIOMASS_FOLIAGE_CTEM]);

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

void Get_Mortality (SPECIES *const s, int years)
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



	if ( s->value[AV_STEM_MASS] > s->value[WS_MAX])
	{

		Log("MORTALITY based SELF-THINNING RULE\n");
		Log("Average Stem Mass > WSMax\n");
		Log("WS MAX = %f kgDM/tree\n",  s->value[WS_MAX]);
		Log("Average Stem Mass = %f kgDM stem /tree\n", s->value[AV_STEM_MASS]);




		Log("Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);
		Log("Tree Stem Mass before Mortality Function = %f\n", s->value[BIOMASS_STEM_CTEM]);

		n = (double)s->counter[N_TREE] / 1000;
		Log("n = %f\n", n);
		x1 = 1000 * s->value[MS] * s->value[BIOMASS_STEM_CTEM] / (double)s->counter[N_TREE];
		Log("x1 = %f\n", x1);
		i = 0;
		while ( 1 )
		{
			i = i + 1;
			Log("i = %d\n", i);
			x2 = s->value[WSX1000] * pow (n, (1 - s->value[THINPOWER]));
			Log("X2 = %f\n", x2);
			fN = x2 - x1 * n - (1 - s->value[MS]) * s->value[BIOMASS_STEM_CTEM];
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

		s->counter[DEL_STEMS] = s->counter[N_TREE] - 1000 * n;
		Log("Dead Tree In Mortality Function = %d trees \n", s->counter[DEL_STEMS]);

		//SERGIO CONTROL: if del_stems < 0 set it to its minimum plausible value; that is 0
		if (s->counter[DEL_STEMS] < 0)
		{
			s->counter[DEL_STEMS]	 = 0;
		}
		//control
		if (s->counter[DEL_STEMS] > s->counter[N_TREE])
		{
			Log("ERROR Number of Dead Trees > N Trees\n");
			Log("Dead Trees = %d\n", s->counter[DEL_STEMS]);
			Log("Live Trees = %d\n", s->counter[N_TREE]);
		}
		else
		{
			s->counter[N_TREE] = s->counter[N_TREE] - s->counter[DEL_STEMS];
			Log("Number of Trees  after mortality = %d trees\n", s->counter[N_TREE]);
			s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[BIOMASS_FOLIAGE_CTEM] / s->counter[N_TREE]);
			s->value[BIOMASS_ROOTS_TOT_CTEM] = s->value[BIOMASS_ROOTS_TOT_CTEM] - s->value[MR] * s->counter[DEL_STEMS] * (s->value[BIOMASS_ROOTS_TOT_CTEM] / s->counter[N_TREE]);
			s->value[BIOMASS_STEM_CTEM] = s->value[BIOMASS_STEM_CTEM] - s->value[MS] * s->counter[DEL_STEMS] * (s->value[BIOMASS_STEM_CTEM] / s->counter[N_TREE]);
			Log("Wf after dead = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);
			Log("Wr after dead = %f tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
			Log("Ws after dead = %f tDM/ha\n", s->value[BIOMASS_STEM_CTEM] );
		}



		//----------------Number of trees after mortality---------------------




		//--------------------------------------------------------------------

		//deselected algorithm for 1Km^2 spatial resolution
		//s->value[WS_MAX] = s->value[WSX1000] * pow((1000 / (double)s->counter[N_TREE]), s->value[THINPOWER]);

		//modifified version for 1Km^2 spatial resolution
		s->value[WS_MAX] = s->value[WSX1000];

		Log("wSmax = %f KgDM/tree\n", s->value[WS_MAX]);
		s->value[AV_STEM_MASS] = s->value[BIOMASS_STEM_CTEM] * 1000 / (double)s->counter[N_TREE];
		Log("AvStemMass after dead = %f Kg/tree\n", s->value[AV_STEM_MASS]);
	}
	else
	{
		Log("NO MORTALITY based SELF-THINNING RULE\n");
		Log("Average Stem Mass < WSMax\n");
	}

	Log("**********************************\n");



}


void Get_stool_mortality (SPECIES *const s, int years)
{
	//to compute stools mortality in function on age only for coppices
	Log("***STOOLS MORTALITY*** \n");

}
