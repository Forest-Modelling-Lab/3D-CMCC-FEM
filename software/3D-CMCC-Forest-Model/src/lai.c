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
extern logger_t* g_debug_log;

void daily_lai (species_t *const s)
{
	double leaf_c;                             //leaf carbon KgC/m^2

	/* NOTE: it mainly follows rationale of TREEDYN 3, Bossel, 1996, Ecological Modelling (eq. 30) */

	logger(g_debug_log, "\n**LEAF AREA INDEX**\n");

	/* convert tC/cell to KgC/m^2 */
	leaf_c = s->value[LEAF_C] * 1000.0 ;
	logger(g_debug_log, "Leaf Biomass = %g KgC/cell\n", leaf_c);
	logger(g_debug_log, "CANOPY_COVER = %g %%\n", s->value[CANOPY_COVER]);

	s->value[LAI] = (leaf_c * s->value[SLA_AVG])/(s->value[CANOPY_COVER] * g_settings->sizeCell);
	logger(g_debug_log, "LAI = %f\n", s->value[LAI]);

	/* Calculate projected LAI for sunlit and shaded canopy portions */
	s->value[LAI_SUN] = 1.0 - exp(-s->value[LAI]);
	s->value[LAI_SHADE] = s->value[LAI] - s->value[LAI_SUN];
	logger(g_debug_log, "LAI SUN = %g m2/m-2\n", s->value[LAI_SUN]);
	logger(g_debug_log, "LAI SHADE = %g m2/m-2\n", s->value[LAI_SHADE]);

	/* Calculate all-sided LAI */
	s->value[ALL_LAI] = s->value[LAI] * (s->value[CANOPY_COVER] * g_settings->sizeCell);
	logger(g_debug_log, "ALL LAI = %g m2\n", s->value[ALL_LAI]);
	logger(g_debug_log,"*****************************\n");

	CHECK_CONDITION(s->value[LAI], < 0.0);
	CHECK_CONDITION(s->value[LAI_SUN], < 0.0);
	CHECK_CONDITION(s->value[LAI_SHADE], < 0.0);
	CHECK_CONDITION(s->value[ALL_LAI], < 0.0);
	CHECK_CONDITION(fabs((s->value[LAI_SUN] + s->value[LAI_SHADE]) - s->value[LAI]), > eps );
}


