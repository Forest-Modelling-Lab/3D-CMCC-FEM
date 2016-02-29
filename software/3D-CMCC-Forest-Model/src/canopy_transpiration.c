/*
 * canopy_transpiration2.c
 *
 *  Created on: 07/nov/2013
 *      Author: alessio
 */




/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_canopy_transpiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, double vpd, int height, int age, int species)
{
	static double defTerm;
	static double duv;                      // 'div' in 3pg
	static double PotEvap;
	double g_corr; //corrector factor from biome

	Log("\nGET_CANOPY_TRANSPIRATION_ROUTINE\n");

	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+273.15)/293.15, 1.75) * 101300/c->air_pressure;

	/*upscale stomatal maximum stomatal conductance to maximum canopy conductance*/
	s->value[MAXCOND] *= s->value[LAI] * g_corr;
	Log("Maximum Canopy Conductance =%f m/sec\n", s->value[MAXCOND]);

	/*Transpiration occurs only if the canopy is dry (see Lawrence et al., 2007)*/
	//Veg period
	if (s->counter[VEG_UNVEG] == 1 && s->value[RAIN_INTERCEPTED] == 0.0 )
	{
		/*Canopy Conductance*/
		if (settings->spatial == 's')
		{
			s->value[CANOPY_CONDUCTANCE] = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, met[month].d[day].ndvi_lai / s->value[LAIGCX]);
		}
		else
		{
			s->value[CANOPY_CONDUCTANCE] = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI] / s->value[LAIGCX]);
		}
		Log("Potential Canopy Conductance = %f m^2/sec\n", s->value[CANOPY_CONDUCTANCE]);
		Log("Potential Canopy Conductance = %f m^2/day\n", s->value[CANOPY_CONDUCTANCE]*met[month].d[day].daylength * 3600.0);

		/*Canopy Transpiration*/
		//todo change all functions with BIOME's or Gerten

		// Penman-Monteith equation for computing canopy transpiration
		// in kg/m2/day, which is converted to mm/day.
		// The following are constants in the PM formula (Landsberg & Gower, 1997)

		s->value[BLCOND] *= g_corr;

		defTerm = met[month].d[day].rho_air * c->lh_vap * (vpd * VPDCONV) * s->value[BLCOND];
		Log("defTerm = %f\n", defTerm);
		duv = (1.0 + E20 + s->value[BLCOND] / s->value[CANOPY_CONDUCTANCE]);
		//Log("duv = %f\n", duv);


		switch (c->daily_layer_number)
		{
		case 1:
			PotEvap = (E20 * s->value[NET_RAD_ABS] + defTerm) / duv;  // in J/m2/s
			break;
		case 2:
			if ( c->heights[height].z == c->top_layer )
			{
				PotEvap = (E20 * s->value[NET_RAD_ABS] + defTerm) / duv;  // in J/m2/s
			}
			else
			{
				PotEvap = (E20 * s->value[NET_RAD_ABS] + defTerm) / duv;  // in J/m2/s
			}
			break;
		case 3:
			if ( c->heights[height].z == c->top_layer )
			{
				PotEvap = (E20 * s->value[NET_RAD_ABS] + defTerm) / duv;  // in J/m2/s
			}
			if ( c->heights[height].z == c->top_layer - 1 )
			{
				PotEvap = (E20 * s->value[NET_RAD_ABS] + defTerm) / duv;  // in J/m2/s
			}
			else
			{
				PotEvap = (E20 * s->value[NET_RAD_ABS]+ defTerm) / duv;  // in J/m2/s
			}
			break;
		}
		Log("PotEvap = %f\n", PotEvap);

		/*compute transpiration*/
		/*dominant layer*/
		if (c->heights[height].z == c->top_layer)
		{
			s->value[DAILY_TRANSP] = (PotEvap / c->lh_vap * (met[month].d[day].daylength * 3600.0))	* s->value[CANOPY_COVER_DBHDC];
			Log("Canopy trasnpiration = %f mm/m2\n", s->value[DAILY_TRANSP]);
			c->daily_c_transp[c->top_layer] += s->value[DAILY_TRANSP];
			Log("Canopy transpiration from dominant layer = %f mm \n", c->daily_c_transp[c->top_layer]);
			/*last height dominant class processed*/
			if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
			{
				/*control*/
				if (c->available_soil_water < c->daily_c_transp[c->top_layer])
				{
					Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
					c->daily_c_transp[c->top_layer] = c->available_soil_water;
				}
			}
		}
		/*dominated*/
		else
		{
			/*dominated layer*/
			if (c->heights[height].z == c->top_layer-1)
			{
				s->value[DAILY_TRANSP] = (PotEvap / c->lh_vap * (met[month].d[day].daylength * 3600.0)) * s->value[CANOPY_COVER_DBHDC];
				Log("Canopy trasnpiration = %f mm\n", s->value[DAILY_TRANSP]);
				c->daily_c_transp[c->top_layer-1] += s->value[DAILY_TRANSP];
				Log("Canopy transpiration  water from dominated layer = %f mm \n", c->daily_c_transp[c->top_layer-1]);
				/*last height dominated class processed*/
				if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
				{
					/*control*/
					if (c->available_soil_water < c->daily_c_transp[c->top_layer-1])
					{
						Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
						c->daily_c_transp[c->top_layer-1] = c->available_soil_water;
					}
				}
			}
			/*subdominated layer*/
			else
			{
				s->value[DAILY_TRANSP] = (PotEvap / c->lh_vap * (met[month].d[day].daylength * 3600.0)) * s->value[CANOPY_COVER_DBHDC];
				Log("Canopy trasnpiration = %f mm\n", s->value[DAILY_TRANSP]);
				c->daily_c_transp[c->top_layer-2] += s->value[DAILY_TRANSP];
				Log("Canopy transpiration  water from dominated layer = %f mm \n", c->daily_c_transp[c->top_layer-2]);
				/*last height subdominated class processed*/
				if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
				{
					/*control*/
					if (c->available_soil_water < c->daily_c_transp[c->top_layer-2])
					{
						Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
						c->daily_c_transp[c->top_layer-2] = c->available_soil_water;
					}
				}
			}
		}
	}
	else
	{
		if(s->counter[VEG_UNVEG] == 1 && (s->value[FRAC_DAYTIME_WET_CANOPY] < 1.0 || s->value[FRAC_DAYTIME_WET_CANOPY] != 0.0))
		{
			s->value[DAILY_TRANSP] = (PotEvap / c->lh_vap * (met[month].d[day].daylength * 3600.0)) * s->value[CANOPY_COVER_DBHDC] * (1.0 - s->value[FRAC_DAYTIME_WET_CANOPY]);
			Log("Partial Canopy transpiration = %f mm\n", s->value[DAILY_TRANSP]);
			//fixme compute for all other layers
		}
		else
		{
			s->value[DAILY_TRANSP] = 0.0;
			Log("No Canopy transpiration = %f mm\n", s->value[DAILY_TRANSP]);
			c->daily_c_transp[c->heights[height].z] = 0.0;
			Log("Transpirated water from layer %d = %f mm \n", c->heights[height].z, c->daily_c_transp[c->heights[height].z]);
		}
	}


	/*compute total daily transpiration*/
	c->daily_tot_c_transp += c->daily_c_transp[c->heights[height].z];
	Log("Daily total canopy transpiration = %f \n", c->daily_tot_c_transp);

	/*compute energy balance transpiration from canopy*/
	c->daily_tot_c_transp_watt = c->daily_tot_c_transp * c->lh_vap /  (met[month].d[day].daylength * 3600);
	Log("Latent heat canopy transpiration = %f W/m^2\n", c->daily_tot_c_transp_watt);

}


















































































