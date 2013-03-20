/*soil_evaporation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_soil_evaporation (SPECIES *const s,  CELL * c, const MET_DATA *const met, int month, int day, int DaysInMonth, float Net_Radiation, int top_layer, int z,
		float Net_Radiation_for_dominated, float Net_Radiation_for_subdominated, int Veg_counter, float daylength)
{
	

	float const e20 = 2.2;          // rate of change of saturated VP with T at 20C
	float const lambda = 2460000;   // latent heat of vapourisation of H2O (J/kg)
	float const PsycConst = 65;      //psychrometer costant
	float const EvapoCoeff = 1.32;        //Priestley Taylor Coefficient
	static float PotEvap;            //Potential evapotranspiration

	Log ("\nGET_SOIL_EVAPORATION_ROUTINE\n\n");



	//todo: a better function that also take into account gaps
	//model uses Net radiation of last height class in last layer * its percentage of light transmitted
	if (settings->time == 'm')
	{
		if (c->monthly_layer_number != 0)
		{
			switch (c->monthly_layer_number)
			{
			case 3:
				if (settings->spatial == 's')
				{
					Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * met[month].ndvi_lai));
				}
				else
				{
					Net_Radiation = Net_Radiation_for_subdominated * (exp(- s->value[K] * s->value[LAI]));
				}
				break;
			case 2:
				if (settings->spatial == 's')
				{
					Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * met[month].ndvi_lai));
				}
				else
				{
					Net_Radiation = Net_Radiation_for_dominated * (exp(- s->value[K] * s->value[LAI]));
				}
				break;
			case 1:
				Log("Net radiation from dominant layer = %g\n", Net_Radiation_for_dominated);
				Net_Radiation = Net_Radiation_for_dominated;
				break;
			}
		}
		else
		{
			Net_Radiation = QA + QB * (met[month].solar_rad * pow (10.0,  6)) / daylength;
		}
	}
	else
	{
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
				}
				break;
			case 1:
				Log("Net radiation from dominant layer = %g W/m^2/hour\n", Net_Radiation_for_dominated);
				Net_Radiation = Net_Radiation_for_dominated;
				break;
			}
		}
		else
		{
			Net_Radiation = QA + QB * (met[month].d[day].solar_rad * pow (10.0,  6)) / daylength;
		}
	}

	PotEvap = (e20 / (e20 + PsycConst )) * Net_Radiation / lambda;
	Log("Net radiation for soil evaporation = %g W/m^2/hour\n", Net_Radiation);

	c->soil_moist_ratio = c->available_soil_water / c->max_asw;

	if (settings->time == 'm')
	{
		c->soil_evaporation = PotEvap * EvapoCoeff * c->soil_moist_ratio * 24 * DaysInMonth;
		Log("Monthly Soil Evaporation = %g \n", c->soil_evaporation );
	}
	else
	{
		c->soil_evaporation = PotEvap * EvapoCoeff * c->soil_moist_ratio * 24 ;
		Log("Daily Soil Evaporation = %g \n", c->soil_evaporation );
	}

	c->total_yearly_soil_evaporation += c->soil_evaporation;
	Log("Total Soil Evaporation = %g mm\n", c->total_yearly_soil_evaporation);

}

