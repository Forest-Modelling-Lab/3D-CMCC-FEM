/*
 * C-evergreen-partitioning-allocation.c
 *
 *  Created on: 20/nov/2013
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

//VERSION CURRENTLY USED
//Evergreen carbon allocation routine
void Evergreen_Partitioning_Allocation (SPECIES *const s, CELL *const c, const MET_DATA *const met, int day, int month, int years, int DaysInMonth, int height, int age, int species)
{
	//CTEM VERSION

	int i;

	//allocation parameter. their sum must be = 1
	double  s0Ctem = s->value[S0CTEM];
	double  r0Ctem = s->value[R0CTEM];
	//double  f0Ctem = s->value[F0CTEM];
	double const omegaCtem = s->value[OMEGA_CTEM];
	double pS_CTEM;
	double pR_CTEM;
	double pF_CTEM;
	//double max_DM_foliage;
	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	double Daily_solar_radiation;
	//double Monthly_solar_radiation;
	double Light_trasm;
	double Par_over;

	double Perc_fine;
	double Perc_coarse;
	//double Perc_leaves;              //percentage of leaves in first growing season
	double oldW;
	//double gammaF;
	//double oldWf;


	double r0Ctem_increment;
	double old_r0Ctem = r0Ctem;
	double s0Ctem_increment;
	double old_s0Ctem = s0Ctem;

	//Marconi here the allocation of biomass reserve is divided in fineroot and leaves following the allocation ratio parameter between them. That because
	//in ev ergreen we don't have bud burst phenology phase, and indeed there are two phenology phases; the former in which carbon is allocated in fineroot and foliage, the latter in
	// every pool except foliage
	static double frac_to_foliage_fineroot;


	Log("GET_ALLOCATION_ROUTINE\n\n");

	Log("Carbon allocation routine for evergreen\n");



	i = c->heights[height].z;

	//	if(day == 0 && month == 0)
	//	{
	//		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
	//		{
	//			s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
	//			Log("Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);
	//		}
	//	}


	Daily_solar_radiation = met[month].d[day].solar_rad * MOLPAR_MJ;
	Par_over = c->par - s->value[APAR];
	Light_trasm = Par_over /Daily_solar_radiation;


	s->value[FRACBB] = s->value[FRACBB1] + (s->value[FRACBB0] - s->value[FRACBB1]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TBB]));

	//7 May 2012
	//compute static ratio of allocation between fine
	//fixme see if chage with new parameters checked in "Pool_fraction"
	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
	Log("Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
	Log("Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
	Perc_coarse = 1- Perc_fine;
	//Log("Percentage of coarse root against total root= %f %%\n", Perc_coarse * 100 );

	if (settings->spatial == 'u')
	{
		oldW = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_BRANCH_tDM];

		//(Arora V. K., Boer G. J., GCB, 2005)

		if (s->management == 0)
		{
			//Log("Management type = TIMBER\n");
		}
		else
		{
			//Log("Management type = COPPICE\n");

			//recompute allocation parameter for coppice

			//TO DO: PUT INTO INPUT.TXT
			//OR CONSIDER YEARS_FROM_COPPICING AS THE AGE SETTED INTO INPUT.TXT
			//double years_from_coppicing = 25;


			Log("min r0 ctem = %f\n",s->value[MIN_R0CTEM] );
			Log("max s0 ctem = %f\n",s->value[MAX_S0CTEM] );
			Log("years for conversion = %f\n",s->value[YEARS_FOR_CONVERSION] );


			//considering a LINEAR increment
			//allocation ratio to roots
			r0Ctem -= s->value[MIN_R0CTEM];
			r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
			r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
			Log ("new r0_CTEM = %f \n", r0Ctem);

			if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
			{
				Log("ERROR IN r0Ctem !!! \n");
			}


			//considering a LINEAR decrement
			//allocation ratio to stem + bb
			s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
			s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
			s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
			Log ("new s0_CTEM = %f \n", s0Ctem);

			if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)
			{
				Log("ERROR IN s0Ctem !!! \n");
			}

		}
		Log("PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
		Log("LAI = %f \n", s->value[LAI]);
		Log("PEAK LAI = %f \n", s->value[PEAK_LAI]);


		switch (s->phenology_phase)
		{
		/************************************************************/
		case 1:
			Log("Allocating only into foliage and fine root pools\n");
			Log("LAI = %f \n", s->value[LAI]);

			/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/


			if (s->value[RESERVE_tDM] < 0.0)
			{
				Log("ATTENTION BIOMASS RESERVE < 0.0\n");
			}


			/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			 * and Barbaroux et al., 2002,
													the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
			 */
			/*the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
			 * sharing the daily remaining amount (taking into account respiration costs)of NSC */
			//fixme model gets 10%
			frac_to_foliage_fineroot = (s->value[RESERVE_tDM]) * 0.05;
			Log("fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_fineroot);

			/*partitioning*/
			if (s->value[NPP_tDM] > 0.0)
			{
				Log("Using ONLY npp...\n");

				s->value[DEL_FOLIAGE] = (s->value[NPP_tDM] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
				s->value[DEL_ROOTS_FINE] = (s->value[NPP_tDM] - s->value[DEL_FOLIAGE]);
				s->value[DEL_RESERVE] = 0;
				s->value[DEL_ROOTS_COARSE] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS] = 0;
				s->value[DEL_BB] = 0;
			}
			else
			{
				Log("Using ONLY reserve...\n");

				s->value[DEL_FOLIAGE] = (frac_to_foliage_fineroot * (1.0 -s->value[FINE_ROOT_LEAF_FRAC]));
				s->value[DEL_ROOTS_FINE] = (frac_to_foliage_fineroot - s->value[DEL_FOLIAGE]);
				s->value[DEL_RESERVE] = - (((fabs(s->value[C_FLUX]) * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell) + frac_to_foliage_fineroot);
				s->value[DEL_ROOTS_COARSE] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS]= 0;
				s->value[DEL_BB]= 0;
			}

			/*allocation*/
			s->value[BIOMASS_FOLIAGE_tDM] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass(Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE_tDM]);
			s->value[BIOMASS_TOT_STEM_tDM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM_tDM]);
			s->value[BIOMASS_STEM_tDM] += s->value[DEL_STEMS];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
			s->value[BIOMASS_BRANCH_tDM] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH_tDM]);
			Log("BEFORE Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE_tDM]);
			s->value[RESERVE_tDM] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE_tDM]);
			s->value[BIOMASS_ROOTS_TOT_tDM] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
			s->value[BIOMASS_FINE_ROOT_tDM] += s->value[DEL_ROOTS_FINE];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_FINE_ROOT_tDM]);
			s->value[BIOMASS_COARSE_ROOT_tDM] += s->value[DEL_ROOTS_COARSE];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_tDM]);

			//check for live and dead tissues
			s->value[BIOMASS_STEM_LIVE_WOOD_tDM] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] += (s->value[DEL_ROOTS_COARSE] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
			//s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
			//s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);

			Daily_lai (&c->heights[height].ages[age].species[species]);

			/*check if re-transfer foliage biomass to reserve*/
			if (s->value[LAI] > s->value[PEAK_LAI])
			{
				Log("LAI exceeds Peak Lai\n");

				/*partitioning*/
				/*re-transfer mass to reserve*/
				Log("retranslocating = %f\n", s->value[BIOMASS_FOLIAGE_tDM] - s->value[MAX_BIOMASS_FOLIAGE_tDM]);
				s->value[DEL_FOLIAGE] = (s->value[BIOMASS_FOLIAGE_tDM] - s->value[MAX_BIOMASS_FOLIAGE_tDM]);
				//s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_RESERVE] += s->value[BIOMASS_FOLIAGE_tDM] - s->value[MAX_BIOMASS_FOLIAGE_tDM];
				s->value[DEL_ROOTS_COARSE] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS] = 0;
				s->value[DEL_BB] = 0;

				/*allocation*/
				s->value[BIOMASS_FOLIAGE_tDM] = s->value[MAX_BIOMASS_FOLIAGE_tDM];
				Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
				s->value[BIOMASS_TOT_STEM_tDM] += s->value[DEL_TOT_STEM];
				Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM_tDM]);
				s->value[BIOMASS_STEM_tDM] += s->value[DEL_STEMS];
				Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
				s->value[BIOMASS_BRANCH_tDM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH_tDM]);
				s->value[RESERVE_tDM] +=  s->value[DEL_RESERVE];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE_tDM]);
				s->value[BIOMASS_ROOTS_TOT_tDM] +=  s->value[DEL_ROOTS_TOT];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
				s->value[BIOMASS_FINE_ROOT_tDM] += s->value[DEL_ROOTS_FINE];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_FINE_ROOT_tDM]);
				s->value[BIOMASS_COARSE_ROOT_tDM] += s->value[DEL_ROOTS_COARSE];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_tDM]);

				//check for live and dead tissues
				s->value[BIOMASS_STEM_LIVE_WOOD_tDM] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] += (s->value[DEL_ROOTS_COARSE] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
				//s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
				//s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);

				/*recompute correct LAI*/
				Daily_lai (&c->heights[height].ages[age].species[species]);
			}


			// Total Biomass
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_ROOTS_TOT_tDM] + s->value[RESERVE_tDM] + s->value[BIOMASS_BRANCH_tDM];
			Log("Previous Total W = %f tDM/area\n", oldW);
			Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);

			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM] );
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE] );
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_leaf_carbon += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_fine_root_carbon += s->value[DEL_ROOTS_FINE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			break;
		case 2:
			Log("allocating into the three pools Ws+Wr+Wreserve\n");
			/*see Barbaroux et al., 2002, Scartazza et al., 2013*/


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Roots CTEM ratio layer = %f %%\n", pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);

			/*partitioning*/
			if (s->value[NPP_tDM] > 0.0)
			{
				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 1.2)
				{
					//NPP for reproduction
					s->value[BIOMASS_FRUIT_tDM] = s->value[NPP_tDM] * s->value[FRUIT_PERC];
					s->value[NPP_tDM] -= s->value[BIOMASS_FRUIT_tDM];
					Log("Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT_tDM]);

					//reproductive life span
					s->value[BIOMASS_FRUIT_tDM] -= (s->value[BIOMASS_FRUIT_tDM] * (1 / s->value[CONES_LIFE_SPAN]));
				}

				s->value[DEL_RESERVE] = s->value[NPP_tDM] * pF_CTEM;
				s->value[DEL_ROOTS_TOT] = s->value[NPP_tDM] * pR_CTEM;
				s->value[DEL_ROOTS_FINE] = s->value[DEL_ROOTS_TOT] * Perc_fine;
				s->value[DEL_ROOTS_COARSE] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE];
				s->value[DEL_TOT_STEM] = s->value[NPP_tDM] * pS_CTEM;
				s->value[DEL_STEMS] = (s->value[NPP_tDM] * pS_CTEM) * (1.0 - s->value[FRACBB]);
				s->value[DEL_BB] = (s->value[NPP_tDM] * pS_CTEM) * s->value[FRACBB];
				s->value[DEL_FOLIAGE] = 0;
			}
			else
			{
				s->value[DEL_FOLIAGE] = 0;
				s->value[DEL_ROOTS_FINE] = 0;
				s->value[DEL_RESERVE] = ((s->value[C_FLUX] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
				s->value[DEL_ROOTS_COARSE] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS]= 0;
				s->value[DEL_BB]= 0;
			}
			/*allocation*/
			s->value[BIOMASS_FOLIAGE_tDM] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
			s->value[BIOMASS_TOT_STEM_tDM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM_tDM]);
			s->value[BIOMASS_STEM_tDM] += s->value[DEL_STEMS];
			Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
			s->value[BIOMASS_BRANCH_tDM] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH_tDM]);
			s->value[RESERVE_tDM] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE_tDM]);
			s->value[BIOMASS_ROOTS_TOT_tDM] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
			s->value[BIOMASS_FINE_ROOT_tDM] += s->value[DEL_ROOTS_FINE];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_FINE_ROOT_tDM]);
			s->value[BIOMASS_COARSE_ROOT_tDM] += s->value[DEL_ROOTS_COARSE];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_tDM]);

			//check for live and dead tissues
			s->value[BIOMASS_STEM_LIVE_WOOD_tDM] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] += (s->value[DEL_ROOTS_COARSE] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
			//s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
			//s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);

			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM] );
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE] );
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_leaf_carbon += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_fine_root_carbon += s->value[DEL_ROOTS_FINE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];


			break;

		}
	}

	CHECK_CONDITION(s->value[RESERVE_tDM], < 0);

	c->daily_lai[i] = s->value[LAI];
	c->annual_delta_wres[i] += s->value[DEL_RESERVE];

	Log("******************************\n");
}

/**/

