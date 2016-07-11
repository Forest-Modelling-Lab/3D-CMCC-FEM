/* heat_fluxes.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "heat_fluxes.h"
#include "constants.h"
#include "logger.h"


extern logger_t* g_log;

void Canopy_latent_heat_fluxes (species_t *const s, const meteo_t *const met, const int month, const int day)
{
	/* canopy canopy level latent heat fluxes (W/m2) */
	s->value[CANOPY_LATENT_HEAT] = s->value[CANOPY_EVAPO_TRANSP] * met[month].d[day].lh_vap / 86400;
	logger(g_log, "CANOPY LATENT HEAT FLUX = %g W/m2\n", s->value[CANOPY_LATENT_HEAT]);
}

void Canopy_sensible_heat_fluxes (cell_t *const c, species_t *const s, const meteo_t *const met, const int month, const int day)
{
	double TairK;
	double TcanopyK;
	double g_corr;
	double gl_bl;
	double gl_sh;
	double gc_sh;
	double rh;
	double rr;
	double rhr;

	TairK = met[month].d[day].tavg + TempAbs;
	TcanopyK = TairK;
	if(s->value[LAI] > 0.0)
	{

		/* temperature and pressure correction factor for conductances */
		g_corr = pow((met[month].d[day].tday+TempAbs)/293.15, 1.75) * 101300/met[month].d[day].air_pressure;

		/* leaf boundary-layer conductance */
		gl_bl = s->value[BLCOND] * g_corr;

		/* Leaf conductance to sensible heat, per unit all-sided LAI */
		gl_sh = gl_bl;

		/* Canopy conductance to sensible heat */
		gc_sh = gl_sh * s->value[LAI];

		/* Canopy resistance to sensible heat */
		rh = 1.0/gc_sh;

		/* calculate resistance to radiative heat transfer through air, rr */
		rr = met[month].d[day].rho_air * CP / (4.0 * SBC_W * (pow(TairK, 3)));

		rhr = (rh * rr) / (rh + rr);

		s->value[CANOPY_SENSIBLE_HEAT] = met[month].d[day].rho_air * CP * ((TcanopyK-TairK)/rhr);

		c->daily_c_sensible_heat_flux += s->value[CANOPY_SENSIBLE_HEAT];

	}

	/* following TLEAF in MAESPA model (physiol.f90, row 197) check for consistency in units */

	//		//TEST
	//		/* CANOPY SENSIBLE HEAT FLUX */
	//		//logger(g_log, "\ncanopy sensible heat\n");
	//
	//		//logger(g_log, "LAI = %g\n", s->value[LAI]);
	//
	//		net_rad = s->value[NET_SW_RAD_ABS];
	//		//logger(g_log, "net rad = %g\n", net_rad);
	//
	//		/* FIRST OF ALL COMPUTE CANOPY TEMPERATURE */
	//		/* calculate temperature offsets for slope estimate */
	//		t1 = met[month].d[day].tday+dt;
	//		t2 = met[month].d[day].tday-dt;
	//
	//		/* calculate saturation vapor pressures (Pa) at t1 and t2 */
	//		pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	//		//logger(g_log, "pvs1 = %g\n", pvs1);
	//		pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));
	//		//logger(g_log, "pvs2 = %g\n", pvs2);
	//
	//		/* calculate slope of pvs vs. T curve, at ta */
	//		//test this is the "DELTA" function as in Webber et al., 2016
	//		delta = (pvs1-pvs2) / (t1-t2);
	//		/* converts into kPA following Webber et al., 2016 */
	//		delta /= 1000.0;
	//		//logger(g_log, "delta = %g KPa\n", delta);
	//
	//		//test
	//		// as in Ryder et al., 2016 resistance to sensible heat flux is equal to boundary layer resistance (see also BIOME)
	//		// so it should be rh (1/gl_sh)
	//
	//		/* canopy resistance m sec-1)*/
	//		//fixme gl_sh or gc_sh? Wang and Leuning 1998 use stomatal conductance
	//		//fixme this is valid for cell level not for class level
	//		rc = 1.0/gc_sh;
	//
	//		//test this is the equivalent "ra" aerodynamic resistance as in Allen et al., 1998
	//		/* calculate resistance to radiative heat transfer through air, rr */
	//		rr = met[month].d[day].rho_air * CP / (4.0 * SBC_W * (pow(tairK, 3)));
	//		rhr = (rh * rr) / (rh + rr);
	//		/* compute product as psychrometric constant and (1+(rc/ra)) see Webber et al., 2016 */
	//		/* then ra = rhr */
	//		psych_p = met[month].d[day].psych *(1+(rc/rhr));
	//		//logger(g_log, "psych_p = %g\n", psych_p);
	//
	//		//test to avoid problems using generic daily data we should divide the fluxes into diurnal (using tday) and nocturnal (using tnight) (but for net_rad???)
	//		//to have tcanopy_day and tcanopy_night considering day length
	//
	//		//fixme conductance and resistance variables following Norman And Campbell should be in mol m sec (not m sec)
	//		//test convert boundary layer conductance from m/sec into mol m/sec
	//		//following Pearcy, Schulze and Zimmermann
	//		rh_mol = rh * 0.0446 * ((TempAbs / (met[month].d[day].tavg+TempAbs))*((met[month].d[day].air_pressure/1000.0)/101.3));
	//		//logger(g_log, "boundary layer resistance = %g mol m/sec\n", rh_mol);
	//
	//		/* canopy temperature as in Webber et al., 2016 it takes rh in m/sec*/
	//		/*tcanopy = met[month].d[day].tavg + ((net_rad * rh_mol)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);*/
	//
	//		//test 07 june 2016 using only boundary layer resistance as in Ryder et al., 2016
	//		//tcanopy = met[month].d[day].tavg + ((net_rad * rh)/(CP*met[month].d[day].rho_air))*(psych_p/(delta*psych_p))- ((1.0-met[month].d[day].rh_f)/delta +psych_p);
	//
	//		tcanopyK = tcanopy + TempAbs;
	//		//logger(g_log, "canopy_temp = %g K\n", tcanopyK);
	//		//logger(g_log, "tairK = %g K\n", tairK);
	//
	//		//fixme this is valid for cell level not for class level
	//		c->daily_canopy_sensible_heat_flux = met[month].d[day].rho_air * CP * ((tcanopyK-tairK)/rhr);
	//		//logger(g_log, "canopy_sensible_heat_flux = %g Wm-2\n", c->daily_canopy_sensible_heat_flux);
	//
	//		//todo
	//		/*following TLEAF in Campbell and Norman "Environmental Biophysics" 1998 pg 225*/

}

