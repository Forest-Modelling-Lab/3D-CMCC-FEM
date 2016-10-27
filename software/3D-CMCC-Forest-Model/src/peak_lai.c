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
extern logger_t* g_debug_log;

void peak_lai(species_t *const s, const int day, const int month, const int years)
{

	logger(g_debug_log, "\n**PEAK LAI**\n");

	/* compute peak lai */
	s->value[PEAK_LAI_PROJ] = ((s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_PROJ];
	logger(g_debug_log, "year %d PEAK_LAI_PROJ from Kostner = %f m2/m2\n",years, s->value[PEAK_LAI_PROJ]);

	/* compute max leaf carbon at peak lai */
	s->value[MAX_LEAF_C] = (s->value[PEAK_LAI_PROJ] / s->value[SLA_AVG]) /1000.0 * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
	logger(g_debug_log, "MAX_LEAF_C (sun and shaded)= %f tC/area \n", s->value[MAX_LEAF_C]);

	s->value[MAX_FINE_ROOT_C] =s->value[MAX_LEAF_C] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
	logger(g_debug_log, "MAX_FINE_ROOT_C = %f tC/area \n", s->value[MAX_LEAF_C]);

	/***************************************************************************************************************/
	/* note: special case for evergreen */

	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && ( !day && !month && !years ))
	{
		if ((s->value[LAI_PROJ] > s->value[PEAK_LAI_PROJ]) || (s->value[LEAF_C] > s->value[MAX_LEAF_C]))
		{
			s->value[LAI_PROJ] = s->value[PEAK_LAI_PROJ];
			logger(g_debug_log, "Initial LAI > PEAK LAI, recompute it\n");
			logger(g_debug_log, "recomputed LAI_PROJ = %f\n", s->value[LAI_PROJ]);

			/*then recompute leaf biomass*/
			s->value[LEAF_C] = s->value[MAX_LEAF_C];
			logger(g_debug_log, "recomputed leaf mass = %f tC\n", s->value[MAX_LEAF_C]);
		}
	}
	/*
	//used in LEAFFALL MARCONI function
	s->value[MAX_FINE_ROOT_C] = s->value[MAX_LEAF_C] * s->value[FINE_ROOT_LEAF_FRAC];
	s->value[MAX_BUD_BURST_C] = s->value[MAX_LEAF_C] + s->value[MAX_FINE_ROOT_C];
	logger(g_debug_log, "MAX_BIOMASS_FOLIAGE = %f tC/cell\n", s->value[MAX_LEAF_C]);
	logger(g_debug_log, "MAX_BIOMASS_FINE_ROOTS = %f tC/cell\n", s->value[MAX_FINE_ROOT_C]);
	logger(g_debug_log, "MAX_BUD_BURST_C = %f tC/cell\n", s->value[MAX_BUD_BURST_C]);

	//used in LEAFFALL MARCONI function
	s->value[MAX_BIOMASS_FINE_ROOTS_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM] * s->value[FINE_ROOT_LEAF_FRAC];
	s->value[MAX_BIOMASS_BUDBURST_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM] + s->value[MAX_BIOMASS_FINE_ROOTS_tDM];
	*/

	/* check for reserve need for budburst (deciduous)*/
	//if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2) CHECK_CONDITION(s->value[RESERVE_C], < s->value[MAX_LEAF_C]);

}
