/*modifiers.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "soil_settings.h"
#include "modifiers.h"
#include "constants.h"
#include "common.h"
#include "settings.h"
#include "logger.h"

/* externs */
extern settings_t* g_settings;
extern logger_t* g_log;
extern soil_settings_t *g_soil_settings;

void Daily_modifiers(cell_t *const c, const int layer, const int height, const int age, const int species, const int management, const meteo_daily_t *const meteo_daily)
{
	double RelAge;
	/*variables for CO2 modifier computation*/
	double KmCO2;	                                   /* affinity coefficients temperature dependent according to Arrhenius relationship */
	double Ea1 = 59400.0; //KJ mol^-1
	double A1 = 2.419 * pow(10,13);
	double Ea2 = 109600.0;	//KJ mol^-1
	double A2 = 1.976 * pow(10,22);
	double KO2;	//Inhibition constant for 02
	double EaKO2 = 13913.5;	//KJ mol^-1
	double AKO2 = 8240;
	double tau;	// CO2/O2  specifity ratio
	double Eatau = -42896.9;
	double Atau = 7.87 * pow(10,-5);
	double tairK;
	double v1, v2;

	static int counter_water_stress;

	age_t *a;
	species_t *s;

	a = &c->heights[height].ages[age];
	s = &a->species[species];

	//test
	//double vpd_open = 6; //value from pietsch in Pa a(600) are converted in hPa = 6
	//double vpd_close = 12; // 12 in taken from Priwitzer et al., 2014 30 from Pietsch in Pa (3000) are converted in hPa = 30

	logger(g_log, "\nDAILY_MODIFIERS\n\n");

	/* CO2 MODIFIER FROM C-FIX */
	tairK = meteo_daily->tavg + TempAbs;

	if (meteo_daily->tavg >= 15)
	{
		KmCO2 = A1 * exp(-Ea1/(Rgas*tairK));
	}
	else
	{
		KmCO2 = A2 * exp (-Ea2/(Rgas*tairK));
	}
	KO2 = AKO2 * exp (-EaKO2/(Rgas*tairK));

	tau = Atau * exp (-Eatau/(Rgas*(tairK)));

	v1 = (g_settings->co2Conc-(O2CONC/(2*tau)))/(refCO2CONC-(O2CONC/(2*tau)));
	v2 = (KmCO2*(1+(O2CONC/KO2))+refCO2CONC)/(KmCO2*(1+(O2CONC/KO2))+g_settings->co2Conc);

	s->value[F_CO2] = v1*v2;
	logger(g_log, "F_CO2 modifier  = %f\n", s->value[F_CO2]);

	/* LIGHT MODIFIER */
	/* (Following Makela et al , 2008, Peltioniemi_etal_2012) */
	//FIXME chose which type of light use and differentiate for different layers
	//following Nolè should be used apar
	//following Peltioniemi should be used par
	//ALESSIOC
	if ( c->heights[height].z == c->heights[height].t_layers[layer].z )
	{
		if (s->value[GAMMA_LIGHT] != -9999)
		{
			s->value[F_LIGHT]= 1.0/ ((s->value[GAMMA_LIGHT]* s->value[APAR]) +1.0);
		}
		else
		{
			s->value[F_LIGHT]= 1.0;
		}
		logger(g_log, "FLight (NOT USED)= %f\n", s->value[F_LIGHT]);
	}

	/* following Biome-BGC */
	/* photosynthetic photon flux density conductance control */
	/* for sun leaves */
	s->value[F_LIGHT_SUN] = s->value[PPFD_ABS_SUN] /(PPFD50 + s->value[PPFD_ABS_SUN]);
	logger(g_log, "F_LIGHT_SUN = %f \n", s->value[F_LIGHT_SUN]);
	/* for shaded leaves */
	s->value[F_LIGHT_SHADE] = s->value[PPFD_ABS_SHADE] /(PPFD50 + s->value[PPFD_ABS_SHADE]);
	logger(g_log, "F_LIGHT_SHADE = %f \n", s->value[F_LIGHT_SHADE]);


	/* TEMPERATURE MODIFIER */
	if (meteo_daily->tday == NO_DATA)
	{
		if ((meteo_daily->tavg <= s->value[GROWTHTMIN]) || (meteo_daily->tavg >= s->value[GROWTHTMAX]))
		{
			s->value[F_T] = 0;
			logger(g_log, "F_T = 0 \n");
		}
		else
		{
			s->value[F_T] = ((meteo_daily->tavg - s->value[GROWTHTMIN]) / (s->value[GROWTHTOPT] - s->value[GROWTHTMIN])) *
					pow(((s->value[GROWTHTMAX] - meteo_daily->tavg) / (s->value[GROWTHTMAX] - s->value[GROWTHTOPT])),
					((s->value[GROWTHTMAX] - s->value[GROWTHTOPT]) / (s->value[GROWTHTOPT] - s->value[GROWTHTMIN])));
		}
	}
	else
	{
		if ((meteo_daily->tday <= s->value[GROWTHTMIN]) || (meteo_daily->tday >= s->value[GROWTHTMAX]))
		{
			logger(g_log, "tday < 0 > GROWTHTMIN o GROWTHTMAX\n");
			s->value[F_T] = 0;
			logger(g_log, "F_T = 0 \n");
		}
		else
		{
			s->value[F_T] = ((meteo_daily->tday - s->value[GROWTHTMIN]) / (s->value[GROWTHTOPT] - s->value[GROWTHTMIN])) *
					pow(((s->value[GROWTHTMAX] - meteo_daily->tday) / (s->value[GROWTHTMAX] - s->value[GROWTHTOPT])),
					((s->value[GROWTHTMAX] - s->value[GROWTHTOPT]) / (s->value[GROWTHTOPT] - s->value[GROWTHTMIN])));
		}
	}
	logger(g_log, "fT = %f\n", s->value[F_T]);

	CHECK_CONDITION(s->value[F_T], > 1);
	CHECK_CONDITION(s->value[F_T], < 0);
	c->daily_f_t = s->value[F_T];
	//average yearly f_vpd modifiers
	s->value[AVERAGE_F_T] += s->value[F_T];

	/*FROST MODIFIER*/
	if(meteo_daily->tday < s->value[GROWTHTMIN])
	{
		s->value[F_FROST] = 0.0;
		logger(g_log, "fFROST - Frost modifier = %f\n", s->value[F_FROST]);
	}
	else
	{
		s->value[F_FROST] = 1.0;
		logger(g_log, "fFROST - Frost modifier = %f\n", s->value[F_FROST]);
	}

	/*VPD MODIFIER*/
	//logger(g_log, "--RH = %f %%\n", met[month].rh);
	//logger(g_log, "--vpd = %f mbar", vpd);

	//The input VPD data is in KPa
	//if the VPD is in KPa
	//Convert to mbar
	//1 Kpa = 10 mbar
	//1 hPa = 1 mbar
	//s->value[F_VPD] = exp (- s->value[COEFFCOND] * vpd) * 10);
	//convert also COEFFCOND multiply it for
	s->value[F_VPD] = exp (- s->value[COEFFCOND] * meteo_daily->vpd);
	c->daily_f_vpd = s->value[F_VPD];
	logger(g_log, "fVPD = %f\n", s->value[F_VPD]);

	//average yearly f_vpd modifiers
	s->value[AVERAGE_F_VPD] += s->value[F_VPD];

	//test following biome-bgc it doesn't seems to work properly here (too many higher values for gpp and le
	/* vapor pressure deficit multiplier, vpd in Pa */
//	if (meteo_daily->vpd < vpd_open)    /* no vpd effect */
//		s->value[F_VPD] = 1.0;
//	else if (meteo_daily->vpd > vpd_close)   /* full vpd effect */
//		s->value[F_VPD] = 0.0;
//	else                   /* partial vpd effect */
//		s->value[F_VPD] = (vpd_close - meteo_daily->vpd) / (vpd_close - vpd_open);

	/* AGE MODIFIER */

	if (a->value != 0)
	{
		if (management == T)
		{
			//for TIMBER
			//AGE FOR TIMBER IS THE EFFECTIVE AGE
			RelAge = (double)a->value / s->value[MAXAGE];
			s->value[F_AGE] = ( 1 / ( 1 + pow ((RelAge / (double)s->value[RAGE]), (double)s->value[NAGE])));
			logger(g_log, "fAge = %f\n", s->value[F_AGE]);
		}
		else
		{
			//for SHOOTS
			//AGE FOR COPPICE IS THE AGE FROM THE COPPICING
			RelAge = (double)a->value / s->value[MAXAGE_S];
			s->value[F_AGE] = ( 1 / ( 1 + pow ((RelAge / (double)s->value[RAGE_S]), (double)s->value[NAGE_S])));
			logger(g_log, "fAge = %f\n", s->value[F_AGE]);
		}
	}
	else
	{
		s->value[F_AGE] = 1;
		logger(g_log, "no data for age F_AGE = 1\n");
	}

	/*SOIL NUTRIENT MODIFIER*/
	s->value[F_NUTR] = 1.0 - ( 1.0- g_soil_settings->values[SOIL_FN0])  * pow ((1.0 - g_soil_settings->values[SOIL_FR]), g_soil_settings->values[SOIL_FNN]);
	logger(g_log, "fNutr = %f\n", s->value[F_NUTR]);


	/*SOIL WATER MODIFIER*/
	//fixme include "dAdjMod" from 3-PG code
	//ALESSIOC
	//c->soil_moist_ratio = c->asw/c->max_asw_fc;
	//s->value[F_SW] = 1.0 / (1.0 + pow(((1.0 - c->soil_moist_ratio) / s->value[SWCONST]), s->value[SWPOWER]));
	CHECK_CONDITION(s->value[F_SW], > 1.0);
	logger(g_log, "ASW = %f mm/m2\n", c->asw);
	logger(g_log, "Wilting point = %f mm/m2\n", c->wilting_point);
	//ALESSIOC
	//logger(g_log, "moist ratio = %f\n", c->soil_moist_ratio);
	logger(g_log, "fSW = %f\n", s->value[F_SW]);


	/* (MPa) water potential of soil and leaves */
	/*SOIL MATRIC POTENTIAL*/

	/* convert kg/m2 or mm  --> m3/m2 --> m3/m3 */
	//100 mm H20 m^-2 = 100 kg H20 m^-2
	/* calculate the soil pressure-volume coefficients from texture data */
	/* Uses the multivariate regressions from Cosby et al., 1984 */
	/* volumetric water content */
	logger(g_log, "\nBIOME SOIL WATER MODIFIER\n");
	logger(g_log, "SWP_OPEN = %f\n", s->value[SWPOPEN]);
	logger(g_log, "SWP_CLOSE = %f\n", s->value[SWPCLOSE]);
	//note:changed from biome
	//ALESSIOC
	//c->vwc = c->asw / c->max_asw_fc /* /(100.0 * g_soil_settings->values[SOIL_DEPTH])*/;
	//logger(g_log, "volumetric available soil water  = %f %(vol)\n", c->vwc);
	//logger(g_log, "vwc_fc = %f (DIM)\n", c->vwc_fc);
	//logger(g_log, "vwc_sat = %f (DIM)\n", c->vwc_sat);
	//logger(g_log, "vwc/vwc_sat = %f \n", c->vwc / c->vwc_sat);
	//logger(g_log, "vwc/vwc_fc = %f \n", c->vwc / c->vwc_fc);
	//c->psi = c->psi_sat * pow((c->vwc/c->vwc_sat), c->soil_b);
	//logger(g_log, "PSI BIOME = %f (MPa)\n", c->psi);

	/*no water stress*/
	if (c->psi > s->value[SWPOPEN])
	{
		logger(g_log, "no water stress\n");
		counter_water_stress = 0;
		s->value[F_PSI] = 1.0;
	}
	/* full water stress */
	else if (c->psi <= s->value[SWPCLOSE])
	{
		logger(g_log, "complete water stress\n");
		//change for multiple class
		counter_water_stress += 1;
		//s->value[F_PSI] = 0.0;
		logger(g_log, "Water stress\n");
		logger(g_log, "F_PSI = %f\n", s->value[F_PSI]);

		/* forced to  0.3 to avoid zero values */
		//see: Clark et al., 2011 for JULES model impose 0.2
		s->value[F_PSI] = 0.3;
		logger(g_log, "F_PSI = %f\n", s->value[F_PSI]);
		//CHECK_CONDITION(counter_water_stress, > 31);

	}
	/* partial water stress */
	else
	{
		logger(g_log, "partial water stress\n");
		counter_water_stress = 0;
		s->value[F_PSI] = (s->value[SWPCLOSE] - c->psi)/(s->value[SWPCLOSE] - s->value[SWPOPEN]);

		//test
		// for consistency with complete stress values
		if(s->value[F_PSI]< 0.3) s->value[F_PSI] = 0.3;
	}

	s->value[F_SW] = s->value[F_PSI];
	logger(g_log, "F_PSI = %f\n", s->value[F_PSI]);

	c->daily_f_psi = s->value[F_PSI];

	//average yearly f_sw modifiers
	s->value[AVERAGE_F_SW] += s->value[F_SW];

	/*PHYSIOLOGICAL MODIFIER*/
	s->value[PHYS_MOD] = MIN (s->value[F_VPD], (s->value[F_SW] * s->value[F_AGE]));
	logger(g_log, "PhysMod = %f\n", s->value[PHYS_MOD]);
	if (s->value[F_VPD] < (s->value[F_SW] * s->value[F_AGE]))
	{
		logger(g_log, "PHYSMOD uses F_VPD = %f\n", s->value[F_VPD]);
	}
	else
	{
		logger(g_log, "PHYSMOD uses F_SW * F_AGE = %f\n", s->value[F_SW] * s->value[F_AGE]);
	}

	s->value[YEARLY_PHYS_MOD] += s->value[PHYS_MOD];
	//logger(g_log, "Yearly Physmod = %f\n", s->value[YEARLY_PHYS_MOD]);


	/*SOIL DROUGHT MODIFIER*/
	//(see Duursma et al., 2008)rev_Angelo
	/*
		//to put in species.txt
		//numbers are not real just used to compile!!!!!!!!
		double leaf_res = 1; //leaf specific plant hydraulic resistance
		double min_leaf_pot = 1; //minimum leaf water potential


		//to put in site.txt ?????
		double soil_res = 1; //soil hydraulic resistance
		double psi0 = 2; //dry soil water potential in MPa
		double soil_coeff = 1; //empirical soil coefficient

		//soil average dimension particle
		//value are averaged from limits in site.txt
		double clay_dim = 0.001; //clay avg dimension of particle
		double silt_dim =  0.026;//silt avg dimension of particle
		double sand_dim =  1.025;//sand avg dimension of particle

	    double bulk_pot; //bulk soil water potential
	    double asw_vol; //available soil water in volume
	    double eq;
		double eq1;
		double eq2;
		double sat_soil_water_cont; //saturated soil water content (m^3 m^-3)
		double soil_avg_dim; //soil mean particle diameter in mm
		double sigma_g; //geometric standard deviation in particle size distribution (mm)
		double pentry_temp; //soil entry water potential (MPa)
		double pentry; //correction for bulk density effects
		double bsl; //coefficient in soil water release curve (-)
		double soil_water_pot_sat; //soil water potential at saturation
		double sat_conduct; //saturated conductivity
		double specific_soil_cond; //specific soil hydraulic conductance
		double leaf_specific_soil_cond;

		//compute soil hydraulic characteristics from soil granulometry
		//from model Hydrall
		eq1 = (g_soil_settings->values[SOIL_CLAY_PERC] * log(clay_dim)) + (g_soil_settings->values[SOIL_silt_perc * log(silt_dim)) + (g_soil_settings->values[SOIL_sand_perc * log(sand_dim));
		logger(g_log, "eq1 = %f\n", eq1);

		//soil mean particle diameter in mm
		soil_avg_dim = exp(eq1);
		logger(g_log, "soil_avg_dim = %f\n", soil_avg_dim);

	    eq2 = sqrt ((pow ((g_soil_settings->values[SOIL_CLAY_PERC] * log(clay_dim)),2)) + (pow ((g_soil_settings->values[SOIL_sand_perc * log(sand_dim)),2)) + (pow ((g_soil_settings->values[SOIL_silt_perc * log(silt_dim)),2)));
	    logger(g_log, "eq2 = %f\n", eq2);

	    //geometric standard deviation in particle size distribution (mm)
	    sigma_g = exp(eq2);
	    logger(g_log, "sigma_g = %f\n", sigma_g);

	    //soil entry water potential (MPa)
	    pentry_temp = -0.5 / sqrt(soil_avg_dim)/1000;
	    logger(g_log, "pentry_temp = %f\n", pentry_temp);
	    //correction for bulk density effects with dens = 1.49 g/cm^3
	    pentry = pentry_temp * pow ((c->bulk_density / 1.3), (0.67 * bsl));
	    logger(g_log, "pentry = %f\n", pentry);

	    bsl = -2 * (pentry * 1000) + 0.2 * sigma_g;
	    logger(g_log, "bsl = %f\n", bsl);

	    //saturated soil water content
	    sat_soil_water_cont= 1.0 - (c->bulk_density/2.56);
	    logger(g_log, "soil water content at saturation = %f\n", sat_soil_water_cont);

	    eq = pentry * pow ((sat_soil_water_cont / c->soil_moist_ratio), bsl);
	    logger(g_log, "eq = %f\n", eq);

	    //compute bulk soil water potential
	    //for psi see Magnani xls
	    bulk_pot = Maximum (eq, min_leaf_pot);
	    logger(g_log, "bulk soil water potential = %f\n", bulk_pot);

	    //compute leaf-specific soil hydraulic conductance
		leaf_specific_soil_cond = sat_conduct * pow ((soil_water_pot_sat / bulk_pot), (2 + (3 / soil_coeff)));
		logger(g_log, "leaf-specific soil hydraulic conductance = %f\n", leaf_specific_soil_cond);


		s->value[F_DROUGHT] = (leaf_res * (bulk_pot - min_leaf_pot)) / (- min_leaf_pot * ((leaf_res + soil_res) * bulk_pot));
		logger(g_log, "F_DROUGHT = %f\n", s->value[F_DROUGHT]);
	 */

	logger(g_log, "-------------------\n");
}
