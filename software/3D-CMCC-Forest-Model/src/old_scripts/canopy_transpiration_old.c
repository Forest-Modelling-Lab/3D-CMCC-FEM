///*
// * canopy_transpiration2.c
// *
// *  Created on: 07/nov/2013
// *      Author: alessio
// */
//
//
//
//
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void Canopy_transpiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int day, int height, int age, int species)
{
	static double defTerm;
	static double duv;                      // 'div' in 3pg
	static double PotEvap;
	double g_corr; //corrector factor from biome
	double maximum_c_conductance;
	double boundary_layer_conductance;

	/*following Priestley and Taylor,1972; Gerten et al., 2004*/
	/**********************************************************/

	logger(g_log, "\n**CANOPY_TRANSPIRATION_ROUTINE**\n");


	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+273.15)/293.15, 1.75) * 101300/met[month].d[day].air_pressure;

	/*upscale maximum stomatal conductance to maximum canopy conductance*/
	logger(g_log, "LAI %f\n", s->value[LAI]);
	logger(g_log, "MAXCOND = %f m/sec\n", s->value[MAXCOND]);
	maximum_c_conductance = s->value[MAXCOND] * s->value[LAI] * g_corr;
	logger(g_log, "maximum_c_conductance = %f m/sec\n", maximum_c_conductance);
	//unexpectd high values in same cases
	boundary_layer_conductance = s->value[BLCOND] * g_corr;
	logger(g_log, "BLCOND = %f m/sec\n", s->value[BLCOND]);
	logger(g_log, "boundary_layer_conductance = %f m/sec\n", boundary_layer_conductance);

	/*Transpiration occurs only if the canopy is dry (see Lawrence et al., 2007)*/
	//Veg period

	/*Canopy Conductance*/
	if (settings->spatial == 's')
	{
		s->value[CANOPY_CONDUCTANCE] = maximum_c_conductance * s->value[PHYS_MOD] * Minimum(1.0, met[month].d[day].ndvi_lai / s->value[LAIGCX]);
	}
	else
	{
		s->value[CANOPY_CONDUCTANCE] = maximum_c_conductance * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI] / s->value[LAIGCX]);
	}
	logger(g_log, "Potential Canopy Conductance = %f m^2/sec\n", s->value[CANOPY_CONDUCTANCE]);
	//logger(g_log, "Potential Canopy Conductance = %f m^2/day\n", s->value[CANOPY_CONDUCTANCE]*met[month].d[day].daylength * 3600.0);

	/*Canopy Transpiration*/
	//todo change all functions with BIOME's or Gerten

	// Penman-Monteith equation for computing canopy transpiration
	// in kg/m2/day, which is converted to mm/day.
	// The following are constants in the PM formula (Landsberg & Gower, 1997)
	logger(g_log, "rhoair = %f\n", met[month].d[day].rho_air);
	logger(g_log, "lh_vap = %f\n", met[month].d[day].lh_vap);
	logger(g_log, "vpd = %f\n", met[month].d[day].vpd);
	logger(g_log, "BLCOND = %f\n", s->value[BLCOND]);
	logger(g_log, "air_pressure = %f\n", met[month].d[day].air_pressure);

	defTerm = met[month].d[day].rho_air * met[month].d[day].lh_vap * (met[month].d[day].vpd * VPDCONV) * s->value[BLCOND];
	logger(g_log, "defTerm = %f\n", defTerm);
	duv = (1.0 + E20 + boundary_layer_conductance / s->value[CANOPY_CONDUCTANCE]);
	//logger(g_log, "duv = %f\n", duv);
	PotEvap = (E20 * s->value[NET_SW_RAD_ABS]+ defTerm) / duv; // in J/m2/s
	logger(g_log, "PotEvap = %f\n", PotEvap);

	/*compute transpiration*/
	if(met[month].d[day].tavg > s->value[GROWTHTMIN] && PotEvap > 0.0)
	{
		s->value[DAILY_TRANSP] = ((PotEvap / met[month].d[day].lh_vap * (met[month].d[day].daylength * 3600.0)) * s->value[CANOPY_COVER_DBHDC]) *
				s->value[FRAC_DAYTIME_TRANSP] * s->value[F_CO2];
		logger(g_log, "Canopy transpiration = %f mm/m2\n", s->value[DAILY_TRANSP]);
	}
	else
	{
		s->value[DAILY_TRANSP] = 0.0;
	}

	/*cumulate values of transpiration*/
	/*dominant layer*/
	if (c->heights[height].z == c->top_layer)
	{
		c->layer_daily_c_transp[c->top_layer] += s->value[DAILY_TRANSP];
		logger(g_log, "Canopy transpiration from dominant layer = %f mm \n", c->layer_daily_c_transp[c->top_layer]);
		/*last height dominant class processed*/
		if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
		{
			/*control*/
			if (c->asw < c->layer_daily_c_transp[c->top_layer])
			{
				logger(g_log, "ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
				c->layer_daily_c_transp[c->top_layer] = c->asw;
			}
		}
	}
	/*dominated*/
	else
	{
		/*dominated layer*/
		if (c->heights[height].z == c->top_layer-1)
		{
			logger(g_log, "Canopy transpiration  water from dominated layer = %f mm \n", c->layer_daily_c_transp[c->top_layer-1]);
			/*last height dominated class processed*/
			if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
			{
				/*control*/
				if (c->asw < c->layer_daily_c_transp[c->top_layer-1])
				{
					logger(g_log, "ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
					c->layer_daily_c_transp[c->top_layer-1] = c->asw;
				}
			}
		}
		/*subdominated layer*/
		else
		{
			c->layer_daily_c_transp[c->top_layer-2] += s->value[DAILY_TRANSP];
			logger(g_log, "Canopy transpiration  water from dominated layer = %f mm \n", c->layer_daily_c_transp[c->top_layer-2]);
			/*last height subdominated class processed*/
			if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
			{
				/*control*/
				if (c->asw < c->layer_daily_c_transp[c->top_layer-2])
				{
					logger(g_log, "ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
					c->layer_daily_c_transp[c->top_layer-2] = c->asw;
				}
			}
		}
	}
	/*compute total daily transpiration*/
	c->daily_c_transp += c->layer_daily_c_transp[c->heights[height].z];
	logger(g_log, "Daily total canopy transpiration = %f \n", c->daily_c_transp);

	/*compute energy balance transpiration from canopy*/
	c->daily_c_transp_watt = c->daily_c_transp * met[month].d[day].lh_vap / 86400.0;
	logger(g_log, "Latent heat canopy transpiration = %f W/m^2\n", c->daily_c_transp_watt);

}




















































































