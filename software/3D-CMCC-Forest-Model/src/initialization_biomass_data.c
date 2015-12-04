/*
 * initialization_biomass_data.c

 *
 *  Created on: 31/ott/2012
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Get_initialization_biomass_data (SPECIES *s, HEIGHT *h, const int years)
{
	//double sapwood_perc;


	/*
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && s->value[LAI] == 0 && s->value[BIOMASS_FOLIAGE] == 0)
	{
		if (settings->spatial == 'u')
		{
			Log("MODEL RUN FOR EVERGREEN COULD NOT RUN WITHOUT INITIAL LAI VALUES!!!!!!!!!!!!!!!!!!!!\n");
			exit (1);
		}
	}
	 */

	Log("\n\n...checking initial biomass data...\n");

	/*check for initial biomass*/
	if (s->value[BIOMASS_STEM]== 0 )
	{
		Log("\nNo Stem Biomass Data are available for model initialization \n");
		Log("...Generating input Stem Biomass biomass data from DBH data...\n");
		//compute stem biomass from DBH
		if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
		{
			//use generic stemconst stempower values
			Log("..computing stem biomass from generic stempower and stemconst\n");
			if (s->value[AVDBH] < 9)
			{
				s->value[AV_STEM_MASS] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_A));
				//(pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_A)))*s->value[STEMCONST];
			}
			else if (s->value[AVDBH] > 9 && s->value[AVDBH] < 15)
			{
				s->value[AV_STEM_MASS] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_B));
				//(pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_B)))*s->value[STEMCONST];
			}
			else
			{
				s->value[AV_STEM_MASS] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_C));
				//(pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_C)))*s->value[STEMCONST];
			}
		}
		else
		{
			//use site specific stemconst stempower values
			Log("..computing stem biomass using site related stemconst and stempower\n");
			Log("STEM POWER = %f\n", s->value[STEMPOWER_P]);
			Log("STEM CONST = %f\n", s->value[STEMCONST_P]);
			s->value[AV_STEM_MASS]  = s->value[STEMCONST_P] * pow (s->value[AVDBH], s->value[STEMPOWER_P]);
			//pow ((s->value[STEMCONST_P] * s->value[AVDBH]), s->value[STEMPOWER_P]);
		}

		//1000 to convert Kg into tons
		//Log("-Individual stem biomass in Kg = %f\n", s->value[AV_STEM_MASS]);
		s->value[BIOMASS_STEM] = s->value[AV_STEM_MASS] * s->counter[N_TREE] / 1000;
		Log("-Stem Biomass initialization data from DBH = %f \n", s->value[BIOMASS_STEM]);
	}
	else
	{
		Log("Ok stem biomass..\n");
		Log("---Stem Biomass from init file = %f\n", s->value[BIOMASS_STEM]);
	}


	/*
	double heart_wood_diameter, heart_wood_stem_mass;

	heart_wood_diameter = 2.0 * sqrt( s->value[HEARTWOOD_AREA]/Pi);
	Log("HEART_WOOD_diameter = %f cm\n", heart_wood_diameter);
	heart_wood_stem_mass = s->value[STEMCONST_P] * pow ((2.0 * sqrt( s->value[HEARTWOOD_AREA]/Pi)), s->value[STEMPOWER_P]);
	Log("HEART_WOOD_STEM MASS = %f Kg\n",  heart_wood_stem_mass);

	Log("HEART_WOOD_STEM PERC = %f \n", (heart_wood_stem_mass*100.0)/s->value[AV_STEM_MASS]);

	Log("SAP_WOOD_STEM PERC = %f \n",  100.0 - (heart_wood_stem_mass*100.0)/s->value[AV_STEM_MASS]);

	 */



	if (s->value[BIOMASS_BRANCH]== 0 )
	{
		Log("\nNo Branch and Bark Data are available from model initialization\n"
				"Is the Stem biomass initial value with Branch and Bark?\n");
		if (s->value[FRACBB0] == 0)
		{
			Log("I don't have FRACBB0 = FRACBB1 \n");
			s->value[FRACBB0] = s->value[FRACBB1];
			Log("FRACBB0 = %f\n", s->value[FRACBB0]);
		}
		else
		{
			s->value[FRACBB] = s->value[FRACBB1]+ (s->value[FRACBB0]- s->value[FRACBB1])* exp(-ln2 * (h->value / s->value[TBB]));
			s->value[BIOMASS_BRANCH] = s->value[BIOMASS_STEM] * s->value[FRACBB];
			Log("-Stem Branch Biomass initialization data from DBH = %f \n", s->value[BIOMASS_BRANCH]);
		}

	}
	else
	{
		Log("Ok stem branch biomass..\n");
		Log("---Stem Branch Biomass from init file = %f\n", s->value[BIOMASS_BRANCH]);
	}

	/*computing total stem biomass*/
	s->value[BIOMASS_TOT_STEM] = s->value[BIOMASS_STEM] + s->value[BIOMASS_BRANCH];
	Log("--Total stem biomass (Ws + Wbb) = %f\n", s->value[BIOMASS_TOT_STEM]);



	if(s->value[BIOMASS_ROOTS_COARSE]== 0)
	{
		Log("\nNo Coarse root Biomass Data are available for model initialization \n");
		Log("...Generating input Coarse root Biomass biomass data from DBH data...\n");
		//compute coarse root biomass using root to shoot ratio
		s->value[BIOMASS_ROOTS_COARSE] = s->value[BIOMASS_STEM] * s->value[COARSE_ROOT_STEM];
		Log("--Coarse Root Biomass initialization data from Stem Biomass = %f \n", s->value[BIOMASS_ROOTS_COARSE]);
	}
	else
	{
		Log("Ok coarse root biomass..\n");
		Log("---Coarse Root Biomass from init file = %f \n", s->value[BIOMASS_ROOTS_COARSE]);
	}

	/*sapwood calculation*/
	Log("SAPWOOD CALCULATION using sapwood area\n");
	s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2), 2)) * Pi);
	//Log("   AvDBH = %f cm \n", s->value[AVDBH]);
	//Log("   BASAL AREA = %f cm^2\n", s->value[BASAL_AREA]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
	Log("   SAPWOOD_AREA = %f cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	Log("   HEART_WOOD_AREA = %f cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	Log("   sapwood perc = %f%%\n", s->value[SAPWOOD_PERC]*100);
	Log("   Stem_biomass = %f class cell \n", s->value[BIOMASS_STEM]);
	s->value[WS_sap] = (s->value[BIOMASS_STEM] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood stem biomass = %f tDM class cell \n", s->value[WS_sap]);
	s->value[WRC_sap] =  (s->value[BIOMASS_ROOTS_COARSE] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood coarse root biomass = %f tDM class cell \n", s->value[WRC_sap]);
	s->value[WBB_sap] = (s->value[BIOMASS_BRANCH] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood branch and bark biomass = %f tDM class cell \n", s->value[WBB_sap]);
	s->value[WTOT_sap] = s->value[WS_sap] + s->value[WRC_sap] + s->value[WBB_sap];
	Log("   Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap]);



	/*reserve*/

	if (s->value[BIOMASS_RESERVE] == 0)
	{
		Log("\nNo Reserve Biomass Data are available for model initialization \n");
		Log("...Generating input Reserve Biomass biomass data\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002

		s->value[BIOMASS_RESERVE] = s->value[WTOT_sap] * s->value[SAP_WRES];
		Log("-----Reserve Biomass initialization data  = %f tDM/cell \n", s->value[BIOMASS_RESERVE]);
		Log("-----Reserve Biomass initialization data  = %f KgC/cell \n", s->value[BIOMASS_RESERVE]/GC_GDM * 1000);
		Log("-----Reserve Biomass initialization data  = %f gC/tree \n", (s->value[BIOMASS_RESERVE]/GC_GDM * 1000)/s->value[N_TREE]);

	}
	else
	{
		Log("Ok reserve biomass..\n");
		Log("---Reserve from init file = %f \n", s->value[BIOMASS_RESERVE]);
	}
	if (s->value[BIOMASS_FOLIAGE] == 0)
	{
		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			/*nothing to do for deciduous*/
		}
		else
		{
			Log("\nNo Foliage Biomass Data are available for model initialization \n");
			Log("...Generating input Foliage Biomass biomass data\n");
			s->value[BIOMASS_FOLIAGE] =  s->value[BIOMASS_RESERVE] * (1.0 - s->value[STEM_LEAF_FRAC]);
			Log("----Foliage Biomass initialization data  = %f \n", s->value[BIOMASS_FOLIAGE]);
		}
	}
	else
	{
		Log("Ok foliage biomass..\n");
		Log("---Foliage Biomass from init file  = %f \n", s->value[BIOMASS_FOLIAGE]);
	}



	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	if (s->value[BIOMASS_ROOTS_FINE] == 0 && (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		Log("\nNo Fine root Biomass Data are available for model initialization \n");
		Log("...Generating input Fine root Biomass biomass data from DBH data...\n");
		//assuming FINE_ROOT_LEAF RATIO AS IN BIOME
		s->value[BIOMASS_ROOTS_FINE] = s->value[BIOMASS_FOLIAGE] * s->value[FINE_ROOT_LEAF];
		Log("---Fine Root Biomass initialization data from Stem Biomass = %f \n", s->value[BIOMASS_ROOTS_FINE]);
	}
	else
	{
		Log("Ok fine root biomass..\n");
		Log("---Fine Root Biomass from init file  = %f \n", s->value[BIOMASS_ROOTS_FINE]);
	}

	s->value[BIOMASS_ROOTS_TOT] = s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_ROOTS_FINE];
	Log("---Total Root Biomass = %f \n", s->value[BIOMASS_ROOTS_TOT]);


	/*COMPUTE BIOMASS LIVE WOOD*/
	//assuming LIVE_DEAD WOOD RATIO AS IN BIOME
	/*FOR STEM*/
	Log("\n*******************************\n");
	Log("Total Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM]);
	s->value[BIOMASS_STEM_LIVE_WOOD]= s->value[BIOMASS_STEM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
	s->value[BIOMASS_STEM_DEAD_WOOD]= s->value[BIOMASS_STEM] -s->value[BIOMASS_STEM_LIVE_WOOD];
	Log("-Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);

	/*FOR COARSE ROOT*/
	Log("Total Root Biomass = %f tDM/cell\n", s->value[BIOMASS_ROOTS_TOT]);
	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]= s->value[BIOMASS_ROOTS_COARSE] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Coarse Root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]= s->value[BIOMASS_ROOTS_COARSE] -s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD];
	Log("-Dead Coarse Root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);

	/*FOR BRANCH*/
	Log("Total BB Biomass = %f tDM/cell\n", s->value[BIOMASS_BRANCH]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]= s->value[BIOMASS_BRANCH] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]= s->value[BIOMASS_BRANCH] -s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD];
	Log("-Dead Stem Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


	s->value[BIOMASS_LIVE_WOOD] = s->value[BIOMASS_STEM_LIVE_WOOD]+
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]+
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]+
			s->value[BIOMASS_ROOTS_FINE]+
			s->value[BIOMASS_FOLIAGE];
	Log("---Live biomass following BIOME = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD]);
	Log("---Live biomass following BIOME = %f %% \n", (s->value[BIOMASS_LIVE_WOOD]*100.0)/ (s->value[BIOMASS_STEM]+
			s->value[BIOMASS_ROOTS_COARSE]+
			s->value[BIOMASS_BRANCH]+
			s->value[BIOMASS_ROOTS_FINE]+
			s->value[BIOMASS_FOLIAGE]));
	s->value[BIOMASS_DEAD_WOOD] = s->value[BIOMASS_STEM_DEAD_WOOD]+
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]+
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD];
	Log("---Dead biomass following BIOME = %f tDM/area\n", s->value[BIOMASS_DEAD_WOOD]);


	Log("***reserves following live tissues  BIOME = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD] * s->value[SAP_WRES] );


}
