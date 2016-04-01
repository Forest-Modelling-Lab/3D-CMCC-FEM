/*
 * soil_evapo_biome.c
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

void soil_evaporation_biome (CELL *const c, const MET_DATA *const met, int month, int day)
{
	double rbl;              /* (m/s) boundary layer resistance */
	double rcorr;            /* correction factor for temp and pressure */
	double ratio;            /* actual/potential evaporation for dry day */
	double rv, rh;

	double net_rad;
	double pot_soil_evap;    /* (kg/m2/s) potential evaporation (daytime) */

	Log("\n**SOIL EVAPORATION BIOME**\n");

	if (c->snow_pack != 0.0)
	{
		/* correct conductances for temperature and pressure based on Jones (1992)
	with standard conditions assumed to be 20 deg C, 101300 Pa */
		rcorr = 1.0/(pow((met[month].d[day].tday+273.15)/293.15, 1.75) * 101300/c->air_pressure);

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

		if (c->Veg_Counter >= 1)
		{
			net_rad = c->net_radiation_for_soil;
		}
		else
		{
			net_rad = c->net_radiation;
		}
		Log("Net Radiation for soil = %f W/m2\n", net_rad);

		/* calculate pot_evap in kg/m2/s */
		pot_soil_evap = Penman_Monteith (met, month, day, rv, rh, net_rad);
		Log("Potential soil evaporation = %.10f mm/m2/sec\n", pot_soil_evap);

		/* covert to daily total kg/m2 */
		pot_soil_evap *= (met[month].d[day].daylength * 3600.0);

		if (c->prcp_rain >= pot_soil_evap)
		{
			/* reset days-since-rain parameter */
			c->days_since_rain = 0.0;

			/* soil evaporation proceeds at potential rate  and scaled to cell uncovered*/
			c->daily_soil_evapo = 0.6 * pot_soil_evap * (1.0 - c->cell_cover);
		}
		else
		{
			/* increment the days since rain */
			c->days_since_rain += 1.0;
			Log("day(s) since rain = %f day(s)\n", c->days_since_rain);

			/* calculate the realized proportion of potential evaporation
		as a function of the days since rain */
			ratio = 0.3/pow(c->days_since_rain,2.0);
			Log("ratio = %f \n", ratio);

			/* calculate evaporation for dry days and scaled to cell uncovered*/
			c->daily_soil_evapo = ratio * pot_soil_evap * (1.0 - c->cell_cover);
		}
		/* for rain events that are smaller than required to reset dsr
	counter, but larger than dry-day evaporation, all rain is evaporated.
	In this case, do not advance the drying curve counter.
	For rain events that are too small to trigger dsr reset, and which
	are smaller than dry-day evap, there will be more evaporation than
	rainfall.  In this case the drying curve counter is advanced. */
		if (c->prcp_rain >c->daily_soil_evapo && c->days_since_rain >= 1.0)
		{
			c->daily_soil_evapo = c->prcp_rain * (1.0 - c->cell_cover);
			c->days_since_rain -= 1.0;

		}
	}
	else
	{
		c->daily_soil_evapo = 0.0;
	}

	Log("day(s) since rain = %f day(s)\n", c->days_since_rain);
	Log("Daily Soil Evaporation = %.10f mm/m2/day\n", c->monthly_soil_evapo);
	c->monthly_soil_evapo += c->daily_soil_evapo;
	Log("Monthly Soil Evaporation = %f mm/m2/month\n", c->monthly_soil_evapo);
	c->annual_soil_evapo += c->daily_soil_evapo;
	Log("Annual Soil Evaporation = %f mm/m2/year\n", c->annual_soil_evapo);

	/*compute a energy balance evaporation from soil*/
	c->daily_soil_evaporation_watt = c->daily_soil_evapo * c->lh_vap_soil / 86400.0;
	Log("Daily Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_evaporation_watt);
}
