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
extern void Get_avg_temperature (CELL * c,  int day, int month, int years, YOS *yos)
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
				//Log("tmax = %f, tmin = %f day = %d month = %d recomputed tavg = %f\n", met[month].d[day].tmax, met[month].d[day].tmin, day+1, month+1, met[month].d[day].tavg);
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
extern void Get_daylight_avg_temperature (CELL * c,  int day, int month, int years, YOS *yos)
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
extern void Get_nightime_avg_temperature (CELL * c,  int day, int month, int years, YOS *yos)
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

extern void Get_avg_monthly_temp (CELL * c, int day, int month, int years, int DaysInMonth, YOS *yos)
{
	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;
	static int counter;
	static double temp_avg_monthly_temp;

	//averaged monthly temp for RothC

	if (settings->time == 'd')
	{
		if (met[month].d[day].n_days == 1)
		{
			counter = 0;
			temp_avg_monthly_temp = 0;
			met[month].avg_monthly_temp = 0;
		}
		if (met[month].d[day].tavg != NO_DATA)
		{
			counter ++;
			temp_avg_monthly_temp += met[month].d[day].tavg;
		}
		if (met[month].d[day].n_days == DaysInMonth)
		{
			met[month].avg_monthly_temp = (temp_avg_monthly_temp / counter);
		}
	}
}

extern void Get_cum_monthly_rain (CELL * c, int day, int month, int years, int DaysInMonth, YOS *yos)
{
	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	//cumulated monthly rain for RothC

	if (settings->time == 'd')
	{
		if (met[month].d[day].n_days == 1)
		{
			met[month].cum_monthly_rain = 0;
		}
		if (met[month].d[day].rain != NO_DATA)
		{
			met[month].cum_monthly_rain += met[month].d[day].rain;
		}
	}
}

extern void Get_thermic_sum (CELL * c, int day, int month, int years, YOS *yos)
{

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	static double previous_thermic_sum;

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
			//Log ("day = %d month = %d somma termica %f\n",day+1, month+1,  met[month].d[day].thermic_sum);
		}
		else
		{
			met[month].d[day].thermic_sum = previous_thermic_sum;
			//Log ("day = %d month = %d somma termica %f\n",day+1, month+1,  met[month].d[day].thermic_sum);
		}
		if (met[month].d[day].tavg == NO_DATA)
			Log("tavg NO_DATA!!\n");
	}
}


extern void Get_air_pressure (CELL *c)
{
	double t1, t2;

	/*compute air pressure*/
	/* daily atmospheric pressure (Pa) as a function of elevation (m) */
	/* From the discussion on atmospheric statics in:
	Iribane, J.V., and W.L. Godson, 1981. Atmospheric Thermodynamics, 2nd
		Edition. D. Reidel Publishing Company, Dordrecht, The Netherlands.
		(p. 168)*/

	t1 = 1.0 - (LR_STD * site->elev)/T_STD;
	t2 = G_STD / (LR_STD * (R / MA));
	c->air_pressure = P_STD * pow (t1, t2);
	//Log("Air pressure = %f Pa\n", c->air_pressure);

}


extern void Get_rho_air (CELL * c, int day, int month, int years, YOS *yos)
{
	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	//TODO CHECK DIFFERENCES IN A FIXED rhoair or in a computed rhoair
	/* temperature and pressure correction factor for conductances */
	//following Solantie R., 2004, Boreal Environmental Research, 9: 319-333, the model uses tday if available


	if (settings->time == 'm')
	{
		if(met[month].tday == NO_DATA)
		{
			met[month].rho_air = 1.292 - (0.00428 * met[month].tavg);
			c->gcorr = pow((met[month].tavg + 273.15)/293.15, 1.75) * 101300.0/c->air_pressure;
			//Log("gcorr = %f\n", c->gcorr);
		}
		else
		{
			met[month].rho_air = 1.292 - (0.00428 * met[month].tday);
			c->gcorr = pow((met[month].tday + 273.15)/293.15, 1.75) * 101300.0/c->air_pressure;
			//Log("gcorr = %f\n", c->gcorr);
		}
		Log("RhoAir = %f\n", met[month].rho_air);
	}
	else
	{
		if(met[month].d[day].tday == NO_DATA)
		{
			met[month].d[day].rho_air = 1.292 - (0.00428 * met[month].d[day].tavg);
			c->gcorr = pow((met[month].d[day].tavg + 273.15)/293.15, 1.75) * 101300.0/c->air_pressure;
			//Log("gcorr = %f\n", c->gcorr);
		}
		else
		{
			met[month].d[day].rho_air= 1.292 - (0.00428 * met[month].d[day].tday);
			c->gcorr = pow((met[month].d[day].tday + 273.15)/293.15, 1.75) * 101300.0/c->air_pressure;
			//Log("gcorr = %f\n", c->gcorr);
		}
		//Log("RhoAir = %f\n", met[month].d[day].rho_air);
	}
}


