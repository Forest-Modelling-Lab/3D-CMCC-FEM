/* includes */
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
//extern soil_settings_t *g_soil_settings;
//extern topo_t *g_topo;

void Rad_abs_transm (cell_t *const c, species_t *const s, double LightAbsorb_sun, double LightAbsorb_shade,
		double LightReflec_par, double LightReflec_net_rad)
{
	/* note: This function computes absorbed and transmitted PAR, NET RADIATION and PPFD through different height classes/layers
	 * considering at square meter WITHOUT takes into account coverage*/

	/*compute APAR (molPAR/m^2 day) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE par = %g molPAR/m^2 day\n", c->par);
	s->value[PAR] = c->par;
	s->value[APAR_SUN] = c->par * LightAbsorb_sun;
	s->value[TRANSM_PAR_SUN] = c->par - s->value[APAR_SUN];
	s->value[APAR_SHADE] = s->value[TRANSM_PAR_SUN] * LightAbsorb_shade;
	s->value[TRANSM_PAR_SHADE] = s->value[TRANSM_PAR_SUN] - s->value[APAR_SHADE];
	/* overall canopy */
	s->value[APAR] = s->value[APAR_SUN] + s->value[APAR_SHADE];
	s->value[TRANSM_PAR] = s->value[TRANSM_PAR_SHADE];
	s->value[REFL_PAR] = c->par * LightReflec_par;
	CHECK_CONDITION(fabs((s->value[APAR] + s->value[TRANSM_PAR])-c->par),>1e-4);
	/* cumulate over the layer filtered par */
	c->par_transm += s->value[TRANSM_PAR];

	logger(g_log, "Apar sun = %g molPAR/m^2 day\n", s->value[APAR_SUN]);
	logger(g_log, "Transmitted Par sun = %g molPAR/m^2 day\n", s->value[TRANSM_PAR_SUN]);
	logger(g_log, "Apar shade = %g molPAR/m^2 day\n", s->value[APAR_SHADE]);
	logger(g_log, "Transmitted Par shade = %g molPAR/m^2 day\n", s->value[TRANSM_PAR_SHADE]);
	logger(g_log, "Apar total = %g molPAR/m^2 day\n", s->value[APAR]);
	logger(g_log, "Transmitted Par total = %g molPAR/m^2 day\n", s->value[TRANSM_PAR]);
	logger(g_log, "Reflected Par = %g molPAR/m^2 day\n", s->value[REFL_PAR]);
	logger(g_log, "Below the canopy par (filtered)= %g molPAR/m^2 day\n", c->par_transm);

	/*compute NetRad (W/m^2) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE net_sw_rad = %g W/m^2\n", c->net_sw_rad);
	s->value[NET_SW_RAD] = c->net_sw_rad;
	s->value[NET_SW_RAD_ABS_SUN] = c->net_sw_rad * LightAbsorb_sun;
	s->value[NET_SW_RAD_TRANSM_SUN] = c->net_sw_rad - s->value[NET_SW_RAD_ABS_SUN];
	s->value[NET_SW_RAD_ABS_SHADE] = s->value[NET_SW_RAD_TRANSM_SUN] * LightAbsorb_shade;
	s->value[NET_SW_RAD_TRANSM_SHADE] = s->value[NET_SW_RAD_TRANSM_SUN] - s->value[NET_SW_RAD_ABS_SHADE];
	/* overall canopy */
	s->value[NET_SW_RAD_ABS] = s->value[NET_SW_RAD_ABS_SUN] + s->value[NET_SW_RAD_ABS_SHADE];
	s->value[NET_SW_RAD_TRANSM] = s->value[NET_SW_RAD_TRANSM_SHADE];
	s->value[SW_RAD_REFL] = c->net_sw_rad * LightReflec_net_rad;
	CHECK_CONDITION(fabs((s->value[NET_SW_RAD_ABS] + s->value[NET_SW_RAD_TRANSM])-c->net_sw_rad),>1e-4);
	/* cumulate over the layer net radiation */
	c->net_sw_rad_transm += s->value[NET_SW_RAD_TRANSM];

	logger(g_log, "Absorbed NetRad sun = %g W/m^2\n", s->value[NET_SW_RAD_ABS_SUN]);
	logger(g_log, "Transmitted NetRad sun = %g W/m^2\n", s->value[NET_SW_RAD_TRANSM_SUN]);
	logger(g_log, "Absorbed NetRad shade = %g W/m^2\n", s->value[NET_SW_RAD_ABS_SHADE]);
	logger(g_log, "Transmitted NetRad shade = %g W/m^2\n", s->value[NET_SW_RAD_TRANSM_SHADE]);
	logger(g_log, "Absorbed total = %g W/m^2\n", s->value[NET_SW_RAD_ABS]);
	logger(g_log, "Transmitted total = %g W/m^2\n", s->value[NET_SW_RAD_TRANSM]);
	logger(g_log, "Below the canopy net radiation (filtered)= %g molPAR/m^2 day\n", c->net_sw_rad_transm);

	/* compute PPFD (umol/m^2/sec) for sun and shaded leaves*/
	logger(g_log, "\nAVAILABLE ppfd = %g umol/m2/sec\n", c->ppfd);
	s->value[PPFD] = c->ppfd;
	s->value[PPFD_ABS_SUN] = c->ppfd * LightAbsorb_sun;
	s->value[PPFD_TRANSM_SUN] = c->ppfd - s->value[PPFD_ABS_SUN];
	s->value[PPFD_ABS_SHADE] = s->value[PPFD_TRANSM_SUN] * LightAbsorb_shade;
	s->value[PPFD_TRANSM_SHADE] = s->value[PPFD_TRANSM_SUN] - s->value[PPFD_ABS_SHADE];
	/* overall canopy */
	s->value[PPFD_ABS] = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
	s->value[PPFD_TRANSM] = s->value[PPFD_TRANSM_SHADE];
	s->value[PPFD_REFL] = c->ppfd * LightReflec_par;
	CHECK_CONDITION(fabs((s->value[PPFD_ABS] + s->value[PPFD_TRANSM])-c->ppfd),>1e-4);
	/* cumulate over the layer ppfd */
	c->ppfd_transm += s->value[PPFD_TRANSM];

	logger(g_log, "Absorbed ppfd sun = %g umol/m2/sec\n", s->value[PPFD_ABS_SUN]);
	logger(g_log, "Transmitted ppfd sun = %g umol/m2/sec\n", s->value[PPFD_TRANSM_SUN]);
	logger(g_log, "Absorbed ppfd shade = %g umol/m2/sec\n", s->value[PPFD_ABS_SHADE]);
	logger(g_log, "Transmitted ppfd shade = %g umol/m2/sec\n", s->value[PPFD_TRANSM_SHADE]);
	logger(g_log, "Absorbed ppfd total  = %g umol/m2/sec\n", s->value[PPFD_ABS]);
	logger(g_log, "Transmitted ppfd total  = %g umol/m2/sec\n", s->value[PPFD_TRANSM]);
	logger(g_log, "Below the canopy ppfd (filtered)= %g molPAR/m^2 day\n", c->ppfd_transm);


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
}

