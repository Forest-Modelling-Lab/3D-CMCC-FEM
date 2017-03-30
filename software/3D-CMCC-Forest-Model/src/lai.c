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
	double leaf_C;                             /* leaf carbon KgC/sizecell */

	/* NOTE: it mainly follows rationale of TREEDYN 3, Bossel, 1996, Ecological Modelling (eq. 30) */
	/* no consideration of leaf angle is taken into account at daily scale as described by Thornton (1998)
	 * it is approximates over the full day */

	logger(g_debug_log, "\n**LEAF AREA INDEX**\n");

	/**************************************************************************************************/

	/* compute LAI for Projected Area */

	/* convert tC/cell to KgC/m^2 */
	leaf_C = s->value[LEAF_C] * 1000.0 ;
	logger(g_debug_log, "Leaf Biomass = %g KgC/sizecell\n", leaf_C);
	logger(g_debug_log, "CANOPY_COVER_PROJ = %g %%\n", s->value[CANOPY_COVER_PROJ]);

	/* compute total LAI for Projected Area */
	s->value[LAI_PROJ] = (leaf_C * s->value[SLA_AVG])/(s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
	s->value[LAI_SUN_PROJ]   = 1. - exp(-s->value[LAI_PROJ]);
	s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];
	logger(g_debug_log, "LAI_PROJ = %f m2/m2\n", s->value[LAI_PROJ]);
	logger(g_debug_log, "LAI_SUN_PROJ = %g m2/m2\n", s->value[LAI_SUN_PROJ]);
	logger(g_debug_log, "LAI_SHADE_PROJ = %g m2/m2\n", s->value[LAI_SHADE_PROJ]);
	logger(g_debug_log, "PEAK_LAI_PROJ = %f m2/m2\n", s->value[PEAK_LAI_PROJ]);

	/**************************************************************************************************/

	/* compute LAI for Exposed Area */

	/* note: is partially based on: Jackson & Palmer, 1979, 1981, 1983; Cannell and Grace 1993, Duursma and Makela 2006 */
	//s->value[LAI_EXP] = s->value[LAI_PROJ] / s->value[CANOPY_COVER_PROJ];
	//s->value[LAI_SUN_EXP] = s->value[LAI_SUN_PROJ] / s->value[CANOPY_COVER_PROJ];
	//s->value[LAI_SHADE_EXP] = s->value[LAI_SHADE_PROJ] / s->value[CANOPY_COVER_PROJ];

	/* differently from above we don't divide for ground area but we multiply for fraction of exposed area */

	logger(g_debug_log, "single height class canopy cover projected = %g %%\n", s->value[CANOPY_COVER_PROJ]*100.0);
	logger(g_debug_log, "single height class canopy cover exposed = %g %%\n", s->value[CANOPY_COVER_EXP]*100.0);

	/* compute total LAI for Exposed Area */
	s->value[LAI_EXP]       = s->value[LAI_PROJ] * (1 + s->value[CANOPY_COVER_EXP]);
	logger(g_debug_log, "LAI_EXP = %f m-2\n", s->value[LAI_EXP]);

	/* compute LAI for sunlit and shaded canopy portions for Exposed Area */
	s->value[LAI_SUN_EXP]   = s->value[LAI_SUN_PROJ] * (1 + s->value[CANOPY_COVER_EXP]);
	s->value[LAI_SHADE_EXP] = s->value[LAI_SHADE_PROJ] * (1 + s->value[CANOPY_COVER_EXP]);
	logger(g_debug_log, "LAI_SUN_EXP = %g m2 m-2\n", s->value[LAI_SUN_EXP]);
	logger(g_debug_log, "LAI_SHADE_EXP = %g m2 m-2\n", s->value[LAI_SHADE_EXP]);

	/**************************************************************************************************/

	/* compute all-sided LAI */
	s->value[ALL_LAI_PROJ] = s->value[LAI_PROJ] * s->value[CANOPY_COVER_PROJ];
	logger(g_debug_log, "ALL_LAI_PROJ = %g (fraction cover)\n", s->value[ALL_LAI_PROJ]);
	logger(g_debug_log,"*****************************\n");

	/* checks */
	CHECK_CONDITION(s->value[LAI_PROJ],       <, 0.0);
	CHECK_CONDITION(s->value[LAI_SUN_PROJ],   <, 0.0);
	CHECK_CONDITION(s->value[LAI_SHADE_PROJ], <, 0.0);
	CHECK_CONDITION(s->value[ALL_LAI_PROJ],   <, 0.0);
	CHECK_CONDITION(fabs((s->value[LAI_SUN_PROJ] + s->value[LAI_SHADE_PROJ]) - s->value[LAI_PROJ]), >, eps );
	CHECK_CONDITION(s->value[LAI_PROJ], >, (s->value[PEAK_LAI_PROJ] + eps));
}


