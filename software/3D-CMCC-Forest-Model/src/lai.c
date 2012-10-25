/*lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

//TO COMPUTE YEARLY PEAK LAI FROM PROVIOUS YEARLY LAI

void Get_initial_month_lai (SPECIES *const s)
{
	if (s->counter[VEG_MONTHS]  == 1)
	{
		s->value[LAI] = (s->value[BIOMASS_RESERVE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * s->value[SLAmkg];
		Log("Reserves pools = %g tDM/ha\n", s->value [BIOMASS_RESERVE_CTEM]);
		Log("++Lai from reserves = %g\n", s->value[LAI]);
	}
	else
	{
		Log("++Lai = %g\n", s->value[LAI]);
	}


}

void Get_end_month_lai (SPECIES *const s)
{
	s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * s->value[SLAmkg];
	Log("++Lai = %g\n", s->value[LAI]);
}
