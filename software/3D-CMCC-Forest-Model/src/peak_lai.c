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
	//static double oldBasalArea;
	//static double sapwood_perc;

	Log ("\nPEAK LAI FUNCTION\n\n");
	Log("sap leaf = %f\n", s->value[SAP_LEAF]);
	Log("sap area = %f\n", s->value[SAPWOOD_AREA]);
	Log("Leaf Area from Kostner-LPJ = %f m^2\n", (s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]);
	Log("Crown diameter = %f m^2\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);
	Log("Crown Area for Kostner = %f m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);

	s->value[PEAK_Y_LAI] = ((s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_DBHDC_FUNC];
	Log("year %d PEAK LAI from Kostner = %f m^2 m^-2\n",years, s->value[PEAK_Y_LAI]);


	/*for dominant layer with sunlit foliage*/
	if (c->top_layer == c->heights[height].z)
	{
		s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] / (s->value[SLAmkg]* GC_GDM))*(s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)/1000);
		Log("Maximum foliage biomass for sunlit leaves = %f tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE]);
	}
	/*for dominated shaded foliage*/
	else
	{
		s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] / ((s->value[SLAmkg] * s->value[LAI_RATIO]) * GC_GDM))*(s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)/1000);
		Log("Maximum foliage biomass for shaded leaves = %f tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE]);
	}

	/*check compatibility of LAI and Biomass with init data for evergreen*/
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && (day == 0 && month == 0 && years == 0))
	{
		Log("azz\n");
		if ((s->value[LAI] > s->value[PEAK_Y_LAI]) || (s->value[BIOMASS_FOLIAGE] > s->value[MAX_BIOMASS_FOLIAGE]))
		{
			s->value[LAI] = s->value[PEAK_Y_LAI];
			Log("Initial LAI > PEAK LAI, recompute it\n");
			Log("recomputed LAI = %f\n", s->value[LAI]);
			/*then recompute foliage biomass*/
			s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
			Log("recomputed FOLIAGE BIOMASS = %f\n", s->value[BIOMASS_FOLIAGE]);
		}
	}

	//DAILY GPP/NPP
	//cell level
	/*
	if (c->heights_count -1  == 0 && c->heights[height].ages_count -1 == 0 && c->heights[height].ages[age].species_count -1 == 0)
	{
		c->annual_peak_lai = s->value[PEAK_Y_LAI];
	}
	*/


	//fixme useful only for one class per layer

	i = c->heights[height].z;

	c->annual_peak_lai[i] = s->value[PEAK_Y_LAI];




}
