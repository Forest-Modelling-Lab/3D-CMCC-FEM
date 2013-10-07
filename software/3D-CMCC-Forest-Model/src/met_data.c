/*met_data.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"
#include "constants.h"

extern void Print_met_daily_data (const YOS *const yos, int day, int month, int years)
{
	MET_DATA *met;
	assert(yos);
	met = (MET_DATA*) yos[years].m;

	if (settings->time == 'd')
	{
		Log("n_days %10d "
				"Rg_f %10g "
				"Tavg %10g "
				"Tmax %10g "
				"Tmin %10g "
				"Precip %10g "
				"Tday %10g "
				"Tnight %10g \n",
				met[month].d[day].n_days,
				met[month].d[day].solar_rad,
				met[month].d[day].tavg,
				met[month].d[day].tmax,
				met[month].d[day].tmin,
				met[month].d[day].rain,
				met[month].d[day].tday,
				met[month].d[day].tnight);
	}

}


//following Running et al., 1987
extern void Get_avg_temperature (CELL * c,  int day, int month, int years, int MonthLength, YOS *yos)
{
	/*
	if (!day )
			Log("computing Get_avg_temperature...\n");
			*/

	MET_DATA *met;
	// check parameters
	met = (MET_DATA*) yos[years].m;


	if (settings->time == 'd')
	{
		if ( met[month].d[day].tavg == NO_DATA)
		{
			if (met[month].d[day].tmax == NO_DATA && met[month].d[day].tmin == NO_DATA)
			{
				Log("NO DATA FOR TEMPERATURE!!!!!!!!!!!!!!!!!!");
			}
			else
			{
				met[month].d[day].tavg =  (0.606 * met[month].d[day].tmax) + (0.394 * met[month].d[day].tmin);
				//Log("tmax = %g, tmin = %g day = %d month = %d recomputed tavg = %g\n", met[month].d[day].tmax, met[month].d[day].tmin, day+1, month+1, met[month].d[day].tavg);
			}
		}
	}
	else
	{
		if ( met[month].tavg == NO_DATA)
		{
			if (met[month].tmax == NO_DATA && met[month].tmin == NO_DATA)
			{
				Log("NO DATA FOR TEMPERATURE!!!!!!!!!!!!!!!!!!");
			}
			{
				met[month].tavg = (0.606 * met[month].tmax) + (0.394 * met[month].tmin);
			}
		}
	}


}

//following BIOME-BGC 4.2 src
//compute daylight average air temperature
extern void Get_daylight_avg_temperature (CELL * c,  int day, int month, int years, int MonthLength, YOS  *yos)
{
	/*
	if (!day)
		Log("computing Get_daylight_avg_temperature...\n");
		*/

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	if (settings->time == 'd')
	{
		if (met[month].d[day].tmax != NO_DATA && met[month].d[day].tmin != NO_DATA)
		{
			met[month].d[day].tday = 0.45 * (met[month].d[day].tmax - met[month].d[day].tavg) + met[month].d[day].tavg;
		}
		else
		{
			met[month].d[day].tday = NO_DATA;
			Log("NO TMAX and TMIN can't compute TDAY!!! \n");
		}
	}
	else
	{
		if (met[month].tmax != NO_DATA && met[month].tmin != NO_DATA)
		{
			met[month].tday = 0.45 * (met[month].tmax - met[month].tavg) + met[month].tavg;
		}
		else
		{
			met[month].tday = NO_DATA;
			Log("NO TMAX and TMIN can't compute TDAY!!! \n");
		}
	}
}

