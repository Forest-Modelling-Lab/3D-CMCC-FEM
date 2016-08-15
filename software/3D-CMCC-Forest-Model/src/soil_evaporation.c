/* soil_evaporation.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "soil_evaporation.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "Penman_Monteith.h"

//extern settings_t* g_settings;
extern logger_t* g_log;

void Soil_evaporation(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double rbl;              /* (m/s) boundary layer resistance */
	double rcorr;            /* correction factor for temp and pressure */
	double ratio;            /* actual/potential evaporation for dry day */
	double rv, rh;
	double net_rad;
	double pot_soil_evap;    /* (kg/m2/s) potential evaporation (daytime) */

	double tairK, tsoilK;

	double tsnow = 0.0, tsnowK;

	double rr;
	double rhr;


	tairK = meteo_daily->tavg + TempAbs;
	tsoilK = meteo_daily->tsoil + TempAbs;

	/* correct conductances for temperature and pressure based on Jones (1992)
		with standard conditions assumed to be 20 deg C, 101300 Pa */
	rcorr = 1.0/(pow((meteo_daily->tday+TempAbs)/293.15, 1.75) * 101300/meteo_daily->air_pressure);

	logger(g_log, "\n**SOIL EVAPORATION BIOME**\n");
	logger(g_log, "snowpack = %f\n", c->snow_pack);

	/* soil evaporation if snowpack = 0 */
	if (c->snow_pack == 0.0)
	{
		/* new bare-soil evaporation routine */
		/* first calculate potential evaporation, assuming the resistance
		for vapor transport is equal to the resistance for sensible heat
		transport.  That is, no additional resistance for vapor transport to
		the soil surface. This represents evaporation from a wet surface with
		a specified aerodynamic resistance (= boundary layer resistance).
		The aerodynamic resistance is for now set as a constant, and is
		taken from observations over bare soil in tiger-bush in south-west
		Niger: rbl = 107 s m-1 (Wallace and Holwill, 1997). */

		rbl = 107.0 * rcorr;
		rv = rbl;
		rh = rbl;

		/* assign net radiation as local variable */
		net_rad = c->sw_rad_for_soil;
		logger(g_log, "net_sw_rad_for_soil = %.10f W/m2\n", c->sw_rad_for_soil);

		/* calculate pot_evap in kg/m2/s */
		pot_soil_evap = Penman_Monteith (meteo_daily, rv, rh, net_rad);
		logger(g_log, "Potential soil evaporation = %.10f mm/m2/sec\n", pot_soil_evap);

		/* covert to daily total kg/m2 */
		pot_soil_evap *= (meteo_daily->daylength * 3600.0);

		if ( c->rain_to_soil >= pot_soil_evap )
		{
			/* reset days-since-rain parameter */
			c->days_since_rain = 0.0;

			/* soil evaporation proceeds at potential rate */
			c->daily_soil_evapo = 0.6 * pot_soil_evap;
		}
		else
		{
			/* increment the days since rain */
			c->days_since_rain += 1.0;

			/* calculate the realized proportion of potential evaporation
			as a function of the days since rain */
			ratio = 0.3/pow(c->days_since_rain,2.0);
			logger(g_log, "ratio = %f \n", ratio);

			/* calculate evaporation for dry days */
			c->daily_soil_evapo = ratio * pot_soil_evap ;
		}
		/* for rain events that are smaller than required to reset days_since_rain
			counter, but larger than dry-day evaporation, all rain is evaporated.
		In this case, do not advance the drying curve counter.
		For rain events that are too small to trigger days_since_rain reset, and which
		are smaller than dry-day evap, there will be more evaporation than
		rainfall. In this case the drying curve counter is advanced. */

		if ( c->rain_to_soil >c->daily_soil_evapo )
		{
			c->daily_soil_evapo = c->rain_to_soil;
			--c->days_since_rain;
		}
	}
	else
	{
		c->daily_soil_evapo = 0.0;
		//todo get functions from snow_melt_subl snow subl (evaporated) or melt (that goes to soil pool) for canopy intercepted snow
	}

	//TODO TODO TODO add snow sublimation to soil evaporation!!!!!!!!!!!!!

	logger(g_log, "day(s) since rain = %f day(s)\n", c->days_since_rain);
	logger(g_log, "Daily Soil Evaporation = %.10f mm/m2/day\n", c->daily_soil_evapo);
	c->monthly_soil_evapo += c->daily_soil_evapo;
	logger(g_log, "Monthly Soil Evaporation = %f mm/m2/month\n", c->monthly_soil_evapo);
	c->annual_soil_evapo += c->daily_soil_evapo;
	logger(g_log, "Annual Soil Evaporation = %f mm/m2/year\n", c->annual_soil_evapo);

	/* compute a energy balance evaporation from soil */
	c->daily_soil_evaporation_watt = c->daily_soil_evapo * meteo_daily->lh_vap_soil / 86400.0;
	c->daily_soil_latent_heat_flux = c->daily_soil_evaporation_watt;
	logger(g_log, "Daily Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_latent_heat_flux);

	//test 9 May 2016 following Maes & Steppe 2012 as in JULES model (Best et al., GMD)
	/* soil sensible heat flux */
	if(c->snow_pack == 0)
	{
		//test 11 May 2016 following Webber et al., 2016 as in JULES model (Best et al., GMD)
		/* soil sensible heat flux */
		/* calculate resistance to radiative heat transfer through air, rr */
		rr = meteo_daily->rho_air * CP / (4.0 * SBC_W * (pow(tsoilK, 3)));
		rhr = (rh * rr)/ (rh + rr);
		c->daily_soil_sensible_heat_flux = meteo_daily->rho_air * CP * ((tairK-tsoilK)/rhr);
	}
	else
	{
		tsnowK = tsnow + TempAbs;
		rr = meteo_daily->rho_air * CP / (4.0 * SBC_W * (pow(tsnowK, 3)));
		/*
		rhr = ???;
		c->daily_soil_sensible_heat_flux = meteo_daily->rho_air * CP * ((tairK-tsnowK)/rhr);
		 */
		c->daily_soil_sensible_heat_flux = 0.0;
	}
	logger(g_log, "Daily soil_sensible_heat flux = %f W/m^2\n", c->daily_soil_sensible_heat_flux);
}


