/*
 * canopy_temperature.c
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"
#include "canopy_radiation_lw_band.h"
#include "canopy_temperature.h"

extern logger_t* g_log;

void canopy_temperature (species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int year)
{
	double ra;    /* bulk canopy resistance to heat and vapour transport */
	double rc;    /* canopy surface resistance to vapour transport */
	double TairK;

	double dt = 0.2;
	double t1, t2;
	double pvs1, pvs2;
	double delta;
	double psych_p;
	double g_corr;
	double gl_bl;
	double gl_sh;
	double gc_sh;
	double gl_e_wv;
	double gc_e_wv;

	/* the function to compute canopy temperature use eq. in Webber et al., 2016
	 * the resistance parts follows the rationale of BIOME-BGC model 	 */



	TairK = met[month].d[day].tavg + TempAbs;

	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+TempAbs)/293.15, 1.75) * 101300/met[month].d[day].air_pressure;

	/* calculate temperature offsets for slope estimate */
	t1 = met[month].d[day].tday+dt;
	t2 = met[month].d[day].tday-dt;

	/* calculate saturation vapor pressures (Pa) at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

	/* calculate slope of pvs vs. T curve, at ta */
	//test this is the "DELTA" function as in Webber et al., 2016
	delta = (pvs1-pvs2) / (t1-t2);
	/* converts into kPA following Webber et al., 2016 */
	delta /= 1000.0;

	/* leaf boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh = gl_bl;

	/* Canopy conductance to sensible heat */
	gc_sh = gl_sh * s->value[LAI];

	/* Canopy resistance to sensible heat */
	ra = 1./gc_sh;

	/* Leaf conductance to evaporated water vapor, per unit projected LAI */
	gl_e_wv = gl_bl;

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI];

	/* Canopy resistance to vapour transport */
	rc = 1./gc_e_wv;

	/* compute product of psychrometric constant */
	psych_p = met[month].d[day].psych * (1. + (rc/ra));


	if(s->counter[VEG_DAYS] == 1 && !day && !month && !year)
	{
		/* assign canopy temperature the first day if simulation */
		//note: special case  leaf/canopy temperature = daily  average temperature
		//fixme it should be forced to air temperature just the first day of simulation!

		s->value[CANOPY_TEMP_K] = met[month].d[day].tavg + TempAbs;

		/* assign previous day canopy temperature to local variable */
		s->value[CANOPY_TEMP_K_OLD] = s->value[CANOPY_TEMP_K];
	}
	else
	{
		/* assign previous day canopy temperature to local variable */
		s->value[CANOPY_TEMP_K_OLD] = s->value[CANOPY_TEMP_K];

		/* compute canopy temperature (Webber et al., 2016) */
		s->value[CANOPY_TEMP_K] =
				TairK +
				((s->value[NET_RAD] * ra)/(met[month].d[day].rho_air * CP)) *
				(psych_p / (delta + psych_p)) -
				((met[month].d[day].es - met[month].d[day].ea)/(delta +psych_p));
	}

	logger(g_log, "difference Tavg Tcanopy temperature = %g (K)\n",TairK - s->value[CANOPY_TEMP_K]);
	if(day == 0)getchar();
}
