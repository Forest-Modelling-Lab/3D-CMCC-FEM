/* met_data.c */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "math.h"
#include "constants.h"
#include "soil_settings.h"
#include "topo.h"
#include "settings.h"
#include "logger.h"
#include "matrix.h"
#include "common.h"

extern settings_t* g_settings;
extern soil_settings_t *g_soil_settings;
extern topo_t *g_topo;
extern logger_t* g_log;



void Sat_vapour_pressure(cell_t *const c, const int day, const int month, const int year)
{
	double e0max;                                                                        //saturation vapour pressure at the maximum air temperature (KPa)
	double e0min;                                                                        //saturation vapour pressure at the minimum air temperature (KPa)

	meteo_t *met;
	met = (meteo_t*) c->years[year].m;

	/* following Allen et al., 1998 */
	/* compute saturation vapour pressure at the maximum and minimum air temperature (KPa) */
	e0max = 0.61076 * exp((17.27*met[month].d[day].tmax)/(met[month].d[day].tmax+237.3));
	e0min = 0.61076 * exp((17.27*met[month].d[day].tmin)/(met[month].d[day].tmin+237.3));

	/* compute weighted mean saturation vapour pressure at the air temperature (KPa)*/
	met[month].d[day].es = ((e0max*c->ni) + (e0min*(1.0-c->ni)));
	//logger(g_log, "es = %f\n", c->es);

	/* compute actual vapour pressure derived from relative humidity data (KPa) */
	met[month].d[day].ea = (met[month].d[day].rh_f/100.0)*met[month].d[day].es;
	//CHECK_CONDITION(met[month].d[day].ea, < 0.0);
	//fixme
	if (met[month].d[day].ea < 0.0)
	{
		met[month].d[day].ea = 0.0;
	}

}

