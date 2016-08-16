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
#include "heat_fluxes.h"

extern logger_t* g_log;

void canopy_interception(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	static int cell_height_class_counter;
	static int layer_height_class_counter;

	static double temp_int_rain;
	static double temp_int_snow;

	double leaf_cell_cover_eff;

	tree_layer_t *l;
	species_t *s;

	l = &c->t_layers[layer];
	s = &c->heights[height].ages[age].species[species];

	/* it computes canopy rain interception with the same rationale
	 * used for canopy radiation absorption */

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if( s->value[LAI] < 1.0 ) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if( leaf_cell_cover_eff > 1.0 ) leaf_cell_cover_eff = 1.0;

	logger(g_log, "\n**CANOPY INTERCEPTION**\n");

	/* first height class in the cell is processed */
	if( !layer_height_class_counter && !cell_height_class_counter )
	{
		/* reset temporary values when the first height class in layer is processed */
		temp_int_rain = 0.0;
		temp_int_snow = 0.0;

		/* assign meteo variables to cell variables */
		/* assign incoming rain */
		c->rain = meteo_daily->rain;

		/* assign incoming snow */
		c->snow = meteo_daily->snow;
	}

	/*****************************************************************************************************************/
	/* counters */
	layer_height_class_counter ++;
	cell_height_class_counter++;

	/*************************************************************************/
	/* shared functions among all class/layers */
	/* compute interception for dry canopy (Lawrence et al., 2006) */
	if( meteo_daily->prcp > 0.0 && s->value[LAI] > 0.0 && s->value[CANOPY_WATER] == 0.0 )
	{
		//double Int_max_snow;                           /*maximum intercepted snow (mm)*/

		/* following Dewire (PhD thesis) and Pomeroy et al., 1998., Hedstrom & Pomeroy, 1998 */
		//Int_max_snow = 4.4 * s->value[LAI];

		/* for rain */
		//ALESSIOC FIXME for multilayer each layer reduce rain interceptable
		if( meteo_daily->rain != 0.0 )
		{
			s->value[CANOPY_INT] = s->value[INT_COEFF] * meteo_daily->rain * (1.0 - exp(-0.5 * s->value[LAI])) * leaf_cell_cover_eff;
			logger(g_log, "CANOPY_INT = %g mm/m2/day\n", s->value[CANOPY_INT]);
			s->value[CANOPY_WATER] = s->value[CANOPY_INT];
			CHECK_CONDITION(s->value[CANOPY_INT], > meteo_daily->rain);
		}
		/* for snow */
		else
		{
			/* see Dewire PhD thesis */
			//s->value[CANOPY_INT_SNOW] = s->value[CANOPY_SNOW] + 0.7 * (Int_max_snow - s->value[CANOPY_SNOW]) *
			//	(1 - exp(-(c->prcp_snow/Int_max_snow))) * leaf_cell_cover_eff;
			logger(g_log, "CANOPY_INT_SNOW = %g mm/m2/day\n", s->value[CANOPY_INT_SNOW]);

			//fixme for now assuming no snow interception
			s->value[CANOPY_INT_SNOW] = 0.0;
		}
	}

	/**********************************************************************************************************/
	/* update temporary rain and snow */
	temp_int_rain += s->value[CANOPY_INT];
	temp_int_snow += s->value[CANOPY_INT_SNOW];

	/**********************************************************************************************************/

	/* when matches the last height class in the layer is processed */
	if ( l->height_class == layer_height_class_counter )
	{
		logger(g_log,"\nlast height class in layer processed\n");
		logger(g_log,"update rain/snow interception values for lower layer\n");

		/* compute interceptable rain for lower layers */
		c->rain -= temp_int_rain;

		/* compute interceptable snow for lower layers */
		c->snow -= temp_int_snow;

		/* reset temporary values when the last height class in layer is processed */
		temp_int_rain = 0.0;
		temp_int_snow = 0.0;

		/* reset counter */
		layer_height_class_counter = 0;
	}

	/**********************************************************************************************************/

	/* when matches the last height class in the cell is processed */
	if ( c->heights_count == cell_height_class_counter )
	{
		logger(g_log,"last height class in cell processed\n");

		logger(g_log,"\n***********************************\n");

		/* assign values for soil */
		c->rain_to_soil = c->rain;
		c->snow_to_soil = c->snow;
		logger(g_log, "rain to soil = %g mm\n", c->rain_to_soil);
		logger(g_log, "snow to soil = %g cm\n", c->snow_to_soil);

		/* reset counter */
		cell_height_class_counter = 0;
	}

	/* update at cell level */
	c->daily_c_rain_int += s->value[CANOPY_INT];
	c->daily_c_snow_int += s->value[CANOPY_INT_SNOW];

}

