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

	/* compute annual Peak Projected Lai (m2/m2) */
	s->value[PEAK_LAI_PROJ] = ((s->value[SAPWOOD_AREA] / 10000.) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_PROJ];
	logger(g_debug_log, "PEAK_LAI_PROJ from Kostner = %f m2/m2\n",s->value[PEAK_LAI_PROJ]);

	/* check if Peak Lai exceeds Maximum prescribed Peak Lai (this shouldn't happen) */
	if (s->value[PEAK_LAI_PROJ] > MAX_PEAK_LAI_PROJ) s->value[PEAK_LAI_PROJ] = MAX_PEAK_LAI_PROJ;

	/* compute max leaf carbon (tC/cell) at Peak Projected Lai */
	s->value[MAX_LEAF_C]   = (s->value[PEAK_LAI_PROJ] / s->value[SLA_AVG]) /1000. * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
	logger(g_debug_log, "MAX_LEAF_C (sun and shaded)= %f tC/area \n", s->value[MAX_LEAF_C]);
	printf("MAX_LEAF_C (sun and shaded)= %f tC/area \n", s->value[MAX_LEAF_C]);getchar();

	/* then compute fine root carbon (tC/cell) at Peak Projected Lai */
	s->value[MAX_FROOT_C]  = s->value[MAX_LEAF_C] * (1.0 - s->value[FROOT_LEAF_FRAC]);
	logger(g_debug_log, "MAX_FINE_ROOT_C = %f tC/area \n", s->value[MAX_FROOT_C]);

	/***************************************************************************************************************/
	/* note: special case for evergreen */

	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && ( !day && !month && !years ))
	{
		if ((s->value[LAI_PROJ] > s->value[PEAK_LAI_PROJ]) || (s->value[LEAF_C] > s->value[MAX_LEAF_C]))
		{
			/* force current LAI to Peak Lai */
			s->value[LAI_PROJ] = s->value[PEAK_LAI_PROJ];
			logger(g_debug_log, "Initial LAI > PEAK LAI, recompute it\n");
			logger(g_debug_log, "recomputed LAI_PROJ = %f m2/m2\n", s->value[LAI_PROJ]);

			/* then recompute leaf biomass */
			s->value[LEAF_C] = s->value[MAX_LEAF_C];
			logger(g_debug_log, "recomputed leaf mass = %f tC\n", s->value[LEAF_C]);

			/* then recompute fine root biomass */
			s->value[FROOT_C] = s->value[MAX_FROOT_C];
			logger(g_debug_log, "recomputed fine root mass = %f tC\n", s->value[FROOT_C]);
		}
	}
}