void Radiation (cell_t *const c, const int day, const int month, const int year)
{
	double a = 107.0;                                                                    //(W/m)  empirical constants for long wave radiation computation
	double b = 0.2;                                                                      //(unit less) empirical constants for long wave radiation computation
	double lat_decimal;
	double lat_degrees;
	double lat_rad;
	double atmospheric_transmissivity;
	double atmospheric_emissivity;                                                       //emissivity of the clear-sky atmosphere

	double dr;                                                                           //inverse relative distance Earth-Sun
	double sigma;                                                                        //solar declination (radians)
	double omega_s;                                                                      //sunset hour angle
	int days_of_year;
	double TmaxK, TminK;

	//logger(g_log, "\nRADIATION ROUTINE\n");

	meteo_t *met;
	met = (meteo_t*) c->years[year].m;

	TmaxK = met[month].d[day].tmax + TempAbs;
	TminK = met[month].d[day].tmin + TempAbs;

	if(IS_LEAP_YEAR(year))
	{
		days_of_year = 366;
	}
	else
	{
		days_of_year = 365;
	}

	/* Following Allen et al., 1998 */
	/* convert latitude in radians */
	lat_decimal = g_soil_settings->values[SOIL_LAT] - (int)g_soil_settings->values[SOIL_LAT];
	lat_degrees = (int)g_soil_settings->values[SOIL_LAT] + (lat_decimal/60.0);
	lat_rad = (Pi/180.0)*lat_degrees;
	//logger(g_log, "lat_rad = %g\n", lat_rad);

	/* compute inverse relative distance Earth-Sun */
	dr = 1.0 + 0.033 * cos(((2*Pi)/days_of_year)*c->doy);
	//logger(g_log, "dr = %g\n", dr);

	/* compute solar declination */
	sigma = 0.409 * sin((((2*Pi)/days_of_year)*c->doy)-1.39);
	//logger(g_log, "sigma = %g\n",sigma);

	/* compute sunset hour angle */
	omega_s = acos((-tan(lat_rad) * tan(sigma)));
	//logger(g_log, "omega_s = %g\n", omega_s);

	/* compute atmospheric transmissivity */
	atmospheric_transmissivity = (0.75 + 2e-5 * g_topo->values[TOPO_ELEV]);
	//logger(g_log, "atmospheric_transmissivity = %g\n", atmospheric_transmissivity);

	/* compute emissivity of the clear-sky atmosphere see Sun et al., 2013; Campbell and Normal 1998; Brutsaert, 1984; from Gao et al., 2008 instead 1.72 uses 1.24*/
	//fixme it should takes into account cloud cover
	atmospheric_emissivity = (1.72 * pow ((met[month].d[day].ea*10)/(met[month].d[day].tavg+TempAbs), 1.0/7.0));
	//logger(g_log, "atmospheric_emissivity = %g\n", atmospheric_emissivity);

	//fixme cos(omega) should takes into account slope and aspect (once they will be included in "topo" files)
	//following Allen et al., 2006 Agric and Forest Meteorology (parag. 2)

	/* compute extra terrestrial radiation (MJ/m^2/day) */
	met[month].d[day].extra_terr_rad_MJ = ((24.0*60.0)/Pi) * Q0_MJ * dr * ((omega_s * sin(lat_rad)* sin(sigma))+(cos(lat_rad)*cos(sigma)*sin(omega_s)));
	//logger(g_log, "extra terrestrial radiation = %g (MJ/m^2/day)\n", met[month].d[day].extra_terr_rad_MJ);

	/* convert into W/m2 */
	met[month].d[day].extra_terr_rad_W = met[month].d[day].extra_terr_rad_MJ * MJ_TO_W;
	//logger(g_log, "extra terrestrial radiation = %g (W/m2)\n", met[month].d[day].extra_terr_rad_W);

	/***************************************************************************************************************************************/

	/* SHORT WAVE RADIATION */
	//logger(g_log, "\nSHORT WAVE RADIATION\n");

	/* INCOMING SHORT WAVE RADIATION */
	//logger(g_log, "\n(incoming short wave)\n");

	/* compute short wave clear sky radiation (Tasumi et al., 2000)*/
	met[month].d[day].sw_clear_sky_MJ = atmospheric_transmissivity * met[month].d[day].extra_terr_rad_MJ;
	//logger(g_log, "Short wave clear_sky_radiation = %g (MJ/m^2/day)\n", met[month].d[day].sw_clear_sky_MJ);

	/* convert into W/m2 */
	met[month].d[day].sw_clear_sky_W = met[month].d[day].sw_clear_sky_MJ * MJ_TO_W;
	//logger(g_log, "Short wave clear_sky_radiation = %g (W/m2)\n", met[month].d[day].sw_clear_sky_W);

	/* compute downward short wave radiation*/
	met[month].d[day].sw_downward_MJ = met[month].d[day].solar_rad;
	//logger(g_log, "Short_wave_radiation (downward) = %g MJ/m^2 day\n", met[month].d[day].sw_downward_MJ);

	/* convert into W/m2 */
	met[month].d[day].sw_downward_W = met[month].d[day].sw_downward_MJ * MJ_TO_W;
	//logger(g_log, "Short wave radiation (downward) = %g W/m2\n", met[month].d[day].sw_downward_W);

	/* convert incoming Short-Wave flux in PAR from MJ/m2/day to molPAR/m2/day (Biome-BGC method)*/
	met[month].d[day].par = (met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR);
	//logger(g_log, "Par = %g molPAR/m^2 day\n", met[month].d[day].par);

	/* convert incoming Short-Wave flux in PPFD from W/m2 to umol/m2/sec (Biome-BGC method)*/
	met[month].d[day].ppfd = met[month].d[day].sw_downward_W * RAD2PAR * EPAR;
	//logger(g_log, "PPFD = %g umolPPFD/m2/sec\n", met[month].d[day].ppfd);

	/* cloud cover fraction from Allen et al., 1998 */
	//note: Allen says that cloud_cover_frac must be li mited to 1.0
	met[month].d[day].cloud_cover_frac = (1.35*(met[month].d[day].sw_downward_MJ/met[month].d[day].sw_clear_sky_MJ)-0.35);
	if(met[month].d[day].cloud_cover_frac > 1.0) met[month].d[day].cloud_cover_frac = 1.0;
	//logger(g_log, "cloud_cover_frac = %g %%\n", met[month].d[day].cloud_cover_frac * 100.0);
	/***************************************************************************************************************************************/

	/* LONG WAVE RADIATION */
	//logger(g_log, "\nLONG WAVE RADIATION\n");
	//logger(g_log, "ea = %g\n", met[month].d[day].ea);

	/* NET LONG WAVE RADIATION */
	//logger(g_log, "\n(net long wave)\n");

	//fixme to avoid crash in model for negative "ea" values use different calculation of long_wave_radiation following Prentice (IT HAS TO BE SOLVED ANYWAY)
	if(met[month].d[day].ea < 0.0)
	{
		/* following Allen et al., 1998 */
		/* Upward long wave radiation (MJ/m2/day) */
		met[month].d[day].lw_net_MJ = SBC_MJ * (((pow(TmaxK, 4)) + (pow(TminK,4)))/2.0)*(0.34-0.14*(sqrt(met[month].d[day].ea)))*met[month].d[day].cloud_cover_frac;
		//logger(g_log, "Net Long wave radiation (Allen)= %g MJ/m^2 day\n", met[month].d[day].lw_net_MJ);

		/* convert into W/m2 */
		met[month].d[day].lw_net_W = met[month].d[day].lw_net_MJ * MJ_TO_W;
		//logger(g_log, "Net Long wave radiation (Allen)= %g W/m2\n", met[month].d[day].lw_net_W);
		/*****************************************************************************************/
	}
	else
	{
		//todo check it Prentice says "net upward long-wave flux"
		/* following Prentice et al., 1993 */
		/* Upward long wave radiation based on Monteith, 1973; Prentice et al., 1993; Linacre, 1986 */
		met[month].d[day].lw_net_W = (b+(1.0-b)*c->ni)*(a - met[month].d[day].tavg);
		//logger(g_log, "Net Long wave radiation (Prentice)= %g W/m2\n", met[month].d[day].lw_net_W);

		/* convert into MJ/m^2 day */
		met[month].d[day].lw_net_MJ = met[month].d[day].lw_net_W * W_TO_MJ;
		//logger(g_log, "Net Long wave radiation (Prentice)= %g MJ/m^2 day\n", met[month].d[day].lw_net_MJ);
		/*****************************************************************************************/
	}

	/* net radiation based on 3-PG method */
	//logger(g_log, "Net radiation using Qa and Qb = %g W/m2\n", QA + QB * (met[month].d[day].solar_rad * pow (10.0, 6)/86400.0));
	//logger(g_log, "Net radiation (3-PG method) = %g W/m2\n", c->net_radiation);
	/***************************************************************************************************************************************/
}


