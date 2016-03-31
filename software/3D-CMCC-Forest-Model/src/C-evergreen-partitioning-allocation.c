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

	double Light_trasm;
//	double Perc_fine;
//	double Perc_coarse;
//	double oldW;

	double r0Ctem_increment;
	double old_r0Ctem = r0Ctem;
	double s0Ctem_increment;
	double old_s0Ctem = s0Ctem;

	//Marconi here the allocation of biomass reserve is divided in fineroot and leaves following the allocation ratio parameter between them. That because
	//in evergreen we don't have bud burst phenology phase, and indeed there are two phenology phases; the former in which carbon is allocated in fineroot and foliage, the latter in
	// every pool except foliage
	static double reserve_for_foliage_budburst;
	static double reserve_for_fine_root_budburst;
	static double reserve_for_budburst;

	/* both used in case of retranslocation carbon to reserve */
	double old_leaf_c;
	double old_fineroot_c;

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


	/* following Arora and Boer 2005 */
	Light_trasm = exp(- s->value[K] * s->value[LAI]);

	//7 May 2012
	//compute static ratio of allocation between fine
	//fixme see if chage with new parameters checked in "Pool_fraction"
//	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
//	Log("Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
//	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
//	Log("Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
//	Perc_coarse = 1- Perc_fine;
	//Log("Percentage of coarse root against total root= %f %%\n", Perc_coarse * 100 );

	/* Partitioning ratios from Arora and Boer 2005 */
	pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
	Log("Roots CTEM ratio layer = %f %%\n", pR_CTEM * 100);
	pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
	Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
	pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
	Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);
	CHECK_CONDITION(pR_CTEM + pS_CTEM + pF_CTEM, != 100.0);

	if (settings->spatial == 'u')
	{
		old_leaf_c = s->value[LEAF_C];
		old_fineroot_c = s->value[FINE_ROOT_C];

		//(Arora V. K., Boer G. J., GCB, 2005)

		if (s->management == 0)
		{
			//Log("Management type = TIMBER\n");
		}
		else
		{
			Log("min r0 ctem = %f\n",s->value[MIN_R0CTEM] );
			Log("max s0 ctem = %f\n",s->value[MAX_S0CTEM] );
			Log("years for conversion = %f\n",s->value[YEARS_FOR_CONVERSION]);

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

			CHECK_CONDITION(s->value[RESERVE_C], < 0.0);

			/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			 * and Barbaroux et al., 2002,
													the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
			 */
			/*the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
			 * sharing the daily remaining amount (taking into account respiration costs)of NSC */
			//fixme model gets 10%
			reserve_for_budburst = (s->value[RESERVE_C]) * 0.05;
			Log("fraction of reserve for foliage and fine root = %f\n", reserve_for_budburst);

			/*partitioning*/
			if (s->value[NPP_tC] > 0.0)
			{
				/* check if minimum reserve pool needs to be refilled */
				/* it doesn't need */
				if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
				{
					Log("Using ONLY npp...\n");
					s->value[C_TO_LEAF] = s->value[NPP_gC] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
					s->value[C_TO_FINEROOT] = s->value[NPP_tC] - s->value[C_TO_LEAF];
					s->value[C_TO_RESERVE] = 0.0;
				}
				/* it needs */
				else
				{
					s->value[C_TO_LEAF] = 0.0;
					s->value[C_TO_FINEROOT] = 0.0;
					s->value[C_TO_RESERVE] = s->value[NPP_tC];
				}

				s->value[C_TO_COARSEROOT] = 0.0;
				s->value[C_TO_STEM] = 0.0;
				s->value[C_TO_TOT_STEM] = 0.0;
				s->value[C_TO_BRANCH] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
				s->value[C_TO_LITTER] = 0.0;
			}
			else
			{
				Log("Using ONLY reserve...\n");
				s->value[C_TO_LEAF] = reserve_for_foliage_budburst;
				s->value[C_TO_FINEROOT] = reserve_for_fine_root_budburst;
				s->value[C_TO_RESERVE] = s->value[NPP_tC] - reserve_for_budburst;
				s->value[C_TO_RESERVE] = 0.0;
				s->value[C_TO_COARSEROOT] = 0.0;
				s->value[C_TO_STEM] = 0.0;
				s->value[C_TO_TOT_STEM] = 0.0;
				s->value[C_TO_BRANCH] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
				s->value[C_TO_LITTER] = 0.0;
			}
			break;
		case 2:
			Log("allocating into the three pools Ws+Wr+Wreserve\n");

			/*partitioning*/
			if (s->value[NPP_tC] > 0.0)
			{
				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if(s->value[PHENOLOGY] == 1.2)
				{
					//NPP for reproduction
					s->value[C_TO_FRUIT] = s->value[NPP_tC] * s->value[FRUIT_PERC];
					s->value[NPP_tC] -= s->value[C_TO_FRUIT];
					Log("Biomass increment into cones = %f tDM/area\n", s->value[C_TO_FRUIT]);
				}
				/* old version */
				//				s->value[C_TO_ROOT] = s->value[NPP_tC] * pR_CTEM;
				//				s->value[C_TO_FINEROOT] = s->value[C_TO_ROOT] * Perc_fine;
				//				s->value[C_TO_COARSEROOT] = s->value[C_TO_ROOT] - s->value[C_TO_FINEROOT];
				/* new one */
				s->value[C_TO_COARSEROOT] = s->value[NPP_tC] * pR_CTEM;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_RESERVE] = s->value[NPP_tC] * pF_CTEM;
				s->value[C_TO_TOT_STEM] = s->value[NPP_tC] * pS_CTEM;
				s->value[C_TO_STEM] = (s->value[NPP_tC] * pS_CTEM) * (1.0 - s->value[FRACBB]);
				s->value[C_TO_BRANCH] = (s->value[NPP_tDM] * pS_CTEM) * s->value[FRACBB];
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
			}
			else
			{
				s->value[C_TO_RESERVE] = s->value[NPP_tC];
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_COARSEROOT] = 0.0;
				s->value[C_TO_TOT_STEM] = 0.0;
				s->value[C_TO_STEM] = 0.0;
				s->value[C_TO_BRANCH] = 0.0;
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
				s->value[C_TO_LITTER] = 0.0;
			}

			break;

		}
	}

	/* recompute biomass pools for reserve, leaf and fine root pools */

	Log("\n*Carbon allocation*\n");
	/* update class level carbon biomass pools */

	s->value[RESERVE_C] +=  s->value[C_TO_RESERVE];
	Log("Reserve Biomass (Wres) = %f tC/area\n", s->value[RESERVE_C]);

	s->value[LEAF_C] += s->value[C_TO_LEAF];
	Log("Foliage Biomass (Wf) = %f tC/area\n", s->value[LEAF_C]);

	Daily_lai (&c->heights[height].ages[age].species[species]);

	/*check if re-transfer leaf or fine root biomass to reserve*/
	Log("retranslocating carbon to reserve\n");
	if (s->value[LAI] > s->value[PEAK_LAI])
	{
		/*partitioning*/
		/* re-transfer mass to reserve */
		Log("LAI exceeds Peak Lai\n");
		s->value[C_TO_LEAF] = s->value[MAX_LEAF_C] - old_leaf_c;
		s->value[LAI] = s->value[MAX_LAI];
		s->value[C_TO_RESERVE] = (s->value[LEAF_C] - s->value[MAX_LEAF_C]);
		/* update biomass pools */
		s->value[LEAF_C] = s->value[MAX_LEAF_C];
		s->value[RESERVE_C] +=  s->value[C_TO_RESERVE];

	}

	s->value[STEM_C] += s->value[C_TO_STEM];
	Log("Stem Biomass (Ws) = %f tC/area\n", s->value[STEM_C]);

	s->value[BRANCH_C] += s->value[C_TO_BRANCH];
	Log("Branch and Bark Biomass (Wbb) = %f tC/area\n", s->value[BRANCH_C]);

	s->value[FINE_ROOT_C] += s->value[C_TO_FINEROOT];
	Log("Fine Root Biomass (Wrf) = %f tC/area\n", s->value[FINE_ROOT_C]);

	//fixme check if necessary or turnover does all
	if(s->value[FINE_ROOT_C] > s->value[MAX_FINE_ROOT_C])
	{
		/*partitioning*/
		/* re-transfer mass to reserve */
		Log("Fine root exceeds Max fine root\n");
		s->value[C_TO_FINEROOT] = s->value[MAX_FINE_ROOT_C] - old_fineroot_c;
		s->value[DEL_RESERVE] = (s->value[FINE_ROOT_C] - s->value[MAX_FINE_ROOT_C]);
		/* update biomass pools */
		s->value[FINE_ROOT_C] = s->value[MAX_FINE_ROOT_C] ;
		s->value[RESERVE_C] +=  s->value[C_TO_RESERVE];
	}

	s->value[COARSE_ROOT_C] += s->value[C_TO_COARSEROOT];
	Log("Coarse Root Biomass (Wrc) = %f tC/area\n", s->value[COARSE_ROOT_C]);

	s->value[TOT_ROOT_C] =  s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	Log("Total Root Biomass (Wr TOT) = %f tC/area\n", s->value[TOT_ROOT_C]);

	s->value[TOT_STEM_C] += s->value[C_TO_TOT_STEM];
	Log("Total Stem Biomass (Wts)= %f tC/area\n", s->value[TOT_STEM_C]);

	s->value[FRUIT_C] += s->value[C_TO_FRUIT];
	Log("Fuit Biomass (Wfruit)= %f tC/area\n", s->value[FRUIT_C]);

	s->value[STEM_LIVE_WOOD_C] += s->value[C_TO_STEM] * s->value[LIVE_TOTAL_WOOD_FRAC];
	Log("Live Stem Biomass (Ws) = %f tC/area\n", s->value[STEM_LIVE_WOOD_C]);

	//s->value[STEM_DEAD_WOOD_C] += s->value[C_TO_STEM] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("Dead Stem Biomass (Ws) = %f tC/area\n", s->value[STEM_DEAD_WOOD_C]);

	s->value[COARSE_ROOT_LIVE_WOOD_C] += (s->value[C_TO_ROOT] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("Live Coarse Biomass (Ws) = %f tC/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);

	//s->value[COARSE_ROOT_DEAD_WOOD_C] += s->value[C_TO_COARSEROOT] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("Dead Coarse Biomass (Ws) = %f tC/area\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

	s->value[BRANCH_LIVE_WOOD_C] += (s->value[C_TO_BRANCH] * s->value[LIVE_TOTAL_WOOD_FRAC]);
	Log("Live Stem Branch Biomass (Ws) = %f tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);

	//s->value[BRANCH_DEAD_WOOD_C] += (s->value[C_TO_BRANCH] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
	Log("Dead Stem Branch Biomass (Ws) = %f tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);

	s->value[TOTAL_C] = s->value[LEAF_C] +s->value[STEM_C] + s->value[BRANCH_C] + s->value[TOT_ROOT_C] + /*s->value[FRUIT_C] +*/ s->value[RESERVE_C];
	Log("Total Carbon Biomass (W) = %f tC/area\n", s->value[TOTAL_C]);

	/* check for closure */
	if (fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C])>1e-4)
	{
		Log("Live stem + dead stem = %f\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
		Log("Total stem = %f\n", s->value[STEM_C]);
		//ALESSIOR
		Log("ATTENTION CHECK BALANCE!\n");
	}
	else
	{
		Log("Live stem + dead stem = %f\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
		Log("Total stem = %f\n", s->value[STEM_C]);
	}
	if (fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C])>1e-4)
	{
		Log("Live coarse + dead coarse = %f\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
		Log("Total coarse = %f\n", s->value[COARSE_ROOT_C]);
		//ALESSIOR
		Log("ATTENTION CHECK BALANCE!\n");
	}
	else
	{
		Log("Live coarse + dead coarse = %f\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
		Log("Total coarse = %f\n", s->value[COARSE_ROOT_C]);
	}
	if (fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C])>1e-4)
	{
		Log("Live branch + dead branch = %f\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
		Log("Total branch = %f\n", s->value[BRANCH_C]);
		//ALESSIOR
		Log("ATTENTION CHECK BALANCE!\n");
	}
	else
	{
		Log("Live branch + dead branch = %f\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
		Log("Total branch = %f\n", s->value[BRANCH_C]);
	}

	CHECK_CONDITION(s->value[RESERVE_C], < 0);
	CHECK_CONDITION(s->value[LEAF_C], < 0);
	CHECK_CONDITION(s->value[FINE_ROOT_C], < 0);
	CHECK_CONDITION(s->value[STEM_C], < 0);
	CHECK_CONDITION(s->value[STEM_LIVE_WOOD_C], < 0);
	CHECK_CONDITION(s->value[STEM_DEAD_WOOD_C], < 0);
	CHECK_CONDITION(s->value[BRANCH_C], < 0);
	CHECK_CONDITION(s->value[BRANCH_LIVE_WOOD_C], < 0);
	CHECK_CONDITION(s->value[BRANCH_DEAD_WOOD_C], < 0);
	CHECK_CONDITION(s->value[COARSE_ROOT_C], < 0);
	CHECK_CONDITION(s->value[COARSE_ROOT_LIVE_WOOD_C], < 0);
	CHECK_CONDITION(s->value[COARSE_ROOT_DEAD_WOOD_C], < 0);
	CHECK_CONDITION(s->value[FRUIT_C], < 0);

	Log("\n-Daily increment in carbon pools-\n");
	Log("C_TO_TOT_STEM = %f tC/cell/day\n", s->value[C_TO_TOT_STEM]);
	Log("C_TO_LEAF = %f tC/cell/day\n", s->value[C_TO_LEAF]);
	Log("C_TO_FINEROOT = %f tC/cell/day\n", s->value[C_TO_FINEROOT]);
	Log("C_TO_COARSEROOT = %f tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	Log("C_TO_STEM = %f tC/cell/day\n", s->value[C_TO_STEM]);
	Log("C_TO_RESERVE = %f tC/cell/day\n", s->value[C_TO_RESERVE]);
	Log("C_TO_BRANCH = %f tC/cell/day\n", s->value[C_TO_BRANCH]);

	c->daily_lai[i] = s->value[LAI];
	c->annual_delta_wres[i] += s->value[DEL_RESERVE];

	/* turnover */
	Turnover(&c->heights[height].ages[age].species[species]);

	/* update class level annual carbon biomass increment in tC/cell/year */
	s->value[DEL_Y_WTS] += s->value[C_TO_TOT_STEM];
	s->value[DEL_Y_WS] += s->value[C_TO_STEM];
	s->value[DEL_Y_WF] += s->value[C_TO_LEAF];
	s->value[DEL_Y_WFR] += s->value[C_TO_FINEROOT];
	s->value[DEL_Y_WCR] += s->value[C_TO_COARSEROOT];
	s->value[DEL_Y_WRES] += s->value[C_TO_RESERVE];
	s->value[DEL_Y_WR] += s->value[C_TO_ROOT];
	s->value[DEL_Y_BB] += s->value[C_TO_BRANCH];

	/* update layer level daily carbon biomass increment in tC/cell/day */
	c->daily_delta_wts[i] = s->value[C_TO_TOT_STEM];
	c->daily_delta_ws[i] = s->value[C_TO_STEM];
	c->daily_delta_wf[i] = s->value[C_TO_LEAF];
	c->daily_delta_wbb[i] = s->value[C_TO_BRANCH];
	c->daily_delta_wfr[i] = s->value[C_TO_FINEROOT];
	c->daily_delta_wcr[i] = s->value[C_TO_COARSEROOT];
	c->daily_delta_wres[i] = s->value[C_TO_RESERVE];

	/* update layer level annual carbon increments and pools in tC/cell/year */
	c->annual_delta_ws[i] += s->value[C_TO_STEM];
	c->annual_layer_stem_c[i] = s->value[STEM_C];
	c->annual_layer_live_stem_c[i] = s->value[STEM_LIVE_WOOD_C];
	c->annual_delta_wres[i] += s->value[C_TO_RESERVE];
	c->annual_layer_reserve_c[i] = s->value[RESERVE_C];
	c->annual_delta_wf[i] += s->value[C_TO_LEAF];
	c->annual_layer_leaf_c[i] = s->value[LEAF_C];
	c->annual_delta_wts[i] += s->value[C_TO_TOT_STEM];
	c->annual_layer_tot_stem_c[i] = s->value[TOT_STEM_C];
	c->annual_delta_wbb[i] += s->value[C_TO_BRANCH];
	c->annual_layer_branch_c[i] = s->value[BRANCH_C];
	c->annual_layer_live_branch_c[i] = s->value[BRANCH_LIVE_WOOD_C];
	c->annual_delta_wfr[i] += s->value[C_TO_FINEROOT];
	c->annual_layer_fineroot_c[i] = s->value[FINE_ROOT_C];
	c->annual_delta_wcr[i] += s->value[C_TO_COARSEROOT];
	c->annual_layer_coarseroot_c[i] = s->value[COARSE_ROOT_C];
	c->annual_layer_live_coarseroot_c[i] = s->value[COARSE_ROOT_LIVE_WOOD_C];

	/* update cell level carbon biomass in gC/m2/day*/
	c->daily_leaf_carbon += s->value[C_TO_LEAF] * 1000000.0 / settings->sizeCell ;
	c->daily_stem_carbon += s->value[C_TO_STEM] * 1000000.0 / settings->sizeCell ;
	c->daily_fine_root_carbon += s->value[C_TO_FINEROOT] * 1000000.0 / settings->sizeCell ;
	c->daily_coarse_root_carbon += s->value[C_TO_COARSEROOT] * 1000000.0 / settings->sizeCell ;
	c->daily_branch_carbon += s->value[C_TO_BRANCH] * 1000000.0 / settings->sizeCell ;
	c->daily_reserve_carbon += s->value[C_TO_RESERVE] * 1000000.0 / settings->sizeCell ;
	c->daily_root_carbon += s->value[C_TO_ROOT] * 1000000.0 / settings->sizeCell ;
	c->daily_litter_carbon += s->value[C_TO_LITTER] * 1000000.0 / settings->sizeCell ;

	/* update cell level carbon biomass in tC/cell/day*/
	c->daily_leaf_carbon_tC += s->value[C_TO_LEAF];
	c->daily_stem_carbon_tC += s->value[C_TO_STEM];
	c->daily_fine_root_carbon_tC += s->value[C_TO_FINEROOT];
	c->daily_coarse_root_carbon_tC += s->value[C_TO_COARSEROOT];
	c->daily_branch_carbon_tC += s->value[C_TO_BRANCH];
	c->daily_reserve_carbon_tC += s->value[C_TO_RESERVE];
	c->daily_root_carbon_tC += s->value[C_TO_ROOT];
	c->daily_litter_carbon_tC += s->value[C_TO_LITTER];

	Log("******************************\n");
}

/**/

