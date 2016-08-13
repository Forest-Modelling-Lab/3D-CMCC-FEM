/*
 * Penman_Monteith.c
 *
 *  Created on: 23/mar/2016
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

double Penman_Monteith(const meteo_daily_t *const meteo_daily, const double rh, const double rv, const double net_rad)
{
	double tairK;
	double delta;
	double t1,t2,pvs1,pvs2;
	double rr, rhr;
	double dt = 0.2;     /* set the temperature offset for slope calculation */

	double evap_or_transp;

    /*
	Combination equation for determining evaporation and transpiration.


    INPUTS:
    tday                    (deg C)       air temperature
    air_pressure            (Pa)          air pressure
    vpd                     (Pa)          vapor pressure deficit
    net_rad                 (W/m2)        incident radient flux density
    rv                      (s/m)         resistance to water vapor flux
    rh                      (s/m)         resistance to sensible heat flux
    air_rho                 (kg/m3)       density of air
	CP                      (J/kg/degC)   specific heat of air
    lh_vap                  (J/kg)        latent heat of vaporization of water

    INTERNAL VARIABLES:
    rr                      (s m-1)       resistance to radiative heat transfer through air
    rhr                     (s m-1)       combined resistance to convective and radiative heat transfer
    pvs1, 2                 (Pa)          saturation vapor pressures at t1 and t2
    delta                   (Pa/degC)     slope of saturation vapor pressure vs T curve

    OUTPUTs:
    evap_or_transp          (kg/m2/s)     water vapor mass flux density  (flag=0)
    evap_or_transp          (W/m2)        latent heat flux density       (flag=1)
    */



	logger(g_log, "*Penman-Monteith function*\n");

	/* convert tday Celsius in Kelvin */
	tairK = meteo_daily->tday + TempAbs;

	/* calculate resistance to radiative heat transfer through air, rr */
	rr = meteo_daily->rho_air * CP / (4.0 * SBC_W * (pow(tairK, 3)));

	/* calculate combined resistance to convective and radiative heat transfer,
	    parallel resistances : rhr = (rh * rr) / (rh + rr) */
	rhr = (rh * rr) / (rh + rr);

	/* calculate temperature offsets for slope estimate */
	t1 = meteo_daily->tday+dt;
	t2 = meteo_daily->tday-dt;

	/* calculate saturation vapor pressures (Pa) at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

	/* calculate slope of pvs vs. T curve, at ta */
	delta = (pvs1-pvs2) / (t1-t2);

	/* latent heat fluxes of evaporation or transpiration W/m2 */
	evap_or_transp = ((delta * net_rad) + (meteo_daily->rho_air * CP * (meteo_daily->vpd / 100.0) / rhr)) /
			(((meteo_daily->air_pressure * CP * rv) / (meteo_daily->lh_vap * EPS * rhr)) + delta);

	/* evaporation or transpiration is converted into kg-mm/m2/sec */
	evap_or_transp /= meteo_daily->lh_vap;

	/* check */
	//deleted to avoid dew fall formation
	//if (evap_or_transp < 0.0) evap_or_transp = 0.0;

	logger(g_log, "Penmon evap_or_transp = %g mm/m2/sec\n", evap_or_transp);

	return(evap_or_transp);
}