//BIOME-BGC version
//Running-Coughlan 1988, Ecological Modelling

void Day_Length(cell_t *const c, const int day, const int month, const int year)
{

	double ampl;  //seasonal variation in Day Length from 12 h
	static int doy;
	//double adjust_latitude;

	meteo_t *met;
	met = c->years[year].m;

	//compute yearday for GeDdayLength function
	if (day == 0 && month == JANUARY)
	{
		doy = 0;
	}
	doy +=1;

	//4/apr/2016
	//test following Schwalm & Ek 2004 instead of only geographical latitude adjusted latitude is used
	// for every 125m in altitude 1° in latitude is added
	//	adjust_latitude = g_topo->values[TOPO_ELEV] / 125.0;
	//	ampl = (exp (7.42 + (0.045 * (g_soil_settings->values[SOIL_LAT]+adjust_latitude)))) / 3600;
	//	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//logger(g_log, "with altitude = %f\n", met[month].d[day].daylength);

	ampl = (exp (7.42 + (0.045 * g_soil_settings->values[SOIL_LAT]))) / 3600;
	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//logger(g_log, "without altitude = %f\n", met[month].d[day].daylength);

	/* compute fraction of daytime */
	c->ni = met[month].d[day].daylength/24.0;

}

/* following Running et al., 1987 */
void Avg_temperature(meteo_t *const met, const int day, const int month) {
	if ( NO_DATA == met[month].d[day].tavg ) {
		if ( (NO_DATA == met[month].d[day].tmax) && (NO_DATA == met[month].d[day].tmin) ) {
			logger(g_log, "NO DATA FOR TEMPERATURE!!!!!!!!!!!!!!!!!!");
		} else {
			met[month].d[day].tavg =  (0.606 * met[month].d[day].tmax) + (0.394 * met[month].d[day].tmin);
		}
	}
}

