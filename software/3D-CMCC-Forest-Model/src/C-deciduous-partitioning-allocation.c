

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



//VERSION CURRENTLY USED
//Deciduous carbon allocation routine
void D_Get_Partitioning_Allocation (SPECIES *const s, CELL *const c, const MET_DATA *const met, int day, int month, int years, int DaysInMonth, int height, int age, int species)
{

	int i;

	//int phenology_phase;
	//allocation parameter. their sum must be = 1

	double  s0Ctem = s->value[S0CTEM];
	double  r0Ctem = s->value[R0CTEM];
	//double  f0Ctem = s->value[F0CTEM];
	double const omegaCtem = s->value[OMEGA_CTEM];
	double pS_CTEM = 0;
	double pR_CTEM = 0;
	double pF_CTEM = 0;
	//double max_DM_foliage;
	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	double oldW;
	double Daily_solar_radiation;
	double Monthly_solar_radiation;
	double Light_trasm;
	double Par_over;
	double Perc_fine;
	double Perc_coarse;
	//double Perc_leaves;              //percentage of leaves in first growing season

	static double frac_to_foliage_stem;
	//Marconi
	double parameter; // parameter for exponential function to be used to gradually allocate biomass reserve during bud burst

	i = c->heights[height].z;

	//CTEM VERSION

	Log("\nGET_ALLOCATION_ROUTINE\n\n");
	Log("Carbon allocation routine for deciduous\n");


	//Log ("S0CTEM  = %f \n", s0Ctem);
	//Log ("R0CTEM  = %f \n", r0Ctem);
	//Log ("F0CTEM  = %f \n", f0Ctem );
	//Log ("OMEGA_CTEM  = %f \n", omegaCtem);
	//Log ("EPSILON_CTEM  = %f \n", epsilon);
	//Log ("KAPPA_CTEM  = %f \n", kappa);


	if (settings->time == 'm')
	{
		Monthly_solar_radiation = met[month].solar_rad * MOLPAR_MJ * DaysInMonth;
		Par_over = c->par - s->value[APAR];
		Light_trasm = Par_over / Monthly_solar_radiation;

		//if (month == 0)
		//leaf_fall_counter = 0;
	}
	else
	{

		Daily_solar_radiation = met[month].d[day].solar_rad * MOLPAR_MJ;

		Par_over = c->par - s->value[APAR];
		Light_trasm = Par_over /Daily_solar_radiation;

		//if (day == 0 && month == 0)
		//leaf_fall_counter = 0;

	}


	if (day == 0 && month == 0 && years == 0)
	{
		s->value[BIOMASS_FRUIT] = 0;
	}



	//7 May 2012
	//compute static ratio of allocation between fine and coarse root
	//deriving data from values reported for BIOME-BGC
	//fixme see if change with new parameters checked in "Get_biome_fraction"
	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
	//Log("Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
	//Log("Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
	Perc_coarse = 1- Perc_fine;


	if (s->counter[VEG_DAYS] == 1)
	{
		s->counter[BUD_BURST_COUNTER] = s->value[BUD_BURST];
		Log("First day of budburst\n");
		Log("Days for bud burst = %f\n", s->value[BUD_BURST]);
	}
	if (s->counter[VEG_DAYS] > 1 && s->counter[VEG_DAYS] <= s->value[BUD_BURST])
	{
		s->counter[BUD_BURST_COUNTER] --;
		Log("++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
	}
	if (s->counter[VEG_DAYS] > s->value[BUD_BURST])
	{
		s->counter[BUD_BURST_COUNTER] = 0;
	}

	if (s->counter[LEAF_FALL_COUNTER] == 1)
	{
		Log("First day of Leaf fall\n");
		Log("\nday, month: %d\t%d", month+1, day+1);
		s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE] = s->value[BIOMASS_FOLIAGE] * s->value[FOLIAGE_REDUCTION_RATE];
		Log("foliage biomass to remove = %f\n", s->value[BIOMASS_FOLIAGE]);
		Log("Daily amount of foliage biomass to remove = %f\n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
		//Marconi: assumed that fine roots for deciduos species progressively die togheter with leaves
		s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE] = s->value[BIOMASS_ROOTS_FINE] * s->value[FOLIAGE_REDUCTION_RATE];
	}



	//I could try to get in instead F_SW the minimum value between F_SW and F_VPD  2 apr 2012
	//reductor = Minimum (s->value[F_SW], s->value[F_VPD]);
	//I could try to get in instead F_SW the minimum value between F_SW and F_NUTR  18 apr 2012
	//reductor = Minimum (s->value[F_SW], s->value[F_NUTR]);
	//reductor = s->value[F_SW];

	//todo use it if a better function of fSW is developed
	/*
		if (reductor == s->value[F_SW])
		{
			Log("reductor in CTEM is F_SW \n");
		}
		else
		{
			Log("reductor in CTEM is F_NUTR \n");
		}
	 */

	Log("(CTEM) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d , --\n", c->heights[height].z);



	if (settings->spatial == 'u')
	{
		oldW = s->value[BIOMASS_FOLIAGE] + s->value[BIOMASS_STEM] + s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_ROOTS_FINE] + s->value[BIOMASS_BRANCH];

		//Log("Percentage of coarse root against total root= %f %%\n", Perc_coarse * 100 );

		//23 May 2012
		//percentage of leaves against fine roots
		//Perc_leaves =  1 - (s->value[FINE_ROOT_LEAF]  / (s->value[FINE_ROOT_LEAF] + 1));
		//Log("Percentage of leaves against fine roots = %f %%\n", Perc_leaves * 100);
		//Log("Percentage of fine roots against leaves = %f %%\n", (1 - Perc_leaves) * 100);

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

			double r0Ctem_increment;
			double old_r0Ctem = r0Ctem;
			double s0Ctem_increment;
			double old_s0Ctem = s0Ctem;

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
		Log("PEAK LAI = %f \n", s->value[PEAK_Y_LAI]);


		switch (s->phenology_phase)
		{
		/************************************************************/
		case 1:
			Log("BUDBURST\n");
			Log("Bud burst phase using both reserve pools and npp\n");
			Log("Allocating only into foliage and fine root pools\n");
			Log("LAI = %f \n", s->value[LAI]);

			s->value[DAILY_DEL_LITTER] = 0;

			/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/


			if (s->value[BIOMASS_RESERVE] < 0.0)
			{
				Log("ATTENTION BIOMASS RESERVE < 0.0\n");
				frac_to_foliage_stem = 0;
				Log("fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_stem);
			}
			else
			{
				//frac_to_foliage_stem = (s->value[BIOMASS_RESERVE]) / s->counter[BUD_BURST_COUNTER];
				parameter = 2.0 / pow(s->value[BUD_BURST],2.0);
				frac_to_foliage_stem = (s->value[BIOMASS_RESERVE]) * parameter * (s->value[BUD_BURST]+1.0 - s->counter[BUD_BURST_COUNTER]);

				Log("fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_stem);
			}
			Log("++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);

			/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			 * and Barbaroux et al., 2002,
								the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
			 */
			/*the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
			 * sharing the daily remaining amount (taking into account respiration costs)of NSC */
			//Angelo try to change with a exponential function as frac_to_foliage = s->value[BIOMASS_RESERVE] * (e^-s->value[BUD_BURST])
			//fixme try to allocate just a part of total reserve not all


			/*partitioning*/
			if (s->value[NPP] > 0.0)
			{
				Log("Using reserve and npp\n");
				/*
				s->value[DEL_FOLIAGE] = (frac_to_foliage_stem * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]))	+ (s->value[NPP] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
				s->value[DEL_ROOTS_FINE_CTEM] = (frac_to_foliage_stem * s->value[FINE_ROOT_LEAF_FRAC]) + (s->value[NPP] * s->value[FINE_ROOT_LEAF_FRAC]);
				 */
				s->value[DEL_FOLIAGE] = (frac_to_foliage_stem * (1.0/ (s->value[STEM_LEAF] +1.0))) + s->value[NPP];
				s->value[DEL_STEMS] = (frac_to_foliage_stem - s->value[DEL_FOLIAGE]);
				s->value[DEL_RESERVE] = - frac_to_foliage_stem;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = s->value[DEL_STEMS];				
				s->value[DEL_BB] = 0;
			}
			else
			{
				if (s->value[BIOMASS_RESERVE] > 0)
				{
					Log("Using ONLY reserve\n");
					/*
					s->value[DEL_FOLIAGE] = (frac_to_foliage_stem * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
					s->value[DEL_ROOTS_FINE_CTEM] = (frac_to_foliage_stem * s->value[FINE_ROOT_LEAF_FRAC]);
					 */
					s->value[DEL_FOLIAGE] = (frac_to_foliage_stem * (1.0/ (s->value[STEM_LEAF] + 1.0)));
					s->value[DEL_STEMS] = (frac_to_foliage_stem - s->value[DEL_FOLIAGE]);
					s->value[DEL_RESERVE] = -(((abs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell) + frac_to_foliage_stem);
					s->value[DEL_ROOTS_FINE_CTEM] = 0;
					s->value[DEL_ROOTS_COARSE_CTEM] = 0;
					s->value[DEL_ROOTS_TOT] = 0;
					s->value[DEL_TOT_STEM] = frac_to_foliage_stem * s->value[STEM_LEAF];
					s->value[DEL_BB]= 0;
				}
				else
				{
					Log("No reserve no NPP\n");
					/*
					s->value[DEL_ROOTS_FINE_CTEM] = (frac_to_foliage_stem * s->value[FINE_ROOT_LEAF_FRAC]);
					s->value[DEL_RESERVE] = ((s->value[C_FLUX] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell)- frac_to_foliage_stem;
					 */
					s->value[DEL_FOLIAGE] = 0;
					s->value[DEL_ROOTS_FINE_CTEM] = 0;

					s->value[DEL_ROOTS_COARSE_CTEM] = 0;
					s->value[DEL_ROOTS_TOT] = 0;
					s->value[DEL_TOT_STEM] = 0;
					s->value[DEL_STEMS]= 0;
					s->value[DEL_BB]= 0;
				}
			}

			/*allocation*/
			s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);
			s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
			s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
			s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			//fixme maybe just tot_stem instead stem + bb
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				Log("computing LAI for dominant trees\n");
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000.0) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				Log("LAI = %f\n", s->value[LAI]);
			}
			/*for dominated shaded foliage*/
			else
			{
				Log("computing LAI for dominated trees\n");
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000.0) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				Log("LAI = %f\n", s->value[LAI]);
			}

			/*check if re-transfer foliage biomass to reserve*/
			if (s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("LAI exceeds Peak Lai\n");
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ (s->value[SLAmkg]* GC_GDM)) / 1000.0;
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ ((s->value[SLAmkg] * s->value[SLA_RATIO])* GC_GDM)) / 1000.0;
				}

				/*partitioning*/
				/*re-transfer mass to reserve*/
				s->value[DEL_FOLIAGE] -= (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]);
				//s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_RESERVE] += s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE];
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS] = 0;
				s->value[DEL_BB] = 0;

				/*allocation*/
				s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
				Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);
				s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
				Log("Total Stem Biomass (Wts) = %f\n", s->value[BIOMASS_TOT_STEM]);
				s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
				Log("Stem Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM]);
				s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
				s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
				s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
				s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
				s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

				//check for live and dead tissues
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

				/*recompute correct LAI*/
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
					Log("recomputed LAI = %f\n", s->value[LAI]);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
					Log("recomputed LAI = %f\n", s->value[LAI]);
				}
			}


			// Total Biomass
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE] + s->value[BIOMASS_STEM] + s->value[BIOMASS_ROOTS_TOT] + s->value[BIOMASS_RESERVE] + s->value[BIOMASS_BRANCH];
			Log("Previous Total W = %f tDM/area\n", oldW);
			Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);

			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_Wts %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			c->daily_wres[i] = s->value[BIOMASS_RESERVE];
			break;

		case 2:
			Log("(PEAK_Y_LAI * 0.5 < LAI < PEAK_Y_LAI) \n");
			Log("LAI = %f \n", s->value[LAI]);
			Log("**Maximum Growth**\n");
			Log("Allocating only into foliage and fine root pools\n");

			s->value[DAILY_DEL_LITTER] = 0;

			//fixme scegliere se usare Magnani o meno
			//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
			/*partitioning*/
			if(s->value[NPP] > 0.0)
			{
				/*allocating into fine root and foliage*/
				s->value[DEL_FOLIAGE] = (s->value[NPP] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
				s->value[DEL_ROOTS_FINE_CTEM] = (s->value[NPP] * s->value[FINE_ROOT_LEAF_FRAC]);
				s->value[DEL_RESERVE] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS]= 0;
				s->value[DEL_BB]= 0;
			}
			else
			{
				s->value[DEL_FOLIAGE] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_RESERVE] = -((abs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS]= 0;
				s->value[DEL_BB]= 0;
			}

			/*allocation*/
			s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);
			s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
			s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
			s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


			//recompute LAI
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				Log("LAI = %f\n", s->value[LAI]);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				Log("LAI = %f\n", s->value[LAI]);
			}

			//control if new Lai exceeds Peak Lai
			if (s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("PHENOLOGICAL PHASE = 2.1 \n (TOO MUCH BIOMASS INTO FOLIAGE)\n");
				Log("LAI = %f \n", s->value[LAI]);

				Log("LAI exceeds Peak Lai\n");
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ (s->value[SLAmkg]* GC_GDM)) / 1000;
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ ((s->value[SLAmkg] * s->value[SLA_RATIO])* GC_GDM)) / 1000;
				}


				Log("Re-allocating %f foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n", (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]));


				/*partitioning*/
				/*re-transfer mass to other pools*/
				s->value[DEL_FOLIAGE] -= (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]);
				s->value[DEL_ROOTS_TOT] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pR_CTEM;
				s->value[DEL_ROOTS_FINE_CTEM] += (s->value[DEL_ROOTS_TOT] * Perc_fine);
				s->value[DEL_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_TOT] * Perc_coarse;
				s->value[DEL_TOT_STEM] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) *  pS_CTEM;
				s->value[DEL_STEMS] += ((s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) *  pS_CTEM) * ( 1.0 - s->value[FRACBB]);
				s->value[DEL_BB] += ((s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pS_CTEM) * s->value[FRACBB];
				s->value[DEL_RESERVE] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pF_CTEM;

				Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);


				/*allocation*/
				s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
				Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);
				s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
				Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
				s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
				Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);
				s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
				s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
				s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
				s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
				s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

				//check for live and dead tissues
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

				//recompute LAI
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
					Log("LAI = %f\n", s->value[LAI]);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
					Log("LAI = %f\n", s->value[LAI]);
				}
			}


			// Total Biomass
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE] + s->value[BIOMASS_STEM] + s->value[BIOMASS_ROOTS_TOT] + s->value[BIOMASS_RESERVE] + s->value[BIOMASS_BRANCH];
			Log("Previous Total W = %f tDM/area\n", oldW);
			Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);

			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			c->daily_wres[i] = s->value[BIOMASS_RESERVE];

			break;
			/************************************************************/
		case 3:
			Log("**Normal Growth**\n");
			Log("Day length > %f \n", /*c->abscission_daylength*/s->value[MINDAYLENGTH] );
			Log("(LAI MAX * 0.5 < LAI < LAI MAX)\n");
			Log("allocating into the three pools Ws+Wr+Wf\n");

			s->value[DAILY_DEL_LITTER] = 0;


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Roots CTEM ratio layer %d = %f %%\n", z, pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			//Log("Foliage CTEM ratio = %f %%\n", pF_CTEM * 100);

			/*partitioning*/
			if (s->value[NPP] > 0.0)
			{
				//fixme do it also for 0.1
				//REPRODUCTION ONLY FOR NEEDLE LEAF DECIDUOUS
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[DEL_FRUIT] = s->value[NPP] * s->value[FRUIT_PERC];
					s->value[NPP] -= s->value[DEL_FRUIT];
					s->value[BIOMASS_FRUIT] += s->value[DEL_FRUIT];
					Log("Biomass increment into cones = %f tDM/area\n", s->value[DEL_FRUIT]);
					//reproductive life span
					s->value[BIOMASS_FRUIT] -= (s->value[BIOMASS_FRUIT] * (1 / s->value[CONES_LIFE_SPAN]));
				}

				s->value[DEL_RESERVE] = 0;
				s->value[DEL_FOLIAGE] = s->value[NPP] * pF_CTEM;
				s->value[DEL_ROOTS_TOT] = s->value[NPP] * pR_CTEM;
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT] * Perc_fine;
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_TOT_STEM] = s->value[NPP] * pS_CTEM;
				s->value[DEL_STEMS] = (s->value[NPP] * pS_CTEM) * (1.0 - s->value[FRACBB]);
				s->value[DEL_BB] = s->value[DEL_TOT_STEM] * s->value[FRACBB];
			}
			else
			{
				s->value[DEL_FOLIAGE] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_RESERVE] = -((abs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS]= 0;
				s->value[DEL_BB]= 0;
			}

			/*allocation*/
			s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
			s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
			Log("Stem Biomass = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
			s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Coarse root Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);




			//recompute LAI
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				Log("LAI = %f\n", s->value[LAI]);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				Log("LAI = %f\n", s->value[LAI]);
			}

			//control if new Lai exceeds Peak Lai
			if (s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("PHENOLOGICAL PHASE = 2.1 \n (TOO MUCH BIOMASS INTO FOLIAGE)\n");
				Log("LAI = %f \n", s->value[LAI]);

				Log("LAI exceeds Peak Lai\n");
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ (s->value[SLAmkg]* GC_GDM)) / 1000;
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[MAX_BIOMASS_FOLIAGE] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ ((s->value[SLAmkg] * s->value[SLA_RATIO])* GC_GDM)) / 1000;
				}


				Log("Re-allocating foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n");

				/*partitioning*/
				/*re-transfer mass to other pools*/
				s->value[DEL_FOLIAGE] -= (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]);
				s->value[DEL_ROOTS_TOT] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pR_CTEM;
				s->value[DEL_ROOTS_FINE_CTEM] += (s->value[DEL_ROOTS_TOT] * Perc_fine);
				s->value[DEL_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_TOT] * Perc_coarse;
				s->value[DEL_TOT_STEM] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) *  pS_CTEM;
				s->value[DEL_STEMS] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * (1.0 - s->value[FRACBB]);
				s->value[DEL_BB] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * s->value[FRACBB];
				s->value[DEL_RESERVE] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pF_CTEM;

				/*allocation*/
				s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
				Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM]);
				s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
				Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
				s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
				Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM]);
				s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
				s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
				s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
				s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
				s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

				//check for live and dead tissues
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

			}

			// Total Biomass
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE] + s->value[BIOMASS_STEM] + s->value[BIOMASS_ROOTS_TOT] + s->value[BIOMASS_RESERVE] + s->value[BIOMASS_BRANCH];
			Log("Previous Total W = %f tDM/area\n", oldW);
			Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);


			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			c->daily_wres[i] = s->value[BIOMASS_RESERVE];

			break;
			/************************************************************************/
		case 4:
			Log("(LAI == PEAK LAI)\n");
			Log("allocating into the three pools Ws+Wr+Wreserve\n");
			/*see Barbaroux et al., 2002, Scartazza et al., 2013*/

			s->value[DAILY_DEL_LITTER] = 0;


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Roots CTEM ratio layer = %f %%\n", pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);

			/*partitioning*/
			if (s->value[NPP] > 0.0)
			{
				//fixme do it also for 0.1
				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[BIOMASS_FRUIT] = s->value[NPP] * s->value[FRUIT_PERC];
					s->value[NPP] -= s->value[BIOMASS_FRUIT];
					Log("Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT]);

					//reproductive life span
					s->value[BIOMASS_FRUIT] -= (s->value[BIOMASS_FRUIT] * (1 / s->value[CONES_LIFE_SPAN]));
				}

				s->value[DEL_RESERVE] = s->value[NPP] * pF_CTEM;
				s->value[DEL_ROOTS_TOT] = s->value[NPP] * pR_CTEM;
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT] * Perc_fine;
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_TOT_STEM] = s->value[NPP] * pS_CTEM;
				s->value[DEL_STEMS] = (s->value[NPP] * pS_CTEM) * (1.0 - s->value[FRACBB]);
				s->value[DEL_BB] = (s->value[NPP] * pS_CTEM) * s->value[FRACBB];
				s->value[DEL_FOLIAGE] = 0;
			}
			else
			{
				s->value[DEL_FOLIAGE] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_RESERVE] = -((abs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS]= 0;
				s->value[DEL_BB]= 0;
			}

			/*allocation*/
			s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
			s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
			Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
			s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			c->daily_wres[i] = s->value[BIOMASS_RESERVE];


			break;
			/**********************************************************************/
		case 5:
			Log("(DayLength < MINDAYLENGTH)\n");
			Log("LEAF FALL\n");
			Log("allocating into W reserve pool\n");
			Log("++Lai before Leaf fall= %f\n", s->value[LAI]);
			Log("Biomass foliage = %f \n", s->value[BIOMASS_FOLIAGE]);
			Log("foliage reduction rate %f \n", s->value[FOLIAGE_REDUCTION_RATE]);
			Log("biomass foliage to remove %f \n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);

			if (s->value[NPP] > 0.0)
			{

				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[BIOMASS_FRUIT] = s->value[NPP] * s->value[FRUIT_PERC];
					s->value[NPP] -= s->value[BIOMASS_FRUIT];
					Log("Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT]);

					//reproductive life span
					s->value[BIOMASS_FRUIT] -= (s->value[BIOMASS_FRUIT] * (1 / s->value[CONES_LIFE_SPAN]));
				}
				s->value[DEL_FOLIAGE] =  -s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE];
				s->value[DAILY_DEL_LITTER] = s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE];
				s->value[DEL_RESERVE] = s->value[NPP];
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = -s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE];
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_BB] = 0;
			}
			else
			{
				s->value[DEL_FOLIAGE] = - s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE];
				s->value[DEL_TOT_STEM] = 0;
				s->value[DEL_STEMS] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = -s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE];
				s->value[DEL_ROOTS_TOT] = 0;
				s->value[DEL_BB] = 0;
			}

			/*allocation*/
			s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
			s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
			Log("Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
			s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

			//recompute LAI
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				Log("Lai = %f\n", s->value[LAI]);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				Log("Lai = %f\n", s->value[LAI]);
			}

			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			c->daily_wres[i] = s->value[BIOMASS_RESERVE];

			c->leafLittering += s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE] / GC_GDM * 1000 / settings->sizeCell;
			c->leaflitN = c->leafLittering /GC_GDM * 1000 / settings->sizeCell /s->value[CN_DEAD_WOODS];

			c->fineRootLittering +=  s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE] / GC_GDM * 1000 / settings->sizeCell;
			c->fineRootlitN += s->value[BIOMASS_ROOTS_FINE] / GC_GDM * 1000 / settings->sizeCell  / s->value[CN_FINE_ROOTS];

			break;

		case 0:

			Log("Unvegetative period \n");

			s->value[DAILY_DEL_LITTER] = 0;

			/*partitioning*/
			s->value[DEL_RESERVE] = -((s->value[TOTAL_AUT_RESP] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
			s->value[DEL_FOLIAGE] = 0;
			s->value[DEL_ROOTS_FINE_CTEM] = 0;
			s->value[DEL_ROOTS_COARSE_CTEM] = 0;
			s->value[DEL_ROOTS_TOT] = 0;
			s->value[DEL_TOT_STEM] = 0;
			s->value[DEL_STEMS]= 0;
			s->value[DEL_BB]= 0;



			/*allocation*/
			s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
			Log("Foliage Biomass = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
			Log("Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
			s->value[BIOMASS_STEM] +=  s->value[DEL_STEMS];
			Log("Stem Biomass = %f tDM/area\n", s->value[BIOMASS_STEM]);
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
			s->value[BIOMASS_RESERVE] += s->value[DEL_RESERVE];
			Log("Reserve Biomass = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);


			s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

			c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
			c->daily_delta_ws[i] = s->value[DEL_STEMS];
			c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
			c->daily_delta_wbb[i] = s->value[DEL_BB];
			c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
			c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
			c->daily_delta_wres[i] = s->value[DEL_RESERVE];

			c->daily_wres[i] = s->value[BIOMASS_RESERVE];

			break;
		}
	}

	//fixme compute from unspatial
	if (settings->spatial == 's')
	{
		Log("Spatial version \n");
		oldW = s->value[BIOMASS_FOLIAGE] + s->value[BIOMASS_STEM] + s->value[BIOMASS_ROOTS_COARSE] + s->value[BIOMASS_ROOTS_FINE];

		//7 May 2012
		//compute static ratio of allocation between fine and coarse root
		//deriving data from values reported for BIOME-BGC
		s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
		//Log("Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
		Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
		//Log("Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
		Perc_coarse = 1- Perc_fine;
		//Log("Percentage of coarse root against total root= %f %%\n", Perc_coarse * 100 );

		//23 May 2012
		//percentage of leaves against fine roots
		//Perc_leaves =  1 - (s->value[FINE_ROOT_LEAF]  / (s->value[FINE_ROOT_LEAF] + 1));
		//Log("Percentage of leaves against fine roots = %f %%\n", Perc_leaves * 100);
		//Log("Percentage of fine roots against leaves = %f %%\n", (1 - Perc_leaves) * 100);



		//(Arora V. K., Boer G. J., GCB, 2005)

		Log("(CTEM) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", c->heights[height].z);
		Log("PEAK_LAI = %f \n", s->value[PEAK_Y_LAI]);

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

			double r0Ctem_increment;
			double old_r0Ctem = r0Ctem;
			double s0Ctem_increment;
			double old_s0Ctem = s0Ctem;

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
			//allocation ratio to stem
			s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
			s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
			s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
			Log ("new s0_CTEM = %f \n", s0Ctem);

			if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)
			{
				Log("ERROR IN s0Ctem !!! \n");
			}

		}

		switch (s->phenology_phase)
		{
		Log("PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
		if (settings->time == 'm')
		{
			Log("NDVI-LAI = %f \n", met[month].ndvi_lai);
		}
		else
		{
			Log("NDVI-LAI = %f \n", met[month].d[day].ndvi_lai);
		}
		/************************************************************/
		case 1:
			Log("(NDVI-LAI < PEAK_Y_LAI * 0.5)\n");
			if (settings->time == 'm')
			{
				Log("NDVI-LAI = %f \n", met[month].ndvi_lai);
			}
			else
			{
				Log("NDVI-LAI = %f \n", met[month].d[day].ndvi_lai);
			}
			Log("**Maximum Growth**\n");
			Log("allocating only into foliage pools\n");

			s->value[DEL_FOLIAGE] = s->value[NPP];
			Log("DEL FOLIAGE = %f\n", s->value[DEL_FOLIAGE]);
			s->value[BIOMASS_FOLIAGE] +=  s->value[DEL_FOLIAGE];
			Log("BiomassFoliage CTEM = %f tDM/area\n", s->value[BIOMASS_FOLIAGE] );

			//recompute LAI
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
			}
			Log("++Lai = %f\n", s->value[LAI]);

			s->value[DEL_STEMS] = 0;
			s->value[DEL_ROOTS_FINE_CTEM] = 0;
			s->value[DEL_ROOTS_COARSE_CTEM] = 0;
			s->value[DEL_ROOTS_TOT] = 0;
			s->value[DEL_RESERVE] = 0;
			s->value[DEL_BB] = 0;

			break;
			/************************************************************/
		case 2:
			Log("**Normal Growth**\n");
			Log("(Day length > %f)\n", /*c->abscission_daylength*/ s->value[MINDAYLENGTH] );
			Log("(LAI MAX * 0.5 < NDVI-LAI < LAI MAX)\n");
			Log("allocating into the three pools Ws+Wr+Wf\n");


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Roots CTEM ratio layer %d = %f %%\n", z, pR_CTEM * 100);


			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);



			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			//Log("Foliage CTEM ratio = %f %%\n", pF_CTEM * 100);

			//REPRODUCTION ONLY FOR NEEDLE LEAF
			if (s->value[PHENOLOGY] == 0.2)
			{
				//NPP for reproduction
				s->value[BIOMASS_FRUIT] = s->value[NPP] * s->value[FRUIT_PERC];
				s->value[NPP] -= s->value[BIOMASS_FRUIT];
				Log("Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT]);

				//reproductive life span
				s->value[BIOMASS_FRUIT] -= (s->value[BIOMASS_FRUIT] * (1 / s->value[CONES_LIFE_SPAN]));
			}


			// Biomass allocation

			s->value[DEL_ROOTS_TOT] = s->value[NPP] * pR_CTEM;
			Log("BiomassRoots increment CTEM = %f tDM/area\n", s->value[DEL_ROOTS_TOT]);

			//7 May 2012
			//comnpute fine and coarse root biomass
			s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT]  * Perc_fine;
			Log("BiomassRoots increment into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
			s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] * Perc_coarse;
			Log("BiomassRoots increment into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

			if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT])
			{
				Log("ERROR IN ROOTS ALLOCATION del coarse + del fine = %f, del tot = %f \n", (s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]), s->value[DEL_ROOTS_TOT]);
			}


			s->value[DEL_STEMS] = s->value[NPP] *  pS_CTEM;
			Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS]);


			s->value[DEL_FOLIAGE] = s->value[NPP] * pF_CTEM;
			Log("BiomassFoliage increment CTEM = %f tDM/area\n", s->value[DEL_FOLIAGE] );

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_STEMS] * s->value[FRACBB];
			Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
			//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_STEMS] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM] +=  s->value[DEL_STEMS];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);

			//allocation to branch and bark
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);

			//allocation to foliage
			s->value[BIOMASS_FOLIAGE] +=  s->value[DEL_FOLIAGE];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);

			//recompute LAI
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
			}

			//allocation to roots
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);

			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			/*stem*/
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			/*coarse root*/
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			/*branch and bark*/
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

			s->value[DEL_RESERVE] = 0;


			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("aF %d = %f \n", c->heights[height].z, pF_CTEM);
			Log("afR %d = %f \n", c->heights[height].z, Perc_fine * pR_CTEM);
			Log("acR %d = %f \n", c->heights[height].z, Perc_coarse * pR_CTEM);
			Log("aS %d = %f \n", c->heights[height].z, pS_CTEM);
			Log("aRes %d = 0 \n", c->heights[height].z);

			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE] );
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);




			break;
			/************************************************************************/
		case 3:
			Log("(NDVI-LAI == PEAK LAI or Month > 6)\n");
			Log("allocating into the three pools Ws+Wr+Wreserve\n");

			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Roots CTEM ratio layer %d = %f %%\n", z, pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			//Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);


			//REPRODUCTION ONLY FOR NEEDLE LEAF
			if (s->value[PHENOLOGY] == 0.2)
			{
				//NPP for reproduction
				s->value[DEL_FRUIT] = s->value[NPP] * s->value[FRUIT_PERC];
				s->value[NPP] -= s->value[DEL_FRUIT];
				s->value[BIOMASS_FRUIT] += s->value[DEL_FRUIT];
				Log("Biomass increment into cones = %f tDM/area\n", s->value[DEL_FRUIT]);

				//reproductive life span
				//s->value[BIOMASS_FRUIT] -= (s->value[BIOMASS_FRUIT] * (1 / s->value[CONES_LIFE_SPAN]));
			}


			// Biomass allocation

			s->value[DEL_ROOTS_TOT] = s->value[NPP] * pR_CTEM;
			Log("BiomassRoots increment CTEM = %f tDM/area\n", s->value[DEL_ROOTS_TOT]);


			//7 May 2012
			//compute fine and coarse root biomass
			s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT]  * Perc_fine;
			Log("BiomassRoots into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
			s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] * Perc_coarse;
			Log("BiomassRoots into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


			s->value[DEL_STEMS] = s->value[NPP] * pS_CTEM;
			Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS]);

			s->value[DEL_RESERVE] = s->value[NPP] * pF_CTEM;
			Log("BiomassReserve increment CTEM = %f tDM/area\n", s->value[DEL_RESERVE] );

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_STEMS] * s->value[FRACBB];
			Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
			//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_STEMS] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM] +=  s->value[DEL_STEMS];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);

			//allocation to branch and bark
			s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);

			//allocation to non structural reserve pool
			s->value[BIOMASS_RESERVE] +=  s->value[DEL_RESERVE];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);

			//allocation to roots
			s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);

			s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			//check for live and dead tissues
			/*stem*/
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			/*coarse root*/
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			/*branch and bark*/
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


			s->value[DEL_Y_WS] += s->value[DEL_STEMS];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
			s->value[DEL_Y_BB] += s->value[DEL_BB];

			Log("aF %d = 0 \n", c->heights[height].z);
			Log("afR %d = %f \n", c->heights[height].z, Perc_fine * pR_CTEM);
			Log("acR %d = %f \n", c->heights[height].z, Perc_coarse * pR_CTEM);
			Log("aS %d = %f \n", c->heights[height].z, pS_CTEM);
			Log("aRes %d = %f \n", c->heights[height].z, pF_CTEM);

			Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE] );
			Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
			Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
			Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
			Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
			Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);


			break;

		case 4:

			Log("Unvegetative period \n");

			Log("aF %d = 0 \n", c->heights[height].z);
			Log("afR %d = 0 \n", c->heights[height].z);
			Log("acR %d = 0 \n", c->heights[height].z);
			Log("aS %d = 0 \n", c->heights[height].z);
			Log("aRes %d = 0 \n", c->heights[height].z);

			s->value[DEL_FOLIAGE] = 0;
			s->value[DEL_ROOTS_FINE_CTEM] = 0;
			s->value[DEL_ROOTS_COARSE_CTEM] = 0;
			s->value[DEL_STEMS]= 0;
			s->value[DEL_RESERVE]= 0;
			s->value[DEL_BB]= 0;

			Log("delta_F %d = 0 \n", c->heights[height].z);
			Log("delta_fR %d = 0 \n", c->heights[height].z);
			Log("delta_cR %d = 0 \n", c->heights[height].z);
			Log("delta_S %d = 0 \n", c->heights[height].z);
			Log("delta_Res %d = 0 \n", c->heights[height].z);

			Log("-Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);
			Log("-Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE]);
			Log("-Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
			Log("-Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
			Log("-Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);

			break;
			/**********************************************************************/
		}
	}
	c->daily_lai[i] = s->value[LAI];
	c->annual_delta_ws[i] += s->value[DEL_STEMS];
	c->annual_ws[i] = s->value[BIOMASS_STEM];
	c->annual_delta_wres[i] += s->value[DEL_RESERVE];
	c->annual_wres[i] = s->value[BIOMASS_RESERVE];
	c->annual_wf[i]= s->value[BIOMASS_FOLIAGE];
	c->annual_wbb[i]= s->value[BIOMASS_BRANCH];
	c->annual_wfr[i]= s->value[BIOMASS_ROOTS_FINE];
	c->annual_wcr[i]= s->value[BIOMASS_ROOTS_COARSE];

	if (c->leafLittering < 0.0) c->leafLittering = 0;

	Log("******************************\n");
}

/**/

