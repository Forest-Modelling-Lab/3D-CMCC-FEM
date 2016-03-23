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

void canopy_evapotranspiration_biome (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height, int age, int species)
{
	//fixme it must be include in species.txt file
	double precip_int_coeff = 0.034; //LAI-1day-1
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

	Log("\n**CANOPY EVAPO-TRANSPIRATION BIOME**\n");

	Log("**CANOPY INTERCEPTION BIOME**\n");

	daylength_sec = met[month].d[day].daylength * 3600.0;

	max_int = precip_int_coeff * s->value[ALL_LAI] * s->value[CANOPY_COVER_DBHDC];

	Log("ALL_LAI = %f mm/m^2/day\n", s->value[ALL_LAI]);

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

	Log("Rain intercepted = %f mm/m^2/day\n", s->value[CANOPY_INT]);
	Log("CANOPY_WATER = %f mm/m^2/day\n", s->value[CANOPY_WATER]);




	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+273.15)/293.15, 1.75) * 101300/c->air_pressure;

	/* calculate leaf- and canopy-level conductances to water vapor and
		sensible heat fluxes */

	/* leaf boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;

	/* leaf cuticular conductance */
	gl_c = s->value[CUTCOND] * g_corr;

	/* leaf stomatal conductance: first generate multipliers, then apply them
		to maximum stomatal conductance */

	/* photosynthetic photon flux density conductance control */
	m_ppfd_sun = (s->value[APAR_SUN] * daylength_sec)/(PPFD50 + (s->value[APAR_SUN]* daylength_sec));
	//Log("m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
	m_ppfd_shade = (s->value[APAR_SHADE] * daylength_sec)/(PPFD50 + (s->value[APAR_SHADE]* daylength_sec));
	//Log("m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);

	/* apply all multipliers to the maximum stomatal conductance */
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
			Log("Canopy wet\n");

			rv = 1.0/gc_e_wv;
			rh = 1.0/gc_sh;
			net_rad = s->value[NET_RAD_ABS];
			/* call penman-monteith function, returns e in kg/m2/s for evaporation and W/m2 for latent heat*/
			evapo = Penman_Monteith (met, month, day, rv, rh, net_rad);
			s->value[CANOPY_EVAPO] = evapo;
			/* calculate the time required to evaporate all the canopy water */
			evap_daylength = s->value[CANOPY_WATER] / s->value[CANOPY_EVAPO];
			Log("evap_daylength = %f sec\n", evap_daylength);

			if(evap_daylength > daylength_sec)
			{
				Log("day not long enough to evap all rain intercepted\n");
				/* day not long enough to evap. all int. water */
				s->value[CANOPY_TRANSP] = 0.0;    /* no time left for transpiration */
				s->value[CANOPY_EVAPO] *= daylength_sec * s->value[CANOPY_COVER_DBHDC];   /* daylength limits canopy evaporation */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
			}
			else
			{
				Log("all intercepted water evaporated\n");
				/* all intercepted water evaporated */
				s->value[CANOPY_EVAPO] = s->value[CANOPY_WATER];
				s->value[CANOPY_WATER] = 0.0;

				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength = daylength_sec - evap_daylength;

				/* calculate transpiration using adjusted daylength */
				rv = 1.0/gl_t_wv_sun;
				rh = 1.0/gl_sh;
				net_rad = s->value[NET_RAD_ABS_SUN];
				/* call penman-monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				transp_sun = Penman_Monteith (met, month, day, rv, rh, net_rad);
				transp_sun *=  transp_daylength * s->value[LAI_SUN];
				Log("transp_sun = %.10f mm/m2/day\n", transp_sun);

				/* next for shaded canopy fraction */
				rv = 1.0/gl_t_wv_shade;
				rh = 1.0/gl_sh;
				net_rad = s->value[NET_RAD_ABS_SHADE];
				/* call penman-monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				transp_shade = Penman_Monteith (met, month, day, rv, rh, net_rad);
				transp_shade *=  transp_daylength * s->value[LAI_SHADE];
				Log("transp_shade = %.10f mm/m2/day\n", transp_shade);

				transp = transp_sun + transp_shade;
				Log("transp = %.10f mm/m2/day\n", transp);
				s->value[CANOPY_TRANSP] = transp;
				/* considering effective coverage of cell */
				s->value[CANOPY_TRANSP] *= s->value[CANOPY_COVER_DBHDC];

				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
			}
		}
		/* if canopy is dry */
		else
		{
			Log("Canopy dry\n");
			/* no canopy evaporation occurs */
			s->value[CANOPY_EVAPO]= 0.0;

			/* compute only transpiration */
			/* first for sunlit canopy fraction */
			rv = 1.0/gl_t_wv_sun;
			rh = 1.0/gl_sh;
			net_rad = s->value[NET_RAD_ABS_SUN];
			transp_sun = Penman_Monteith (met, month, day, rv, rh, net_rad);
			transp_sun *= daylength_sec * s->value[LAI_SUN];
			Log("transp_sun = %.10f mm/m2/day\n", transp_sun);

			/* next for shaded canopy fraction */
			rv = 1.0/gl_t_wv_shade;
			rh = 1.0/gl_sh;
			net_rad = s->value[NET_RAD_ABS_SHADE];
			transp_shade = Penman_Monteith (met, month, day, rv, rh, net_rad);
			transp_shade *= daylength_sec * s->value[LAI_SHADE];
			Log("transp_shade = %.10f mm/m2/day\n", transp_shade);

			transp = transp_sun + transp_shade;
			Log("transp = %.10f mm/m2/day\n", transp);
			s->value[CANOPY_TRANSP] = transp;
			/* considering effective coverage of cell and convert to daily amount */
			s->value[CANOPY_TRANSP] *= s->value[CANOPY_COVER_DBHDC];
			s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];

		}
	}
	else
	{
		s->value[CANOPY_WATER] = 0.0;
		s->value[CANOPY_TRANSP] = 0.0;
		s->value[CANOPY_EVAPO] = 0.0;
		s->value[CANOPY_INT] = 0.0;
		s->value[CANOPY_EVAPO_TRANSP] = 0.0;
	}

	Log("CANOPY_TRANSP = %.10f mm/m2/day\n", s->value[CANOPY_TRANSP]);
	Log("CANOPY_WATER = %.10f mm/m2/day\n", s->value[CANOPY_WATER]);
	Log("CANOPY_INT = %.10f mm/m2/day\n", s->value[CANOPY_INT]);
	Log("CANOPY_EVAPO = %.10f mm/m2/day\n", s->value[CANOPY_EVAPO]);
	Log("CANOPY_EVAPO_TRANSP = %f mm/m2/day\n", s->value[CANOPY_EVAPO_TRANSP]);

	c->daily_c_int += s->value[CANOPY_INT];
	c->daily_c_evapo += s->value[CANOPY_EVAPO];
	c->daily_c_transp += s->value[CANOPY_TRANSP];
	c->daily_c_water_stored += s->value[CANOPY_WATER];
	c->daily_c_evapotransp += s->value[CANOPY_EVAPO_TRANSP];

	/*compute energy balance transpiration from canopy*/
	c->daily_c_evapo_watt = c->daily_c_evapo * c->lh_vap / 86400.0;
	c->daily_c_transp_watt = c->daily_c_transp * c->lh_vap / 86400.0;
	c->daily_c_evapotransp_watt = c->daily_c_evapo_watt + c->daily_c_transp_watt;
	Log("Latent heat canopy evapotranspiration = %f W/m^2\n", c->daily_c_evapotransp_watt);
}


