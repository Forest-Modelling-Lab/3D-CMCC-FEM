/*biomass_increment.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Tree_Branch_Bark (SPECIES *s, AGE *a, int heigth, int age, int species)
{

	if (s->value[FRACBB0] == 0)
	{
		Log("I don't have FRACBB0 = FRACBB1 \n");
		s->value[FRACBB0] = s->value[FRACBB1];
		Log("FRACBB0 = %f\n", s->value[FRACBB0]);
	}
	else
	{
		s->value[FRACBB] = s->value[FRACBB1] + (s->value[FRACBB0]- s->value[FRACBB1])* exp(-ln2 * (a->value / s->value[TBB]));

	}

}
void Biomass_increment_BOY (CELL *const c, SPECIES *const s, int height, int age, int years)
{
	double oldBasalArea;

	Log("BIOMASS_INCREMENT_Beginning of Year\n");

	//compute Basal Area
	if (years == 0)
	{
		Log("avdbh = %f\n",s->value[AVDBH] );
		s->value[BASAL_AREA] = (((pow((s->value[AVDBH] / 2), 2)) * Pi) / 10000);
		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	}
	else
	{
		oldBasalArea = s->value[BASAL_AREA];
		s->value[BASAL_AREA] = (((pow((s->value[AVDBH] / 2), 2)) * Pi) / 10000);
		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
		Log("old basal area = %f \n", oldBasalArea);
		Log(" Basal Area Increment= %f m^2/tree \n", s->value[BASAL_AREA] - oldBasalArea);
		Log(" Basal Area Increment= %f cm^2/tree \n", (s->value[BASAL_AREA] - oldBasalArea) * 10000);

	}

	Log("Basal Area for this layer = %f cm^2/tree\n", s->value[BASAL_AREA]*10000);
	Log("Stand Basal Area for this layer = %f m^2/area\n", s->value[STAND_BASAL_AREA]);


	//Log("**Kostner eq** \n");
	//sapwood area
	//see Kostner et al in Biogeochemistry of Forested Catchments in a Changing Environment, Matzner, Springer for Q. petraea
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);

	Log("sapwood area from Kostner = %f cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = (s->value[BASAL_AREA] * 10000) - s->value[SAPWOOD_AREA];
	Log("heartwood from Wang et al 2010 = %f cm^2\n", s->value[HEARTWOOD_AREA]);

	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA] / 10000) / s->value[BASAL_AREA];
	Log("Sapwood/Basal Area = %f \n", s->value[SAPWOOD_PERC] );
	Log("Sapwood/Basal Area = %f %%\n",s->value[SAPWOOD_PERC] * 100);

	//compute sap wood pools and heart wood pool in Carbon
	s->value[STEM_SAPWOOD_C] =  s->value[STEM_C] * s->value[SAPWOOD_PERC];
	Log("Sapwood stem biomass = %f tC/area \n", s->value[STEM_SAPWOOD_C]);
	s->value[STEM_HEARTWOOD_C] = s->value[STEM_C] - s->value[STEM_SAPWOOD_C];
	Log("Heartwood stem biomass = %f tC/area \n", s->value[STEM_HEARTWOOD_C]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	Log("Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	s->value[COARSE_ROOT_HEARTWOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_SAPWOOD_C];
	Log("Heartwood coarse root biomass = %f tC/area \n", s->value[COARSE_ROOT_HEARTWOOD_C]);
	s->value[BRANCH_SAPWOOD_C] =  (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	Log("Sapwood branch biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[BRANCH_HEARTWOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C];
	Log("Heartwood branch biomass = %f tC/area \n", s->value[BRANCH_HEARTWOOD_C]);

	/*COMPUTE BIOMASS LIVE WOOD*/
	s->value[LIVE_WOOD_C] = s->value[STEM_LIVE_WOOD_C]+
			s->value[COARSE_ROOT_LIVE_WOOD_C]+
			s->value[BRANCH_LIVE_WOOD_C];
	Log("Live biomass = %f tC/area\n", s->value[LIVE_WOOD_C]);
	s->value[DEAD_WOOD_C] = s->value[STEM_DEAD_WOOD_C]+
			s->value[COARSE_ROOT_DEAD_WOOD_C]+
			s->value[BRANCH_DEAD_WOOD_C];
	Log("Dead biomass = %f tC/area\n", s->value[DEAD_WOOD_C]);

	/* check for closure */

	CHECK_CONDITION(fabs((s->value[STEM_SAPWOOD_C] + s->value[STEM_HEARTWOOD_C])-s->value[STEM_C]),>1e-4);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_SAPWOOD_C] + s->value[COARSE_ROOT_HEARTWOOD_C])-s->value[COARSE_ROOT_C]),>1e-4);
	CHECK_CONDITION(fabs((s->value[BRANCH_SAPWOOD_C] + s->value[BRANCH_HEARTWOOD_C])-s->value[BRANCH_C]),>1e-4);
}

