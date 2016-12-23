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
#include "canopy_interception.h"
#include "constants.h"
#include "logger.h"
#include "Penman_Monteith.h"
#include "heat_fluxes.h"

extern logger_t* g_debug_log;

void canopy_evapotranspiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	double g_corr;
	double gl_bl;
	double gl_x;                                                          /* maximum stomatal conductance */
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
	double rh;                                                             /* Leaf-Canopy resistance to sensible heat */
	double daylength_sec;                                                  /* daylength in sec */
	double evap_daylength_sec;
	double transp_daylength_sec;
	double evapo;
	double transp, transp_sun, transp_shade;
	static int days_with_canopy_wet;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	daylength_sec = meteo_daily->daylength * 3600.0;

	/* it mainly follows rationale and algorithms of BIOME-BGC v.4.2 */
	/* it computes canopy interception, evaporation and transpiration */

	/* assign values of previous day canopy water/snow */
	/* reset if LAI == 0.0*/
	if( !s->value[LAI_PROJ] )
	{
		s->value[OLD_CANOPY_WATER]= 0.0;
		s->value[CANOPY_WATER] = 0.0;
		s->value[CANOPY_SNOW] = 0.0;
		s->value[OLD_CANOPY_SNOW] = 0.0;
	}
	/* otherwise assign values of the day before */
	else
	{
		/* in case of water on canopy */
		if ( s->value[CANOPY_WATER] >= 0.0 )s->value[OLD_CANOPY_WATER]= s->value[CANOPY_WATER];

		/* in case of snow on canopy */
		if ( s->value[CANOPY_SNOW] >= 0 ) s->value[OLD_CANOPY_SNOW]= s->value[CANOPY_SNOW];
	}

	/********************************************************************************************************/

	logger(g_debug_log, "\n**CANOPY EVAPO-TRANSPIRATION**\n");

	/********************************************************************************************************/

	/* call canopy interception */
	canopy_interception( c, layer, height, dbh, age, species, meteo_daily );

	/********************************************************************************************************/
	/* temperature and pressure correction factor for conductances */
	g_corr = pow((meteo_daily->tday+TempAbs)/293.15, 1.75) * 101300/meteo_daily->air_pressure;

	/* calculate leaf- and canopy-level conductances to water vapor and
		sensible heat fluxes */

	/* leaf aerodynamic-boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;

#if 0
	/**************************************************************************/
	/* aerodynamic boundary layer conductance corrected for wind speed */
	/* following Shi et al., 2008 Journal of Biophysical Research */
	if ( meteo_daily->windspeed != NO_DATA)
	{
		double zero = 20;    /* height measurement of wind speed (m) */ //fixme add to settings.txt data
		double rough;        /* roughness length */
		double plane;        /* zero place displacement */

		/* compute roughness length (m) */
		rough = 0.032 * h->value;

		/* compute zero plane displacement (m) */
		plane = 0.88 * h->value;

		/* aerodynamic boundary layer conductance */
		gl_bl = pow ( KARM , 2.) / pow ( log ( ( zero - plane) / rough ) , 2. );
	}
	/**************************************************************************/
