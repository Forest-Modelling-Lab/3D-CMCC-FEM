/*
 * new_class.c
 *
 *  Created on: 11/apr/2016
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include "types.h"
#include "constants.h"

int Create_new_class(CELL *const c, const int height, const int age, const int species)
{
	int i;
	int y;
	int z;
	int flag;
	HEIGHT *h;
	AGE *a;
	SPECIES *s;
	SPECIES *s2;

	Log("Creating new class....\n");
	Log("Replanted trees = %f\n", settings->replanted_tree);
	/* fill the row with the new variables */
	/* ist is used only with "human" regeneration */
	Log("Human management\n");

	// add height
	if (!alloc_struct((void **)&c->heights, &c->heights_count, sizeof(HEIGHT)) )
	{
		return 0;
	}
	h = &c->heights[c->heights_count-1];
	h->value = settings->height_sapling;
	
	/*

	ALESSIOC
	
	h->dominance
	h->layer_coverage
	h->layer
	h->dominance
	h->z
	*/

	/* NON TOCCARE ! */
	h->ages = NULL;
	h->ages_count = 0;
	
	// add age
	if ( !alloc_struct((void **)&h->ages, &h->ages_count, sizeof(AGE)) )
	{
		return 0;
	}
	a = &h->ages[h->ages_count-1];
	a->value = settings->age_sapling;
	a->species = NULL;
	a->species_count = 0;

	// add species
	if ( !alloc_struct((void **)&a->species, &a->species_count, sizeof(SPECIES)) )
	{
		return 0;
	}
	s = &a->species[a->species_count-1];
	s->name = mystrdup(settings->replanted_species);
	if ( ! s->name ) {
		Log("unable to copy replanted species from settins. out of memory.");
		return 0;
	}

	/* reset all values */
	for ( i = 0; i < VALUES; ++i ) {
		s->value[i] = INVALID_VALUE;
	}

	/* check number of species */
	flag = 0;
	for ( i = c->heights_count - 2; i >= 0 ; --i ) {
		/* check for same species */
		for ( y = 0; y < c->heights[i].ages_count; ++y ) {
			for ( z = 0; z < c->heights[y].ages[y].species_count; ++z ) {
				if ( ! mystricmp(c->heights[y].ages[y].species[z].name, s->name) ) {
					for ( i = 0; i < AVDBH; ++i ) {
						s->value[i] = c->heights[y].ages[y].species[z].value[i];
					}
					flag = 1;
					break;
				}
			}
			if ( flag ) break;
		}
		if ( flag ) break;
	}

	/* load from file */
	if ( ! flag ) {
		if ( ! fill_species_from_file(s) )
		{
			return 0;
		}
	}
	/* ALESSIOC fixme */
	s->value[DENMIN] = INVALID_VALUE;

	/* fill the structs with new variables incrementig counters */
	Log("heights_count = %d \n", c->heights_count);
	Log("ages_count = %d \n", h->ages_count);
	Log("species_count = %d \n", a->species_count);

	/* assign height value */
	Log("height_sapling = %f\n", h->value);
	/* assign age value */
	Log("age_sapling = %d\n", a->value);
	/* assign species name */
	
	/* assign dbh value */
	s->value[AVDBH] = settings->avdbh_sapling;
	Log("avdbh sampling = %f\n", s->value[AVDBH]);
	/* compute density */
	s->counter[N_TREE] = settings->replanted_tree;
	Log("n tree = %d\n", s->counter[N_TREE]);
	
	/* compute all other variables */
	/* stem biomass */
	s->value[AV_STEM_MASS_KgDM] =
			s->value[STEMCONST] *
			(pow (s->value[AVDBH], STEMPOWER_B));
	s->value[STEM_C] =
			s->value[AV_STEM_MASS_KgDM] *
			s->counter[N_TREE] / 1000.0;
	/* in tDM */
	s->value[BIOMASS_STEM_tDM] =
			s->value[AV_STEM_MASS_KgDM] *
			s->counter[N_TREE] / 1000.0 * GC_GDM;

	/* branch and bark biomass */
	s->value[FRACBB] =
			s->value[FRACBB1]+
			(s->value[FRACBB0]-
					s->value[FRACBB1])*
					exp(-ln2 * (c->heights[c->heights_count-1].value /
							s->value[TBB]));
	s->value[BRANCH_C] =
			s->value[STEM_C] *
			s->value[FRACBB];
	s->value[AV_BRANCH_MASS_KgDM] =
			s->value[BRANCH_C] *
			s->counter[N_TREE] * 1000.0;

	/* sapwood calculation */
	Log("\nSAPWOOD CALCULATION using sapwood area\n");
	s->value[BASAL_AREA] =
			((pow((s->value[AVDBH] / 2.0), 2.0)) * Pi);
	Log("   BASAL AREA = %f cm^2\n", s->value[BASAL_AREA]);

	s->value[SAPWOOD_AREA] =
			s->value[SAP_A] *
			pow (s->value[AVDBH],
					s->value[SAP_B]);
	Log("   SAPWOOD_AREA = %f cm^2\n", s->value[SAPWOOD_AREA]);

	s->value[HEARTWOOD_AREA] =
			s->value[BASAL_AREA] -
			s->value[SAPWOOD_AREA];
	Log("   HEART_WOOD_AREA = %f cm^2\n", s->value[HEARTWOOD_AREA]);

	s->value[SAPWOOD_PERC] =
			(s->value[SAPWOOD_AREA]) /
			s->value[BASAL_AREA];
	Log("   sapwood perc = %f%%\n", s->value[SAPWOOD_PERC]*100);

	s->value[WS_sap_tDM] =
			(s->value[BIOMASS_STEM_tDM] *
					s->value[SAPWOOD_PERC]);

	s->value[STEM_SAPWOOD_C] =
			(s->value[STEM_C] *
					s->value[SAPWOOD_PERC]);
	Log("   Sapwood stem biomass = %f tDM class cell \n", s->value[WS_sap_tDM]);
	Log("   Sapwood stem biomass = %f tC class cell \n", s->value[STEM_SAPWOOD_C]);

	s->value[WRC_sap_tDM] =
			(s->value[BIOMASS_COARSE_ROOT_tDM] *
					s->value[SAPWOOD_PERC]);
	s->value[COARSE_ROOT_SAPWOOD_C] =
			(s->value[COARSE_ROOT_C] *
					s->value[SAPWOOD_PERC]);
	Log("   Sapwood coarse root biomass = %f tDM class cell \n", s->value[WRC_sap_tDM]);
	Log("   Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);

	s->value[WBB_sap_tDM] =
			(s->value[BIOMASS_BRANCH_tDM] *
					s->value[SAPWOOD_PERC]);
	s->value[BRANCH_SAPWOOD_C] =
			(s->value[BRANCH_C] *
					s->value[SAPWOOD_PERC]);
	Log("   Sapwood branch and bark biomass = %f tDM class cell \n", s->value[WBB_sap_tDM]);
	Log("   Sapwood branch and bark biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);

	s->value[WTOT_sap_tDM] =
			s->value[WS_sap_tDM] +
			s->value[WRC_sap_tDM] + s->value[WBB_sap_tDM];
	s->value[TOT_SAPWOOD_C] =
			s->value[STEM_SAPWOOD_C] +
			s->value[COARSE_ROOT_SAPWOOD_C] +
			s->value[BRANCH_SAPWOOD_C];
	Log("   Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap_tDM]);
	Log("   Total Sapwood biomass per tree = %f tDM tree \n", s->value[WTOT_sap_tDM]/
			s->counter[N_TREE]);
	Log("   Total Sapwood biomass per tree = %f KgDM tree \n", (s->value[WTOT_sap_tDM]/
			s->counter[N_TREE])*1000.0);
	Log("   Total Sapwood biomass per tree = %f gDM tree \n", (s->value[WTOT_sap_tDM]/
			s->counter[N_TREE])*1000000.0);
	Log("   Total Sapwood biomass = %f tC class cell \n", s->value[TOT_SAPWOOD_C]);
	Log("   Total Sapwood biomass per tree = %f tC tree \n", s->value[TOT_SAPWOOD_C]/
			s->counter[N_TREE]);
	Log("   Total Sapwood biomass per tree = %f KgC tree \n", (s->value[TOT_SAPWOOD_C]/
			s->counter[N_TREE])*1000.0);
	Log("   Total Sapwood biomass per tree = %f gC tree \n", (s->value[TOT_SAPWOOD_C]/
			s->counter[N_TREE])*1000000.0);

	/* reserve */
	Log("\nNo Reserve Biomass Data are available for model initialization \n");
	Log("...Generating input Reserve Biomass biomass data\n");
	//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
	//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

	/* IMPORTANT! reserve computation if not in init data are computed from DM */
	s->value[RESERVE_tDM] =
			s->value[WTOT_sap_tDM] *
			s->value[SAP_WRES];
	//fixme
	s->value[RESERVE_C]=
			s->value[WTOT_sap_tDM] *
			s->value[SAP_WRES];

	Log("\n!!!!RESERVE ARE COMPUTED AS A COSTANT FRACTION OF SAPWOOD IN DRYMATTER!!!!\n");
	Log("-----Reserve initialization data  = %f tDM/cell \n", s->value[RESERVE_tDM]);
	Log("-----Reserve initialization data  = %f Kg res/cell \n", s->value[RESERVE_tDM] * 1000);
	Log("-----Reserve initialization data  = %f g res/tree \n", (s->value[RESERVE_tDM] * 1000000)/
			(int)s->counter[N_TREE]);
	Log("-----Reserve initialization data  = %f t res/cell \n", s->value[RESERVE_C]);
	Log("-----Reserve initialization data  = %f Kg res/cell \n", s->value[RESERVE_C] * 1000);
	Log("-----Reserve initialization data  = %f g res/tree \n", (s->value[RESERVE_C] * 1000000)/
			(int)s->counter[N_TREE]);

	s->value[AV_RESERVE_MASS_KgDM] =
			s->value[RESERVE_tDM] *
			1000.0 /s->counter[N_TREE];
	s->value[AV_RESERVE_MASS_KgC] = s->value[RESERVE_C] *1000.0 /s->counter[N_TREE];
	Log("-Individual reserve = %f KgDM/tree\n", s->value[AV_RESERVE_MASS_KgDM]);
	Log("-Individual reserve = %f Kg/tree\n", s->value[AV_RESERVE_MASS_KgC]);
	/* compute minimum reserve pool */
	s->value[MIN_RESERVE_C] = s->value[RESERVE_C];
	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C]*
			1000.0 /s->counter[N_TREE];
	Log("-Minimum reserve  = %f tres/tree\n", s->value[MIN_RESERVE_C]);
	Log("-Individual minimum reserve = %f KgC/tree\n", s->value[AV_MIN_RESERVE_KgC]);

	/* leaf */
	if (s->value[BIOMASS_FOLIAGE_tDM] == 0.0)
	{
		if (s->value[PHENOLOGY] == 0.1 ||
				s->value[PHENOLOGY] == 0.2)
		{
			/* assuming no leaf at 1st of January */
			s->value[BIOMASS_FOLIAGE_tDM] = 0.0;
			s->value[LEAF_C] = 0.0;
		}
		else
		{
			Log("\nNo Foliage Biomass Data are available for model initialization \n");
			Log("...Generating input Foliage Biomass biomass data\n");
			//fixme it seems to not have sense
			s->value[BIOMASS_FOLIAGE_tDM] =
					s->value[RESERVE_tDM] *
					(1.0 - s->value[STEM_LEAF_FRAC]);
			s->value[LEAF_C] = s->value[RESERVE_C] *
					(1.0 - s->value[STEM_LEAF_FRAC]);
			Log("----Foliage Biomass initialization data  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
			Log("----Foliage Biomass initialization data  = %f tC cell\n", s->value[LEAF_C]);
		}
	}
	else
	{
		s->value[LEAF_C] = s->value[BIOMASS_FOLIAGE_tDM]/GC_GDM;
		Log("Ok foliage biomass..\n");
		Log("---Foliage Biomass from init file  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
		Log("---Foliage Biomass from init file  = %f tC cell\n", s->value[LEAF_C]);
	}
	s->value[AV_FOLIAGE_MASS_KgDM] =
			s->value[BIOMASS_FOLIAGE_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_LEAF_MASS_KgC] =
			s->value[LEAF_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual foliage biomass = %f KgDM\n", s->value[AV_FOLIAGE_MASS_KgDM]);
	Log("-Individual foliage biomass = %f KgC\n", s->value[AV_LEAF_MASS_KgC]);


	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	/* fine root biomass */
	if (s->value[BIOMASS_FINE_ROOT_tDM] == 0.0 &&
			(s->value[PHENOLOGY] == 1.1 ||
					s->value[PHENOLOGY] == 1.2))
	{
		Log("\nNo Fine root Biomass Data are available for model initialization \n");
		Log("...Generating input Fine root Biomass biomass data from DBH data...\n");
		//assuming FINE_ROOT_LEAF RATIO AS IN BIOME
		s->value[BIOMASS_FINE_ROOT_tDM] =
				s->value[BIOMASS_FOLIAGE_tDM] *
				s->value[FINE_ROOT_LEAF];
		s->value[FINE_ROOT_C] =
				s->value[LEAF_C] *
				s->value[FINE_ROOT_LEAF];
		Log("---Fine Root Biomass initialization data from Stem Biomass = %f tDM cell\n", s->value[BIOMASS_FINE_ROOT_tDM]);
		Log("---Fine Root Biomass initialization data from Stem Biomass = %f tC cell\n", s->value[FINE_ROOT_C]);
	}
	else
	{
		/* assuming no fine root at 1st of January */
		s->value[BIOMASS_FINE_ROOT_tDM] = 0.0;
		s->value[FINE_ROOT_C] = 0.0;
		Log("Ok fine root biomass..\n");
		Log("---Fine Root Biomass from init file  = %f tDM cell\n", s->value[BIOMASS_FINE_ROOT_tDM]);
		Log("---Fine Root Biomass from init file  = %f tC cell\n", s->value[FINE_ROOT_C]);
	}
	s->value[AV_FINE_ROOT_MASS_KgDM] =
			s->value[BIOMASS_FINE_ROOT_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_FINE_ROOT_MASS_KgC] =
			s->value[FINE_ROOT_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual fine root biomass = %f KgDM\n", s->value[AV_FINE_ROOT_MASS_KgDM]);
	Log("-Individual fine root biomass = %f KgC\n", s->value[AV_FINE_ROOT_MASS_KgC]);
	s->value[BIOMASS_ROOTS_TOT_tDM] =
			s->value[BIOMASS_COARSE_ROOT_tDM] +
			s->value[BIOMASS_FINE_ROOT_tDM];
	s->value[TOT_ROOT_C] =
			s->value[COARSE_ROOT_C] +
			s->value[FINE_ROOT_C];
	Log("---Total Root Biomass = %f tDM cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	Log("---Total Root Biomass = %f tC cell\n", s->value[TOT_ROOT_C]);
	s->value[AV_ROOT_MASS_KgDM] =
			s->value[BIOMASS_ROOTS_TOT_tDM] * 1000.0 /
			s->counter[N_TREE];
	s->value[AV_ROOT_MASS_KgC] =
			s->value[TOT_ROOT_C] * 1000.0 /
			s->counter[N_TREE];
	CHECK_CONDITION(fabs((s->value[TOT_ROOT_C])-
			(s->value[COARSE_ROOT_C] +
					s->value[FINE_ROOT_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_ROOT_MASS_KgC])-
			(s->value[AV_COARSE_ROOT_MASS_KgC] +
					s->value[AV_FINE_ROOT_MASS_KgC])), >1e-4);

	/* compute biomass live wood */
	//assuming LIVE_DEAD WOOD RATIO AS IN BIOME
	Log("\n*******************************\n");
	/*FOR STEM*/
	Log("Total Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_tDM]);
	Log("Total Stem Biomass = %f tC cell\n", s->value[STEM_C]);
	s->value[BIOMASS_STEM_LIVE_WOOD_tDM]=
			s->value[BIOMASS_STEM_tDM] *
			(s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C]=
			s->value[STEM_C] *
			(s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
	Log("-Live Stem Biomass = %f tC cell\n", s->value[STEM_LIVE_WOOD_C]);
	s->value[BIOMASS_STEM_DEAD_WOOD_tDM]=
			s->value[BIOMASS_STEM_tDM] -
			s->value[BIOMASS_STEM_LIVE_WOOD_tDM];
	s->value[STEM_DEAD_WOOD_C]=
			s->value[STEM_C] -
			s->value[STEM_LIVE_WOOD_C];
	Log("-Dead Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
	Log("-Dead Stem Biomass = %f tC cell\n", s->value[STEM_DEAD_WOOD_C]);
	s->value[AV_LIVE_STEM_MASS_KgDM] =
			s->value[BIOMASS_STEM_LIVE_WOOD_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_LIVE_STEM_MASS_KgC] =
			s->value[STEM_LIVE_WOOD_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual live wood biomass = %f KgDM\n", s->value[AV_LIVE_STEM_MASS_KgDM]);
	Log("-Individual live wood biomass = %f KgC\n", s->value[AV_LIVE_STEM_MASS_KgC]);
	s->value[AV_DEAD_STEM_MASS_KgDM] =
			s->value[BIOMASS_STEM_DEAD_WOOD_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_DEAD_STEM_MASS_KgC] =
			s->value[STEM_DEAD_WOOD_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual dead wood biomass = %f KgDM\n", s->value[AV_DEAD_STEM_MASS_KgDM]);
	Log("-Individual dead wood biomass = %f KgC\n", s->value[AV_DEAD_STEM_MASS_KgC]);
	CHECK_CONDITION(fabs((s->value[STEM_C])-
			(s->value[STEM_LIVE_WOOD_C] +
					s->value[STEM_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_STEM_MASS_KgC])-
			(s->value[AV_LIVE_STEM_MASS_KgC] +
					s->value[AV_DEAD_STEM_MASS_KgC])), >1e-4);

	/* corase live wood */
	Log("Total Root Biomass = %f tDM cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	Log("Total Root Biomass = %f tC cell\n", s->value[TOT_ROOT_C]);
	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]=
			s->value[BIOMASS_COARSE_ROOT_tDM] *
			(s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C]=
			s->value[COARSE_ROOT_C] *
			(s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Coarse Root Biomass = %f tDM cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
	Log("-Live Coarse Root Biomass = %f tC cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]=
			s->value[BIOMASS_COARSE_ROOT_tDM] -
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM];
	s->value[COARSE_ROOT_DEAD_WOOD_C]=
			s->value[COARSE_ROOT_C] -
			s->value[COARSE_ROOT_LIVE_WOOD_C];
	Log("-Dead Coarse Root Biomass = %f tDM cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
	Log("-Dead Coarse Root Biomass = %f tC cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] =
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] =
			s->value[COARSE_ROOT_LIVE_WOOD_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual live coarse root biomass = %f KgDM\n", s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	Log("-Individual live coarse root biomass = %f KgC\n", s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM] =
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM] =
			s->value[COARSE_ROOT_DEAD_WOOD_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual live coarse root biomass = %f KgDM\n", s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	Log("-Individual live coarse root biomass = %f KgC\n", s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_C])-
			(s->value[COARSE_ROOT_LIVE_WOOD_C] +
					s->value[COARSE_ROOT_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_COARSE_ROOT_MASS_KgC])-
			(s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] +
					s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM])), >1e-4);

	/* branch live wood */
	Log("Total Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
	Log("Total BB Branch = %f tDM/cell\n", s->value[BRANCH_C]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]=
			s->value[BIOMASS_BRANCH_tDM] *
			(s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C]=
			s->value[BRANCH_C] *
			(s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Branch Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	Log("-Live Stem Branch Biomass = %f tC cell\n", s->value[BRANCH_LIVE_WOOD_C]);
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]=
			s->value[BIOMASS_BRANCH_tDM] -
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[BRANCH_DEAD_WOOD_C]=
			s->value[BRANCH_C] -
			s->value[BRANCH_LIVE_WOOD_C];
	Log("-Dead Stem Branch Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
	Log("-Dead Stem Branch Biomass = %f tC cell\n", s->value[BRANCH_DEAD_WOOD_C]);
	s->value[AV_LIVE_BRANCH_MASS_KgDM] =
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_LIVE_BRANCH_MASS_KgC] =
			s->value[BRANCH_LIVE_WOOD_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual live branch biomass = %f KgDM\n", s->value[AV_LIVE_BRANCH_MASS_KgDM]);
	Log("-Individual live branch biomass = %f KgC\n", s->value[AV_LIVE_BRANCH_MASS_KgC]);
	s->value[AV_DEAD_BRANCH_MASS_KgDM] =
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] *1000.0 /
			s->counter[N_TREE];
	s->value[AV_DEAD_BRANCH_MASS_KgC] =
			s->value[BRANCH_DEAD_WOOD_C] *1000.0 /
			s->counter[N_TREE];
	Log("-Individual dead branch biomass = %f KgDM\n", s->value[AV_DEAD_BRANCH_MASS_KgDM]);
	Log("-Individual dead branch biomass = %f KgC\n", s->value[AV_DEAD_BRANCH_MASS_KgC]);
	CHECK_CONDITION(fabs((s->value[BRANCH_C])-
			(s->value[BRANCH_LIVE_WOOD_C] +
					s->value[BRANCH_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_BRANCH_MASS_KgC])-
			(s->value[AV_LIVE_BRANCH_MASS_KgC] +
					s->value[AV_DEAD_BRANCH_MASS_KgC])), >1e-4);

	s->value[BIOMASS_LIVE_WOOD_tDM] =
			s->value[BIOMASS_STEM_LIVE_WOOD_tDM]+
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[LIVE_WOOD_C] =
			s->value[STEM_LIVE_WOOD_C]+
			s->value[COARSE_ROOT_LIVE_WOOD_C]+
			s->value[BRANCH_LIVE_WOOD_C];
	Log("---Live biomass = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD_tDM]);
	Log("---Live biomass = %f tC/area\n", s->value[LIVE_WOOD_C]);
	s->value[AV_LIVE_WOOD_MASS_KgDM] =
			s->value[BIOMASS_LIVE_WOOD_tDM] * 1000.0/
			s->counter[N_TREE];
	s->value[AV_LIVE_WOOD_MASS_KgC] =
			s->value[LIVE_WOOD_C] * 1000.0/
			s->counter[N_TREE];
	Log("-Individual total live biomass = %f KgDM\n", s->value[AV_LIVE_WOOD_MASS_KgDM]);
	Log("-Individual total live biomass = %f KgC\n", s->value[AV_LIVE_WOOD_MASS_KgC]);

	s->value[BIOMASS_DEAD_WOOD_tDM] =
			s->value[BIOMASS_STEM_DEAD_WOOD_tDM]+
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM];
	s->value[DEAD_WOOD_C] =
			s->value[STEM_DEAD_WOOD_C]+
			s->value[COARSE_ROOT_DEAD_WOOD_C]+
			s->value[BRANCH_DEAD_WOOD_C];
	Log("---Dead biomass = %f tDM/area\n", s->value[BIOMASS_DEAD_WOOD_tDM]);
	Log("---Dead biomass = %f tC/area\n", s->value[DEAD_WOOD_C]);
	s->value[AV_DEAD_WOOD_MASS_KgDM] =
			s->value[BIOMASS_DEAD_WOOD_tDM] * 1000.0/
			s->counter[N_TREE];
	s->value[AV_DEAD_WOOD_MASS_KgC] =
			s->value[DEAD_WOOD_C] * 1000.0/
			s->counter[N_TREE];
	Log("-Individual total dead biomass = %f KgDM\n", s->value[AV_DEAD_WOOD_MASS_KgDM]);
	Log("-Individual total dead biomass = %f KgC\n", s->value[AV_DEAD_WOOD_MASS_KgC]);

	/* compute percentage of live vs total biomass */
	s->value[TOTAL_WOOD_C] =
			s->value[STEM_C] +
			s->value[TOT_ROOT_C] +
			s->value[BRANCH_C];
	s->value[AV_TOT_WOOD_MASS_KgC] =
			s->value[AV_STEM_MASS_KgC] +
			s->value[AV_ROOT_MASS_KgC] +
			s->value[AV_BRANCH_MASS_KgC];
	Log("----Total wood = %f tC/cell\n",s->value[TOTAL_WOOD_C]);
	Log("----Total wood = %f KgC/tree\n",s->value[AV_TOT_WOOD_MASS_KgC]);
	Log("----Live wood vs total biomass = %f %%\n", (s->value[LIVE_WOOD_C] /
			s->value[TOTAL_WOOD_C]) * 100.0);
	Log("----Dead wood vs total biomass = %f %%\n", (s->value[DEAD_WOOD_C] /
			s->value[TOTAL_WOOD_C]) * 100.0);
	Log("----Live wood vs total biomass = %f %%\n", (s->value[AV_LIVE_WOOD_MASS_KgC] /
			s->value[AV_TOT_WOOD_MASS_KgC]) * 100.0);
	Log("----Dead wood vs total biomass = %f %%\n", (s->value[AV_DEAD_WOOD_MASS_KgC] /
			s->value[AV_TOT_WOOD_MASS_KgC]) * 100.0);
	
	return 1;
}
