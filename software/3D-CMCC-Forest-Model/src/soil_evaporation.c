/*soil_evaporation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_soil_evaporation (SPECIES *const s,  CELL * c, const MET_DATA *const met, int month, int day, int DaysInMonth, double Net_Radiation, int top_layer, int z,
		double Net_Radiation_for_dominated, double Net_Radiation_for_subdominated, int Veg_counter)
{
	static double PotEvap;            //Potential evapotranspiration
	double cc;
	double lhvap;
	Log ("\nGET_SOIL_EVAPORATION_ROUTINE\n");

	/*computing latent heat following BIOME approach*/
	lhvap = 2.5023e6 - 2430.54 * met[month].d[day].tday;

	/*following Gerten et al., 2004*/

	//todo: a better function that also take into account gaps
	//model uses Net radiation of last height class in last layer * its percentage of light transmitted
	if (c->daily_layer_number != 0)
	{
		switch (c->daily_layer_number)
		{
		case 3:
			if (settings->spatial == 's')
			{
				Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * met[month].d[day].ndvi_lai));
			}
			else
			{
				Net_Radiation = Net_Radiation_for_subdominated * (exp(- s->value[K] * s->value[LAI]));
				cc = c->canopy_cover_subdominated;

			}
			break;
		case 2:
			if (settings->spatial == 's')
			{
				Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * met[month].d[day].ndvi_lai));
			}
			else
			{
				Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * s->value[LAI]));
				cc = c->canopy_cover_dominated;

			}
			break;
		case 1:
			Log("Net radiation from dominant layer = %f W/m^2/hour\n", Net_Radiation_for_dominated);
			Net_Radiation = Net_Radiation_for_dominated;
			cc = c->canopy_cover_dominant;

			break;
		}
	}
	else
	{
		Net_Radiation = (QA + QB * (met[month].d[day].solar_rad * pow (10.0,  6))) / met[month].d[day].daylength;
		cc = c->canopy_cover_dominant;

	}


	PotEvap = (E20 / (E20 + PSYCCONST )) * (Net_Radiation/3600.0) / lhvap;
	Log("Net radiation for soil evaporation = %f W/m^2/hour\n", Net_Radiation);
	//Log("Net radiation for soil evaporation = %f W/m^2/sec\n", Net_Radiation/3600.0);

	c->soil_moist_ratio = c->available_soil_water / c->max_asw;


	c->soil_evaporation = (PotEvap * EVAPOCOEFF * c->soil_moist_ratio * (1-cc) * met[month].d[day].daylength * 3600.0) + c->snow_subl;

	//c->soil_evaporation = (PotEvap * EVAPOCOEFF * c->soil_moist_ratio * (met[month].d[day].daylength * 3600.0)) + c->snow_subl;
	Log("Daily Soil Evaporation = %fmm \n", c->soil_evaporation );


	c->total_yearly_soil_evaporation += c->soil_evaporation;
	//Log("Total Yearly Soil Evaporation = %f mm\n", c->total_yearly_soil_evaporation);

}

