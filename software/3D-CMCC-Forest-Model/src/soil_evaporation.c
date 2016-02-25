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
	double lhvap_soil; //J Kg^-1
	Log ("\nGET_SOIL_EVAPORATION_ROUTINE\n");
	Log("T_soil = %f\n", met[month].d[day].tsoil);
	if (met[month].d[day].tsoil > 0)
	{
		/*computing latent heat following BIOME approach*/
		lhvap_soil = 2.5023e6 - 2430.54 * met[month].d[day].tsoil;
		Log("lhvap_soil = %f\n", lhvap_soil);

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
					//Net_Radiation = Net_Radiation_for_subdominated * (exp(- s->value[K] * s->value[LAI]));
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
					//Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * s->value[LAI]));
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
			Log("ONLY ONE LAYER\n");
			//Net_Radiation = (QA + QB * (met[month].d[day].solar_rad * pow (10.0, 6))) / met[month].d[day].daylength;
			cc = c->canopy_cover_dominant;

		}

		//converting MJ in Joule/m^2/day
		Net_Radiation = (met[month].d[day].solar_rad * pow (10.0, 6));
		Log("Net radiation for soil evaporation = %f J/m^2/day\n", Net_Radiation);

		PotEvap = (E20 / (E20 + PSYCCONST )) * Net_Radiation / lhvap_soil;
		Log("Potential Evaporation = %f mm/day\n", PotEvap);
		Log("fraction of cell not covered by vegetation = %f\n", 1-cc);

		c->soil_moist_ratio = c->available_soil_water / c->max_asw;
		Log("Soil moisture = %f %\n", c->soil_moist_ratio );

		/*following Gerten et al., 2004 soil evaporation occurs at the simulated cell not covered by vegetation (e.g. 1-cc)*/
		if(cc>=1)
		{
			cc = 1;
		}
		c->soil_evaporation = (PotEvap * EVAPOCOEFF * c->soil_moist_ratio * (1-cc)) + c->snow_subl;
		Log("Daily Soil Evaporation = %fmm/day \n", c->soil_evaporation );
	}
	else
	{
		c->soil_evaporation = 0;
	}

	c->total_yearly_soil_evaporation += c->soil_evaporation;
	Log("Total Yearly Soil Evaporation = %f mm\n", c->total_yearly_soil_evaporation);

}

