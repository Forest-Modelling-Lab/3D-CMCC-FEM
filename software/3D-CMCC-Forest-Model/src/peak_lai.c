/*peak_lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_peak_lai_from_pipe_model (SPECIES *const s, CELL *const c, int years, int month, int day, int height, int age)
{
	int i;
	double max_leafc;
	//static double oldBasalArea;
	//static double sapwood_perc;

	Log ("\nPEAK LAI FUNCTION\n");
	Log("sap leaf = %f\n", s->value[SAP_LEAF]);
	Log("sap area = %f\n", s->value[SAPWOOD_AREA]);
	Log("Leaf Area from Kostner-LPJ = %f m^2\n", (s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]);
	Log("Crown diameter = %f m^2\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);
	Log("Crown Area for Kostner = %f m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);

	s->value[PEAK_LAI] = ((s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_DBHDC_FUNC];
	Log("year %d PEAK LAI from Kostner = %f m^2 m^-2\n",years, s->value[PEAK_LAI]);


	max_leafc = s->value[PEAK_LAI] / s->value[SLA_AVG];
	Log("Maximum foliage biomass (sun and shaded)= %f KgC/m^2 \n", max_leafc);

	s->value[MAX_BIOMASS_FOLIAGE] = ((max_leafc / 1000.0) * GC_GDM) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
	Log("Maximum foliage biomass (sun and shaded)= %f tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE]);


	/*check compatibility of LAI and Biomass with init data for evergreen*/
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && (day == 0 && month == 0 && years == 0))
	{
		Log("azz\n");
		if ((s->value[LAI] > s->value[PEAK_LAI]) || (s->value[BIOMASS_FOLIAGE] > s->value[MAX_BIOMASS_FOLIAGE]))
		{
			s->value[LAI] = s->value[PEAK_LAI];
			Log("Initial LAI > PEAK LAI, recompute it\n");
			Log("recomputed LAI = %f\n", s->value[LAI]);
			/*then recompute foliage biomass*/
			s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
			Log("recomputed FOLIAGE BIOMASS = %f\n", s->value[BIOMASS_FOLIAGE]);
		}
	}

	Log("BIOMASS_RESERVE = %f tDM/area\n", s->value[RESERVE]);

	s->value[MAX_BIOMASS_BUDBURST] = s->value[MAX_BIOMASS_FOLIAGE] / (1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
	s->value[MAX_BIOMASS_FINE_ROOTS] = s->value[MAX_BIOMASS_BUDBURST] - s->value[MAX_BIOMASS_FOLIAGE];
	Log("MAX_BIOMASS_FOLIAGE = %f tDM/area\n", s->value[MAX_BIOMASS_FOLIAGE]);
	Log("MAX_BIOMASS_BUDBURST = %f tDM/area\n", s->value[MAX_BIOMASS_BUDBURST]);
	Log("MAX_BIOMASS_FINE_ROOTS = %f tDM/area\n", s->value[MAX_BIOMASS_FINE_ROOTS]);

	/*check for reserve need for budburst*/
	if(s->value[RESERVE] >= (s->value[MAX_BIOMASS_BUDBURST]/2.0))
	{
		Log("There are enough reserve to reach 0.5 Peak Lai\n");
	}
	else
	{
		Log("There areNT enough reserve to reach 0.5 Peak Lai\n");
	}

	//fixme useful only for one class per layer

	i = c->heights[height].z;

	c->annual_peak_lai[i] = s->value[PEAK_LAI];
}
