/* allometry.c */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


//not used
void Get_allometry (SPECIES *const s, AGE *const a, int years)
{
	double WFkgm2;

	Log("\n*********ALLOMETRY FUNCTION *********\n");
	/*
	   Log("-Chapman-Richards allometry function-\n");

	//tree height from champan-richards function




	if (!years)
	{
	s->value[INITIAL_TREE_HEIGHT_CR] = 1.3 + s->value[CRA] * pow (1 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
	Log("Initial Tree Height from Chapman-Richards function = %f m\n", s->value[INITIAL_TREE_HEIGHT_CR]);
	//control
	if (s->value[INITIAL_TREE_HEIGHT_CR] > s->value[HMAX])
	{
	s->value[INITIAL_TREE_HEIGHT_CR] = s->value[HMAX];
	Log("Initial Tree Height CR > HMAX !!  \n");
	Log("Initial Tree Height = HMAX  = %f m\n", s->value[INITIAL_TREE_HEIGHT_CR]);
	}

	}
	else
	{
	s->value[TREE_HEIGHT_CR] = 1.3 + s->value[CRA] * pow (1 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
	Log("Tree Height from Chapman-Richards function = %f m\n", s->value[TREE_HEIGHT_CR]);
	//control
	if (s->value[TREE_HEIGHT_CR] > s->value[HMAX])
	{
	s->value[TREE_HEIGHT_CR] = s->value[HMAX];
	Log("Tree Height CR > HMAX !!  \n");
	Log("Tree Height = HMAX  = %f m\n", s->value[TREE_HEIGHT_CR]);
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

	Log("LPJ Height = %f m\n", s->value[TREE_HEIGHT_LPJ]);
	Log("LPJ Crown Area = %f m^2\n", s->value[CROWN_AREA_LPJ] );

	s->value[CROWN_DIAMETER_LPJ] = sqrt (s->value[CROWN_AREA_LPJ]  / ( Pi / 4) ) ;
	Log("LPJ Crown Diameter = %f m\n", s->value[CROWN_DIAMETER_LPJ]);


	s->value[CANOPY_COVER_LPJ] = s->counter[N_TREE] * s->value[CROWN_AREA_LPJ] / SIZECELL ;

	Log("LPJ Canopy Cover = %f m^2\n", s->value[CANOPY_COVER_LPJ]);
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
	s->value[CROWN_RADIUS_SORTIE] = s->value[RPOWER] * (s->value[AVDBH] * exp (b_RPOWER));
	}
	Log("Tree Height from Sortie Allometry Function = %f m\n", s->value[TREE_HEIGHT_SORTIE] );
	Log("Crown Radius from Sortie Allometry Function = %f m\n", s->value[CROWN_RADIUS_SORTIE]);

	// Crown Height in m
	s->value[CROWN_HEIGHT_SORTIE] = (s->value[CHPOWER] * (s->value[TREE_HEIGHT_SORTIE] * exp (b_CHPOWER));
	Log("Crown Height from Sortie Allometry Function = %f m\n", s->value[CROWN_HEIGHT_SORTIE]);

	// Crown Diameter in m
	s->value[CROWN_DIAMETER_SORTIE] = s->value[CROWN_RADIUS_SORTIE] * 2;
	Log("Crown Diameter from Sortie Allometry Function = %f m\n", s->value[CROWN_DIAMETER_SORTIE]);
	Log("-------------------------\n");
	 */




	//la canopy cover totale deve essere = 1
	//deve essere la somma quindi di tutte le specie che compongono lo strato dominante!!!!!!!!!!!!!!!!!!!!!!!!!
	//anche se difficile che specie diverse abbiano esasttamente la stessa altezza per far parte dello stesso layer





	//LAI 3D-CMCC

	//This function convert Biomass allocated in foliage compart from tDM/ha in KgDM/Area Effectively covered by Trees
	Log("**3D-CMCC Model LAI Function**\n");

	Log("Wf Foliage Biomass from previous year = %f tDM/ha\n", s->value[BIOMASS_FOLIAGE_CTEM]);
	Log("Canopy Cover in LOWER Layer = %f \n", s->value[CANOPY_COVER_DBHDC]);



	WFkgm2 = s->value[BIOMASS_FOLIAGE_CTEM]* (1000 / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell));
	Log("Foliage Biomass to LAI function = %f Kg/m^2\n", WFkgm2);
	Log("SLA-3D-CMCC Model = %f cm^2/g\n", s->value[SLA]);
	//Log("SLA-3D-CMCC Model = %f m^2/Kg\n", SLAmkg);

	s->value[LAI] = WFkgm2  * SLAMKG ;
	Log("LAI-3D-CMCC Forest Model = %f\n" , s->value[LAI]);


}
