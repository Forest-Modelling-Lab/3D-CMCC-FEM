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
extern logger_t* g_debug_log;

void soil_evaporation(cell_t *const c, const meteo_daily_t *const meteo_daily)
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


	tairK  = meteo_daily->tavg  + TempAbs;
	tsoilK = meteo_daily->tsoil + TempAbs;

	/* correct conductances for temperature and pressure based on Jones (1992)
		with standard conditions assumed to be 20 deg C, 101300 Pa */
	rcorr = 1. / ( pow ( ( meteo_daily->tday + TempAbs ) / 293.15 , 1.75 ) * 101300 / meteo_daily->air_pressure );

	logger(g_debug_log, "\n**SOIL EVAPORATION**\n");

	/* soil evaporation if snowpack = 0 */
	if ( ! c->snow_pack )
	{
		/* new bare-soil evaporation routine */
		/* first calculate potential evaporation, assuming the resistance
		for vapor transport is equal to the resistance for sensible heat
		transport. That is, no additional resistance for vapor transport to
		the soil surface. This represents evaporation from a wet surface with
		a specified aerodynamic resistance (= boundary layer resistance).
		*/

		/*
		note: The aerodynamic resistance is for now set as a constant, and is
		taken from observations over bare soil in tiger-bush in south-west
		Niger: rbl = 107 s m-1 (Wallace and Holwill, 1997).
		*/

		rbl = 107.0 * rcorr;

		rv = rbl;
		rh = rbl;

		/* assign net radiation as local variable */
		//fixme it should net rad
#if 0
		net_rad = c->sw_rad_abs_soil;
#else
		net_rad = c->net_rad_abs_soil;
#endif

		/* calculate pot_evap in kg/m2/s */
		pot_soil_evap = Penman_Monteith ( meteo_daily, rv, rh, net_rad );

		/* covert to daily total kg/m2 */
		pot_soil_evap *= meteo_daily->daylength_sec;

		if ( meteo_daily->rain >= pot_soil_evap )
		{

			/* reset days-since-rain parameter */
			c->days_since_rain  = 0;

			/* soil evaporation proceeds at potential rate */
			c->daily_soil_evapo = 0.6 * pot_soil_evap;
		}
		else
		{
			/* increment the days since rain */
			++c->days_since_rain;

			/* calculate the realized proportion of potential evaporation
			as a function of the days since rain */
			ratio = 0.3 / pow ( c->days_since_rain , 2. );

			/* calculate evaporation for dry days */
			c->daily_soil_evapo = ratio * pot_soil_evap ;

			/* for rain events that are smaller than required to reset days_since_rain
			counter, but larger than dry-day evaporation, all rain is evaporated.
			In this case, do not advance the drying curve counter.
			For rain events that are too small to trigger days_since_rain reset, and which
			are smaller than dry-day evap, there will be more evaporation than
			rainfall. In this case the drying curve counter is advanced. */

			if ( meteo_daily->rain > c->daily_soil_evapo )
			{
				c->daily_soil_evapo = meteo_daily->rain;

				--c->days_since_rain;
			}
		}
	}
	else
	{
		c->daily_soil_evapo = c->daily_snow_subl;
		//todo get functions from snow_melt_subl snow subl (evaporated) or melt (that goes to soil pool) for canopy intercepted snow
	}

	c->monthly_soil_evapo += c->daily_soil_evapo;

	c->annual_soil_evapo += c->daily_soil_evapo;


	/* compute a energy balance evaporation from soil */
	c->daily_soil_evapo_watt       = c->daily_soil_evapo * meteo_daily->lh_vap_soil / 86400.;
	c->daily_soil_lh_flux          = c->daily_soil_evapo_watt;


	//test 9 May 2016 following Maes & Steppe 2012 as in JULES model (Best et al., GMD)
	/* soil sensible heat flux */
	if( ! c->snow_pack )
	{
		//test 11 May 2016 following Webber et al., 2016 as in JULES model (Best et al., GMD)
		/* soil sensible heat flux */
		/* calculate resistance to radiative heat transfer through air, rr */
		rr = meteo_daily->rho_air * CP / ( 4. * SBC_W * ( pow ( tsoilK , 3. ) ) );
		rhr = ( rh * rr )/ ( rh + rr );
		c->daily_soil_sh_flux = meteo_daily->rho_air * CP * ( ( tairK - tsoilK ) / rhr );
	}
	else
	{
		tsnowK = tsnow + TempAbs;
		rr = meteo_daily->rho_air * CP / ( 4. * SBC_W * ( pow ( tsnowK , 3. ) ) );
		/*
		rhr = ???;
		c->daily_soil_sensible_heat_flux = meteo_daily->rho_air * CP * ((tairK-tsnowK)/rhr);
		 */
		c->daily_soil_sh_flux = 0.;
	}
}


void Soil_evaporation_old(cell_t *const c, const meteo_daily_t *const meteo_daily)
{
	double PotEvap;            //Potential evapotranspiration
	double cc = 0.;
	double sat, gamma;

	/*following Priestley and Taylor,1972; Gerten et al., 2004*/
	/**********************************************************/

	logger(g_debug_log, "\nSOIL_EVAPORATION_ROUTINE\n");

	gamma = 65.05 + meteo_daily->tday * 0.064;
	sat   = ( ( 2.503e6 * exp ( ( 17.268 * meteo_daily->tday ) / ( 237.3 + meteo_daily->tday ) ) ) )/
			pow( ( 237.3 + meteo_daily->tday ) , 2. );

	if ( meteo_daily->tsoil > 0. )
	{

		//FIXME SHOULD ADD PART OF NET RAD TRASMITTED THORUGH THE CANOPIES
		/* converting W/m^2 in Joule/m^2/day */
		PotEvap = ( sat / ( sat + gamma ) ) * ( meteo_daily->sw_downward_W * 86400. ) / meteo_daily->lh_vap_soil;

		if( PotEvap < 0. )
		{
			PotEvap = 0.;
		}

		c->soil_moist_ratio = c->asw / c->max_asw_fc;

		/*following Gerten et al., 2004 soil evaporation occurs at the simulated cell not covered by vegetation (e.g. 1-cc)*/
		//note: it shouldn't takes into account CC
		if( cc >= 1 )
		{
			cc = 1;
		}

		c->daily_soil_evapo = ( PotEvap * EVAPOCOEFF * c->soil_moist_ratio * ( 1. - cc ) ) + c->daily_snow_subl;

	}
	else
	{
		c->daily_soil_evapo = 0.;
	}

	c->monthly_soil_evapo += c->daily_soil_evapo;

	c->annual_soil_evapo += c->daily_soil_evapo;

	/*compute a energy balance evaporation from soil*/
	c->daily_soil_evapo_watt = c->daily_soil_evapo * meteo_daily->lh_vap_soil / 86400.;

}

