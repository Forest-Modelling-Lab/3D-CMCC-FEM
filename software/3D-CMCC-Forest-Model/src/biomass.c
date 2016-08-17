/* biomass.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "biomass.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_log;

void live_total_wood_age(const age_t *const a, const int species)
{
	/* this function update based on current tree age the amount of live:total wood ratio
	 * based on the assumption that the live wood decrease linearly increasing age */
	/* e.g. for Fagus sylvatica  base on simulation for Hesse site (age 30) and Collelongo site (age 160)*/
	// fixme values should be included in species.txt
	double max_live_total_ratio;
	double min_live_total_ratio;

	double min_age = 1; //30;

	double t1;
	double t2;

	species_t *s;
	s = &a->species[species];

	logger(g_log, "*live:total wood age*\n");

	// fixme values should be included in species.txt
	max_live_total_ratio = s->value[LIVE_TOTAL_WOOD]; /* for min_age = 1 */
	min_live_total_ratio = 0.04; /* for max_age = 160 */

	//fixme it should be included in the species.txt files */
//	/* for deciduous */
//	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
//	{
//		max_live_total_ratio = 0.15;
//	}
//	/* for evergreen */
//	else
//	{
//		max_live_total_ratio = 0.10;
//	}

	t1 = max_live_total_ratio - min_live_total_ratio;
	t2 = s->value[MAXAGE] - min_age;

	s->value[EFF_LIVE_TOTAL_WOOD_FRAC] = (t1/t2)*(s->value[MAXAGE] - a->value) + min_live_total_ratio;
	logger(g_log, "Effective live:total wood fraction based on stand age = %f\n", s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
}


void annual_tree_increment(cell_t *const c, const int layer, const int height, const int age, const int species)
{

	double MassDensity;

	double single_tree_prev_vol;
	double single_tree_vol;


	age_t *a;
	species_t *s;

	a = &c->heights[height].ages[age];
	s = &c->heights[height].ages[age].species[species];

	/* in m^3/area/yr */
	/* Cai = Volume t1 - Volume t0 */
	/* Mai = Volume t1 / Age */

	/*CURRENT ANNUAL INCREMENT-CAI*/

	logger(g_log, "***CAI & MAI***\n");

	MassDensity = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TRHO]));

	/* STAND VOLUME-(STEM VOLUME) */
	/* assign previous volume */
	s->value[PREVIOUS_VOLUME] = s->value[VOLUME];
	single_tree_prev_vol = s->value[PREVIOUS_VOLUME] / (int) s->counter[N_TREE];

	/* new volume is computed using DM biomass */
	s->value[VOLUME] = s->value[STEM_C] * GC_GDM * (1 - s->value[FRACBB]) / MassDensity;
	single_tree_vol = s->value[VOLUME] / (int) s->counter[N_TREE];;

	/* CAI-Current Annual Increment */
	s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
	logger(g_log, "CAI-Current Annual Increment = %f m^3/area/yr\n", s->value[CAI]);

	/* MAI-Mean Annual Increment */
	s->value[MAI] = s->value[VOLUME] / (double)a->value;
	logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI]);

	/* check */
	CHECK_CONDITION(single_tree_vol, < single_tree_prev_vol);

}

void abg_bgb_biomass(cell_t *const c, const int height, const int age, const int species)
{

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	logger(g_log, "**AGB & BGB**\n");
	logger(g_log, "-for Class\n");
	s->value[CLASS_AGB] = s->value[TOT_STEM_C] + s->value[LEAF_C];
	logger(g_log, "Yearly Class AGB = %f tC/area year\n", s->value[CLASS_AGB]);
	s->value[CLASS_BGB] = s->value[TOT_ROOT_C];
	logger(g_log, "Yearly Class BGB = %f tC/area year\n", s->value[CLASS_BGB]);

	logger(g_log, "-for Stand\n");
	c->agb += s->value[CLASS_AGB] * s->value[PERC];
	logger(g_log, "Yearly Stand AGB = %f tC/area year\n", c->agb);
	c->bgb += s->value[CLASS_BGB] * s->value[PERC];
	logger(g_log, "Yearly Stand BGB = %f tC/area year\n", c->bgb);
	s->value[CLASS_AGB] = 0.;
	s->value[CLASS_BGB] = 0.;

}

