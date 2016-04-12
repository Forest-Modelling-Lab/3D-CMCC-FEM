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

void Create_new_class(CELL *c, HEIGHT *h, AGE *a, SPECIES *s, int height, int age, int species)
{
	Log("Creating new class....\n");
	Log("Replanted trees = %f\n", settings->replanted_tree);
	/* fill the row with the new variables */
	/* ist is used only with "human" regeneration */
	if(!mystricmp(settings->management, "on"))
	{
		Log("Human management\n");
		//fixme is it necessary?? i.e. use the row struct
//		r->x = c->x;
//		r->y = c->y;
//		r->height = settings->height_sapling;
//		r->avdbh = settings->avdbh_sapling;
//		r->age = settings->age_sapling;
//		r->species = settings->replanted_species;
//		r->n = settings->replanted_tree;
//		r->lai = settings->lai_sapling;


		//fixme use "fill_cell..." functions in matrix.c??

		//fixme or just need this??
		/* fill the structs with new variables incrementig counters */
		c->heights_count ++;
		h->ages_count ++;
		a->species_count ++;
		Log("heights_count = %d \n", c->heights_count);
		Log("ages_count = %d \n", h->ages_count);
		Log("species_count = %d \n", a->species_count);


		/* assign height value */
		c->heights[c->heights_count+1].value = settings->height_sapling;
		Log("height_sapling = %f\n", c->heights[c->heights_count+1].value);
		/* assign age value */
		h->ages[h->ages_count+1].value = settings->age_sapling;
		Log("age_sapling = %f\n", h->ages[h->ages_count+1].value);
		/* assign species name */
		//a->species[a->species_count+1].name = mystrdup (settings->replanted_species);
		//Log("replanted species = %s\n", a->species[a->species_count+1].name);
		/* assign dbh value */
		a->species[a->species_count+1].value[AVDBH] = settings->avdbh_sapling;
		/* compute density */
		a->species[a->species_count+1].counter[N_TREE] = settings->replanted_tree;
	}
	else
	{
		/* use natural regeneration to fill the rows */
	}

	/* compute all other variables */
	/* stem biomass */
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_STEM_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEMCONST] *
			(pow (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AVDBH], STEMPOWER_B));
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_STEM_MASS_KgDM] *
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE] / 1000.0;
	/* in tDM */
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_tDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_STEM_MASS_KgDM] *
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE] / 1000.0 * GC_GDM;

	/* branch and bark biomass */
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FRACBB] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FRACBB1]+
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FRACBB0]-
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FRACBB1])*
					exp(-ln2 * (c->heights[c->heights_count+1].value /
							c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TBB]));
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C] *
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FRACBB];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_BRANCH_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C] *
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE] * 1000.0;

	/* sapwood calculation */
	Log("\nSAPWOOD CALCULATION using sapwood area\n");
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BASAL_AREA] =
			((pow((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AVDBH] / 2.0), 2.0)) * Pi);
	Log("   BASAL AREA = %f cm^2\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BASAL_AREA]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_AREA] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAP_A] *
			pow (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AVDBH],
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAP_B]);
	Log("   SAPWOOD_AREA = %f cm^2\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_AREA]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[HEARTWOOD_AREA] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BASAL_AREA] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_AREA];
	Log("   HEART_WOOD_AREA = %f cm^2\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[HEARTWOOD_AREA]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_AREA]) /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BASAL_AREA];
	Log("   sapwood perc = %f%%\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]*100);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WS_sap_tDM] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_tDM] *
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_SAPWOOD_C] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C] *
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]);
	Log("   Sapwood stem biomass = %f tDM class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WS_sap_tDM]);
	Log("   Sapwood stem biomass = %f tC class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_SAPWOOD_C]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WRC_sap_tDM] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_tDM] *
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_SAPWOOD_C] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_C] *
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]);
	Log("   Sapwood coarse root biomass = %f tDM class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WRC_sap_tDM]);
	Log("   Sapwood coarse root biomass = %f tC class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_SAPWOOD_C]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WBB_sap_tDM] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_BRANCH_tDM] *
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_SAPWOOD_C] =
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C] *
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAPWOOD_PERC]);
	Log("   Sapwood branch and bark biomass = %f tDM class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WBB_sap_tDM]);
	Log("   Sapwood branch and bark biomass = %f tC class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_SAPWOOD_C]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WS_sap_tDM] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WRC_sap_tDM] + c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WBB_sap_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_SAPWOOD_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_SAPWOOD_C] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_SAPWOOD_C] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_SAPWOOD_C];
	Log("   Total Sapwood biomass = %f tDM class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM]);
	Log("   Total Sapwood biomass per tree = %f tDM tree \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM]/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE]);
	Log("   Total Sapwood biomass per tree = %f KgDM tree \n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM]/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE])*1000.0);
	Log("   Total Sapwood biomass per tree = %f gDM tree \n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM]/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE])*1000000.0);
	Log("   Total Sapwood biomass = %f tC class cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_SAPWOOD_C]);
	Log("   Total Sapwood biomass per tree = %f tC tree \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_SAPWOOD_C]/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE]);
	Log("   Total Sapwood biomass per tree = %f KgC tree \n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_SAPWOOD_C]/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE])*1000.0);
	Log("   Total Sapwood biomass per tree = %f gC tree \n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_SAPWOOD_C]/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE])*1000000.0);

	/* reserve */
	Log("\nNo Reserve Biomass Data are available for model initialization \n");
	Log("...Generating input Reserve Biomass biomass data\n");
	//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
	//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

	/* IMPORTANT! reserve computation if not in init data are computed from DM */
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_tDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM] *
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAP_WRES];
	//fixme
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[WTOT_sap_tDM] *
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[SAP_WRES];

	Log("\n!!!!RESERVE ARE COMPUTED AS A COSTANT FRACTION OF SAPWOOD IN DRYMATTER!!!!\n");
	Log("-----Reserve initialization data  = %f tDM/cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_tDM]);
	Log("-----Reserve initialization data  = %f Kg res/cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_tDM] * 1000);
	Log("-----Reserve initialization data  = %f g res/tree \n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_tDM] * 1000000)/
			(int)c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE]);
	Log("-----Reserve initialization data  = %f t res/cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C]);
	Log("-----Reserve initialization data  = %f Kg res/cell \n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C] * 1000);
	Log("-----Reserve initialization data  = %f g res/tree \n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C] * 1000000)/
			(int)c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_RESERVE_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_tDM] *
			1000.0 /c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_RESERVE_MASS_KgC] = c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C] *1000.0 /c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual reserve = %f KgDM/tree\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_RESERVE_MASS_KgDM]);
	Log("-Individual reserve = %f Kg/tree\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_RESERVE_MASS_KgC]);
	/* compute minimum reserve pool */
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[MIN_RESERVE_C] = c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_MIN_RESERVE_KgC] = c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[MIN_RESERVE_C]*
			1000.0 /c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Minimum reserve  = %f tres/tree\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[MIN_RESERVE_C]);
	Log("-Individual minimum reserve = %f KgC/tree\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_MIN_RESERVE_KgC]);

	/* leaf */
	if (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM] == 0.0)
	{
		if (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[PHENOLOGY] == 0.1 ||
				c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[PHENOLOGY] == 0.2)
		{
			/* assuming no leaf at 1st of January */
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM] = 0.0;
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C] = 0.0;
		}
		else
		{
			Log("\nNo Foliage Biomass Data are available for model initialization \n");
			Log("...Generating input Foliage Biomass biomass data\n");
			//fixme it seems to not have sense
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM] =
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_tDM] *
					(1.0 - c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LEAF_FRAC]);
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C] = c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[RESERVE_C] *
					(1.0 - c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LEAF_FRAC]);
			Log("----Foliage Biomass initialization data  = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM]);
			Log("----Foliage Biomass initialization data  = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C]);
		}
	}
	else
	{
		c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C] = c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM]/GC_GDM;
		Log("Ok foliage biomass..\n");
		Log("---Foliage Biomass from init file  = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM]);
		Log("---Foliage Biomass from init file  = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C]);
	}
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FOLIAGE_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LEAF_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual foliage biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FOLIAGE_MASS_KgDM]);
	Log("-Individual foliage biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LEAF_MASS_KgC]);


	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	/* fine root biomass */
	if (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM] == 0.0 &&
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[PHENOLOGY] == 1.1 ||
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[PHENOLOGY] == 1.2))
	{
		Log("\nNo Fine root Biomass Data are available for model initialization \n");
		Log("...Generating input Fine root Biomass biomass data from DBH data...\n");
		//assuming FINE_ROOT_LEAF RATIO AS IN BIOME
		c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM] =
				c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FOLIAGE_tDM] *
				c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_LEAF];
		c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C] =
				c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LEAF_C] *
				c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_LEAF];
		Log("---Fine Root Biomass initialization data from Stem Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM]);
		Log("---Fine Root Biomass initialization data from Stem Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C]);
	}
	else
	{
		/* assuming no fine root at 1st of January */
		c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM] = 0.0;
		c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C] = 0.0;
		Log("Ok fine root biomass..\n");
		Log("---Fine Root Biomass from init file  = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM]);
		Log("---Fine Root Biomass from init file  = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C]);
	}
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FINE_ROOT_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FINE_ROOT_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual fine root biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FINE_ROOT_MASS_KgDM]);
	Log("-Individual fine root biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FINE_ROOT_MASS_KgC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_ROOTS_TOT_tDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_tDM] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_FINE_ROOT_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_ROOT_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_C] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C];
	Log("---Total Root Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_ROOTS_TOT_tDM]);
	Log("---Total Root Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_ROOT_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_ROOT_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_ROOTS_TOT_tDM] * 1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_ROOT_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_ROOT_C] * 1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_ROOT_C])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_C] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[FINE_ROOT_C])), >1e-4);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_ROOT_MASS_KgC])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_COARSE_ROOT_MASS_KgC] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_FINE_ROOT_MASS_KgC])), >1e-4);

	/* compute biomass live wood */
	//assuming LIVE_DEAD WOOD RATIO AS IN BIOME
	Log("\n*******************************\n");
	/*FOR STEM*/
	Log("Total Stem Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_tDM]);
	Log("Total Stem Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_LIVE_WOOD_tDM]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_tDM] *
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_TOTAL_WOOD_FRAC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LIVE_WOOD_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C] *
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_LIVE_WOOD_tDM]);
	Log("-Live Stem Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LIVE_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_DEAD_WOOD_tDM]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_tDM] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_LIVE_WOOD_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_DEAD_WOOD_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LIVE_WOOD_C];
	Log("-Dead Stem Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_DEAD_WOOD_tDM]);
	Log("-Dead Stem Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_DEAD_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_STEM_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_LIVE_WOOD_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_STEM_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LIVE_WOOD_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual live wood biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_STEM_MASS_KgDM]);
	Log("-Individual live wood biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_STEM_MASS_KgC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_STEM_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_DEAD_WOOD_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_STEM_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_DEAD_WOOD_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual dead wood biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_STEM_MASS_KgDM]);
	Log("-Individual dead wood biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_STEM_MASS_KgC]);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LIVE_WOOD_C] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_STEM_MASS_KgC])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_STEM_MASS_KgC] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_STEM_MASS_KgC])), >1e-4);

	/* corase live wood */
	Log("Total Root Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_ROOTS_TOT_tDM]);
	Log("Total Root Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_ROOT_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_tDM] *
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_TOTAL_WOOD_FRAC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_LIVE_WOOD_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_C] *
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Coarse Root Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
	Log("-Live Coarse Root Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_LIVE_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_tDM] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_DEAD_WOOD_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_C] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_LIVE_WOOD_C];
	Log("-Dead Coarse Root Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
	Log("-Dead Coarse Root Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_DEAD_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_COARSE_ROOT_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_COARSE_ROOT_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_LIVE_WOOD_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual live coarse root biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	Log("-Individual live coarse root biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_COARSE_ROOT_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_COARSE_ROOT_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_DEAD_WOOD_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual live coarse root biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	Log("-Individual live coarse root biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_C])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_LIVE_WOOD_C] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_COARSE_ROOT_MASS_KgC])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_COARSE_ROOT_MASS_KgDM] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_COARSE_ROOT_MASS_KgDM])), >1e-4);

	/* branch live wood */
	Log("Total Branch Biomass = %f tDM/cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_BRANCH_tDM]);
	Log("Total BB Branch = %f tDM/cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_BRANCH_tDM] *
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_TOTAL_WOOD_FRAC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_LIVE_WOOD_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C] *
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_TOTAL_WOOD_FRAC]);
	Log("-Live Stem Branch Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	Log("-Live Stem Branch Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_LIVE_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_BRANCH_tDM] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_DEAD_WOOD_C]=
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C] -
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_LIVE_WOOD_C];
	Log("-Dead Stem Branch Biomass = %f tDM cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
	Log("-Dead Stem Branch Biomass = %f tC cell\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_DEAD_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_BRANCH_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_BRANCH_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_LIVE_WOOD_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual live branch biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_BRANCH_MASS_KgDM]);
	Log("-Individual live branch biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_BRANCH_MASS_KgC]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_BRANCH_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_BRANCH_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_DEAD_WOOD_C] *1000.0 /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual dead branch biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_BRANCH_MASS_KgDM]);
	Log("-Individual dead branch biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_BRANCH_MASS_KgC]);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_LIVE_WOOD_C] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_BRANCH_MASS_KgC])-
			(c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_BRANCH_MASS_KgC] +
					c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_BRANCH_MASS_KgC])), >1e-4);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_LIVE_WOOD_tDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_LIVE_WOOD_tDM]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_WOOD_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_LIVE_WOOD_C]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_LIVE_WOOD_C]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_LIVE_WOOD_C];
	Log("---Live biomass = %f tDM/area\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_LIVE_WOOD_tDM]);
	Log("---Live biomass = %f tC/area\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_WOOD_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_LIVE_WOOD_tDM] * 1000.0/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_WOOD_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_WOOD_C] * 1000.0/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual total live biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_WOOD_MASS_KgDM]);
	Log("-Individual total live biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_WOOD_MASS_KgC]);

	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_DEAD_WOOD_tDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_DEAD_WOOD_tDM]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[DEAD_WOOD_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_DEAD_WOOD_C]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[COARSE_ROOT_DEAD_WOOD_C]+
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_DEAD_WOOD_C];
	Log("---Dead biomass = %f tDM/area\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_DEAD_WOOD_tDM]);
	Log("---Dead biomass = %f tC/area\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[DEAD_WOOD_C]);
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_WOOD_MASS_KgDM] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BIOMASS_DEAD_WOOD_tDM] * 1000.0/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_WOOD_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[DEAD_WOOD_C] * 1000.0/
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].counter[N_TREE];
	Log("-Individual total dead biomass = %f KgDM\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_WOOD_MASS_KgDM]);
	Log("-Individual total dead biomass = %f KgC\n", c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_WOOD_MASS_KgC]);

	/* compute percentage of live vs total biomass */
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOTAL_WOOD_C] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[STEM_C] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOT_ROOT_C] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[BRANCH_C];
	c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_TOT_WOOD_MASS_KgC] =
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_STEM_MASS_KgC] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_ROOT_MASS_KgC] +
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_BRANCH_MASS_KgC];
	Log("----Total wood = %f tC/cell\n",c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOTAL_WOOD_C]);
	Log("----Total wood = %f KgC/tree\n",c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_TOT_WOOD_MASS_KgC]);
	Log("----Live wood vs total biomass = %f %%\n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[LIVE_WOOD_C] /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOTAL_WOOD_C]) * 100.0);
	Log("----Dead wood vs total biomass = %f %%\n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[DEAD_WOOD_C] /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[TOTAL_WOOD_C]) * 100.0);
	Log("----Live wood vs total biomass = %f %%\n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_LIVE_WOOD_MASS_KgC] /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_TOT_WOOD_MASS_KgC]) * 100.0);
	Log("----Dead wood vs total biomass = %f %%\n", (c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_DEAD_WOOD_MASS_KgC] /
			c->heights[c->heights_count+1].ages[h->ages_count+1].species[a->species_count+1].value[AV_TOT_WOOD_MASS_KgC]) * 100.0);
	//exit(1);
}
