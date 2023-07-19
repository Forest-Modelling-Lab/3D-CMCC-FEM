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

void photosynthesis_LUE(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_annual_t *const meteo_annual)
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

	/******************************************************************/

	if ( s->value[ALPHA] != NO_DATA )
	{
		/* compute actual light use efficiency from potential one */
		//fixme use or not s->value[F_CO2_TR]???????
		Alpha_C   = s->value[ALPHA] * s->value[F_CO2] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] /* * s->value[F_CO2_TR] */;

		/* molC/molPAR/m2/day --> gC/MJ/m2/day */
		Epsilon_C = Alpha_C * MOLPAR_MJ * GC_MOL;
	}
	else
	{
		/* compute actual light use efficiency from potential one */
		//fixme use or not s->value[F_CO2_TR]???????
		Epsilon_C = s->value[EPSILONgCMJ] * s->value[F_CO2] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] /* * s->value[F_CO2_TR] */;

		s->value[ALPHA] = s->value[EPSILONgCMJ] / (MOLPAR_MJ * GC_MOL);

		/* gC/MJ/m2/day --> molC/molPAR/m2/day */
		Alpha_C   = Epsilon_C / (MOLPAR_MJ * GC_MOL);
	}

	/* note: special case when fSW <= WATER_STRESS_LIMIT for coupling with canopy transpiration */
	/* to be fixed once */
	if ( s->value[F_SW] <= WATER_STRESS_LIMIT )
	{
		if ( ! s->value[CANOPY_TRANSP] )
		{
			Alpha_C  = 0.;
		}
		else
		{
			Alpha_C *=  s->value[F_SW];
		}
	}

	/* check if current Alpha exceeds (saturates) maximum Alpha */
	/* (canopy saturation at 600 ppmv see Medlyn, 1996; Medlyn et al., 2011) */
	if (Alpha_C > s->value[ALPHA])
	{
		/* set Alpha C to s->value[ALPHA] */
		Alpha_C = s->value[ALPHA];
	}

	/* Light Use Efficiency Actual and Potential */
	/* new 07 June 2017 included APAR downregulation */
	Lue           = s->value[APAR]       * Alpha_C;
	Lue_max       = s->value[PAR]        * Alpha_C;
	Lue_sun       = s->value[APAR_SUN]   * Alpha_C * s->value[F_LIGHT_SUN_MAKELA];
	Lue_sun_max   = s->value[PAR_SUN]    * Alpha_C;
	Lue_shade     = s->value[APAR_SHADE] * Alpha_C * s->value[F_LIGHT_SHADE_MAKELA];
	Lue_shade_max = s->value[PAR_SHADE]  * Alpha_C;

	/* check */
	/* overall canopy */
	if ( Lue > Lue_max )
	{
		/* current Lue cannot exceeds Lue max */
		Lue = Lue_max;
	}
	/* canopy sun */
	if ( Lue_sun > Lue_sun_max )
	{
		/* Actual Lue cannot exceed Lue max */
		Lue_sun = Lue_sun_max;
	}
	/*canopy shade */
	if ( Lue_shade > Lue_shade_max )
	{
		/* Actual Lue cannot exceed Lue max */
		Lue_shade = Lue_shade_max;
	}

	/* GPP */
	/* Daily GPP in molC/m^2/day */
	GPPmolC        = Lue       /* FIXME it should account for transp * s->value[CANOPY_FRAC_DAY_TRANSP]*/;
	GPP_sun_molC   = Lue_sun   /* FIXME it should account for sun transp * s->value[CANOPY_SUN_FRAC_DAY_TRANSP]*/;
	GPP_shade_molC = Lue_shade /* FIXME it should account for shade transp * s->value[CANOPY_SHADE_FRAC_DAY_TRANSP]*/;

	/* check */
	CHECK_CONDITION( GPPmolC , < , ZERO );

	/* Daily assimilation (gC/m2/day) */
	/* molC/m2/day --> gC/m2/day */
	s->value[ASSIMILATION_SUN]   = GPP_sun_molC   * GC_MOL;
	s->value[ASSIMILATION_SHADE] = GPP_shade_molC * GC_MOL;

	/* total net assimilation */
	s->value[ASSIMILATION]       = s->value[ASSIMILATION_SUN] + s->value[ASSIMILATION_SHADE];

	s->value[GPP_SUN]            = s->value[ASSIMILATION_SUN];
	s->value[GPP_SHADE]          = s->value[ASSIMILATION_SHADE];

	/* total gpp */
	s->value[GPP]                = s->value[GPP_SUN] + s->value[GPP_SHADE];

	/* gC/m2/day --> tC/cell/day */
	s->value[GPP_tC]             = s->value[GPP] / 1e6 * g_settings->sizeCell ;

	/* class level */
	s->value[MONTHLY_GPP]       += s->value[GPP];
	s->value[MONTHLY_GPP_SUN]   += s->value[GPP_SUN];
	s->value[MONTHLY_GPP_SHADE] += s->value[GPP_SHADE];

	s->value[YEARLY_GPP]        += s->value[GPP];
	s->value[YEARLY_GPP_SUN]    += s->value[GPP_SUN];
	s->value[YEARLY_GPP_SHADE]  += s->value[GPP_SHADE];


	s->value[MONTHLY_ASSIMILATION]       += s->value[ASSIMILATION];
	s->value[MONTHLY_ASSIMILATION_SUN]   += s->value[ASSIMILATION_SUN];
	s->value[MONTHLY_ASSIMILATION_SHADE] += s->value[ASSIMILATION_SHADE];

	s->value[YEARLY_ASSIMILATION]        += s->value[ASSIMILATION];
	s->value[YEARLY_ASSIMILATION_SUN]    += s->value[ASSIMILATION_SUN];
	s->value[YEARLY_ASSIMILATION_SHADE]  += s->value[ASSIMILATION_SHADE];

	/* cell level */
	c->daily_gpp                += s->value[GPP];
	c->monthly_gpp              += s->value[GPP];
	c->annual_gpp               += s->value[GPP];

	c->daily_ass                += s->value[ASSIMILATION];
	c->monthly_ass              += s->value[ASSIMILATION];
	c->annual_ass               += s->value[ASSIMILATION];

	c->daily_gpp_tC             += s->value[GPP_tC];
	c->monthly_gpp_tC           += s->value[GPP_tC];
	c->annual_gpp_tC            += s->value[GPP_tC];

	/* yearly veg days counter */
	if ( s->value[GPP] > 0. )
	{
		++s->counter[YEARLY_VEG_DAYS];
	}


	/************************************************************************************************************************************/

	/* compute actual quantum canopy efficiency (molC/molphotons PAR) */
	if ( Lue       > 0. ) s->value[ALPHA_EFF]          = Lue       / s->value[PAR];
	else                  s->value[ALPHA_EFF]          = 0.;
	if ( Lue_sun   > 0. ) s->value[ALPHA_EFF_SUN]      = Lue_sun   / s->value[PAR_SUN];
	else                  s->value[ALPHA_EFF_SUN]      = 0.;
	if ( Lue_shade > 0. ) s->value[ALPHA_EFF_SHADE]    = Lue_shade / s->value[PAR_SHADE];
	else                  s->value[ALPHA_EFF_SHADE]    = 0.;

	/************************************************************************************************************************************/

}
