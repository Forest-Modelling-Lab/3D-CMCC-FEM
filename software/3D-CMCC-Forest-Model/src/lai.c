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
	logger(g_debug_log, "CANOPY_COVER_PROJ = %g %%\n", s->value[CANOPY_COVER_PROJ]);

	/* compute total projected LAI */
	s->value[LAI_PROJ] = (leaf_c * s->value[SLA_AVG])/(s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
	logger(g_debug_log, "LAI_PROJ = %f m2/m-2\n", s->value[LAI_PROJ]);

	/* compute projected LAI for sunlit and shaded canopy portions */
	s->value[LAI_SUN_PROJ] = 1.0 - exp(-s->value[LAI_PROJ]);
	s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];
	logger(g_debug_log, "LAI_SUN_PROJ = %g m2/m-2\n", s->value[LAI_SUN_PROJ]);
	logger(g_debug_log, "LAI_SHADE_PROJ = %g m2/m-2\n", s->value[LAI_SHADE_PROJ]);

	/**************************************************************************************************/

	/* compute LAI for Ground Area */
	/* Jackson & Palmer, 1979, 1981, 1983; Cannell and Grace 1993, Duursma and Makela 2006 */

	logger(g_debug_log, "single height class canopy cover = %g %%\n", s->value[CANOPY_COVER_PROJ]*100.0);

	/* compute total LAI for Ground Area */
	s->value[LAI_EXP] = s->value[LAI_PROJ] / s->value[CANOPY_COVER_PROJ];
	logger(g_debug_log, "LAI_EXP = %f m-2\n", s->value[LAI_EXP]);

	/* compute LAI for sunlit and shaded canopy portions for Ground Area */
	s->value[LAI_SUN_EXP] = s->value[LAI_SUN_PROJ] / s->value[CANOPY_COVER_PROJ];
	s->value[LAI_SHADE_EXP] = s->value[LAI_SHADE_PROJ] / s->value[CANOPY_COVER_PROJ];
	logger(g_debug_log, "LAI_SUN_EXP = %g m2 m-2\n", s->value[LAI_SUN_EXP]);
	logger(g_debug_log, "LAI_SHADE_EXP = %g m2 m-2\n", s->value[LAI_SHADE_EXP]);

	/**************************************************************************************************/

	/* compute all-sided LAI */
	s->value[ALL_LAI_PROJ] = s->value[LAI_PROJ] * s->value[CANOPY_COVER_PROJ];
	logger(g_debug_log, "ALL_LAI_PROJ = %g m2\n", s->value[ALL_LAI_PROJ]);
	logger(g_debug_log,"*****************************\n");

	CHECK_CONDITION(s->value[LAI_PROJ], < 0.0);
	CHECK_CONDITION(s->value[LAI_SUN_PROJ], < 0.0);
	CHECK_CONDITION(s->value[LAI_SHADE_PROJ], < 0.0);
	CHECK_CONDITION(s->value[ALL_LAI_PROJ], < 0.0);
	CHECK_CONDITION(fabs((s->value[LAI_SUN_PROJ] + s->value[LAI_SHADE_PROJ]) - s->value[LAI_PROJ]), > eps );
}


