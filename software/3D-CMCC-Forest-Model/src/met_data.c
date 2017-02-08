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

extern soil_settings_t *g_soil_settings;
extern topo_t *g_topo;
extern logger_t* g_debug_log;

void Sat_vapour_pressure(cell_t *const c, const int day, const int month, const int year)
{
	double e0max;                                    /* saturation vapour pressure at the maximum air temperature (KPa) */
	double e0min;                                    /* saturation vapour pressure at the minimum air temperature (KPa) */
	const double A = 17.27;
	const double Tstroke = 36;
	double TmaxK;
	double TminK;

	meteo_t *met;
	met = (meteo_t*) c->years[year].m;

	TmaxK = met[month].d[day].tmax + TempAbs;
	TminK = met[month].d[day].tmin + TempAbs;

	/* following Allen et al., 1998 */
	/* compute saturation vapour pressure at the maximum and minimum air temperature (KPa) */
	e0max = ESTAR * exp((A*met[month].d[day].tmax)/(TmaxK - Tstroke));
	e0min = ESTAR * exp((A*met[month].d[day].tmin)/(TminK - Tstroke));

	/* compute weighted mean saturation vapour pressure at the air temperature (KPa)*/
	met[month].d[day].es = ((e0max*met[month].d[day].ni) + (e0min*(1.0-met[month].d[day].ni)));

	/* compute actual vapour pressure derived from relative humidity data (KPa) */
	met[month].d[day].ea = (met[month].d[day].rh_f/100.0)*met[month].d[day].es;
	//CHECK_CONDITION(met[month].d[day].ea, < 0.0);
	//fixme
	if (met[month].d[day].ea < 0.0)
	{
		met[month].d[day].ea = 0.0;
	}
}

static double get_daily_potential_radiation(const double latitude, const double longitude, const int d_, const double t_) {
	double localstandardtime;
	double localapparentsolartime;
	double tthet;
	double signedLAS;
	double omega;
	double decl_rad;
	double lat_rad;
	double theta_rad;
	double Rpot;
	double Rpot_h;

	localstandardtime = t_;
	tthet = 2.*Pi*(d_-1.) / 365.;

	localapparentsolartime = localstandardtime;
	signedLAS = 12.-localapparentsolartime;
	signedLAS = fabs(signedLAS);

	omega = -15.*signedLAS;
	decl_rad = 0.006918-0.399912*cos(tthet)+0.070257*sin(tthet)-0.006758*cos(2*tthet)+0.000907*sin(2*tthet)-0.002697*cos(3*tthet)+0.00148*sin(3*tthet);
	lat_rad = latitude*Pi/180.;

	theta_rad = acos(sin(decl_rad)*sin(lat_rad)+cos(decl_rad)*cos(lat_rad)*cos(omega*Pi/180.));

	/* Compute Potential short wave downward radiation [W m-2] (short wave clear sky radiation) */
	Rpot = Q0*(1.00011+0.034221*cos(tthet)+0.00128*sin(tthet)+0.000719*cos(2*tthet)+0.000077*sin(2*tthet));
	Rpot_h = Rpot*cos(theta_rad);

	return (Rpot_h > 0) ? Rpot_h : 0.0;
}

static double compute_potential_rad(const double lat, const double lon, const int day) {
#define ROWS_PER_DAY	(60*24)
	int i;
	double rpot;

	/* potential radiation is computed for each minute */
	rpot = 0.;
	for ( i = 0; i < ROWS_PER_DAY; ++i ) {
		double doy;
		double hrs;
		doy = ((double)(day*ROWS_PER_DAY+i) / ROWS_PER_DAY) + 1;
		hrs = (double)((day*ROWS_PER_DAY+i) % ROWS_PER_DAY) / 60.;
		rpot += get_daily_potential_radiation(lat, lon, doy, hrs);
	}
	rpot /= ROWS_PER_DAY;
	return rpot;

#undef ROWS_PER_DAY
}

