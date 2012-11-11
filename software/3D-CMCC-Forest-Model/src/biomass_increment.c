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