void canopy_radiation(species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species)
{

	double LightAbsorb, LightAbsorb_sun, LightAbsorb_shade;                               //fraction of light absorbed
	double LightTransm, LightTransm_sun, LightTransm_shade;                               //fraction of light transmitted
	double LightReflec_net_rad;                                                           //fraction of light reflected (for net radiation)
	double LightReflec_par;                                                               //fraction of light reflected (for par)
	double LightReflec_soil;

	int counter;

	double leaf_cover_eff;                                                               //fraction of square meter covered by leaf over the gridcell
	double gap_canopy_cover_eff;                                                         //fraction of square meter un-covered by leaf over the gridcell
	double cumulated_leaf_cover_eff;                                                      //fraction of square meter covered by leaf over the gridcell
	double cumulated_gap_cover_eff;                                                       //fraction of square meter un-covered by leaf over the gridcell

	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	double actual_albedo;

	//fixme move soil albedo into soil.txt file
	//test check albedo for other typos
	double soil_albedo = 0.17; //(see Wiki)

	logger(g_log, "\n**RADIATION ROUTINE**\n");
	logger(g_log, "-INCOMING RADIATION\n");
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


	/* RADIATION */
	/**************************************************************************/
	/* NET SHORT WAVE RADIATION */

	//fixme the light reflected should consider all reflected light through the cell (soil included)
	c->short_wave_radiation_upward_W = met[month].d[day].sw_downward_W  * LightReflec_net_rad;
	logger(g_log, "Short wave radiation (upward) = %g W/m2\n", c->short_wave_radiation_upward_W);

	/* net short wave radiation */
	c->net_short_wave_radiation_W = met[month].d[day].sw_downward_W - c->short_wave_radiation_upward_W;
	logger(g_log, "Net Short wave radiation = %g W/m2\n", c->net_short_wave_radiation_W);


	/*****************************************************************************************/

	/* NET RADIATION */
	logger(g_log, "\nNET RADIATION\n");

	//fixme DOES IT MUST TAKES INTO ACCOUNT INCOMING LONG WAVE RADIATION??
	c->net_sw_rad = c->net_short_wave_radiation_W - met[month].d[day].lw_net_W;
	logger(g_log, "Net radiation = %g W/m2\n", c->net_sw_rad);

	/*****************************************************************************************/

	/* PAR RADIATION */
	logger(g_log, "\nPAR RADIATION\n");

	/* convert MJ/m2/day to molPAR/m2/day (Biome-BGC method)*/
	c->par = (met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR);
	logger(g_log, "Par = %g molPAR/m^2 day\n", c->par);

	/* Remove the reflected PAR */
	c->par_reflected = c->par * LightReflec_par;
	c->par -= c->par_reflected;
	logger(g_log, "Par less reflected part = %g molPAR/m^2 day\n", c->par);
	/*****************************************************************************************/

	/* PPFD RADIATION */
	logger(g_log, "\nPPFD RADIATION\n");

	/* compute PPFD (umol/m2/sec) */
	c->ppfd = met[month].d[day].sw_downward_W * RAD2PAR * EPAR;
	logger(g_log, "PPFD = %g umolPPFD/m2/sec\n", c->ppfd);

	/* Remove the reflected PPFD */
	c->ppfd *= (1.0 - LightReflec_par);
	logger(g_log, "PPFD less reflected = %g umolPPFD/m2/sec\n", c->ppfd);
	/*****************************************************************************************/


	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{
		/* compute light absorbed from dominant and transmitted to lower layers/soil layer */
		if ( c->heights[height].z == c->top_layer )
		{
			logger(g_log, "**LIGHT DOMINANT**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			/* reset counter and cumulated canopy cover at first height class processes */
			if (c->dominant_veg_counter == 1) counter = 0;
			/* increment layer counter */
			counter ++;

			/* compute absorbed and transmitted Par, Net radiation and ppfd */
			Rad_abs_transm (c, &c->heights[height].ages[age].species[species], LightAbsorb_sun, LightAbsorb_shade, LightReflec_par, LightReflec_net_rad);

			/* first height class processed */
			if(counter == 1)
			{
				/* reset values */
				cumulated_leaf_cover_eff = 0.0;
				cumulated_gap_cover_eff = 0.0;
			}

			logger(g_log, "\n**SINGLE HEIGHT CLASS COMPUTATION**\n");

			/* compute effective canopy cover */
			if(s->value[LAI] < 1.0)
			{
				/* special case when LAI = < 1.0 */
				leaf_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
			}
			else
			{
				leaf_cover_eff = s->value[CANOPY_COVER_DBHDC];
			}
			gap_canopy_cover_eff = 1.0 - leaf_cover_eff;

			/* check for the special case in which is allowed to have more 100% of grid cell covered */
			if(leaf_cover_eff > 1.0)
			{
				leaf_cover_eff = 1.0;
				gap_canopy_cover_eff = 0.0;
			}
			logger(g_log, "single height class light absorption = %g %%\n", LightAbsorb*100.0);
			logger(g_log, "single height class canopy cover = %g %%\n", leaf_cover_eff*100.0);
			logger(g_log, "single height class gap cover = %g %%\n", gap_canopy_cover_eff*100.0);

			logger(g_log, "\n**LAYER LEVEL COMPUTATION**\n");

			/* compute cumulated canopy cover and gap for layer */
			cumulated_leaf_cover_eff += leaf_cover_eff;
			cumulated_gap_cover_eff = 1.0 - cumulated_leaf_cover_eff;
			logger(g_log, "layer level canopy cover = %g %%\n", cumulated_leaf_cover_eff * 100.0);
			logger(g_log, "layer level gap cover = %g %%\n", cumulated_gap_cover_eff * 100.0);
			CHECK_CONDITION(cumulated_leaf_cover_eff + cumulated_gap_cover_eff, == 0);

			/* last height class processed assign value for lower/soil layers */
			if(counter == c->dominant_veg_counter)
			{
				logger(g_log, "\n**LIGHT FOR LOWER/SOIL LAYER**\n");

				/* note : lower layers use c->par, c->net_radiation, c->ppfd */
				/* compute weighted average radiation taking into account "pure, un-filtered un-reflected light" and "filtered and reflected light" over grid cell */
				/* if there's absorption from trees */
				if(cumulated_leaf_cover_eff > 0.0)
				{
					logger(g_log,"\n-WITHOUT ABSORPTION BY THE TREES..\n");
					logger(g_log,"incoming par = %g MJ/m^2/day\n", (met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR));
					logger(g_log,"incoming net radiation = %g W/m^2\n", (met[month].d[day].sw_downward_W - met[month].d[day].lw_net_W ));
					logger(g_log,"incoming ppfd = %g umol/m^2/sec\n", (met[month].d[day].sw_downward_W * RAD2PAR * EPAR));
					logger(g_log,"\n-WITH ABSORPTION BY THE TREES..\n");
					logger(g_log,"par for lower/soil layer = %g MJ/m^2/day\n", c->par_transm);
					logger(g_log,"net radiation for lower/soil layer = %g W/m^2\n", c->net_sw_rad_transm);
					logger(g_log,"ppfd for lower/soil layer = %g umol/m^2/sec\n", c->ppfd_transm);

					/* first term is un-filtered light, the second is filtered light */
					c->par = ((met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR)*cumulated_gap_cover_eff) + (c->par_transm * cumulated_leaf_cover_eff) ;
					c->net_sw_rad = ((met[month].d[day].sw_downward_W - met[month].d[day].lw_net_W )*cumulated_gap_cover_eff) + (c->net_sw_rad_transm * cumulated_leaf_cover_eff);
					c->ppfd = ((met[month].d[day].sw_downward_W * RAD2PAR * EPAR)*cumulated_gap_cover_eff) + (c->ppfd_transm * cumulated_leaf_cover_eff);

				}
				logger(g_log,"\n-AVERAGING BETWEEN COVERAGE AND GAPS..\n");
				logger(g_log,"par for lower/soil layer = %g MJ/m^2/day\n", c->par);
				logger(g_log,"net radiation for lower/soil layer = %g W/m^2\n", c->net_sw_rad);
				logger(g_log,"ppfd for lower/soil layer = %g umol/m^2/sec\n", c->ppfd);
			}

			/* assign to soil layer if no other height classes in dominated layer */
			if(c->height_class_in_layer_dominated_counter == 0)
			{
				/* remove reflected part */
				c->par_reflected_soil = c->par * LightReflec_soil;
				c->sw_rad_for_soil_refl = c->net_sw_rad * LightReflec_soil;

				/* Par and net radiation for the soil during growing season */
				c->par_for_soil = c->par - c->par_reflected_soil;
				logger(g_log, "Par for soil during growing season = %g \n", c->net_sw_rad_for_soil);
				c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
				logger(g_log, "Net Radiation for soil during growing season = %g \n", c->net_sw_rad_for_soil);
			}
		}
		/*************************************************************************************************************************************************************************************/
		/* dominated layers */
		else if (c->heights[height].z == c->top_layer - 1)
		{
			//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
			//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell & Norman (2000, p. 259)
			logger(g_log, "**LIGHT DOMINATED**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominated_counter);

			/* reset counter and cumulated canopy cover at first height class processes */
			if (c->dominated_veg_counter == 1) counter = 0;
			/* increment layer counter */
			counter ++;

			/* compute absorbed and transmitted Par, Net radiation and ppfd */
			Rad_abs_transm (c, &c->heights[height].ages[age].species[species], LightAbsorb_sun, LightAbsorb_shade, LightReflec_par, LightReflec_net_rad);

			/* first height class processed */
			if(counter == 1)
			{
				/* reset values */
				cumulated_leaf_cover_eff = 0.0;
				cumulated_gap_cover_eff = 0.0;
			}

			logger(g_log, "\n**SINGLE HEIGHT CLASS COMPUTATION**\n");

			/* compute effective canopy cover */
			if(s->value[LAI] < 1.0)
			{
				/* special case when LAI = < 1.0 */
				leaf_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
			}
			else
			{
				leaf_cover_eff = s->value[CANOPY_COVER_DBHDC];
			}
			gap_canopy_cover_eff = 1.0 - leaf_cover_eff;

			/* check for the special case in which is allowed to have more 100% of grid cell covered */
			if(leaf_cover_eff > 1.0)
			{
				leaf_cover_eff = 1.0;
				gap_canopy_cover_eff = 0.0;
			}
			logger(g_log, "single height class light absorption = %g %%\n", LightAbsorb*100.0);
			logger(g_log, "single height class canopy cover = %g %%\n", leaf_cover_eff*100.0);
			logger(g_log, "single height class gap cover = %g %%\n", gap_canopy_cover_eff*100.0);

			logger(g_log, "\n**LAYER LEVEL COMPUTATION**\n");

			/* compute cumulated canopy cover and gap for layer */
			cumulated_leaf_cover_eff += leaf_cover_eff;
			cumulated_gap_cover_eff = 1.0 - cumulated_leaf_cover_eff;
			logger(g_log, "layer level canopy cover = %g %%\n", cumulated_leaf_cover_eff * 100.0);
			logger(g_log, "layer level gap cover = %g %%\n", cumulated_gap_cover_eff * 100.0);
			CHECK_CONDITION(cumulated_leaf_cover_eff + cumulated_gap_cover_eff, == 0);

			/* last height class processed assign value for lower/soil layers */
			if(counter == c->dominated_veg_counter)
			{
				logger(g_log, "\n**LIGHT FOR LOWER/SOIL LAYER**\n");

				/* note : lower layers use c->par, c->net_radiation, c->ppfd */
				/* compute weighted average radiation taking into account "pure, un-filtered un-reflected light" and "filtered and reflected light" over gridcell */
				/* if there's absorption from trees */
				if(cumulated_leaf_cover_eff > 0.0)
				{
					logger(g_log,"\n-WITHOUT ABSORPTION BY THE TREES..\n");
					logger(g_log,"incoming par = %g MJ/m^2/day\n", (met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR));
					logger(g_log,"incoming net radiation = %g W/m^2\n", (met[month].d[day].sw_downward_W - met[month].d[day].lw_net_W));
					logger(g_log,"incoming ppfd = %g umol/m^2/sec\n", (met[month].d[day].sw_downward_W * RAD2PAR * EPAR));
					logger(g_log,"\n-WITH ABSORPTION BY THE TREES..\n");
					logger(g_log,"par for lower/soil layer = %g MJ/m^2/day\n", c->par_transm);
					logger(g_log,"net radiation for lower/soil layer = %g W/m^2\n", c->net_sw_rad_transm);
					logger(g_log,"ppfd for lower/soil layer = %g umol/m^2/sec\n", c->ppfd_transm);

					/* first term is un-filtered light, the second is filtered light */
					c->par = ((met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR)*cumulated_gap_cover_eff) + (c->par_transm * cumulated_leaf_cover_eff) ;
					c->net_sw_rad = ((met[month].d[day].sw_downward_W - met[month].d[day].lw_net_W)*cumulated_gap_cover_eff) + (c->net_sw_rad_transm * cumulated_leaf_cover_eff);
					c->ppfd = ((met[month].d[day].sw_downward_W * RAD2PAR * EPAR)*cumulated_gap_cover_eff) + (c->ppfd_transm * cumulated_leaf_cover_eff);

				}
				logger(g_log,"\n-AVERAGING BETWEEN COVERAGE AND GAPS..\n");
				logger(g_log,"par for lower/soil layer = %g MJ/m^2/day\n", c->par);
				logger(g_log,"net radiation for lower/soil layer = %g W/m^2\n", c->net_sw_rad);
				logger(g_log,"ppfd for lower/soil layer = %g umol/m^2/sec\n", c->ppfd);
			}

			/* assign to soil layer if no other height classes in subdominated layer */
			if(c->height_class_in_layer_subdominated_counter == 0)
			{
				/* remove reflected part */
				c->par_reflected_soil = c->par *LightReflec_soil;
				c->sw_rad_for_soil_refl = c->net_sw_rad * LightReflec_soil;

				/* Par and net radiation for the soil during growing season */
				c->par_for_soil = c->par - c->par_reflected_soil;
				logger(g_log, "Par for soil during growing season = %g \n", c->net_sw_rad_for_soil);
				c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
				logger(g_log, "Net Radiation for soil during growing season = %g \n", c->net_sw_rad_for_soil);
			}
		}
		/*******************************************************************************************************************************************************************************/
		/* subdominated layer */
		else
		{
			//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
			//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell&Norman (2000, p. 259)

			logger(g_log, "**LIGHT SUB-DOMINATED**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_subdominated_counter);

			/* reset counter and cumulated canopy cover at first height class processes */
			if (c->subdominated_veg_counter == 1) counter = 0;
			/* increment layer counter */
			counter ++;

			/* compute absorbed and transmitted Par, Net radiation and ppfd */
			Rad_abs_transm (c, &c->heights[height].ages[age].species[species], LightAbsorb_sun, LightAbsorb_shade, LightReflec_par, LightReflec_net_rad);

			/* first height class processed */
			if(counter == 1)
			{
				/* reset values */
				cumulated_leaf_cover_eff = 0.0;
				cumulated_gap_cover_eff = 0.0;
			}

			logger(g_log, "\n**SINGLE HEIGHT CLASS COMPUTATION**\n");

			/* compute effective canopy cover */
			if(s->value[LAI] < 1.0)
			{
				/* special case when LAI = < 1.0 */
				leaf_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
			}
			else
			{
				leaf_cover_eff = s->value[CANOPY_COVER_DBHDC];
			}
			gap_canopy_cover_eff = 1.0 - leaf_cover_eff;

			/* check for the special case in which is allowed to have more 100% of grid cell covered */
			if(leaf_cover_eff > 1.0)
			{
				leaf_cover_eff = 1.0;
				gap_canopy_cover_eff = 0.0;
			}
			logger(g_log, "single height class light absorption = %g %%\n", LightAbsorb*100.0);
			logger(g_log, "single height class canopy cover = %g %%\n", leaf_cover_eff*100.0);
			logger(g_log, "single height class gap cover = %g %%\n", gap_canopy_cover_eff*100.0);

			logger(g_log, "\n**LAYER LEVEL COMPUTATION**\n");

			/* compute cumulated canopy cover and gap for layer */
			cumulated_leaf_cover_eff += leaf_cover_eff;
			cumulated_gap_cover_eff = 1.0 - cumulated_leaf_cover_eff;
			logger(g_log, "layer level canopy cover = %g %%\n", cumulated_leaf_cover_eff * 100.0);
			logger(g_log, "layer level gap cover = %g %%\n", cumulated_gap_cover_eff * 100.0);
			CHECK_CONDITION(cumulated_leaf_cover_eff + cumulated_gap_cover_eff, == 0);

			/* last height class processed assign value for lower/soil layers */
			if(counter == c->subdominated_veg_counter)
			{
				logger(g_log, "\n**LIGHT FOR LOWER/SOIL LAYER**\n");

				/* note : lower layers use c->par, c->net_sw_rad, c->ppfd */
				/* compute weighted average radiation taking into account "pure, un-filtered un-reflected light" and "filtered and reflected light" over gridcell */
				/* if there's absorption from trees */
				if(cumulated_leaf_cover_eff > 0.0)
				{
					logger(g_log,"\n-WITHOUT ABSORPTION BY THE TREES..\n");
					logger(g_log,"incoming par = %g MJ/m^2/day\n", (met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR));
					logger(g_log,"incoming net radiation = %g W/m^2\n", (met[month].d[day].sw_downward_W - met[month].d[day].lw_net_W));
					logger(g_log,"incoming ppfd = %g umol/m^2/sec\n", (met[month].d[day].sw_downward_W * RAD2PAR * EPAR));
					logger(g_log,"\n-WITH ABSORPTION BY THE TREES..\n");
					logger(g_log,"par for lower/soil layer = %g MJ/m^2/day\n", c->par_transm);
					logger(g_log,"net radiation for lower/soil layer = %g W/m^2\n", c->net_sw_rad_transm);
					logger(g_log,"ppfd for lower/soil layer = %g umol/m^2/sec\n", c->ppfd_transm);

					/* first term is un-filtered light, the second is filtered light */
					c->par = ((met[month].d[day].sw_downward_MJ * RAD2PAR * EPAR)*cumulated_gap_cover_eff) + (c->par_transm * cumulated_leaf_cover_eff) ;
					c->net_sw_rad = ((met[month].d[day].sw_downward_W - met[month].d[day].lw_net_W)*cumulated_gap_cover_eff) + (c->net_sw_rad_transm * cumulated_leaf_cover_eff);
					c->ppfd = ((met[month].d[day].sw_downward_W * RAD2PAR * EPAR)*cumulated_gap_cover_eff) + (c->ppfd_transm * cumulated_leaf_cover_eff);

				}
				logger(g_log,"\n-AVERAGING BETWEEN COVERAGE AND GAPS..\n");
				logger(g_log,"par for lower/soil layer = %g MJ/m^2/day\n", c->par);
				logger(g_log,"net radiation for lower/soil layer = %g W/m^2\n", c->net_sw_rad);
				logger(g_log,"ppfd for lower/soil layer = %g umol/m^2/sec\n", c->ppfd);
			}

			/* assign to soil layer */
			if(c->height_class_in_layer_subdominated_counter == 0)
			{
				/* remove reflected part */
				c->par_reflected_soil = c->par * LightReflec_soil;
				c->sw_rad_for_soil_refl = c->net_sw_rad * LightReflec_soil;

				/* Par and net radiation for the soil during growing season */
				c->par_for_soil = c->par - c->par_reflected_soil;
				logger(g_log, "Par for soil during growing season = %g \n", c->net_sw_rad_for_soil);
				c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
				logger(g_log, "Net Radiation for soil during growing season = %g \n", c->net_sw_rad_for_soil);
			}
		}
	}
	/**********************************************************************************************************************************************************************************/
	/* outside growing season */
	else
	{
		logger(g_log, "\n**LIGHT FOR SOIL LAYER (outside the growing season)**\n");
		//fixme
		//c->par = (c->par * cumulated_leaf_cover_eff) + (c->par * cumulated_gap_cover_eff);
		/* remove reflected part */
		c->par_reflected_soil = c->par * LightReflec_soil;
		c->sw_rad_for_soil_refl = c->net_sw_rad * LightReflec_soil;

		/* Par and net radiation for the soil outside growing season (bare soil condition) */
		c->par_for_soil = c->par - c->par_reflected_soil;
		logger(g_log, "Par for soil outside growing season = %g \n", c->par_for_soil);
		c->net_sw_rad_for_soil = c->net_sw_rad - c->sw_rad_for_soil_refl;
		logger(g_log, "Net Radiation for soil outside growing season = %g \n", c->net_sw_rad_for_soil);
	}

	/* for radiative balance */
	/* cumulate radiation */
	c->apar += (s->value[APAR] * cumulated_leaf_cover_eff);
	c->par_reflected += (s->value[REFL_PAR] * cumulated_leaf_cover_eff);
	c->net_sw_rad_abs += s->value[NET_SW_RAD_ABS];
	c->sw_rad_refl += s->value[SW_RAD_REFL];

}