void compute_atm_lw_downward_W(cell_t *const c, const int day, const int month, const int year) {

	/*calculates longwave radiation from Vapour Pressure and Air Temp. and Radiation
	-------------------------------------------------------------------------------
	implemented by Sönke Zaehle (szaehle@bgc-jena.mpg.de)
	MPI for Biogeochemistry, Jena Germany

	DISCLAIMER:
	This software comes with absolutely no warrenty whatsoever wrt to its performance
	or scientific correctness. Any error associated with results produced from
	this routine is not in the responsibility of the author of this routine
	-------------------------------------------------------------------------------
	Input:
	 -Tair: Air temperature [°C]
	 -sw_downward_W: Shortwave downward radiation [W m-2]
	 -Rg_pot: Potential shortwave downward radiation [W m-2]
	 -VP:  Vapour Pressure [Pa]
	Output:
	 -atm_lw_downward_W: Longwave downward radiation [W m-2]
	-------------------------------------------------------------------------------
	Calculation of Longwave follows the JSBACH algorithm
	Downward long wave radiation flux "R_d" [W/m^2] is according to [1],[2] computed by
	(1) R_d = r_cloud * epsA * SBC_W * T^4,
	where "TairK" is the air temperature [K] (in the bottom layer?), SBC_W is the Stefan-Boltzmann constant (in W),
	"epsA" is the emissivity of the cloudless atmosphere given by
	(2) epsA = epsA0 * (e_A/T)^(1/7).
	Here "e_vap" is the vapor pressure in [Pa] from above, and epsA0 = 0.64 (units: [(K/Pa)^(1/7)]) a constant (see [2]).
	The factor "r_cloud" in (1) corrects for clouds and is given by
	(3) r_cloud = 1+0.22*n^2,
	where "n" is the cloudcover.
	[1] W. Knorr, "Satellite remote sensing and modelling of the global CO2 exchange of land vegetation", Examensarbeit 49,
	(Max Planck Institut fuer Meteorologie, Hamburg, 1998).
	[2] W. Brutsaert, "Evaporation into the Atmosphere", (Reidel, Dordrecht, 1982), pp. 299.
	Calculation of Vapour Pressure follows Monteith & Unsworth 2008, page 11f [3] */

	double fpar;
	//double cloud_cover;
	//double r_cloud;
	double vp;
	//double epsA;                                /* emissivity of the cloudless atmosphere */
	//double lw_downward_W;

	/* input met data */
	double sw_in;
	double ta;
	double vpd;                        /* deficit vapour pressure at the air temperature in hPa */
	double sw_pot_in;
	double esat;                        /* saturation vapour pressure at the air temperature (KPa)*/
	double TairK;

	meteo_t *met;
	met = (meteo_t*) c->years[year].m;

	sw_in = met[month].d[day].sw_downward_W;
	ta = met[month].d[day].tavg;
	vpd = met[month].d[day].vpd;
	sw_pot_in = met[month].d[day].sw_pot_downward_W;
	esat = met[month].d[day].es;
	TairK = ta + TempAbs;


	fpar = 0.;
	if ( ! (0. == sw_pot_in) && ! IS_INVALID_VALUE(sw_in) )
	{
		fpar = sw_in / sw_pot_in;
		if ( fpar < 0. )
		{
			fpar = 0.;
		}
	}

	/* Cloud cover and cloud correction factor Eq. (3) */
	met[month].d[day].cloud_cover_frac = 1.0 - (fpar - 0.5) / 0.4;
	if ( met[month].d[day].cloud_cover_frac > 1.0 )
	{
		met[month].d[day].cloud_cover_frac = 1.0;
	}
	if ( met[month].d[day].cloud_cover_frac < 0. )
	{
		met[month].d[day].cloud_cover_frac = 0.0;
	}
	met[month].d[day].cloud_cover_frac_corr = 1 + 0.22 * pow(met[month].d[day].cloud_cover_frac, 2.0);

	/* Saturation and actual Vapour pressure [3], and associated emissivity Eq. [2] */
	/* convert esat kPa --> Pa */
	esat *= 1000.0;

	/* convert vpd hPa --> Pa */
	vpd *= 100.0;

	//fixme it is .ea
	vp = esat - vpd;
	if ( vp < 0.0 )
	{
		vp = 3.3546e-004;
	}
	met[month].d[day].emis_atm_clear_sky = 0.64 * pow(vp / TairK, 0.14285714);

	/* compute atmopsheric emissivity based on cloud cover corrected */
	met[month].d[day].emis_atm = met[month].d[day].cloud_cover_frac_corr * met[month].d[day].emis_atm_clear_sky;

	/* Atmospheric Longwave radiation flux downward Eq. [1] */
	met[month].d[day].atm_lw_downward_W = met[month].d[day].emis_atm * SBC_W * pow(TairK, 4);
	if ( (met[month].d[day].atm_lw_downward_W < 10.) || (met[month].d[day].atm_lw_downward_W > 1000.) )
	{
		met[month].d[day].atm_lw_downward_W = INVALID_VALUE;
	}
}