void canopy_evapotranspiration(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
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
	double rh;                                                             /* Leaf-Canopy resistance to sensible heat */
	double daylength_sec;                                                  /* daylength in sec */
	double evap_daylength_sec;
	double transp_daylength_sec;
	double evapo;
	//double evapo, evapo_sun, evapo_shade;
	double transp, transp_sun, transp_shade;

	double leaf_cell_cover_eff;                                            /* fraction of square meter covered by leaf over the grid cell */
	static int days_with_canopy_wet;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	daylength_sec = meteo_daily->daylength * 3600.0;

	/* it mainly follows rationale and algorithms of BIOME-BGC v.4.2 */
	/* it computes canopy interception, evaporation and transpiration */

	/* assign values of previous day canopy water/snow */
	/* reset if LAI == 0.0*/
	if( !s->value[LAI] )
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

	/*********************************************************************************************************/
	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if( s->value[LAI] < 1.0 ) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if( leaf_cell_cover_eff > 1.0 ) leaf_cell_cover_eff = 1.0;
	//logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/********************************************************************************************************/

	/* call canopy interception */
	canopy_interception( c, layer, height, age, species, meteo_daily );

	/********************************************************************************************************/

	logger(g_log, "\n**CANOPY EVAPO-TRANSPIRATION**\n");

	/* temperature and pressure correction factor for conductances */
	g_corr = pow((meteo_daily->tday+TempAbs)/293.15, 1.75) * 101300/meteo_daily->air_pressure;

	/* calculate leaf- and canopy-level conductances to water vapor and
		sensible heat fluxes */

	/* leaf boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;

	/* canopy boundary layer conductance */
	s->value[CANOPY_BLCOND] = gl_bl * s->value[LAI];

	/* upscaled to coverage **/
	s->value[CANOPY_BLCOND] *= leaf_cell_cover_eff;

	/* leaf cuticular conductance corrected for temperature and air pressure */
	gl_c = s->value[CUTCOND] * g_corr;

	/* leaf stomatal conductance: first generate multipliers, then apply them to maximum stomatal conductance */
	/* apply all multipliers to the maximum stomatal conductance */
	/* differently from BIOME we use F_T that takes into account not only minimum temperature effects */
	/* differently from BIOME we use also F_AGE */
	m_final_sun = s->value[F_LIGHT_SUN] * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];
	m_final_shade = s->value[F_LIGHT_SHADE] * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD] * s->value[F_AGE];

	if (m_final_sun < 0.00000001) m_final_sun = 0.00000001;
	if (m_final_shade < 0.00000001) m_final_shade = 0.00000001;

	/* folllowing Jarvis 1997 approach */
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

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI];

	/* Canopy conductance to sensible heat */
	gc_sh = gl_sh * s->value[LAI];

	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat.  Calculate the time required to
	evaporate all the canopy water at the daily average conditions, and
	subtract that time from the daylength to get the effective daylength for
	transpiration. */

	/* note: as in Campbell and Norman, Environmental Biophysiscs,
	model should use Absorbed Radiation (instead just incident) but
	composed of Short and Long wave radiation */

	if(s->value[LAI]>0.0)
	{
		//fixme why for evaporation BIOME uses stomatal conductance??
		/* if canopy has water */
		if(s->value[CANOPY_WATER] > 0.0)
		{
			logger(g_log, "\n*CANOPY EVAPORATION (Canopy Wet) *\n");
			logger(g_log, "LAI = %g\n",s->value[LAI]);
			logger(g_log, "CANOPY_WATER = %g mm\n",s->value[CANOPY_WATER]);

			rv = 1.0/gc_e_wv;
			rh = 1.0/gc_sh;
			net_rad = s->value[SW_RAD_ABS];

			/* call Penman-Monteith function, it returns Potential evaporation in kg/m2/s for evaporation and W/m2 for latent heat*/
			//fixme use correct net radiation
			evapo = Penman_Monteith (meteo_daily, rv, rh, net_rad);

			/* check for negative values */
			if(evapo < 0.0) evapo = 0.0;

			s->value[CANOPY_EVAPO] = evapo;

			/* calculate the time required to evaporate all the canopy water */
			evap_daylength_sec = s->value[CANOPY_WATER] / s->value[CANOPY_EVAPO];
			logger(g_log, "evap_daylength_sec = %g sec\n", evap_daylength_sec);

			/* day not long enough to evap. all int. water */
			if(evap_daylength_sec > daylength_sec)
			{
				logger(g_log, "day not long enough to evap all rain intercepted\n");

				logger(g_log, "\n*NO CANOPY TRANSPIRATION (Canopy completely Wet)!!!*\n");

				++days_with_canopy_wet;

				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength_sec = 0.0;
				s->value[CANOPY_FRAC_DAY_TRANSP] = 0.0;
				logger(g_log, "transp_daylength_sec = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				s->value[CANOPY_TRANSP] = 0.0;                                  /* no time left for transpiration */

				s->value[CANOPY_EVAPO] *= daylength_sec;                        /* day length limits canopy evaporation */

				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];

				/* remove evaporated water from canopy */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];

				/* check if canopy is wet for too long period */
				//CHECK_CONDITION(days_with_canopy_wet, > 10);
			}
			/* all intercepted water evaporated */
			else
			{
				logger(g_log, "all intercepted water evaporated\n");
				days_with_canopy_wet = 0;

				s->value[CANOPY_EVAPO] = s->value[CANOPY_WATER];                 /* all canopy water evaporates */
				s->value[CANOPY_WATER] -= s->value[CANOPY_EVAPO];

				logger(g_log, "\n*CANOPY TRANSPIRATION (Partial Canopy Wet)*\n");

				/* adjust daylength for transpiration */
				//fixme this variable should be used also in photosynthesis
				transp_daylength_sec = daylength_sec - evap_daylength_sec;

				s->value[CANOPY_FRAC_DAY_TRANSP] = transp_daylength_sec / daylength_sec;
				logger(g_log, "transp_daylength_sec = %g\n", s->value[CANOPY_FRAC_DAY_TRANSP]);

				/* calculate transpiration using adjusted day length */
				rv = 1.0/gl_t_wv_sun;
				rh = 1.0/gl_sh;

				/* note: Net Rad is Short wave flux */
				net_rad = s->value[SW_RAD_ABS_SUN];
				logger(g_log, "net rad = %g\n", net_rad);

				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat */
				//fixme use correct net radiation
				transp_sun = Penman_Monteith (meteo_daily, rv, rh, net_rad);
				transp_sun *= transp_daylength_sec * s->value[LAI_SUN];
				logger(g_log, "transp_sun = %g mm/m2/day\n", transp_sun);

				/* next for shaded canopy fraction */
				rv = 1.0/gl_t_wv_shade;
				rh = 1.0/gl_sh;

				/* note: Net Rad is Short wave flux */
				net_rad = s->value[SW_RAD_ABS_SHADE];

				/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
				//fixme use correct net radiation
				transp_shade = Penman_Monteith (meteo_daily, rv, rh, net_rad);
				transp_shade *= transp_daylength_sec * s->value[LAI_SHADE];
				logger(g_log, "transp_shade = %g mm/m2/day\n", transp_shade);

				transp = transp_sun + transp_shade;
				logger(g_log, "transp = %g mm/m2/day\n", transp);

				/* check for negative values */
				if(transp < 0.0) transp = 0.0;

				s->value[CANOPY_TRANSP] = transp;

				/* considering effective coverage of cell */
				s->value[CANOPY_TRANSP] *= leaf_cell_cover_eff;
				s->value[CANOPY_EVAPO_TRANSP] = s->value[CANOPY_EVAPO] + s->value[CANOPY_TRANSP];

			}
		}
		/* if canopy has snow */
		else if (s->value[CANOPY_SNOW] > 0.0)
		{
			//todo get functions from snow_melt_subl snow subl(evaporated) or melt (that goes to soil pool) for canopy intercepted snow
		}
		/* if canopy is dry */
		else
		{
			/* no canopy evaporation occurs */
			s->value[CANOPY_EVAPO] = 0.0;

			logger(g_log, "*CANOPY TRANSPIRATION (Canopy Dry)*\n");

			/* all day transp */
			transp_daylength_sec = 1.0;
			s->value[CANOPY_FRAC_DAY_TRANSP] = 1.0;

			/* compute only transpiration */
			/* first for sunlit canopy fraction */
			logger(g_log,"transpiration for sun leaves\n");
			rv = 1.0/gl_t_wv_sun;
			rh = 1.0/gl_sh;

			/* note: Net Rad is Short wave flux */
			net_rad = s->value[SW_RAD_ABS_SUN];
			logger(g_log, "sw rad for sun leaves = %g\n", net_rad);

			/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
			//fixme use correct net radiation
			transp_sun = Penman_Monteith (meteo_daily, rv, rh, net_rad);
			transp_sun *= daylength_sec * s->value[LAI_SUN];
			logger(g_log, "transp_sun = %g mm/m2/day\n", transp_sun);

			/* next for shaded canopy fraction */
			logger(g_log,"transpiration for shaded leaves\n");
			rv = 1.0/gl_t_wv_shade;
			rh = 1.0/gl_sh;

			/* note: Net Rad is Short wave flux */
			net_rad = s->value[SW_RAD_ABS_SHADE];
			logger(g_log, "sw rad for shaded leaves = %g\n", net_rad);

			/* call Penman-Monteith function, returns e in kg/m2/s for transpiration and W/m2 for latent heat*/
			transp_shade = Penman_Monteith (meteo_daily, rv, rh, net_rad);
			transp_shade *= daylength_sec * s->value[LAI_SHADE];
			logger(g_log, "transp_shade = %g mm/m2/day\n", transp_shade);

			transp = transp_sun + transp_shade;
			logger(g_log, "transp (sun+shade) = %g mm/m2/day\n", transp);

			/* check for negative values */
			if(transp < 0.0) transp = 0.0;

			/* assign values */
			s->value[CANOPY_TRANSP] = transp;

			/* considering effective coverage of cell and convert to daily amount */
			s->value[CANOPY_TRANSP] *= leaf_cell_cover_eff;
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

	logger(g_log, "OLD CANOPY_WATER = %g mm/m2/day\n", s->value[OLD_CANOPY_WATER]);
	logger(g_log, "CANOPY_WATER = %g mm/m2/day\n", s->value[CANOPY_WATER]);
	logger(g_log, "CANOPY_EVAPO = %g mm/m2/day\n", s->value[CANOPY_EVAPO]);
	logger(g_log, "CANOPY_TRANSP = %g mm/m2/day\n", s->value[CANOPY_TRANSP]);
	logger(g_log, "CANOPY_EVAPO_TRANSP = %g mm/m2/day\n", s->value[CANOPY_EVAPO_TRANSP]);

	/* compute latent heat fluxes for canopy */
	Canopy_latent_heat_fluxes (s, meteo_daily);

	/* compute sensible heat fluxes for canopy */
	Canopy_sensible_heat_fluxes(c, layer, height, age, species, meteo_daily);

	c->daily_c_evapo += s->value[CANOPY_EVAPO];
	c->daily_c_transp += s->value[CANOPY_TRANSP];
	c->daily_c_water_stored += (s->value[CANOPY_WATER] - s->value[OLD_CANOPY_WATER]);
	c->daily_c_evapotransp += s->value[CANOPY_EVAPO_TRANSP];
}



