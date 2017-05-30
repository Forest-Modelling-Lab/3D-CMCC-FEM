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

#define TEST 0

void canopy_evapotranspiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, meteo_daily_t *const meteo_daily)
{
	double g_corr;
	double gl_bl;
	double gl_x;                                                          /* maximum stomatal conductance */
	//double gl_s, gl_s_sun, gl_s_shade;
	double gl_c;
	double m_final, m_final_sun, m_final_shade;
	double gl_e_wv;
	//double gl_t_wv, gl_t_wv_sun, gl_t_wv_shade;
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
	static int days_with_canopy_wet;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	daylength_sec = meteo_daily->daylength * 3600.0;

	/* it mainly follows rationale and algorithms of BIOME-BGC v.4.2 */
	/* it computes canopy interception, evaporation and transpiration */

	/********************************************************************************************************/

	logger(g_debug_log, "\n**CANOPY EVAPO-TRANSPIRATION**\n");

	/********************************************************************************************************/

	/* call canopy interception */
	canopy_interception( c, layer, height, dbh, age, species, meteo_daily );

	/********************************************************************************************************/
	/* temperature and pressure correction factor for conductances */
	g_corr = pow( ( meteo_daily->tday + TempAbs ) / 293.15, 1.75) * 101300. / meteo_daily->air_pressure;
	logger(g_debug_log, "g_corr = %f \n",g_corr);

	/* calculate leaf- and canopy-level conductances to water vapor and
		sensible heat fluxes */

	/* leaf aerodynamic-boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;
	logger(g_debug_log, "gl_bl NOT corrected for wind speed = %f mm\n",gl_bl);

#if 0
	/**************************************************************************/
	/* aerodynamic boundary layer conductance corrected for wind speed */
	/* following Shi et al., 2008 Journal of Biophysical Research */
	if ( meteo_daily->windspeed != NO_DATA)
	{
		double zero = 20;    /* height measurement of wind speed (m) */ //fixme add to settings.txt data
		double rough;        /* roughness length */
		double plane;        /* zero place displacement */
		height_t *h;
		h = &c->heights[height];

		/* compute roughness length (m) */
		rough = 0.032 * h->value;

		/* compute zero plane displacement (m) */
		plane = 0.88 * h->value;

		/* aerodynamic boundary layer conductance */
		gl_bl = pow ( KARM , 2.) / pow ( log ( ( zero - plane) / rough ) , 2. );
		logger(g_debug_log, "gl_bl corrected for wind speed = %f mm\n",gl_bl);
		printf("wind speed = %f gl_bl corrected for wind speed %f\n", meteo_daily->windspeed, gl_bl);

		//gl_bl *= g_corr;
		//printf("wind speed = %f gl_bl corrected for wind speed and corrected for g_corr %f\n", meteo_daily->windspeed, gl_bl);

	}
	/**************************************************************************/
#endif

	/* canopy boundary layer conductance */
	s->value[CANOPY_BLCOND] = gl_bl * s->value[LAI_PROJ];
	logger(g_debug_log, "CANOPY_BLCOND = %f \n",s->value[CANOPY_BLCOND]);

	/* leaf cuticular conductance corrected for temperature and air pressure */
	gl_c = s->value[CUTCOND] * g_corr;
	logger(g_debug_log, "gl_c          = %f \n",gl_c);

	/* leaf stomatal conductance: first generate multipliers, then apply them to maximum stomatal conductance */
	/* apply all multipliers to the maximum stomatal conductance */
	/* differently from BIOME we use F_T that takes into account not only minimum temperature effects */
	/* differently from BIOME we use also F_AGE */

	/* original BIOME version (Jarvis method) */
	m_final       = s->value[F_LIGHT]       * s->value[F_SW] * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];
	m_final_sun   = s->value[F_LIGHT_SUN]   * s->value[F_SW] * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];
	m_final_shade = s->value[F_LIGHT_SHADE] * s->value[F_SW] * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];

	logger(g_debug_log, "m_final       = %f \n",m_final);
	logger(g_debug_log, "m_final_sun   = %f \n",m_final_sun);
	logger(g_debug_log, "m_final_shade = %f \n",m_final_shade);

	/* check */
	if (m_final       < eps) m_final       = eps;
	if (m_final_sun   < eps) m_final_sun   = eps;
	if (m_final_shade < eps) m_final_shade = eps;

	/** stomatal conductance **/
	/* correct maximum stomatal conductance for CO2 concentration*/
	gl_x = (s->value[F_CO2_TR] / 0.9116) * s->value[MAXCOND];
	logger(g_debug_log, "gl_x = %f\n",gl_x);

	/* following Jarvis 1997 approach (not more used) */
	//gl_s_sun = s->value[MAXCOND] * m_final_sun * g_corr;
	//gl_s_shade = s->value[MAXCOND] * m_final_shade * g_corr;

	/* following Jarvis 1997 + Frank et al., 2013 + Hidy et al., 2016 GMD */
	s->value[STOMATAL_CONDUCTANCE]       = gl_x * m_final       * g_corr;
	s->value[STOMATAL_SUN_CONDUCTANCE]   = gl_x * m_final_sun   * g_corr;
	s->value[STOMATAL_SHADE_CONDUCTANCE] = gl_x * m_final_shade * g_corr;
	logger(g_debug_log, "STOMATAL_CONDUCTANCE       = %f\n",s->value[STOMATAL_CONDUCTANCE]);
	logger(g_debug_log, "STOMATAL_SUN_CONDUCTANCE   = %f\n",s->value[STOMATAL_SUN_CONDUCTANCE]);
	logger(g_debug_log, "STOMATAL_SHADE_CONDUCTANCE = %f\n",s->value[STOMATAL_SHADE_CONDUCTANCE]);

	/* calculate leaf-and canopy-level conductances to water vapor and
		sensible heat fluxes, to be used in Penman-Monteith calculations of
		canopy evaporation and canopy transpiration */

	/* Leaf conductance to evaporated water vapor, per unit projected LAI */
	gl_e_wv       = gl_bl;
	logger(g_debug_log, "gl_e_wv = %f\n",gl_e_wv);

	/** leaf conductance **/
	/* Leaf conductance to transpired water vapor, per unit projected
		LAI.  This formula is derived from stomatal and cuticular conductances
		in parallel with each other, and both in series with leaf boundary
		layer conductance. */
	s->value[LEAF_CONDUCTANCE]       = (gl_bl * (s->value[STOMATAL_CONDUCTANCE]       + gl_c)) / (gl_bl + s->value[STOMATAL_CONDUCTANCE]       + gl_c);
	s->value[LEAF_SUN_CONDUCTANCE]   = (gl_bl * (s->value[STOMATAL_SUN_CONDUCTANCE]   + gl_c)) / (gl_bl + s->value[STOMATAL_SUN_CONDUCTANCE]   + gl_c);
	s->value[LEAF_SHADE_CONDUCTANCE] = (gl_bl * (s->value[STOMATAL_SHADE_CONDUCTANCE] + gl_c)) / (gl_bl + s->value[STOMATAL_SHADE_CONDUCTANCE] + gl_c);
	logger(g_debug_log, "LEAF_CONDUCTANCE       = %f\n",s->value[LEAF_CONDUCTANCE]);
	logger(g_debug_log, "LEAF_SUN_CONDUCTANCE   = %f\n",s->value[LEAF_SUN_CONDUCTANCE]);
	logger(g_debug_log, "LEAF_SHADE_CONDUCTANCE = %f\n",s->value[LEAF_SHADE_CONDUCTANCE]);

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh         = gl_bl;
	logger(g_debug_log, "gl_sh = %f\n",gl_sh);

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv       = gl_e_wv * s->value[LAI_PROJ];
	logger(g_debug_log, "gc_e_wv = %f\n",gc_e_wv);

	/* Canopy conductance to sensible heat */
	gc_sh         = gl_sh   * s->value[LAI_PROJ];
	logger(g_debug_log, "gc_sh = %f\n",gc_sh);

	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat. Calculate the time required to
	evaporate all the canopy water at the daily average conditions, and
	subtract that time from the daylength to get the effective daylength for
	transpiration. */

	/* note: as in Campbell and Norman, Environmental Biophysics,
	model should use Absorbed Radiation (instead just incident) but
	composed of Short and Long wave radiation */

	if( s->value[LAI_PROJ] > 0. )
	{
		/* if canopy has water */
		if(s->value[CANOPY_WATER] > 0.)
		{
			logger(g_debug_log, "\n*CANOPY EVAPORATION (Canopy Wet) *\n");
			logger(g_debug_log, "LAI_PROJ = %f\n",s->value[LAI_PROJ]);
			logger(g_debug_log, "CANOPY_WATER = %f mm\n",s->value[CANOPY_WATER]);

			rv = 1. / gc_e_wv;
			rh = 1. / gc_sh;

			/* radiation */
			net_rad = s->value[SW_RAD_ABS];
			logger(g_debug_log, "sw rad for evaporation (tot LAI) = %f W/m2\n", net_rad);

			/* call Penman-Monteith function, it returns Potential evaporation in kg/m2/s for evaporation and W/m2 for latent heat */
			//fixme use correct net radiation
			evapo = Penman_Monteith ( meteo_daily, rv, rh, net_rad );

			/* check for negative values */
			if( evapo < 0. ) evapo  = 0.;

			s->value[CANOPY_EVAPO] = evapo;

			/* calculate the time required to evaporate all the canopy water */
			evap_daylength_sec     = s->value[CANOPY_WATER] / s->value[CANOPY_EVAPO];
			logger(g_debug_log, "evap_daylength_sec = %f sec\n", evap_daylength_sec);

			/* day not long enough to evap. all int. water */
			if( evap_daylength_sec > daylength_sec )
			{
				logger(g_debug_log, "day not long enough to evap all rain intercepted\n");

				logger(g_debug_log, "\n*NO CANOPY TRANSPIRATION (Canopy completely Wet)!!!*\n");

				++days_with_canopy_wet;

				/* adjust day length for transpiration */
				transp_daylength_sec             = 0.;
				s->value[CANOPY_FRAC_DAY_TRANSP] = 0.;
				logger(g_debug_log, "transp_daylength_sec = %f\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				/* no time left for transpiration */
				s->value[CANOPY_TRANSP] = 0.;

				/* day length limits canopy evaporation */
				evapo *= daylength_sec;
				s->value[CANOPY_EVAPO] = evapo;
				logger(g_debug_log, "Canopy evaporation = %f mm\n", s->value[CANOPY_EVAPO]);

				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
				logger(g_debug_log, "Canopy evapo-transpiration = %f mm\n", s->value[CANOPY_EVAPO_TRANSP]);

				/* remove evaporated water from canopy water pool */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				logger(g_debug_log, "Canopy water = %f mm\n", s->value[CANOPY_WATER]);

			}
			/* all intercepted water evaporated */
			else
			{
				logger(g_debug_log, "all intercepted water evaporated\n");
				days_with_canopy_wet = 0;

				/* all canopy water evaporates */
				evapo = s->value[CANOPY_WATER];

				s->value[CANOPY_EVAPO]  = evapo;
				logger(g_debug_log, "Canopy evaporation = %f mm\n", s->value[CANOPY_EVAPO]);

				/* reset canopy water from canopy water pool */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];
				logger(g_debug_log, "Canopy water = %f mm\n", s->value[CANOPY_WATER]);

				logger(g_debug_log, "\n*CANOPY TRANSPIRATION (Partial Canopy Wet)*\n");

				/* adjust day length for transpiration */
				transp_daylength_sec   = daylength_sec - evap_daylength_sec;

				s->value[CANOPY_FRAC_DAY_TRANSP] = transp_daylength_sec / daylength_sec;
				logger(g_debug_log, "transp_daylength_sec = %f\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				/* Leaf-Canopy resistance to sensible heat */
				rh = 1. / gl_sh;

				/************************************************************************************/
				/* calculate transpiration using adjusted day length */
				/** LAI SUN **/
				logger(g_debug_log, "\n--Transpiration for LAI sun--\n");

				/* for sun canopy fraction */
				rv = 1. / s->value[LEAF_SUN_CONDUCTANCE];
				logger(g_debug_log, "Leaf sun resistance = %f \n",rv);

				/* note: Net Rad is Short wave flux */
				/* convert radiation to stomatal scale */
				//fixme why??????????

				net_rad = s->value[SW_RAD_ABS_SUN] / s->value[LAI_SUN_PROJ];
				logger(g_debug_log, "sw rad for evaporation (LAI sun ) = %f W/m2\n", net_rad);

				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat */
				//fixme use correct net radiation
				s->value[CANOPY_TRANSP_SUN]  = Penman_Monteith (meteo_daily, rv, rh, net_rad);
				s->value[CANOPY_TRANSP_SUN] *= ( transp_daylength_sec * s->value[LAI_SUN_PROJ] * s->value[DAILY_CANOPY_COVER_PROJ] );
				logger(g_debug_log, "transp_sun = %f mm/m2/day\n", s->value[CANOPY_TRANSP_SUN]);

				/** LAI SHADE **/
				logger(g_debug_log, "\n--Transpiration for LAI shade--\n");

				/* for shaded canopy fraction */
				rv = 1. / s->value[LEAF_SHADE_CONDUCTANCE];
				logger(g_debug_log, "Leaf shade resistance = %f \n",rv);

				/* note: Net Rad is Short wave flux */
				/* convert radiation to stomatal scale */
				//fixme why??????????
				net_rad = s->value[SW_RAD_ABS_SHADE] / s->value[LAI_SHADE_PROJ];
				logger(g_debug_log, "sw rad for evaporation (LAI shade) = %f W/m2\n", net_rad);

				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				//fixme use correct net radiation
				s->value[CANOPY_TRANSP_SHADE]  = Penman_Monteith (meteo_daily, rv, rh, net_rad);
				s->value[CANOPY_TRANSP_SHADE] *= ( transp_daylength_sec * s->value[LAI_SHADE_PROJ] * s->value[DAILY_CANOPY_COVER_PROJ] );
				logger(g_debug_log, "transp_shade = %f mm/m2/day\n", s->value[CANOPY_TRANSP_SHADE]);

				/************************************************************************************/
				/* overall canopy */
				s->value[CANOPY_TRANSP] = s->value[CANOPY_TRANSP_SUN] + s->value[CANOPY_TRANSP_SHADE];

				/* check for negative values */
				if( s->value[CANOPY_TRANSP] < 0. ) s->value[CANOPY_TRANSP]  = 0.;

				/* canopy evapotranspiration */
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
			}

			/* note special case for deciduous: assuming that all canopy water evaporates during last day of leaffall */
			if ( ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 ) &&
					( s->counter[DAYS_LEAFFALL] == s->counter[LEAF_FALL_COUNTER] ) )
			{
				logger(g_debug_log, "\nlast day of leaf fall !!!\n");
				s->value[CANOPY_EVAPO]       += s->value[CANOPY_WATER];
				s->value[CANOPY_WATER]        = 0.;
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
			}
		}
		/* if canopy has snow */
		else if (s->value[CANOPY_SNOW] > 0.)
		{
			//todo get functions from snow_melt_subl snow subl(evaporated) or melt (that goes to soil pool) for canopy intercepted snow
		}
		/** if canopy is dry **/
		else
		{
			/* no canopy evaporation occurs */
			evapo = 0.;

			s->value[CANOPY_EVAPO] = evapo;
			logger(g_debug_log, "Canopy evaporation = %f mm\n", s->value[CANOPY_EVAPO]);

			logger(g_debug_log, "*CANOPY TRANSPIRATION (Canopy Dry)*\n");

			/* all day transp */
			transp_daylength_sec             = daylength_sec;
			logger(g_debug_log, "transp_daylength_sec = %f\n",transp_daylength_sec);

			s->value[CANOPY_FRAC_DAY_TRANSP] = 1.;

			/* Leaf-Canopy resistance to sensible heat */
			rh = 1. / gl_sh;
			logger(g_debug_log, "rh = %f\n",rh);

			/************************************************************************************/
			/* calculate transpiration using adjusted day length */
			/* compute only transpiration */

			/** LAI SUN **/
			logger(g_debug_log, "\n--Transpiration for LAI sun--\n");

			rv = 1. / s->value[LEAF_SUN_CONDUCTANCE];
			logger(g_debug_log, "Leaf sun resistance = %f \n",rv);

			/* note: Net Rad is Short wave flux */
			/* convert radiation to stomatal scale */
			//fixme why??????????
			net_rad = s->value[SW_RAD_ABS_SUN] / s->value[LAI_SUN_PROJ];
			logger(g_debug_log, "sw rad for evaporation (LAI sun) = %f W/m2\n", net_rad);

			/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
			//fixme use correct net radiation
			s->value[CANOPY_TRANSP_SUN]  = Penman_Monteith (meteo_daily, rv, rh, net_rad);
			s->value[CANOPY_TRANSP_SUN] *= ( transp_daylength_sec * s->value[LAI_SUN_PROJ] * s->value[DAILY_CANOPY_COVER_PROJ] );
			logger(g_debug_log, "transp_sun = %f mm/m2/day\n", s->value[CANOPY_TRANSP_SUN]);

			/** LAI SHADE **/
			logger(g_debug_log, "\n--Transpiration for LAI shade--\n");

			rv = 1. / s->value[LEAF_SHADE_CONDUCTANCE];
			logger(g_debug_log, "Leaf shade resistance = %f \n",rv);

			/* note: Net Rad is Short wave flux */
			/* convert radiation to stomatal scale */
			//fixme why??????????
			net_rad = s->value[SW_RAD_ABS_SHADE] / s->value[LAI_SHADE_PROJ];
			logger(g_debug_log, "sw rad for evaporation (LAI shade) = %f W/m2\n", net_rad);

			/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
			s->value[CANOPY_TRANSP_SHADE]  = Penman_Monteith (meteo_daily, rv, rh, net_rad);
			s->value[CANOPY_TRANSP_SHADE] *= ( transp_daylength_sec * s->value[LAI_SHADE_PROJ] * s->value[DAILY_CANOPY_COVER_PROJ] );
			logger(g_debug_log, "transp_shade = %f mm/m2/day\n", s->value[CANOPY_TRANSP_SHADE]);

			/************************************************************************************/
			/* overall canopy */
			s->value[CANOPY_TRANSP] = s->value[CANOPY_TRANSP_SUN] + s->value[CANOPY_TRANSP_SHADE];

			/* check for negative values */
			if(s->value[CANOPY_TRANSP] < 0.) s->value[CANOPY_TRANSP] = 0.;

			/* canopy evapotranspiration */
			s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];
		}
	}
	else
	{
		s->value[CANOPY_INT]          = 0.;
		s->value[CANOPY_WATER]        = 0.;
		s->value[CANOPY_TRANSP]       = 0.;
		s->value[CANOPY_TRANSP_SHADE] = 0.;
		s->value[CANOPY_TRANSP_SHADE] = 0.;
		s->value[CANOPY_EVAPO]        = 0.;
		s->value[CANOPY_EVAPO_TRANSP] = 0.;
	}

	logger(g_debug_log,"\n--Overall canopy--\n");
	logger(g_debug_log, "CANOPY_WATER        = %f mm/m2/day\n", s->value[CANOPY_WATER]);
	logger(g_debug_log, "CANOPY_EVAPO        = %f mm/m2/day\n", s->value[CANOPY_EVAPO]);
	logger(g_debug_log, "CANOPY_TRANSP       = %f mm/m2/day\n", s->value[CANOPY_TRANSP]);
	logger(g_debug_log, "CANOPY_TRANSP_SUN   = %f mm/m2/day\n", s->value[CANOPY_TRANSP_SUN]);
	logger(g_debug_log, "CANOPY_TRANSP_SHADE = %f mm/m2/day\n", s->value[CANOPY_TRANSP_SHADE]);
	logger(g_debug_log, "CANOPY_EVAPO_TRANSP = %f mm/m2/day\n", s->value[CANOPY_EVAPO_TRANSP]);

	/**********************************************************************************/
	/**********************************************************************************/
	/* control if canopy transpiration exceeds available soil water */
	/* in case it happens firstly down-regulate transpiration to soil water modifier
	 * if it is not enough than superimpose to zero */
	if ( s->value[CANOPY_TRANSP] > c->asw )
	{
		printf("ATTENTION CANOPY TRANSPIRATION EXCEEDS ASW!!!!!!\n");

		s->value[CANOPY_TRANSP] *= s->value[F_SW];

		if ( s->value[CANOPY_TRANSP] > c->asw )
		{
			s->value[CANOPY_TRANSP] = 0.;
			printf("CANOPY_TRANSP = 0 !!!!!!\n");
		}
		/* check */
		CHECK_CONDITION ( s->value[CANOPY_TRANSP] , > , c->asw );
	}
	/**********************************************************************************/
	/**********************************************************************************/

	/* compute latent heat fluxes for canopy */
	Canopy_latent_heat_fluxes   (s, meteo_daily);

	/* compute sensible heat fluxes for canopy */
	Canopy_sensible_heat_fluxes (c, layer, height, dbh, age, species, meteo_daily);

	s->value[MONTHLY_CANOPY_TRANSP]       += s->value[CANOPY_TRANSP];
	s->value[MONTHLY_CANOPY_TRANSP_SUN]   += s->value[CANOPY_TRANSP_SUN];
	s->value[MONTHLY_CANOPY_TRANSP_SHADE] += s->value[CANOPY_TRANSP_SHADE];
	s->value[MONTHLY_CANOPY_EVAPO_TRANSP] += s->value[CANOPY_EVAPO_TRANSP];
	s->value[YEARLY_CANOPY_TRANSP]        += s->value[CANOPY_TRANSP];
	s->value[YEARLY_CANOPY_TRANSP_SUN]    += s->value[CANOPY_TRANSP_SUN];
	s->value[YEARLY_CANOPY_TRANSP_SHADE]  += s->value[CANOPY_TRANSP_SHADE];
	s->value[YEARLY_CANOPY_EVAPO_TRANSP]  += s->value[CANOPY_EVAPO_TRANSP];

	c->daily_canopy_evapo                 += s->value[CANOPY_EVAPO];
	c->daily_canopy_transp                += s->value[CANOPY_TRANSP];
	c->daily_canopy_et                    += s->value[CANOPY_EVAPO_TRANSP];

	/* update canopy water pool */
	c->canopy_water_stored                += ( s->value[CANOPY_INT] - s->value[CANOPY_EVAPO] );
}