void Radiation (cell_t *const c, const int day, const int month, const int year)
{
	double a = 107.0;                                                                    //(W/m)  empirical constants for long wave radiation computation
	double b = 0.2;                                                                      //(unit less) empirical constants for long wave radiation computation
	double TmaxK, TminK;

	//double fpar;


	meteo_t *met;
	met = (meteo_t*) c->years[year].m;

	TmaxK = met[month].d[day].tmax + TempAbs;
	TminK = met[month].d[day].tmin + TempAbs;

	/***************************************************************************************************************************************/

	/* SHORT WAVE RADIATION */
	/* INCOMING SHORT WAVE RADIATION */

	/* compute downward Short-Wave radiation */
	met[month].d[day].sw_downward_MJ = met[month].d[day].solar_rad;
	//logger(g_debug_log, "Short_wave_radiation (downward) = %g MJ/m^2 day\n", met[month].d[day].sw_downward_MJ);

	/* convert into W/m2 */
	met[month].d[day].incoming_sw_downward_W = met[month].d[day].sw_downward_MJ * MJ_TO_W;
	met[month].d[day].sw_downward_W = met[month].d[day].incoming_sw_downward_W ;
	//logger(g_debug_log, "Short wave radiation (downward) = %g W/m2\n", met[month].d[day].sw_downward_W);

	met[month].d[day].sw_pot_downward_W = compute_potential_rad(g_soil_settings->values[SOIL_LAT], g_soil_settings->values[SOIL_LON], day);
	//logger(g_debug_log, "sw_pot_downward_W = %g W/m2\n", met[month].d[day].sw_pot_downward_W);

	/* convert incoming Short-Wave flux in PAR from MJ/m2/day to molPAR/m2/day (Biome-BGC method) */
	met[month].d[day].incoming_par = (met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR);
	met[month].d[day].par = met[month].d[day].incoming_par;
	//logger(g_debug_log, "Par = %g molPAR/m^2 day\n", met[month].d[day].par);

	/* convert incoming Short-Wave flux in PPFD from W/m2 to umol/m2/sec (Biome-BGC method) */
	met[month].d[day].incoming_ppfd = met[month].d[day].sw_downward_W * RAD2PAR * EPAR;
	met[month].d[day].ppfd = met[month].d[day].incoming_ppfd;
	//logger(g_debug_log, "PPFD = %g umolPPFD/m2/sec\n", met[month].d[day].ppfd);

	/***************************************************************************************************************************************/

	/* LONG WAVE RADIATION */
	/* INCOMING LONG WAVE RADIATION */

	/* compute Long-Wave radiation flux downward (following JSBACH model) */
	compute_atm_lw_downward_W(c, day, month, year);

	/* NET LONG WAVE RADIATION */

	//fixme to avoid crash in model for negative "ea" values use different calculation of long_wave_radiation following Prentice (IT HAS TO BE SOLVED ANYWAY)
	if(met[month].d[day].ea > 0.0)
	{
		/* following Allen et al., 1998 */
		/* it represents the outgoing part of long wave radiation */
		met[month].d[day].lw_net_MJ = SBC_MJ * (pow(((TmaxK + TminK)/2.0),4))*(0.34-0.14*(sqrt(met[month].d[day].ea)))*met[month].d[day].cloud_cover_frac;
		//logger(g_debug_log, "Net Long wave radiation (Allen)= %g MJ/m^2 day\n", met[month].d[day].lw_net_MJ);

		/* convert into W/m2 */
		met[month].d[day].lw_net_W = met[month].d[day].lw_net_MJ * MJ_TO_W;
		//logger(g_debug_log, "Net Long wave radiation (Allen)= %g W/m2\n", met[month].d[day].lw_net_W);
		/*****************************************************************************************/
	}
	else
	{
		//todo check it Prentice says "net upward long-wave flux"
		/* following Prentice et al., 1993 */
		met[month].d[day].lw_net_W = (b+(1.0-b)*met[month].d[day].ni)*(a - met[month].d[day].tavg);
		//logger(g_debug_log, "Net Long wave radiation (Prentice)= %g W/m2\n", met[month].d[day].lw_net_W);

		/* convert into MJ/m^2 day */
		met[month].d[day].lw_net_MJ = met[month].d[day].lw_net_W * W_TO_MJ;
		//logger(g_debug_log, "Net Long wave radiation (Prentice)= %g MJ/m^2 day\n", met[month].d[day].lw_net_MJ);
		/*****************************************************************************************/
	}

	/* net radiation based on 3-PG method */
	//logger(g_debug_log, "Net radiation using Qa and Qb = %g W/m2\n", QA + QB * (met[month].d[day].solar_rad * pow (10.0, 6)/86400.0));
	//logger(g_debug_log, "Net radiation (3-PG method) = %g W/m2\n", c->net_radiation);
	/***************************************************************************************************************************************/


}

