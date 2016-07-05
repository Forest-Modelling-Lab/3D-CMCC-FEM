/*
 * canopy_evapotranspiration.c
 *
 *  Created on: 23/mar/2016
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "canopy_evapotranspiration.h"
#include "constants.h"
#include "logger.h"
#include "Penman_Monteith.h"

extern logger_t* g_log;

void canopy_evapotranspiration(species_t *const s, cell_t *const c, const meteo_t *const met, const int month, const int day, const int height, const int age, const int species)
{

	//double max_int;

	double g_corr;
	double gl_bl;
	double gl_s_sun, gl_s_shade;
	double gl_c;
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

	double leaf_cover_eff;                                            /* fraction of square meter covered by leaf over the gridcell */
	static int days_with_canopy_wet;

	double tairK;
	//double tsoilK;

	//test
	double psych_p;
	double t1, t2;
	double pvs1, pvs2;
	double dt = 0.2;
	double delta;
	double rc, rr, rhr;
	double rh_mol; //boundary layer resistance in mol m/sec
	double tcanopy, tcanopyK;

	/* it mainly follows rationale and algorithms of BIOME-BGC v.4.2 */

	tairK = met[month].d[day].tavg + TempAbs;

	/* assign values of previous day canopy water (it determines canopy water fluxes */
	/* reset if LAI == 0.0*/
	if(s->value[LAI] == 0.0)
	{
		s->value[OLD_CANOPY_WATER]= 0.0;
		s->value[CANOPY_WATER] = 0.0;
	}
	/* otherwise assign values of the day before */
	else
	{
		s->value[OLD_CANOPY_WATER]= s->value[CANOPY_WATER];
	}

	logger(g_log, "\n**CANOPY EVAPO-TRANSPIRATION**\n");

	/* compute effective canopy cover */
	if(s->value[LAI] < 1.0)
	{
		/* special case when LAI = < 1.0 */
		leaf_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	}
	else
	{
		leaf_cover_eff = s->value[CANOPY_COVER_DBHDC];
	}
	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cover_eff > 1.0)
	{
		leaf_cover_eff = 1.0;
	}

