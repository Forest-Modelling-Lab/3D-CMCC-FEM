/*biomass_increment.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Get_tree_BB (CELL *const c,  int years)
{
	int height;
	int age;
	int species;



	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				if (!years && c->heights[height].ages[age].species[species].value[FRACBB0] == 0)
				{
					Log("I don't have FRACBB0 = FRACBB1 \n");
					c->heights[height].ages[age].species[species].value[FRACBB0] = c->heights[height].ages[age].species[species].value[FRACBB1];
					Log("FRACBB0 = %g\n", c->heights[height].ages[age].species[species].value[FRACBB0]);
				}
				else
				{
					c->heights[height].ages[age].species[species].value[FRACBB] = c->heights[height].ages[age].species[species].value[FRACBB1]
											  + (c->heights[height].ages[age].species[species].value[FRACBB0]
											 - c->heights[height].ages[age].species[species].value[FRACBB1])
										 * exp(-ln2 * (c->heights[height].ages[age].value / c->heights[height].ages[age].species[species].value[TBB]));

				}
			}
		}
	}
}

void Get_biomass_increment (CELL *const c, SPECIES *const s, int top_layer, int z, int height, int age)
{
	/*CURRENT ANNUAL INCREMENT-CAI*/
	float MassDensity;
	float dominant_prec_volume;
	float dominated_prec_volume;
	float subdominated_prec_volume;

	//in m^3/area/yr
	//Cai = Volume t1 - Volume t0
	//Mai = Volume t1 / Age

	Log("***CAI & MAI***\n");

	//sergio if prec_volume stands for precedent (year) volume, shouldn't the CAI be quantified before updating prec_volume? in this way it shold alway return ~0, being the product of the same formula expressed with the same quantities

	MassDensity = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TRHO]));
	/*STAND VOLUME-(STEM VOLUME)*/
	s->value[VOLUME] = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) / MassDensity;
	Log("Volume for this class = %g m^3/area\n", s->value[VOLUME]);


	if (settings->spatial == 'u')
	{
		switch (c->annual_layer_number)
		{
		case 3:
			if (c->heights[height].z == 2)
			{
				dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				Log("DOMINANT CAI = %g m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/area/yr \n", s->value[MAI] );
				if (dominant_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %g\n", dominant_prec_volume - s->value[VOLUME]);
				}
			}
			else if (c->heights[height].z == 1)
			{
				dominated_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominatedVolume = %g m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				Log("DOMINATED CAI = %g m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/area/yr\n", s->value[MAI] );
				if (dominated_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %g\n", dominated_prec_volume - s->value[VOLUME]);
				}
			}
			else
			{
				subdominated_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("SubDominatedVolume = %g m^3/cell resolution\n", subdominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - subdominated_prec_volume;
				Log("SUBDOMINATED CAI = %g m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/area/yr\n", s->value[MAI] );
				if (subdominated_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %g\n", subdominated_prec_volume - s->value[VOLUME]);
				}
			}
			break;
		case 2:
			if (c->heights[height].z == 1)
			{
				dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				Log("DOMINANT CAI = %g m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/area/yr \n", s->value[MAI] );
				if (dominant_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %g\n", dominant_prec_volume - s->value[VOLUME]);
				}
			}
			else
			{
				dominated_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominatedVolume = %g m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				Log("DOMINATED CAI = %g m^3/area/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/area/yr\n", s->value[MAI] );
				if (dominated_prec_volume > s->value[VOLUME])
				{
					Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
					Log("prev_volume - VOLUME = %g\n", dominated_prec_volume - s->value[VOLUME]);
				}
			}

			break;
		case 1:
			/*
			dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
			Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
			s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
			Log("DOMINANT CAI = %g m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
			Log("MAI-Mean Annual Increment = %g m^3/area/yr \n", s->value[MAI] );

			if (dominant_prec_volume > s->value[VOLUME])
			{
				Log("ERROR IN CAI FUNCTION!!!!\nprev_volume > VOLUME\n");
				Log("prev_volume - VOLUME = %g\n", dominant_prec_volume - s->value[VOLUME]);
			}
			*/
			Log("PREVIOUS Volume = %g m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
			Log("CURRENT Volume = %g m^3/cell resolution\n", s->value[VOLUME]);
			s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
			Log("Yearly Stand CAI = %g m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
			Log("Yearly Stand MAI = %g m^3/area/yr \n", s->value[MAI]);
			break;
		}
	}
	else
	{
		if (settings->presence == 't')
		{
			Log("PREVIOUS Volume = %g m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
			Log("CURRENT Volume = %g m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
			s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
			Log("Yearly Stand CAI = %g m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
			Log("Yearly Stand MAI = %g m^3/area/yr \n", s->value[MAI]);
		}
		else
		{
			Log("PREVIOUS Volume = %g m^3/cell resolution\n", s->value[PREVIOUS_VOLUME]);
			s->value[CAI] = s->value[VOLUME] - s->value[PREVIOUS_VOLUME];
			s->value[CAI] = s->value[VOLUME] * s->value[PERC] - s->value[PREVIOUS_VOLUME];
			Log("Yearly Stand CAI = %g m^3/area/yr\n", s->value[CAI]);
			s->value[MAI] = (s->value[VOLUME] * s->value[PERC] )/ (float)c->heights[height].ages[age].value ;
			Log("Yearly Stand MAI = %g m^3/area/yr \n", s->value[MAI] );
		}
	}
}

void Get_AGB_BGB_biomass (CELL *const c, int height, int age, int species)
{

	Log("**AGB & BGB**\n");
	Log("-for Class\n");
	c->heights[height].ages[age].species[species].value[CLASS_AGB] = c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]
																	 + c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM];
	Log("Yearly Class AGB = %g tDM/area year\n", c->heights[height].ages[age].species[species].value[CLASS_AGB]);
	c->heights[height].ages[age].species[species].value[CLASS_BGB] = c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM];
	Log("Yearly Class BGB = %g tDM/area year\n", c->heights[height].ages[age].species[species].value[CLASS_BGB]);


	Log("-for Stand\n");
	if (settings->presence == 't')
	{
		c->stand_agb += c->heights[height].ages[age].species[species].value[CLASS_AGB];
		Log("Yearly Stand AGB = %g tDM/area year\n", c->stand_agb);
		c->stand_bgb += c->heights[height].ages[age].species[species].value[CLASS_BGB];
		Log("Yearly Stand BGB = %g tDM/area year\n", c->stand_bgb);
	}
	else
	{
		c->stand_agb += c->heights[height].ages[age].species[species].value[CLASS_AGB] * c->heights[height].ages[age].species[species].value[PERC];
		Log("Yearly Stand AGB = %g tDM/area year\n", c->stand_agb);
		c->stand_bgb += c->heights[height].ages[age].species[species].value[CLASS_BGB] * c->heights[height].ages[age].species[species].value[PERC];
		Log("Yearly Stand BGB = %g tDM/area year\n", c->stand_bgb);
	}
	c->heights[height].ages[age].species[species].value[CLASS_AGB] = 0;
	c->heights[height].ages[age].species[species].value[CLASS_BGB] = 0;

}

extern void Get_average_biomass (SPECIES *s)
{

	s->value[AV_STEM_MASS] = s->value[BIOMASS_STEM_CTEM] * 	1000 / s->counter[N_TREE];
	s->value[AV_ROOT_MASS] = s->value[BIOMASS_ROOTS_TOT_CTEM] * 1000 / s->counter[N_TREE];

	Log("Average Stem Mass = %g kgDM stem /tree\n", s->value[AV_STEM_MASS]);
}

extern void Get_total_class_level_biomass (SPECIES *s)
{
	// Total Biomass less Litterfall and Root turnover
	s->value[TOTAL_W] =  s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] +s->value[BIOMASS_STEM_CTEM];
	Log("Total Biomass less Litterfall and Root Turnover = %g tDM/area\n", s->value[TOTAL_W]);
}
