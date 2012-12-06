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
	float rhoAir;       // density of air, kg/m3
	float const lambda = 2460000;   // latent heat of vapourisation of H2O (J/kg)
	float const VPDconv = 0.000622; // convert VPD to saturation deficit = 18/29/1000
	static float defTerm;
	static float duv;                      // 'div' in 3pg
	static float Etransp;
	static float DailyTransp;
	float alpha_evapo = 0.65;
	float beta_evapo = 0.95;
	//static float MonthTransp;


	//following BIOME
	rhoAir = 1.292 - (0.00428 * met[month].tav);

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
	//todo change all functions with BIOME's

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


	//CANOPY TRASPIRATION FOLLOWING BIOME APPROACH
	float tav_k; //Average temperature in Kelvin
	float rr; //resistance to radiative heat transfer through air


	//todo move into atmosphere.c
	/*compute air pressure*/
	float t1, t2;

	/* daily atmospheric pressure (Pa) as a function of elevation (m) */
	/* From the discussion on atmospheric statics in:
	Iribane, J.V., and W.L. Godson, 1981. Atmospheric Thermodynamics, 2nd
		Edition. D. Reidel Publishing Company, Dordrecht, The Netherlands.
		(p. 168)
	*/
	//todo insert elev in struct site and in site.txt file
	t1 = 1.0 - (LR_STD * 500/*site->elev*/)/T_STD;
	t2 = G_STD / (LR_STD * (R / MA));
	//todo move air_pressure into met file
	c->air_pressure = P_STD * pow (t1, t2);
	Log("Air pressure = %g Pa\n", c->air_pressure);

	/* temperature and pressure correction factor for conductances */
	//c->gcorr = pow((met[month].tav + 273.15)/293.15, 1.75) * 101300/c->air_pressure;



	//todo per finire la parte di BIOME devo inserire anche la parte di VPD
	/* assign tav (Celsius) and tav_k (Kelvins) */
	tav_k = met[month].tav + 273.15;

    /* calculate density of air (rho) as a function of air temperature */
	rhoAir = 1.292 - (0.00428 * met[month].tav);

    /* calculate resistance to radiative heat transfer through air, rr */
    rr = rhoAir * CP / (4.0 * SBC * (tav_k*tav_k*tav_k));









}
