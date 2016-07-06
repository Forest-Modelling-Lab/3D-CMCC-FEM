/*
 * canopy_radiation.c
 *
 *  Created on: 05 lug 2016
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "light.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_settings.h"
#include "topo.h"

extern logger_t* g_log;

void new_canopy_abs_transm_refl_radiation (cell_t *const c, species_t *const s, double LightAbsorb_sun, double LightAbsorb_shade, double LightReflec_par, double LightReflec_net_rad)
{
	double leaf_cell_cover_eff;       /* effective fraction of leaf cover over the cell (ratio) */

	/* note: This function works at class level computing absorbed transmitted and reflected PAR, NET RADIATION
	 * and PPFD through different height * classes/layers considering at square meter takes into account coverage,
	 * it means that a square meter grid cell * represents overall grid cell (see Duursma and Makela, 2007) */

	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;

	/*compute APAR (molPAR/m^2 covered/day) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE par = %g molPAR/m^2 covered day\n", s->value[PAR]);
	s->value[APAR_SUN] = s->value[PAR] * LightAbsorb_sun * leaf_cell_cover_eff;
	s->value[TRANSM_PAR_SUN] = s->value[PAR] - s->value[APAR_SUN];
	s->value[APAR_SHADE] = s->value[TRANSM_PAR_SUN] * LightAbsorb_shade * leaf_cell_cover_eff;
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

	/*compute NetRad (W/m^2 covered ) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE net_radiation = %g W/m^2 covered\n", s->value[NET_RAD]);
	s->value[NET_RAD_ABS_SUN] = s->value[NET_RAD] * LightAbsorb_sun * leaf_cell_cover_eff;
	s->value[NET_RAD_TRANSM_SUN] = s->value[NET_RAD] - s->value[NET_RAD_ABS_SUN];
	s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_TRANSM_SUN] * LightAbsorb_shade * leaf_cell_cover_eff;
	s->value[NET_RAD_TRANSM_SHADE] = s->value[NET_RAD_TRANSM_SUN] - s->value[NET_RAD_ABS_SHADE];
	/* overall canopy */
	s->value[NET_RAD_ABS] = s->value[NET_RAD_ABS_SUN] + s->value[NET_RAD_ABS_SHADE];
	s->value[NET_RAD_TRANSM] = s->value[NET_RAD_TRANSM_SHADE];
	CHECK_CONDITION(fabs((s->value[NET_RAD_ABS] + s->value[NET_RAD_TRANSM])-s->value[NET_RAD]),>1e-4);

	logger(g_log, "Absorbed NetRad sun = %g W/m^2 covered\n", s->value[NET_RAD_ABS_SUN]);
	logger(g_log, "Transmitted NetRad sun = %g W/m^2 covered\n", s->value[NET_RAD_TRANSM_SUN]);
	logger(g_log, "Absorbed NetRad shade = %g W/m^2 covered\n", s->value[NET_RAD_ABS_SHADE]);
	logger(g_log, "Transmitted NetRad shade = %g W/m^2 covered\n", s->value[NET_RAD_TRANSM_SHADE]);
	logger(g_log, "Absorbed total = %g W/m^2 covered\n", s->value[NET_RAD_ABS]);
	logger(g_log, "Transmitted total = %g W/m^2 covered\n", s->value[NET_RAD_TRANSM]);

	/* compute PPFD (umol/m^2 covered/sec) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE ppfd = %g umol/m2 covered/sec\n", s->value[PPFD]);
	s->value[PPFD_ABS_SUN] = s->value[PPFD] * LightAbsorb_sun * leaf_cell_cover_eff;
	s->value[PPFD_TRANSM_SUN] = s->value[PPFD] - s->value[PPFD_ABS_SUN];
	s->value[PPFD_ABS_SHADE] = s->value[PPFD_TRANSM_SUN] * LightAbsorb_shade* leaf_cell_cover_eff;
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
}

void new_canopy_radiation (species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species)
{
	double LightAbsorb, LightAbsorb_sun, LightAbsorb_shade;                               /* (ratio) fraction of light absorbed */
	double LightTransm, LightTransm_sun, LightTransm_shade;                               /* (ratio) fraction of light transmitted */
	double LightReflec_net_rad;                                                           /* (ratio) fraction of light reflected (for net radiation) */
	double LightReflec_par;                                                               /* (ratio) fraction of light reflected (for par) */
	double LightReflec_soil;                                                              /* (ratio) fraction of light reflected (for soil) */
	double leaf_cell_cover_eff;                                                           /* (ratio) fraction of square meter covered by leaf over the grid cell */

	static double temp_apar;                                                              /* temporary absorbed PAR for layer */
	static double temp_par_reflected;                                                     /* temporary reflected PAR for layer */
	static double temp_net_radiation_abs;                                                 /* temporary absorbed Net Radiation for layer */
	static double temp_net_radiation_reflected;                                           /* temporary reflected Net Radiation for layer */
	static double temp_ppfd_abs;                                                          /* temporary absorbed PPFD for layer */
	static double temp_ppfd_reflected;                                                    /* temporary reflected PPFD for layer */

	static int counter;


	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	//double actual_albedo;

	//fixme move soil albedo into soil.txt file
	//test check albedo for other typos
	double soil_albedo = 0.17;

	logger(g_log, "\n**RADIATION ROUTINE**\n");
	logger(g_log, "-INCOMING RADIATION\n");
	logger(g_log, "-extra terrestrial radiation = %g (MJ/m^2/day)\n", met[month].d[day].extra_terr_rad_MJ);
	logger(g_log, "-extra terrestrial radiation = %g (W/m2)\n", met[month].d[day].extra_terr_rad_W);
	logger(g_log, "-Short wave clear_sky_radiation = %g (MJ/m^2/day)\n", met[month].d[day].sw_clear_sky_MJ);
	logger(g_log, "-Short wave clear_sky_radiation = %g (W/m2)\n", met[month].d[day].sw_clear_sky_W);
	logger(g_log, "-Short_wave_radiation (downward) = %g MJ/m^2 day\n", met[month].d[day].sw_downward_MJ);
	logger(g_log, "-Short wave radiation (downward) = %g W/m2\n", met[month].d[day].sw_downward_W);
	logger(g_log, "-Net Long wave radiation = %g MJ/m^2 day\n", met[month].d[day].lw_net_MJ);
	logger(g_log, "-Net Long wave radiation = %g W/m2\n", met[month].d[day].lw_net_W);
	logger(g_log, "-cloud_cover_frac = %g %%\n", met[month].d[day].cloud_cover_frac * 100.0);

	/* compute fractions of light intercepted, transmitted and reflected from the canopy */
	/* fraction of light transmitted through the canopy */
	LightTransm = (exp(- s->value[K] * s->value[LAI]));
	LightTransm_sun = (exp(- s->value[K] * s->value[LAI_SUN]));
	LightTransm_shade = (exp(- s->value[K] * s->value[LAI_SHADE]));

	/* fraction of light absorbed by the canopy */
	LightAbsorb = 1.0 - LightTransm;
	LightAbsorb_sun = 1.0 - LightTransm_sun;
	LightAbsorb_shade = 1.0 - LightTransm_shade;

	/* fraction of light reflected by the canopy */
	/* for net radiation and par */
	//following BIOME albedo for PAR is 1/3 of albedo
	//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
	//PAR is reflected than net_radiation (Jones 1992)
	if(s->value[LAI] >= 1.0)
	{
		LightReflec_net_rad = s->value[ALBEDO];
		//		LightReflec_net_rad_sun = s->value[ALBEDO];
		//		LightReflec_net_rad_shade = s->value[ALBEDO];
		LightReflec_par = s->value[ALBEDO]/3.0;
		//		LightReflec_par_sun = s->value[ALBEDO]/3.0;
		//		LightReflec_par_shade = s->value[ALBEDO]/3.0;
	}
	else
	{
		LightReflec_net_rad = s->value[ALBEDO] * s->value[LAI];
		//		LightReflec_net_rad_sun = s->value[ALBEDO]* s->value[LAI_SUN];
		//		LightReflec_net_rad_shade = s->value[ALBEDO] * * s->value[LAI_SHADE];
		LightReflec_par = (s->value[ALBEDO]/3.0) * s->value[LAI];
		//		LightReflec_par_sun =  (s->value[ALBEDO]/3.0) * s->value[LAI_SUN];
		//		LightReflec_par_shade =  (s->value[ALBEDO]/3.0) * s->value[LAI_SHADE];
	}

	logger(g_log, "LightAbsorb_sun = %g %%\n", LightAbsorb_sun);
	logger(g_log, "LightTrasm_sun = %g %%\n", LightTransm_sun);
	logger(g_log, "LightAbsorb_shade = %g %%\n", LightAbsorb_shade);
	logger(g_log, "LightTrasm_sun = %g %%\n", LightTransm_shade);
	logger(g_log, "LightAbsorb (sun+shaded) = %g %%\n", LightAbsorb);
	logger(g_log, "LightTrasm (sun+shaded)= %g %%\n", LightTransm);
	logger(g_log, "LightReflec_net_rad = %g %%\n", LightReflec_net_rad);
	logger(g_log, "LightReflec_par = %g %%\n", LightReflec_par);

	/* fraction of light reflected by the soil */
	LightReflec_soil = soil_albedo;
	logger(g_log, "LightReflec_soil = %g %%\n", LightReflec_par);

	/***********************************************************************************************************/
	/* compute effective canopy cover */
	/* special case when LAI = < 1.0 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
	else leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
	logger(g_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);

	/***********************************************************************************************************/

	/* assign incoming PAR */
	c->par = met[month].d[day].par;

	/* assign incoming PPFD */
	c->ppfd = met[month].d[day].ppfd;

	/* RADIATION */
	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{
		/*************************************************************************************************************************************************************************************/
		/* dominant layer */
		if ( c->heights[height].z == c->top_layer )
		{
			logger(g_log, "**LIGHT DOMINANT**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			/* reset previous day temporary values */
			if (c->dominant_veg_counter == 1)
			{
				counter = 0;
				temp_apar = 0.0;
				temp_par_reflected = 0.0;
				temp_net_radiation_abs = 0.0;
				temp_net_radiation_reflected = 0.0;
				temp_ppfd_abs = 0.0;
				temp_ppfd_reflected = 0.0;
			}
			/* increment counter */
			counter++;

			/* PAR computation */
			/* Remove the reflected PAR */
			s->value[REFL_PAR] = c->par * LightReflec_par * leaf_cell_cover_eff;
			/*assign to class PAR */
			s->value[PAR] = c->par - s->value[REFL_PAR];
			logger(g_log, "Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);

			/* Net Radiation computation */
			//note: as in BIOME_BGC model considers just SHORT WAVE FLUXES
			/* Remove the reflected radiation */
			c->short_wave_radiation_upward_W = met[month].d[day].sw_downward_W * LightReflec_net_rad * leaf_cell_cover_eff;
			logger(g_log, "Short wave radiation (upward) = %g W/m2\n", c->short_wave_radiation_upward_W);
			/* Net Short Wave radiation */
			c->net_short_wave_radiation_W = met[month].d[day].sw_downward_W - c->short_wave_radiation_upward_W;
			logger(g_log, "Net Short wave radiation = %g W/m2\n", c->net_short_wave_radiation_W);
			/* Net Radiation */
			s->value[NET_RAD_REFL] = c->short_wave_radiation_upward_W;
			/*assign to class Net Radiation */
			s->value[NET_RAD] = (met[month].d[day].sw_downward_W - s->value[NET_RAD_REFL]);
			logger(g_log, "Net Short Wave Radiation less reflected part = %g W/m2\n", s->value[NET_RAD]);
			/* assign to cell Net Radiation */
			c->net_radiation = s->value[NET_RAD];

			/* PPFD computation */
			/* Remove the reflected PPFD */
			s->value[PPFD_REFL] = c->ppfd * LightReflec_par * leaf_cell_cover_eff;
			/*assign to class PPFD */
			s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];
			logger(g_log, "PPFD less reflected part = %g umol/m^2/sec\n", s->value[PPFD]);

			/* compute absorbed and transmitted Par, Net radiation and ppfd class level */
			new_canopy_abs_transm_refl_radiation (c, &c->heights[height].ages[age].species[species], LightAbsorb_sun, LightAbsorb_shade, LightReflec_par, LightReflec_net_rad);

			/* update temporary absorbed and reflected PAR for lower layer */
			temp_apar += s->value[APAR];
			c->apar += s->value[APAR];
			temp_par_reflected += s->value[REFL_PAR];
			c->par_reflected += s->value[REFL_PAR];

			/* update temporary absorbed and transmitted par net radiation lower layer */
			temp_net_radiation_abs += s->value[NET_RAD_ABS];
			c->net_radiation_absorbed += s->value[NET_RAD_ABS];
			temp_net_radiation_reflected += s->value[REFL_PAR];
			c->net_radiation_reflected += s->value[NET_RAD_REFL];
			logger(g_log, "net_radiation_reflected = %g \n", s->value[NET_RAD_REFL] );
			//getchar();

			/* update temporary absorbed and transmitted par net radiation lower layer */
			temp_ppfd_abs += s->value[PPFD_ABS];
			c->ppfd_abs += s->value[PPFD_ABS];
			temp_ppfd_reflected += s->value[PPFD_REFL];
			c->ppfd_reflected += s->value[PPFD_REFL];

			/* compute PAR for lower layers at the last height class processed */
			if(counter == c->dominant_veg_counter)
			{
				/* compute par for lower layer */
				c->par -= (temp_apar + temp_par_reflected);

				/* compute net radiation for lower layer */
				c->net_radiation -= (temp_net_radiation_abs/* + temp_net_radiation_reflected*/);

				/* compute ppfd for lower layer */
				c->ppfd -= (temp_ppfd_abs + temp_ppfd_reflected);
			}

			/* assign to soil layer if no other height classes in dominated layer */
			if(c->height_class_in_layer_dominated_counter == 0)
			{
				/* remove reflected part */
				c->par_reflected_soil = c->par * LightReflec_soil;
				c->net_radiation_for_soil_reflected = c->net_radiation * LightReflec_soil;
				c->ppfd_reflected_soil = c->ppfd * LightReflec_soil;

				/* Par and net radiation for the soil during growing season */
				c->par_for_soil = c->par - c->par_reflected_soil;
				c->net_radiation_for_soil = c->net_radiation - c->net_radiation_for_soil_reflected;
				c->ppfd_for_soil = c->ppfd - c->ppfd_reflected_soil;
				logger(g_log, "PAR for soil during growing season = %g molPAR/m^2/day\n", c->par_for_soil);
				logger(g_log, "Net Radiation for soil during growing season = %g W/m2\n", c->net_radiation_for_soil);
				logger(g_log, "PPFD for soil during growing season = %g W/m2\n", c->ppfd_for_soil);
			}
		}
		/*************************************************************************************************************************************************************************************/
		/* dominated layer */
		else if (c->heights[height].z == c->top_layer - 1)
		{
			//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
			//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell & Norman (2000, p. 259)
			logger(g_log, "**LIGHT DOMINATED**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominated_counter);

			/* reset previous day temporary values */
			if (c->dominant_veg_counter == 1)
			{
				counter = 0;
				temp_apar = 0.0;
				temp_par_reflected = 0.0;
				temp_net_radiation_abs = 0.0;
				temp_net_radiation_reflected = 0.0;
				temp_ppfd_abs = 0.0;
				temp_ppfd_reflected = 0.0;
			}
			/* increment counter */
			counter++;

			/* PAR computation */
			/* Remove the reflected PAR */
			s->value[REFL_PAR] = c->par * LightReflec_par * leaf_cell_cover_eff;
			/*assign to class PAR */
			s->value[PAR] = c->par - s->value[REFL_PAR];
			logger(g_log, "Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);

			/* Net Radiation computation */
			/* Remove the reflected radiation */
			s->value[NET_RAD_REFL] = c->net_radiation *  LightReflec_net_rad * leaf_cell_cover_eff;
			/*assign to class Net Radiation */
			s->value[NET_RAD] = c->net_radiation - s->value[NET_RAD_REFL];
			logger(g_log, "Net Radiation less reflected part = %g W/m2\n", s->value[NET_RAD]);

			/* PPFD computation */
			/* Remove the reflected PPFD */
			s->value[PPFD_REFL] = c->ppfd * LightReflec_par * leaf_cell_cover_eff;
			/*assign to class PPFD */
			s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];
			logger(g_log, "PPFD less reflected part = %g umol/m^2/sec\n", s->value[PPFD]);

			/* compute absorbed and transmitted Par, Net radiation and ppfd class level */
			new_canopy_abs_transm_refl_radiation (c, &c->heights[height].ages[age].species[species], LightAbsorb_sun, LightAbsorb_shade, LightReflec_par, LightReflec_net_rad);

			/* update temporary absorbed and reflected PAR for lower layer */
			temp_apar += s->value[APAR];
			c->apar += s->value[APAR];
			temp_par_reflected += s->value[REFL_PAR];
			c->par_reflected += s->value[REFL_PAR];

			/* update temporary absorbed and transmitted par net radiation lower layer */
			temp_net_radiation_abs += s->value[NET_RAD_ABS];
			c->net_radiation_absorbed += s->value[NET_RAD_ABS];
			temp_net_radiation_reflected += s->value[REFL_PAR];
			c->net_radiation_reflected += s->value[NET_RAD_REFL];

			/* update temporary absorbed and transmitted par net radiation lower layer */
			temp_ppfd_abs += s->value[PPFD_ABS];
			c->ppfd_abs += s->value[PPFD_ABS];
			temp_ppfd_reflected += s->value[PPFD_REFL];
			c->ppfd_reflected += s->value[PPFD_REFL];

			/* compute PAR for lower layers at the last height class processed */
			if(counter == c->dominated_veg_counter)
			{
				/* compute par for lower layer */
				c->par -= (temp_apar + temp_par_reflected);

				/* compute net radiation for lower layer */
				c->net_radiation -= (temp_net_radiation_abs + temp_net_radiation_reflected);

				/* compute ppfd for lower layer */
				c->ppfd -= (temp_ppfd_abs + temp_ppfd_reflected);
			}
			/* assign to soil layer if no other height classes in dominated layer */
			if(c->height_class_in_layer_subdominated_counter == 0)
			{
				/* remove reflected part */
				c->par_reflected_soil = c->par * LightReflec_soil;
				c->net_radiation_for_soil_reflected = c->net_radiation * LightReflec_soil;
				c->ppfd_reflected_soil = c->ppfd * LightReflec_soil;

				/* Par and net radiation for the soil during growing season */
				c->par_for_soil = c->par - c->par_reflected_soil;
				c->net_radiation_for_soil = c->net_radiation - c->net_radiation_for_soil_reflected;
				c->ppfd_for_soil = c->ppfd - c->ppfd_reflected_soil;
				logger(g_log, "PAR for soil during growing season = %g molPAR/m^2/day\n", c->par_for_soil);
				logger(g_log, "Net Radiation for soil during growing season = %g W/m2\n", c->net_radiation_for_soil);
				logger(g_log, "PPFD for soil during growing season = %g W/m2\n", c->ppfd_for_soil);
			}
		}
		/*******************************************************************************************************************************************************************************/
		/* sub-dominated layer */
		else
		{
			//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
			//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell&Norman (2000, p. 259)
			logger(g_log, "**LIGHT SUB-DOMINATED**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_subdominated_counter);

			/* assign PAR at the first class considering absorbed and transmitted par from above layer and reset previous day temporary values */
			if (c->subdominated_veg_counter == 1)
			{
				counter = 0;
				temp_apar = 0.0;
				temp_par_reflected = 0.0;
				temp_net_radiation_abs = 0.0;
				temp_net_radiation_reflected = 0.0;
				temp_ppfd_abs = 0.0;
				temp_ppfd_reflected = 0.0;
			}
			counter++;

			/* PAR computation */
			/* Remove the reflected PAR */
			s->value[REFL_PAR] = c->par * LightReflec_par * leaf_cell_cover_eff;
			/*assign to class PAR */
			s->value[PAR] = c->par - s->value[REFL_PAR];
			logger(g_log, "Par less reflected part = %g molPAR/m^2/day\n", s->value[PAR]);

			/* Net Radiation computation */
			/* Remove the reflected radiation */
			s->value[NET_RAD_REFL] = c->net_radiation *  LightReflec_net_rad * leaf_cell_cover_eff;
			/*assign to class Net Radiation */
			s->value[NET_RAD] = c->net_radiation - s->value[NET_RAD_REFL];
			logger(g_log, "Net Radiation less reflected part = %g W/m2\n", s->value[NET_RAD]);

			/* PPFD computation */
			/* Remove the reflected PPFD */
			s->value[PPFD_REFL] = c->ppfd * LightReflec_par * leaf_cell_cover_eff;
			/*assign to class PPFD */
			s->value[PPFD] = c->ppfd - s->value[PPFD_REFL];
			logger(g_log, "PPFD less reflected part = %g umol/m^2/sec\n", s->value[PPFD]);

			/* compute absorbed and transmitted Par, Net radiation and ppfd class level */
			new_canopy_abs_transm_refl_radiation (c, &c->heights[height].ages[age].species[species], LightAbsorb_sun, LightAbsorb_shade, LightReflec_par, LightReflec_net_rad);

			/* update temporary absorbed and reflected PAR for lower layer */
			temp_apar += s->value[APAR];
			c->apar += s->value[APAR];
			temp_par_reflected += s->value[REFL_PAR];
			c->par_reflected += s->value[REFL_PAR];

			/* update temporary absorbed and transmitted par net radiation lower layer */
			temp_net_radiation_abs += s->value[NET_RAD_ABS];
			c->net_radiation_absorbed += s->value[NET_RAD_ABS];
			temp_net_radiation_reflected += s->value[REFL_PAR];
			c->net_radiation_reflected += s->value[NET_RAD_REFL];

			/* update temporary absorbed and transmitted par net radiation lower layer */
			temp_ppfd_abs += s->value[PPFD_ABS];
			c->ppfd_abs += s->value[PPFD_ABS];
			temp_ppfd_reflected += s->value[PPFD_REFL];
			c->ppfd_reflected += s->value[PPFD_REFL];

			/* compute PAR for lower layers at the last height class processed */
			if(counter == c->subdominated_veg_counter)
			{
				/* compute par for lower layer */
				c->par -= (temp_apar + temp_par_reflected);

				/* compute net radiation for lower layer */
				c->net_radiation -= (temp_net_radiation_abs + temp_net_radiation_reflected);

				/* compute ppfd for lower layer */
				c->ppfd -= (temp_ppfd_abs + temp_ppfd_reflected);
			}

			/* assign to soil layer if no other height classes in dominated layer */
			if(c->height_class_in_layer_subdominated_counter == 0)
			{
				/* remove reflected part */
				c->par_reflected_soil = c->par * LightReflec_soil;
				c->net_radiation_for_soil_reflected = c->net_radiation * LightReflec_soil;
				c->ppfd_reflected_soil = c->ppfd * LightReflec_soil;

				/* Par and net radiation for the soil during growing season */
				c->par_for_soil = c->par - c->par_reflected_soil;
				c->net_radiation_for_soil = c->net_radiation - c->net_radiation_for_soil_reflected;
				c->ppfd_for_soil = c->ppfd - c->ppfd_reflected_soil;
				logger(g_log, "PAR for soil during growing season = %g molPAR/m^2/day\n", c->par_for_soil);
				logger(g_log, "Net Radiation for soil during growing season = %g W/m2\n", c->net_radiation_for_soil);
				logger(g_log, "PPFD for soil during growing season = %g W/m2\n", c->ppfd_for_soil);
			}
		}
	}
	/**********************************************************************************************************************************************************************************/
	/* outside growing season */
	else
	{
		logger(g_log, "\n**LIGHT FOR SOIL LAYER (outside the growing season)**\n");

		/* Remove the reflected PAR */
		c->par_reflected_soil = met[month].d[day].par * LightReflec_soil;

		/* Remove the reflected radiation */
		c->short_wave_radiation_upward_W = met[month].d[day].sw_downward_W * LightReflec_soil;
		logger(g_log, "Short wave radiation (upward) = %g W/m2\n", c->short_wave_radiation_upward_W);
		/* Net Short Wave radiation */
		c->net_short_wave_radiation_W = met[month].d[day].sw_downward_W - c->short_wave_radiation_upward_W;
		logger(g_log, "Net Short wave radiation = %g W/m2\n", c->net_short_wave_radiation_W);

		c->net_radiation_for_soil_reflected = c->short_wave_radiation_upward_W;

		/* Remove the reflected PPFD */
		c->ppfd_reflected_soil = met[month].d[day].ppfd * LightReflec_soil;

		/* compute radiation absorbed by the soil */
		/* Par and net radiation for the soil outside growing season (bare soil condition) */
		c->par_for_soil = met[month].d[day].par - c->par_reflected_soil;
		c->net_radiation_for_soil = met[month].d[day].sw_downward_W - c->short_wave_radiation_upward_W;
		c->ppfd_for_soil = met[month].d[day].ppfd - c->ppfd_reflected_soil;
		logger(g_log, "PAR for soil outside growing season = %g \n", c->par_for_soil);
		logger(g_log, "Net Radiation for soil outside growing season = %g \n", c->net_radiation_for_soil);
		logger(g_log, "PPFD for soil outside growing season = %g \n", c->ppfd_for_soil);
	}

	//if(month == 7)getchar();
}