void Check_prcp(cell_t *c, const int day, const int month, const int year)
{
	meteo_t *met;
	met = c->years[year].m;
	if(met[month].d[day].prcp > 0.0)
	{
		if (met[month].d[day].tavg > 0.0)
		{
			met[month].d[day].rain = met[month].d[day].prcp;
			met[month].d[day].snow = 0.0;
		}
		else
		{
			met[month].d[day].snow = met[month].d[day].prcp;
			met[month].d[day].rain = 0.0;
		}
	}
	else
	{
		met[month].d[day].rain = 0.0;
		met[month].d[day].snow = 0.0;
	}

	/* following Lagergren et al., 2006 */
	/* impose zero value for tsoil in case of snow presence */
	//todo move to soil when snow melt and subl will go to soil
	//if(c->snow_pack != 0) met[month].d[day].tsoil = 0.0;
}

void Day_Length(cell_t *c, const int day, const int month, const int year)
{

	double ampl;  //seasonal variation in Day Length from 12 h
	static int doy;
	//double adjust_latitude;

	/* BIOME-BGC version */
	/* Running-Coughlan 1988, Ecological Modelling */

	meteo_t *met;
	met = c->years[year].m;

	/* compute doy for GeDdayLength function */
	if (!day && month == JANUARY)
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
	//logger(g_debug_log, "with altitude = %f\n", met[month].d[day].daylength);

	ampl = (exp (7.42 + (0.045 * g_soil_settings->values[SOIL_LAT]))) / 3600;
	met[month].d[day].daylength = ampl * (sin ((doy - 79) * 0.01721)) + 12;
	//logger(g_debug_log, "without altitude = %f\n", met[month].d[day].daylength);

	/* compute fraction of daytime */
	met[month].d[day].ni = met[month].d[day].daylength/24.0;

}

/* following Running et al., 1987 */
void Avg_temperature(meteo_t *met, const int day, const int month)
{
	if ( NO_DATA == met[month].d[day].tavg )
	{
		if ( (NO_DATA == met[month].d[day].tmax) && (NO_DATA == met[month].d[day].tmin) )
		{
			logger(g_debug_log, "NO DATA FOR TEMPERATURE!\n");
		} else {
			met[month].d[day].tavg =  (0.606 * met[month].d[day].tmax) + (0.394 * met[month].d[day].tmin);
		}
	}
}

