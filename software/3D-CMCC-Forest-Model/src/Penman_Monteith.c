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
#include "types.h"
#include "constants.h"

double Penman_Monteith (const MET_DATA *const met, int month, int day, int rh, int rv, double net_rad)
{
	double tairK;
	double delta;
	double t1,t2,pvs1,pvs2;
	double rr, rhr;
	double dt = 0.2;     /* set the temperature offset for slope calculation */

	double evap_or_transp;



	Log("**Penmon**\n");

	/* assign ta (Celsius) and tk (Kelvins) */
	tairK = met[month].d[day].tday + TempAbs;

	/* calculate resistance to radiative heat transfer through air, rr */
	rr = met[month].d[day].rho_air * CP / (4.0 * SBC * (pow(tairK, 3)));

	/* calculate combined resistance to convective and radiative heat transfer,
	    parallel resistances : rhr = (rh * rr) / (rh + rr) */
	rhr = (rh * rr) / (rh + rr);

	/* calculate temperature offsets for slope estimate */
	t1 = met[month].d[day].tday+dt;
	t2 = met[month].d[day].tday-dt;

	/* calculate saturation vapor pressures at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

	/* calculate slope of pvs vs. T curve, at ta */
	delta = (pvs1-pvs2) / (t1-t2);

	/* latent heat fluxes of evaporation or transpiration W/m2 */
	evap_or_transp = ((delta * net_rad) + (met[month].d[day].rho_air * CP * (met[month].d[day].vpd / 100.0) / rhr)) /
			(((met[month].d[day].air_pressure * CP * rv) / (met[month].d[day].lh_vap * EPS * rhr)) + delta);

	/* evporation or transpiration is converted into kg-mm/m2/sec */
	evap_or_transp /= met[month].d[day].lh_vap;

	/* check */
	if (evap_or_transp < 0.0) evap_or_transp = 0.0;
	Log("Penmon evap_or_transp = %.10f mm/m2/sec\n", evap_or_transp);

	return(evap_or_transp);
}