//following BIOME-BGC 4.2 src
//compute nightime average air temperature
extern void Get_nightime_avg_temperature (CELL * c,  int day, int month, int years, int MonthLength, YOS  *yos)
{
	/*
	if (!day)
		Log("computing Get_nightime_avg_temperature...\n");
		*/

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	if (settings->time == 'd')
	{
		if (met[month].d[day].tday != NO_DATA )
		{
			met[month].d[day].tnight = (met[month].d[day].tday + met[month].d[day].tmin)/2 ;
		}
		else
		{
			met[month].d[day].tnight = NO_DATA;
			Log("NO TMAX and TMIN can't compute TNIGHT!!! \n");
		}
	}
	else
	{
		if (met[month].tday != NO_DATA )
		{
			met[month].tnight = (met[month].tday + met[month].tmin)/2 ;
		}
		else
		{
			met[month].tnight = NO_DATA;
			Log("NO TMAX and TMIN can't compute TNIGHT!!! \n");
		}
	}
}


extern void Get_thermic_sum (CELL * c, int day, int month, int years, int MonthLength, YOS *yos)
{

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	static float previous_thermic_sum;

	if (day == 0 && month == 0)
	{
		met[month].d[day].thermic_sum = 0;

		if(met[month].d[day].tavg > settings->gdd_basis)
		{
			met[month].d[day].thermic_sum = met[month].d[day].tavg - settings->gdd_basis;
			previous_thermic_sum = met[month].d[day].thermic_sum;
		}
		else
		{
			met[month].d[day].thermic_sum = 0;
			previous_thermic_sum = 0;
		}
		if (met[month].d[day].tavg == NO_DATA)
		Log("tavg NO_DATA!!\n");
	}
	else
	{
		if(met[month].d[day].tavg > settings->gdd_basis)
		{
			met[month].d[day].thermic_sum = previous_thermic_sum + (met[month].d[day].tavg -settings->gdd_basis);
			previous_thermic_sum = met[month].d[day].thermic_sum;
		}
		else
		{
			met[month].d[day].thermic_sum = previous_thermic_sum;
		}
		if (met[month].d[day].tavg == NO_DATA)
		Log("tavg NO_DATA!!\n");
	}
}


void Get_snow_met_data (CELL *c, const MET_DATA *const met, int month, int day)
{

	//FOLLOWING BIOME APPROACH
	/* temperature and radiation snowmelt,
	from Joseph Coughlan PhD thesis, 1991 */

	static float snow_abs = 0.6; // absorptivity of snow
	static float t_coeff = 0.65; // (kg/m2/deg C/d) temp. snowmelt coeff
	float incident_rad;  //incident radiation (kJ/m2/d) incident radiation
	float melt, t_melt, r_melt, r_sub;


	t_melt = r_melt = r_sub = 0;
	if(settings->time == 'm')
	{
		t_melt = t_coeff * met[month].tavg;
	}
	else
	{
		t_melt = t_coeff * met[month].d[day].tavg;
	}

	/* canopy transmitted radiation: convert from W/m2 --> KJ/m2/d */
	if (settings->time == 'd')
	{
		incident_rad = c->net_radiation_for_soil * snow_abs * 0.001;
	}
	else
	{
		/*no snow calculations in monthly time step*/
	}
	Log("net_radiation for soil = %g\n", c->net_radiation_for_soil);
	Log("incident radiation for soil = %g\n", incident_rad);

	if (settings->time == 'd')
	{
		if (c->snow != 0)
		{
			/* temperature and radiation melt from snowpack */
			if (met[month].d[day].tavg > 0)
			{
				r_melt = incident_rad / LATENT_HEAT_FUSION;
				melt = t_melt + r_melt;

				if (melt > c->snow)
				{
					melt = c->snow;
					//add snow to soil water
					c->available_soil_water += c->snow;
					/*reset snow*/
					c->snow = 0;
				}
			}
			/* sublimation from snowpack */
			else
			{
				r_sub = incident_rad / LATENT_HEAT_SUBLIMATION;
				Log("r_sub = %g\n", r_sub);
				/*snow sublimation*/
				if (r_sub > c->snow)
				{
					r_sub = c->snow;
					c->snow_subl = r_sub;
					/*reset*/
					c->snow = 0;
				}
			}
		}
	}
	else
	{
		/*no snow calculations in monthly time step*/
	}
}

