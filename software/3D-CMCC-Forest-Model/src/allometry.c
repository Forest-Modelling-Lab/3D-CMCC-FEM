/* allometry.c */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"

void Get_allometry (SPECIES *const, int);


void Get_allometry (SPECIES *const s, int years)
{
	float WFkgm2;

	Log("\n*********ALLOMETRY FUNCTION *********\n");
	/*
	   Log("-Chapman-Richards allometry function-\n");

	//tree height from champan-richards function

	if (!years)
	{
	s->value[INITIAL_TREE_HEIGHT_CR] = 1.3 + s->value[CRA] * pow (1 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
	Log("Initial Tree Height from Chapman-Richards function = %g m\n", s->value[INITIAL_TREE_HEIGHT_CR]);
	//control
	if (s->value[INITIAL_TREE_HEIGHT_CR] > s->value[HMAX])
	{
	s->value[INITIAL_TREE_HEIGHT_CR] = s->value[HMAX];
	Log("Initial Tree Height CR > HMAX !!  \n");
	Log("Initial Tree Height = HMAX  = %g m\n", s->value[INITIAL_TREE_HEIGHT_CR]);
	}

	}
	else
	{
	s->value[TREE_HEIGHT_CR] = 1.3 + s->value[CRA] * pow (1 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
	Log("Tree Height from Chapman-Richards function = %g m\n", s->value[TREE_HEIGHT_CR]);
	//control
	if (s->value[TREE_HEIGHT_CR] > s->value[HMAX])
	{
	s->value[TREE_HEIGHT_CR] = s->value[HMAX];
	Log("Tree Height CR > HMAX !!  \n");
	Log("Tree Height = HMAX  = %g m\n", s->value[TREE_HEIGHT_CR]);
	}
	}
	 */

	/*LPJ allometry function*/
	/*
	   Log("-LPJ-allometry function-\n");
	   if(!years)
	   {
	   s->value[TREE_HEIGHT_LPJ] = s->value[KALLOMB] * pow (( s->value[AVDBH] / 100) , s->value[KALLOMC] );
	// Based on inversion of Reinecke's Rule
	s->value[CROWN_AREA_LPJ] = s->value[KALLOMA] * pow (( s->value[AVDBH] / 100) , s->value[KRP] );

	}
	else
	{
	s->value[TREE_HEIGHT_LPJ] = s->value[KALLOMB] * pow (( s->value[AVDBH] / 100) , s->value[KALLOMC] );
	// Based on inversion of Reinecke's Rule
	s->value[CROWN_AREA_LPJ] = s->value[KALLOMA] * pow (( s->value[AVDBH] / 100) , s->value[KRP] );
	}

	Log("LPJ Height = %g m\n", s->value[TREE_HEIGHT_LPJ]);
	Log("LPJ Crown Area = %g m^2\n", s->value[CROWN_AREA_LPJ] );

	s->value[CROWN_DIAMETER_LPJ] = sqrt (s->value[CROWN_AREA_LPJ]  / ( Pi / 4) ) ;
	Log("LPJ Crown Diameter = %g m\n", s->value[CROWN_DIAMETER_LPJ]);


	s->value[CANOPY_COVER_LPJ] = s->counter[N_TREE] * s->value[CROWN_AREA_LPJ] / SIZECELL ;

	Log("LPJ Canopy Cover = %g m^2\n", s->value[CANOPY_COVER_LPJ]);
	Log("-------------------------\n");
	 */


	/*Sortie Allometry Function*/
	/*
	   Log("-SORTIE allometry function-\n");

	   if(!years)
	   {
	// Tree Height in m
	s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
	// Crown Radius in m
	s->value[CROWN_RADIUS_SORTIE] = s->value[RPOWER] * s->value[AVDBH] ;

	}
	else
	{
	// Tree Height in m
	s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
	// Crown Radius in m
	s->value[CROWN_RADIUS_SORTIE] = s->value[RPOWER] * s->value[AVDBH];
	}
	Log("Tree Height from Sortie Allometry Function = %g m\n", s->value[TREE_HEIGHT_SORTIE] );
	Log("Crown Radius from Sortie Allometry Function = %g m\n", s->value[CROWN_RADIUS_SORTIE]);

	// Crown Height in m
	s->value[CROWN_HEIGHT_SORTIE] = s->value[CHPOWER] * s->value[TREE_HEIGHT_SORTIE];
	Log("Crown Height from Sortie Allometry Function = %g m\n", s->value[CROWN_HEIGHT_SORTIE]);

	// Crown Diameter in m
	s->value[CROWN_DIAMETER_SORTIE] = s->value[CROWN_RADIUS_SORTIE] * 2;
	Log("Crown Diameter from Sortie Allometry Function = %g m\n", s->value[CROWN_DIAMETER_SORTIE]);
	Log("-------------------------\n");
	 */




	//la canopy cover totale deve essere = 1
	//deve essere la somma quindi di tutte le specie che compongono lo strato dominante!!!!!!!!!!!!!!!!!!!!!!!!!
	//anche se difficile che specie diverse abbiano esasttamente la stessa altezza per far parte dello stesso layer



	/*LAI*/
	/*
	   Get_Lai_3DVT ();

	   Get_Lai_3PG ();

	   Get_Lai_CTEM ();

	   Get_Lai_LPJ ();

	   Get_Lai_NASACASA ();
	 */


	//LAI 3D-CMCC

	//This function convert Biomass allocated in foliage compart from tDM/ha in KgDM/Area Effectively covered by Trees
	Log("**3D-CMCC Model LAI Function**\n");

	Log("Wf Foliage Biomass from previous year = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);
	Log("Canopy Cover in LOWER Layer = %g \n", s->value[CANOPY_COVER_DBHDC]);



	WFkgm2 = s->value[BIOMASS_FOLIAGE_CTEM]* (1000 / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell));
	Log("Foliage Biomass to LAI function = %g Kg/m^2\n", WFkgm2);
	Log("SLA-3D-CMCC Model = %g cm^2/g\n", s->value[SLA]);
	Log("SLA-3D-CMCC Model = %g m^2/Kg\n", SLAmkg);

	s->value[LAI] = WFkgm2  * s->value[SLAmkg] ;
	Log("LAI-3D-CMCC Forest Model = %g\n" , s->value[LAI]);

	//LAI 3PG
	/*
	Log("**3PG LAI Function**\n");

	//This function convert Biomass allocated in foliage compart from tDM/ha in KgDM/Area Effectively covered by Trees
	Log("Wf Foliage Biomass from previous year = %g tDM/ha\n", s->value[WF]);
	s->value[SLA] = s->value[SLA1] + (s->value[SLA0] - s->value[SLA1]) * exp (-ln2 * pow((s->counter[TREE_AGE]  / s->value[TSLA]), 2));
	Log("3PG Specific Leaf Area (SLA) = %g cm^2/g\n", s->value[SLA] );
	s->value[LAI_3PG] = s->value[WF] * s->value[SLA] * 0.1;
	Log("LAI-3PG  = %g \n", s->value[LAI_3PG] );
	//dovrei aggiungere la canopycover.....

	 */



	//LAI LPJ
	//using foliage biomass from 3PG function

	Log("**LPJ LAI Function**\n");


	Log("3D-CMCC Wf Foliage Biomass from previous year = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);

	//SLA from LPJ
	s->value[SLA_LPJ] = ((2 * pow (10 , (-4))) * ((exp (6.15) / (pow (12 , 0.46))))) * 10000;
	Log("Specific Leaf Area (SLA) from LPJ = %g cm^2/gC\n", s->value[SLA_LPJ]);
	Log("Specific Leaf Area (SLA) from LPJ = %g m^2/Kg\n", s->value[SLA_LPJ] * 0.1);
	s->value[LAI_LPJ] = ( s->value[SLA_LPJ] * 0.1) * (s->value[BIOMASS_FOLIAGE_CTEM] * (1000 / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))) / s->value[CROWN_AREA];
	Log("LAI-LPJ = %g\n", s->value[LAI_LPJ]);

	//LAI NASACASA
	//using foliage biomass from NASA CASA function


	Log("**Nasa Casa LAI Function**\n");
	Log("Nasa Casa Wf Foliage Biomass from previous year = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_NASA_CASA]);


	s->value[LAI_NASA_CASA] = s->value[BIOMASS_FOLIAGE_NASA_CASA] * (1000 / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)) * s->value[SLAmkg] ;
	Log("LAI-NASACASA = %g\n", s->value[LAI_NASA_CASA]);

	//LAI CTEM

	//using foliage biomass from CTEM function

	Log("**CTEM LAI Function**\n");
	Log("CTEM Wf Foliage Biomass from previous year = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);

	s->value[LAI_CTEM] = s->value[BIOMASS_FOLIAGE_CTEM] * (1000 / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)) * s->value[SLAmkg] ;
	Log("LAI-CTEM = %g\n", s->value[LAI_CTEM]);
}