void Get_snow_met_data (CELL *c, MET_DATA *met, int month, int day)
{

	//FOLLOWING BIOME APPROACH
	/* temperature and radiation snowmelt,
	from Joseph Coughlan PhD thesis, 1991 */

	static double snow_abs = 0.6; // absorptivity of snow
	static double t_coeff = 0.65; // (kg/m2/deg C/d) temp. snowmelt coeff
	double incident_rad;  //incident radiation (kJ/m2/d) incident radiation
	double melt, t_melt, r_melt, r_sub;


	Log("-GET SNOW MET DATA-\n");



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
	//Log("net_radiation for soil = %f\n", c->net_radiation_for_soil);
	//Log("incident radiation for soil = %f\n", incident_rad);

	if (settings->time == 'd')
	{
		/* temperature and radiation melt from snowpack */
		if (met[month].d[day].tavg > 0.0)
		{
			if (c->snow > 0.0)
			{
				Log("tavg = %f\n", met[month].d[day].tavg);
				Log("snow = %f\n", c->snow);
				Log("Snow melt!!\n");
				r_melt = incident_rad / LATENT_HEAT_FUSION;
				melt = t_melt + r_melt;


				if (melt > c->snow)
				{
					melt = c->snow;
					/*reset snow*/
					c->snow = 0;
				}
				//add snow to soil water
				/*check for balance*/
				c->snow_to_soil = melt;
				if (c->snow_to_soil < c->snow)
				{
					c->available_soil_water += c->snow_to_soil;
					c->snow -= c->snow_to_soil;
				}
				else
				{
					c->available_soil_water += c->snow;
					c->snow = 0.0;
				}
				Log("snow to soil = %f\n", c->snow_to_soil);
			}

		}
		/* sublimation from snowpack */
		else
		{
			Log("tavg = %f\n", met[month].d[day].tavg);
			Log("snow = %f\n", c->snow);
			Log("rain becomes snow\n");
			c->snow += met[month].d[day].rain;
			Log("Day %d month %d snow = %f (mm-kgH2O/m2)\n", day+1, month+1 , c->snow);
			met[month].d[day].rain = 0;
			r_sub = incident_rad / LATENT_HEAT_SUBLIMATION;
			//Log("r_sub = %f\n", r_sub);
			if (c->snow > 0.0)
			{
				/*snow sublimation*/
				if (r_sub > c->snow)
				{
					Log("Snow sublimation!!\n");
					r_sub = c->snow;
					c->snow_subl = r_sub;
					/*check for balance*/
					if (c->snow_subl < c->snow)
					{
						c->snow -= c->snow_subl;
					}
					else
					{
						c->snow_subl = c->snow;
						c->snow = 0.0;
					}
				}
				else
				{
					c->snow_subl = 0.0;
				}
			}
		}
	}
	else
	{
		/*no snow calculations in monthly time step*/
	}
	Log("*****************************************\n");
}

/*
double Get_vpd (const MET_DATA *const met, int month)
{
	double svp;
	double vp;
	double vpd;

	//Log("\n GET_VPD_ROUTINE \n");


	//compute vpd
	//see triplex model Peng et al., 2002
	svp = 6.1076 * exp ((17.269 * met[month].tavg) / (met[month].tavg + 237.3));
	//Log("svp = %f\n", svp);
	vp = met[month].rh * (svp /100);
	//Log("vp = %f\n", vp);
	vpd = svp -vp;
	//Log("vpd = %f \n", vpd);

	return vpd;
}
 */