void Biomass_increment_EOY (CELL *const c, SPECIES *const s, int top_layer, int z, int height, int age)
{
	/*CURRENT ANNUAL INCREMENT-CAI*/
	double MassDensity;
	double dominant_prec_volume;
	double dominated_prec_volume;
	double subdominated_prec_volume;

	//in m^3/area/yr
	//Cai = Volume t1 - Volume t0
	//Mai = Volume t1 / Age

	Log("***CAI & MAI***\n");

	//sergio if prec_volume stands for precedent (year) volume, shouldn't the CAI be quantified before updating prec_volume? in this way it shold alway return ~0, being the product of the same formula expressed with the same quantities

	MassDensity = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TRHO]));
	/*STAND VOLUME-(STEM VOLUME)*/

	//new volume is computed using DM biomass
	s->value[VOLUME] = s->value[STEM_C] * GC_GDM * (1 - s->value[FRACBB]) / MassDensity;


	if (settings->spatial == 'u')
	{
		switch (c->annual_layer_number)
		{
		case 3:
			if (c->heights[height].z == 2)
			{
				dominant_prec_volume = s->value[VOLUME];
				Log("DominantVolume = %f m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				Log("DOMINANT CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI] );
				if (dominant_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %f\n", dominant_prec_volume - s->value[VOLUME]);
				}
			}
			else if (c->heights[height].z == 1)
			{
				dominated_prec_volume = s->value[VOLUME];
				Log("DominatedVolume = %f m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				Log("DOMINATED CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %f m^3/area/yr\n", s->value[MAI] );
				if (dominated_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %f\n", dominated_prec_volume - s->value[VOLUME]);
				}
			}
			else
			{
				subdominated_prec_volume = s->value[VOLUME];
				Log("SubDominatedVolume = %f m^3/cell resolution\n", subdominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - subdominated_prec_volume;
				Log("SUBDOMINATED CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %f m^3/area/yr\n", s->value[MAI] );
				if (subdominated_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %f\n", subdominated_prec_volume - s->value[VOLUME]);
				}
			}
			break;
		case 2:
			if (c->heights[height].z == 1)
			{
				dominant_prec_volume = s->value[VOLUME];
				Log("DominantVolume = %f m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				Log("DOMINANT CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI] );
				if (dominant_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %f\n", dominant_prec_volume - s->value[VOLUME]);
				}
			}
			else
			{
				dominated_prec_volume = s->value[VOLUME];
				Log("DominatedVolume = %f m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				Log("DOMINATED CAI = %f m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %f m^3/area/yr\n", s->value[MAI] );
				if (dominated_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %f\n", dominated_prec_volume - s->value[VOLUME]);
				}
			}

			break;
		case 1:
			/*
			dominant_prec_volume = s->value[BIOMASS_STEM] * (1 - s->value[FRACBB]) /	MassDensity;
			Log("DominantVolume = %f m^3/cell resolution\n", dominant_prec_volume);
			s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
			Log("DOMINANT CAI = %f m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
			Log("MAI-Mean Annual Increment = %f m^3/area/yr \n", s->value[MAI] );

			if (dominant_prec_volume > s->value[VOLUME])
			{
				Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
				Log("prev_volume - VOLUME = %f\n", dominant_prec_volume - s->value[VOLUME]);
			}
			 */
			Log("PREVIOUS Volume = %f m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
			Log("CURRENT Volume = %f m^3/cell resolution\n", s->value[VOLUME]);
			s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
			Log("Yearly Stand CAI = %f m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (double)c->heights[height].ages[age].value ;
			Log("Yearly Stand MAI = %f m^3/area/yr \n", s->value[MAI]);
			break;
		}
	}
	else
	{
		Log("PREVIOUS Volume = %f m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
		s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
		s->value[CAI] = s->value[VOLUME] * s->value[PERC] - s->value[PREVIOUS_VOLUME];
		Log("Yearly Stand CAI = %f m^3/area/yr\n", s->value[CAI]);
		s->value[MAI] = (s->value[VOLUME] * s->value[PERC] )/ (double)c->heights[height].ages[age].value ;
		Log("Yearly Stand MAI = %f m^3/area/yr \n", s->value[MAI] );
	}
	s->value[PREVIOUS_VOLUME] = s->value[VOLUME];
}

void AGB_BGB_biomass (CELL *const c, int height, int age, int species)
{

	Log("**AGB & BGB**\n");
	Log("-for Class\n");
	c->heights[height].ages[age].species[species].value[CLASS_AGB] = c->heights[height].ages[age].species[species].value[TOT_STEM_C]
																														 + c->heights[height].ages[age].species[species].value[LEAF_C];
	Log("Yearly Class AGB = %f tC/area year\n", c->heights[height].ages[age].species[species].value[CLASS_AGB]);
	c->heights[height].ages[age].species[species].value[CLASS_BGB] = c->heights[height].ages[age].species[species].value[TOT_ROOT_C];
	Log("Yearly Class BGB = %f tC/area year\n", c->heights[height].ages[age].species[species].value[CLASS_BGB]);


	Log("-for Stand\n");

	c->stand_agb += c->heights[height].ages[age].species[species].value[CLASS_AGB] * c->heights[height].ages[age].species[species].value[PERC];
	Log("Yearly Stand AGB = %f tC/area year\n", c->stand_agb);
	c->stand_bgb += c->heights[height].ages[age].species[species].value[CLASS_BGB] * c->heights[height].ages[age].species[species].value[PERC];
	Log("Yearly Stand BGB = %f tC/area year\n", c->stand_bgb);
	c->heights[height].ages[age].species[species].value[CLASS_AGB] = 0;
	c->heights[height].ages[age].species[species].value[CLASS_BGB] = 0;

}

void Average_tree_biomass (SPECIES *s)
{
	/* compute tree average biomass */
	s->value[AV_LEAF_MASS_KgC] = (s->value[LEAF_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_STEM_MASS_KgC] = (s->value[STEM_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_ROOT_MASS_KgC] = (s->value[TOT_ROOT_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_FINE_ROOT_MASS_KgC] = (s->value[FINE_ROOT_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_COARSE_ROOT_MASS_KgC] =(s->value[COARSE_ROOT_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_RESERVE_MASS_KgC] = (s->value[RESERVE_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_BRANCH_MASS_KgC] = (s->value[BRANCH_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_LIVE_STEM_MASS_KgC] = (s->value[STEM_LIVE_WOOD_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_DEAD_STEM_MASS_KgC] = (s->value[STEM_DEAD_WOOD_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_LIVE_WOOD_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgC] = (s->value[COARSE_ROOT_DEAD_WOOD_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_LIVE_BRANCH_MASS_KgC] = (s->value[BRANCH_LIVE_WOOD_C]/(double)s->counter[N_TREE])/1000.0;
	s->value[AV_DEAD_BRANCH_MASS_KgC] = (s->value[BRANCH_DEAD_WOOD_C]/(double)s->counter[N_TREE])/1000.0;

}

void Total_class_level_biomass (SPECIES *s)
{
	// Total Biomass less Litterfall and Root turnover
	s->value[TOTAL_W] =  s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] +s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_BRANCH_tDM];
	Log("Total Biomass less Litterfall and Root Turnover = %f tDM/area\n", s->value[TOTAL_W]);

	// Total Biomass less Litterfall and Root turnover
	s->value[TOTAL_C] =  s->value[LEAF_C] + s->value[FINE_ROOT_C] + s->value[COARSE_ROOT_C] +s->value[STEM_C] + s->value[BRANCH_C];
	Log("Total Biomass less Litterfall and Root Turnover = %f tC/area\n", s->value[TOTAL_C]);
}