void average_tree_biomass(species_t *const s)
{
	/* compute tree average biomass */
	s->value[AV_LEAF_MASS_KgC] = (s->value[LEAF_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_STEM_MASS_KgC] = (s->value[STEM_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_ROOT_MASS_KgC] = (s->value[TOT_ROOT_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_FINE_ROOT_MASS_KgC] = (s->value[FINE_ROOT_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_COARSE_ROOT_MASS_KgC] =(s->value[COARSE_ROOT_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_RESERVE_MASS_KgC] = (s->value[RESERVE_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_BRANCH_MASS_KgC] = (s->value[BRANCH_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_LIVE_STEM_MASS_KgC] = (s->value[STEM_LIVE_WOOD_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_DEAD_STEM_MASS_KgC] = (s->value[STEM_DEAD_WOOD_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_LIVE_WOOD_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_DEAD_WOOD_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_LIVE_BRANCH_MASS_KgC] = (s->value[BRANCH_LIVE_WOOD_C]/(double)s->counter[N_TREE])*1000.0;
	s->value[AV_DEAD_BRANCH_MASS_KgC] = (s->value[BRANCH_DEAD_WOOD_C]/(double)s->counter[N_TREE])*1000.0;
}


//not used
//void Tree_Branch_Bark (species_t *s, age_t *a, int heigth, int age, int species)
//{
//
//	if (s->value[FRACBB0] == 0)
//	{
//		logger(g_log, "I don't have FRACBB0 = FRACBB1 \n");
//		s->value[FRACBB0] = s->value[FRACBB1];
//		logger(g_log, "FRACBB0 = %f\n", s->value[FRACBB0]);
//	}
//	else
//	{
//		s->value[FRACBB] = s->value[FRACBB1] + (s->value[FRACBB0]- s->value[FRACBB1])* exp(-ln2 * (a->value / s->value[TBB]));
//	}
//
//}

//not used
//void Biomass_increment_BOY (cell_t *const c, species_t *const s, int height, int age, int years)
//{
//	double oldBasalArea;
//
//	logger(g_log, "BIOMASS_INCREMENT_Beginning of Year\n");
//
//	//compute Basal Area
//	if (years == 0)
//	{
//		logger(g_log, "avdbh = %f\n",s->value[AVDBH] );
//		/* compute basal area in cm^2 */
//		s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2), 2)) * Pi);
//		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
//	}
//	else
//	{
//		oldBasalArea = s->value[BASAL_AREA];
//		s->value[BASAL_AREA] = (((pow((s->value[AVDBH] / 2), 2)) * Pi) / 10000);
//		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
//		logger(g_log, "old basal area = %f \n", oldBasalArea);
//		logger(g_log, " Basal Area Increment= %f m^2/tree \n", s->value[BASAL_AREA] - oldBasalArea);
//		logger(g_log, " Basal Area Increment= %f cm^2/tree \n", (s->value[BASAL_AREA] - oldBasalArea) * 10000);
//
//	}
//
//	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
//	logger(g_log, " BASAL BASAL_AREA_m2 = %f m^2\n", s->value[BASAL_AREA_m2]);
//
//	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
//	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
//	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
//	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
//
//	logger(g_log, "Basal Area for this layer = %f cm^2/tree\n", s->value[BASAL_AREA]*10000);
//	logger(g_log, "Stand Basal Area for this layer = %f m^2/area\n", s->value[STAND_BASAL_AREA]);
//
//	/* sapwood area */
//	/* see Kostner et al in Biogeochemistry of Forested Catchments in
//	 * a Changing Environment, Matzner, Springer for Q. petraea */
//	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
//
//	logger(g_log, "sapwood area from Kostner = %f cm^2\n", s->value[SAPWOOD_AREA]);
//	s->value[HEARTWOOD_AREA] = (s->value[BASAL_AREA] * 10000) - s->value[SAPWOOD_AREA];
//	logger(g_log, "heartwood from Wang et al 2010 = %f cm^2\n", s->value[HEARTWOOD_AREA]);
//
//	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA] / 10000) / s->value[BASAL_AREA];
//	logger(g_log, "Sapwood/Basal Area = %f \n", s->value[SAPWOOD_PERC] );
//	logger(g_log, "Sapwood/Basal Area = %f %%\n",s->value[SAPWOOD_PERC] * 100);
//
//	//compute sap wood pools and heart wood pool in Carbon
//	s->value[STEM_SAPWOOD_C] =  s->value[STEM_C] * s->value[SAPWOOD_PERC];
//	logger(g_log, "Sapwood stem biomass = %f tC/area \n", s->value[STEM_SAPWOOD_C]);
//	s->value[STEM_HEARTWOOD_C] = s->value[STEM_C] - s->value[STEM_SAPWOOD_C];
//	logger(g_log, "Heartwood stem biomass = %f tC/area \n", s->value[STEM_HEARTWOOD_C]);
//	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
//	logger(g_log, "Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
//	s->value[COARSE_ROOT_HEARTWOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_SAPWOOD_C];
//	logger(g_log, "Heartwood coarse root biomass = %f tC/area \n", s->value[COARSE_ROOT_HEARTWOOD_C]);
//	s->value[BRANCH_SAPWOOD_C] =  (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
//	logger(g_log, "Sapwood branch biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
//	s->value[BRANCH_HEARTWOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C];
//	logger(g_log, "Heartwood branch biomass = %f tC/area \n", s->value[BRANCH_HEARTWOOD_C]);
//
//	/*COMPUTE BIOMASS LIVE WOOD*/
//	s->value[LIVE_WOOD_C] = s->value[STEM_LIVE_WOOD_C]+
//			s->value[COARSE_ROOT_LIVE_WOOD_C]+
//			s->value[BRANCH_LIVE_WOOD_C];
//	logger(g_log, "Live biomass = %f tC/area\n", s->value[LIVE_WOOD_C]);
//	s->value[DEAD_WOOD_C] = s->value[STEM_DEAD_WOOD_C]+
//			s->value[COARSE_ROOT_DEAD_WOOD_C]+
//			s->value[BRANCH_DEAD_WOOD_C];
//	logger(g_log, "Dead biomass = %f tC/area\n", s->value[DEAD_WOOD_C]);
//
//	/* check for closure */
//
//	CHECK_CONDITION(fabs((s->value[STEM_SAPWOOD_C] + s->value[STEM_HEARTWOOD_C])-s->value[STEM_C]),>1e-4);
//	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_SAPWOOD_C] + s->value[COARSE_ROOT_HEARTWOOD_C])-s->value[COARSE_ROOT_C]),>1e-4);
//	CHECK_CONDITION(fabs((s->value[BRANCH_SAPWOOD_C] + s->value[BRANCH_HEARTWOOD_C])-s->value[BRANCH_C]),>1e-4);
//}