//	leaf_cover_eff = s->value[CANOPY_COVER_DBHDC];
//	if(leaf_cover_eff > 1.0) leaf_cover_eff = 1.0;

	daylength_sec = met[month].d[day].daylength * 3600.0;
	
	/********************************************************************************************************/
	/* compute interception for dry canopy (Lawrence et al., 2006) */
	if(met[month].d[day].prcp > 0.0 && s->value[LAI]>0.0 && s->value[CANOPY_WATER] == 0.0)
	{
		/* for rain */
		if(c->prcp_rain != 0.0)
		{
			s->value[CANOPY_INT] = s->value[INT_COEFF]*c->prcp_rain*(1.0 - exp(-0.5 * s->value[LAI])) * leaf_cover_eff;
			s->value[CANOPY_WATER] = s->value[CANOPY_INT];
			CHECK_CONDITION(s->value[CANOPY_INT], > c->prcp_rain);
		}
		/* for snow */
		else
		{
			//s->value[CANOPY_INT] = s->value[INT_COEFF]*c->prcp_snow*(1.0 - exp(-0.5 * s->value[LAI])) * leaf_cover_eff;
			//s->value[CANOPY_SNOW] = s->value[CANOPY_INT];
		}
	}
	else s->value[CANOPY_INT] = 0.0;
	/********************************************************************************************************/
	
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
	s->value[CANOPY_BLCOND] *= daylength_sec * leaf_cover_eff;

	/* leaf cuticular conductance */
	gl_c = s->value[CUTCOND] * g_corr;

	/* leaf stomatal conductance: first generate multipliers, then apply them to maximum stomatal conductance */
	/* apply all multipliers to the maximum stomatal conductance */
	/* differently from BIOME we use F_T that takes into account not only minimum temperature effects */
	m_final_sun = s->value[F_LIGHT_SUN] * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];
	m_final_shade = s->value[F_LIGHT_SHADE] * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];

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
	gl_t_wv_sun = (gl_bl * (gl_s_sun + gl_c)) / (gl_bl + gl_s_sun + gl_c);
	gl_t_wv_shade = (gl_bl * (gl_s_shade + gl_c)) / (gl_bl + gl_s_shade + gl_c);

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh = gl_bl;
	//logger(g_log, "Leaf conductance to sensible heat = %g\n", gl_sh);

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI];

	/* Canopy conductance to sensible heat */
	/* note: not clear why not shared between sun and shaded */
	gc_sh = gl_sh * s->value[LAI];

	s->value[CANOPY_EVAPO] = s->value[CANOPY_TRANSP] = 0.0;

	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat.  Calculate the time required to
	evaporate all the canopy water at the daily average conditions, and
	subtract that time from the daylength to get the effective daylength for
	transpiration. */

	if(s->value[LAI]>0.0)
	{
		//fixme why for evaporation BIOME uses stomatal conductance??
		/* if canopy is wet */
		if(s->value[CANOPY_WATER] > 0.0)
		{
			logger(g_log, "\n*CANOPY EVAPORATION (Canopy Wet) *\n");
			logger(g_log, "LAI = %g\n",s->value[LAI]);
			logger(g_log, "CANOPY_WATER = %g mm\n",s->value[CANOPY_WATER]);

			rv = 1.0/gc_e_wv;
			rh = 1.0/gc_sh;
			net_rad = s->value[NET_RAD_ABS];

			/* call Penman-Monteith function, returns e in kg/m2/s for evaporation and W/m2 for latent heat*/
			evapo = Penman_Monteith (met, month, day, rv, rh, net_rad);

			/* check for negative values */
			if(evapo < 0.0) evapo = 0.0;

			s->value[CANOPY_EVAPO] = evapo;

			/* calculate the time required to evaporate all the canopy water */
			evap_daylength = s->value[CANOPY_WATER] / s->value[CANOPY_EVAPO];
			logger(g_log, "evap_daylength = %g sec\n", evap_daylength);

			/* day not long enough to evap. all int. water */
			if(evap_daylength > daylength_sec)
			{
				logger(g_log, "day not long enough to evap all rain intercepted\n");

				days_with_canopy_wet ++;

				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength = 0.0;
				s->value[CANOPY_FRAC_DAY_TRANSP] = 0.0;
				logger(g_log, "transp_daylength = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				s->value[CANOPY_TRANSP] = 0.0;             /* no time left for transpiration */
				s->value[CANOPY_EVAPO] *= daylength_sec;   /* daylength limits canopy evaporation */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
				/* check if canopy is wet for too long period */
				//CHECK_CONDITION(days_with_canopy_wet, > 10);
			}
			/* all intercepted water evaporated */
			else
			{
				logger(g_log, "all intercepted water evaporated\n");
				logger(g_log, "\n*CANOPY TRANSPIRATION (Canopy Dry)*\n");

				days_with_canopy_wet = 0;
				s->value[CANOPY_EVAPO] = s->value[CANOPY_WATER];
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];

				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength = daylength_sec - evap_daylength;
				s->value[CANOPY_FRAC_DAY_TRANSP] = transp_daylength / daylength_sec;
				logger(g_log, "transp_daylength = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				/* calculate transpiration using adjusted daylength */
				rv = 1.0/gl_t_wv_sun;
				rh = 1.0/gl_sh;

				net_rad = s->value[NET_RAD_ABS_SUN] / (1.0 - exp(- s->value[LAI]));
				logger(g_log, "net rad = %g\n", net_rad);
				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				transp_sun = Penman_Monteith (met, month, day, rv, rh, net_rad);
				transp_sun *=  transp_daylength * s->value[LAI_SUN];
				logger(g_log, "transp_sun = %g mm/m2/day\n", transp_sun);

				/* next for shaded canopy fraction */
				rv = 1.0/gl_t_wv_shade;
				rh = 1.0/gl_sh;

				net_rad = s->value[NET_RAD_ABS_SHADE] / (s->value[LAI] - s->value[LAI_SUN]);
				logger(g_log, "net rad = %g\n", net_rad);

				/* call penman-monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				transp_shade = Penman_Monteith (met, month, day, rv, rh, net_rad);
				transp_shade *=  transp_daylength * s->value[LAI_SHADE];
				logger(g_log, "transp_shade = %g mm/m2/day\n", transp_shade);

				transp = transp_sun + transp_shade;
				logger(g_log, "transp = %g mm/m2/day\n", transp);

				/* check for negative values */
				if(transp < 0.0) transp = 0.0;

				s->value[CANOPY_TRANSP] = transp;

				/* considering effective coverage of cell */
				//test 6 July 2016
				//s->value[CANOPY_TRANSP] *= leaf_cover_eff;

				/* including CO2 effect */
				s->value[CANOPY_TRANSP] *= s->value[F_CO2];

				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
			}
		}
		/* if canopy is dry */
		else
		{
			logger(g_log, "*CANOPY TRANSPIRATION (Canopy Dry)*\n");
			/* no canopy evaporation occurs */
			s->value[CANOPY_EVAPO]= 0.0;

			/* compute only transpiration */
			/* first for sunlit canopy fraction */
			rv = 1.0/gl_t_wv_sun;
			rh = 1.0/gl_sh;

			/* note: differently from Biome model uses Net Radiation instead Short wave flux */
			net_rad = s->value[NET_RAD_ABS_SUN] / (1.0 - exp(- s->value[LAI]));
			logger(g_log, "net rad = %g\n", net_rad);

			/* all day transp */
			transp_daylength = 1.0;
			s->value[CANOPY_FRAC_DAY_TRANSP] = 1.0;
			logger(g_log, "transp_daylength = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

			transp_sun = Penman_Monteith (met, month, day, rv, rh, net_rad);
			transp_sun *= daylength_sec * s->value[LAI_SUN];
			logger(g_log, "transp_sun = %g mm/m2/day\n", transp_sun);

			/* next for shaded canopy fraction */
			rv = 1.0/gl_t_wv_shade;
			rh = 1.0/gl_sh;

			/* note: differently from Biome model uses Net Radiation instead Short wave flux */
			net_rad = s->value[NET_RAD_ABS_SHADE] / (s->value[LAI] - s->value[LAI_SUN]);
			logger(g_log, "net rad = %g\n", net_rad);
			transp_shade = Penman_Monteith (met, month, day, rv, rh, net_rad);
			transp_shade *= daylength_sec * s->value[LAI_SHADE];
			logger(g_log, "transp_shade = %g mm/m2/day\n", transp_shade);

			transp = transp_sun + transp_shade;
			logger(g_log, "transp = %g mm/m2/day\n", transp);

			/* check for negative values */
			if(transp < 0.0) transp = 0.0;

			/* assign values */
			s->value[CANOPY_TRANSP] = transp;

			/* considering effective coverage of cell and convert to daily amount */
			//test 6 July 2016
			//s->value[CANOPY_TRANSP] *= leaf_cover_eff;

			/* including CO2 effect */
			s->value[CANOPY_TRANSP] *= s->value[F_CO2];

			s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
		}

		/********************************************************************************************************************/

		/* following TLEAF in MAESPA model (physiol.f90, row 197) check for consistency in units */

		//TEST
		/* CANOPY SENSIBLE HEAT FLUX */
		//logger(g_log, "\ncanopy sensible heat\n");

		//logger(g_log, "LAI = %g\n", s->value[LAI]);

		net_rad = s->value[NET_RAD_ABS];
		//logger(g_log, "net rad = %g\n", net_rad);

		/* FIRST OF ALL COMPUTE CANOPY TEMPERATURE */
		/* calculate temperature offsets for slope estimate */
		t1 = met[month].d[day].tday+dt;
		t2 = met[month].d[day].tday-dt;

		/* calculate saturation vapor pressures (Pa) at t1 and t2 */
		pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
		//logger(g_log, "pvs1 = %g\n", pvs1);
		pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));
		//logger(g_log, "pvs2 = %g\n", pvs2);

		/* calculate slope of pvs vs. T curve, at ta */
		//test this is the "DELTA" function as in Webber et al., 2016
		delta = (pvs1-pvs2) / (t1-t2);
		/* converts into kPA following Webber et al., 2016 */
		delta /= 1000.0;
		//logger(g_log, "delta = %g KPa\n", delta);

		//test
		// as in Ryder et al., 2016 resistance to sensible heat flux is equal to boundary layer resistance (see also BIOME)
		// so it should be rh (1/gl_sh)

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
		psych_p = met[month].d[day].psych *(1+(rc/rhr));
		//logger(g_log, "psych_p = %g\n", psych_p);

		//test to avoid problems using generic daily data we should divide the fluxes into diurnal (using tday) and nocturnal (using tnight) (but for net_rad???)
		//to have tcanopy_day and tcanopy_night considering day length

		//fixme conductance and resistance variables following Norman And Campbell should be in mol m sec (not m sec)
		//test convert boundary layer conductance from m/sec into mol m/sec
		//following Pearcy, Schulze and Zimmermann
		rh_mol = rh * 0.0446 * ((TempAbs / (met[month].d[day].tavg+TempAbs))*((met[month].d[day].air_pressure/1000.0)/101.3));
		//logger(g_log, "boundary layer resistance = %g mol m/sec\n", rh_mol);

		/* canopy temperature as in Webber et al., 2016 it takes rh in m/sec*/
		tcanopy = met[month].d[day].tavg + ((net_rad * rh_mol)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);

		//test 07 june 2016 using only boundary layer resistance as in Ryder et al., 2016
		//tcanopy = met[month].d[day].tavg + ((net_rad * rh)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);

		tcanopyK = tcanopy + TempAbs;
		//logger(g_log, "canopy_temp = %g K\n", tcanopyK);
		//logger(g_log, "tairK = %g K\n", tairK);

		//fixme this is valid for cell level not for class level
		c->daily_canopy_sensible_heat_flux = met[month].d[day].rho_air * CP * ((tcanopyK-tairK)/rhr);
		//logger(g_log, "canopy_sensible_heat_flux = %g Wm-2\n", c->daily_canopy_sensible_heat_flux);

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

	logger(g_log, "OLD CANOPY_WATER = %g mm/m2/day\n", s->value[OLD_CANOPY_WATER]);
	logger(g_log, "CANOPY_WATER = %g mm/m2/day\n", s->value[CANOPY_WATER]);
	logger(g_log, "CANOPY_INT = %g mm/m2/day\n", s->value[CANOPY_INT]);
	logger(g_log, "CANOPY_EVAPO = %g mm/m2/day\n", s->value[CANOPY_EVAPO]);
	logger(g_log, "CANOPY_TRANSP = %g mm/m2/day\n", s->value[CANOPY_TRANSP]);
	logger(g_log, "CANOPY_EVAPO_TRANSP = %g mm/m2/day\n", s->value[CANOPY_EVAPO_TRANSP]);

	c->daily_c_int += s->value[CANOPY_INT];
	c->daily_c_evapo += s->value[CANOPY_EVAPO];
	c->daily_c_transp += s->value[CANOPY_TRANSP];
	c->daily_c_water_stored += (s->value[CANOPY_WATER] - s->value[OLD_CANOPY_WATER]);
	c->daily_c_evapotransp += s->value[CANOPY_EVAPO_TRANSP];
	c->daily_c_bl_cond += s->value[CANOPY_BLCOND];
}



