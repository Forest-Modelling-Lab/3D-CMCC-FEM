/*crowding competition.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"


// per determinare l'incremento di Height e DBH in funzione della densità di popolazione
// per l'Height utilizzo quella della Chapman-Richards Function


//(A. Collalti, 2009 not published)(J. H. M. Thornley, 1999, Annals of Botany)


void Get_crowding_competition (SPECIES *const s, int z, int years, int top_layer)
{

	static float delHeight;    //height increment
	static float delDBH;       //DBH increment
	static float oldWS;



	/* WS before allocation of delWS */
	oldWS = s->value[BIOMASS_STEM_CTEM] -  s->value[DEL_STEMS_CTEM];


	if ( !years )
	{
		s->value[CC_TREE_HEIGHT] = s->value[TREE_HEIGHT];
		s->value[CC_AVDBH] = s->value[AVDBH];
	}


	/*dominant layer with gap*/
	if (s->value[CANOPY_COVER_DBHDC] <  1 && z == top_layer)
	{
		Log("DOMINANT LAYER\n");
		Log("Canopy Cover < 100 ->  Low Crowding Competition!!\n");

		//Tree Dimension Under Low Crowding Competition

		// Compute and use H/D effective

		s->value[HD_EFF] = (( s->value[HDMAX] - s->value[HDMIN]) / (settings->max_layer_cover - settings->min_layer_cover)) *
				( s->value[CANOPY_COVER_DBHDC] - settings->min_layer_cover) + s->value[HDMIN];
		Log("H/D Ratio Effective = %g\n", s->value[HD_EFF]);


		// Compute DBH & Height

		// Compute Height increment
		delHeight = (s->value[HD_EFF] /((s->value[HD_EFF] / s->value[CC_TREE_HEIGHT]) + (2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS_CTEM];
		Log("Height Increment = %g m\n", delHeight);
		// Compute Height
		s->value[CC_TREE_HEIGHT] = s->value[CC_TREE_HEIGHT] + delHeight;
		Log("Height from CC Func = %g m\n", s->value[CC_TREE_HEIGHT]);

		/*control*/
		/*
		   if ( s->value[CC_TREE_HEIGHT] < s->value[TREE_HEIGHT] )
		   {
		   Log("ERROR IN H/D STEM PARTITIONING!! HEIGHT ERROR!!\n");
		   }
		 */

		// Compute DBH increment
		delDBH = ( 1 /((s->value[HD_EFF] / s->value[CC_TREE_HEIGHT]) +
				(2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS_CTEM];
		//Log("DBH Increment from CC Func  = %g m\n", delDBH);
		Log("DBH Increment from CC Func  = %g cm\n", delDBH * 100);
		// Compute DBH
		s->value[CC_AVDBH] = s->value[CC_AVDBH] + (delDBH * 100);
		Log("DBH from CC Func = %g cm\n", s->value[CC_AVDBH]);

		/*
		   if ( s->value[CC_AVDBH] < s->value[AVDBH] )
		   {
		   Log("ERROR IN H/D STEM PARTITIONING!! DBH ERROR!!\n");
		   }
		 */


	}
	/*dominant layer fully covered or dominated layer*/
	else
	{
		if ( z == top_layer)
		{
			Log("DOMINANT LAYER\n");
			Log("Canopy Cover exceed 100 ---> High Crowding Competition!!-NO GAPS!!\n");
		}
		else
		{
			Log("DOMINATED LAYER\n");
			Log("Crowding Competition Function use H/D Max!!\n");
		}

		//Tree Dimension Under High Crowding Competition

		// Use H/D MAX
		// Compute DBH & Height

		// Compute Height increment
		delHeight = (s->value[HDMAX] /((s->value[HDMAX] / s->value[CC_TREE_HEIGHT]) + (2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS_CTEM];
		//Log("oldWS = %g \n", oldWS);
		//Log("del WS = %g \n", s->value[DEL_STEMS_CTEM]);
		Log("Height Increment from CC Func  = %g m\n", delHeight);
		// Compute Height
		s->value[CC_TREE_HEIGHT] = s->value[CC_TREE_HEIGHT] + delHeight;
		Log("Height from CC Func  = %g m\n", s->value[CC_TREE_HEIGHT]);

		/*control*/
		/*
		   if ( s->value[CC_TREE_HEIGHT] < s->value[TREE_HEIGHT] )
		   {
		   Log("ERROR IN H/D STEM PARTITIONING!! HEIGHT ERROR!!\n");
		   }
		 */


		// Compute DBH increment

		s->value[HD_EFF] = (( s->value[HDMAX] - s->value[HDMIN]) / (settings->max_layer_cover - settings->min_layer_cover)) * ( s->value[CANOPY_COVER_DBHDC] - settings->min_layer_cover) + s->value[HDMIN];
		delDBH = ( 1 /((s->value[HD_EFF] / s->value[CC_TREE_HEIGHT]) + (2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS_CTEM];



		//Log("HD EFF = %g cm\n", s->value[HD_EFF]);
		//Log("DBH before Func  = %g cm\n", s->value[AVDBH]);
		Log("DBH Increment from CC Func  = %g m\n", delDBH);
		Log("DBH Increment from CC Func  = %g cm\n", delDBH * 100);
		// Compute DBH
		s->value[CC_AVDBH] = s->value[CC_AVDBH] + (delDBH * 100);
		Log("DBH from CC Func = %g cm\n", s->value[CC_AVDBH]);

		/*control*/
		/*
		   if ( s->value[CC_AVDBH] < s->value[AVDBH] )
		   {
		   Log("ERROR IN H/D STEM PARTITIONING!! DBH ERROR!!\n");
		   }
		 */

	}

}