void Soil_evaporation_old(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	static double PotEvap;            //Potential evapotranspiration
	double cc = 0.;
	double sat, gamma;

	/*following Priestley and Taylor,1972; Gerten et al., 2004*/
	/**********************************************************/

	logger(g_log, "\nSOIL_EVAPORATION_ROUTINE\n");

	gamma = 65.05+meteo_daily->tday*0.064;
	sat = ((2.503e6 * exp((17.268*meteo_daily->tday)/(237.3+meteo_daily->tday))))/
			pow((237.3+meteo_daily->tday),2);

	logger(g_log, "T_soil = %f\n", meteo_daily->tsoil);
	if (meteo_daily->tsoil > 0)
	{

		//FIXME SHOULD ADD PART OF NET RAD TRASMITTED THORUGH THE CANOPIES
		/* converting W/m^2 in Joule/m^2/day */
		PotEvap = (sat / (sat + gamma )) * (c->sw_rad_for_soil * 86400) / meteo_daily->lh_vap_soil;
		logger(g_log, "Soil Potential Evaporation = %f mm+Kg/day\n", PotEvap);
		if(PotEvap <0)
		{
			PotEvap = 0;
		}

		c->soil_moist_ratio = c->asw / c->max_asw_fc;
		logger(g_log, "Soil moisture = %f %\n", c->soil_moist_ratio );

		/*following Gerten et al., 2004 soil evaporation occurs at the simulated cell not covered by vegetation (e.g. 1-cc)*/
		if( cc >= 1 )
		{
			cc = 1;
		}

		c->daily_soil_evapo = (PotEvap * EVAPOCOEFF * c->soil_moist_ratio * (1-cc)) + c->snow_subl;
		logger(g_log, "Daily Soil Evaporation = %fmm/day \n", c->daily_soil_evapo );
	}
	else
	{
		logger(g_log, "\n");
		c->daily_soil_evapo = 0;
	}

	c->monthly_soil_evapo += c->daily_soil_evapo;
	logger(g_log, "Monthly Soil Evaporation = %f mm/month\n", c->monthly_soil_evapo);
	c->annual_soil_evapo += c->daily_soil_evapo;
	logger(g_log, "Annual Soil Evaporation = %f mm/year\n", c->annual_soil_evapo);

	/*compute a energy balance evaporation from soil*/
	c->daily_soil_evaporation_watt = c->daily_soil_evapo * meteo_daily->lh_vap_soil / 86400.0;
	logger(g_log, "Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_evaporation_watt);

}

