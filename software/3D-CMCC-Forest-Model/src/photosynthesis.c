/* photosynthesis.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "photosynthesis.h"
#include "canopy_evapotranspiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void phosynthesis(cell_t *const c, const int layer, const int height, const int age, const int species, const int DaysInMonth)
{
	double Alpha_C;
	double Epsilon;
	double GPPmolC, GPPmolC_sun, GPPmolC_shaded, GPPmolC_tot;
	double leaf_cell_cover_eff;                                                                //fraction of square meter covered by leaf over the gridcell


	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	logger(g_log, "\n**PHOTOSYNTHESIS**\n");

	logger(g_log, "VegUnveg = %d\n", s->counter[VEG_UNVEG]);


	leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;


	//fixme photosynthesis should occurs only in the fraction of the days in which also daily transp occurs

	//test without F_CO2
	if (s->value[ALPHA] != NO_DATA)
	{
		Alpha_C = (s->value[ALPHA] * /* s->value[F_CO2] * */ s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] * s->value[F_FROST])
							 /**s->value[FRAC_DAYTIME_TRANSP] */;
		logger(g_log, "Alpha C (Effective Quantum Canopy Efficiency)= %g molC/molPAR\n", Alpha_C);

		/* convert epsilon from gCMJ^-1 to molCmolPAR^-1 */
		Epsilon = Alpha_C * MOLPAR_MJ * GC_MOL;
	}
	else
	{
		Epsilon = (s->value[EPSILONgCMJ] * /* s->value[F_CO2] * */s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD]* s->value[F_FROST])
							/**s->value[FRAC_DAYTIME_TRANSP] */;
		logger(g_log, "Epsilon (LUE) = %g gDM/MJ\n", Epsilon);

		Alpha_C = Epsilon / (MOLPAR_MJ * GC_MOL);
		logger(g_log, "Alpha C = %g molC/molPAR m2/day\n", Alpha_C);
	}

	//test 12 May 2016 test
	//GPP depends on canopy wet (no photosynthesis occurs if canopy is wet)
	//fraction depend on partial canopy wet
	//Alpha_C *= s->value[CANOPY_FRAC_DAY_TRANSP];

	/* GPP */
	logger(g_log, "Apar for GPP = %g molPAR/m2/day\n", s->value[APAR]);

	/* DailyGPP in mol of C/m^2 */
	GPPmolC = s->value[APAR] * Alpha_C;
	GPPmolC_sun = s->value[APAR_SUN]* Alpha_C;
	GPPmolC_shaded = s->value[APAR_SHADE]* Alpha_C;
	GPPmolC_tot = GPPmolC_sun + GPPmolC_shaded;
	logger(g_log, "GPPmolC = %g molC/m^2 day\n", GPPmolC);
	logger(g_log, "GPPmolC_sun = %g molC/m^2 day\n", GPPmolC_sun);
	logger(g_log, "GPPmolC_shade = %g molC/m^2 day\n", GPPmolC_shaded);
	logger(g_log, "GPPmolC_tot = %g molC/m^2 day\n", GPPmolC_tot);

	CHECK_CONDITION(fabs(GPPmolC - GPPmolC_tot), > 1e-4);

	/* Daily GPP in grams of C/m^2 */
	/* Convert molC into grams */
	s->value[DAILY_POINT_GPP_gC] = GPPmolC_tot * GC_MOL;
	logger(g_log, "DAILY_POINT_GPP_gC = %g gC/m2/day \n", s->value[DAILY_POINT_GPP_gC] );

	/* it converts value of GPP gC/m2/day in gC/m2 area covered/day */
	//test 5 July 2016 without leaf_cell_cover_eff
	s->value[DAILY_GPP_gC] = s->value[DAILY_POINT_GPP_gC] /* * leaf_cell_cover_eff*/;
	logger(g_log, "DAILY_GPP_gC = %g gC/m2 area covered/day\n", s->value[DAILY_GPP_gC]);

	/* class level */
	s->value[MONTHLY_GPP_gC] += s->value[DAILY_GPP_gC];
	s->value[YEARLY_POINT_GPP_gC] += s->value[DAILY_GPP_gC];

	/* cell level */
	c->daily_gpp += s->value[DAILY_GPP_gC];
	c->monthly_gpp += s->value[DAILY_GPP_gC];
	c->annual_gpp += s->value[DAILY_GPP_gC];

}
