/* biomass.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "biomass.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void live_total_wood_age(const age_t *const a, const int species)
{
	/* this function update based on current tree age the amount of live:total wood ratio
	 * based on the assumption that the live wood decrease linearly increasing age */
	/* e.g. for Fagus sylvatica  base on simulation for Hesse site (age 30) and Collelongo site (age 160)*/
	// fixme values should be included in species.txt
	double max_live_total_ratio;
	double min_live_total_ratio;

	//01 June 2016 changed from 160 to MAX_AGE
	double max_age;
	double min_age = 30;

	double t1;
	double t2;

	species_t *s;

	s = &a->species[species];

	// fixme values should be included in species.txt
	max_live_total_ratio = s->value[LIVE_TOTAL_WOOD]; /* for min_age = 30 */
	min_live_total_ratio = 0.04; /* for max_age = 160 */

	//01 June 2016 changed from 160 to MAX_AGE
	max_age = s->value[MAXAGE]; /* for min_live_total_wood = 0.04 */


	//fixme it should be included in the species.txt files */
	/* for deciduous */
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
	t2 = max_age - min_age;

	s->value[EFF_LIVE_TOTAL_WOOD_FRAC] = (t1/t2)*(max_age - a->value) + min_live_total_ratio;
	logger(g_log, "Effective live:total wood fraction based on stand age = %f\n", s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
}


void Biomass_increment_EOY(cell_t *const c, const int layer, const int height, const int age, const int species)
{
	/*CURRENT ANNUAL INCREMENT-CAI*/
	double MassDensity;
	double dominant_prec_volume;
	double dominated_prec_volume;
	double subdominated_prec_volume;

	age_t *a;
	species_t *s;

	a = &c->heights[height].ages[age];
	s = &a->species[species];

	//in m^3/area/yr
	//Cai = Volume t1 - Volume t0
	//Mai = Volume t1 / Age

	logger(g_log, "***CAI & MAI***\n");

	//sergio if prec_volume stands for precedent (year) volume, shouldn't the CAI be quantified before updating prec_volume? in this way it shold alway return ~0, being the product of the same formula expressed with the same quantities

	MassDensity = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TRHO]));
	/*STAND VOLUME-(STEM VOLUME)*/

	//new volume is computed using DM biomass
	s->value[VOLUME] = s->value[STEM_C] * GC_GDM * (1 - s->value[FRACBB]) / MassDensity;


	if (g_settings->spatial == 'u')
	{
		//ALESSIOC
		//switch (c->annual_layer_number)
		// ALESSIOR: mi raccomando ALESSIOC rimuovi la riga qui sotto ( SOLO LA RIGA! )
		switch ( 0 )
		{
		case 3://ALESSIOC CHECK IT!!
			//if (c->t_layers[layer].z == c->heights[height].z)
			if( layer == c->heights[height].z )
			{
				dominant_prec_volume = s->value[VOLUME];
				logger(g_log, "DominantVolume = %f m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				logger(g_log, "DOMINANT CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)a->value;
				logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI] );
				if (dominant_prec_volume > s->value[VOLUME])
				{
					logger(g_log, "ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					logger(g_log, "prev_volume - VOLUME = %f\n", dominant_prec_volume - s->value[VOLUME]);
				}
			}//ALESSIOC CHECK IT!!
			// ALESSIOR: LA CONDIZIONE E' IDENTICA A QUELLA SOPRA!!!!!!
			//else if (c->t_layers[layer].z == c->heights[height].z)
			else if( layer == c->heights[height].z )
			{
				dominated_prec_volume = s->value[VOLUME];
				logger(g_log, "DominatedVolume = %f m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				logger(g_log, "DOMINATED CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)a->value;
				logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr\n", s->value[MAI] );
				if (dominated_prec_volume > s->value[VOLUME])
				{
					logger(g_log, "ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					logger(g_log, "prev_volume - VOLUME = %f\n", dominated_prec_volume - s->value[VOLUME]);
				}
			}
			else
			{
				subdominated_prec_volume = s->value[VOLUME];
				logger(g_log, "SubDominatedVolume = %f m^3/cell resolution\n", subdominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - subdominated_prec_volume;
				logger(g_log, "SUBDOMINATED CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)a->value;
				logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr\n", s->value[MAI] );
				if (subdominated_prec_volume > s->value[VOLUME])
				{
					logger(g_log, "ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					logger(g_log, "prev_volume - VOLUME = %f\n", subdominated_prec_volume - s->value[VOLUME]);
				}
			}
			break;
		case 2://ALESSIOC CHECK IT!!
			//if (c->t_layers[layer].z == c->heights[height].z)
			if( layer == c->heights[height].z )
			{
				dominant_prec_volume = s->value[VOLUME];
				logger(g_log, "DominantVolume = %f m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				logger(g_log, "DOMINANT CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)a->value;
				logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI] );
				if (dominant_prec_volume > s->value[VOLUME])
				{
					logger(g_log, "ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					logger(g_log, "prev_volume - VOLUME = %f\n", dominant_prec_volume - s->value[VOLUME]);
				}
			}
			else
			{
				dominated_prec_volume = s->value[VOLUME];
				logger(g_log, "DominatedVolume = %f m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				logger(g_log, "DOMINATED CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)a->value;
				logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr\n", s->value[MAI] );
				if (dominated_prec_volume > s->value[VOLUME])
				{
					logger(g_log, "ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					logger(g_log, "prev_volume - VOLUME = %f\n", dominated_prec_volume - s->value[VOLUME]);
				}
			}

			break;
		case 1:
			/*
			dominant_prec_volume = s->value[BIOMASS_STEM] * (1 - s->value[FRACBB]) /	MassDensity;
			logger(g_log, "DominantVolume = %f m^3/cell resolution\n", dominant_prec_volume);
			s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
			logger(g_log, "DOMINANT CAI = %f m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (double)c->t_layers[layer].heights[height].ages[age].value ;
			logger(g_log, "MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI] );

			if (dominant_prec_volume > s->value[VOLUME])
			{
				logger(g_log, "ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
				logger(g_log, "prev_volume - VOLUME = %f\n", dominant_prec_volume - s->value[VOLUME]);
			}
			 */
			logger(g_log, "PREVIOUS Volume = %f m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
			logger(g_log, "CURRENT Volume = %f m^3/cell resolution\n", s->value[VOLUME]);
			s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
			logger(g_log, "Yearly Stand CAI = %f m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (double)a->value;
			logger(g_log, "Yearly Stand MAI = %f m^3/area/yr \n", s->value[MAI]);
			break;
		}
	}
	else
	{
		logger(g_log, "PREVIOUS Volume = %f m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
		s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
		s->value[CAI] = s->value[VOLUME] * s->value[PERC] - s->value[PREVIOUS_VOLUME];
		logger(g_log, "Yearly Stand CAI = %f m^3/area/yr\n", s->value[CAI]);
		s->value[MAI] = (s->value[VOLUME] * s->value[PERC] )/ (double)a->value ;
		logger(g_log, "Yearly Stand MAI = %f m^3/area/yr \n", s->value[MAI] );
	}
	s->value[PREVIOUS_VOLUME] = s->value[VOLUME];
}

void AGB_BGB_biomass(cell_t *const c, const int height, const int age, const int species)
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

void Average_tree_biomass(species_t *const s)
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
