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

extern logger_t* g_log;

void phosynthesis(cell_t *const c, const int layer, const int height, const int age, const int species, const int DaysInMonth)
{
	double Alpha_C;
	double Epsilon_C;
	double GPPmolC;
	double GPPmolC_sun;
	double GPPmolC_shaded;
	double GPPmolC_tot;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	logger(g_log, "\n**PHOTOSYNTHESIS**\n");

	logger(g_log, "VegUnveg = %d\n", s->counter[VEG_UNVEG]);

	//fixme photosynthesis should occurs only in the fraction of the days in which also daily transp occurs

	//test without F_CO2
	if (s->value[ALPHA] != NO_DATA)
	{
		/* compute effective light use efficiency */
		Alpha_C = (s->value[ALPHA] * /* s->value[F_CO2] * */ s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD])
							 /**s->value[FRAC_DAYTIME_TRANSP] */;
		logger(g_log, "Alpha C = %g molC/molPAR/m2/day\n", Alpha_C);

		/* molC/molPAR/m2/day --> gC/MJ/m2/day */
		Epsilon_C = Alpha_C * MOLPAR_MJ * GC_MOL;
	}
	else
	{
		/* compute effective light use efficiency */
		Epsilon_C = (s->value[EPSILONgCMJ] * /* s->value[F_CO2] * */s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD])
							/**s->value[FRAC_DAYTIME_TRANSP] */;
		logger(g_log, "Epsilon C = %g gC/MJ/m2/day\n", Epsilon_C);

		/* gC/MJ/m2/day --> molC/molPAR/m2/day */
		Alpha_C = Epsilon_C / (MOLPAR_MJ * GC_MOL);
		logger(g_log, "Alpha C = %g molC/molPAR/m2/day\n", Alpha_C);
	}

	//test 12 May 2016 test
	//GPP depends on canopy wet (no photosynthesis occurs if canopy is wet)
	//fraction depend on partial canopy wet
	//Alpha_C *= s->value[CANOPY_FRAC_DAY_TRANSP];

	/* GPP */
	/* Daily GPP in molC/m^2/day */
	GPPmolC = s->value[APAR] * Alpha_C;
	GPPmolC_sun = s->value[APAR_SUN]* Alpha_C;
	GPPmolC_shaded = s->value[APAR_SHADE]* Alpha_C;
	GPPmolC_tot = GPPmolC_sun + GPPmolC_shaded;
	logger(g_log, "Apar for GPP = %g molPAR/m2/day\n", s->value[APAR]);
	logger(g_log, "GPPmolC = %g molC/m^2/day\n", GPPmolC);
	logger(g_log, "GPPmolC_sun = %g molC/m^2/day\n", GPPmolC_sun);
	logger(g_log, "GPPmolC_shade = %g molC/m^2/day\n", GPPmolC_shaded);
	logger(g_log, "GPPmolC_tot = %g molC/m^2/day\n", GPPmolC_tot);

	/* check */
	CHECK_CONDITION( GPPmolC, < 0 );
	CHECK_CONDITION( fabs ( GPPmolC - GPPmolC_tot ), > 1e-4 );

	/* Daily GPP in gC/m2/day */
	/* molC/m2/day --> gC/m2/day */
	s->value[DAILY_GPP_gC] = GPPmolC_tot * GC_MOL;
	logger(g_log, "DAILY_GPP_gC = %g gC/m2/day\n", s->value[DAILY_GPP_gC]);

	/* class level */
	s->value[MONTHLY_GPP_gC] += s->value[DAILY_GPP_gC];
	s->value[YEARLY_GPP_gC] += s->value[DAILY_GPP_gC];

	/* cell level */
	c->daily_gpp += s->value[DAILY_GPP_gC];
	c->monthly_gpp += s->value[DAILY_GPP_gC];
	c->annual_gpp += s->value[DAILY_GPP_gC];

}
