/*met_data.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"
#include "constants.h"
#include "soil.h"
#include "topo.h"
#include "logger.h"

/* externs */
extern logger_t* g_log;
extern soil_t *g_soil;
extern topo_t *g_topo;


void Solar_Radiation (CELL * c, int day, int month, int years, YOS *yos)
{
	double a = 107.0;                                                                    //(W/m)  empirical constants for long wave radiation computation
	double b = 0.2;                                                                      //(unit less) empirical constants for long wave radiation computation
	double ni;                                                                           //proportion of dayl ength
	double lat_decimal;
	double lat_degrees;
	double lat_rad;
	double atmospheric_transmissivity;

	double dr;                                                                           //inverse relative distance Earth-Sun
	double sigma;                                                                        //solar declination (radians)
	double omega_s;                                                                      //sunset hour angle
	int days_of_year;
	double TmaxK, TminK;

	double e0max;                                                                        //saturation vapour pressure at the maximum air temperature (KPa)
	double e0min;                                                                        //saturation vapour pressure at the minimum air temperature (KPa)
	double es;                                                                           //mean saturation vapour pressure at the air temperature (KPa)
	double ea;                                                                           //actual vapour pressure derived from relative humidity data (KPa)

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	TmaxK = met[month].d[day].tmax + TempAbs;
	TminK = met[month].d[day].tmin + TempAbs;

	/*proportion of day length*/
	ni = met[month].d[day].daylength/24.0;


	logger(g_log, "\nRADIATION ROUTINE\n");

	/* following Allen et al., 1998 */
	/* compute saturation vapour pressure at the maximum and minimum air temperature (KPa) */
	e0max = 0.61076 * exp((17.27*met[month].d[day].tmax)/(met[month].d[day].tmax+237.3));
	e0min = 0.61076 * exp((17.27*met[month].d[day].tmin)/(met[month].d[day].tmin+237.3));

	/* compute mean saturation vapour pressure at the air temperature (KPa)*/
	es = (e0max + e0min)/2.0;

	/* compute actual vapour pressure derived from relative humidity data (KPa) */
	ea = (met[month].d[day].rh_f/100.0)*es;
	CHECK_CONDITION(ea, < 0.0);

	if(IS_LEAP_YEAR(years))
	{
		days_of_year = 366;
	}
	else
	{
		days_of_year = 365;
	}

	/* Following Allen et al., 1998 */
	/* convert latitude in radians */
	lat_decimal = g_soil->values[SOIL_LAT] - (int)g_soil->values[SOIL_LAT];
	lat_degrees = (int)g_soil->values[SOIL_LAT] + (lat_decimal/60.0);
	lat_rad = (Pi/180.0)*lat_degrees;
	//logger(g_log, "lat_rad = %f\n", lat_rad);

	/* compute inverse relative distance Earth-Sun */
	dr = 1.0 + 0.033 * cos(((2*Pi)/days_of_year)*c->doy);
	//logger(g_log, "dr = %f\n", dr);

	/* compute solar declination */
	sigma = 0.409 * sin((((2*Pi)/days_of_year)*c->doy)-1.39);
	//logger(g_log, "sigma = %f\n",sigma);

	/* compute sunset hour angle */
	omega_s = acos((-tan(lat_rad) * tan(sigma)));
	//logger(g_log, "omega_s = %f\n", omega_s);

	/* compute atmospheric transmissivity */
	atmospheric_transmissivity = (0.75 + 2e-5 * g_topo->values[TOPO_ELEV]);

	//fixme cos(omega) should takes into account slope and aspect (once they will be included in "topo" files)
	//following Allen et al., 2006 Agric and Forest Meteorology (parag. 2)

	/* compute extra terrestrial radiation (MJ/m^2/day) */
	c->extra_terr_radiation_MJ = ((24.0*60.0)/Pi) * Q0_MJ * dr * ((omega_s * sin(lat_rad)* sin(sigma))+(cos(lat_rad)*cos(sigma)*sin(omega_s)));
	logger(g_log, "extra terrestrial radiation = %f (MJ/m^2/day)\n", c->extra_terr_radiation_MJ);

	/* convert into W/m2 */
	c->extra_terr_radiation_W = c->extra_terr_radiation_MJ * MJ_TO_W;
	logger(g_log, "extra terrestrial radiation = %f (W/m2)\n", c->extra_terr_radiation_W);


	/*****************************************************************************************/
	/* SHORT WAVE RADIATION */
	logger(g_log, "\nSHORT WAVE RADIATION\n");

	/* INCOMING SHORT WAVE RADIATION */
	logger(g_log, "\(incoming short wave)\n");

	/* compute short wave clear sky radiation (Tasumi et al., 2000)*/
	c->short_wave_clear_sky_radiation_MJ = atmospheric_transmissivity * c->extra_terr_radiation_MJ;
	logger(g_log, "Short wave clear_sky_radiation = %f (MJ/m^2/day)\n", c->short_wave_clear_sky_radiation_MJ);

	/* convert into W/m2 */
	c->short_wave_clear_sky_radiation_W = c->short_wave_clear_sky_radiation_MJ * MJ_TO_W;
	logger(g_log, "Short wave clear_sky_radiation = %f (W/m2)\n", c->short_wave_clear_sky_radiation_W);

	/* from input met data */
	//logger(g_log, "Solar_rad = %f MJ/m^2 day\n", met[month].d[day].solar_rad);

	c->short_wave_radiation_DW_MJ = met[month].d[day].solar_rad;
	logger(g_log, "Short_wave_radiation_DW_MJ (downward) = %f MJ/m^2 day\n", c->short_wave_radiation_DW_MJ);

	/* convert into W/m2 */
	c->short_wave_radiation_DW_W = c->short_wave_radiation_DW_MJ * MJ_TO_W;
	logger(g_log, "Short wave radiation (downward) = %f W/m2\n", c->short_wave_radiation_DW_W);

	/* cloud cover fraction */
	c->cloud_cover_frac = 1.0 -(c->short_wave_radiation_DW_MJ/c->short_wave_clear_sky_radiation_MJ);
	logger(g_log, "cloud_cover_frac = %f %%\n", c->cloud_cover_frac * 100.0);


	/*****************************************************************************************/
	/* LONG WAVE RADIATION */
	logger(g_log, "\nLONG WAVE RADIATION\n");

	/* INCOMING LONG WAVE RADIATION */
	logger(g_log, "\(incoming long wave)\n");

	/* following Costa dos Santos et al., */


	/* OUTGOING LONG WAVE RADIATION */
	logger(g_log, "\(outgoing long wave)\n");

	/* following Allen et al., 1998 */
	/* Upward long wave radiation (MJ/m2/day) based on Allen et al., 1998 */
	c->long_wave_radiation_UW_MJ = SBC_MJ * (((pow(TmaxK, 4)) + (pow(TminK,4)))/2.0)*(0.34-0.14*(sqrt(ea)))*(1.35*(c->short_wave_radiation_DW_MJ/c->short_wave_clear_sky_radiation_MJ)-0.35);
	logger(g_log, "Long wave radiation (upward) (Allen)= %f MJ/m^2 day\n", c->long_wave_radiation_UW_MJ);

	/* convert into W/m2 */
	c->long_wave_radiation_UW_W = c->long_wave_radiation_UW_MJ * MJ_TO_W;
	logger(g_log, "Long wave radiation (upward) (Allen)= %f W/m2\n", c->long_wave_radiation_UW_W);

	/* following Prentice et al., 1993 */
	/* Upward long wave radiation based on Monteith, 1973; Prentice et al., 1993; Linacre, 1986 */
	c->long_wave_radiation_UW_W = (b+(1.0-b)*ni)*(a - met[month].d[day].tavg);
	logger(g_log, "Long wave radiation (upward) (Prentice)= %f W/m2\n", c->long_wave_radiation_UW_W);

	/* convert into W/m2 */
	c->long_wave_radiation_UW_MJ = c->long_wave_radiation_UW_W * W_TO_MJ;
	logger(g_log, "Long wave radiation (upward) (Prentice)= %f MJ/m^2 day\n", c->long_wave_radiation_UW_MJ);

	//getchar();


}


