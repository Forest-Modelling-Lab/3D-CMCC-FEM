/*canopy transpiration.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"


float Get_canopy_transpiration (SPECIES *const s,  const MET_DATA *const met, int month, float daylength, int DaysInMonth, float vpd, float Net_Radiation)
{
	Log("\n GET_CANOPY_TRANSPIRATION_ROUTINE \n");

	static float CanCond;
	static float CanopyTranspiration;
	float const e20 = 2.2;          // rate of change of saturated VPD with T at 20C
	float const rhoAir = 1.2;       // density of air, kg/m3
	float const lambda = 2460000;   // latent heat of vapourisation of H2O (J/kg)
	float const VPDconv = 0.000622; // convert VPD to saturation deficit = 18/29/1000
	static float defTerm;
	static float duv;                      // 'div' in 3pg
	static float Etransp;
	static float DailyTransp;
	static float MonthTransp;

	/*Canopy Conductance*/

	//Lai is different among layers so CanCond is different
	//Log("Lai for Can Cond = %g\n", s->value[LAI]);

	CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, met[month].lai / s->value[LAIGCX]);

	//Log("Canopy Conductance  = %g\n", CanCond);

	/*Canopy Traspiration*/

	// Penman-Monteith equation for computing canopy transpiration
	// in kg/m2/day, which is converted to mm/day.
	// The following are constants in the PM formula (Landsberg & Gower, 1997)
	defTerm = rhoAir * lambda * (VPDconv * vpd) * s->value[BLCOND];
	//Log("defTerm = %g\n", defTerm);
	duv = (1.0 + e20 + s->value[BLCOND] / CanCond);
	//Log("duv = %g\n", duv);

	//10 july 2012
	//net radiation should be takes into account of the ALBEDO effect
	//for APAR the ALBEDO is 1/3 as large for PAR because less PAR is reflected than NetRad
	//see Biome-BGC 4.2 tech report
	Etransp = (e20 * (Net_Radiation * ( 1 - s->value[MAXALB])) + defTerm) / duv;
	Log("Etransp con ALBEDO = %g J/m^2/sec\n", Etransp);
	Log("NET RADIATION with ALBEDO = %g \n", Net_Radiation * (1 - s->value[MAXALB]));

	Etransp = (e20 * Net_Radiation + defTerm) / duv;  // in J/m2/s
	Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
	Log("NET RADIATION = %g \n", Net_Radiation);

	CanopyTranspiration = Etransp / lambda * daylength;         // converted to kg-mm H2o/m2/day
	//1Kg m^2 H2o correspond to 1mm H2o
	Log("Daily Canopy Transpiration = %g mm-Kg H2o/m^2/day\n", CanopyTranspiration);
	DailyTransp = CanopyTranspiration;
	Log("Daily Transpiration = %g mm/m^2/day\n", DailyTransp);
	//initial transpiration from Penman-Monteith (mm/day converted to mm/month)
	MonthTransp = CanopyTranspiration * DaysInMonth;
	Log("Monthly Canopy Transpiration = %g mm-Kg H2o/m^2/month\n", MonthTransp);





	return MonthTransp;
}
