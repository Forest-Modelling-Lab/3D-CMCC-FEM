/*modifiers.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

void Get_modifiers (SPECIES *const s,  const MET_DATA *const met, int year,  int month, int daysinmonth, float available_soil_water, float vpd, float Soil_Moist_Ratio, int z, int management )
{
	float RelAge;



	Log("\n GET_MODIFIERS\n\n");

	/*TEMPERATURE MODIFIER*/

	Log("--Temperature Average %g °C\n", met[month].tav);
	//Log("--Optimum temperature for species %s = %g °C\n", s->name, s->value[GROWTHTOPT]);
	//Log("--Maximum temperature for species %s = %g °C\n", s->name, s->value[GROWTHTMAX]);
	//Log("--Minimum temperature for species %s = %g °C\n", s->name, s->value[GROWTHTMIN]);

	if ((met[month].tav <= s->value[GROWTHTMIN]) || (met[month].tav >= s->value[GROWTHTMAX]))
	{
		s->value[F_T] = 0;
		Log("F_T = 0 \n");
	}
	else
	{
		s->value[F_T] = ((met[month].tav - s->value[GROWTHTMIN]) / (s->value[GROWTHTOPT] - s->value[GROWTHTMIN])) * pow(((s->value[GROWTHTMAX] - met[month].tav) / (s->value[GROWTHTMAX] - s->value[GROWTHTOPT])),
				((s->value[GROWTHTMAX] - s->value[GROWTHTOPT]) / (s->value[GROWTHTOPT] - s->value[GROWTHTMIN])));
	}
	Log("fT - Temperature Modifier = %g\n", s->value[F_T]);

	if (s->value[F_T] > 1)
	{
	    Log("ATTENTION fT EXCEEDS 1 \n");
	    s->value[F_T] = 1;
	}


	//average yearly f_vpd modifiers
	s->value[AVERAGE_F_T] += s->value[F_T];



	/*FROST MODIFIER*/

	//I dati di Rocca non hanno i giorni di gelo!!!!!!!!!!!!!!!!!!!!!!!!
	//Log("Frost Days = %g\n", met[month].frostdays);
	//s->value[F_FROST] = ( 1 - (met[month].frostdays / (float)daysinmonth));
	//Log("fFROST - Frost modifier = %g\n", s->value[F_FROST]);
	s->value[F_FROST] = 1;


	/*VPD MODIFIER*/

	//Log("--RH = %g %%\n", met[month].rh);
	//Log("--vpd = %g mbar", vpd);

	//The input VPD data is in mbar
	//if the VPD is in KPa
	//Convert to mbar
	//1 Kpa = 10 mbar
	//s->value[F_VPD] = exp (- s->value[COEFFCOND] * vpd) * 10);
	//convert also COEFFCOND moltiply it for
	s->value[F_VPD] = exp (- s->value[COEFFCOND] * vpd);
	Log("fVPD - VPD modifier = %g\n", s->value[F_VPD]);

	//average yearly f_vpd modifiers
	s->value[AVERAGE_F_VPD] += s->value[F_VPD];


	//look the version 1 for 3pg vpd modifier version

	/*AGE MODIFIER*/


     if (management == 0)
     {
        //for TIMBER
        //AGE FOR TIMBER IS THE EFFECTIVE AGE
        RelAge = (float)s->counter[TREE_AGE] / s->value[MAXAGE];
        s->value[F_AGE] = ( 1 / ( 1 + pow ((RelAge / (float)s->value[RAGE]), (float)s->value[NAGE] )));
        //Log("--Rel Age = %g years\n", RelAge);
        //Log("--Age = %d years\n", s->counter[TREE_AGE]);
        Log("fAge - Age modifier for timber= %g\n", s->value[F_AGE]);
     }
     else
     {
        //for SHOOTS
        //AGE FOR COPPICE IS THE AGE FROM THE COPPICING
        RelAge = (float)s->counter[TREE_AGE] / s->value[MAXAGE_S];
        s->value[F_AGE] = ( 1 / ( 1 + pow ((RelAge / (float)s->value[RAGE_S]), (float)s->value[NAGE_S] )));
        //Log("--Rel Age = %g years\n", RelAge);
        //Log("--Age = %d years\n", s->counter[TREE_AGE]);
        Log("fAge - Age modifier for coppice = %g\n", s->value[F_AGE]);

     }





	/*SOIL NUTRIENT MODIFIER*/

	s->value[F_NUTR] = 1.0 - ( 1.0- site->fn0)  * pow ((1.0 - site->fr), site->fnn);
	Log("fNutr - Soil Nutrient modifier = %g\n", s->value[F_NUTR]);




	/*SOIL NUTRIENT MODIFIER*/
	//rev 16 May 2012
	//Log("Soil Nitrogen Content = %g g m^-2 \n", site->sN);


	/*SOIL WATER MODIFIER*/

	//AVAILABLE SOIL WATER???DA FILE ROCCA (MA IN %) O COME IN FILE DI INTESTAZIONE?

	//float MoistRatio_from_data;
	/*

	if ( ! year  && ! month )
	{
		if ( met[month].swc == -9999)
		{
			Log("No Data for SWC!!\n");
			Log("swc from data = %g\n", met[month].swc);
			Log("Function use default data\n");

			//in realtà solo per il primo mese dovrebbe prendere INITIALAVAILABLESOILWATER poi dovrebbe ricalcolarselo
			MoistRatio = site->initialAvailableSoilWater / site->maxAsw;
			Log("-year %d\n", year);
			Log("-Initial Available Soil Water = %f mm\n", site->initialAvailableSoilWater);
			Log("-Moist Ratio = %g \n", MoistRatio);
		}
		else
		{
			MoistRatio = met[month].swc / 100;
			Log("OK Data for SWC!!\n");
			Log("swc from data = %g %%\n", met[month].swc);
			Log("Function use default data\n");
			Log("-Moist Ratio = %g \n", MoistRatio);
		}
	}
	else
	{
		MoistRatio = available_soil_water / site->maxAsw;
		//Log("-Available Soil Water = %g mm\n", available_soil_water);
		Log("-Moist Ratio = %g \n", MoistRatio);
	}
	*/

	s->value[F_SW] = 1.0 / (1.0 + pow(((1.0 - Soil_Moist_Ratio) / s->value[SWCONST]), s->value[SWPOWER]));
	//Log("Moist Ratio = %g \n", Soil_Moist_Ratio);
	//Log("SWCONST = %g \n", s->value[SWCONST]);
	//Log("SWPOWER = %g \n", s->value[SWPOWER]);
	if ( s->value[F_SW] > 1  )
	{
		Log("PROBLEM IN fSW !!!!!!!!!!\n");
		s->value[F_SW] = 1;
		Log("fSW - Soil Water modifier layer %d = %g\n", z,  s->value[F_SW]);
	}
	else
	{
        Log("fSW - Soil Water modifier layer %d = %g\n", z,  s->value[F_SW]);
	}

        //average yearly f_sw modifiers
	s->value[AVERAGE_F_SW] += s->value[F_SW];

	/*PHYSIOLOGICAL MODIFIER*/
	s->value[PHYS_MOD]= Minimum(s->value[F_VPD], s->value[F_SW]) * s->value[F_AGE];
	Log("PhysMod = %g\n", s->value[PHYS_MOD]);
	if (s->value[F_VPD] < s->value[F_SW])
	{
	        Log("PHYSMOD uses F_VPD * F_AGE\n");
	}
	else
	{
                Log("PHYSMOD uses F_SW * F_AGE\n");
	}

	s->value[YEARLY_PHYS_MOD] += s->value[PHYS_MOD];
	//Log("Yearly Physmod = %g\n", s->value[YEARLY_PHYS_MOD]);


	/*SOIL DROUGHT MODIFIER*/
	//(see Duursma et al., 2008)
