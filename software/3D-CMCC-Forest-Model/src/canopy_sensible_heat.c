/*
 * canopy_sensible_heat.c
 *
 *  Created on: 08 giu 2016
 *      Author: alessio
 */


/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void canopy_sensible_heat (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height, int age, int species)
{

	double rad_abs;           //absorbed shortwave longwave radiation (W/m2)
	double psych;             //psychrometric constant (KPa/°C)
	double psych_p;           //product as psychrometric constant and (1+(rc/ra)) see Webber et al., 2016
	double dt = 0.2;
	double t1, t2;            //temperature offset (°C)
	double pvs1, pvs2;        //calculate saturation vapor pressures (Pa)
	double delta;             //slope of pvs vs. T curve, at ta
	double tairK;             //air temperature (K)
	double tcanopy,           //canopy temperature (°C)
	       t_canopy_day,      //day time canopy temperature (°C)
	       tcanopy_night;     //night time canopy temperature (°C)
	double tcanopy_K,         //canopy temperature (K)
	       t_canopy_day_K,    //day time canopy temperature (K)
	       tcanopy_night_K;   //night time canopy temperature (K)


	/* CANOPY SENSIBLE HEAT FLUX */
	logger(g_log, "\ncanopy sensible heat\n");

	logger(g_log, "LAI = %f\n", s->value[LAI]);

//	rad_abs = ;
	logger(g_log, "rad_abs = %f\n", rad_abs);

	/* FIRST OF ALL COMPUTE CANOPY TEMPERATURE */
	/* compute psychrometric (KPa/°C) constant as in Allen et al., 1998 */
	psych = ((CP/1000000.0)*(met[month].d[day].air_pressure/1000.0))/(MWratio*(met[month].d[day].lh_vap/1000000.0));
	logger(g_log, "psych = %f\n", psych);

	/* calculate temperature offsets for slope estimate */
	t1 = met[month].d[day].tday+dt;
	t2 = met[month].d[day].tday-dt;

	/* calculate saturation vapor pressures (Pa) at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	logger(g_log, "pvs1 = %f\n", pvs1);
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));
	logger(g_log, "pvs2 = %f\n", pvs2);

	/* calculate slope of pvs vs. T curve, at ta */
	//test this is the "DELTA" function as in Webber et al., 2016
	delta = (pvs1-pvs2) / (t1-t2);
	/* converts into kPA following Webber et al., 2016 */
	delta /= 1000.0;
	logger(g_log, "delta = %f KPa\n", delta);

	//test
	// as in Ryder et al., 2016 resistance to sensible heat flux is equal to boundary layer resistance (see also BIOME)
	// so it should be rh (1/gl_sh)


//	/* canopy resistance m sec-1)*/
//	//fixme gl_sh or gc_sh? Wang and Leuning 1998 use stomatal conductance
//	//fixme this is valid for cell level not for class level
//	rc = 1.0/gc_sh;
//
//	//test this is the equivalent "ra" aerodynamic resistance as in Allen et al., 1998
//	/* calculate resistance to radiative heat transfer through air, rr */
//	rr = met[month].d[day].rho_air * CP / (4.0 * SBC * (pow(tairK, 3)));
//	rhr = (rh * rr) / (rh + rr);
//	/* compute product as psychrometric constant and (1+(rc/ra)) see Webber et al., 2016 */
//	/* then ra = rhr */
//	psych_p = psych *(1+(rc/rhr));
//	logger(g_log, "psych_p = %f\n", psych_p);
//
//	//test to avoid problems using generic daily data we should divide the fluxes into diurnal (using tday) and nocturnal (using tnight) (but for net_rad???)
//	//to have tcanopy_day and tcanopy_night considering day length
//
//	//fixme conductance and resistance variables following Norman And Campbell should be in mol m sec (not m sec)
//	//test convert boundary layer conductance from m/sec into mol m/sec
//	//following Pearcy, Schulze and Zimmermann
//	rh_mol = rh * 0.0446 * ((TempAbs / (met[month].d[day].tavg+TempAbs))*((met[month].d[day].air_pressure/1000.0)/101.3));
//	logger(g_log, "boundary layer resistance = %g mol m/sec\n", rh_mol);
//
//	/* canopy temperature as in Webber et al., 2016 it takes rh in m/sec*/
//	tcanopy = met[month].d[day].tavg + ((rad_abs * rh_mol)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);
//
//	//test 07 june 2016 using only boundary layer resistance as in Ryder et al., 2016
//	//tcanopy = met[month].d[day].tavg + ((net_rad * rh)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);
//
//	logger(g_log, "rad_abs = %f\n", rad_abs);
//	logger(g_log, "rho_air = %f\n", met[month].d[day].rho_air);
//	logger(g_log, "rc = %f\n", rc);
//	logger(g_log, "rhr = %f\n", rhr);
//	logger(g_log, "tavg = %f °C\n", met[month].d[day].tavg);
//	logger(g_log, "canopy temp = %f °C\n", tcanopy);
//	logger(g_log, "differences = %f °C\n", tcanopy - met[month].d[day].tavg);
//	//getchar();
//
//	tcanopy_K = tcanopy + TempAbs;
//
//	logger(g_log, "canopy_temp = %f K\n", tcanopy_K);
//	logger(g_log, "tairK = %f K\n", tairK);
//
//	//fixme this is valid for cell level not for class level
//	c->daily_canopy_sensible_heat_flux = met[month].d[day].rho_air * CP * ((tcanopy_K-tairK)/rhr);
//	logger(g_log, "canopy_sensible_heat_flux = %f Wm-2\n", c->daily_canopy_sensible_heat_flux);
//	//getchar();
//
//	//todo
//	/*following TLEAF in Campbell and Norman "Environmental Biophysics" 1998 pg 225*/

}
