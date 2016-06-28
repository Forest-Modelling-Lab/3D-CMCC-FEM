/*lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lai.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void Daily_lai (species_t *const s)
{
	double leaf_c;                             //leaf carbon KgC/m^2

	logger(g_log, "\n**daily LAI**\n");

	/* convert tC/cell to KgC/m^2 */
	leaf_c = s->value[LEAF_C] * 1000.0 ;

	logger(g_log, "Leaf Biomass = %f KgC/cell\n", leaf_c);

	s->value[LAI] = (leaf_c * s->value[SLA_AVG])/(s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell);
	logger(g_log, "LAI = %f\n", s->value[LAI]);
	s->value[ALL_LAI] = s->value[LAI] * s->value[LAI_RATIO];
	//logger(g_log, "ALL LAI BIOME = %f\n", s->value[ALL_LAI]);

	/* Calculate projected LAI for sunlit and shaded canopy portions */
	s->value[LAI_SUN] = 1.0 - exp(-s->value[LAI]);
	s->value[LAI_SHADE] = s->value[LAI] - s->value[LAI_SUN];
	logger(g_log, "LAI SUN = %f\n", s->value[LAI_SUN]);
	logger(g_log, "LAI SHADE = %f\n", s->value[LAI_SHADE]);

	/*compute SLA for SUN and SHADED*/
	if(s->value[LAI_SUN] > 0.0)
	s->value[SLA_SUN] = (s->value[LAI_SUN] + (s->value[LAI_SHADE]/s->value[SLA_RATIO]))/(leaf_c /(s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell));
	logger(g_log, "SLA SUN = %f m^2/KgC\n", s->value[SLA_SUN]);
	if(s->value[LAI_SHADE] > 0.0)
	s->value[SLA_SHADE] = s->value[SLA_SUN] * s->value[SLA_RATIO];
	logger(g_log, "SLA SHADE = %f m^2/KgC\n", s->value[SLA_SHADE]);
	logger(g_log, "*****************************\n");

	CHECK_CONDITION(fabs(s->value[LAI]), < 0.0);
	CHECK_CONDITION(fabs(s->value[LAI_SUN]), < 0.0);
	CHECK_CONDITION(fabs(s->value[LAI_SHADE]), < 0.0);
	CHECK_CONDITION(fabs(s->value[LAI]), > g_settings->maxlai);
//	CHECK_CONDITION(s->value[LAI], > s->value[PEAK_LAI])
}


