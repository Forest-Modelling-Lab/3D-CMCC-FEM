/*soil_evaporation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


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
		PotEvap = (sat / (sat + gamma )) * (c->net_radiation * 86400) / c->lh_vap_soil;
		Log("Soil Potential Evaporation = %f mm+Kg/day\n", PotEvap);
		if(PotEvap <0)
		{
			PotEvap = 0;
		}

		c->soil_moist_ratio = c->asw / c->max_asw;
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
	c->daily_soil_evaporation_watt = c->daily_soil_evapo * c->lh_vap_soil / 86400.0;
	Log("Latent heat soil evaporation = %f W/m^2\n", c->daily_soil_evaporation_watt);

}