void Psychrometric(meteo_t *met, const int day, const int month) {
	/* compute psychrometric (KPa/°C) constant as in Allen et al., 1998 */
	met[month].d[day].psych = ((CP/1000000.0)*(met[month].d[day].air_pressure/1000.0))/(MWratio*(met[month].d[day].lh_vap/1000000.0));
}

void Daylight_avg_temperature(meteo_t *const met, const int day, const int month)
{
	/* BIOME-BGC version */
	/* Running-Coughlan 1988, Ecological Modelling */

	if (met[month].d[day].tmax != NO_DATA && met[month].d[day].tmin != NO_DATA)
	{
		met[month].d[day].tday = 0.45 * (met[month].d[day].tmax - met[month].d[day].tavg) + met[month].d[day].tavg;
	}
	else
	{
		met[month].d[day].tday = NO_DATA;
		logger(g_debug_log, "NO TMAX and TMIN can't compute TDAY!!! \n");
	}
}

void Nightime_avg_temperature(meteo_t *met, const int day, const int month)
{
	/* BIOME-BGC version */
	/* Running-Coughlan 1988, Ecological Modelling */

	if (met[month].d[day].tday != NO_DATA )
	{
		met[month].d[day].tnight = (met[month].d[day].tday + met[month].d[day].tmin)/2 ;
	}
	else
	{
		met[month].d[day].tnight = NO_DATA;
		logger(g_debug_log, "NO TMAX and TMIN can't compute TNIGHT!!! \n");
	}
}

void Thermic_sum (meteo_t *met, const int day, const int month, const int year) {
	static double previous_thermic_sum;

	if (!day && !month)
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
			logger(g_debug_log, "tavg NO_DATA!!\n");
	}
	else
	{
		if(met[month].d[day].tavg > GDD_BASIS)
		{
			met[month].d[day].thermic_sum = previous_thermic_sum + (met[month].d[day].tavg - GDD_BASIS);
			previous_thermic_sum = met[month].d[day].thermic_sum;
		}
		else
		{
			met[month].d[day].thermic_sum = previous_thermic_sum;
		}
		if (met[month].d[day].tavg == NO_DATA)
			logger(g_debug_log, "tavg NO_DATA!!\n");
	}
}

void Air_pressure(meteo_t *met, const int day, const int month)
{
	double t1, t2;

	/* compute air pressure */
	/* BIOME-BGC version */

	/* daily atmospheric pressure (Pa) as a function of elevation (m) */
	/* From the discussion on atmospheric statics in:
	Iribane, J.V., and W.L. Godson, 1981. Atmospheric Thermodynamics, 2nd
		Edition. D. Reidel Publishing Company, Dordrecht, The Netherlands.
		(p. 168)*/

	t1 = 1.0 - (LR_STD * g_topo->values[TOPO_ELEV])/T_STD;
	t2 = G_STD / (LR_STD * (Rgas / MA));
	met[month].d[day].air_pressure = P_STD * pow (t1, t2);
	//logger(g_debug_log, "Air pressure = %f Pa\n", met[month].d[day].air_pressure);
}


void Air_density (meteo_t *met, const int day, const int month) {

	/* compute density of air (in kg/m3) */
	/* following Solantie R., 2004, Boreal Environmental Research, 9: 319-333, the model uses tday if available */

	if(met[month].d[day].tday == NO_DATA)
	{
		met[month].d[day].rho_air = 1.292 - (0.00428 * met[month].d[day].tavg);
	}
	else
	{
		met[month].d[day].rho_air= 1.292 - (0.00428 * met[month].d[day].tday);
	}
}

