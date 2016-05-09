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


	Log("\n**CANOPY EVAPO-TRANSPIRATION BIOME**\n");

	Log("**CANOPY INTERCEPTION BIOME**\n");

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
//	//Log("m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
//	m_ppfd_shade = (s->value[APAR_SHADE] * daylength_sec)/(PPFD50 + (s->value[APAR_SHADE]* daylength_sec));
//	//Log("m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);

	//04/apr/2016
	/* photosynthetic photon flux density conductance control */
	m_ppfd_sun = s->value[PPFD_SUN] /(PPFD50 + s->value[PPFD_SUN]);
	Log("m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
	m_ppfd_shade = s->value[PPFD_SHADE] /(PPFD50 + s->value[PPFD_SHADE]);
	Log("m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);

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
	Log("Leaf conductance to sensible heat = %f\n", gl_sh);

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
				/* day not long enough to evap. all int. water */
				Log("day not long enough to evap all rain intercepted\n");

				days_with_canopy_wet ++;
				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength = 0.0;

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
				Log("all intercepted water evaporated\n");
				days_with_canopy_wet = 0;
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

				//test include F_CO2 effect
				transp = transp_sun + transp_shade;
				Log("transp = %.10f mm/m2/day\n", transp);
				s->value[CANOPY_TRANSP] = transp;
				/* considering effective coverage of cell */
				s->value[CANOPY_TRANSP] *= cell_coverage;

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

			//test include F_CO2 effect
			transp = transp_sun + transp_shade;
			Log("transp = %.10f mm/m2/day\n", transp);
			s->value[CANOPY_TRANSP] = transp;
			/* considering effective coverage of cell and convert to daily amount */
			s->value[CANOPY_TRANSP] *= cell_coverage;
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
	c->daily_c_bl_cond += s->value[CANOPY_BLCOND];

}