/*
float Get_vpd (const MET_DATA *const met, int month)
{
	float svp;
	float vp;
	float vpd;

	//Log("\n GET_VPD_ROUTINE \n");


	//compute vpd
	//see triplex model Peng et al., 2002
	svp = 6.1076 * exp ((17.269 * met[month].tavg) / (met[month].tavg + 237.3));
	//Log("svp = %g\n", svp);
	vp = met[month].rh * (svp /100);
	//Log("vp = %g\n", vp);
	vpd = svp -vp;
	//Log("vpd = %g \n", vpd);

	return vpd;
}
 */

extern void Get_air_pressure (CELL *c)
{
	float t1, t2;

	/*compute air pressure*/
	/* daily atmospheric pressure (Pa) as a function of elevation (m) */
	/* From the discussion on atmospheric statics in:
	Iribane, J.V., and W.L. Godson, 1981. Atmospheric Thermodynamics, 2nd
		Edition. D. Reidel Publishing Company, Dordrecht, The Netherlands.
		(p. 168)*/

	t1 = 1.0 - (LR_STD * site->elev)/T_STD;
	t2 = G_STD / (LR_STD * (R / MA));
	c->air_pressure = P_STD * pow (t1, t2);
	//Log("Air pressure = %g Pa\n", c->air_pressure);

}

void Print_met_data (const MET_DATA *const met, float vpd, int month, int day)
{
	//here is valid only into function
	static int doy;


	if (settings->time == 'm')
	{
		Log("***************\n");
		Log("**Monthly MET DATA**\n");
		Log("-average solar_rad = %g MJ/m^2/day\n"
				"-tavg = %g °C\n"
				"-tmax = %g °C\n"
				"-tmin = %g °C\n"
				"-tday = %g °C\n"
				"-tnight = %g °C\n"
				//"-rh = %g %%\n"
				"-vpd = %g mbar\n"
				"-ts_f = %g °C\n"
				"-rain = %g mm\n"
				"-swc = %g %vol\n"
				"-daylength = %g hrs\n",
				met[month].solar_rad,
				met[month].tavg,
				met[month].tmax,
				met[month].tmin,
				met[month].tday,
				met[month].tnight,
				//met[month].rh,
				vpd,
				met[month].ts_f,
				met[month].rain,
				met[month].swc,
				met[month].daylength);

		if (settings->spatial == 's')
		{
			Log("-lai from NDVI = %g \n", met[month].ndvi_lai);
		}
	}
	else
	{
		if (day == 0 && month == 0)
		{
			doy = 0;
		}
		doy += 1;

		Log("***************\n");
		Log("**Daily MET DATA day %d month %d**\n", day+1, month+1);
		Log("-average solar_rad = %g MJ/m^2/day\n"
				"-tavg = %g °C\n"
				"-tmax = %g °C\n"
				"-tmin = %g °C\n"
				"-tday = %g °C\n"
				"-tnight = %g °C\n"
				//"-rh = %g %%\n"
				"-vpd = %g mbar\n"
				"-ts_f = %g °C\n"
				"-rain = %g mm\n"
				"-swc = %g %vol\n"
				"-thermic_sum = %g °C\n"
				"-day %d month %d daylength = %g hrs\n"
				"-DOY = %d\n",
				met[month].d[day].solar_rad,
				met[month].d[day].tavg,
				met[month].d[day].tmax,
				met[month].d[day].tmin,
				met[month].d[day].tday,
				met[month].d[day].tnight,
				//met[month].rh,
				vpd,
				met[month].d[day].ts_f,
				met[month].d[day].rain,
				met[month].d[day].swc,
				met[month].d[day].thermic_sum,
				day +1,
				month +1,
				met[month].d[day].daylength,
				doy);

		if (settings->spatial == 's')
		{
			Log("-lai from NDVI = %g \n", met[month].d[day].ndvi_lai);
		}
	}

	Log("***************\n");

}

void Get_soil_temperature (CELL * c,  int day, int month, int years, int MonthLength, YOS *yos)
{
	static float avg;
	static int i;
	float peso[10];
	/*
	for (i=0; i<10; i++)
	{
		if
	}
	*/



}




