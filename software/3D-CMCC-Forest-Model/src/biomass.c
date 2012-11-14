/*biomass_increment.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

void Get_biomass_increment (CELL *const c, SPECIES *const s, int top_layer, int z, int height, int age)
{
	/*CURRENT ANNUAL INCREMENT-CAI*/

	//in m^3/ha/yr
	//Cai = Volume t1 - Volume t0
	//Mai = Volume t1 / Age

	Log("***CAI & MAI***\n");

	float MassDensity;
	float dominant_prec_volume;
	float dominated_prec_volume;
	float subdominated_prec_volume;

	MassDensity = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TRHO]));
	/*STAND VOLUME-(STEM VOLUME)*/
	s->value[VOLUME] = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) / MassDensity;
	Log("Volume for this class = %g m^3/ha\n", s->value[VOLUME]);


	if (settings->version == 'u')
	{
		switch (c->annual_layer_number)
		{
		case 3:
			if (c->heights[height].z == 2)
			{
				dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				Log("DOMINANT CAI = %g m^3/ha/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/ha/yr \n", s->value[MAI] );
			}
			else if (c->heights[height].z == 1)
			{
				dominated_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominatedVolume = %g m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				Log("DOMINATED CAI = %g m^3/ha/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/ha/yr\n", s->value[MAI] );
			}
			else
			{
				subdominated_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("SubDominatedVolume = %g m^3/cell resolution\n", subdominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - subdominated_prec_volume;
				Log("SUBDOMINATED CAI = %g m^3/ha/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/ha/yr\n", s->value[MAI] );
			}
			break;
		case 2:
			if (c->heights[height].z == 1)
			{
				dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
				Log("DOMINANT CAI = %g m^3/ha/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/ha/yr \n", s->value[MAI] );
			}
			else
			{
				dominated_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
				Log("DominatedVolume = %g m^3/cell resolution\n", dominated_prec_volume);
				s->value[CAI] = s->value[VOLUME] - dominated_prec_volume;
				Log("DOMINATED CAI = %g m^3/ha/yr\n", s->value[CAI]);
				s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
				Log("MAI-Mean Annual Increment = %g m^3/ha/yr\n", s->value[MAI] );
			}

			break;
		case 1:
			dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
			Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
			s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
			Log("DOMINANT CAI = %g m^3/ha/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
			Log("MAI-Mean Annual Increment = %g m^3/ha/yr \n", s->value[MAI] );
			break;
		}
	}
	else
	{
		dominant_prec_volume = s->value[BIOMASS_STEM_CTEM] * (1 - s->value[FRACBB]) /	MassDensity;
		Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
		s->value[CAI] = s->value[VOLUME] - dominant_prec_volume;
		Log("Yearly Stand CAI = %g m^3/ha/yr\n", s->value[CAI]);
		s->value[MAI] = s->value[VOLUME] / (float)c->heights[height].ages[age].value ;
		Log("Yearly Stand MAI = %g m^3/ha/yr \n", s->value[MAI] );
	}
}

void Get_AGB_BGB_biomass (CELL *const c, int height, int age, int species)
{

	Log("**AGB & BGB**\n");
	Log("-for Class\n");
	c->heights[height].ages[age].species[species].value[CLASS_AGB] = c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]
																	 + c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM];
	Log("Yearly Class AGB = %g tDM/ha year\n", c->heights[height].ages[age].species[species].value[CLASS_AGB]);
	c->heights[height].ages[age].species[species].value[CLASS_BGB] = c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM];
	Log("Yearly Class BGB = %g tDM/ha year\n", c->heights[height].ages[age].species[species].value[CLASS_BGB]);


	Log("-for Stand\n");
	c->stand_agb += c->heights[height].ages[age].species[species].value[CLASS_AGB];
	Log("Yearly Stand AGB = %g tDM/ha year\n", c->stand_agb);
	c->stand_bgb += c->heights[height].ages[age].species[species].value[CLASS_BGB];
	Log("Yearly Stand BGB = %g tDM/ha year\n", c->stand_bgb);
	c->heights[height].ages[age].species[species].value[CLASS_AGB] = 0;
	c->heights[height].ages[age].species[species].value[CLASS_BGB] = 0;

}

extern void Get_average_biomass (SPECIES *s)
{

	s->value[AV_STEM_MASS] = s->value[BIOMASS_STEM_CTEM] * 	1000 / s->counter[N_TREE];
	s->value[AV_ROOT_MASS] = s->value[BIOMASS_ROOTS_TOT_CTEM] * 1000 / s->counter[N_TREE];

	Log("Average Stem Mass = %g kgDM stem /tree\n", s->value[AV_STEM_MASS]);
}
