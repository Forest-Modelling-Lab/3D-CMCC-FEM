/*peak_lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Peak_lai(SPECIES *const s, CELL *const c, int years, int month, int day, int height, int age)
{
	int i;
	double max_leafc;

	Log ("\n**PEAK LAI**\n");

	s->value[PEAK_LAI] = ((s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_DBHDC_FUNC];
	Log("year %d PEAK LAI from Kostner = %f m^2 m^-2\n",years, s->value[PEAK_LAI]);

	s->value[MAX_LEAF_C] = (s->value[PEAK_LAI] / s->value[SLA_AVG]) /1000.0 * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
	Log("MAX_LEAF_C (sun and shaded)= %f tC/area \n", s->value[MAX_LEAF_C]);

	/*check compatibility of LAI and Biomass with init data for evergreen*/
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && (day == 0 && month == 0 && years == 0))
	{
		if ((s->value[LAI] > s->value[PEAK_LAI]) || (s->value[LEAF_C] > s->value[MAX_LEAF_C]))
		{
			s->value[LAI] = s->value[PEAK_LAI];
			Log("Initial LAI > PEAK LAI, recompute it\n");
			Log("recomputed LAI = %f\n", s->value[LAI]);
			/*then recompute foliage biomass*/
			s->value[LEAF_C] = s->value[MAX_LEAF_C];
			Log("recomputed leaf mass = %f tC\n", s->value[MAX_LEAF_C]);
		}
	}
	//ued in LEAFFALL MARCONI function
	s->value[MAX_FINE_ROOT_C] = s->value[MAX_LEAF_C] * s->value[FINE_ROOT_LEAF_FRAC];
	s->value[MAX_BUD_BURST_C] = s->value[MAX_LEAF_C] + s->value[MAX_FINE_ROOT_C];
	Log("MAX_BIOMASS_FOLIAGE = %f tC/cell\n", s->value[MAX_LEAF_C]);
	Log("MAX_BIOMASS_FINE_ROOTS = %f tC/cell\n", s->value[MAX_FINE_ROOT_C]);
	Log("MAX_BUD_BURST_C = %f tC/cell\n", s->value[MAX_BUD_BURST_C]);

	//ued in LEAFFALL MARCONI function
	s->value[MAX_BIOMASS_FINE_ROOTS_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM] * s->value[FINE_ROOT_LEAF_FRAC];
	s->value[MAX_BIOMASS_BUDBURST_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM] + s->value[MAX_BIOMASS_FINE_ROOTS_tDM];
//	Log("MAX_BIOMASS_FOLIAGE = %f tDM/cell\n", s->value[MAX_BIOMASS_FOLIAGE_tDM]);
//	Log("MAX_BIOMASS_FINE_ROOTS = %f tDM/cell\n", s->value[MAX_BIOMASS_FINE_ROOTS_tDM]);
//	Log("MAX_BIOMASS_BUDBURST = %f tDM/cell\n", s->value[MAX_BIOMASS_BUDBURST_tDM]);

	/*check for reserve need for budburst*/
	//
	if(s->value[RESERVE_C] >= (s->value[MAX_LEAF_C]))
	{
		Log("There are enough reserve to reach Peak Lai\n");
	}
	else
	{
		Log("There areNT enough reserve to reach Peak Lai\n");
		exit(1);
	}

	i = c->heights[height].z;
	c->annual_peak_lai[i] = s->value[PEAK_LAI];
}
