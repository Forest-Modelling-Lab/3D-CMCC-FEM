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


void photosynthesis(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int DaysInMonth, const meteo_annual_t *const meteo_annual)
{
	double Alpha_C;
	double Epsilon_C;
	double GPPmolC;
	double GPP_sun_molC;
	double GPP_shade_molC;
	double Lue;
	double Lue_max;
	double Lue_sun;
	double Lue_sun_max;
	double Lue_shade;
	double Lue_shade_max;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**PHOTOSYNTHESIS**\n");

	logger(g_debug_log, "VegUnveg = %d\n", s->counter[VEG_UNVEG]);

	//note: photosynthesis in controlled by transpiration through the F_VPD modifier that also controls transpiration */

	if (s->value[ALPHA] != NO_DATA)
	{
		/* compute effective light use efficiency */
		Alpha_C = s->value[ALPHA] * s->value[F_CO2] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD];

		/* molC/molPAR/m2/day --> gC/MJ/m2/day */
		Epsilon_C = Alpha_C * MOLPAR_MJ * GC_MOL;
	}
	else
	{
		/* compute effective light use efficiency */
		Epsilon_C = s->value[EPSILONgCMJ] * s->value[F_CO2] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD];

		/* gC/MJ/m2/day --> molC/molPAR/m2/day */
		Alpha_C = Epsilon_C / (MOLPAR_MJ * GC_MOL);
	}

	/* check if current Alpha exceeds (saturates) maximum Alpha */
	/* (canopy saturation at 600 ppmv see Medlyn, 1996;  Medlyn et al., 2011) */
	if (Alpha_C > s->value[ALPHA])
	{
		/* set Alpha C to s->value[ALPHA] */
		Alpha_C = s->value[ALPHA];
	}

	/* Light Use Efficiency Actual and Potential */
	Lue           = s->value[APAR]       * Alpha_C;
	Lue_max       = s->value[PAR]        * Alpha_C;
	Lue_sun       = s->value[APAR_SUN]   * Alpha_C;
	Lue_sun_max   = s->value[PAR_SUN]    * Alpha_C;
	Lue_shade     = s->value[APAR_SHADE] * Alpha_C;
	Lue_shade_max = s->value[PAR_SHADE]  * Alpha_C;

	/* check */
	/* overall canopy */
	if (Lue > Lue_max)
	{
		/* current Lue cannot exceeds Lue max */
		Lue = Lue_max;
	}
	/* canopy sun */
	if (Lue_sun > Lue_sun_max)
	{
		/* current Lue cannot exceeds Lue max */
		Lue_sun = Lue_sun_max;
	}
	/*canopy shade */
	if (Lue_shade > Lue_shade_max)
	{
		/* current Lue cannot exceeds Lue max */
		Lue_shade = Lue_shade_max;
	}

	/* compute assimilation modifier (test) */
	s->value[F_A_SUN]   = Lue_sun   / Lue_sun_max;
	s->value[F_A_SHADE] = Lue_shade / Lue_shade_max;
	logger(g_debug_log, "Assimilation modifier (sun)   = %g (DIM)\n", s->value[F_A_SUN]);
	logger(g_debug_log, "Assimilation modifier (shade) = %g (DIM)\n", s->value[F_A_SHADE]);

	/* GPP */
	/* Daily GPP in molC/m^2/day */
	GPPmolC        = Lue       /* FIXME it should accounts for transp * s->value[CANOPY_FRAC_DAY_TRANSP]*/;
	GPP_sun_molC   = Lue_sun   /* FIXME it should accounts for sun transp * s->value[CANOPY_SUN_FRAC_DAY_TRANSP]*/;
	GPP_shade_molC = Lue_shade /* FIXME it should accounts for shade transp* s->value[CANOPY_SHADE_FRAC_DAY_TRANSP]*/;

	/* check */
	CHECK_CONDITION( GPPmolC, <, ZERO);

	/* Daily GPP in gC/m2/day */
	/* molC/m2/day --> gC/m2/day */

	s->value[GPP_SUN]   = GPP_sun_molC   * GC_MOL;
	s->value[GPP_SHADE] = GPP_shade_molC * GC_MOL;
	s->value[GPP]       = s->value[GPP_SUN] + s->value[GPP_SHADE];

	logger(g_debug_log, "GPP_gC = %g gC/m^2/day\n", s->value[GPP]);

	/* gC/m2/day --> tC/cell/day */
	s->value[GPP_tC]          = s->value[GPP] / 1e6 * g_settings->sizeCell ;

	/* class level */
	s->value[MONTHLY_GPP]       += s->value[GPP];
	s->value[MONTHLY_GPP_SUN]   += s->value[GPP_SUN];
	s->value[MONTHLY_GPP_SHADE] += s->value[GPP_SHADE];
	s->value[YEARLY_GPP]        += s->value[GPP];
	s->value[YEARLY_GPP_SUN]    += s->value[GPP_SUN];
	s->value[YEARLY_GPP_SHADE]  += s->value[GPP_SHADE];

	/* cell level */
	c->daily_gpp             += s->value[GPP];
	c->monthly_gpp           += s->value[GPP];
	c->annual_gpp            += s->value[GPP];

	c->daily_gpp_tC          += s->value[GPP_tC];
	c->monthly_gpp_tC        += s->value[GPP_tC];
	c->annual_gpp_tC         += s->value[GPP_tC];

	/* yearly veg days counter */
	if ( s->value[GPP] > 0. )
	{
		++s->counter[YEARLY_VEG_DAYS];
	}

}
