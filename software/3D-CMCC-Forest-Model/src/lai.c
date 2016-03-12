/*lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Daily_lai (SPECIES *const s)
{
	double leaf_c; //leaf carbon KgC/m^2

	Log("DAILY_LAI\n");

	/*CONVERT tDM/cell to KgC/m^2*/
	leaf_c = (s->value[BIOMASS_FOLIAGE] * 1000.0 * (1.0/GC_GDM));

	s->value[LAI] = (leaf_c * s->value[SLA_AVG])/(s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
	Log("LAI = %f\n", s->value[LAI]);
	s->value[ALL_LAI] = s->value[LAI] * s->value[LAI_RATIO];
	//Log("ALL LAI BIOME = %f\n", s->value[ALL_LAI]);

	/* Calculate projected LAI for sunlit and shaded canopy portions */
	s->value[LAI_SUN] = 1.0 - exp(-s->value[LAI]);
	s->value[LAI_SHADE] = s->value[LAI] - s->value[LAI_SUN];
	Log("LAI SUN = %f\n", s->value[LAI_SUN]);
	Log("LAI SHADE = %f\n", s->value[LAI_SHADE]);

	Log("SLA_RATIO = %f\n", s->value[SLA_RATIO]);

	/*compute SLA for SUN and SHADED*/
	s->value[SLA_SUN] = (s->value[LAI_SUN] + (s->value[LAI_SHADE]/s->value[SLA_RATIO]))
						/(leaf_c /(s->value[CANOPY_COVER_DBHDC] * settings->sizeCell));
	Log("SLA SUN = %f m^2/KgC\n", s->value[SLA_SUN]);
	s->value[SLA_SHADE] = s->value[SLA_SUN] * s->value[SLA_RATIO];
	Log("SLA SHADE = %f m^2/KgC\n", s->value[SLA_SHADE]);

	if(s->value[LAI] < 0.0)
	{
		ERROR(s->value[LAI], "LAI < 0.0");
	}
	if(s->value[LAI_SUN] < 0.0)
	{
		ERROR(s->value[LAI_SUN], "LAI_SUN < 0.0");
	}
	if(s->value[LAI_SHADE] < 0.0)
	{
		ERROR(s->value[LAI_SHADE], "LAI_SHADE < 0.0");
	}


}