void Latent_heat(meteo_t *met, const int day, const int month)
{
	/*BIOME-BGC APPROACH*/
	/*compute latent heat of vaporization (J/Kg)*/

	met[month].d[day].lh_vap = 2.5023e6 - 2430.54 * met[month].d[day].tavg;
	met[month].d[day].lh_vap_soil = 2.5023e6 - 2430.54 * met[month].d[day].tsoil;
	/*latent heat of fusion (KJ/Kg)*/
	met[month].d[day].lh_fus = 335.0;
	/*latent heat of sublimation (KJ/Kg)*/
	met[month].d[day].lh_sub = 2845.0;
}

void Soil_temperature(meteo_t* met, const int day, const int month) {
	double avg = 0;
	int i;
	int day_temp = day;
	int day_avg = 11;
	int month_temp = month;
	int weight;
	int incr_weight;
	const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


	/* following BIOME-bgc 4.2 */
	/* for this version, an 11-day running weighted average of daily
	average temperature is used as the soil temperature at 10 cm.
	For days 1-10, a 1-10 day running weighted average is used instead.
	The tail of the running average is weighted linearly from 1 to 11.
	There are no corrections for snowpack or vegetation cover.
	 */

	//FIXME model doesn't get for the first 10 days of the year the averaged values
	//TODO CHECK SOIL TEMPÈRATURE CORRECTION FROM BIOME
	/* soil temperature correction using difference from annual average tair */
	/*file bgc.c*/
	/* original biome_bgc version */
	/* *
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


	if ( day < day_avg && !month )
	{
		if ( met[month].d[day].ts_f != NO_DATA )
		{
			met[month_temp].d[day_temp].tsoil = met[month].d[day].ts_f;
		}
		else
		{
			met[month_temp].d[day_temp].tsoil = met[month_temp].d[day_temp].tavg;
		}
	}
	else
	{
		//note new 08 Feb 2017
		for ( i = day_avg; i > 0; --i )
		{
			weight = i;

			incr_weight += i;

			if ( day_temp >= 0 )
			{
				avg += (met[month].d[day_temp].tavg * weight);
			}
			else
			{
				avg += (met[month-1].d[days_per_month[month-1] +(day_temp)].tavg * weight);
				//note:
				//avg += (met[month-1].d[days_per_month[month-1] - fabs(day_temp)].tavg * weight);
			}
			--day_temp;
		}

		/* compute average */
		avg = avg / (double)incr_weight;
		met[month].d[day].tsoil = avg;

	}
}


void Ten_day_tavg (cell_t *c, const int day, const int month, const int year)
{
	double avg = 0.0;
	int i;
	int day_temp = day;
	int day_avg = 10;
	int weight;
	int incr_weight;
	int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	//note: include for leap_years
	//note: for the first ten days consider days before
	if ( day < day_avg && !month )
	{
		c->years[year].m[month].d[day].ten_day_tavg = c->years[year].m[month].d[day].tavg;
	}
	else
	{
		for ( i = day_avg; i > 0; --i )
		{
			weight = i;

			incr_weight += i;

			if ( day_temp >= 0 )
			{
				avg += (c->years[year].m[month].d[day_temp].tavg * weight);
			}
			else
			{
				if ( (month == MARCH) && IS_LEAP_YEAR (c->years[year].year) )
				{
					avg += (c->years[year].m[month-1].d[(days_per_month[month-1]+1) + day_temp].tday * weight);
				}
				else
				{
					avg += (c->years[year].m[month-1].d[days_per_month[month-1] +(day_temp)].tday * weight);
				}
			}
			--day_temp;
		}

		/* compute average */
		c->years[year].m[month].d[day].ten_day_tavg = avg / incr_weight;
	}
}