void Latent_heat_flux (cell_t *const c, const meteo_t *const met, const int month, const int day)
{
	logger(g_log, "\nLATENT_HEAT_ROUTINE\n");

	/*compute latent heat from canopy*/
	c->daily_c_evapo_watt = c->daily_c_evapo * met[month].d[day].lh_vap / 86400.0;
	c->daily_c_transp_watt = c->daily_c_transp * met[month].d[day].lh_vap / 86400.0;
	c->daily_c_evapotransp_watt = c->daily_c_evapo_watt + c->daily_c_transp_watt;
	c->daily_c_latent_heat_flux = c->daily_c_evapotransp_watt;
	logger(g_log, "Latent heat canopy evapotranspiration = %g W/m^2\n", c->daily_c_latent_heat_flux);
	logger(g_log, "Latent heat soil evaporation = %g W/m^2\n", c->daily_soil_evaporation_watt);

	/* adding soil latent heat flux to overall latent heat flux */
	c->daily_latent_heat_flux = c->daily_c_evapotransp_watt + c->daily_soil_evaporation_watt;

	/*in case of snow formation or dew fall*/
	if(c->prcp_snow != 0.0)
	{
		logger(g_log, "implement negative heat fluxes!\n");
	}
	/*in case of snow sublimation*/
	if(c->snow_subl != 0.0)
	{
		c->daily_latent_heat_flux += c->snow_subl * (met[month].d[day].lh_sub * 1000.0) / 86400.0;
		logger(g_log, "Daily total latent heat flux with sublimation = %g W/m\n", c->daily_latent_heat_flux);
	}
	else
	{
		logger(g_log, "Daily total latent heat flux = %g W/m\n", c->daily_latent_heat_flux);
	}

	c->monthly_latent_heat_flux += c->daily_latent_heat_flux;
	c->annual_latent_heat_flux += c->daily_latent_heat_flux;

}

void Sensible_heat_flux (cell_t *const c, const meteo_t *const met, const int month, const int day)
{
	logger(g_log, "\nSENSIBLE_HEAT_ROUTINE\n");

	c->daily_sensible_heat_flux = c->daily_c_sensible_heat_flux + c->daily_soil_sensible_heat_flux;
	logger(g_log, "Daily sensible heat flux = %g W/m\n", c->daily_sensible_heat_flux);

	c->monthly_sensible_heat_flux += c->daily_latent_heat_flux;
	c->annual_sensible_heat_flux += c->daily_latent_heat_flux;
}
