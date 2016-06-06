/*
 * canopy_evapo_biome.c
 *
 *  Created on: 23/mar/2016
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void canopy_evapotranspiration_biome (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height, int age, int species)
{

	double max_int;

	double g_corr;
	double gl_bl;
	double gl_s_sun, gl_s_shade;
	double gl_c;
	double m_ppfd_sun, m_ppfd_shade;
	double m_final_sun, m_final_shade;
	double gl_e_wv;
	double gl_t_wv_sun, gl_t_wv_shade;
	double gl_sh;
	double gc_e_wv;
	double gc_sh;
	double net_rad;
	double rv;
	double rh;
	double daylength_sec; //daylength in sec
	double evap_daylength;
	double transp_daylength;
	double evapo;
	//double evapo, evapo_sun, evapo_shade;
	double transp, transp_sun, transp_shade;

	double cell_coverage;
	static int days_with_canopy_wet;

	double tairK;
	double tsoilK;

	//test
	double psych, psych_p;
	double t1, t2;
	double pvs1, pvs2;
	double dt = 0.2;
	double delta;
	double rc, rr, rhr;
	double tcanopy, tcanopyK;

	tairK = met[month].d[day].tavg + TempAbs;
	tsoilK = met[month].d[day].tsoil + TempAbs;


	logger(g_log, "\n**CANOPY EVAPO-TRANSPIRATION BIOME**\n");

	logger(g_log, "**CANOPY INTERCEPTION BIOME**\n");

	if(s->value[CANOPY_COVER_DBHDC] > 1.0)
	{
		cell_coverage = 1.0;
	}
	else
	{
		cell_coverage = s->value[CANOPY_COVER_DBHDC];
	}

	daylength_sec = met[month].d[day].daylength * 3600.0;

	max_int = s->value[INT_COEFF] * s->value[ALL_LAI];

	logger(g_log, "ALL_LAI = %f mm/m^2/day\n", s->value[ALL_LAI]);

	/* no rain interception if canopy is wet from the day(s) before */
	if (c->prcp_rain>0.0 && s->value[ALL_LAI]>0.0 && s->value[CANOPY_WATER] == 0.0)
	{
		/* all rain intercepted */
		if (c->prcp_rain <= max_int)
		{
			s->value[CANOPY_INT] = c->prcp_rain;
		}
		/* canopy limits interception */
		else
		{
			s->value[CANOPY_INT] = max_int;
			c->prcp_rain -= s->value[CANOPY_INT];

		}

		s->value[CANOPY_WATER] = s->value[CANOPY_INT];
		s->value[CANOPY_INT] = 0.0;
	}
	/* in case of snow no interception */
	else
	{
		s->value[CANOPY_INT] = 0.0;
	}

	logger(g_log, "Rain intercepted = %f mm/m^2/day\n", s->value[CANOPY_INT]);
	logger(g_log, "CANOPY_WATER = %f mm/m^2/day\n", s->value[CANOPY_WATER]);


	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+TempAbs)/293.15, 1.75) * 101300/met[month].d[day].air_pressure;

	/* calculate leaf- and canopy-level conductances to water vapor and
		sensible heat fluxes */

	/* leaf boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;

	/* canopy boundary layer conductance */
	//add 6 May 2016
	s->value[CANOPY_BLCOND] = gl_bl * s->value[LAI];
	/* upscaled to day time */
	s->value[CANOPY_BLCOND] *= daylength_sec * cell_coverage;

	/* leaf cuticular conductance */
	gl_c = s->value[CUTCOND] * g_corr;

	/* leaf stomatal conductance: first generate multipliers, then apply them
		to maximum stomatal conductance */

	/* photosynthetic photon flux density conductance control */
	//	m_ppfd_sun = (s->value[APAR_SUN] * daylength_sec)/(PPFD50 + (s->value[APAR_SUN]* daylength_sec));
	//	//logger(g_log, "m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
	//	m_ppfd_shade = (s->value[APAR_SHADE] * daylength_sec)/(PPFD50 + (s->value[APAR_SHADE]* daylength_sec));
	//	//logger(g_log, "m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);

	//04/apr/2016
	/* photosynthetic photon flux density conductance control */
	m_ppfd_sun = s->value[PPFD_SUN] /(PPFD50 + s->value[PPFD_SUN]);
	logger(g_log, "m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
	m_ppfd_shade = s->value[PPFD_SHADE] /(PPFD50 + s->value[PPFD_SHADE]);
	logger(g_log, "m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);

	/* apply all multipliers to the maximum stomatal conductance */
	/* differently from BIOME we use F_T that takes into account not only minimum temperature effects */
	m_final_sun = m_ppfd_sun * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];
	m_final_shade = m_ppfd_shade * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];

	if (m_final_sun < 0.00000001) m_final_sun = 0.00000001;
	if (m_final_shade < 0.00000001) m_final_shade = 0.00000001;

	gl_s_sun = s->value[MAXCOND] * m_final_sun * g_corr;
	gl_s_shade = s->value[MAXCOND] * m_final_shade * g_corr;

	/* calculate leaf-and canopy-level conductances to water vapor and
		sensible heat fluxes, to be used in Penman-Monteith calculations of
		canopy evaporation and canopy transpiration. */

	/* Leaf conductance to evaporated water vapor, per unit projected LAI */
	gl_e_wv = gl_bl;

	/* Leaf conductance to transpired water vapor, per unit projected
		LAI.  This formula is derived from stomatal and cuticular conductances
		in parallel with each other, and both in series with leaf boundary
		layer conductance. */
	//gl_t_wv = (gl_bl * (gl_s + gl_c)) / (gl_bl + gl_s + gl_c);
	gl_t_wv_sun = (gl_bl * (gl_s_sun + gl_c)) / (gl_bl + gl_s_sun + gl_c);
	gl_t_wv_shade = (gl_bl * (gl_s_shade + gl_c)) / (gl_bl + gl_s_shade + gl_c);

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh = gl_bl;
	logger(g_log, "Leaf conductance to sensible heat = %f\n", gl_sh);

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI];

	/* Canopy conductance to sensible heat */
	/* not clear why not shared between sun and shaded */
	gc_sh = gl_sh * s->value[LAI];

	s->value[CANOPY_EVAPO] = s->value[CANOPY_TRANSP] = 0.0;

	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat.  Calculate the time required to
	evaporate all the canopy water at the daily average conditions, and
	subtract that time from the daylength to get the effective daylength for
	transpiration. */

	if(s->value[ALL_LAI]>0.0)
	{
		/* if canopy is wet */
		if(s->value[CANOPY_WATER] > 0.0)
		{
			logger(g_log, "Canopy wet\n");

			rv = 1.0/gc_e_wv;
			rh = 1.0/gc_sh;
			net_rad = s->value[NET_RAD_ABS];
			/* call penman-monteith function, returns e in kg/m2/s for evaporation and W/m2 for latent heat*/
			evapo = Penman_Monteith (met, month, day, rv, rh, net_rad);
			s->value[CANOPY_EVAPO] = evapo;
			/* calculate the time required to evaporate all the canopy water */
			evap_daylength = s->value[CANOPY_WATER] / s->value[CANOPY_EVAPO];
			logger(g_log, "evap_daylength = %f sec\n", evap_daylength);

			if(evap_daylength > daylength_sec)
			{
				/* day not long enough to evap. all int. water */
				logger(g_log, "day not long enough to evap all rain intercepted\n");

				days_with_canopy_wet ++;
				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength = 0.0;
				s->value[CANOPY_FRAC_DAY_TRANSP] = 0.0;
				logger(g_log, "transp_daylength = %f\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				s->value[CANOPY_TRANSP] = 0.0;    /* no time left for transpiration */
				s->value[CANOPY_EVAPO] *= daylength_sec * cell_coverage;   /* daylength limits canopy evaporation */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
				/* check if canopy is wet for too long period */
				//CHECK_CONDITION(days_with_canopy_wet, > 10);
			}
			else
			{
				/* all intercepted water evaporated */
				logger(g_log, "all intercepted water evaporated\n");
				days_with_canopy_wet = 0;
				s->value[CANOPY_EVAPO] = s->value[CANOPY_WATER];
				s->value[CANOPY_WATER] = 0.0;

				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength = daylength_sec - evap_daylength;
				s->value[CANOPY_FRAC_DAY_TRANSP] = transp_daylength / daylength_sec;
				logger(g_log, "transp_daylength = %f\n", s->value[CANOPY_FRAC_DAY_TRANSP]);


				/* calculate transpiration using adjusted daylength */
				rv = 1.0/gl_t_wv_sun;
				rh = 1.0/gl_sh;

				//test 11 MAY 2016 following biome approach
				//net_rad = s->value[NET_RAD_ABS_SUN]
				net_rad = s->value[NET_RAD_ABS_SUN] / (1.0 - exp(- s->value[LAI]));
				logger(g_log, "net rad = %f\n", net_rad);
				/* call penman-monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				transp_sun = Penman_Monteith (met, month, day, rv, rh, net_rad);
				transp_sun *=  transp_daylength * s->value[LAI_SUN];
				logger(g_log, "transp_sun = %.10f mm/m2/day\n", transp_sun);

				/* next for shaded canopy fraction */
				rv = 1.0/gl_t_wv_shade;
				rh = 1.0/gl_sh;

				//test 11 May 2016 following biome approach
				//net_rad = s->value[NET_RAD_ABS_SHADE];
				net_rad = s->value[NET_RAD_ABS_SHADE] / (s->value[LAI] - s->value[LAI_SUN]);
				logger(g_log, "net rad = %f\n", net_rad);
				/* call penman-monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				transp_shade = Penman_Monteith (met, month, day, rv, rh, net_rad);
				transp_shade *=  transp_daylength * s->value[LAI_SHADE];
				logger(g_log, "transp_shade = %.10f mm/m2/day\n", transp_shade);

				//test include F_CO2 effect
				transp = transp_sun + transp_shade;
				logger(g_log, "transp = %.10f mm/m2/day\n", transp);
				s->value[CANOPY_TRANSP] = transp;

				/* considering effective coverage of cell */
				s->value[CANOPY_TRANSP] *= cell_coverage;

				//test 12 May 2016 test
				/* including CO2 effect */
				s->value[CANOPY_TRANSP] *= s->value[F_CO2];

				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
			}
		}
		/* if canopy is dry */
		else
		{
			logger(g_log, "Canopy dry\n");
			/* no canopy evaporation occurs */
			s->value[CANOPY_EVAPO]= 0.0;

			/* compute only transpiration */
			/* first for sunlit canopy fraction */
			rv = 1.0/gl_t_wv_sun;
			rh = 1.0/gl_sh;

			//test 11 MAY 2016 following biome approach
			//net_rad = s->value[NET_RAD_ABS_SUN]
			net_rad = s->value[NET_RAD_ABS_SUN] / (1.0 - exp(- s->value[LAI]));
			logger(g_log, "net rad = %f\n", net_rad);

			/* all day transp */
			transp_daylength = 1.0;
			s->value[CANOPY_FRAC_DAY_TRANSP] = 1.0;
			logger(g_log, "transp_daylength = %f\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

			transp_sun = Penman_Monteith (met, month, day, rv, rh, net_rad);
			transp_sun *= daylength_sec * s->value[LAI_SUN];
			logger(g_log, "transp_sun = %.10f mm/m2/day\n", transp_sun);

			/* next for shaded canopy fraction */
			rv = 1.0/gl_t_wv_shade;
			rh = 1.0/gl_sh;

			//test 11 May 2016 following biome approach
			//net_rad = s->value[NET_RAD_ABS_SHADE];
			net_rad = s->value[NET_RAD_ABS_SHADE] / (s->value[LAI] - s->value[LAI_SUN]);
			logger(g_log, "net rad = %f\n", net_rad);
			transp_shade = Penman_Monteith (met, month, day, rv, rh, net_rad);
			transp_shade *= daylength_sec * s->value[LAI_SHADE];
			logger(g_log, "transp_shade = %.10f mm/m2/day\n", transp_shade);

			//test include F_CO2 effect
			transp = transp_sun + transp_shade;
			logger(g_log, "transp = %.10f mm/m2/day\n", transp);
			s->value[CANOPY_TRANSP] = transp;

			/* considering effective coverage of cell and convert to daily amount */
			s->value[CANOPY_TRANSP] *= cell_coverage;

			//test 12 May 2016 test
			/* including CO2 effect */
			s->value[CANOPY_TRANSP] *= s->value[F_CO2];

			s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
		}

		/* following TLEAF in MAESPA model (physiol.f90, row 197) check for consistency in units */

		//TEST
		/* CANOPY SENSIBLE HEAT FLUX */
		logger(g_log, "\ncanopy sensible heat\n");

		logger(g_log, "LAI = %f\n", s->value[LAI]);

		net_rad = s->value[NET_RAD_ABS_SUN];
		logger(g_log, "net rad = %f\n", net_rad);

		/* FIRST OF ALL COMPUTE CANOPY TEMPERATURE */
		/* compute psychrometric (KPa/°C) constant as in Allen et al., 1998 */
		psych = ((CP/1000000.0)*(met[month].d[day].air_pressure/1000.0))/(MWratio*(met[month].d[day].lh_vap/1000000.0));
		logger(g_log, "psych = %f\n", psych);

		/* calculate temperature offsets for slope estimate */
		t1 = met[month].d[day].tday+dt;
		t2 = met[month].d[day].tday-dt;

		/* calculate saturation vapor pressures (Pa) at t1 and t2 */
		pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
		logger(g_log, "pvs1 = %f\n", pvs1);
		pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));
		logger(g_log, "pvs2 = %f\n", pvs2);

		/* calculate slope of pvs vs. T curve, at ta */
		//test this is the "DELTA" function as in Webber et al., 2016
		delta = (pvs1-pvs2) / (t1-t2);
		/* converts into kPA fgollowing Webber et al., 2016 */
		delta /= 1000.0;
		logger(g_log, "delta = %f KPa\n", delta);

		//test
		/* canopy resistance m sec-1)*/
		//fixme gl_sh or gc_sh? Wang and Leuning 1998 use stomatal conductance
		//fixme this is valid for cell level not for class level
		rc = 1.0/gc_sh;

		//test this is the equivalent "ra" aerodynamic resistance as in Allen et al., 1998
		/* calculate resistance to radiative heat transfer through air, rr */
		rr = met[month].d[day].rho_air * CP / (4.0 * SBC * (pow(tairK, 3)));
		rhr = (rh * rr) / (rh + rr);
		/* compute product as psychrometric constant and (1+(rc/ra)) see Webber et al., 2016 */
		/* then ra = rhr */
		psych_p = psych *(1+(rc/rhr));
		logger(g_log, "psych_p = %f\n", psych_p);

		/* canopy temperature as in Webber et al., 2016 */
		tcanopy = met[month].d[day].tavg + ((net_rad * rhr)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);
		logger(g_log, "met[month].d[day].rho_air = %f\n", met[month].d[day].rho_air);
		logger(g_log, "rc = %f\n", rc);
		logger(g_log, "rhr = %f\n", rhr);
		logger(g_log, "tavg = %f °C\n", met[month].d[day].tavg);
		logger(g_log, "canopy temp = %f °C\n", tcanopy);
		logger(g_log, "differences = %f °C\n", tcanopy - met[month].d[day].tavg);

		tcanopyK = tcanopy + TempAbs;

		logger(g_log, "canopy_temp = %f K\n", tcanopyK);
		logger(g_log, "tairK = %f K\n", tairK);

		//fixme this is valid for cell level not for class level
		c->daily_canopy_sensible_heat_flux = met[month].d[day].rho_air * CP * ((tcanopyK-tairK)/rhr);
		logger(g_log, "canopy_sensible_heat_flux = %f Wm-2\n", c->daily_canopy_sensible_heat_flux);
		//getchar();

		//todo
		/*following TLEAF in Campbell and Norman "Environmental Biophysics" 1998 pg 225*/



	}
	else
	{
		s->value[CANOPY_WATER] = 0.0;
		s->value[CANOPY_TRANSP] = 0.0;
		s->value[CANOPY_EVAPO] = 0.0;
		s->value[CANOPY_INT] = 0.0;
		s->value[CANOPY_EVAPO_TRANSP] = 0.0;
	}

	logger(g_log, "CANOPY_TRANSP = %.10f mm/m2/day\n", s->value[CANOPY_TRANSP]);
	logger(g_log, "CANOPY_WATER = %.10f mm/m2/day\n", s->value[CANOPY_WATER]);
	logger(g_log, "CANOPY_INT = %.10f mm/m2/day\n", s->value[CANOPY_INT]);
	logger(g_log, "CANOPY_EVAPO = %.10f mm/m2/day\n", s->value[CANOPY_EVAPO]);
	logger(g_log, "CANOPY_EVAPO_TRANSP = %f mm/m2/day\n", s->value[CANOPY_EVAPO_TRANSP]);

	c->daily_c_int += s->value[CANOPY_INT];
	c->daily_c_evapo += s->value[CANOPY_EVAPO];
	c->daily_c_transp += s->value[CANOPY_TRANSP];
	c->daily_c_water_stored += s->value[CANOPY_WATER];
	c->daily_c_evapotransp += s->value[CANOPY_EVAPO_TRANSP];
	c->daily_c_bl_cond += s->value[CANOPY_BLCOND];

}



