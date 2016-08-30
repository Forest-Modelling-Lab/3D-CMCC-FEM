/*
 * canopy_radiation.c
 *
 *  Created on: 05 lug 2016
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"
#include "canopy_radiation_sw_band.h"

extern logger_t* g_log;

void canopy_sw_band_abs_trans_refl_radiation(cell_t *const c, const int height, const int dbh, const int age, const int species, double Light_abs_frac,
		double Light_abs_frac_sun, double Light_abs_frac_shade, double Light_refl_par_frac, double Light_refl_sw_rad_canopy_frac)
{
	double leaf_cell_cover_eff;       /* effective fraction of leaf cover over the cell (ratio) */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* note: This function works at layer/class level computing absorbed transmitted and reflected PAR, NET RADIATION
	 * and PPFD through different height classes/layers considering at square meter takes into account coverage,
	 * it means that a square meter grid cell * represents overall grid cell (see Duursma and Makela, 2007) */

	/* it follows a little bit different rationale compared to BIOME-BGC approach
	* in BIOME_BGC:
	* apar = par * (1 - (exp(- K * LAI)));
	* apar_sun = par * (1 - (exp(- K * LAI_SUN)));
	* apar_shade = apar- apar_sun;
	*
	* in 3D-CMCC FEM:
	* apar_sun = par * (1 - (exp(- K * LAI_SUN)));
	* par_transm_sun  = par - apar_sun;
	* apar_shade = par_transm_sun * (1 - (exp(- K * LAI_SHADE)));
	* apar = apar_sun + apar_shade;
	*
	* then it consider that an amount of sunlit leaf are not completely outside the canopy
	* but there's an exponential decay of absorption also for sunlit foliage	 *
	*/

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
	logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/*****************************************************************************************************************/
	/* light reflection, absorption and transmission */
	logger(g_log,"\n*Light reflection, absorption and transmission*\n");

	/***********************************************************************************************/
	/* PAR computation (molPAR/m2 covered/day) */
	logger(g_log,"\n-PAR-\n");

	/* compute reflected PAR */
	s->value[PAR_REFL] = c->par * Light_refl_par_frac * leaf_cell_cover_eff;

	/*assign to class PAR */
	s->value[PAR] = c->par - s->value[PAR_REFL];

	/* compute absorbed and transmitted PAR for sun and shaded leaves */
	s->value[APAR_SUN] = s->value[PAR] * Light_abs_frac_sun * leaf_cell_cover_eff;
	s->value[TRANSM_PAR_SUN] = s->value[PAR] - s->value[APAR_SUN];
	s->value[APAR_SHADE] = s->value[TRANSM_PAR_SUN] * Light_abs_frac_shade * leaf_cell_cover_eff;
	s->value[TRANSM_PAR_SHADE] = s->value[TRANSM_PAR_SUN] - s->value[APAR_SHADE];

	/* overall canopy */
	s->value[APAR] = s->value[APAR_SUN] + s->value[APAR_SHADE];
	s->value[TRANSM_PAR] = s->value[TRANSM_PAR_SHADE];

	logger(g_log, "-Par incoming = %g molPAR/m^2/day\n", c->par);
	logger(g_log, "-Par reflected = %g molPAR/m^2/day\n", s->value[PAR_REFL]);
	logger(g_log, "-Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);
	logger(g_log, "-Apar sun = %g molPAR/m^2 covered/day\n", s->value[APAR_SUN]);
	logger(g_log, "-Transmitted Par sun = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR_SUN]);
	logger(g_log, "-Apar shade = %g molPAR/m^2 covered/day\n", s->value[APAR_SHADE]);
	logger(g_log, "-Transmitted Par shade = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR_SHADE]);
	logger(g_log, "-Apar total = %g molPAR/m^2 covered/day\n", s->value[APAR]);
	logger(g_log, "-Transmitted Par total = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR]);

	/* check */
	CHECK_CONDITION(s->value[APAR], < 0.);
	CHECK_CONDITION(s->value[TRANSM_PAR], < 0.);
	CHECK_CONDITION(fabs((s->value[APAR] + s->value[TRANSM_PAR])-s->value[PAR]),>1e-4);

	/***********************************************************************************************/
	/* Short Wave computation (W/m2 covered) */
	logger(g_log,"\n-Short Wave-\n");

	/* compute reflected Short Wave */
	s->value[SW_RAD_REFL] = c->sw_rad_down_W * Light_refl_sw_rad_canopy_frac * leaf_cell_cover_eff;

	/*assign to class Short Wave Radiation */
	s->value[SW_RAD] = c->sw_rad_down_W - s->value[SW_RAD_REFL];

	/*compute absorbed and transmitted Short Wave for sun and shaded leaves*/
	s->value[SW_RAD_ABS_SUN] = s->value[SW_RAD] * Light_abs_frac_sun * leaf_cell_cover_eff;
	s->value[SW_RAD_TRANSM_SUN] = s->value[SW_RAD] - s->value[SW_RAD_ABS_SUN];
	s->value[SW_RAD_ABS_SHADE] = s->value[SW_RAD_TRANSM_SUN] * Light_abs_frac_shade * leaf_cell_cover_eff;
	s->value[SW_RAD_TRANSM_SHADE] = s->value[SW_RAD_TRANSM_SUN] - s->value[SW_RAD_ABS_SHADE];

	/* overall canopy */
	s->value[SW_RAD_ABS] = s->value[SW_RAD_ABS_SUN] + s->value[SW_RAD_ABS_SHADE];
	s->value[SW_RAD_TRANSM] = s->value[SW_RAD_TRANSM_SHADE];

	logger(g_log, "-Short Wave incoming = %gW/m2\n", c->sw_rad_down_W);
	logger(g_log, "-Short Wave reflected = %gW/m2\n", s->value[SW_RAD_REFL]);
	logger(g_log, "-Short Wave radiation less reflected part = %g W/m2\n", s->value[SW_RAD]);
	logger(g_log, "-Absorbed Short Wave radiation sun = %g W/m^2 covered\n", s->value[SW_RAD_ABS_SUN]);
	logger(g_log, "-Transmitted Short Wave radiation sun = %g W/m^2 covered\n", s->value[SW_RAD_TRANSM_SUN]);
	logger(g_log, "-Absorbed Short Wave radiation shade = %g W/m^2 covered\n", s->value[SW_RAD_ABS_SHADE]);
	logger(g_log, "-Transmitted Short Wave radiation shade = %g W/m^2 covered\n", s->value[SW_RAD_TRANSM_SHADE]);
	logger(g_log, "-Absorbed total = %g W/m^2 covered\n", s->value[SW_RAD_ABS]);
	logger(g_log, "-Transmitted total = %g W/m^2 covered\n", s->value[SW_RAD_TRANSM]);

	/* check */
	CHECK_CONDITION(s->value[SW_RAD_ABS], < 0.);
	CHECK_CONDITION(s->value[SW_RAD_TRANSM], < 0.);
	CHECK_CONDITION(fabs((s->value[SW_RAD_ABS] + s->value[SW_RAD_TRANSM])-s->value[SW_RAD]),>1e-4);

	/***********************************************************************************************/
	/* PPFD computation (umol/m2 covered/sec) */
	logger(g_log,"\n-PPFD-\n");

	/* compute reflected PPFD */
	s->value[PPFD_REFL] = c->ppfd * Light_refl_par_frac * leaf_cell_cover_eff;

	/*assign to class PPFD */
	s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];

	/*compute absorbed and transmitted PPFD for sun and shaded leaves*/
	s->value[PPFD_ABS_SUN] = s->value[PPFD] * Light_abs_frac_sun * leaf_cell_cover_eff;
	s->value[PPFD_TRANSM_SUN] = s->value[PPFD] - s->value[PPFD_ABS_SUN];
	s->value[PPFD_ABS_SHADE] = s->value[PPFD_TRANSM_SUN] * Light_abs_frac_shade* leaf_cell_cover_eff;
	s->value[PPFD_TRANSM_SHADE] = s->value[PPFD_TRANSM_SUN] - s->value[PPFD_ABS_SHADE];

	/* overall canopy */
	s->value[PPFD_ABS] = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
	s->value[PPFD_TRANSM] = s->value[PPFD_TRANSM_SHADE];

	logger(g_log, "-PPFD incoming = %g umol/m2/sec\n", c->ppfd);
	logger(g_log, "-PPFD reflected = %g umol/m2/sec\n", s->value[PPFD_REFL]);
	logger(g_log, "-PPFD less reflected part = %g umol/m2/sec\n", s->value[PPFD]);
	logger(g_log, "-Absorbed ppfd sun = %g umol/m2 covered/sec\n", s->value[PPFD_ABS_SUN]);
	logger(g_log, "-Transmitted ppfd sun = %g umol/m2 covered/sec\n", s->value[PPFD_TRANSM_SUN]);
	logger(g_log, "-Absorbed ppfd shade = %g umol/m2 covered/sec\n", s->value[PPFD_ABS_SHADE]);
	logger(g_log, "-Transmitted ppfd shade = %g umol/m2 covered/sec\n", s->value[PPFD_TRANSM_SHADE]);
	logger(g_log, "-Absorbed ppfd total = %g umol/m2 covered/sec\n", s->value[PPFD_ABS]);
	logger(g_log, "-Transmitted ppfd total  = %g umol/m2 covered/sec\n", s->value[PPFD_TRANSM]);
	logger(g_log, "***********************************\n");

	/* check */
	CHECK_CONDITION(s->value[PPFD_ABS], < 0.);
	CHECK_CONDITION(s->value[PPFD_TRANSM], < 0.);
	CHECK_CONDITION(fabs((s->value[PPFD_ABS] + s->value[PPFD_TRANSM])-s->value[PPFD]),>1e-4);

	/***********************************************************************************************/

	/* it follows rationale of BIOME-BGC to obtain m2 instead m2/m2 */
	//fixme then recompute transmitted fraction!!!!!!!!!!!!
	/*
	if(s->value[PPFD_ABS_SHADE] < 0.0)
	{
		s->value[PPFD_ABS_SHADE]  = 0.0;
	}
	if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
	{
		s->value[PPFD_ABS_SUN] /= s->value[LAI_SUN];
		s->value[PPFD_ABS_SHADE] /= s->value[LAI_SHADE];
	}
	else
	{
		s->value[PPFD_ABS_SUN] = s->value[PPFD_ABS_SHADE] = 0.0;

	}
	s->value[PPFD_ABS] = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
	*/
}

void canopy_radiation_sw_band(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	static int cell_height_class_counter;
	static int layer_height_class_counter;

	double Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade;                      /* (ratio) fraction of PAR and Short Wave radiation absorbed */
	double Light_trasm_frac, Light_trasm_frac_sun, Light_trasm_frac_shade;                /* (ratio) fraction of PAR and Short Wave radiation transmitted */
	double Light_refl_sw_rad_canopy_frac;                                                 /* (ratio) fraction of Short Wave radiation reflected from the canopy */
	double Light_refl_sw_rad_soil_frac;                                                   /* (ratio) fraction of Short Wave radiation reflected from the soil */
	double Light_refl_par_frac;                                                           /* (ratio) fraction of PAR reflected from canopy */
	static double temp_apar;                                                              /* temporary absorbed PAR for layer */
	static double temp_par_refl;                                                          /* temporary reflected PAR for layer */
	static double temp_sw_rad_abs;                                                        /* temporary absorbed short wave for layer */
	static double temp_sw_rad_refl;                                                       /* temporary reflected short wave for layer */
	static double temp_ppfd_abs;                                                          /* temporary absorbed PPFD for layer */
	static double temp_ppfd_refl;                                                         /* temporary reflected PPFD for layer */

	double leaf_cell_cover_eff;                                                           /* (ratio) fraction of square meter covered by leaf over the grid cell */

	//fixme move soil albedo into soil.txt file
	const double soil_albedo = 0.15;                                                      /* (ratio) soil albedo without snow (see MAESPA model) */

	tree_layer_t *l;
	//height_t *h;
	species_t *s;

	l = &c->t_layers[layer];
	//h = &c->heights[height];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	//double actual_albedo;

	logger(g_log, "\n**SHORT WAVE BAND RADIATION**\n");

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if( s->value[LAI] < 1.0 ) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if( leaf_cell_cover_eff > 1.0 ) leaf_cell_cover_eff = 1.0;
	logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/***********************************************************************************************************/

	/* SHORT WAVE RADIATION FRACTIONS */
	/* compute fractions of light intercepted, transmitted and reflected from the canopy */
	/* fraction of light transmitted through the canopy */
	Light_trasm_frac = (exp(- s->value[K] * s->value[LAI])); //not used
	Light_trasm_frac_sun = (exp(- s->value[K] * s->value[LAI_SUN]));
	Light_trasm_frac_shade = (exp(- s->value[K] * s->value[LAI_SHADE]));

	/* fraction of light absorbed by the canopy */
	Light_abs_frac = 1.0 - Light_trasm_frac; //not used
	Light_abs_frac_sun = 1.0 - Light_trasm_frac_sun;
	Light_abs_frac_shade = 1.0 - Light_trasm_frac_shade;

	/* fraction of light reflected by the canopy */
	/* for Short Wave radiation and PAR */
	/* following BIOME albedo for PAR is 1/3 of albedo. the absorbed PAR is
	calculated similarly to sw except that albedo is 1/3 for PAR because less
	PAR is reflected than sw_radiation (Jones 1992)*/

	if( s->value[LAI] >= 1.0 )
	{
		Light_refl_sw_rad_canopy_frac = s->value[ALBEDO];
		//		LightReflec_net_rad_sun = s->value[ALBEDO];
		//		LightReflec_net_rad_shade = s->value[ALBEDO];
		Light_refl_par_frac = s->value[ALBEDO]/3.0;
		//		LightReflec_par_sun = s->value[ALBEDO]/3.0;
		//		LightReflec_par_shade = s->value[ALBEDO]/3.0;
	}
	else if ( !s->value[LAI])
	{
		Light_refl_sw_rad_canopy_frac = 0.0;
		//		LightReflec_net_rad_sun = 0.0;
		//		LightReflec_net_rad_shade = 0.0;
		Light_refl_par_frac = 0.0;
		//		LightReflec_par_sun = 0.0;
		//		LightReflec_par_shade = 0.0;

	}
	else
	{
		Light_refl_sw_rad_canopy_frac = s->value[ALBEDO] * s->value[LAI];
		//		LightReflec_net_rad_sun = s->value[ALBEDO]* s->value[LAI_SUN];
		//		LightReflec_net_rad_shade = s->value[ALBEDO] * * s->value[LAI_SHADE];
		Light_refl_par_frac = (s->value[ALBEDO]/3.0) * s->value[LAI];
		//		LightReflec_par_sun =  (s->value[ALBEDO]/3.0) * s->value[LAI_SUN];
		//		LightReflec_par_shade =  (s->value[ALBEDO]/3.0) * s->value[LAI_SHADE];
	}

	logger(g_log, "*Fractions of light absorbed, transmitted and reflected*\n\n");
	logger(g_log, "Light_abs_frac_sun = %g %%\n", Light_abs_frac_sun * 100);
	logger(g_log, "LightTrasm_sun = %g %%\n\n", Light_trasm_frac_sun * 100);
	logger(g_log, "Light_abs_frac_shade = %g %%\n", Light_abs_frac_shade * 100);
	logger(g_log, "LightTrasm_sun = %g %%\n\n", Light_trasm_frac_shade * 100);
	logger(g_log, "Light_abs_frac (sun+shaded) = %g %%\n", Light_abs_frac * 100);
	logger(g_log, "LightTrasm (sun+shaded)= %g %%\n\n", Light_trasm_frac * 100);
	logger(g_log, "Light_refl_sw_rad_canopy_frac = %g %%\n", Light_refl_sw_rad_canopy_frac * 100);
	logger(g_log, "LightReflec_par = %g %%\n\n", Light_refl_par_frac * 100);

	/* fraction of light reflected by the soil */
	Light_refl_sw_rad_soil_frac = soil_albedo;
	logger(g_log, "LightReflec_soil = %g %%\n", Light_refl_sw_rad_soil_frac * 100);
	logger(g_log, "******************************************************\n");

	//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
	//fixme following MAESPA (Duursma et al.,) and from Campbell&Norman (2000, p. 259) dominated layers should have just shaded leaves

	/* RADIATION */
	/*****************************************************************************************************************/
	/* first height class in the cell is processed */
	if( !layer_height_class_counter && !cell_height_class_counter )
	{
		/* reset temporary values when the first height class in layer is processed */
		temp_apar = 0.0;
		temp_par_refl = 0.0;
		temp_sw_rad_abs = 0.0;
		temp_sw_rad_refl = 0.0;
		temp_ppfd_abs = 0.0;
		temp_ppfd_refl = 0.0;

		/* assign meteo variables to cell variables */
		/* assign incoming PAR */
		c->par = meteo_daily->par;

		/* assign Shortwave Radiation */
		c->sw_rad_down_W = meteo_daily->sw_downward_W;

		/* assign incoming PPFD */
		c->ppfd = meteo_daily->ppfd;

	}

	/*****************************************************************************************************************/
	/* shared functions among all class/layers */
	/* counters */
	layer_height_class_counter ++;
	cell_height_class_counter++;

	/*************************************************************************/
	/* compute reflected, absorbed and transmitted Par, Short Wave radiation and PPFD class level */
	canopy_sw_band_abs_trans_refl_radiation (c, height, dbh, age, species, Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade,
			Light_refl_par_frac, Light_refl_sw_rad_canopy_frac);

	/*************************************************************************/
	/* temporary absorbed and reflected values */
	/* update temporary absorbed and reflected PAR for lower layer */
	logger(g_log,"\ntemporary cumulated absorbed and reflect light\n");
	temp_apar += s->value[APAR];
	c->apar += s->value[APAR];
	logger(g_log,"cum apar = %g\n", c->apar);
	temp_par_refl += s->value[PAR_REFL];
	c->par_refl += s->value[PAR_REFL];
	logger(g_log,"cum par_refl = %g\n", c->par_refl);

	/* update temporary absorbed and transmitted Short Wave radiation lower layer */
	temp_sw_rad_abs += s->value[SW_RAD_ABS];
	c->sw_rad_abs += s->value[SW_RAD_ABS];
	logger(g_log,"cum sw_rad_abs = %g\n", c->sw_rad_abs);
	temp_sw_rad_refl += s->value[SW_RAD_REFL];
	c->sw_rad_refl += s->value[SW_RAD_REFL];
	logger(g_log,"cum sw_rad_refl = %g\n", c->sw_rad_refl);

	/* update temporary absorbed and transmitted PPFD lower layer */
	temp_ppfd_abs += s->value[PPFD_ABS];
	c->ppfd_abs += s->value[PPFD_ABS];
	logger(g_log,"cum ppfd_abs = %g\n", c->ppfd_abs);
	temp_ppfd_refl += s->value[PPFD_REFL];
	c->ppfd_refl += s->value[PPFD_REFL];
	logger(g_log,"cum ppfd_refl = %g\n", c->ppfd_refl);

	/*****************************************************************************************************************/
	/* when matches the last height class in the layer is processed */
	if ( l->layer_n_height_class == layer_height_class_counter )
	{
		logger(g_log,"\nlast height class in layer processed\n");
		logger(g_log,"update radiation values for lower layer\n");
		/* compute values for lower layer when last height class in layer is processed */
		/* compute par for lower layer */
		c->par -= (temp_apar + temp_par_refl);

		/* compute Short Wave radiation for lower layesr */
		c->sw_rad_down_W -= (temp_sw_rad_abs + temp_sw_rad_refl);

		/* compute ppfd for lower layer */
		c->ppfd -= (temp_ppfd_abs + temp_ppfd_refl);

		/* reset temporary values when the last height class in layer is processed */
		temp_apar = 0.0;
		temp_par_refl = 0.0;
		temp_sw_rad_abs = 0.0;
		temp_sw_rad_refl = 0.0;
		temp_ppfd_abs = 0.0;
		temp_ppfd_refl = 0.0;

		/* reset counter */
		layer_height_class_counter = 0;
	}

	/*************************************************************************/
	/* when matches the last height class in the cell is processed */
	if ( c->heights_count == cell_height_class_counter )
	{
		logger(g_log,"last height class in cell processed\n");

		logger(g_log,"\n***********************************\n");

		/* compute values for soil layer when last height class in cell is processed */
		/* remove reflected part */
		logger(g_log,"incoming light for soil\n");
		logger(g_log,"incoming PAR for soil = %g molPAR/m^2/day\n", c->par);
		logger(g_log,"incoming Short Wave radiation = %g W/m2\n", c->sw_rad_down_W);
		logger(g_log,"incoming PPFD for soil = %g umol/m2/sec\n", c->ppfd);
		c->par_refl_soil = c->par * Light_refl_sw_rad_soil_frac;
		c->sw_rad_for_soil_refl = c->sw_rad_down_W * Light_refl_sw_rad_soil_frac;
		c->ppfd_refl_soil = c->ppfd * Light_refl_sw_rad_soil_frac;
		logger(g_log,"reflected light from the soil\n");
		logger(g_log,"par_refl_soil = %g molPAR/m^2/day\n", c->par_refl_soil);
		logger(g_log,"sw_rad_for_soil_refl = %g W/m2\n", c->sw_rad_for_soil_refl);
		logger(g_log,"ppfd_refl_soil = %g umol/m2/sec\n", c->ppfd_refl_soil);


		/* Par Short Wave radiation and PPFD for the soil during growing season */
		logger(g_log,"incoming light for soil less reflected part\n");
		c->par_for_soil = c->par - c->par_refl_soil;
		c->sw_rad_for_soil = c->sw_rad_down_W - c->sw_rad_for_soil_refl;
		c->ppfd_for_soil = c->ppfd - c->ppfd_refl_soil;
		logger(g_log, "PAR for soil = %g molPAR/m^2/day\n", c->par_for_soil);
		logger(g_log, "Short Wave radiation for soil = %g W/m2\n", c->sw_rad_for_soil);
		logger(g_log, "PPFD for soil = %g umol/m2/sec\n", c->ppfd_for_soil);

		/* reset counter */
		cell_height_class_counter = 0;
	}
	/*****************************************************************************************************************/
}