/*
	//to put in species.txt
	//numbers are not real just used for compile!!!!!!!!
	float leaf_res = 1; //leaf specific plant hydraulic resistance
	float min_leaf_pot = 1; //minimum leaf water potential


	//to put in site.txt ?????
	float soil_res = 1; //soil hydraulic resistance
	float psi0 = 2; //dry soil water potential in MPa
	float soil_coeff = 1; //empirical soil coefficient
	//soil percentage characteristics have to be with value 0-1
	float clay_perc; //percentage of clay
	float silt_perc; //percentage of silt
	float sand_perc; //percentage of sand
	//soil average dimension particle
	float clay_dim; //clay avg dimension of particle
	float silt_dim; //silt avg dimension of particle
	float sand_dim; //sand avg dimension of particle

	float dens = 1.49; //g/cm^3




    float bulk_pot; //bulk soil water potential
    float asw_vol; //available soil water in volume
    float eq;
	float eq1;
	float eq2;
	float sat_soil_water_cont; //saturated soil water content (m^3 m^-3)
	float soil_avg_dim; //soil mean particle diameter in mm
	float sigma_g; //geometric standard deviation in particle size distribution (mm)
	float pentry_temp; //soil entry water potential (MPa)
	float pentry; //correction for bulk density effects
	float bsl; //coefficient in soil water release curve (-)
	float soil_water_pot_sat; //soil water potential at saturation
	float sat_conduct; //saturated conductivity
	float specific_soil_cond; //specific soil hydraulic conductance


	//compute soil hydraulic characteristics from soil granulometry
	//from model Hydrall
	eq1 = (clay_perc * log(clay_dim)) + (silt_perc * log(silt_perc)) + (sand_perc * log(sand_dim));

	//soil mean particle diameter in mm
	soil_avg_dim = exp(eq1);


    eq2 = sqrt ((pow ((clay_perc * log(clay_dim)),2)) + (pow ((sand_perc * log(sand_dim)),2)) + (pow ((silt_perc * log(silt_dim)),2)));

    //geometric standard deviation in particle size distribution (mm)
    sigma_g = exp(eq2);

    //soil entry water potential (MPa)
    pentry_temp = -0.5 / sqrt(soil_avg_dim)/1000;
    //correction for bulk density effects with dens = 1.49 g/cm^3
    pentry = pentry_temp * pow ((dens / 1.3), (0.67 * bsl));

    bsl = -2 * (pentry * 1000) + 0.2 * sigma_g;

    //saturated soil water content
    sat_soil_water_cont= 1.0 - (dens/2.56);

    eq = pentry * pow ((sat_soil_water_cont / Soil_Moist_Ratio), bsl);

    //compue bulk soil water potential
    //for psi see Magani xls
    bulk_pot = Maximum (eq, min_leaf_pot);

    //compute leaf-specific soil hydraulic conductance
	leaf_specific_soil_cond = sat_conduct * pow ((soil_water_pot_sat / bulk_pot), (2 + (3 / soil_coeff)));


	s->value[F_DROUGHT] = (leaf_res * (bulk_pot - min_leaf_pot)) / (- min_leaf_pot * ((leaf_res + soil_res) * bulk_pot));

*/


	/*CO2 MODIFIER FROM C-FIX*/

	Log("-------------------\n");
}