double Penman_Monteith (const MET_DATA *const met, int month, int day, int rh, int rv, double net_rad)
{
	double tk;
	double esse;
	double t1,t2,pvs1,pvs2;
	double rr, rhr;
	double dt = 0.2;     /* set the temperature offset for slope calculation */
	double airpressure;

	double evap_or_transp;

	double a1, a2;

	double lhvap;


	Log("**Penmon**\n");
	/*compute air pressure*/
	/* daily atmospheric pressure (Pa) as a function of elevation (m) */
	/* From the discussion on atmospheric statics in:
	Iribane, J.V., and W.L. Godson, 1981. Atmospheric Thermodynamics, 2nd
		Edition. D. Reidel Publishing Company, Dordrecht, The Netherlands.
		(p. 168)*/
	a1 = 1.0 - (LR_STD * site->elev)/T_STD;
	a2 = G_STD / (LR_STD * (Rgas / MA));
	airpressure = P_STD * pow (a1, a2);

	/* compute latent heat */
	lhvap = 2.5023e6 - 2430.54 * met[month].d[day].tavg;

	/* assign ta (Celsius) and tk (Kelvins) */
	tk = met[month].d[day].tday + 273.15;

	/* calculate resistance to radiative heat transfer through air, rr */
	rr = met[month].d[day].rho_air * CP / (4.0 * SBC * (tk*tk*tk));

	/* calculate combined resistance to convective and radiative heat transfer,
	    parallel resistances : rhr = (rh * rr) / (rh + rr) */
	rhr = (rh * rr) / (rh + rr);

	/* calculate temperature offsets for slope estimate */
	t1 = met[month].d[day].tday+dt;
	t2 = met[month].d[day].tday-dt;

	/* calculate saturation vapor pressures at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

	/* calculate slope of pvs vs. T curve, at ta */
	esse = (pvs1-pvs2) / (t1-t2);

	/* latent heat fluxes of evaporation or transpiration W/m2 */
	evap_or_transp = ((esse * net_rad) + (met[month].d[day].rho_air * CP * (met[month].d[day].vpd / 100.0) / rhr)) /
			(((airpressure * CP * rv) / (lhvap * EPS * rhr)) + esse);

	/* evporation or transpiration in convrte into kg-mm/m2/sec */
	evap_or_transp /= lhvap;

	/* check */
	if (evap_or_transp < 0.0) evap_or_transp = 0.0;

	return(evap_or_transp);
}
