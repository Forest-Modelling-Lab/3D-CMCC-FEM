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

void Get_initialization_biomass_data (SPECIES *s, const YOS *const yos, const int years)
{
	//float sapwood_perc;

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

/*
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && s->value[LAI] == 0 && s->value[BIOMASS_FOLIAGE_CTEM] == 0)
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
	if (s->value[BIOMASS_STEM_CTEM]== 0 )
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
			Log("STEM POWER = %g\n", s->value[STEMPOWER_P]);
			Log("STEM CONST = %g\n", s->value[STEMCONST_P]);
			s->value[AV_STEM_MASS]  = s->value[STEMCONST_P] * pow (s->value[AVDBH], s->value[STEMPOWER_P]);
					//pow ((s->value[STEMCONST_P] * s->value[AVDBH]), s->value[STEMPOWER_P]);
		}

		//1000 to convert Kg into tons
		Log("-Individual stem biomass in Kg = %g\n", s->value[AV_STEM_MASS]);
		s->value[BIOMASS_STEM_CTEM] = s->value[AV_STEM_MASS] * s->counter[N_TREE] / 1000;
		Log("-Stem Biomass initialization data from DBH = %g \n", s->value[BIOMASS_STEM_CTEM]);
	}
	if (s->value[BIOMASS_STEM_BRANCH_CTEM]== 0 )
	{
		Log("\nNo Branch and Bark Data are available from model initialization\n"
				"Is the Stem biomass initial value with Branch and Bark?\n");
	}
	if(s->value[BIOMASS_ROOTS_COARSE_CTEM] == 0)
	{
		Log("\nNo Coarse root Biomass Data are available for model initialization \n");
		Log("...Generating input Coarse root Biomass biomass data from DBH data...\n");
		//compute coarse root biomass using root to shoot ratio
		s->value[BIOMASS_ROOTS_COARSE_CTEM]  = s->value[BIOMASS_STEM_CTEM] * s->value[COARSE_ROOT_STEM];
		Log("--Coarse Root Biomass initialization data from Stem Biomass = %g \n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);
	}


	s->value[BIOMASS_ROOTS_TOT_CTEM]= s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM];
	Log("---Total Root Biomass initialization data from Stem Biomass = %g \n", s->value[BIOMASS_ROOTS_TOT_CTEM]);


	s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2), 2)) * Pi);
	Log("   AvDBH = %g cm \n", s->value[AVDBH]);
	Log("   BASAL AREA = %g cm^2\n", s->value[BASAL_AREA]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
	Log("   SAPWOOD_AREA = %g cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	Log("   HEART_WOOD_AREA = %g cm^2\n", s->value[HEARTWOOD_AREA]);

/*
	float heart_wood_diameter, heart_wood_stem_mass;

	heart_wood_diameter = 2.0 * sqrt( s->value[HEARTWOOD_AREA]/Pi);
	Log("HEART_WOOD_diameter = %g cm\n", heart_wood_diameter);
	heart_wood_stem_mass = s->value[STEMCONST_P] * pow ((2.0 * sqrt( s->value[HEARTWOOD_AREA]/Pi)), s->value[STEMPOWER_P]);
	Log("HEART_WOOD_STEM MASS = %g Kg\n",  heart_wood_stem_mass);

	Log("HEART_WOOD_STEM PERC = %g \n", (heart_wood_stem_mass*100.0)/s->value[AV_STEM_MASS]);

	Log("SAP_WOOD_STEM PERC = %g \n",  100.0 - (heart_wood_stem_mass*100.0)/s->value[AV_STEM_MASS]);

*/




	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	Log("   sapwood perc = %g%\n", s->value[SAPWOOD_PERC]*100);
	Log("   Stem_biomass = %g class cell \n", s->value[BIOMASS_STEM_CTEM]);
	s->value[WS_sap] =  (s->value[BIOMASS_STEM_CTEM] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood stem biomass = %g tDM class cell \n", s->value[WS_sap]);
	s->value[WRC_sap] =  (s->value[BIOMASS_ROOTS_COARSE_CTEM] * s->value[SAPWOOD_PERC]);
	Log("   Sapwood coarse root biomass = %g tDM class cell \n", s->value[WRC_sap]);


	if (s->value[BIOMASS_RESERVE_CTEM] == 0 && s->value[BIOMASS_FOLIAGE_CTEM] == 0)
	{
		Log("\nNo Reserve Biomass Data are available for model initialization \n");
		Log("...Generating input Reserve Biomass biomass data\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002

		s->value[BIOMASS_RESERVE_CTEM]= s->value[WS_sap] * s->value[SAP_WRES];

		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			s->value[BIOMASS_FOLIAGE_CTEM] = 0;
		}
		else
		{
			s->value[BIOMASS_FOLIAGE_CTEM] =  s->value[BIOMASS_RESERVE_CTEM] * (1.0/s->value[STEM_LEAF]);
		}
		Log("----Foliage Biomass initialization data  = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
		Log("-----Reserve Biomass initialization data  = %g KgDM/tree\n", s->value[BIOMASS_RESERVE_CTEM]);
	}


	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS
	if (s->value[BIOMASS_ROOTS_FINE_CTEM] == 0 && (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		Log("\nNo Fine root Biomass Data are available for model initialization \n");
		Log("...Generating input Fine root Biomass biomass data from DBH data...\n");
		//compute fine root biomass setting by default to 0
		//todo questa è valida solo per le decidue
		s->value[BIOMASS_ROOTS_FINE_CTEM] = s->value[BIOMASS_FOLIAGE_CTEM];
		Log("---Fine Root Biomass initialization data from Stem Biomass = %g \n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
	}



}
