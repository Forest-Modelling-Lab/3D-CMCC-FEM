/*mortality.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Get_layer_cover_mortality ( SPECIES *const s, CELL *const c, float layer_cover, int tree_number, int z, int height)
{
	//int oldNtree;
	int deadtree;
	int oldNstump;
	int deadstump;

	//the model makes die trees of the lower height class for that layer because
	//it passes through the function sort_by_height_desc the height classes starting from the lowest


	Log ("MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", z);

	Log ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CONTROL ROOT BIOMASS!!!\n");


	//Layer coverage mortality for timber
	//mortality occurs directly for timber
	if (s->management == T)
	{

		Log("Layer coverage mortality for timber \n");

		//compute average biomass
		s->value[AV_STEM_MASS] = s->value[BIOMASS_STEM_CTEM] / (float)s->counter[N_TREE];
		s->value[AV_FINE_ROOT_MASS] = s->value[BIOMASS_ROOTS_FINE_CTEM] / (float)s->counter[N_TREE];
		s->value[AV_COARSE_ROOT_MASS] = s->value[BIOMASS_ROOTS_COARSE_CTEM] / (float)s->counter[N_TREE];
		s->value[AV_RESERVE_BIOMASS] = s->value[BIOMASS_RESERVE_CTEM] / (float)s->counter[N_TREE];
		//Log(" Av stem mass = %g tDM/tree\n", s->value[AV_STEM_MASS] );

		Log("Tot Root Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stem Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_STEM_CTEM] );
		Log("Canopy Cover in while = %g \n", s->value[CANOPY_COVER_DBHDC]);


		// levato
		//s->value[AV_ROOT_MASS] = s->value[AV_ROOT_MASS] = (s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM])/ (float)s->counter[N_TREE];
		//Log(" Av root mass = %g tDM/tree\n", s->value[AV_ROOT_MASS] );

		//oldNtree = s->counter[N_TREE];

		while (layer_cover >= settings->max_layer_cover )
		{
			s->counter[N_TREE] -= 1;
			deadtree += 1;

			//todo in this case the model takes into account not NTREE of layer but just for class
			//insert a variable linked to cell for ntree
			layer_cover = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / settings->sizeCell;
			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / settings->sizeCell;
		}
		//oldNtree -= s->counter[N_TREE];
		//s->value[BIOMASS_FOLIAGE_CTEM] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);

		if (c->annual_layer_number == 1)
		{
			c->daily_dead_tree[0] += deadtree;
			c->monthly_dead_tree[0] += deadtree;
			c->annual_dead_tree[0] += deadtree;
		}
		if (c->annual_layer_number == 2)
		{
			if (c->heights[height].z == 1)
			{
				c->daily_dead_tree[1] += deadtree;
				c->monthly_dead_tree[1] += deadtree;
				c->annual_dead_tree[1] += deadtree;
			}
			else
			{
				c->daily_dead_tree[0] += deadtree;
				c->monthly_dead_tree[0] += deadtree;
				c->annual_dead_tree[0] += deadtree;
			}
		}
		if (c->annual_layer_number == 3)
		{
			if (c->heights[height].z == 2)
			{
				c->daily_dead_tree[2] += deadtree;
				c->monthly_dead_tree[2] += deadtree;
				c->annual_dead_tree[2] += deadtree;
			}
			if (c->heights[height].z == 1)
			{
				c->daily_dead_tree[1] += deadtree;
				c->monthly_dead_tree[1] += deadtree;
				c->annual_dead_tree[1] += deadtree;
			}
			if (c->heights[height].z == 0)
			{
				c->daily_dead_tree[0] += deadtree;
				c->monthly_dead_tree[0] += deadtree;
				c->annual_dead_tree[0] += deadtree;
			}
		}

		c->daily_tot_dead_tree += deadtree;
		c->monthly_tot_dead_tree += deadtree;
		c->annual_tot_dead_tree += deadtree;





		s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / settings->sizeCell;

		s->value[BIOMASS_ROOTS_FINE_CTEM] -= (s->value[AV_FINE_ROOT_MASS] * deadtree);
		s->value[BIOMASS_ROOTS_COARSE_CTEM] -= (s->value[AV_COARSE_ROOT_MASS] * deadtree);
		s->value[BIOMASS_STEM_CTEM] -= (s->value[AV_STEM_MASS] * deadtree);
		s->value[BIOMASS_RESERVE_CTEM] -= (s->value[AV_RESERVE_BIOMASS] * deadtree);
		Log("Tot Root Biomass after reduction = %g tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stem Biomass after reduction = %g tDM/tree\n", s->value[BIOMASS_STEM_CTEM] );
		Log("Number of Trees = %d trees \n", s->counter[N_TREE]);
		Log("Tree Removed for Crowding Competition = %d trees\n", deadtree );
		Log("Canopy Cover after while = %g \n", s->value[CANOPY_COVER_DBHDC]);

		//reset dead tree
		deadtree = 0;
	}
	//Layer coverage mortality for coppice
	//mortality occurs only for stools
	else
	{
		Log("Layer coverage mortality for coppice \n");

		//compute average biomass
		s->value[AV_STEM_MASS] = s->value[BIOMASS_STEM_CTEM] / (float)s->counter[N_STUMP];
		//Log(" Av stump mass = %g tDM/tree\n", s->value[AV_STEM_MASS] );


		// levato
		//s->value[AV_ROOT_MASS] = s->value[AV_ROOT_MASS] = (s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM])/ (float)s->counter[N_TREE];
		//Log(" Av root mass = %g tDM/tree\n", s->value[AV_ROOT_MASS] );

		oldNstump = s->counter[N_STUMP];

		while (layer_cover >= settings->max_layer_cover )
		{
			s->counter[N_STUMP] -= 1;
			deadstump += 1;
			//layer_cover = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_STUMP] / settings->sizeCell;
			s->value[CANOPY_COVER_DBHDC] = (s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE]) / settings->sizeCell;
		}
		oldNstump -= s->counter[N_STUMP];
		//s->value[BIOMASS_FOLIAGE_CTEM] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);
		Log("Tot Root Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stump Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_STEM_CTEM] );
		s->value[BIOMASS_ROOTS_FINE_CTEM] -= (s->value[AV_ROOT_MASS] * deadstump);
		s->value[BIOMASS_ROOTS_COARSE_CTEM] -= (s->value[AV_ROOT_MASS] * deadstump);
		s->value[BIOMASS_STEM_CTEM] -= (s->value[AV_STEM_MASS] * deadstump);
		Log("Tot Root Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stump Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_STEM_CTEM] );
		Log("Number of Trees = %d trees \n", s->counter[N_STUMP]);
		Log("Tree Removed for Crowding Competition = %d trees\n", deadstump );
		Log("Canopy Cover in while = %g \n", s->value[CANOPY_COVER_DBHDC]);

	}

}


/*Age mortality function from LPJ*/
//todo add to log results this function
void Get_Age_Mortality (SPECIES *const s, AGE *const a)
{

	static int Dead_trees;




	//Age probability function
	s->value[AGEMORT] = (-(3 * log (0.001)) / (s->value[MAXAGE])) * pow (((float)a->value /s->value[MAXAGE]), 2);


	if ((s->counter[N_TREE] * s->value[AGEMORT]) > 1)
	{
		Log("**MORTALITY based on Tree Age (LPJ)**\n");
		Log("Age = %d years\n", a->value);
		Log("Age Mortality based on Tree Age (LPJ) = %g\n", s->value[AGEMORT]);
		Dead_trees = s->counter[N_TREE] * s->value[AGEMORT];
		Log("DEAD TREES = %d\n", Dead_trees);
		s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MF] * Dead_trees * (s->value[BIOMASS_FOLIAGE_CTEM] / s->counter[N_TREE]);
		s->value[BIOMASS_ROOTS_TOT_CTEM] = s->value[BIOMASS_ROOTS_TOT_CTEM] - s->value[MR] * Dead_trees * (s->value[BIOMASS_ROOTS_TOT_CTEM] / s->counter[N_TREE]);
		s->value[BIOMASS_STEM_CTEM] = s->value[BIOMASS_STEM_CTEM] - s->value[MS] * Dead_trees * (s->value[BIOMASS_STEM_CTEM] / s->counter[N_TREE]);
		//Log("Wf after dead = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);
		//Log("Wr after dead = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
		//Log("Ws after dead = %g tDM/ha\n", s->value[BIOMASS_STEM_CTEM] );

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
	static float greff;
	static float kmort1 = 0.02; //modified from original version
	static float kmort2 = 0.3;
	static float mortgreff;
	static int GreffDeadTrees;

	greff = (s->value[DEL_TOTAL_W] / (s->value[BIOMASS_FOLIAGE_CTEM] * s->value[SLA]));
	//Log("greff from LPJ = %g\n", greff);
	//Log("DEL_TOTAL_W = %g\n", s->value[DEL_TOTAL_W]);
	//Log("WF= %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

	mortgreff = kmort1 / (1 + kmort2 * greff);
	//Log("rate mort for greff from LPJ = %g\n", mortgreff);

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
	float fN, dfN;
	float dN, n, x1, x2;


	//deselected algorithm for 1Km^2 spatial resolution
	/*m->cells[cell].heights[height].ages[age].species[species].value[WS_MAX] = m->cells[cell].heights[height].ages[age].species[species].value[WSX1000] *
        pow((1000 / (float)m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]),
                m->cells[cell].heights[height].ages[age].species[species].value[THINPOWER]);
	 */

	//modifified version for 1Km^2 spatial resolution
	s->value[WS_MAX] = s->value[WSX1000];



	if ( s->value[AV_STEM_MASS] > s->value[WS_MAX])
	{

		Log("MORTALITY based SELF-THINNING RULE\n");
		Log("Average Stem Mass > WSMax\n");
		Log("WS MAX = %g kgDM/tree\n",  s->value[WS_MAX]);
		Log("Average Stem Mass = %g kgDM stem /tree\n", s->value[AV_STEM_MASS]);




		Log("Tree Number before Mortality Function = %d\n", s->counter[N_TREE]);
		Log("Tree Stem Mass before Mortality Function = %g\n", s->value[BIOMASS_STEM_CTEM]);

		n = (float)s->counter[N_TREE] / 1000;
		Log("n = %g\n", n);
		x1 = 1000 * s->value[MS] * s->value[BIOMASS_STEM_CTEM] / (float)s->counter[N_TREE];
		Log("x1 = %g\n", x1);
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
			Log("Wf after dead = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);
			Log("Wr after dead = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
			Log("Ws after dead = %g tDM/ha\n", s->value[BIOMASS_STEM_CTEM] );
		}



		//----------------Number of trees after mortality---------------------




		//--------------------------------------------------------------------

		//deselected algorithm for 1Km^2 spatial resolution
		//s->value[WS_MAX] = s->value[WSX1000] * pow((1000 / (float)s->counter[N_TREE]), s->value[THINPOWER]);

		//modifified version for 1Km^2 spatial resolution
		s->value[WS_MAX] = s->value[WSX1000];

		Log("wSmax = %g KgDM/tree\n", s->value[WS_MAX]);
		s->value[AV_STEM_MASS] = s->value[BIOMASS_STEM_CTEM] * 1000 / (float)s->counter[N_TREE];
		Log("AvStemMass after dead = %g Kg/tree\n", s->value[AV_STEM_MASS]);
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
