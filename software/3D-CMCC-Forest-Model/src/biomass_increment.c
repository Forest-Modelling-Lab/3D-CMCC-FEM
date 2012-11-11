/*biomass_increment.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

void Get_biomass_increment ( SPECIES *const s, int top_layer, int z, int heights_count, float dominant_prec_volume, float dominated_prec_volume, float subdominated_prec_volume )
{
	/*CURRENT ANNUAL INCREMENT-CAI*/
	Log("***CAI & MAI***\n");

	//in m^3/ha/yr
	//Cai = Volume t1 - Volume t0
	if (heights_count >= 3)//3 heights classes or more
	{
		if (z >= 3)
		{
			s->value[CAI] = s->value[STAND_VOLUME] - dominant_prec_volume;
			Log("DOMINANT CAI = %g m^3/ha/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[STAND_VOLUME] / (float)s->counter[TREE_AGE] ;
			Log("MAI-Mean Annual Increment = %g m^3/ha/yr \n", s->value[MAI] );
		}
		else if (z == 2)
		{
			s->value[CAI] = s->value[STAND_VOLUME] - dominated_prec_volume;
			Log("DOMINATED CAI = %g m^3/ha/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[STAND_VOLUME] / (float)s->counter[TREE_AGE] ;
			Log("MAI-Mean Annual Increment = %g m^3/ha/yr\n", s->value[MAI] );
		}
		else
		{
			s->value[CAI] = s->value[STAND_VOLUME] - subdominated_prec_volume;
			Log("SUBDOMINATED CAI = %g m^3/ha/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[STAND_VOLUME] / (float)s->counter[TREE_AGE] ;
			Log("MAI-Mean Annual Increment = %g m^3/ha/yr\n", s->value[MAI] );
		}

	}
	else
	{
		if (z == 1)
		{
			s->value[CAI] = s->value[STAND_VOLUME] - dominant_prec_volume;
			Log("Previous year volume = %.20g\n", dominant_prec_volume);
			Log("Current year volume = %.20g\n", s->value[STAND_VOLUME]);
			Log("Yearly Stand CAI = %.20g m^3/ha/yr\n", s->value[CAI]);
			s->value[MAI] = s->value[STAND_VOLUME] / (float)s->counter[TREE_AGE] ;
			Log("Yearly Stand MAI = %g m^3/ha/yr\n", s->value[MAI] );
		}
		else
		{
			s->value[CAI] = s->value[STAND_VOLUME] - dominated_prec_volume;
			s->value[MAI] = s->value[STAND_VOLUME] / (float)s->counter[TREE_AGE] ;
			Log("Stand CAI = %g m^3/ha/yr\n", s->value[CAI] );
			Log("Stand MAI = %g m^3/ha/yr\n", s->value[MAI] );
		}
	}
}
