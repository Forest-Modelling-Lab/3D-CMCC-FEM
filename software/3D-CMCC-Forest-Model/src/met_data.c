/*met_data.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"
#include "constants.h"
#include "topo.h"

extern soil_t *g_soil;
extern topo_t *g_topo;


//BIOME-BGC version
//Running-Coughlan 1988, Ecological Modelling

void Day_Length ( CELL * c,  int day, int month, int years, YOS *yos)
{

	double ampl;  //seasonal variation in Day Length from 12 h
	static int doy;
	double adjust_latitude;

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	//compute yearday for GeDdayLength function
	if (day == 0 && month == JANUARY)
	{
		doy = 0;
	}
	doy +=1;

	//4/apr/2016
	//test following Schwalm & Ek 2004 instead of only geographical latitude adjusted latitude is used
	// for every 125m in altitude 1° in latitude is added
	adjust_latitude = g_topo->values[TOPO_ELEV] / 125.0;
	ampl = (exp (7.42 + (0.045 * (g_soil->lat+adjust_latitude)))) / 3600;
	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//Log("with altitude = %f\n", met[month].d[day].daylength);

	//	ampl = (exp (7.42 + (0.045 * g_soil->lat))) / 3600;
	//	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//	Log("without altitude = %f\n", met[month].d[day].daylength);

}

//following Running et al., 1987
void Avg_temperature (CELL * c, int day, int month, int years)
{
	/*
	if (!day )
			Log("computing Get_avg_temperature...\n");
	 */

	MET_DATA *met;
	// check parameters
	met = (MET_DATA*) c->years[years].m;

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

