/*soil_evaporation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_soil_evaporation (CELL * c, const MET_DATA *const met, int month, int day)
{
	static double PotEvap;            //Potential evapotranspiration
	double cc;
	Log ("\nGET_SOIL_EVAPORATION_ROUTINE\n");
	//Log("T_soil = %f\n", met[month].d[day].tsoil);
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

		//converting W/m^2 in Joule/m^2/day
		PotEvap = (E20 / (E20 + PSYCCONST )) * (c->net_radiation * 86400) / c->lh_vap_soil;
		Log("Soil Potential Evaporation = %f mm+Kg/day\n", PotEvap);

		c->soil_moist_ratio = c->available_soil_water / c->max_asw;
		//Log("Soil moisture = %f %\n", c->soil_moist_ratio );

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
		Log("\n");
		c->soil_evaporation = 0;
	}

	c->total_yearly_soil_evaporation += c->soil_evaporation;
	Log("Total Yearly Soil Evaporation = %f mm\n", c->total_yearly_soil_evaporation);

	/*compute a energy balance evaporation from soil*/
	c->daily_soil_evaporation_watt = c->soil_evaporation * c->lh_vap_soil / 86400;
	Log("Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_evaporation_watt);

}