void Psychrometric(meteo_t *const met, const int day, const int month) {
	/* compute psychrometric (KPa/°C) constant as in Allen et al., 1998 */
	met[month].d[day].psych = ((CP/1000000.0)*(met[month].d[day].air_pressure/1000.0))/(MWratio*(met[month].d[day].lh_vap/1000000.0));
}

//following BIOME-BGC 4.2 src
//compute daylight average air temperature
void Daylight_avg_temperature(meteo_t *const met, const int day, const int month) {
	if (met[month].d[day].tmax != NO_DATA && met[month].d[day].tmin != NO_DATA) {
		met[month].d[day].tday = 0.45 * (met[month].d[day].tmax - met[month].d[day].tavg) + met[month].d[day].tavg;
	} else {
		met[month].d[day].tday = NO_DATA;
		logger(g_log, "NO TMAX and TMIN can't compute TDAY!!! \n");
	}
}

//following BIOME-BGC 4.2 src
//compute nightime average air temperature
void Nightime_avg_temperature(meteo_t *const met, const int day, const int month)
{
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

void Thermic_sum (meteo_t *const met, const int day, const int month) {
	static double previous_thermic_sum;

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


void Air_pressure(meteo_t *const met, const int day, const int month)
{
	double t1, t2;

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


void Air_density (meteo_t *const met, const int day, const int month) {
	/*compute density of air (in kg/m3)*/
	//following Solantie R., 2004, Boreal Environmental Research, 9: 319-333, the model uses tday if available

	if(met[month].d[day].tday == NO_DATA)
	{
		met[month].d[day].rho_air = 1.292 - (0.00428 * met[month].d[day].tavg);
	}
	else
	{
		met[month].d[day].rho_air= 1.292 - (0.00428 * met[month].d[day].tday);
	}
}

void Latent_heat(meteo_t *met, const int day, const int month) {
	/*BIOME-BGC APPROACH*/
	/*compute latent heat of vaporization (J/Kg)*/
	met[month].d[day].lh_vap = 2.5023e6 - 2430.54 * met[month].d[day].tavg;
	met[month].d[day].lh_vap_soil = 2.5023e6 - 2430.54 * met[month].d[day].tsoil;
	/*latent heat of fusion (KJ/Kg)*/
	met[month].d[day].lh_fus = 335.0;
	/*latent heat of sublimation (KJ/Kg)*/
	met[month].d[day].lh_sub = 2845.0;
}

void Soil_temperature(meteo_t* const met, const int day, const int month) {
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

	//FIXME model doesn't get for the first 10 days of the year the averaged values
	//TODO CHECK SOIL TEMPÈRATURE CORRECTION FROM BIOME
	/* soil temperature correction using difference from annual average tair */
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
void Dew_temperature(meteo_t *const met, const int day, const int month) {
	/* dew point temperature based on Allen et al., 1998; Bosen, 1958; Murray, 1967 */
	met[month].d[day].tdew = (116.91 + 237.3 * log(met[month].d[day].ea))/(16.78 - log(met[month].d[day].ea));
}

void Annual_CO2_concentration (meteo_t *const met, const int day, const int month, const int year) {
	static double previous_co2_conc;

	/* recompute co2 concentration at the beginning of each year */
	if( ! string_compare_i(g_settings->CO2_fixed, "off") && day == 0 && month == 0)
	{
		/* assign first year value from site.txt */
		if(year == 0)
		{
			met[month].d[day].co2_conc = g_settings->co2Conc;
			previous_co2_conc = met[month].d[day].co2_conc;
		}
		else
		{
			/* then for other years increment each beginning of year */
			met[month].d[day].co2_conc = previous_co2_conc + (previous_co2_conc * g_settings->co2_incr);
			previous_co2_conc = met[month].d[day].co2_conc;
			logger(g_log, "CO2 annual increment = %f ppmv\n", met[month].d[day].co2_conc * g_settings->co2_incr);
			logger(g_log, "CO2 concentration  = %f ppmv\n", met[month].d[day].co2_conc);
		}
	}
}

void Annual_met_values(cell_t *const c, const int day, const int month, const int year) {
	meteo_t* met;

	assert(c);

	met = c->years[year].m;
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
		logger(g_log, "**ANNUAL MET VALUES day = %d month = %d year = %d**\n", day+1, month+1, year+1);
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



