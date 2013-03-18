/*met_data.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"
#include "constants.h"

extern int DaysInMonth[];

void Get_snow_met_data (CELL *c, const MET_DATA *const met, int month, int day)
{

	//todo inserire la funzione anche nelle altre parti del treemodel.c

	//FOLLOWING BIOME APPROACH
	/* temperature and radiation snowmelt,
	from Joseph Coughlan PhD thesis, 1991 */

	static float snow_abs = 0.6; // absorptivity of snow
	static float t_coeff = 0.65; // (kg/m2/deg C/d) temp. snowmelt coeff
	float snow_melt;
	float snow_sublimation;
	float incident_rad;  //incident radiation (kJ/m2/d) incident radiation
	float melt, t_melt, r_melt, r_sub;

	t_melt = r_melt = r_sub = 0;

	/* canopy transmitted radiation: convert from W/m2 --> KJ/m2/d */
	if (settings->time == 'm')
	{
		incident_rad = c->net_radiation_for_soil * DaysInMonth[month] * snow_abs * 0.001;
	}
	else
	{
		incident_rad = c->net_radiation_for_soil * snow_abs * 0.001;
	}
	Log("net_radiation for soil = %g\n", c->net_radiation_for_soil);
	Log("incident radiation for soil = %g\n", incident_rad);

	if (settings->time == 'm')
	{
		if (met[month].tavg < 0) /* sublimation from snowpack */
		{
			r_sub = incident_rad / LATENT_HEAT_SUBLIMATION;
			Log("r_sub = %g\n", r_sub);
			if (r_sub > c->snow)
			{

			}


		}
		else if (met[month].tavg > 0 && !c->snow) /* temperature and radiation melt from snowpack */
		{

		}
		else
		{

		}
	}
	else
	{
		if (met[month].d[day].tavg < 0) /* sublimation from snowpack */
		{
			r_sub = incident_rad / LATENT_HEAT_SUBLIMATION;
			Log("r_sub = %g\n", r_sub);
			if (r_sub > c->snow)
			{
				//
			}


		}
		else if (met[month].d[day].tavg > 0 && !c->snow) /* temperature and radiation melt from snowpack */
		{

		}
		else
		{

		}
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
	svp = 6.1076 * exp ((17.269 * met[month].tavg) / (met[month].tav + 237.3));
	//Log("svp = %g\n", svp);
	vp = met[month].rh * (svp /100);
	//Log("vp = %g\n", vp);
	vpd = svp -vp;
	//Log("vpd = %g \n", vpd);

	return vpd;
}
 */

void Print_met_data (const MET_DATA *const met, float vpd, int month, int day, float daylength)
{
	if (settings->time == 'm')
	{
		Log("***************\n");
		Log("**Monthly MET DATA**\n");
		Log("-average solar_rad = %g MJ/m^2/day\n"
				"-tav = %g °C\n"
				//"-rh = %g %%\n"
				"-vpd = %g mbar\n"
				"-ts_f = %g °C\n"
				"-rain = %g mm\n"
				"-swc = %g %vol\n",
				met[month].solar_rad,
				met[month].tavg,
				//met[month].rh,
				vpd,
				met[month].ts_f,
				met[month].rain,
				met[month].swc);

		if (settings->spatial == 's')
		{
			Log("-lai from NDVI = %g \n", met[month].ndvi_lai);
		}
	}
	else
	{
		Log("***************\n");
		Log("**Daily MET DATA**\n");
		Log("-average solar_rad = %g MJ/m^2/day\n"
				"-tavg = %g °C\n"
				//"-rh = %g %%\n"
				"-vpd = %g mbar\n"
				"-ts_f = %g °C\n"
				"-rain = %g mm\n"
				"-swc = %g %vol\n",
				met[month].d[day].solar_rad,
				met[month].d[day].tavg,
				//met[month].rh,
				vpd,
				met[month].d[day].ts_f,
				met[month].d[day].rain,
				met[month].d[day].swc);

		if (settings->spatial == 's')
		{
			Log("-lai from NDVI = %g \n", met[month].d[day].ndvi_lai);
		}
	}


	Log("-Day length = %g hours \n", daylength);
	Log("***************\n");

}