//BIOME-BGC version
//Running-Coughlan 1988, Ecological Modelling

void Day_Length (CELL * c, int day, int month, int years, YOS *yos)
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
	ampl = (exp (7.42 + (0.045 * (g_soil->values[SOIL_LAT]+adjust_latitude)))) / 3600;
	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//logger(g_log, "with altitude = %f\n", met[month].d[day].daylength);

	//	ampl = (exp (7.42 + (0.045 * g_soil->values[SOIL_lat))) / 3600;
	//	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//	logger(g_log, "without altitude = %f\n", met[month].d[day].daylength);

}

//following Running et al., 1987
void Avg_temperature (CELL * c, int day, int month, int years)
{
	/*
	if (!day )
			logger(g_log, "computing Get_avg_temperature...\n");
	 */

	MET_DATA *met;
	// check parameters
	met = (MET_DATA*) c->years[years].m;

	if ( met[month].d[day].tavg == NO_DATA)
	{
		if (met[month].d[day].tmax == NO_DATA && met[month].d[day].tmin == NO_DATA)
		{
			logger(g_log, "NO DATA FOR TEMPERATURE!!!!!!!!!!!!!!!!!!");
		}
		else
		{
			met[month].d[day].tavg =  (0.606 * met[month].d[day].tmax) + (0.394 * met[month].d[day].tmin);
			//logger(g_log, "tmax = %f, tmin = %f day = %d month = %d recomputed tavg = %f\n", met[month].d[day].tmax, met[month].d[day].tmin, day+1, month+1, met[month].d[day].tavg);
		}
	}

}

