/*soil_evaporation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"


float Get_soil_evaporation (SPECIES *const s,  CELL * c, const MET_DATA *const met, int month, int DaysInMonth, float Net_Radiation, int top_layer, int z,
		float Net_Radiation_for_dominated, float Net_Radiation_for_subdominated, int Veg_counter, float daylength)
{
	Log ("\n GET_SOIL_EVAPORATION_ROUTINE\n");

	float const e20 = 2.2;          // rate of change of saturated VP with T at 20C
	float const lambda = 2460000;   // latent heat of vapourisation of H2O (J/kg)
	float const PsycConst = 65;      //psychrometer costant
	float const EvapoCoeff = 1.32;        //Priestley Taylor Coefficient
	static float MoistRatio;
	static float PotEvap;            //Potential evapotranspiration


	Log("\nthere is a bug in the Net Radiation that model uses \nit should get not the radiation for this layer but for the lowers\n\n");

	//PotEvap = (e20 / (e20 + PsycConst )) * Net_Radiation / lambda;
	//Log("Net radiation for soil evaporation = %g \n", Net_Radiation);

	//todo: improve see line 34 and 38
	if (c->monthly_layer_number != 0)
	{
		switch (c->monthly_layer_number)
		{
		case 3:
			Net_Radiation = Net_Radiation_for_subdominated;
			Log("Using Net_Radiation_for_dominated \n");
			break;
		case 2:
			Net_Radiation = Net_Radiation_for_subdominated;
			Log("Using Net_Radiation_for_dominated \n");
			break;
		case 1:
			Net_Radiation = Net_Radiation_for_dominated;
			Log("Using Net_Radiation_for_dominated \n");
			break;
		}
	}
	else
	{
		Net_Radiation = QA + QB * (met[month].solar_rad * pow (10.0,  6)) / daylength;
	}





	PotEvap = (e20 / (e20 + PsycConst )) * Net_Radiation / lambda;
	Log("Net radiation for soil evaporation = %g W/m^2/hour\n", Net_Radiation);



	//Log("Hourly Potential Evapotranspiration from LPJ = %g mm H2O/h\n", PotEvap);
	//Log("Daily Potential Evapotranspiration from LPJ = %g mm H2O/day\n", PotEvap * 24 );
	//Log("Monthly Potential Evapotranspiration from LPJ = %g mm H2O/month\n", PotEvap * 24 * DaysInMonth [month]);
	MoistRatio = c->available_soil_water / site->maxAsw;
	//Log("Moist Ratio = %g\n", MoistRatio);
	c->soil_evaporation = PotEvap * EvapoCoeff * MoistRatio * 24 * DaysInMonth;

	return 0.1; // If you define a float returning function, you must return a float!!!

}

