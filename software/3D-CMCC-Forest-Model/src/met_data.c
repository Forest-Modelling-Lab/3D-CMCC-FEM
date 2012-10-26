/*met_data.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"


float Get_vpd (const MET_DATA *const met, int month)
{
	float svp;
	float vp;
	float vpd;

	//Log("\n GET_VPD_ROUTINE \n");


	//compute vpd
	//see triplex model Peng et al., 2002
	svp = 6.1076 * exp ((17.269 * met[month].tav) / (met[month].tav + 237.3));
	//Log("svp = %g\n", svp);
	vp = met[month].rh * (svp /100);
	//Log("vp = %g\n", vp);
	vpd = svp -vp;
	//Log("vpd = %g \n", vpd);

	return vpd;
}

void Print_met_data (const MET_DATA *const met, float vpd, int month, float daylength)
{
	Log("***************\n");
	Log("**Monthly MET DATA**\n");
	Log("-average solar_rad = %g MJ/m^2/day\n"
			"-tav = %g °C\n"
			"-rh = %g %%\n"
			"-vpd = %g mbar\n"
			"-ts_f = %g °C\n"
			"-rain = %g mm\n"
			"-swc = %g %vol\n",
			met[month].solar_rad,
			met[month].tav,
			met[month].rh,
			vpd,
			met[month].ts_f,
			met[month].rain,
			met[month].swc);

	if (settings->version == 's')
	{
		Log("-lai from NDVI = %g \n", met[month].ndvi_lai);
	}


	Log("-Day length = %g hours \n", daylength);
	Log("***************\n");

}
