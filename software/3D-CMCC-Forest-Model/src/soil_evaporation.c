/*soil_evaporation.c*/

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

	//test check albedo for other typos
	double soil_albedo = 0.20; /* for deciduous Monteith 1973 */
	double net_rad;
	double pot_soil_evap;    /* (kg/m2/s) potential evaporation (daytime) */

	double tairK, tsoilK;

	//test
	double rr;
	double rhr;


	tairK = met[month].d[day].tavg + TempAbs;
	tsoilK = met[month].d[day].tsoil + TempAbs;

	/* correct conductances for temperature and pressure based on Jones (1992)
		with standard conditions assumed to be 20 deg C, 101300 Pa */
	rcorr = 1.0/(pow((met[month].d[day].tday+TempAbs)/293.15, 1.75) * 101300/met[month].d[day].air_pressure);

	Log("\n**SOIL EVAPORATION BIOME**\n");
	Log("snowpack = %f\n", c->snow_pack);

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

		if (c->Veg_Counter >= 1)
		{
			net_rad = c->net_radiation_for_soil * (1.0 - soil_albedo);
			Log("Filtered Net Radiation for soil = %f W/m2\n", net_rad);
		}
		else
		{
			net_rad = c->net_radiation * (1.0 - soil_albedo);
			Log("UN-Filtered Net Radiation for soil = %f W/m2\n", net_rad);
		}


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

			/* calculate the realized proportion of potential evaporation
			as a function of the days since rain */
			ratio = 0.3/pow(c->days_since_rain,2.0);
			Log("ratio = %f \n", ratio);

			/* calculate evaporation for dry days and scaled to cell uncovered*/
			c->daily_soil_evapo = ratio * pot_soil_evap * (1.0 - c->cell_cover);
		}
		/* for rain events that are smaller than required to reset days_since_rain
	counter, but larger than dry-day evaporation, all rain is evaporated.
	In this case, do not advance the drying curve counter.
	For rain events that are too small to trigger days_since_rain reset, and which
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
	Log("Daily Soil Evaporation = %.10f mm/m2/day\n", c->daily_soil_evapo);
	c->monthly_soil_evapo += c->daily_soil_evapo;
	Log("Monthly Soil Evaporation = %f mm/m2/month\n", c->monthly_soil_evapo);
	c->annual_soil_evapo += c->daily_soil_evapo;
	Log("Annual Soil Evaporation = %f mm/m2/year\n", c->annual_soil_evapo);

	/* compute a energy balance evaporation from soil */
	c->daily_soil_evaporation_watt = c->daily_soil_evapo * met[month].d[day].lh_vap_soil / 86400.0;
	Log("Daily Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_evaporation_watt);


	//test 11 May 2016 following Webber et al., 2016 as in JULES model (Best et al., GMD)
	/* soil sensible heat flux */
	/* calculate resistance to radiative heat transfer through air, rr */
	rr = met[month].d[day].rho_air * CP / (4.0 * SBC * (pow(tsoilK, 3)));
	rhr = (rh * rr)/ (rh + rr);

	//test 9 May 2016 following Maes & Steppe 2012 as in JULES model (Best et al., GMD)
	/* soil sensible heat flux */
	if(c->snow_pack == 0)
	{
		c->daily_soil_sensible_heat_flux = met[month].d[day].rho_air * CP * ((tairK-tsoilK)/rhr);
	}
	else
	{
		//todo compute sensible heat flux from snow
		c->daily_soil_sensible_heat_flux = 0.0;
	}
	Log("Daily soil_sensible_heat flux = %f W/m^2\n", c->daily_soil_sensible_heat_flux);
	if(c->daily_soil_sensible_heat_flux > 0.0)getchar();

}


extern void Soil_evaporation (CELL * c, const MET_DATA *const met, int month, int day)
{
	static double PotEvap;            //Potential evapotranspiration
	double cc;
	double sat, gamma;
	Log ("\nSOIL_EVAPORATION_ROUTINE\n");

	gamma = 65.05+met[month].d[day].tday*0.064;
	sat = ((2.503e6 * exp((17.268*met[month].d[day].tday)/(237.3+met[month].d[day].tday))))/
			pow((237.3+met[month].d[day].tday),2);

	Log("T_soil = %f\n", met[month].d[day].tsoil);
	if (met[month].d[day].tsoil > 0)
	{

		//fixme HOW COMPUTE A CUMULATIVE CANOPY COVER AMONG ALLO CLASSES!!
		/*following Gerten et al., 2004*/
		if (c->daily_layer_number != 0)
		{
			switch (c->daily_layer_number)
			{
			case 3:
				if (settings->spatial == 's')
				{
					//Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * met[month].d[day].ndvi_lai));
				}
				else
				{
					cc = c->canopy_cover_subdominated;
				}
				break;
			case 2:
				if (settings->spatial == 's')
				{
					//Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * met[month].d[day].ndvi_lai));
				}
				else
				{
					cc = c->canopy_cover_dominated;
				}
				break;
			case 1:
				//Net_Radiation = Net_Radiation_for_dominated;
				cc = c->canopy_cover_dominant;
				break;
			}
		}
		else
		{
			//Log("ONLY ONE LAYER\n");
			cc = c->canopy_cover_dominant;

		}

		//FIXME SHOULD ADD PART OF NET RAD TRASMITTED THORUGH THE CANOPIES
		//converting W/m^2 in Joule/m^2/day
		PotEvap = (sat / (sat + gamma )) * (c->net_radiation * 86400) / met[month].d[day].lh_vap_soil;
		Log("Soil Potential Evaporation = %f mm+Kg/day\n", PotEvap);
		if(PotEvap <0)
		{
			PotEvap = 0;
		}

		c->soil_moist_ratio = c->asw / c->max_asw_fc;
		//Log("Soil moisture = %f %\n", c->soil_moist_ratio );

		/*following Gerten et al., 2004 soil evaporation occurs at the simulated cell not covered by vegetation (e.g. 1-cc)*/
		if(cc>=1)
		{
			cc = 1;
		}
		c->daily_soil_evapo = (PotEvap * EVAPOCOEFF * c->soil_moist_ratio * (1-cc)) + c->snow_subl;
		Log("Daily Soil Evaporation = %fmm/day \n", c->daily_soil_evapo );
	}
	else
	{
		Log("\n");
		c->daily_soil_evapo = 0;
	}

	c->monthly_soil_evapo += c->daily_soil_evapo;
	Log("Monthly Soil Evaporation = %f mm/month\n", c->monthly_soil_evapo);
	c->annual_soil_evapo += c->daily_soil_evapo;
	Log("Annual Soil Evaporation = %f mm/year\n", c->annual_soil_evapo);

	/*compute a energy balance evaporation from soil*/
	c->daily_soil_evaporation_watt = c->daily_soil_evapo * met[month].d[day].lh_vap_soil / 86400.0;
	Log("Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_evaporation_watt);

}

