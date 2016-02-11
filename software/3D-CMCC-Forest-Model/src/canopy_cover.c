/*canopy cover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"




double Get_canopy_cover (SPECIES *const s, int z, int years, int top_layer)
{
	//DBH-CrownDiameter Function
	//Effective ratio dbh-crown diameter in function of Density (Ntree/10000 m^2)
	static int oldNtree;
	static int deadtree;
	double DBHDCeffective;
	//double CrownDiameterDBHDC;
	double Av_stem_mass;
	double Av_root_mass;
	double Av_bb_mass;
	double Av_res_mass;
	double Av_foliage_mass;  //only for evergreen

	//it takes the approach of Purves et al., "Perfect Plasticity Approximation", see also Fisher et al., 2015 GMD


	Log("** CANOPY COVER DBH-DC Function FOR LAYER %d **\n", z);


	/*Density*/

	s->value[DENSITY] = (double)s->counter[N_TREE] / settings->sizeCell;
	Log("Tree Number = %d trees/area\n", s->counter[N_TREE]);
	Log("Density = %f trees/area\n", s->value[DENSITY]);


	/*DBH-DC Ratio effective*/
	//todo model should take into account also variations for dbh increments
	//todo invert dbhdc max with min
	DBHDCeffective = (( s->value[DBHDCMAX] - s->value[DBHDCMIN] ) / ( s->value[DENMAX] - s->value[DENMIN] ) * (s->value[DENSITY] - s->value[DENMIN] ) + s->value[DBHDCMAX]);
	Log("DBHDC effective to apply = %f\n", DBHDCeffective);


	if (DBHDCeffective > s->value[DBHDCMAX])
	{
		//Log("DBHDC effective for Dominant Layer > DBHDCMAX!!!\n");
		DBHDCeffective = s->value[DBHDCMAX];
		Log("DBHDC effective applied = %f\n", DBHDCeffective);
	}


	/*Crown Diameter using DBH-DC*/

	s->value[CROWN_DIAMETER_DBHDC_FUNC] = s->value[AVDBH] * DBHDCeffective;
	Log("-Crown Diameter from DBHDC function  = %f m\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);

	/*Crown Area using DBH-DC*/
	s->value[CROWN_AREA_DBHDC_FUNC] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER_DBHDC_FUNC], 2 );
	Log("-Crown Area from DBHDC function = %f m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);



	//todo add canopy depth to functions!!


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

	//mortality class related
	if (s->value[CANOPY_COVER_DBHDC] >= 1)
	{
		Log ("MORTALITY BASED ON HIGH CANOPY COVER!!!\n");

		//compute average biomass
		Av_stem_mass = s->value[BIOMASS_STEM] / (double)s->counter[N_TREE];
		Log(" Av stem mass = %f tDM/tree\n", Av_stem_mass );

		Av_root_mass = (s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_ROOTS_FINE]) / (double)s->counter[N_TREE];
		Log(" Av root mass = %f tDM/tree\n", Av_root_mass );

		Av_bb_mass = s->value[BIOMASS_BRANCH] / (double)s->counter[N_TREE];

		Av_res_mass = s->value[BIOMASS_RESERVE] / (double)s->counter[N_TREE];

		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
		{
			Av_foliage_mass = s->value[BIOMASS_FOLIAGE] / (double)s->counter[N_TREE];
		}

		oldNtree = s->counter[N_TREE];

		while ( s->value[CANOPY_COVER_DBHDC] >= 1 )
		{
			s->counter[N_TREE] -= 1;
			deadtree += 1;
			s->value[CANOPY_COVER_DBHDC] = s->value[CROWN_AREA_DBHDC_FUNC] * s->counter[N_TREE] / settings->sizeCell;
		}
		oldNtree -= s->counter[N_TREE];
		//s->value[BIOMASS_FOLIAGE] = s->value[WF] - s->value[MF] * s->counter[DEL_STEMS] * (s->value[WF] / s->counter[N_TREE]);
		Log("Tot Root Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_ROOTS_FINE] );
		Log("Stem Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_STEM] );


		s->value[BIOMASS_ROOTS_FINE] -= (Av_root_mass * deadtree);
		s->value[BIOMASS_ROOTS_COARSE] -= (Av_root_mass * deadtree);
		s->value[BIOMASS_STEM] -= (Av_root_mass * deadtree);
		s->value[BIOMASS_BRANCH] -= (Av_bb_mass * deadtree);
		s->value[BIOMASS_RESERVE] -= (Av_res_mass * deadtree);
		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
		{
			s->value[BIOMASS_FOLIAGE] -= (Av_foliage_mass * deadtree);
		}


		Log("Tot Root Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_ROOTS_FINE] );
		Log("Stem Biomass before reduction = %f tDM/tree\n", s->value[BIOMASS_STEM] );
		Log("Number of Trees = %d trees \n", s->counter[N_TREE]);
		Log("Tree Removed for Crowding Competition = %d trees\n", deadtree );
		Log("Canopy Cover in while = %f \n", s->value[CANOPY_COVER_DBHDC]);
	}

	return s->value[CANOPY_COVER_DBHDC];

	/*
	   Log("** CANOPY COVER 'P' Function **\n");


	//H/D Ratio effective

	s->value[HD_EFF] = s->value[TREE_HEIGHT_CR] / (s->value[AVDBH] / 100);

	Log("H/D effective ratio  = %f\n", s->value[HD_EFF] );



	// Prof PORTOGHESI
	// Diametro chioma = dbh * 18-20 / 100
	//Crown diameter in m
	//nel primo anno la crown diameter utilizza la Portoghesi relationship
	//DBHDC È IL RAPPORTO FISSO TRA DBH E CROWN DIAMETER

	//Crown Diameter

	s->value[CROWN_DIAMETER] = s->value[AVDBH] * s->value[DBHDC];
	s->value[CANOPY_COVER_P] = ((double)s->counter[N_TREE]  * (((Pi / 4) * (pow (s->value[CROWN_DIAMETER], 2))))) / SIZECELL;

	Log("Crown Diameter using Portoghesi relationship = %f m\n", s->value[CROWN_DIAMETER]);
	Log("Canopy Cover from 'Portoghesi'= %f %%\n", s->value[CANOPY_COVER_P] * 100 );


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
	Log("Free Canopy Cover from 'Portoghesi'= %f \n", s->value[FREE_CANOPY_COVER_P]);
	Log("Free Canopy Cover from 'Portoghesi'= %f %%\n", s->value[FREE_CANOPY_COVER_P] * 100);
	Log("---------------------------------------\n");
	 */
}