void Print_met_data (const MET_DATA *const met, double vpd, int month, int day)
{
	//here is valid only into function
	static int doy;


	if (settings->time == 'm')
	{
		Log("***************\n");
		Log("**Monthly MET DATA**\n");
		Log("-average solar_rad = %f MJ/m^2/day\n"
				"-tavg = %f °C\n"
				"-tmax = %f °C\n"
				"-tmin = %f °C\n"
				"-tday = %f °C\n"
				"-tnight = %f °C\n"
				//"-rh = %f %%\n"
				"-vpd = %f mbar\n"
				"-ts_f = %f °C\n"
				"-rain = %f mm\n"
				"-swc = %f %vol\n"
				"-daylength = %f hrs\n",
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
			Log("-lai from NDVI = %f \n", met[month].ndvi_lai);
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
		Log("**Daily MET DATA day %d month %d**\n", met[month].d[day].n_days, month+1);
		Log("-average solar_rad = %.2f MJ/m^2/day\n"
				"-tavg = %.2f °C\n"
				"-tmax = %.2f °C\n"
				"-tmin = %.2f °C\n"
				"-tday = %.2f °C\n"
				"-tnight = %.2f °C\n"
				//"-rh = %f %%\n"
				"-vpd = %.2f mbar\n"
				"-ts_f = %.2f °C\n"
				"-rain = %.2f mm\n"
				"-swc = %.2f %vol\n"
				"-thermic_sum = %.2f °C\n"
				"-daylength = %.2f hrs\n"
				"-DOY = %d\n"
				"-tsoil = %.2f °C\n"
				"-month avg temp = %.2f °C\n"
				"-month cum rain = %.2f mm\n",
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
				met[month].d[day].daylength,
				doy,
				met[month].d[day].tsoil,
				met[month].avg_monthly_temp,
				met[month].cum_monthly_rain);

		if (settings->spatial == 's')
		{
			Log("-lai from NDVI = %f \n", met[month].d[day].ndvi_lai);
		}
	}

	Log("***************\n");

}

void Get_soil_temperature (CELL * c, int day, int month, int years, YOS *yos)
{
	double avg = 0;
	int i;
	int day_temp = day;
	int month_temp = month;
	int weight;
	const int days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	//Log("\n\nGET_SOIL_TEMPERATURE\n");

	/*following BIOME-bgc 4.1.2*/
	/* for this version, an 11-day running weighted average of daily
	average temperature is used as the soil temperature at 10 cm.
	For days 1-10, a 1-10 day running weighted average is used instead.
	The tail of the running average is weighted linearly from 1 to 11.
	There are no corrections for snowpack or vegetation cover.
	 */

	MET_DATA *met;
	// check parameters
	met = (MET_DATA*) yos[years].m;


	//FIXME model doesn't get for the first 10 days of the year the averaged values
	//TODO CHECK SOIL TEMPÈERATURE COORECTION FROM BIOME
	/* soil temperature correction using difference from
				annual average tair */
	/*file bgc.c

	 *
			tdiff = tair_avg - metv.tsoil;
			if (ws.snoww)
			{
				metv.tsoil += 0.83 * tdiff;
			}
			else
			{
				metv.tsoil += 0.2 * tdiff;
			}
	 */

	//compute soil temperature if no data are available from met_data files
	if (met[month].d[day].ts_f == -9999)
	{
		if (day < 11.0 && month == 0)
		{
			met[month_temp].d[day_temp].tsoil = met[month].d[day].tavg;
		}
		else
		{
			for (i=0; i <11; i++)
			{
				weight = 11-i;

				if (day > 10.0)
				{
					avg += (met[month_temp].d[day_temp].tavg * weight);
					day_temp--;
				}
				else
				{
					if(day_temp == 0)
					{
						avg += (met[month_temp].d[day_temp].tavg * weight);
						month_temp--;
						day_temp = days_per_month[month_temp] - 1.0;
					}
					else
					{
						avg += (met[month_temp].d[day_temp].tavg * weight);
						day_temp--;
					}
				}
			}
			avg = avg / 77;
			met[month].d[day].tsoil = avg;
		}
	}
	else
	{
		met[month].d[day].tsoil = met[month].d[day].ts_f;
	}
}




