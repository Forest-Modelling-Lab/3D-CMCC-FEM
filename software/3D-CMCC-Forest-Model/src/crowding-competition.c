/*crowding competition.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


// per determinare l'incremento di Height e DBH in funzione della densità di popolazione
// per l'Height utilizzo quella della Chapman-Richards Function


//(A. Collalti, 2009 not published)(J. H. M. Thornley, 1999, Annals of Botany)


// per determinare l'incremento di Height e DBH in funzione della densità di popolazione
//alta densità maggior altezza
//bassa densità maggior dbh
// in the first year avdbh and height are from input data


void Crowding_competition (SPECIES *const s, HEIGHT *h, int z, int years, int top_layer)
{

	static double delHeight;    //height increment
	static double delDBH;       //DBH increment
	static double oldWS;

	Log("GET CROWDING COMPETITION FUNCTION\n");

	/* WS before allocation of delWS */
	oldWS = s->value[BIOMASS_STEM_tDM] - s->value[DEL_STEMS];


	if ( !years )
	{
		s->value[CC_TREE_HEIGHT] = h->value;
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
		Log("H/D Ratio Effective = %f\n", s->value[HD_EFF]);


		// Compute DBH & Height

		// Compute Height increment
		delHeight = (s->value[HD_EFF] /((s->value[HD_EFF] / s->value[CC_TREE_HEIGHT]) + (2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS];
		Log("Height Increment = %f m\n", delHeight);
		// Compute Height
		s->value[CC_TREE_HEIGHT] = s->value[CC_TREE_HEIGHT] + delHeight;
		Log("Height from CC Func = %f m\n", s->value[CC_TREE_HEIGHT]);



		// Compute DBH increment
		delDBH = ( 1 /((s->value[HD_EFF] / s->value[CC_TREE_HEIGHT]) +
				(2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS];
		//Log("DBH Increment from CC Func  = %f m\n", delDBH);
		Log("DBH Increment from CC Func  = %f cm\n", delDBH * 100);
		// Compute DBH
		s->value[CC_AVDBH] = s->value[CC_AVDBH] + (delDBH * 100);
		Log("DBH from CC Func = %f cm\n", s->value[CC_AVDBH]);

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
		delHeight = (s->value[HDMAX] /((s->value[HDMAX] / s->value[CC_TREE_HEIGHT]) + (2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS];
		//Log("oldWS = %f \n", oldWS);
		//Log("del WS = %f \n", s->value[DEL_STEMS]);
		Log("Height Increment from CC Func  = %f m\n", delHeight);
		// Compute Height
		s->value[CC_TREE_HEIGHT] = s->value[CC_TREE_HEIGHT] + delHeight;
		Log("Height from CC Func  = %f m\n", s->value[CC_TREE_HEIGHT]);


		// Compute DBH increment

		s->value[HD_EFF] = (( s->value[HDMAX] - s->value[HDMIN]) / (settings->max_layer_cover - settings->min_layer_cover)) * ( s->value[CANOPY_COVER_DBHDC] - settings->min_layer_cover) + s->value[HDMIN];
		delDBH = ( 1 /((s->value[HD_EFF] / s->value[CC_TREE_HEIGHT]) + (2 / ( s->value[CC_AVDBH] / 100)))) * (1 / oldWS) * s->value[DEL_STEMS];



		//Log("HD EFF = %f cm\n", s->value[HD_EFF]);
		//Log("DBH before Func  = %f cm\n", s->value[AVDBH]);
		Log("DBH Increment from CC Func  = %f m\n", delDBH);
		Log("DBH Increment from CC Func  = %f cm\n", delDBH * 100);
		// Compute DBH
		s->value[CC_AVDBH] = s->value[CC_AVDBH] + (delDBH * 100);
		Log("DBH from CC Func = %f cm\n", s->value[CC_AVDBH]);

		/*control*/
		/*
		   if ( s->value[CC_AVDBH] < s->value[AVDBH] )
		   {
		   Log("ERROR IN H/D STEM PARTITIONING!! DBH ERROR!!\n");
		   }
		 */

	}

}