void Ten_day_tday (meteo_t* met, const int day, const int month)
{
	double avg = 0.0;
	int i;
	int day_temp = day;
	int day_avg = 10;
	int weight;
	int incr_weight;
	const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	//note: include for leap_years
	//note: for the first ten days consider days before
	if ( day < day_avg && !month )
	{
		met[month].d[day].ten_day_tday = met[month].d[day].tday;
	}
	else
	{
		for ( i = day_avg; i > 0; --i )
		{
			weight = i;

			incr_weight += i;

			if ( day_temp >= 0 )
			{
				avg += (met[month].d[day_temp].tday * weight);
			}
			else
			{
				avg += (met[month-1].d[days_per_month[month-1] +(day_temp)].tday * weight);
				//note:
				//avg += (met[month-1].d[days_per_month[month-1] - fabs(day_temp)].tavg * weight);
			}
			--day_temp;
		}

		/* compute average */
		met[month].d[day].ten_day_tday = avg / incr_weight;
	}
}

void Ten_day_tnight (meteo_t* met, const int day, const int month)
{
	double avg = 0.0;
	int i;
	int day_temp = day;
	int day_avg = 10;
	int weight;
	int incr_weight;
	const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	//note: include for leap_years
	//note: for the first ten days consider days before
	if ( day < day_avg && !month )
	{
		met[month].d[day].ten_day_tnight = met[month].d[day].tnight;
	}
	else
	{
		for ( i = day_avg; i > 0; --i )
		{
			weight = i;

			incr_weight += i;

			if ( day_temp >= 0 )
			{
				avg += (met[month].d[day_temp].tnight * weight);
			}
			else
			{
				avg += (met[month-1].d[days_per_month[month-1] +(day_temp)].tnight * weight);
				//note:
				//avg += (met[month-1].d[days_per_month[month-1] - fabs(day_temp)].tavg * weight);
			}
			--day_temp;
		}

		/* compute average */
		met[month].d[day].ten_day_tnight = avg / incr_weight;
	}
}

void Ten_day_tsoil (meteo_t* met, const int day, const int month)
{
	double avg = 0.0;
	int i;
	int day_temp = day;
	int day_avg = 10;
	int weight;
	int incr_weight;
	const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	//note: include for leap_years
	//note: for the first ten days consider days before
	if ( day < day_avg && !month )
	{
		met[month].d[day].ten_day_tsoil = met[month].d[day].tsoil;
	}
	else
	{
		for ( i = day_avg; i > 0; --i )
		{
			weight = i;

			incr_weight += i;

			if ( day_temp >= 0 )
			{
				avg += (met[month].d[day_temp].tsoil * weight);
			}
			else
			{
				avg += (met[month-1].d[days_per_month[month-1] +(day_temp)].tsoil * weight);
				//note:
				//avg += (met[month-1].d[days_per_month[month-1] - fabs(day_temp)].tavg * weight);
			}
			--day_temp;
		}

		/* compute average */
		met[month].d[day].ten_day_tsoil = avg / incr_weight;
	}
}

void Dew_temperature(meteo_t *const met, const int day, const int month) {
	/* dew point temperature based on Allen et al., 1998; Bosen, 1958; Murray, 1967 */
	met[month].d[day].tdew = (116.91 + 237.3 * log(met[month].d[day].ea))/(16.78 - log(met[month].d[day].ea));
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
		logger(g_debug_log, "**ANNUAL MET VALUES day = %d month = %d year = %d**\n", day+1, month+1, year+1);
		logger(g_debug_log, "-Annual average tavg = %f C°\n", c->annual_tavg);
		logger(g_debug_log, "-Annual average tmax = %f C°\n", c->annual_tmax);
		logger(g_debug_log, "-Annual average tmin = %f C°\n", c->annual_tmin);
		logger(g_debug_log, "-Annual average tday = %f C°\n", c->annual_tday);
		logger(g_debug_log, "-Annual average tnight = %f C°\n", c->annual_tnight);
		logger(g_debug_log, "-Annual average tsoil = %f C°\n", c->annual_tsoil);
		logger(g_debug_log, "-Annual average solar rad = %f MJ/m2/day\n", c->annual_solar_rad);
		logger(g_debug_log, "-Annual average prcp = %f mm/m2/day\n", c->annual_precip);
		logger(g_debug_log, "-Annual average vpd = %f hPa/day \n", c->annual_vpd);
	}

}



