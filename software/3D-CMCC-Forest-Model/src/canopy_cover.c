/*canopy cover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"




float Get_canopy_cover (SPECIES *const s, int z, int years, int top_layer)
{
	//DBH-CrownDiameter Function
	//Effective ratio dbh-crown diameter in function of Density (Ntree/10000 m^2)
	static int oldNtree;
	static int deadtree;
	float DBHDCeffective;
	//float CrownDiameterDBHDC;
	float Av_stem_mass;
	float Av_root_mass;





	Log("** CANOPY COVER DBH-DC Function FOR LAYER %d **\n", z);


	/*Density*/

	s->value[DENSITY] = (float)s->counter[N_TREE] / settings->sizeCell;
	Log("Tree Number = %d trees/area\n", s->counter[N_TREE]);
	Log("Density = %g trees/area\n", s->value[DENSITY]);


	/*DBH-DC Ratio effective*/
	DBHDCeffective = (( s->value[DBHDCMAX] - s->value[DBHDCMIN] ) / ( s->value[DENMAX] - s->value[DENMIN] ) * (s->value[DENSITY] - s->value[DENMIN] ) + s->value[DBHDCMIN]);
	Log("DBHDC effective to apply = %g\n", DBHDCeffective);


	if (DBHDCeffective > s->value[DBHDCMAX])
	{
		//Log("DBHDC effective for Dominant Layer > DBHDCMAX!!!\n");
		DBHDCeffective = s->value[DBHDCMAX];
		Log("DBHDC effective applied = %g\n", DBHDCeffective);
	}


	/*Crown Diameter using DBH-DC*/

	s->value[CROWN_DIAMETER_DBHDC_FUNC] = s->value[AVDBH] * DBHDCeffective;
	Log("-Crown Diameter from DBHDC function  = %g m\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);

	/*Crown Area using DBH-DC*/
	s->value[CROWN_AREA_DBHDC_FUNC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC_FUNC], 2 );
	Log("-Crown Area from DBHDC function = %g m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);


	/*Canopy Cover using DBH-DC*/

	s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE] / settings->sizeCell;


	/*
	   if (s->value[CANOPY_COVER_DBHDC] > 1)
	   {
	   s->value[CANOPY_COVER_DBHDC] = 1;
	//Log("Canopy Cover exceed 1 model reset to 1\n");
	//Log("No Free SPACE available!!!\n");
	}
	 */


	if (s->value[CANOPY_COVER_DBHDC] >= 1)
	{
		Log ("MORTALITY BASED ON HIGH CANOPY COVER!!!\n");

		//compute average biomass
		Av_stem_mass = s->value[BIOMASS_STEM_CTEM] / (float)s->counter[N_TREE];
		Log(" Av stem mass = %g tDM/tree\n", Av_stem_mass );

		Av_root_mass = (s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM]) / (float)s->counter[N_TREE];
		Log(" Av root mass = %g tDM/tree\n", Av_root_mass );

		oldNtree = s->counter[N_TREE];

		while ( s->value[CANOPY_COVER_DBHDC] >= 1 )
		{
			s->counter[N_TREE] -= 1;
			deadtree += 1;
			s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE] / settings->sizeCell;
		}
		oldNtree -= s->counter[N_TREE];
		//s->value[BIOMASS_FOLIAGE_CTEM] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);
		Log("Tot Root Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stem Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_STEM_CTEM] );
		s->value[BIOMASS_ROOTS_FINE_CTEM] -= (Av_root_mass * deadtree);
		s->value[BIOMASS_ROOTS_COARSE_CTEM] -= (Av_root_mass * deadtree);
		s->value[BIOMASS_STEM_CTEM] -= (Av_root_mass * deadtree);
		Log("Tot Root Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] );
		Log("Stem Biomass before reduction = %g tDM/tree\n", s->value[BIOMASS_STEM_CTEM] );
		Log("Number of Trees = %d trees \n", s->counter[N_TREE]);
		Log("Tree Removed for Crowding Competition = %d trees\n", deadtree );
		Log("Canopy Cover in while = %g \n", s->value[CANOPY_COVER_DBHDC]);
	}

	return s->value[CANOPY_COVER_DBHDC];

	/*
	   Log("** CANOPY COVER 'P' Function **\n");


	//H/D Ratio effective

	s->value[HD_EFF] = s->value[TREE_HEIGHT_CR] / (s->value[AVDBH] / 100);

	Log("H/D effective ratio  = %g\n", s->value[HD_EFF] );



	// Prof PORTOGHESI
	// Diametro chioma = dbh * 18-20 / 100
	//Crown diameter in m
	//nel primo anno la crown diameter utilizza la Portoghesi relationship
	//DBHDC È IL RAPPORTO FISSO TRA DBH E CROWN DIAMETER

	//Crown Diameter

	s->value[CROWN_DIAMETER] = s->value[AVDBH] * s->value[DBHDC];
	s->value[CANOPY_COVER_P] = ((float)s->counter[N_TREE]  * (((Pi / 4) * (pow (s->value[CROWN_DIAMETER], 2))))) / SIZECELL;

	Log("Crown Diameter using Portoghesi relationship = %g m\n", s->value[CROWN_DIAMETER]);
	Log("Canopy Cover from 'Portoghesi'= %g %%\n", s->value[CANOPY_COVER_P] * 100 );


	if (s->value[CANOPY_COVER_P] < 1)
	{
	Log("Canopy Cover < 100% No Crowding competition!!\n");
	}
	else if (s->value[CANOPY_COVER_P] == 1)
	{
	Log("Canopy Cover = 100%---> NO GAPS!!\n");
	}
	else
	{
	s->value[CANOPY_COVER_P] = 1;
	Log("Canopy Cover exceed 1---> Crowding Competition!!-NO GAPS!!\n");
	Log("Canopy Cover exceed 1--->CanopyCover forced to 1 \n");
	}
	//la canopy cover totale deve essere = 1
	//deve essere la somma quindi di tutte le specie che compongono lo strato dominante


	s->value[FREE_CANOPY_COVER_P] = 1 - s->value[CANOPY_COVER_P];
	Log("Free Canopy Cover from 'Portoghesi'= %g \n", s->value[FREE_CANOPY_COVER_P]);
	Log("Free Canopy Cover from 'Portoghesi'= %g %%\n", s->value[FREE_CANOPY_COVER_P] * 100);
	Log("---------------------------------------\n");
	 */
}