//following BIOME-BGC 4.2 src
//compute daylight average air temperature
extern void Daylight_avg_temperature (CELL * c, int day, int month, int years, YOS *yos)
{
	/*
	if (!day)
		logger(g_log, "computing Get_daylight_avg_temperature...\n");
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
		logger(g_log, "NO TMAX and TMIN can't compute TDAY!!! \n");
	}
}

//following BIOME-BGC 4.2 src
//compute nightime average air temperature
extern void Nightime_avg_temperature (CELL * c,  int day, int month, int years, YOS *yos)
{
	/*
	if (!day)
		logger(g_log, "computing Get_nightime_avg_temperature...\n");
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
		logger(g_log, "NO TMAX and TMIN can't compute TNIGHT!!! \n");
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
			logger(g_log, "tavg NO_DATA!!\n");
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
			logger(g_log, "tavg NO_DATA!!\n");
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
	//logger(g_log, "Air pressure = %f Pa\n", met[month].d[day].air_pressure);
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
		//logger(g_log, "gcorr = %f\n", c->gcorr);
	}
	else
	{
		met[month].d[day].rho_air= 1.292 - (0.00428 * met[month].d[day].tday);
		c->gcorr = pow((met[month].d[day].tday + TempAbs)/293.15, 1.75) * 101300.0/met[month].d[day].air_pressure;
		//logger(g_log, "gcorr = %f\n", c->gcorr);
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

	//logger(g_log, "\n\nGET_SOIL_TEMPERATURE\n");

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
	if( ! string_compare_i(settings->CO2_fixed, "off") && day == 0 && month == 0)
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
			logger(g_log, "CO2 annual increment = %f ppmv\n", met[month].d[day].co2_conc * settings->co2_incr);
			logger(g_log, "CO2 concentration  = %f ppmv\n", met[month].d[day].co2_conc);
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
		logger(g_log, "**ANNUAL MET VALUES day = %d month = %d year = %d**\n", day+1, month+1, years+1);
		logger(g_log, "-Annual average tavg = %f C°\n", c->annual_tavg);
		logger(g_log, "-Annual average tmax = %f C°\n", c->annual_tmax);
		logger(g_log, "-Annual average tmin = %f C°\n", c->annual_tmin);
		logger(g_log, "-Annual average tday = %f C°\n", c->annual_tday);
		logger(g_log, "-Annual average tnight = %f C°\n", c->annual_tnight);
		logger(g_log, "-Annual average tsoil = %f C°\n", c->annual_tsoil);
		logger(g_log, "-Annual average solar rad = %f MJ/m2/day\n", c->annual_solar_rad);
		logger(g_log, "-Annual average prcp = %f mm/m2/day\n", c->annual_precip);
		logger(g_log, "-Annual average vpd = %f hPa/day \n", c->annual_vpd);
	}

}



