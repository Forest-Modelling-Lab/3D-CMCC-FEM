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

void Initialization_biomass_data (SPECIES *s, HEIGHT *h)
{
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
	if (s->value[BIOMASS_STEM_tDM]== 0.0 )
	{
		Log("\nNo Stem Biomass Data are available for model initialization \n");
		Log("...Generating input Stem Biomass biomass data from DBH data...\n");
		//compute stem biomass from DBH
		if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
		{
			//use generic stemconst stempower values
			Log("..computing stem biomass from generic stempower and stemconst DBH = %f cm\n", s->value[AVDBH]);
			if (s->value[AVDBH] < 9)
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_A));
				//(pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_A)))*s->value[STEMCONST];
			}
			else if (s->value[AVDBH] > 9 && s->value[AVDBH] < 15)
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_B));
				//(pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_B)))*s->value[STEMCONST];
			}
			else
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_C));
				//(pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_C)))*s->value[STEMCONST];
			}
		}
		else
		{
			//use site specific stemconst stempower values
			Log("..computing stem biomass from generic stempower and stemconst DBH = %f cm\n", s->value[AVDBH]);
			//Log("STEM POWER = %f\n", s->value[STEMPOWER_P]);
			//Log("STEM CONST = %f\n", s->value[STEMCONST_P]);
			s->value[AV_STEM_MASS_KgDM]  = s->value[STEMCONST_P] * pow (s->value[AVDBH], s->value[STEMPOWER_P]);
			//pow ((s->value[STEMCONST_P] * s->value[AVDBH]), s->value[STEMPOWER_P]);
		}
		//presumibly dry matter
		Log("-Individual stem biomass = %f KgDM\n", s->value[AV_STEM_MASS_KgDM]);
		//1000 to convert Kg into tons
		s->value[BIOMASS_STEM_tDM] = s->value[AV_STEM_MASS_KgDM] * s->counter[N_TREE] / 1000;
		s->value[STEM_C] = (s->value[AV_STEM_MASS_KgDM] * s->counter[N_TREE] / 1000)/2.0;
		Log("-Class stem Biomass initialization data from DBH = %f tDM cell\n", s->value[BIOMASS_STEM_tDM]);
	}
	else
	{
		Log("Ok stem biomass..\n");
		Log("---Stem Biomass from init file = %f tDM/cell\n", s->value[BIOMASS_STEM_tDM]);
		s->value[STEM_C] = s->value[STEM_C] /2.0;
	}

	if (s->value[BIOMASS_BRANCH_tDM]== 0 )
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
			s->value[BIOMASS_BRANCH_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[FRACBB];
			s->value[BRANCH_C] = s->value[STEM_C] * s->value[FRACBB];
			Log("-Stem Branch Biomass initialization data from DBH = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
		}

	}
	else
	{
		Log("Ok stem branch biomass..\n");
		Log("---Stem Branch Biomass from init file = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
		s->value[BRANCH_C] = s->value[BIOMASS_BRANCH_tDM] /2.0;
	}

	/*computing total stem biomass*/
	s->value[BIOMASS_TOT_STEM_tDM] = s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_BRANCH_tDM];
	s->value[TOT_STEM_C] = s->value[STEM_C] + s->value[BRANCH_C];
	Log("--Class Total stem biomass (Ws + Wbb) = %f tDM/cell\n", s->value[BIOMASS_TOT_STEM_tDM]);



	if(s->value[BIOMASS_COARSE_ROOT_tDM]== 0)
	{
		Log("\nNo Coarse root Biomass Data are available for model initialization \n");
		Log("...Generating input Coarse root Biomass biomass data from DBH data...\n");
		//compute coarse root biomass using root to shoot ratio
		s->value[BIOMASS_COARSE_ROOT_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[COARSE_ROOT_STEM];
		s->value[COARSE_ROOT_C] = s->value[STEM_C] * s->value[COARSE_ROOT_STEM];
		Log("--Coarse Root Biomass initialization data from Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_tDM]);
	}
	else
	{
		Log("Ok coarse root biomass..\n");
		Log("---Coarse Root Biomass from init file = %ftDM/cell \n", s->value[BIOMASS_COARSE_ROOT_tDM]);
		s->value[COARSE_ROOT_C] = s->value[BIOMASS_COARSE_ROOT_tDM]/2.0;
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
	Log("   Stem_biomass = %f class cell \n", s->value[BIOMASS_STEM_tDM]);
	s->value[WS_sap] = (s->value[BIOMASS_STEM_tDM] * s->value[SAPWOOD_PERC]);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood stem biomass = %f tDM class cell \n", s->value[WS_sap]);
	s->value[WRC_sap] =  (s->value[BIOMASS_COARSE_ROOT_tDM] * s->value[SAPWOOD_PERC]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood coarse root biomass = %f tDM class cell \n", s->value[WRC_sap]);
	s->value[WBB_sap] = (s->value[BIOMASS_BRANCH_tDM] * s->value[SAPWOOD_PERC]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood branch and bark biomass = %f tDM class cell \n", s->value[WBB_sap]);
	s->value[WTOT_sap] = s->value[WS_sap] + s->value[WRC_sap] + s->value[WBB_sap];
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	Log("   Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap]);
	Log("   Total Sapwood biomass per tree = %f tDM tree \n", s->value[WTOT_sap]/s->counter[N_TREE]);
	Log("   Total Sapwood biomass per tree = %f KgDM tree \n", (s->value[WTOT_sap]/s->counter[N_TREE])*1000.0);
	Log("   Total Sapwood biomass per tree = %f gDM tree \n", (s->value[WTOT_sap]/s->counter[N_TREE])*1000000.0);
	Log("   Total Sapwood biomass per tree = %f gC tree \n", ((s->value[WTOT_sap]/s->counter[N_TREE])*1000000.0)/2.0);
	Log("   Total Sapwood biomass = %f tC class cell \n", s->value[TOT_SAPWOOD_C]);
	Log("   Total Sapwood biomass per tree = %f tC tree \n", s->value[TOT_SAPWOOD_C]/s->counter[N_TREE]);
	Log("   Total Sapwood biomass per tree = %f KgC tree \n", (s->value[TOT_SAPWOOD_C]/s->counter[N_TREE])*1000.0);
	Log("   Total Sapwood biomass per tree = %f gC tree \n", (s->value[TOT_SAPWOOD_C]/s->counter[N_TREE])*1000000.0);

	/*reserve*/

	if (s->value[RESERVE_tDM] == 0)
	{
		Log("\nNo Reserve Biomass Data are available for model initialization \n");
		Log("...Generating input Reserve Biomass biomass data\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)
		s->value[RESERVE_tDM] = s->value[WTOT_sap] * s->value[SAP_WRES];
		s->value[RESERVE_C]= s->value[TOT_SAPWOOD_C] * s->value[SAP_WRES];
		Log("-----Reserve Biomass initialization data  = %f tDM/cell \n", s->value[RESERVE_tDM]);
		Log("-----Reserve Biomass initialization data  = %f Kg res/cell \n", s->value[RESERVE_tDM]/GC_GDM * 1000);
		Log("-----Reserve Biomass initialization data  = %f g res/tree \n", (s->value[RESERVE_tDM]/GC_GDM * 1000000)/(int)s->counter[N_TREE]);
		Log("-----Reserve Biomass initialization data  = %f t res/cell \n", s->value[RESERVE_C]);
		Log("-----Reserve Biomass initialization data  = %f Kg res/cell \n", s->value[RESERVE_C] * 1000);
		Log("-----Reserve Biomass initialization data  = %f g res/tree \n", (s->value[RESERVE_C] * 1000000)/(int)s->counter[N_TREE]);

	}
	else
	{
		Log("Ok reserve biomass..\n");
		Log("---Reserve from init file = %f \n", s->value[RESERVE_tDM]);
	}
	if (s->value[BIOMASS_FOLIAGE_tDM] == 0)
	{
		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			/*nothing to do for deciduous*/
		}
		else
		{
			Log("\nNo Foliage Biomass Data are available for model initialization \n");
			Log("...Generating input Foliage Biomass biomass data\n");
			s->value[BIOMASS_FOLIAGE_tDM] =  s->value[RESERVE_tDM] * (1.0 - s->value[STEM_LEAF_FRAC]);
			s->value[LEAF_C] =  s->value[RESERVE_C] * (1.0 - s->value[STEM_LEAF_FRAC]);
			Log("----Foliage Biomass initialization data  = %f \n", s->value[BIOMASS_FOLIAGE_tDM]);
		}
	}
	else
	{
		s->value[LEAF_C] = s->value[BIOMASS_FOLIAGE_tDM]/2.0;
		Log("Ok foliage biomass..\n");
		Log("---Foliage Biomass from init file  = %f \n", s->value[BIOMASS_FOLIAGE_tDM]);
	}



	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	if (s->value[BIOMASS_FINE_ROOT_tDM] == 0 && (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		Log("\nNo Fine root Biomass Data are available for model initialization \n");
		Log("...Generating input Fine root Biomass biomass data from DBH data...\n");
		//assuming FINE_ROOT_LEAF RATIO AS IN BIOME
		s->value[BIOMASS_FINE_ROOT_tDM] = s->value[BIOMASS_FOLIAGE_tDM] * s->value[FINE_ROOT_LEAF];
		s->value[FINE_ROOT_C] = s->value[LEAF_C] * s->value[FINE_ROOT_LEAF];
		Log("---Fine Root Biomass initialization data from Stem Biomass = %f \n", s->value[BIOMASS_FINE_ROOT_tDM]);
	}
	else
	{
		s->value[FINE_ROOT_C] = s->value[BIOMASS_FINE_ROOT_tDM]/2.0;
		Log("Ok fine root biomass..\n");
		Log("---Fine Root Biomass from init file  = %f \n", s->value[BIOMASS_FINE_ROOT_tDM]);
	}

	s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM];
	s->value[ROOT_C] = s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	Log("---Total Root Biomass = %f \n", s->value[BIOMASS_ROOTS_TOT_tDM]);


	/*COMPUTE BIOMASS LIVE WOOD*/
	//assuming LIVE_DEAD WOOD RATIO AS IN BIOME
	/*FOR STEM*/
	Log("\n*******************************\n");
	Log("Total Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_tDM]);
	Log("Total Stem Biomass per tree = %f tDM/tree\n", s->value[BIOMASS_STEM_tDM]/ s->counter[N_TREE]);
	s->value[BIOMASS_STEM_LIVE_WOOD_tDM]= s->value[BIOMASS_STEM_tDM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C]= s->value[STEM_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
	s->value[BIOMASS_STEM_DEAD_WOOD_tDM]= s->value[BIOMASS_STEM_tDM] -s->value[BIOMASS_STEM_LIVE_WOOD_tDM];
	s->value[STEM_DEAD_WOOD_C]= s->value[STEM_C] -s->value[STEM_LIVE_WOOD_C];
	Log("-Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);

	/*FOR COARSE ROOT*/
	Log("Total Root Biomass = %f tDM/cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]= s->value[BIOMASS_COARSE_ROOT_tDM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C]= s->value[COARSE_ROOT_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Coarse Root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]= s->value[BIOMASS_COARSE_ROOT_tDM] -s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM];
	s->value[COARSE_ROOT_DEAD_WOOD_C]= s->value[COARSE_ROOT_C] -s->value[COARSE_ROOT_LIVE_WOOD_C];
	Log("-Dead Coarse Root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);

	/*FOR BRANCH*/
	Log("Total BB Biomass = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C]= s->value[BRANCH_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] -s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[BRANCH_DEAD_WOOD_C]= s->value[BRANCH_C] -s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	Log("-Dead Stem Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);


	s->value[BIOMASS_LIVE_WOOD_tDM] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM]+
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]
					 //FIXME
					 /*+
			s->value[BIOMASS_FINE_ROOT_tDM]+
			s->value[BIOMASS_FOLIAGE]*/;
	s->value[LIVE_WOOD_C] = s->value[STEM_LIVE_WOOD_C]+
				s->value[COARSE_ROOT_LIVE_WOOD_C]+
				s->value[BRANCH_LIVE_WOOD_C]
					//FIXME
					 /*+
				s->value[BIOMASS_FINE_ROOT_tDM]+
				s->value[BIOMASS_FOLIAGE]*/;
	Log("---Live biomass following BIOME = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD_tDM]);
	Log("---Live biomass following BIOME = %f tC/area\n", s->value[LIVE_WOOD_C]);

	s->value[BIOMASS_DEAD_WOOD_tDM] = s->value[BIOMASS_STEM_DEAD_WOOD_tDM]+
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM];
	s->value[DEAD_WOOD_C] = s->value[STEM_DEAD_WOOD_C]+
			s->value[COARSE_ROOT_DEAD_WOOD_C]+
			s->value[BRANCH_DEAD_WOOD_C];
	Log("---Dead biomass following BIOME = %f tDM/area\n", s->value[BIOMASS_DEAD_WOOD_tDM]);
	Log("---Dead biomass following BIOME = %f tC/area\n", s->value[DEAD_WOOD_C]);


	Log("***reserves following live tissues DM (not used) BIOME = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD_tDM] * s->value[SAP_WRES]);
	Log("***reserves following live tissues C (not used) BIOME = %f tC/area\n", s->value[LIVE_WOOD_C] * s->value[SAP_WRES] );


}
