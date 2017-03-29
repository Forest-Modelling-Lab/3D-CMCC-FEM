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

extern logger_t* g_debug_log;
extern settings_t* g_settings;


void photosynthesis(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int DaysInMonth, const yos_t *const meteo_annual)
{
	double Alpha_C;
	double Epsilon_C;
	double GPPmolC;
	double Lue;
	double Lue_max;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**PHOTOSYNTHESIS**\n");

	logger(g_debug_log, "VegUnveg = %d\n", s->counter[VEG_UNVEG]);

	//note: photosynthesis in controlled by transpiration through the F_VPD modifier that also controls transpiration */

	if (s->value[ALPHA] != NO_DATA)
	{
		/* compute effective light use efficiency */
		Alpha_C = s->value[ALPHA] * s->value[F_CO2] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD];
		logger(g_debug_log, "Alpha C = %g molC/molPAR/m2/day\n", Alpha_C);

		/* molC/molPAR/m2/day --> gC/MJ/m2/day */
		Epsilon_C = Alpha_C * MOLPAR_MJ * GC_MOL;
	}
	else
	{
		/* compute effective light use efficiency */
		Epsilon_C = s->value[EPSILONgCMJ] * s->value[F_CO2] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD];
		logger(g_debug_log, "Epsilon C = %g gC/MJ/m2/day\n", Epsilon_C);

		/* gC/MJ/m2/day --> molC/molPAR/m2/day */
		Alpha_C = Epsilon_C / (MOLPAR_MJ * GC_MOL);
		logger(g_debug_log, "Alpha C = %g molC/molPAR/m2/day\n", Alpha_C);
	}

	/* check if current Alpha exceeds (saturates) maximum Alpha */
	/* (canopy saturation at 600 ppmv see Medlyn, 1996;  Medlyn et al., 2011) */
	if (Alpha_C > s->value[ALPHA])
	{
		logger(g_debug_log, "Alpha C = %g molC/molPAR/m2/day\n", Alpha_C);
		logger(g_debug_log, "ALPHA = %g molC/molPAR/m2/day\n", s->value[ALPHA]);
		logger(g_debug_log, "co2 conc = %g ppmv\n", meteo_annual->co2Conc);

		/* set Alpha C to s->value[ALPHA] */
		Alpha_C = s->value[ALPHA];
	}

	/* Light Use Efficiency Actual and Potential */
	Lue = s->value[APAR] * Alpha_C;
	Lue_max = s->value[PAR] * Alpha_C;

	/* check */
	if (Lue > Lue_max)
	{
		/* current Lue cannot exceed Lue max */
		Lue = Lue_max;
	}

	/* GPP */
	/* Daily GPP in molC/m^2/day */
	GPPmolC = Lue /* * s->value[CANOPY_FRAC_DAY_TRANSP]*/;

	/* check */
	CHECK_CONDITION( GPPmolC, <, 0 );

	/* Daily GPP in gC/m2/day */
	/* molC/m2/day --> gC/m2/day */
	s->value[GPP_gC]          = GPPmolC * GC_MOL;
	s->value[GPP_tC]          = s->value[GPP_gC] / 1e6 * g_settings->sizeCell ;
	logger(g_debug_log, "GPP_gC = %g gC/m^2/day\n", s->value[GPP_gC]);

	/* class level */
	s->value[MONTHLY_GPP_gC] += s->value[GPP_gC];
	s->value[YEARLY_GPP_gC]  += s->value[GPP_gC];

	/* cell level */
	c->daily_gpp             += s->value[GPP_gC];
	c->monthly_gpp           += s->value[GPP_gC];
	c->annual_gpp            += s->value[GPP_gC];

	/* yearly veg days counter */
	if ( s->value[GPP_gC] > 0. )
	{
		++s->counter[YEARLY_VEG_DAYS];
	}

}