//following BIOME-BGC 4.2 src
//compute daylight average air temperature
extern void Daylight_avg_temperature (CELL * c, int day, int month, int years, YOS *yos)
{
	/*
	if (!day)
		Log("computing Get_daylight_avg_temperature...\n");
	 */

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

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

//following BIOME-BGC 4.2 src
//compute nightime average air temperature
extern void Nightime_avg_temperature (CELL * c,  int day, int month, int years, YOS *yos)
{
	/*
	if (!day)
		Log("computing Get_nightime_avg_temperature...\n");
	 */

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

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

extern void Thermic_sum (CELL * c, int day, int month, int years, YOS *yos)
{

	static double previous_thermic_sum;

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	if (day == 0 && month == 0)
	{
		met[month].d[day].thermic_sum = 0;

		if(met[month].d[day].tavg > GDD_BASIS)
		{
			met[month].d[day].thermic_sum = met[month].d[day].tavg - GDD_BASIS;
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
		if(met[month].d[day].tavg > GDD_BASIS)
		{
			met[month].d[day].thermic_sum = previous_thermic_sum + (met[month].d[day].tavg - GDD_BASIS);
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


void Air_pressure (CELL *c, int day, int month, int years, YOS *yos)
{
	double t1, t2;
	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	/*compute air pressure*/
	/* daily atmospheric pressure (Pa) as a function of elevation (m) */
	/* From the discussion on atmospheric statics in:
	Iribane, J.V., and W.L. Godson, 1981. Atmospheric Thermodynamics, 2nd
		Edition. D. Reidel Publishing Company, Dordrecht, The Netherlands.
		(p. 168)*/

	t1 = 1.0 - (LR_STD * g_topo->values[TOPO_ELEV])/T_STD;
	t2 = G_STD / (LR_STD * (Rgas / MA));
	met[month].d[day].air_pressure = P_STD * pow (t1, t2);
	//Log("Air pressure = %f Pa\n", met[month].d[day].air_pressure);
}


void Air_density (CELL * c, int day, int month, int years, YOS *yos)
{
	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	/*compute density of air (in kg/m3)*/

	//TODO CHECK DIFFERENCES IN A FIXED rhoair or in a computed rhoair
	/* temperature and pressure correction factor for conductances */
	//following Solantie R., 2004, Boreal Environmental Research, 9: 319-333, the model uses tday if available


	if(met[month].d[day].tday == NO_DATA)
	{
		met[month].d[day].rho_air = 1.292 - (0.00428 * met[month].d[day].tavg);
		c->gcorr = pow((met[month].d[day].tavg + TempAbs)/293.15, 1.75) * 101300.0/met[month].d[day].air_pressure;
		//Log("gcorr = %f\n", c->gcorr);
	}
	else
	{
		met[month].d[day].rho_air= 1.292 - (0.00428 * met[month].d[day].tday);
		c->gcorr = pow((met[month].d[day].tday + TempAbs)/293.15, 1.75) * 101300.0/met[month].d[day].air_pressure;
		//Log("gcorr = %f\n", c->gcorr);
	}
}

void Latent_heat (CELL *c, int day, int month, int years, YOS *yos)
{
	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	/*BIOME-BGC APPROACH*/
	/*compute latent heat of vaporization (J/Kg)*/
	met[month].d[day].lh_vap = 2.5023e6 - 2430.54 * met[month].d[day].tavg;
	met[month].d[day].lh_vap_soil = 2.5023e6 - 2430.54 * met[month].d[day].tsoil;
	/*latent heat of fusion (KJ/Kg)*/
	met[month].d[day].lh_fus = 335.0;
	/*latent heat of sublimation (KJ/Kg)*/
	met[month].d[day].lh_sub = 2845.0;
}

void Soil_temperature (CELL * c, int day, int month, int years, YOS *yos)
{
	double avg = 0;
	int i;
	int day_temp = day;
	int month_temp = month;
	int weight;
	const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

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
	//TODO CHECK SOIL TEMPÈRATURE CORRECTION FROM BIOME
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
						day_temp = days_per_month[month_temp] - 1;
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

void Annual_CO2_concentration (CELL *c, int day, int month, int years, YOS *yos)
{
	static double previous_co2_conc;

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	/* recompute co2 concentration at the beginning of each year */
	if( ! mystricmp(settings->CO2_fixed, "off") && day == 0 && month == 0)
	{
		/* assign first year value from site.txt */
		if(years == 0)
		{
			met[month].d[day].co2_conc = settings->co2Conc;
			previous_co2_conc = met[month].d[day].co2_conc;
		}
		else
		{
			/* then for other years increment each beginning of year */
			met[month].d[day].co2_conc = previous_co2_conc + (previous_co2_conc * settings->co2_incr);
			previous_co2_conc = met[month].d[day].co2_conc;
			Log("CO2 annual increment = %f ppmv\n", met[month].d[day].co2_conc * settings->co2_incr);
			Log("CO2 concentration  = %f ppmv\n", met[month].d[day].co2_conc);
		}
	}
}

void Annual_met_values (CELL * c, int day, int month, int years, YOS *yos)
{
	MET_DATA *met;
	// check parameters
	met = (MET_DATA*) yos[years].m;

	if(day == 0 && month == 0)
	{
		c->annual_tavg = 0.0;
		c->annual_tmax = 0.0;
		c->annual_tmin = 0.0;
		c->annual_tday = 0.0;
		c->annual_tnight = 0.0;
		c->annual_tsoil = 0.0;
		c->annual_solar_rad = 0.0;
		c->annual_precip = 0.0;
		c->annual_vpd = 0.0;
	}
	c->annual_tavg += met[month].d[day].tavg;
	c->annual_tmax += met[month].d[day].tmax;
	c->annual_tmin += met[month].d[day].tmin;
	c->annual_tday += met[month].d[day].tday;
	c->annual_tnight += met[month].d[day].tmin;
	c->annual_tsoil += met[month].d[day].tsoil;
	c->annual_solar_rad += met[month].d[day].solar_rad;
	c->annual_precip += met[month].d[day].prcp;
	c->annual_vpd += met[month].d[day].vpd;
	if(day == 30 && month == 11)
	{
		c->annual_tavg /= 365;
		c->annual_tmax /= 365;
		c->annual_tmin /= 365;
		c->annual_tday /= 365;
		c->annual_tnight /= 365;
		c->annual_tsoil /= 365;
		c->annual_solar_rad /= 365;
		//c->annual_precip = 365;
		c->annual_vpd /= 365;
		Log("**ANNUAL MET VALUES day = %d month = %d year = %d**\n", day+1, month+1, years+1);
		Log("-Annual average tavg = %f C°\n", c->annual_tavg);
		Log("-Annual average tmax = %f C°\n", c->annual_tmax);
		Log("-Annual average tmin = %f C°\n", c->annual_tmin);
		Log("-Annual average tday = %f C°\n", c->annual_tday);
		Log("-Annual average tnight = %f C°\n", c->annual_tnight);
		Log("-Annual average tsoil = %f C°\n", c->annual_tsoil);
		Log("-Annual average solar rad = %f MJ/m2/day\n", c->annual_solar_rad);
		Log("-Annual average prcp = %f mm/m2/day\n", c->annual_precip);
		Log("-Annual average vpd = %f hPa/day \n", c->annual_vpd);
	}

}