#endif

	/* canopy boundary layer conductance */
	s->value[CANOPY_BLCOND] = gl_bl * s->value[LAI_PROJ];

	/* leaf cuticular conductance corrected for temperature and air pressure */
	gl_c = s->value[CUTCOND] * g_corr;

	/* leaf stomatal conductance: first generate multipliers, then apply them to maximum stomatal conductance */
	/* apply all multipliers to the maximum stomatal conductance */
	/* differently from BIOME we use F_T that takes into account not only minimum temperature effects */
	/* differently from BIOME we use also F_AGE */

	m_final_sun = s->value[F_LIGHT_SUN] * s->value[F_SW] * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];
	m_final_shade = s->value[F_LIGHT_SHADE] * s->value[F_SW]  * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];

	if (m_final_sun < 0.00000001) m_final_sun = 0.00000001;
	if (m_final_shade < 0.00000001) m_final_shade = 0.00000001;


	/* correct maximum stomatal conductance for CO2 concentration*/
	gl_x = (s->value[F_CO2_TR] / 0.9116) * s->value[MAXCOND];

	/* following Jarvis 1997 approach (not more used) */
	//gl_s_sun = s->value[MAXCOND] * m_final_sun * g_corr;
	//gl_s_shade = s->value[MAXCOND] * m_final_shade * g_corr;

	/* following Jarvis 1997 + Frank et al., 2013 + Hidy et al., 2016 GMDD */
	gl_s_sun = gl_x * m_final_sun * g_corr;
	gl_s_shade = gl_x * m_final_shade * g_corr;

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

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI_PROJ];

	/* Canopy conductance to sensible heat */
	gc_sh = gl_sh * s->value[LAI_PROJ];

	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat.  Calculate the time required to
	evaporate all the canopy water at the daily average conditions, and
	subtract that time from the daylength to get the effective daylength for
	transpiration. */

	/* note: as in Campbell and Norman, Environmental Biophysics,
	model should use Absorbed Radiation (instead just incident) but
	composed of Short and Long wave radiation */

	if(s->value[LAI_PROJ]>0.0)
	{
		/* if canopy has water */
		if(s->value[CANOPY_WATER] > 0.0)
		{
			logger(g_debug_log, "\n*CANOPY EVAPORATION (Canopy Wet) *\n");
			logger(g_debug_log, "LAI_PROJ = %g\n",s->value[LAI_PROJ]);
			logger(g_debug_log, "CANOPY_WATER = %g mm\n",s->value[CANOPY_WATER]);

			rv = 1.0/gc_e_wv;
			rh = 1.0/gc_sh;

			/* radiation */
			net_rad = s->value[SW_RAD_ABS];
			logger(g_debug_log, "sw rad for evaporation (tot LAI) = %g W/m2\n", net_rad);

			/* call Penman-Monteith function, it returns Potential evaporation in kg/m2/s for evaporation and W/m2 for latent heat*/
			//fixme use correct net radiation
			evapo = Penman_Monteith (meteo_daily, rv, rh, net_rad);

			/* check for negative values */
			if(evapo < 0.0) evapo = 0.0;

			s->value[CANOPY_EVAPO] = evapo;

			/* calculate the time required to evaporate all the canopy water */
			evap_daylength_sec = s->value[CANOPY_WATER] / s->value[CANOPY_EVAPO];
			logger(g_debug_log, "evap_daylength_sec = %g sec\n", evap_daylength_sec);

			/* day not long enough to evap. all int. water */
			if(evap_daylength_sec > daylength_sec)
			{
				logger(g_debug_log, "day not long enough to evap all rain intercepted\n");

				logger(g_debug_log, "\n*NO CANOPY TRANSPIRATION (Canopy completely Wet)!!!*\n");

				++days_with_canopy_wet;

				/* adjust daylength for transpiration */
				transp_daylength_sec = 0.0;
				s->value[CANOPY_FRAC_DAY_TRANSP] = 0.0;
				logger(g_debug_log, "transp_daylength_sec = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				/* no time left for transpiration */
				s->value[CANOPY_TRANSP] = 0.0;

				/* day length limits canopy evaporation */
				evapo *= daylength_sec;
				s->value[CANOPY_EVAPO] = evapo;
				logger(g_debug_log, "Canopy evaporation = %g mm\n", s->value[CANOPY_EVAPO]);

				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
				logger(g_debug_log, "Canopy evapo-transpiration = %g mm\n", s->value[CANOPY_EVAPO_TRANSP]);

				/* remove evaporated water from canopy */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				logger(g_debug_log, "Canopy water = %g mm\n", s->value[CANOPY_WATER]);

				/* check if canopy is wet for too long period */
				//CHECK_CONDITION(days_with_canopy_wet, >, 30);
			}
			/* all intercepted water evaporated */
			else
			{
				logger(g_debug_log, "all intercepted water evaporated\n");
				days_with_canopy_wet = 0;

				/* all canopy water evaporates */
				evapo = s->value[CANOPY_WATER];

				s->value[CANOPY_EVAPO] = evapo;
				logger(g_debug_log, "Canopy evaporation = %g mm\n", s->value[CANOPY_EVAPO]);

				/* reset canopy water */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				logger(g_debug_log, "Canopy water = %g mm\n", s->value[CANOPY_WATER]);

				logger(g_debug_log, "\n*CANOPY TRANSPIRATION (Partial Canopy Wet)*\n");

				/* adjust daylength for transpiration */
				transp_daylength_sec = daylength_sec - evap_daylength_sec;

				s->value[CANOPY_FRAC_DAY_TRANSP] = transp_daylength_sec / daylength_sec;
				logger(g_debug_log, "transp_daylength_sec = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				/* Leaf-Canopy resistance to sensible heat */
				rh = 1.0/gl_sh;

				/************************************************************************************/
				/* calculate transpiration using adjusted day length */
				/* LAI SUN */
				logger(g_debug_log, "\n--Transpiration for LAI sun--\n");

				/* for sun canopy fraction */
				rv = 1.0/gl_t_wv_sun;

				/* note: Net Rad is Short wave flux */
				net_rad = s->value[SW_RAD_ABS_SUN] / (1.0 - exp(- s->value[LAI_PROJ]));
				logger(g_debug_log, "sw rad for evaporation (LAI sun) = %g W/m2\n", net_rad);

				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat */
				//fixme use correct net radiation
				transp_sun = Penman_Monteith (meteo_daily, rv, rh, net_rad);
				transp_sun *= transp_daylength_sec * s->value[LAI_SUN_PROJ];
				logger(g_debug_log, "transp_sun = %g mm/m2/day\n", transp_sun);

				/************************************************************************************/
				/* calculate transpiration using adjusted day length */
				/* LAI SHADE */
				logger(g_debug_log, "\n--Transpiration for LAI shade--\n");

				/* for shaded canopy fraction */
				rv = 1.0/gl_t_wv_shade;

				/* note: Net Rad is Short wave flux */
				net_rad = s->value[SW_RAD_ABS_SHADE] / (s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ]);
				logger(g_debug_log, "sw rad for evaporation (LAI shade) = %g W/m2\n", net_rad);

				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				//fixme use correct net radiation
				transp_shade = Penman_Monteith (meteo_daily, rv, rh, net_rad);
				transp_shade *= transp_daylength_sec * s->value[LAI_SHADE_PROJ];
				logger(g_debug_log, "transp_shade = %g mm/m2/day\n", transp_shade);

				/************************************************************************************/
				/* overall canopy */
				transp = transp_sun + transp_shade;

				/* check for negative values */
				if(transp < 0.0) transp = 0.0;

				s->value[CANOPY_TRANSP] = transp;

				/* considering effective coverage of cell */
				s->value[CANOPY_TRANSP] *= /*leaf_cell_cover_eff*/s->value[DAILY_CANOPY_COVER_EXP];
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];

			}
		}
		/* if canopy has snow */
		else if (s->value[CANOPY_SNOW] > 0.0)
		{
			//todo get functions from snow_melt_subl snow subl(evaporated) or melt (that goes to soil pool) for canopy intercepted snow
		}
		/** if canopy is dry **/
		else
		{
			/* no canopy evaporation occurs */
			evapo = 0.0;

			s->value[CANOPY_EVAPO] = evapo;
			logger(g_debug_log, "Canopy evaporation = %g mm\n", s->value[CANOPY_EVAPO]);

			logger(g_debug_log, "*CANOPY TRANSPIRATION (Canopy Dry)*\n");

			/* all day transp */
			transp_daylength_sec = 1.0;
			s->value[CANOPY_FRAC_DAY_TRANSP] = 1.0;

			/* Leaf-Canopy resistance to sensible heat */
			rh = 1.0/gl_sh;

			/************************************************************************************/
			/* calculate transpiration using adjusted day length */
			/* compute only transpiration */

			/* LAI SUN */
			logger(g_debug_log, "\n--Transpiration for LAI sun--\n");

			rv = 1.0/gl_t_wv_sun;

			/* note: Net Rad is Short wave flux */
			//fixme why??????????
			net_rad = s->value[SW_RAD_ABS_SUN] / (1.0 - exp(- s->value[LAI_PROJ]));
			logger(g_debug_log, "sw rad for evaporation (LAI sun ) = %g W/m2\n", net_rad);

			/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
			//fixme use correct net radiation
			transp_sun = Penman_Monteith (meteo_daily, rv, rh, net_rad);
			transp_sun *= daylength_sec * s->value[LAI_SUN_PROJ];
			logger(g_debug_log, "transp_sun = %g mm/m2/day\n", transp_sun);

			/************************************************************************************/
			/* calculate transpiration using adjusted day length */
			/* LAI SHADE */
			logger(g_debug_log, "\n--Transpiration for LAI shade--\n");

			rv = 1.0/gl_t_wv_shade;

			/* note: Net Rad is Short wave flux */
			//fixme why??????????
			net_rad = s->value[SW_RAD_ABS_SHADE] / (s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ]);
			logger(g_debug_log, "sw rad for evaporation (LAI shade) = %g W/m2\n", net_rad);

			/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
			transp_shade = Penman_Monteith (meteo_daily, rv, rh, net_rad);
			transp_shade *= daylength_sec * s->value[LAI_SHADE_PROJ];
			logger(g_debug_log, "transp_shade = %g mm/m2/day\n", transp_shade);

			/************************************************************************************/
			/* overall canopy */
			transp = transp_sun + transp_shade;

			/* check for negative values */
			if(transp < 0.0) transp = 0.0;

			/* assign values */
			s->value[CANOPY_TRANSP] = transp;

			/* considering effective coverage of cell and convert to daily amount */
			s->value[CANOPY_TRANSP] *= /*leaf_cell_cover_eff*/s->value[DAILY_CANOPY_COVER_EXP];
			s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
		}
	}
	else
	{
		s->value[CANOPY_INT] = 0.0;
		s->value[CANOPY_WATER] = 0.0;
		s->value[CANOPY_TRANSP] = 0.0;
		s->value[CANOPY_EVAPO] = 0.0;
		s->value[CANOPY_EVAPO_TRANSP] = 0.0;
	}

	logger(g_debug_log,"\n--Overall canopy--\n");
	logger(g_debug_log, "OLD CANOPY_WATER = %g mm/m2/day\n", s->value[OLD_CANOPY_WATER]);
	logger(g_debug_log, "CANOPY_WATER = %g mm/m2/day\n", s->value[CANOPY_WATER]);
	logger(g_debug_log, "CANOPY_EVAPO = %g mm/m2/day\n", s->value[CANOPY_EVAPO]);
	logger(g_debug_log, "CANOPY_TRANSP = %g mm/m2/day\n", s->value[CANOPY_TRANSP]);
	logger(g_debug_log, "CANOPY_EVAPO_TRANSP = %g mm/m2/day\n", s->value[CANOPY_EVAPO_TRANSP]);

	/* compute latent heat fluxes for canopy */
	Canopy_latent_heat_fluxes (s, meteo_daily);

	/* compute sensible heat fluxes for canopy */
	Canopy_sensible_heat_fluxes(c, layer, height, dbh, age, species, meteo_daily);

	s->value[MONTHLY_CANOPY_TRANSP] += s->value[CANOPY_TRANSP];
	s->value[MONTHLY_CANOPY_EVAPO_TRANSP] += s->value[CANOPY_EVAPO_TRANSP];
	s->value[YEARLY_CANOPY_TRANSP] += s->value[CANOPY_TRANSP];
	s->value[YEARLY_CANOPY_EVAPO_TRANSP] += s->value[CANOPY_EVAPO_TRANSP];

	c->daily_c_evapo += s->value[CANOPY_EVAPO];
	c->daily_c_transp += s->value[CANOPY_TRANSP];
	c->daily_c_water_stored += (s->value[CANOPY_WATER] - s->value[OLD_CANOPY_WATER]);
	c->daily_c_evapotransp += s->value[CANOPY_EVAPO_TRANSP];
}



