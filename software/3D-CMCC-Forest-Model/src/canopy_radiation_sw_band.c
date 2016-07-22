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

void canopy_sw_band_abs_trans_refl_radiation(cell_t *const c, const int height, const int age, const int species, double Light_abs_frac
		, double Light_abs_frac_sun, double Light_abs_frac_shade, double Light_refl_par_frac, double Light_refl_sw_rad_canopy_frac)
{
	double leaf_cell_cover_eff;       /* effective fraction of leaf cover over the cell (ratio) */

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	/* note: This function works at class level computing absorbed transmitted and reflected PAR, NET RADIATION
	 * and PPFD through different height * classes/layers considering at square meter takes into account coverage,
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

	/*compute APAR (molPAR/m^2 covered/day) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE par = %g molPAR/m^2 covered day\n", s->value[PAR]);
	s->value[APAR_SUN] = s->value[PAR] * Light_abs_frac_sun * leaf_cell_cover_eff;
	s->value[TRANSM_PAR_SUN] = s->value[PAR] - s->value[APAR_SUN];
	s->value[APAR_SHADE] = s->value[TRANSM_PAR_SUN] * Light_abs_frac_shade * leaf_cell_cover_eff;
	s->value[TRANSM_PAR_SHADE] = s->value[TRANSM_PAR_SUN] - s->value[APAR_SHADE];
	/* overall canopy */
	s->value[APAR] = s->value[APAR_SUN] + s->value[APAR_SHADE];
	s->value[TRANSM_PAR] = s->value[TRANSM_PAR_SHADE];
	CHECK_CONDITION(fabs((s->value[APAR] + s->value[TRANSM_PAR])-s->value[PAR]),>1e-4);

	logger(g_log, "Apar sun = %g molPAR/m^2 covered/day\n", s->value[APAR_SUN]);
	logger(g_log, "Transmitted Par sun = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR_SUN]);
	logger(g_log, "Apar shade = %g molPAR/m^2 covered/day\n", s->value[APAR_SHADE]);
	logger(g_log, "Transmitted Par shade = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR_SHADE]);
	logger(g_log, "Apar total = %g molPAR/m^2 covered/day\n", s->value[APAR]);
	logger(g_log, "Transmitted Par total = %g molPAR/m^2 covered/day\n", s->value[TRANSM_PAR]);
	logger(g_log, "Reflected Par = %g molPAR/m^2 day\n", s->value[REFL_PAR]);

	/*compute Net Short Wave radiation (W/m^2 covered ) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE Net Short Wave radiation = %g W/m^2 covered\n", s->value[NET_SW_RAD]);
	s->value[NET_SW_RAD_ABS_SUN] = s->value[NET_SW_RAD] * Light_abs_frac_sun * leaf_cell_cover_eff;
	s->value[NET_SW_RAD_TRANSM_SUN] = s->value[NET_SW_RAD] - s->value[NET_SW_RAD_ABS_SUN];
	s->value[NET_SW_RAD_ABS_SHADE] = s->value[NET_SW_RAD_TRANSM_SUN] * Light_abs_frac_shade * leaf_cell_cover_eff;
	s->value[NET_SW_RAD_TRANSM_SHADE] = s->value[NET_SW_RAD_TRANSM_SUN] - s->value[NET_SW_RAD_ABS_SHADE];
	/* overall canopy */
	s->value[NET_SW_RAD_ABS] = s->value[NET_SW_RAD_ABS_SUN] + s->value[NET_SW_RAD_ABS_SHADE];
	s->value[NET_SW_RAD_TRANSM] = s->value[NET_SW_RAD_TRANSM_SHADE];
	CHECK_CONDITION(fabs((s->value[NET_SW_RAD_ABS] + s->value[NET_SW_RAD_TRANSM])-s->value[NET_SW_RAD]),>1e-4);

	logger(g_log, "Absorbed Net Short Wave radiation sun = %g W/m^2 covered\n", s->value[NET_SW_RAD_ABS_SUN]);
	logger(g_log, "Transmitted Net Short Wave radiation sun = %g W/m^2 covered\n", s->value[NET_SW_RAD_TRANSM_SUN]);
	logger(g_log, "Absorbed Net Short Wave radiation shade = %g W/m^2 covered\n", s->value[NET_SW_RAD_ABS_SHADE]);
	logger(g_log, "Transmitted Net Short Wave radiation shade = %g W/m^2 covered\n", s->value[NET_SW_RAD_TRANSM_SHADE]);
	logger(g_log, "Absorbed total = %g W/m^2 covered\n", s->value[NET_SW_RAD_ABS]);
	logger(g_log, "Transmitted total = %g W/m^2 covered\n", s->value[NET_SW_RAD_TRANSM]);

	/* compute PPFD (umol/m^2 covered/sec) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE ppfd = %g umol/m2 covered/sec\n", s->value[PPFD]);
	s->value[PPFD_ABS_SUN] = s->value[PPFD] * Light_abs_frac_sun * leaf_cell_cover_eff;
	s->value[PPFD_TRANSM_SUN] = s->value[PPFD] - s->value[PPFD_ABS_SUN];
	s->value[PPFD_ABS_SHADE] = s->value[PPFD_TRANSM_SUN] * Light_abs_frac_shade* leaf_cell_cover_eff;
	s->value[PPFD_TRANSM_SHADE] = s->value[PPFD_TRANSM_SUN] - s->value[PPFD_ABS_SHADE];
	/* overall canopy */
	s->value[PPFD_ABS] = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
	s->value[PPFD_TRANSM] = s->value[PPFD_TRANSM_SHADE];
	CHECK_CONDITION(fabs((s->value[PPFD_ABS] + s->value[PPFD_TRANSM])-s->value[PPFD]),>1e-4);

	logger(g_log, "Absorbed ppfd sun = %g umol/m2 covered/sec\n", s->value[PPFD_ABS_SUN]);
	logger(g_log, "Transmitted ppfd sun = %g umol/m2 covered/sec\n", s->value[PPFD_TRANSM_SUN]);
	logger(g_log, "Absorbed ppfd shade = %g umol/m2 covered/sec\n", s->value[PPFD_ABS_SHADE]);
	logger(g_log, "Transmitted ppfd shade = %g umol/m2 covered/sec\n", s->value[PPFD_TRANSM_SHADE]);
	logger(g_log, "Absorbed ppfd total  = %g umol/m2 covered/sec\n", s->value[PPFD_ABS]);
	logger(g_log, "Transmitted ppfd total  = %g umol/m2 covered/sec\n", s->value[PPFD_TRANSM]);

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

void canopy_radiation_sw_band(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	static int layer;

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

	static int counter;
	//fixme move soil albedo into soil.txt file
	const double soil_albedo = 0.15;                                                      /* (ratio) soil albedo without snow (see MAESPA model) */

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	//double actual_albedo;

	logger(g_log, "\n**SHORT WAVE BAND RADIATION ROUTINE**\n");

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
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

	if(s->value[LAI] >= 1.0)
	{
		Light_refl_sw_rad_canopy_frac = s->value[ALBEDO];
		//		LightReflec_net_rad_sun = s->value[ALBEDO];
		//		LightReflec_net_rad_shade = s->value[ALBEDO];
		Light_refl_par_frac = s->value[ALBEDO]/3.0;
		//		LightReflec_par_sun = s->value[ALBEDO]/3.0;
		//		LightReflec_par_shade = s->value[ALBEDO]/3.0;
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

	logger(g_log, "Light_abs_frac_sun = %g %%\n", Light_abs_frac_sun);
	logger(g_log, "LightTrasm_sun = %g %%\n", Light_trasm_frac_sun);
	logger(g_log, "Light_abs_frac_shade = %g %%\n", Light_abs_frac_shade);
	logger(g_log, "LightTrasm_sun = %g %%\n", Light_trasm_frac_shade);
	logger(g_log, "Light_abs_frac (sun+shaded) = %g %%\n", Light_abs_frac);
	logger(g_log, "LightTrasm (sun+shaded)= %g %%\n", Light_trasm_frac);
	logger(g_log, "LightReflec_net_rad = %g %%\n", Light_refl_sw_rad_canopy_frac);
	logger(g_log, "LightReflec_par = %g %%\n", Light_refl_par_frac);

	/* fraction of light reflected by the soil */
	Light_refl_sw_rad_soil_frac = soil_albedo;
	logger(g_log, "LightReflec_soil = %g %%\n", Light_refl_par_frac);

	/* assign incoming PAR */
	c->par = meteo_daily->par;

	/* assign incoming PPFD */
	c->ppfd = meteo_daily->ppfd;

	/* RADIATION */
	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{
		//test
		for (layer = c->t_layers_count -1 ; layer >= 0; layer-- )
		{
			//move everything here
			/* special case for layer that use incoming radiation (dominant trees) */
			if(layer == c->t_layers_count -1)
			{

			}
		}
		/*************************************************************************************************************************************************************************************/
		/* dominant layer */
		if(c->heights[height].z == c->heights[height].z)
		{
			logger(g_log, "**LIGHT DOMINANT**\n");
			// ALESSIOC
			//logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			/* reset previous day temporary values */
			//ALESSIOC
			/*
			if (c->dominant_veg_counter == 1)
			{
				counter = 0;

				temp_apar = 0.0;
				temp_par_refl = 0.0;
				temp_sw_rad_abs = 0.0;
				temp_sw_rad_refl = 0.0;
				temp_ppfd_abs = 0.0;
				temp_ppfd_refl = 0.0;
			}
			*/

			/* increment counter */
			counter++;

			/* PAR computation */
			/* Remove the reflected PAR */
			s->value[REFL_PAR] = c->par * Light_refl_par_frac * leaf_cell_cover_eff;
			/*assign to class PAR */
			s->value[PAR] = c->par - s->value[REFL_PAR];
			logger(g_log, "Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);

			/* Net Short Wave Radiation computation */
			//note: as in BIOME_BGC model considers just SHORT WAVE FLUXES
			/* Remove the reflected radiation */
			c->short_wave_radiation_upward_W = meteo_daily->sw_downward_W * Light_refl_sw_rad_canopy_frac * leaf_cell_cover_eff;
			logger(g_log, "Short wave radiation (upward) = %g W/m2\n", c->short_wave_radiation_upward_W);
			/* Net Short Wave radiation */
			c->net_short_wave_radiation_W = meteo_daily->sw_downward_W - c->short_wave_radiation_upward_W;
			logger(g_log, "Net Short wave radiation = %g W/m2\n", c->net_short_wave_radiation_W);
			/* Net Short Wave radiation */
			s->value[SW_RAD_REFL] = c->short_wave_radiation_upward_W;
			/*assign to class Net Short Wave Radiation */
			s->value[NET_SW_RAD] = (meteo_daily->sw_downward_W - s->value[SW_RAD_REFL]);
			logger(g_log, "Net Short Wave Radiation less reflected part = %g W/m2\n", s->value[NET_SW_RAD]);
			/* assign to cell Net Short Wave Radiation */
			c->net_sw_rad = s->value[NET_SW_RAD];

			/* PPFD computation */
			/* Remove the reflected PPFD */
			s->value[PPFD_REFL] = c->ppfd * Light_refl_par_frac * leaf_cell_cover_eff;
			/*assign to class PPFD */
			s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];
			logger(g_log, "PPFD less reflected part = %g umol/m^2/sec\n", s->value[PPFD]);

			/* compute absorbed and transmitted Par, Short Wave radiation and ppfd class level */
			canopy_sw_band_abs_trans_refl_radiation (c, height, age, species, Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade,
					Light_refl_par_frac, Light_refl_sw_rad_canopy_frac);

			/* update temporary absorbed and reflected PAR for lower layer */
			temp_apar += s->value[APAR];
			c->apar += s->value[APAR];
			temp_par_refl += s->value[REFL_PAR];
			c->par_reflected += s->value[REFL_PAR];

			/* update temporary absorbed and transmitted Net Short Wave radiation lower layer */
			temp_sw_rad_abs += s->value[NET_SW_RAD_ABS];
			c->net_sw_rad_abs += s->value[NET_SW_RAD_ABS];
			temp_sw_rad_refl += s->value[REFL_PAR];
			c->sw_rad_refl += s->value[SW_RAD_REFL];
			logger(g_log, "net_radiation_reflected = %g \n", s->value[SW_RAD_REFL] );

			/* update temporary absorbed and transmitted PPFD lower layer */
			temp_ppfd_abs += s->value[PPFD_ABS];
			c->ppfd_abs += s->value[PPFD_ABS];
			temp_ppfd_refl += s->value[PPFD_REFL];
			c->ppfd_reflected += s->value[PPFD_REFL];

			/* compute PAR Net Short Wave radiation and PPFD for lower layers at the last height class processed */
			//ALESSIOC
			//if(counter == c->dominant_veg_counter)
			//{
			//	/* compute par for lower layer */
			//	c->par -= (temp_apar + temp_par_refl);

			//	/* compute Net Short Wave radiation for lower layer */
			//	c->net_sw_rad -= (temp_sw_rad_abs/* + temp_net_radiation_reflected*/);

			//	/* compute ppfd for lower layer */
			//	c->ppfd -= (temp_ppfd_abs + temp_ppfd_refl);
			//}

			/* assign to soil layer if no other height classes in dominated layer */
			// ALESSIOC
			//if(c->height_class_in_layer_dominated_counter == 0)
			//{
			//	/* remove reflected part */
			//	c->par_reflected_soil = c->par * Light_refl_sw_rad_soil_frac;
			//	c->sw_rad_for_soil_refl = c->net_sw_rad * Light_refl_sw_rad_soil_frac;
			//	c->ppfd_reflected_soil = c->ppfd * Light_refl_sw_rad_soil_frac;

			//	/* Par Net Short Wave radiation and PPFD for the soil during growing season */
			//	c->par_for_soil = c->par - c->par_reflected_soil;
			//	c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
			//	c->ppfd_for_soil = c->ppfd - c->ppfd_reflected_soil;
			//	logger(g_log, "PAR for soil during growing season = %g molPAR/m^2/day\n", c->par_for_soil);
			//	logger(g_log, "Net Short Wave radiation for soil during growing season = %g W/m2\n", c->net_sw_rad_for_soil);
			//	logger(g_log, "PPFD for soil during growing season = %g W/m2\n", c->ppfd_for_soil);
			//}
		}
		/*************************************************************************************************************************************************************************************/
		/* dominated layer */
		// ALESSIOC: STESSA CONDIZIONE IF!!!!!
		//else if (c->heights[height].z == c->t_layers[layer].z)
		else if(c->heights[height].z == c->heights[height].z)
		{
			//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
			//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell & Norman (2000, p. 259)
			logger(g_log, "**LIGHT DOMINATED**\n");
			// ALESSIOC
			//logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominated_counter);

			/* reset previous day temporary values */
			//ALESSIOC
			/*
			if (c->dominant_veg_counter == 1)
			{
				counter = 0;

				temp_apar = 0.0;
				temp_par_refl = 0.0;
				temp_sw_rad_abs = 0.0;
				temp_sw_rad_refl = 0.0;
				temp_ppfd_abs = 0.0;
				temp_ppfd_refl = 0.0;
			}
			*/
			/* increment counter */
			counter++;

			/* PAR computation */
			/* Remove the reflected PAR */
			s->value[REFL_PAR] = c->par * Light_refl_par_frac * leaf_cell_cover_eff;
			/*assign to class PAR */
			s->value[PAR] = c->par - s->value[REFL_PAR];
			logger(g_log, "Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);

			/* Net Short Wave Radiation computation */
			/* Remove the reflected radiation */
			s->value[SW_RAD_REFL] = c->net_sw_rad *  Light_refl_sw_rad_canopy_frac * leaf_cell_cover_eff;
			/*assign to class Net Short Wave Radiation */
			s->value[NET_SW_RAD] = c->net_sw_rad - s->value[SW_RAD_REFL];
			logger(g_log, "Net Short Wave radiation less reflected part = %g W/m2\n", s->value[NET_SW_RAD]);

			/* PPFD computation */
			/* Remove the reflected PPFD */
			s->value[PPFD_REFL] = c->ppfd * Light_refl_par_frac * leaf_cell_cover_eff;
			/*assign to class PPFD */
			s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];
			logger(g_log, "PPFD less reflected part = %g umol/m^2/sec\n", s->value[PPFD]);

			/* compute absorbed and transmitted Par, Net Short Wave radiation and ppfd class level */
			canopy_sw_band_abs_trans_refl_radiation (c, height, age, species, Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade,
								Light_refl_par_frac, Light_refl_sw_rad_canopy_frac);

			/* update temporary absorbed and reflected PAR for lower layer */
			temp_apar += s->value[APAR];
			c->apar += s->value[APAR];
			temp_par_refl += s->value[REFL_PAR];
			c->par_reflected += s->value[REFL_PAR];

			/* update temporary absorbed and transmitted Net Short Wave radiation lower layer */
			temp_sw_rad_abs += s->value[NET_SW_RAD_ABS];
			c->net_sw_rad_abs += s->value[NET_SW_RAD_ABS];
			temp_sw_rad_refl += s->value[REFL_PAR];
			c->sw_rad_refl += s->value[SW_RAD_REFL];

			/* update temporary absorbed and transmitted PPFD lower layer */
			temp_ppfd_abs += s->value[PPFD_ABS];
			c->ppfd_abs += s->value[PPFD_ABS];
			temp_ppfd_refl += s->value[PPFD_REFL];
			c->ppfd_reflected += s->value[PPFD_REFL];

			/* compute PAR for lower layers at the last height class processed */
			//ALESSIOC
			//if(counter == c->dominated_veg_counter)
			//{
			//	/* compute par for lower layer */
			//	c->par -= (temp_apar + temp_par_refl);

			//	/* compute Net Short Wave radiation for lower layer */
			//	c->net_sw_rad -= (temp_sw_rad_abs + temp_sw_rad_refl);

			//	/* compute ppfd for lower layer */
			//	c->ppfd -= (temp_ppfd_abs + temp_ppfd_refl);
			//}
			/* assign to soil layer if no other height classes in dominated layer */
			//if(c->height_class_in_layer_subdominated_counter == 0)
			//{
			//	/* remove reflected part */
			//	c->par_reflected_soil = c->par * Light_refl_sw_rad_soil_frac;
			//	c->sw_rad_for_soil_refl = c->net_sw_rad * Light_refl_sw_rad_soil_frac;
			//	c->ppfd_reflected_soil = c->ppfd * Light_refl_sw_rad_soil_frac;

			//	/* Par, Net Short Wave radiation and PPFD for the soil during growing season */
			//	c->par_for_soil = c->par - c->par_reflected_soil;
			//	c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
			//	c->ppfd_for_soil = c->ppfd - c->ppfd_reflected_soil;
			//	logger(g_log, "PAR for soil during growing season = %g molPAR/m^2/day\n", c->par_for_soil);
			//	logger(g_log, "Net Short Wave radiation for soil during growing season = %g W/m2\n", c->net_sw_rad_for_soil);
			//	logger(g_log, "PPFD for soil during growing season = %g W/m2\n", c->ppfd_for_soil);
			//}
		}
		/*******************************************************************************************************************************************************************************/
		/* sub-dominated layer */
		else
		{
			//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
			//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell&Norman (2000, p. 259)
			logger(g_log, "**LIGHT SUB-DOMINATED**\n");
			// ALESSIOC
			//logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_subdominated_counter);

			/* assign PAR at the first class considering absorbed and transmitted par from above layer and reset previous day temporary values */
			//ALESSIOC
			/*
			if (c->subdominated_veg_counter == 1)
			{
				counter = 0;

				temp_apar = 0.0;
				temp_par_refl = 0.0;
				temp_sw_rad_abs = 0.0;
				temp_sw_rad_refl = 0.0;
				temp_ppfd_abs = 0.0;
				temp_ppfd_refl = 0.0;
			}
			*/
			counter++;

			/* PAR computation */
			/* Remove the reflected PAR */
			s->value[REFL_PAR] = c->par * Light_refl_par_frac * leaf_cell_cover_eff;
			/*assign to class PAR */
			s->value[PAR] = c->par - s->value[REFL_PAR];
			logger(g_log, "Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);

			/* Net Short Wave Radiation computation */
			/* Remove the reflected radiation */
			s->value[SW_RAD_REFL] = c->net_sw_rad * Light_refl_sw_rad_canopy_frac * leaf_cell_cover_eff;
			/*assign to class Net Short Wave Radiation */
			s->value[NET_SW_RAD] = c->net_sw_rad - s->value[SW_RAD_REFL];
			logger(g_log, "Net Short Wave radiation less reflected part = %g W/m2\n", s->value[NET_SW_RAD]);

			/* PPFD computation */
			/* Remove the reflected PPFD */
			s->value[PPFD_REFL] = c->ppfd * Light_refl_par_frac * leaf_cell_cover_eff;
			/*assign to class PPFD */
			s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];
			logger(g_log, "PPFD less reflected part = %g umol/m^2/sec\n", s->value[PPFD]);

			/* compute absorbed and transmitted Par, Net Short Wave radiation and ppfd class level */
			canopy_sw_band_abs_trans_refl_radiation (c, height, age, species, Light_abs_frac, Light_abs_frac_sun, Light_abs_frac_shade,
								Light_refl_par_frac, Light_refl_sw_rad_canopy_frac);

			/* update temporary absorbed and reflected PAR for lower layer */
			temp_apar += s->value[APAR];
			c->apar += s->value[APAR];
			temp_par_refl += s->value[REFL_PAR];
			c->par_reflected += s->value[REFL_PAR];

			/* update temporary absorbed and transmitted Net Short Wave radiation lower layer */
			temp_sw_rad_abs += s->value[NET_SW_RAD_ABS];
			c->net_sw_rad_abs += s->value[NET_SW_RAD_ABS];
			temp_sw_rad_refl += s->value[REFL_PAR];
			c->sw_rad_refl += s->value[SW_RAD_REFL];

			/* update temporary absorbed and transmitted PPFD lower layer */
			temp_ppfd_abs += s->value[PPFD_ABS];
			c->ppfd_abs += s->value[PPFD_ABS];
			temp_ppfd_refl += s->value[PPFD_REFL];
			c->ppfd_reflected += s->value[PPFD_REFL];

			/* compute PAR for lower layers at the last height class processed */
			//if(counter == c->subdominated_veg_counter)
			//{
			//	/* compute par for lower layer */
			//	c->par -= (temp_apar + temp_par_refl);

			//	/* compute Net Short Wave radiation for lower layer */
			//	c->net_sw_rad -= (temp_sw_rad_abs + temp_sw_rad_refl);

			//	/* compute ppfd for lower layer */
			//	c->ppfd -= (temp_ppfd_abs + temp_ppfd_refl);
			//}

			/* assign to soil layer if no other height classes in dominated layer */
			// ALESSIOC
			//if(c->height_class_in_layer_subdominated_counter == 0)
			//{
			//	/* remove reflected part */
			//	c->par_reflected_soil = c->par * Light_refl_sw_rad_soil_frac;
			//	c->sw_rad_for_soil_refl = c->net_sw_rad * Light_refl_sw_rad_soil_frac;
			//	c->ppfd_reflected_soil = c->ppfd * Light_refl_sw_rad_soil_frac;

			//	/* Par, Net Short Wave radiation and PPFD for the soil during growing season */
			//	c->par_for_soil = c->par - c->par_reflected_soil;
			//	c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
			//	c->ppfd_for_soil = c->ppfd - c->ppfd_reflected_soil;
			//	logger(g_log, "PAR for soil during growing season = %g molPAR/m^2/day\n", c->par_for_soil);
			//	logger(g_log, "Net Short Wave radiation for soil during growing season = %g W/m2\n", c->net_sw_rad_for_soil);
			//	logger(g_log, "PPFD for soil during growing season = %g W/m2\n", c->ppfd_for_soil);
			//}
		}
	}
	/**********************************************************************************************************************************************************************************/
	/* outside growing season */
	else
	{
		logger(g_log, "\n**LIGHT FOR SOIL LAYER (outside the growing season)**\n");

		/* Remove the reflected PAR */
		c->par_reflected_soil = meteo_daily->par * Light_refl_sw_rad_soil_frac;

		/* Remove the reflected radiation */
		c->short_wave_radiation_upward_W = meteo_daily->sw_downward_W * Light_refl_sw_rad_soil_frac;
		logger(g_log, "Short wave radiation (upward) = %g W/m2\n", c->short_wave_radiation_upward_W);

		/* Net Short Wave Short Wave radiation */
		c->net_short_wave_radiation_W = meteo_daily->sw_downward_W - c->short_wave_radiation_upward_W;
		logger(g_log, "Net Short wave radiation = %g W/m2\n", c->net_short_wave_radiation_W);

		c->sw_rad_for_soil_refl = c->short_wave_radiation_upward_W;

		/* Remove the reflected PPFD */
		c->ppfd_reflected_soil = meteo_daily->ppfd * Light_refl_sw_rad_soil_frac;

		/* compute radiation absorbed by the soil */
		/* Par and Net Short Wave radiation for the soil outside growing season (bare soil condition) */
		c->par_for_soil = meteo_daily->par - c->par_reflected_soil;
		c->net_sw_rad_for_soil = meteo_daily->sw_downward_W - c->short_wave_radiation_upward_W;
		c->ppfd_for_soil = meteo_daily->ppfd - c->ppfd_reflected_soil;
		logger(g_log, "PAR for soil outside growing season = %g \n", c->par_for_soil);
		logger(g_log, "Net Short Wave radiation for soil outside growing season = %g \n", c->net_sw_rad_for_soil);
		logger(g_log, "PPFD for soil outside growing season = %g \n", c->ppfd_for_soil);
	}

	//if(month == 7)getchar();
}
