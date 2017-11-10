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

void specific_leaf_area ( const age_t *const a, species_t *const s )
{

	double n = 2;

	logger(g_debug_log, "\n**SPECIFIC LEAF AREA**\n");

	/* ager related SLA based on 3-PG model approach */

	s->value[SLA_AVG] = s->value[SLA_AVG1] + ( s->value[SLA_AVG0] - s->value[SLA_AVG1] ) * exp( -LN2 * pow(((double)a->value / s->value[TSLA]), n));
	logger(g_debug_log, "SLA_AVG = %f (kgC/m2)\n", s->value[SLA_AVG]);
}

void daily_lai (cell_t* const c, species_t *const s)
{
	double leaf_DM;                             /* leaf carbon KgDM/sizecell */
	double Leaf_sun_ratio;

	/* NOTE: it mainly follows rationale of TREEDYN 3, Bossel, 1996, Ecological Modelling (eq. 30) */
	/* no consideration of leaf angle is taken into account at daily scale as described by Thornton (1998)
	 * it is approximates over the full day */

	logger(g_debug_log, "\n**LEAF AREA INDEX**\n");

	/* convert tC/cell to KgDM/m^2 */
	leaf_DM = s->value[LEAF_C] * 1e3 * GC_GDM;
	logger(g_debug_log, "Leaf Biomass      = %f Kg/cell\n",  leaf_DM);
	logger(g_debug_log, "SLA_AVG           = %f kg/m2\n",    s->value[SLA_AVG]);
	logger(g_debug_log, "CANOPY_COVER_PROJ = %f \n",         s->value[CANOPY_COVER_PROJ]);

	/**************************************************************************************************/

	/* compute LAI for Projected Area */
	/* see Campbell and Norman, Environmental Biophysics 2nd Edition pg 259 */

	s->value[LAI_PROJ]       = ( leaf_DM * s->value[SLA_AVG] ) / ( s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell );
	s->value[LAI_SUN_PROJ]   = 1. - exp ( - s->value[LAI_PROJ] );
	s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];

	logger(g_debug_log, "LAI_PROJ       = %f m2/m2\n", s->value[LAI_PROJ]);
	logger(g_debug_log, "LAI_SUN_PROJ   = %f m2/m2\n", s->value[LAI_SUN_PROJ]);
	logger(g_debug_log, "LAI_SHADE_PROJ = %f m2/m2\n", s->value[LAI_SHADE_PROJ]);
	logger(g_debug_log, "PEAK_LAI_PROJ  = %f m2/m2\n", s->value[PEAK_LAI_PROJ]);

	/* assign max annual LAI */
	s->value[MAX_LAI_PROJ]   = MAX(s->value[MAX_LAI_PROJ], s->value[LAI_PROJ]);
	logger(g_debug_log, "MAX_LAI_PROJ   = %f m2/m2\n", s->value[MAX_LAI_PROJ]);

	/* compute ratio between leaf sun and leaf shade */
	Leaf_sun_ratio   = s->value[LAI_SUN_PROJ] / s->value[LAI_PROJ];

	/* calculate Leaf sun and shade carbon amount */
	s->value[LEAF_SUN_C]   = s->value[LEAF_C] * Leaf_sun_ratio;
	s->value[LEAF_SHADE_C] = s->value[LEAF_C] - s->value[LEAF_SUN_C];

	/**************************************************************************************************/

	/* compute LAI for Exposed Area */

	/* note: is partially based on: Jackson & Palmer, 1979, 1981, 1983; Cannell and Grace 1993, Duursma and Makela 2007 */
	s->value[LAI_EXP]        = ( leaf_DM * s->value[SLA_AVG] ) / ( s->value[CANOPY_COVER_EXP] * g_settings->sizeCell );
	s->value[LAI_SUN_EXP]    = 1. - exp ( - s->value[LAI_EXP] );
	s->value[LAI_SHADE_EXP]  = s->value[LAI_EXP] - s->value[LAI_SUN_EXP];

	logger(g_debug_log, "LAI_EXP       = %f m2/m2\n", s->value[LAI_EXP]);
	logger(g_debug_log, "LAI_SUN_EXP   = %f m2/m2\n", s->value[LAI_SUN_EXP]);
	logger(g_debug_log, "LAI_SHADE_EXP = %f m2/m2\n", s->value[LAI_SHADE_EXP]);
	logger(g_debug_log, "PEAK_LAI_EXP  = %f m2/m2\n", s->value[PEAK_LAI_EXP]);

	/* assign max annual LAI EXP */
	s->value[MAX_LAI_EXP]   = MAX(s->value[MAX_LAI_EXP], s->value[LAI_EXP]);
	logger(g_debug_log, "MAX_LAI_EXP   = %f m2/m2\n", s->value[MAX_LAI_EXP]);

	/**************************************************************************************************/

	logger(g_debug_log, "single height class canopy cover projected = %f %%\n", s->value[CANOPY_COVER_PROJ] * 100.);
	logger(g_debug_log, "single height class canopy cover exposed   = %f %%\n", s->value[CANOPY_COVER_EXP]  * 100.);

	/* compute all-sided Leaf Area */
	s->value[ALL_LAI_PROJ] = s->value[LAI_PROJ] * s->value[CANOPY_COVER_PROJ];
	logger(g_debug_log, "ALL_LAI_PROJ = %f (m2)\n", s->value[ALL_LAI_PROJ]);


	/* controls it should anyway never happens */
	if ( s->value[LAI_PROJ] > s->value[PEAK_LAI_PROJ] )
	{
		s->value[LAI_PROJ]       =  s->value[PEAK_LAI_PROJ];
		s->value[LAI_SUN_PROJ]   = 1. - exp ( - s->value[LAI_PROJ] );
		s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];
	}


	logger(g_debug_log,"*****************************\n");

	/* checks */
	CHECK_CONDITION(s->value[LAI_PROJ],       <, ZERO);
	CHECK_CONDITION(s->value[LAI_SUN_PROJ],   <, ZERO);
	CHECK_CONDITION(s->value[LAI_SHADE_PROJ], <, ZERO);
	CHECK_CONDITION(s->value[ALL_LAI_PROJ],   <, ZERO);
	CHECK_CONDITION(fabs((s->value[LAI_SUN_PROJ] + s->value[LAI_SHADE_PROJ]) - s->value[LAI_PROJ]), >, eps );
	CHECK_CONDITION(s->value[LAI_PROJ], >, (s->value[PEAK_LAI_PROJ] + eps));


	/* test: computing cell level max_lai */
	//fixme in case of multi-class!!!!!

	c->max_lai_proj = s->value[MAX_LAI_PROJ];

}


