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

extern logger_t* g_debug_log;

void canopy_temperature(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
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

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* the function to compute canopy temperature use eq. in Webber et al., 2016
	 * the resistance parts follows the rationale of BIOME-BGC model 	 */

	TairK = meteo_daily->tavg + TempAbs;

	/* temperature and pressure correction factor for conductances */
	g_corr = pow((meteo_daily->tavg+TempAbs)/293.15, 1.75) * 101300/meteo_daily->air_pressure;

	/* calculate temperature offsets for slope estimate */
	t1 = meteo_daily->tavg+dt;
	t2 = meteo_daily->tavg-dt;

	/* calculate saturation vapor pressures (Pa) at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

	/* calculate slope of pvs vs. T curve, at ta */
	/* Slope of the saturated vapour pressure curve*/
	/* this is NOT the "DELTA" function as in Webber et al., 2016
	 * it use:
	 * -Tc ad Ta instead t1 and t2
	 * -ec and ea instead pvs1 and pvs2
	 * ec refers to saturated vapour pressure of air at the top of the canopy
	 * */

	delta = (pvs1-pvs2) / (t1-t2);
	/* converts into kPA following Webber et al., 2016 */
	delta /= 1e3;

	/* leaf boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh = gl_bl;

	/* Canopy conductance to sensible heat */
	gc_sh = gl_sh * s->value[LAI_PROJ];

	/* Canopy resistance to sensible heat */
	ra = 1. / gc_sh;

	/* Leaf conductance to evaporated water vapor, per unit projected LAI */
	gl_e_wv = gl_bl;

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI_PROJ];

	/* Canopy resistance to vapour transport */
	rc = 1. / gc_e_wv;

	/* compute product of psychrometric constant */
	psych_p = meteo_daily->psych * (1. + ( rc / ra ));

	if(s->counter[VEG_DAYS] == 1)
	{

		//fixme it should be only for the first day of the first year
		s->value[CANOPY_TEMP_K] = TairK;

		/* assign previous day canopy temperature to local variable for long wave computation */
		logger(g_debug_log, "difference Tavg Tcanopy temperature = %g (K)\n",TairK - s->value[CANOPY_TEMP_K]);
	}
	else if (s->counter[VEG_DAYS] > 1)
	{

		/* compute canopy temperature (Webber et al., 2016) */
		s->value[CANOPY_TEMP_K] =
				TairK +
				((s->value[NET_RAD] * ra)/(meteo_daily->rho_air * CP)) *
				(psych_p / (delta + psych_p)) -
				((meteo_daily->es - meteo_daily->ea)/(delta +psych_p));

		/* assign previous day canopy temperature to local variable for long wave computation */
		s->value[CANOPY_TEMP_K_OLD] = s->value[CANOPY_TEMP_K];
		logger(g_debug_log, "difference Tavg Tcanopy temperature = %g (K)\n",TairK - s->value[CANOPY_TEMP_K]);
	}
	else
	{
		s->value[CANOPY_TEMP_K] = 0.;
	}

	//FIXME it hasn't sense at cell level just to print
	c->canopy_temp = s->value[CANOPY_TEMP_K] - TempAbs;
	logger(g_debug_log, "canopy_temp = %g (K)\n",c->canopy_temp);
	c->canopy_temp_k = s->value[CANOPY_TEMP_K];
	c->canopy_temp_diff = c->canopy_temp_k - TairK;

}
