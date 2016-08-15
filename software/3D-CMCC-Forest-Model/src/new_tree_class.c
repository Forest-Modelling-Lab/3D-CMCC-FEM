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
#include "matrix.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "common.h"

extern settings_t* g_settings;
extern logger_t* g_log;

int Create_new_class(cell_t *const c, const int height, const int age, const int species)
{
	int i;
	int y;
	int z;
	int flag;
	height_t *h;
	age_t *a;
	species_t *s;

	logger(g_log, "Creating new class....\n");

	/* it is used only with "human" regeneration */
	logger(g_log, "Human management\n");

	// add height
	if (!alloc_struct((void **)&c->heights, &c->heights_count, sizeof(height_t)) )
	{
		return 0;
	}
	h = &c->heights[c->heights_count-1];
	h->value = g_settings->height_sapling;

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
	if ( !alloc_struct((void **)&h->ages, &h->ages_count, sizeof(age_t)) )
	{
		return 0;
	}
	a = &h->ages[h->ages_count-1];
	a->value = g_settings->age_sapling;
	a->species = NULL;
	a->species_count = 0;

	// add species
	if ( !alloc_struct((void **)&a->species, &a->species_count, sizeof(species_t)) )
	{
		return 0;
	}
	s = &a->species[a->species_count-1];
	s->name = string_copy(g_settings->replanted_species);
	if ( ! s->name ) {
		logger(g_log, "unable to copy replanted species from settins. out of memory.");
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
				if ( ! string_compare_i(c->heights[y].ages[y].species[z].name, s->name) ) {
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
	logger(g_log, "**heights_count = %d \n", c->heights_count);
	logger(g_log, "**ages_count = %d \n", h->ages_count);
	logger(g_log, "**species_count = %d \n", a->species_count);
	logger(g_log, "**height_sapling = %f\n", h->value);
	logger(g_log, "**age_sapling = %d\n", a->value);
	s->value[AVDBH] = g_settings->avdbh_sapling;
	logger(g_log, "**avdbh sampling = %f\n", s->value[AVDBH]);
	s->counter[N_TREE] = g_settings->replanted_tree;
	logger(g_log, "**n tree %d of %s\n", s->counter[N_TREE], s->name);

	/* compute all other variables */
	Allometry_Power_Function(a, s);
	/* stem biomass */
	s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_B));
	logger(g_log, "**AV_STEM_MASS_KgDM = %f\n", s->value[AV_STEM_MASS_KgDM]);
	s->value[STEM_C] = 	s->value[AV_STEM_MASS_KgDM] * s->counter[N_TREE] / 1000.0;
	logger(g_log, "**STEM_C = %f\n", s->value[STEM_C]);
	s->value[AV_STEM_MASS_KgC] = s->value[STEM_C] * 1000.0 / s->counter[N_TREE];
	logger(g_log, "**AV_STEM_MASS_KgC = %f\n", s->value[AV_STEM_MASS_KgC]);

	/* in tDM */
	s->value[BIOMASS_STEM_tDM] =s->value[AV_STEM_MASS_KgDM] *s->counter[N_TREE] / 1000.0 * GC_GDM;
	logger(g_log, "**BIOMASS_STEM_tDM = %f\n", s->value[BIOMASS_STEM_tDM]);

	/* branch and bark biomass */
	s->value[FRACBB] =s->value[FRACBB1]+(s->value[FRACBB0]-s->value[FRACBB1])*exp(-ln2 * (c->heights[c->heights_count-1].value /
							s->value[TBB]));
	logger(g_log, "**FRACBB = %f\n", s->value[FRACBB]);
	s->value[BRANCH_C] = s->value[STEM_C] * s->value[FRACBB];
	logger(g_log, "**BRANCH_C = %f\n", s->value[BRANCH_C]);
	s->value[AV_BRANCH_MASS_KgC] = s->value[BRANCH_C] * 1000.0 / s->counter[N_TREE];
	logger(g_log, "**AV_BRANCH_MASS_KgC = %f\n", s->value[AV_BRANCH_MASS_KgC]);

	/* sapwood calculation */
	logger(g_log, "\nSAPWOOD CALCULATION using sapwood area\n");
	s->value[BASAL_AREA] =((pow((s->value[AVDBH] / 2.0), 2.0)) * Pi) * 0.0001;
	logger(g_log, "   BASAL AREA = %f cm^2\n", s->value[BASAL_AREA]);

	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
	logger(g_log, " BASAL BASAL_AREA_m2 = %f m^2\n", s->value[BASAL_AREA_m2]);

	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);

	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);

	s->value[SAPWOOD_AREA] =s->value[SAP_A] *pow (s->value[AVDBH],s->value[SAP_B]);
	logger(g_log, "   SAPWOOD_AREA = %f cm^2\n", s->value[SAPWOOD_AREA]);

	s->value[HEARTWOOD_AREA] =s->value[BASAL_AREA] -s->value[SAPWOOD_AREA];
	logger(g_log, "   HEART_WOOD_AREA = %f cm^2\n", s->value[HEARTWOOD_AREA]);

	s->value[SAPWOOD_PERC] =
			(s->value[SAPWOOD_AREA]) /
			s->value[BASAL_AREA];
	logger(g_log, "   sapwood perc = %f%%\n", s->value[SAPWOOD_PERC]*100);

	s->value[WS_sap_tDM] =(s->value[BIOMASS_STEM_tDM] *s->value[SAPWOOD_PERC]);
	logger(g_log, "   WS_sap_tDM = %f\n", s->value[WS_sap_tDM]);

	s->value[STEM_SAPWOOD_C] =(s->value[STEM_C] *s->value[SAPWOOD_PERC]);
	logger(g_log, "   Sapwood stem biomass = %f tDM class cell \n", s->value[WS_sap_tDM]);
	logger(g_log, "   Sapwood stem biomass = %f tC class cell \n", s->value[STEM_SAPWOOD_C]);


	//compute coarse root biomass using root to shoot ratio
	s->value[BIOMASS_COARSE_ROOT_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[COARSE_ROOT_STEM];
	s->value[COARSE_ROOT_C] = s->value[STEM_C] * s->value[COARSE_ROOT_STEM];
	s->value[AV_COARSE_ROOT_MASS_KgC] = s->value[COARSE_ROOT_C] * 1000.0 / s->counter[N_TREE];
	logger(g_log, "   Coarse root biomass = %f tDM class cell \n", s->value[BIOMASS_COARSE_ROOT_tDM]);
	logger(g_log, "   Coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_C]);
	logger(g_log, "AV_COARSE_ROOT_MASS_KgC = %f\n", s->value[AV_COARSE_ROOT_MASS_KgC]);

	s->value[WRC_sap_tDM] =
			(s->value[BIOMASS_COARSE_ROOT_tDM] *
					s->value[SAPWOOD_PERC]);
	s->value[COARSE_ROOT_SAPWOOD_C] =
			(s->value[COARSE_ROOT_C] *
					s->value[SAPWOOD_PERC]);
	logger(g_log, "   Sapwood coarse root biomass = %f tDM class cell \n", s->value[WRC_sap_tDM]);
	logger(g_log, "   Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);

	//compute branch and bark biomass using FRACBB
	s->value[FRACBB] = s->value[FRACBB1]+ (s->value[FRACBB0]- s->value[FRACBB1])* exp(-ln2 * (h->value / s->value[TBB]));
	s->value[BIOMASS_BRANCH_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[FRACBB];
	s->value[BRANCH_C] = s->value[STEM_C] * s->value[FRACBB];
	logger(g_log, "-Stem Branch Biomass initialization data from DBH = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
	logger(g_log, "-Stem Branch Biomass initialization data from DBH = %f tC/cell\n", s->value[BRANCH_C]);

	s->value[WBB_sap_tDM] =(s->value[BIOMASS_BRANCH_tDM] *s->value[SAPWOOD_PERC]);
	s->value[BRANCH_SAPWOOD_C] =(s->value[BRANCH_C] *s->value[SAPWOOD_PERC]);
	logger(g_log, "   Sapwood branch and bark biomass = %f tDM class cell \n", s->value[WBB_sap_tDM]);
	logger(g_log, "   Sapwood branch and bark biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);

	s->value[WTOT_sap_tDM] =s->value[WS_sap_tDM] +s->value[WRC_sap_tDM] + s->value[WBB_sap_tDM];
	s->value[TOT_SAPWOOD_C] =s->value[STEM_SAPWOOD_C] +s->value[COARSE_ROOT_SAPWOOD_C] +s->value[BRANCH_SAPWOOD_C];
	logger(g_log, "   STEM_SAPWOOD_C = %f tC \n", s->value[WTOT_sap_tDM]);
	logger(g_log, "   COARSE_ROOT_SAPWOOD_C = %f tC \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	logger(g_log, "   BRANCH_SAPWOOD_C = %f tC \n", s->value[BRANCH_SAPWOOD_C]);
	logger(g_log, "   Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap_tDM]);
	logger(g_log, "   Total Sapwood biomass per tree = %f tDM tree \n", s->value[WTOT_sap_tDM]/s->counter[N_TREE]);
	logger(g_log, "   Total Sapwood biomass per tree = %f KgDM tree \n", (s->value[WTOT_sap_tDM]/s->counter[N_TREE])*1000.0);
	logger(g_log, "   Total Sapwood biomass per tree = %f gDM tree \n", (s->value[WTOT_sap_tDM]/s->counter[N_TREE])*1000000.0);
	logger(g_log, "   Total Sapwood biomass = %f tC class cell \n", s->value[TOT_SAPWOOD_C]);
	logger(g_log, "   Total Sapwood biomass per tree = %f tC tree \n", s->value[TOT_SAPWOOD_C]/s->counter[N_TREE]);
	logger(g_log, "   Total Sapwood biomass per tree = %f KgC tree \n", (s->value[TOT_SAPWOOD_C]/s->counter[N_TREE])*1000.0);
	logger(g_log, "   Total Sapwood biomass per tree = %f gC tree \n", (s->value[TOT_SAPWOOD_C]/s->counter[N_TREE])*1000000.0);

	/* reserve */
	logger(g_log, "\nNo Reserve Biomass Data are available for model initialization \n");
	logger(g_log, "...Generating input Reserve Biomass biomass data\n");
	//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
	//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

	/* IMPORTANT! reserve computation if not in init data are computed from DM */
	s->value[RESERVE_tDM] =	s->value[WTOT_sap_tDM] *s->value[SAP_WRES];
	s->value[RESERVE_C]=s->value[WTOT_sap_tDM] *s->value[SAP_WRES];

	logger(g_log, "\n!!!!RESERVE ARE COMPUTED AS A COSTANT FRACTION OF SAPWOOD IN DRYMATTER!!!!\n");
	logger(g_log, "-----Reserve initialization data  = %f tDM/cell \n", s->value[RESERVE_tDM]);
	logger(g_log, "-----Reserve initialization data  = %f Kg res/cell \n", s->value[RESERVE_tDM] * 1000);
	logger(g_log, "-----Reserve initialization data  = %f g res/tree \n", (s->value[RESERVE_tDM] * 1000000)/
			(int)s->counter[N_TREE]);
	logger(g_log, "-----Reserve initialization data  = %f t res/cell \n", s->value[RESERVE_C]);
	logger(g_log, "-----Reserve initialization data  = %f Kg res/cell \n", s->value[RESERVE_C] * 1000);
	logger(g_log, "-----Reserve initialization data  = %f g res/tree \n", (s->value[RESERVE_C] * 1000000)/
			(int)s->counter[N_TREE]);

	s->value[AV_RESERVE_MASS_KgDM] =s->value[RESERVE_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_RESERVE_MASS_KgC] = s->value[RESERVE_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual reserve = %f KgDM/tree\n", s->value[AV_RESERVE_MASS_KgDM]);
	logger(g_log, "-Individual reserve = %f Kg/tree\n", s->value[AV_RESERVE_MASS_KgC]);
	/* compute minimum reserve pool */
	s->value[MIN_RESERVE_C] = s->value[RESERVE_C];
	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C]*
			1000.0 /s->counter[N_TREE];
	logger(g_log, "-Minimum reserve  = %f tres/tree\n", s->value[MIN_RESERVE_C]);
	logger(g_log, "-Individual minimum reserve = %f KgC/tree\n", s->value[AV_MIN_RESERVE_KgC]);

	/* leaf */
	if (s->value[PHENOLOGY] == 0.1 ||
			s->value[PHENOLOGY] == 0.2)
	{
		/* assuming no leaf at 1st of January */
		s->value[BIOMASS_FOLIAGE_tDM] = 0.0;
		s->value[LEAF_C] = 0.0;
	}
	else
	{
		logger(g_log, "\nNo Foliage Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Foliage Biomass biomass data\n");
		//fixme it seems to not have sense
		s->value[BIOMASS_FOLIAGE_tDM] =
				s->value[RESERVE_tDM] *
				(1.0 - s->value[STEM_LEAF_FRAC]);
		s->value[LEAF_C] = s->value[RESERVE_C] *
				(1.0 - s->value[STEM_LEAF_FRAC]);
		logger(g_log, "----Foliage Biomass initialization data  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
		logger(g_log, "----Foliage Biomass initialization data  = %f tC cell\n", s->value[LEAF_C]);
	}
	s->value[LEAF_C] = s->value[BIOMASS_FOLIAGE_tDM]/GC_GDM;
	logger(g_log, "---Foliage Biomass from init file  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
	logger(g_log, "---Foliage Biomass from init file  = %f tC cell\n", s->value[LEAF_C]);

	s->value[AV_FOLIAGE_MASS_KgDM] =s->value[BIOMASS_FOLIAGE_tDM] *1000.0 /	s->counter[N_TREE];
	s->value[AV_LEAF_MASS_KgC] =s->value[LEAF_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual foliage biomass = %f KgDM\n", s->value[AV_FOLIAGE_MASS_KgDM]);
	logger(g_log, "-Individual foliage biomass = %f KgC\n", s->value[AV_LEAF_MASS_KgC]);


	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	/* fine root biomass */
	if (s->value[BIOMASS_FINE_ROOT_tDM] == 0.0 &&
			(s->value[PHENOLOGY] == 1.1 ||
					s->value[PHENOLOGY] == 1.2))
	{
		logger(g_log, "\nNo Fine root Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Fine root Biomass biomass data from DBH data...\n");
		//assuming FINE_ROOT_LEAF RATIO AS IN BIOME
		s->value[BIOMASS_FINE_ROOT_tDM] =
				s->value[BIOMASS_FOLIAGE_tDM] *
				s->value[FINE_ROOT_LEAF];
		s->value[FINE_ROOT_C] =
				s->value[LEAF_C] *
				s->value[FINE_ROOT_LEAF];
		logger(g_log, "---Fine Root Biomass initialization data from Stem Biomass = %f tDM cell\n", s->value[BIOMASS_FINE_ROOT_tDM]);
		logger(g_log, "---Fine Root Biomass initialization data from Stem Biomass = %f tC cell\n", s->value[FINE_ROOT_C]);
	}
	else
	{
		/* assuming no fine root at 1st of January */
		s->value[BIOMASS_FINE_ROOT_tDM] = 0.0;
		s->value[FINE_ROOT_C] = 0.0;
		logger(g_log, "Ok fine root biomass..\n");
		logger(g_log, "---Fine Root Biomass from init file  = %f tDM cell\n", s->value[BIOMASS_FINE_ROOT_tDM]);
		logger(g_log, "---Fine Root Biomass from init file  = %f tC cell\n", s->value[FINE_ROOT_C]);
	}
	s->value[AV_FINE_ROOT_MASS_KgDM] =s->value[BIOMASS_FINE_ROOT_tDM] *1000.0 /	s->counter[N_TREE];
	s->value[AV_FINE_ROOT_MASS_KgC] =s->value[FINE_ROOT_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual fine root biomass = %f KgDM\n", s->value[AV_FINE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual fine root biomass = %f KgC\n", s->value[AV_FINE_ROOT_MASS_KgC]);

	s->value[BIOMASS_ROOTS_TOT_tDM] =s->value[BIOMASS_COARSE_ROOT_tDM] +s->value[BIOMASS_FINE_ROOT_tDM];
	s->value[TOT_ROOT_C] =s->value[COARSE_ROOT_C] +s->value[FINE_ROOT_C];
	logger(g_log, "---Total Root Biomass = %f tDM cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	logger(g_log, "---Total Root Biomass = %f tC cell\n", s->value[TOT_ROOT_C]);

	s->value[AV_ROOT_MASS_KgDM] =s->value[BIOMASS_ROOTS_TOT_tDM] * 1000.0 /s->counter[N_TREE];
	s->value[AV_ROOT_MASS_KgC] =s->value[TOT_ROOT_C] * 1000.0 /	s->counter[N_TREE];
	CHECK_CONDITION(fabs((s->value[TOT_ROOT_C])-(s->value[COARSE_ROOT_C] +s->value[FINE_ROOT_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_ROOT_MASS_KgC])-(s->value[AV_COARSE_ROOT_MASS_KgC] +s->value[AV_FINE_ROOT_MASS_KgC])), >1e-4);

	/* compute biomass live wood */
	//assuming LIVE_DEAD WOOD RATIO AS IN BIOME

	/* compute biome fractions */
	Pool_fraction (s);

	logger(g_log, "\n*******************************\n");
	/*FOR STEM*/
	logger(g_log, "Total Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_tDM]);
	logger(g_log, "Total Stem Biomass = %f tC cell\n", s->value[STEM_C]);

	s->value[BIOMASS_STEM_LIVE_WOOD_tDM]=s->value[BIOMASS_STEM_tDM] *(s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C]= s->value[STEM_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "-Live Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
	logger(g_log, "-Live Stem Biomass = %f tC cell\n", s->value[STEM_LIVE_WOOD_C]);

	s->value[BIOMASS_STEM_DEAD_WOOD_tDM]=s->value[BIOMASS_STEM_tDM] -s->value[BIOMASS_STEM_LIVE_WOOD_tDM];
	s->value[STEM_DEAD_WOOD_C]=	s->value[STEM_C] -s->value[STEM_LIVE_WOOD_C];
	logger(g_log, "-Dead Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
	logger(g_log, "-Dead Stem Biomass = %f tC cell\n", s->value[STEM_DEAD_WOOD_C]);

	s->value[AV_LIVE_STEM_MASS_KgDM] =s->value[BIOMASS_STEM_LIVE_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_LIVE_STEM_MASS_KgC] =s->value[STEM_LIVE_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live wood biomass = %f KgDM\n", s->value[AV_LIVE_STEM_MASS_KgDM]);
	logger(g_log, "-Individual live wood biomass = %f KgC\n", s->value[AV_LIVE_STEM_MASS_KgC]);

	s->value[AV_DEAD_STEM_MASS_KgDM] =s->value[BIOMASS_STEM_DEAD_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_DEAD_STEM_MASS_KgC] =s->value[STEM_DEAD_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual dead wood biomass = %f KgDM\n", s->value[AV_DEAD_STEM_MASS_KgDM]);
	logger(g_log, "-Individual dead wood biomass = %f KgC\n", s->value[AV_DEAD_STEM_MASS_KgC]);

	CHECK_CONDITION(fabs((s->value[STEM_C])-(s->value[STEM_LIVE_WOOD_C] +s->value[STEM_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_STEM_MASS_KgC])-(s->value[AV_LIVE_STEM_MASS_KgC] +s->value[AV_DEAD_STEM_MASS_KgC])), >1e-4);

	/* coarse live wood */
	logger(g_log, "Total Root Biomass = %f tDM cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	logger(g_log, "Total Root Biomass = %f tC cell\n", s->value[TOT_ROOT_C]);

	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]=s->value[BIOMASS_COARSE_ROOT_tDM] *(s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C]=s->value[COARSE_ROOT_C] *(s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "-Live Coarse Root Biomass = %f tDM cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
	logger(g_log, "-Live Coarse Root Biomass = %f tC cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]=s->value[BIOMASS_COARSE_ROOT_tDM] -	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM];
	s->value[COARSE_ROOT_DEAD_WOOD_C]=s->value[COARSE_ROOT_C] -	s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_log, "-Dead Coarse Root Biomass = %f tDM cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
	logger(g_log, "-Dead Coarse Root Biomass = %f tC cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] =s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] =s->value[COARSE_ROOT_LIVE_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live coarse root biomass = %f KgDM\n", s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual live coarse root biomass = %f KgC\n", s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);

	s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM] =s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM] =s->value[COARSE_ROOT_DEAD_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live coarse root biomass = %f KgDM\n", s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual live coarse root biomass = %f KgC\n", s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_C])-(s->value[COARSE_ROOT_LIVE_WOOD_C] +s->value[COARSE_ROOT_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_COARSE_ROOT_MASS_KgC])-(s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] +s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM])), >1e-4);

	/* branch live wood */
	logger(g_log, "Total Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
	logger(g_log, "Total BB Branch = %f tDM/cell\n", s->value[BRANCH_C]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]=s->value[BIOMASS_BRANCH_tDM] *(s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C]=s->value[BRANCH_C] *(s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "-Live Stem Branch Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	logger(g_log, "-Live Stem Branch Biomass = %f tC cell\n", s->value[BRANCH_LIVE_WOOD_C]);

	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] - s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[BRANCH_DEAD_WOOD_C]= s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
	logger(g_log, "-Dead Stem Branch Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
	logger(g_log, "-Dead Stem Branch Biomass = %f tC cell\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[AV_LIVE_BRANCH_MASS_KgDM] = s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] *1000.0 / s->counter[N_TREE];
	s->value[AV_LIVE_BRANCH_MASS_KgC] = s->value[BRANCH_LIVE_WOOD_C] *1000.0 / s->counter[N_TREE];
	logger(g_log, "-Individual live branch biomass = %f KgDM\n", s->value[AV_LIVE_BRANCH_MASS_KgDM]);
	logger(g_log, "-Individual live branch biomass = %f KgC\n", s->value[AV_LIVE_BRANCH_MASS_KgC]);

	s->value[AV_DEAD_BRANCH_MASS_KgDM] = s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] *1000.0 / s->counter[N_TREE];
	s->value[AV_DEAD_BRANCH_MASS_KgC] = s->value[BRANCH_DEAD_WOOD_C] *1000.0 / s->counter[N_TREE];
	logger(g_log, "-Individual dead branch biomass = %f KgDM\n", s->value[AV_DEAD_BRANCH_MASS_KgDM]);
	logger(g_log, "-Individual dead branch biomass = %f KgC\n", s->value[AV_DEAD_BRANCH_MASS_KgC]);
	CHECK_CONDITION(fabs((s->value[BRANCH_C])- (s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_BRANCH_MASS_KgC])- (s->value[AV_LIVE_BRANCH_MASS_KgC] + s->value[AV_DEAD_BRANCH_MASS_KgC])), >1e-4);

	s->value[BIOMASS_LIVE_WOOD_tDM] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM]+ s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[LIVE_WOOD_C] = s->value[STEM_LIVE_WOOD_C]+ s->value[COARSE_ROOT_LIVE_WOOD_C]+ s->value[BRANCH_LIVE_WOOD_C];
	logger(g_log, "---Live biomass = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD_tDM]);
	logger(g_log, "---Live biomass = %f tC/area\n", s->value[LIVE_WOOD_C]);

	s->value[AV_LIVE_WOOD_MASS_KgDM] = s->value[BIOMASS_LIVE_WOOD_tDM] * 1000.0/ s->counter[N_TREE];
	s->value[AV_LIVE_WOOD_MASS_KgC] = s->value[LIVE_WOOD_C] * 1000.0/ s->counter[N_TREE];
	logger(g_log, "-Individual total live biomass = %f KgDM\n", s->value[AV_LIVE_WOOD_MASS_KgDM]);
	logger(g_log, "-Individual total live biomass = %f KgC\n", s->value[AV_LIVE_WOOD_MASS_KgC]);

	s->value[BIOMASS_DEAD_WOOD_tDM] = s->value[BIOMASS_STEM_DEAD_WOOD_tDM]+ s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM];
	s->value[DEAD_WOOD_C] = s->value[STEM_DEAD_WOOD_C]+ s->value[COARSE_ROOT_DEAD_WOOD_C]+ s->value[BRANCH_DEAD_WOOD_C];
	logger(g_log, "---Dead biomass = %f tDM/area\n", s->value[BIOMASS_DEAD_WOOD_tDM]);
	logger(g_log, "---Dead biomass = %f tC/area\n", s->value[DEAD_WOOD_C]);

	s->value[AV_DEAD_WOOD_MASS_KgDM] =s->value[BIOMASS_DEAD_WOOD_tDM] * 1000.0/s->counter[N_TREE];
	s->value[AV_DEAD_WOOD_MASS_KgC] =s->value[DEAD_WOOD_C] * 1000.0/s->counter[N_TREE];
	logger(g_log, "-Individual total dead biomass = %f KgDM\n", s->value[AV_DEAD_WOOD_MASS_KgDM]);
	logger(g_log, "-Individual total dead biomass = %f KgC\n", s->value[AV_DEAD_WOOD_MASS_KgC]);

	/* compute percentage of live vs total biomass */
	s->value[TOT_WOOD_C] =s->value[STEM_C] + s->value[TOT_ROOT_C] + s->value[BRANCH_C];
	s->value[AV_TOT_WOOD_MASS_KgC] =s->value[AV_STEM_MASS_KgC] +s->value[AV_ROOT_MASS_KgC] +s->value[AV_BRANCH_MASS_KgC];
	logger(g_log, "----Total wood = %f tC/cell\n",s->value[TOT_WOOD_C]);
	logger(g_log, "----Total wood = %f KgC/tree\n",s->value[AV_TOT_WOOD_MASS_KgC]);
	logger(g_log, "----Live wood vs total biomass = %f %%\n", (s->value[LIVE_WOOD_C] /s->value[TOT_WOOD_C]) * 100.0);
	logger(g_log, "----Dead wood vs total biomass = %f %%\n", (s->value[DEAD_WOOD_C] /s->value[TOT_WOOD_C]) * 100.0);
	logger(g_log, "----Live wood vs total biomass = %f %%\n", (s->value[AV_LIVE_WOOD_MASS_KgC] /s->value[AV_TOT_WOOD_MASS_KgC]) * 100.0);
	logger(g_log, "----Dead wood vs total biomass = %f %%\n", (s->value[AV_DEAD_WOOD_MASS_KgC] /s->value[AV_TOT_WOOD_MASS_KgC]) * 100.0);

	return 1;
}
