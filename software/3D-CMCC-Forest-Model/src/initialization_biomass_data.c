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
	float sapwood_perc;

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;




	if(settings->spatial == 's')
	{
		Log("LAI from NDVI = %g\n", met[0].ndvi_lai);
	}

	if (s->value[BIOMASS_STEM_CTEM] == 0)
	{
		Log("No Stem Biomass Data are available for model initialization \n");
		Log("...Generating input Stem Biomass biomass data from DBH data...\n");
	}
	if (s->value[BIOMASS_ROOTS_COARSE_CTEM] == 0)
	{
		Log("No Coarse root Biomass Data are available for model initialization \n");
		Log("...Generating input Coarse root Biomass biomass data from DBH data...\n");
	}
	if (s->value[BIOMASS_ROOTS_FINE_CTEM] == 0)
	{
		Log("No Fine root Biomass Data are available for model initialization \n");
		Log("...Generating input Fine root Biomass biomass data from DBH data...\n");
	}
	if (s->value[BIOMASS_RESERVE_CTEM] == 0)
	{
		Log("No Reserve Biomass Data are available for model initialization \n");
		Log("...Generating input Reserve Biomass biomass data\n");
	}






	if (s->value[PHENOLOGY] == 1 && s->value[LAI] == 0 && s->value[BIOMASS_FOLIAGE_CTEM] == 0)
	{
		if (settings->spatial == 'u')
		{
			Log("MODEL RUN FOR EVERGREEN COULD NOT RUN WITHOUT INITIAL LAI VALUES!!!!!!!!!!!!!!!!!!!!\n");
			exit (1);
		}
	}

	//todo:
	/*
	if m->cells[cell].heights[height].ages[age].species[species].phenology == E
			&& m->cells[cell].heights[height].ages[age].species[species].value[LAI] == 0)
	{
		//compute lai
	}
	if m->cells[cell].heights[height].ages[age].species[species].phenology == E
			&& m->cells[cell].heights[height].ages[age].species[species].value[LAI] == 0)
	{
		//compute foliage biomass
	}
	 */


	if (s->value[BIOMASS_STEM_CTEM]== 0 && s->value[BIOMASS_ROOTS_FINE_CTEM]== 0 && s->value[BIOMASS_ROOTS_COARSE_CTEM]== 0)
	{

		//compute stem biomass from DBH

		if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
		{
			//use generic stemconst stempower values
			if (s->value[AVDBH] < 9)
			{
				s->value[AV_STEM_MASS]  = (pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_A)))*s->value[STEMCONST];
			}
			else if (s->value[AVDBH] > 9 && s->value[AVDBH] < 15)
			{
				s->value[AV_STEM_MASS]  = (pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_B)))*s->value[STEMCONST];
			}
			else
			{
				s->value[AV_STEM_MASS]  = (pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_C)))*s->value[STEMCONST];
			}
		}
		else
		{
			//use site specific stemconst stempower values
			s->value[AV_STEM_MASS]  = (pow (s->value[AVDBH], 1.0/(1.0/STEMPOWER_P)))*s->value[STEMCONST_P];
		}

		//1000 to convert Kg into tons
		s->value[BIOMASS_STEM_CTEM] = s->value[AV_STEM_MASS] * s->counter[N_TREE] / 1000;
		Log("-Stem Biomass initialization data from DBH = %g \n", s->value[BIOMASS_STEM_CTEM]);



		//compute coarse root biomass using root to shoot ratio
		s->value[BIOMASS_ROOTS_COARSE_CTEM]  = s->value[BIOMASS_STEM_CTEM] * s->value[COARSE_ROOT_STEM];
		Log("--Coarse Root Biomass initialization data from Stem Biomass = %g \n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

		//compute fine root biomass setting by default to 0
		//todo questa è valida solo per le decidue
		s->value[BIOMASS_ROOTS_FINE_CTEM] = 0;
		Log("---Fine Root Biomass initialization data from Stem Biomass = %g \n", s->value[BIOMASS_ROOTS_FINE_CTEM]);

		s->value[BIOMASS_ROOTS_TOT_CTEM]= s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM];
		Log("---Total Root Biomass initialization data from Stem Biomass = %g \n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
	}

	s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2), 2)) * Pi);
	Log("AvDBH = %g cm \n", s->value[AVDBH]);
	Log("BASAL AREA = %g cm^2\n", s->value[BASAL_AREA]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
	Log("SAPWOOD_AREA = %g cm^2\n", s->value[SAPWOOD_AREA]);
	sapwood_perc = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	Log("sapwood perc = %g%\n", sapwood_perc);
	s->value[WS_sap] =  (s->value[BIOMASS_STEM_CTEM] * sapwood_perc);
	Log("WS_SAP = %g tDM tree\n", s->value[WS_sap]);



	if (s->value[BIOMASS_RESERVE_CTEM] == 0)
	{
		Log("NO RESERVES FROM INITIALIZATION FILE...computing it\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002

		s->value[BIOMASS_RESERVE_CTEM]= s->value[WS_sap] * s->value[SAP_WRES];

		if (s->value[PHENOLOGY] == 0)
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



}
