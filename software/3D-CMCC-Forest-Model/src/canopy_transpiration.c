/*canopy transpiration.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"


extern void Get_canopy_transpiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month,  int DaysInMonth, float vpd, int height)
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
	//static float MonthTransp;



	float alpha_evapo = 0.65;
	float beta_evapo = 0.95;

	/*Canopy Conductance*/

	//Lai is different among layers so CanCond is different
	//Log("Lai for Can Cond = %g\n", s->value[LAI]);

	if (settings->version == 's')
	{
		CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, met[month].ndvi_lai / s->value[LAIGCX]);
	}
	else
	{
		CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI]  / s->value[LAIGCX]);
	}

	//Log("Canopy Conductance  = %g\n", CanCond);

	/*Canopy Transpiration*/

	// Penman-Monteith equation for computing canopy transpiration
	// in kg/m2/day, which is converted to mm/day.
	// The following are constants in the PM formula (Landsberg & Gower, 1997)
	defTerm = rhoAir * lambda * (VPDconv * vpd) * s->value[BLCOND];
	//Log("defTerm = %g\n", defTerm);
	duv = (1.0 + e20 + s->value[BLCOND] / CanCond);
	//Log("duv = %g\n", duv);

	//10 July 2012
	//net radiation should be takes into account of the ALBEDO effect
	//for APAR the ALBEDO is 1/3 as large for PAR because less PAR is reflected than NetRad
	//see Biome-BGC 4.2 tech report


	/*
	Etransp = (e20 * (c->net_radiation * ( 1 - s->value[MAXALB])) + defTerm) / duv;
	Log("Etransp con ALBEDO = %g J/m^2/sec\n", Etransp);
	Log("NET RADIATION with ALBEDO = %g \n", c->net_radiation * (1 - s->value[MAXALB]));
	 */

	switch (c->monthly_layer_number)
	{
	case 1:
		Etransp = (e20 * c->net_radiation + defTerm) / duv;  // in J/m2/s
		Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
		Log("NET RADIATION = %g \n", c->net_radiation);
		break;
	case 2:
		if ( c->heights[height].z == c->top_layer )
		{
			Etransp = (e20 * c->net_radiation + defTerm) / duv;  // in J/m2/s
			Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
			Log("NET RADIATION = %g \n", c->net_radiation);
		}
		else
		{
			Etransp = (e20 * c->net_radiation_for_dominated + defTerm) / duv;  // in J/m2/s
			Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
			Log("NET RADIATION = %g \n", c->net_radiation);
		}
		break;
	case 3:
		if ( c->heights[height].z == c->top_layer )
		{
			Etransp = (e20 * c->net_radiation + defTerm) / duv;  // in J/m2/s
			Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
			Log("NET RADIATION = %g \n", c->net_radiation);
		}
		if ( c->heights[height].z == c->top_layer - 1 )
		{
			Etransp = (e20 * c->net_radiation_for_dominated + defTerm) / duv;  // in J/m2/s
			Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
			Log("NET RADIATION = %g \n", c->net_radiation);
		}
		else
		{
			Etransp = (e20 * c->net_radiation_for_subdominated + defTerm) / duv;  // in J/m2/s
			Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
			Log("NET RADIATION = %g \n", c->net_radiation);
		}

		break;
	}



	CanopyTranspiration = Etransp / lambda * c->daylength;         // converted to kg-mm H2o/m2/day
	//1Kg m^2 H2o correspond to 1mm H2o
	Log("Daily Canopy Transpiration = %g mm-Kg H2o/m^2/day\n", CanopyTranspiration);
	DailyTransp = CanopyTranspiration;
	Log("Daily Transpiration = %g mm/m^2/day\n", DailyTransp);
	//initial transpiration from Penman-Monteith (mm/day converted to mm/month)
	s->value[MONTH_TRANSP] = CanopyTranspiration * DaysInMonth;
	Log("Monthly Canopy Transpiration = %g mm-Kg H2o/m^2/month\n", s->value[MONTH_TRANSP]);

	//5 october 2012 "simplified evapotranspiration modifier" f(E), Angelo Nolè
	//alpha e beta andranno inserite come specie specifiche!!!!
	/*

	s->value[F_EVAPO] = 1 - exp (- alpha_evapo * pow (c->soil_moist_ratio, beta_evapo));
	Log("ANGELO F_EVAPO = %g \n", s->value[F_EVAPO] );


	s->value[MONTH_TRANSP] *= s->value[F_EVAPO];
	Log("ANGELO MonthTransp = %g \n", s->value[MONTH_TRANSP]);
	*/
}
