/*peak_lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "peak_lai.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void Peak_lai(species_t *const s, const int day, const int month, const int years)
{
	//int i;

	logger(g_log, "\n**PEAK LAI**\n");

	s->value[PEAK_LAI] = ((s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_DBHDC_FUNC];
	logger(g_log, "year %d PEAK LAI from Kostner = %f m2/m2\n",years, s->value[PEAK_LAI]);

	s->value[MAX_LEAF_C] = (s->value[PEAK_LAI] / s->value[SLA_AVG]) /1000.0 * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell);
	logger(g_log, "MAX_LEAF_C (sun and shaded)= %f tC/area \n", s->value[MAX_LEAF_C]);

	s->value[MAX_FINE_ROOT_C] =s->value[MAX_LEAF_C] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
	logger(g_log, "MAX_FINE_ROOT_C = %f tC/area \n", s->value[MAX_LEAF_C]);

	/* check compatibility of LAI and Biomass with initial data for evergreen */
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && (day == 0 && month == 0 && years == 0))
	{
		if ((s->value[LAI] > s->value[PEAK_LAI]) || (s->value[LEAF_C] > s->value[MAX_LEAF_C]))
		{
			s->value[LAI] = s->value[PEAK_LAI];
			logger(g_log, "Initial LAI > PEAK LAI, recompute it\n");
			logger(g_log, "recomputed LAI = %f\n", s->value[LAI]);

			/*then recompute foliage biomass*/
			s->value[LEAF_C] = s->value[MAX_LEAF_C];
			logger(g_log, "recomputed leaf mass = %f tC\n", s->value[MAX_LEAF_C]);
		}
	}
	//used in LEAFFALL MARCONI function
	s->value[MAX_FINE_ROOT_C] = s->value[MAX_LEAF_C] * s->value[FINE_ROOT_LEAF_FRAC];
	s->value[MAX_BUD_BURST_C] = s->value[MAX_LEAF_C] + s->value[MAX_FINE_ROOT_C];
	logger(g_log, "MAX_BIOMASS_FOLIAGE = %f tC/cell\n", s->value[MAX_LEAF_C]);
	logger(g_log, "MAX_BIOMASS_FINE_ROOTS = %f tC/cell\n", s->value[MAX_FINE_ROOT_C]);
	logger(g_log, "MAX_BUD_BURST_C = %f tC/cell\n", s->value[MAX_BUD_BURST_C]);

	//used in LEAFFALL MARCONI function
	s->value[MAX_BIOMASS_FINE_ROOTS_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM] * s->value[FINE_ROOT_LEAF_FRAC];
	s->value[MAX_BIOMASS_BUDBURST_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM] + s->value[MAX_BIOMASS_FINE_ROOTS_tDM];

	/* check for reserve need for budburst (deciduous)*/
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2) CHECK_CONDITION(s->value[RESERVE_C], < s->value[MAX_LEAF_C]);

}
