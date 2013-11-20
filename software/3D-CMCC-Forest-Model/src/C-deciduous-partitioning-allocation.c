

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



//VERSION CURRENTLY USED
//Deciduous carbon allocation routine
void D_Get_Partitioning_Allocation_CTEM (SPECIES *const s, CELL *const c, const MET_DATA *const met, int day, int month, int years, int DaysInMonth, int height, int age, int species)
{

	int i;

	//int phenology_phase;
	//allocation parameter. their sum must be = 1
	float  s0Ctem = s->value[S0CTEM];
	float  r0Ctem = s->value[R0CTEM];
	//float  f0Ctem = s->value[F0CTEM];
	float const omegaCtem = s->value[OMEGA_CTEM];
	float pS_CTEM = 0;
	float pR_CTEM = 0;
	float pF_CTEM = 0;
	//float max_DM_foliage;
	//float reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	float oldW;
	float Daily_solar_radiation;
	float Monthly_solar_radiation;
	float Light_trasm;
	float Par_over;
	float Perc_fine;
	float Perc_coarse;
	//float Perc_leaves;              //percentage of leaves in first growing season


	float Biomass_exceeding;


	//CTEM VERSION

	Log("\nGET_ALLOCATION_ROUTINE\n\n");
	Log("Carbon allocation routine for deciduous\n");


	//Log ("S0CTEM  = %g \n", s0Ctem);
	//Log ("R0CTEM  = %g \n", r0Ctem);
	//Log ("F0CTEM  = %g \n", f0Ctem );
	//Log ("OMEGA_CTEM  = %g \n", omegaCtem);
	//Log ("EPSILON_CTEM  = %g \n", epsilon);
	//Log ("KAPPA_CTEM  = %g \n", kappa);


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
		s->value[BIOMASS_CONES_CTEM] = 0;
	}

	if (s->counter[LEAF_FALL_COUNTER] == 1)
	{
		Log("First day of Leaf fall\n");
		s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE] = s->value[BIOMASS_FOLIAGE_CTEM] * s->value[FOLIAGE_REDUCTION_RATE];
		Log("Daily amount of foliage biomass to remove = %g\n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
	}

	//7 May 2012
	//compute static ratio of allocation between fine and coarse root
	//deriving data from values reported for BIOME-BGC
	//fixme see if change with new parameters checked in "Get_biome_fraction"
	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
	//Log("Fine/Coarse root ratio = %g\n", s->value[FR_CR] );
	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
	//Log("Percentage of fine root against total root= %g %%\n", Perc_fine * 100 );
	Perc_coarse = 1- Perc_fine;


	if (s->value[NPP] > 0)
	{
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
		//Log("PEAK_LAI = %g \n", s->value[PEAK_Y_LAI]);


		if (settings->spatial == 'u')
		{
			oldW = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM];

			//Log("Percentage of coarse root against total root= %g %%\n", Perc_coarse * 100 );

			//23 May 2012
			//percentage of leaves against fine roots
			//Perc_leaves =  1 - (s->value[FINE_ROOT_LEAF]  / (s->value[FINE_ROOT_LEAF] + 1));
			//Log("Percentage of leaves against fine roots = %g %%\n", Perc_leaves * 100);
			//Log("Percentage of fine roots against leaves = %g %%\n", (1 - Perc_leaves) * 100);

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
				//float years_from_coppicing = 25;

				float r0Ctem_increment;
				float old_r0Ctem = r0Ctem;
				float s0Ctem_increment;
				float old_s0Ctem = s0Ctem;

				Log("min r0 ctem = %g\n",s->value[MIN_R0CTEM] );
				Log("max s0 ctem = %g\n",s->value[MAX_S0CTEM] );
				Log("years for conversion = %g\n",s->value[YEARS_FOR_CONVERSION] );


				//considering a LINEAR increment
				//allocation ratio to roots
				r0Ctem -= s->value[MIN_R0CTEM];
				r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
				r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
				Log ("new r0_CTEM = %g \n", r0Ctem);

				if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
				{
					Log("ERROR IN r0Ctem !!! \n");
				}


				//considering a LINEAR decrement
				//allocation ratio to stem
				s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
				s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
				s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
				Log ("new s0_CTEM = %g \n", s0Ctem);

				if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)
				{
					Log("ERROR IN s0Ctem !!! \n");
				}

			}

			Log("PHENOLOGICAL PHASE = %d\n", s->phenology_phase);

			switch (s->phenology_phase)
			{
			Log("LAI = %g \n", s->value[LAI]);
			/************************************************************/
			case 1:
				Log("(LAI < PEAK_Y_LAI * 0.5) \n");
				Log("LAI = %g \n", s->value[LAI]);
				Log("**Maximum Growth**\n");
				Log("Bud burst phase allocating only into foliage and fine root pools\n");


				//fixme scegliere se usare Magnani o meno
				//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
				//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio

				/*
				s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * (1.0 - s->value[STEM_LEAF_FRAC]);
				s->value[DEL_STEMS_CTEM] = (s->value[NPP] - s->value[DEL_FOLIAGE_CTEM]);


				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("Biomass Foliage CTEM = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);
				Log("Reserve Biomass = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Biomass Stem CTEM = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM] );
				*/
				/*allocating into fine root and foliage*/
				s->value[DEL_FOLIAGE_CTEM] += (s->value[NPP] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
				Log("Biomass NPP allocated to foliage pool = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);


				s->value[DEL_ROOTS_FINE_CTEM] += (s->value[NPP] * s->value[FINE_ROOT_LEAF_FRAC]);
				Log("Biomass NPP allocated to fine root pool = %g\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);



				//recompute LAI
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}
				Log("++Lai = %g\n", s->value[LAI]);

				//s->value[DEL_STEMS_CTEM] = 0;
				s->value[DEL_STEMS_CTEM] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT_CTEM] = 0;
				s->value[DEL_RESERVE_CTEM] = 0;
				s->value[DEL_BB] = 0;

				//control if new Lai exceeds Peak Lai
				if (s->value[LAI] > s->value[PEAK_Y_LAI])
				{
					Log("PHENOLOGICAL PHASE = 1.1 \n (TOO MUCH BIOMASS INTO FOLIAGE)\n");
					Log("LAI = %g \n", s->value[LAI]);


					Log("Re-allocating foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n");

					Biomass_exceeding = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MAX_BIOMASS_FOLIAGE_CTEM];
					Log("Foliar Biomass exceeding = %g tDM \n", Biomass_exceeding);
					Log("Biomass Foliage = %g tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);
					s->value[BIOMASS_FOLIAGE_CTEM] = s->value[MAX_BIOMASS_FOLIAGE_CTEM];
					Log("Max Biomass Foliage = %g tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]);
					Log("Biomass Foliage = %g tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);


					pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
					Log("Roots CTEM ratio = %g %%\n",  pR_CTEM * 100);
					pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
					Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);

					//reserve ratio
					pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
					Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);


					// Biomass allocation

					s->value[DEL_ROOTS_TOT_CTEM] = Biomass_exceeding * pR_CTEM;
					Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


					//7 May 2012
					//compute fine and coarse root biomass increment
					s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_fine;
					Log("BiomassRoots into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
					s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
					Log("BiomassRoots into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


					s->value[DEL_STEMS_CTEM] = Biomass_exceeding *  pS_CTEM;
					Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

					s->value[DEL_RESERVE_CTEM] = Biomass_exceeding * pF_CTEM;
					Log("BiomassReserve increment CTEM = %g tDM/area\n", s->value[DEL_RESERVE_CTEM] );

					//Total Stem Biomass
					//remove the part allocated to the branch and bark
					s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
					Log("BiomassStemBB increment CTEM = %g tDM/area\n", s->value[DEL_BB]);
					//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
					//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

					//allocation to stem
					s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
					s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
					Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

					//allocation to branch and bark
					s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
					Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

					//allocation to non structural reserve pool
					s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
					Log("Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

					//allocation to roots
					s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
					Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

					s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
					Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
					s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
					Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

					//check for live and dead tissues
					/*stem*/
					s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
					Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
					//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
					Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
					/*coarse root*/
					s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
					Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
					s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
					Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
					/*branch and bark*/
					s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
					Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
					s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
					Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

					s->value[DEL_FOLIAGE_CTEM] = 0;

					Log("LAI setted to Peak Lai = %g \n", s->value[PEAK_Y_LAI]);
					s->value[LAI] = s->value[PEAK_Y_LAI];

					// Total Biomass Increment
					s->value[DEL_TOTAL_W] = s->value[DEL_FOLIAGE_CTEM] + s->value[DEL_ROOTS_TOT_CTEM] + s->value[DEL_STEMS_CTEM];
					Log("Increment Monthly Total Biomass  (delTotalW) = %g tDM/area\n", s->value[DEL_TOTAL_W]);


					// Total Biomass
					s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_TOT_CTEM];
					Log("Previous Total W = %g tDM/area\n", oldW);
					Log("Total Biomass = %g tDM/area\n", s->value[TOTAL_W]);


					s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
					s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
					s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
					s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
					s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
					s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
					s->value[DEL_Y_BB] += s->value[DEL_BB];
				}
				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];

				break;
				/************************************************************/
			case 2:
				Log("**Normal Growth**\n");
				Log("Day length > %g \n", /*c->abscission_daylength*/s->value[MINDAYLENGTH] );
				Log("(LAI MAX * 0.5 < LAI < LAI MAX)\n");
				Log("allocating into the three pools Ws+Wr+Wf\n");


				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Roots CTEM ratio layer %d = %g %%\n", z, pR_CTEM * 100);


				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);



				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				//Log("Foliage CTEM ratio = %g %%\n", pF_CTEM * 100);

				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
					s->value[NPP] -= s->value[DEL_CONES_CTEM];
					s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
					Log("Biomass increment into cones = %g tDM/area\n", s->value[DEL_CONES_CTEM]);

					//reproductive life span
					s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
				}


				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
				Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);

				//7 May 2012
				//comnpute fine and coarse root biomass
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
				Log("BiomassRoots increment into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  - s->value[DEL_ROOTS_FINE_CTEM] ;
				Log("BiomassRoots increment into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

				if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT_CTEM])
				{
					Log("ERROR IN ROOTS ALLOCATION del coarse + del fine = %g, del tot = %g \n", (s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]), s->value[DEL_ROOTS_TOT_CTEM]);
				}


				s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
				Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);


				s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * pF_CTEM;
				Log("BiomassFoliage increment CTEM = %g tDM/area\n", s->value[DEL_FOLIAGE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %g tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to foliage
				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);



				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				Log("Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//check for live and dead tissues
				/*stem*/
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				/*coarse root*/
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				/*branch and bark*/
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


				//recompute LAI
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}


				s->value[DEL_RESERVE_CTEM] = 0;


				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				Log("aF %d = %g \n", c->heights[height].z, pF_CTEM);
				Log("afR %d = %g \n", c->heights[height].z, Perc_fine * pR_CTEM);
				Log("acR %d = %g \n", c->heights[height].z, Perc_coarse * pR_CTEM);
				Log("aS %d = %g \n", c->heights[height].z, pS_CTEM);
				Log("aRes %d = 0 \n", c->heights[height].z);

				Log("delta_F %d = %g \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
				Log("delta_fR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
				Log("delta_cR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
				Log("delta_S %d = %g \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
				Log("delta_Res %d = %g \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
				Log("delta_BB %d = %g \n", c->heights[height].z, s->value[DEL_BB]);

				//control if new Lai exceeds Peak Lai
				if (s->value[LAI] > s->value[PEAK_Y_LAI])
				{
					Log("PHENOLOGICAL PHASE = 2.1 \n (TOO MUCH BIOMASS INTO FOLIAGE)\n");
					Log("LAI = %g \n", s->value[LAI]);


					Log("Re-allocating foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n");

					Biomass_exceeding = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MAX_BIOMASS_FOLIAGE_CTEM];
					Log("Foliar Biomass exceeding = %g tDM \n", Biomass_exceeding);
					Log("Biomass Foliage = %g tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);
					s->value[BIOMASS_FOLIAGE_CTEM] = s->value[MAX_BIOMASS_FOLIAGE_CTEM];
					Log("Max Biomass Foliage = %g tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]);
					Log("Biomass Foliage = %g tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);


					pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
					Log("Roots CTEM ratio layer = %g %%\n", pR_CTEM * 100);
					pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
					Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);

					//reserve ratio
					pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
					Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);


					// Biomass allocation

					s->value[DEL_ROOTS_TOT_CTEM] = Biomass_exceeding * pR_CTEM;
					Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


					//7 May 2012
					//comnpute fine and coarse root biomass
					s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
					Log("BiomassRoots into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
					s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
					Log("BiomassRoots into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


					s->value[DEL_STEMS_CTEM] = Biomass_exceeding *  pS_CTEM;
					Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

					s->value[DEL_RESERVE_CTEM] = Biomass_exceeding * pF_CTEM;
					Log("BiomassReserve increment CTEM = %g tDM/area\n", s->value[DEL_RESERVE_CTEM] );

					//Total Stem Biomass
					//remove the part allocated to the branch and bark
					s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
					//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
					//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

					//allocation to stem
					s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
					s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
					Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

					//allocation to branch and bark
					s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
					Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

					//allocation to non structural reserve pool
					s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
					Log("Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

					//allocation to roots
					s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
					Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

					s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
					Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
					s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
					Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

					//check for live and dead tissues
					/*stem*/
					s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
					Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
					//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
					Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
					/*coarse root*/
					s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
					Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
					s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
					Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
					/*branch and bark*/
					s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
					Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
					s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
					Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

					s->value[DEL_FOLIAGE_CTEM] = 0;

					Log("LAI setted to Peak Lai= %g \n", s->value[LAI]);
					s->value[LAI] = s->value[PEAK_Y_LAI];

					// Total Biomass Increment
					s->value[DEL_TOTAL_W] = s->value[DEL_FOLIAGE_CTEM] + s->value[DEL_ROOTS_TOT_CTEM] + s->value[DEL_STEMS_CTEM];
					Log("Increment Monthly Total Biomass  (delTotalW) = %g tDM/area\n", s->value[DEL_TOTAL_W]);


					// Total Biomass
					s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_TOT_CTEM];
					Log("Previous Total W = %g tDM/area\n", oldW);
					Log("Total Biomass = %g tDM/area\n", s->value[TOTAL_W]);


					s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
					s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
					s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
					s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
					s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
					s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
					s->value[DEL_Y_BB] += s->value[DEL_BB];
				}
				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];

				break;
				/************************************************************************/
			case 3:
				Log("(LAI == PEAK LAI)\n");
				Log("allocating into the three pools Ws+Wr+Wreserve\n");
				/*see Barbaroux et al., 2002, Scartazza et al., 2013*/


				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Roots CTEM ratio layer = %g %%\n", pR_CTEM * 100);
				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);
				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);

				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[BIOMASS_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
					s->value[NPP] -= s->value[BIOMASS_CONES_CTEM];
					Log("Biomass increment into cones = %g tDM/area\n", s->value[BIOMASS_CONES_CTEM]);

					//reproductive life span
					s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
				}

				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
				Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


				//7 May 2012
				//compute fine and coarse root biomass
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
				Log("BiomassRoots into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
				Log("BiomassRoots into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


				s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
				Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

				s->value[DEL_RESERVE_CTEM] = s->value[NPP] * pF_CTEM;
				Log("BiomassReserve increment CTEM = %g tDM/area\n", s->value[DEL_RESERVE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %g tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to non structural reserve pool
				s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
				Log("Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				//check for live and dead tissues
				/*stem*/
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				/*coarse root*/
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				/*branch and bark*/
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


				Log("Foliage Biomass (Wf) = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);

				s->value[DEL_FOLIAGE_CTEM] = 0;

				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				Log("aF %d = 0 \n", c->heights[height].z);
				Log("afR %d = %g \n", c->heights[height].z, Perc_fine * pR_CTEM);
				Log("acR %d = %g \n", c->heights[height].z, Perc_coarse * pR_CTEM);
				Log("aS %d = %g \n", c->heights[height].z, pS_CTEM);
				Log("aRes %d = %g \n", c->heights[height].z, pF_CTEM);

				Log("delta_F %d = %g \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
				Log("delta_fR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
				Log("delta_cR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
				Log("delta_S %d = %g \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
				Log("delta_Res %d = %g \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
				Log("delta_BB %d = %g \n", c->heights[height].z, s->value[DEL_BB]);

				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];


				break;
				/**********************************************************************/
			case 0:
				Log("(DayLength < MINDAYLENGTH)\n");
				Log("LEAF FALL\n");
				//Log("allocating into the three pools Ws+Wr+Wreserve \nwith leaf fall\n");
				Log("allocating into W reserve pool\n");

				//FIXME ALLOCATE ALL BIOMASS INTO ONLY RESERVE POOL see also Scartazza et al., 2013

				//leaf fall counter to compute in the first day of leaf fall the amount of biomass to remove to
				//have a linear decrease of foliage biomass and then LAI values

				/*
				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Roots CTEM ratio layer = %g %%\n", pR_CTEM * 100);
				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);
				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);
				 */
				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[BIOMASS_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
					s->value[NPP] -= s->value[BIOMASS_CONES_CTEM];
					Log("Biomass increment into cones = %g tDM/area\n", s->value[BIOMASS_CONES_CTEM]);

					//reproductive life span
					s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
				}
				s->value[DEL_RESERVE_CTEM]= s->value[NPP];
				s->value[BIOMASS_RESERVE_CTEM] += s->value[DEL_RESERVE_CTEM];

				/*
				// Biomass allocation


				s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
				Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


				//7 May 2012
				//compute fine and coarse root biomass
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_fine;
				Log("BiomassRoots into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
				Log("BiomassRoots into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


				s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
				Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

				s->value[DEL_RESERVE_CTEM] = s->value[NPP] * pF_CTEM;
				Log("BiomassReserve increment CTEM = %g tDM/area\n", s->value[DEL_RESERVE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %g tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to non structural reserve pool
				s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
				Log("Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				 */
				//check for live and dead tissues
				/*stem*/
				//s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
				//Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
				//Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				/*coarse root*/
				//s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
				//Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				//s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
				//Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				/*branch and bark*/
				//s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] * s->value[LIVE_TOTAL_WOOD_FRAC]);
				//Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
				//Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


				Log("Foliage Biomass (Wf) = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);

				s->value[DEL_FOLIAGE_CTEM] = 0;
				/*
				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				Log("aF %d = 0 \n", c->heights[height].z);
				Log("afR %d = %g \n", c->heights[height].z, Perc_fine * pR_CTEM);
				Log("acR %d = %g \n", c->heights[height].z, Perc_coarse * pR_CTEM);
				Log("aS %d = %g \n", c->heights[height].z, pS_CTEM);
				Log("aRes %d = %g \n", c->heights[height].z, pF_CTEM);

				Log("delta_F %d = %g \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
				Log("delta_fR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
				Log("delta_cR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
				Log("delta_S %d = %g \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
				Log("delta_Res %d = %g \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
				Log("delta_BB %d = %g \n", c->heights[height].z, s->value[DEL_BB]);
				 */

				Log("***LEAF FALL**\n");
				//COMPUTE LITTERFALL using BIOME_BGC approach
				Log("++Lai before Leaf fall= %g\n", s->value[LAI]);
				Log("Biomass foliage = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
				Log("foliage reduction rate %g \n", s->value[FOLIAGE_REDUCTION_RATE]);
				Log("biomass foliage to remove %g \n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
				s->value[BIOMASS_FOLIAGE_CTEM] -= s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE];;
				Log("Biomass foliage = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);


				//recompute LAI
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}
				Log("++Lai = %g\n", s->value[LAI]);

				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];


				break;

			case 4:

				Log("Unvegetative period \n");

				Log("aF %d = 0 \n", c->heights[height].z);
				Log("afR %d = 0 \n", c->heights[height].z);
				Log("acR %d = 0 \n", c->heights[height].z);
				Log("aS %d = 0 \n", c->heights[height].z);
				Log("aRes %d = 0 \n", c->heights[height].z);

				s->value[DEL_FOLIAGE_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_STEMS_CTEM]= 0;
				s->value[DEL_RESERVE_CTEM]= 0;
				s->value[DEL_BB]= 0;

				Log("delta_F %d = 0 \n", c->heights[height].z);
				Log("delta_fR %d = 0 \n", c->heights[height].z);
				Log("delta_cR %d = 0 \n", c->heights[height].z);
				Log("delta_S %d = 0 \n", c->heights[height].z);
				Log("delta_Res %d = 0 \n", c->heights[height].z);

				Log("-Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);
				Log("-Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);
				Log("-Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
				Log("-Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				Log("-Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				Log("++Lai = %g\n", s->value[LAI]);

				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];

				break;
			}

		}
		if (settings->spatial == 's')
		{
			Log("Spatial version \n");




			oldW = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM];

			//7 May 2012
			//compute static ratio of allocation between fine and coarse root
			//deriving data from values reported for BIOME-BGC
			s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
			//Log("Fine/Coarse root ratio = %g\n", s->value[FR_CR] );
			Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
			//Log("Percentage of fine root against total root= %g %%\n", Perc_fine * 100 );
			Perc_coarse = 1- Perc_fine;
			//Log("Percentage of coarse root against total root= %g %%\n", Perc_coarse * 100 );

			//23 May 2012
			//percentage of leaves against fine roots
			//Perc_leaves =  1 - (s->value[FINE_ROOT_LEAF]  / (s->value[FINE_ROOT_LEAF] + 1));
			//Log("Percentage of leaves against fine roots = %g %%\n", Perc_leaves * 100);
			//Log("Percentage of fine roots against leaves = %g %%\n", (1 - Perc_leaves) * 100);



			//(Arora V. K., Boer G. J., GCB, 2005)

			Log("(CTEM) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", c->heights[height].z);
			Log("PEAK_LAI = %g \n", s->value[PEAK_Y_LAI]);

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
				//float years_from_coppicing = 25;

				float r0Ctem_increment;
				float old_r0Ctem = r0Ctem;
				float s0Ctem_increment;
				float old_s0Ctem = s0Ctem;

				Log("min r0 ctem = %g\n",s->value[MIN_R0CTEM] );
				Log("max s0 ctem = %g\n",s->value[MAX_S0CTEM] );
				Log("years for conversion = %g\n",s->value[YEARS_FOR_CONVERSION] );


				//considering a LINEAR increment
				//allocation ratio to roots
				r0Ctem -= s->value[MIN_R0CTEM];
				r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
				r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
				Log ("new r0_CTEM = %g \n", r0Ctem);

				if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
				{
					Log("ERROR IN r0Ctem !!! \n");
				}


				//considering a LINEAR decrement
				//allocation ratio to stem
				s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
				s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
				s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
				Log ("new s0_CTEM = %g \n", s0Ctem);

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
				Log("NDVI-LAI = %g \n", met[month].ndvi_lai);
			}
			else
			{
				Log("NDVI-LAI = %g \n", met[month].d[day].ndvi_lai);
			}
			/************************************************************/
			case 1:
				Log("(NDVI-LAI < PEAK_Y_LAI * 0.5)\n");
				if (settings->time == 'm')
				{
					Log("NDVI-LAI = %g \n", met[month].ndvi_lai);
				}
				else
				{
					Log("NDVI-LAI = %g \n", met[month].d[day].ndvi_lai);
				}
				Log("**Maximum Growth**\n");
				Log("allocating only into foliage pools\n");

				s->value[DEL_FOLIAGE_CTEM] = s->value[NPP];
				Log("DEL FOLIAGE = %g\n", s->value[DEL_FOLIAGE_CTEM]);
				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("BiomassFoliage CTEM = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM] );

				//recompute LAI
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}
				Log("++Lai = %g\n", s->value[LAI]);

				s->value[DEL_STEMS_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_ROOTS_TOT_CTEM] = 0;
				s->value[DEL_RESERVE_CTEM] = 0;
				s->value[DEL_BB] = 0;

				break;
				/************************************************************/
			case 2:
				Log("**Normal Growth**\n");
				Log("(Day length > %g)\n", /*c->abscission_daylength*/ s->value[MINDAYLENGTH] );
				Log("(LAI MAX * 0.5 < NDVI-LAI < LAI MAX)\n");
				Log("allocating into the three pools Ws+Wr+Wf\n");


				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Roots CTEM ratio layer %d = %g %%\n", z, pR_CTEM * 100);


				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);



				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				//Log("Foliage CTEM ratio = %g %%\n", pF_CTEM * 100);

				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[BIOMASS_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
					s->value[NPP] -= s->value[BIOMASS_CONES_CTEM];
					Log("Biomass increment into cones = %g tDM/area\n", s->value[BIOMASS_CONES_CTEM]);

					//reproductive life span
					s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
				}


				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
				Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);

				//7 May 2012
				//comnpute fine and coarse root biomass
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
				Log("BiomassRoots increment into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
				Log("BiomassRoots increment into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

				if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT_CTEM])
				{
					Log("ERROR IN ROOTS ALLOCATION del coarse + del fine = %g, del tot = %g \n", (s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]), s->value[DEL_ROOTS_TOT_CTEM]);
				}


				s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
				Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);


				s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * pF_CTEM;
				Log("BiomassFoliage increment CTEM = %g tDM/area\n", s->value[DEL_FOLIAGE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %g tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to foliage
				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);

				//recompute LAI
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}



				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				//check for live and dead tissues
				/*stem*/
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				/*coarse root*/
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				/*branch and bark*/
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

				s->value[DEL_RESERVE_CTEM] = 0;


				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				Log("aF %d = %g \n", c->heights[height].z, pF_CTEM);
				Log("afR %d = %g \n", c->heights[height].z, Perc_fine * pR_CTEM);
				Log("acR %d = %g \n", c->heights[height].z, Perc_coarse * pR_CTEM);
				Log("aS %d = %g \n", c->heights[height].z, pS_CTEM);
				Log("aRes %d = 0 \n", c->heights[height].z);

				Log("delta_F %d = %g \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
				Log("delta_fR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
				Log("delta_cR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
				Log("delta_S %d = %g \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
				Log("delta_Res %d = %g \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
				Log("delta_BB %d = %g \n", c->heights[height].z, s->value[DEL_BB]);


				break;
				/************************************************************************/
			case 3:
				Log("(NDVI-LAI == PEAK LAI or Month > 6)\n");
				Log("allocating into the three pools Ws+Wr+Wreserve\n");

				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Roots CTEM ratio layer %d = %g %%\n", z, pR_CTEM * 100);
				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);
				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				//Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);


				//REPRODUCTION ONLY FOR NEEDLE LEAF
				if (s->value[PHENOLOGY] == 0.2)
				{
					//NPP for reproduction
					s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
					s->value[NPP] -= s->value[DEL_CONES_CTEM];
					s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
					Log("Biomass increment into cones = %g tDM/area\n", s->value[DEL_CONES_CTEM]);

					//reproductive life span
					//s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
				}


				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
				Log("BiomassRoots increment CTEM = %g tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


				//7 May 2012
				//compute fine and coarse root biomass
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
				Log("BiomassRoots into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
				Log("BiomassRoots into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


				s->value[DEL_STEMS_CTEM] = s->value[NPP] * pS_CTEM;
				Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

				s->value[DEL_RESERVE_CTEM] = s->value[NPP] * pF_CTEM;
				Log("BiomassReserve increment CTEM = %g tDM/area\n", s->value[DEL_RESERVE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %g tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to non structural reserve pool
				s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
				Log("Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				//check for live and dead tissues
				/*stem*/
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				/*coarse root*/
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				/*branch and bark*/
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
				Log("Dead Stem Branch Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);


				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				Log("aF %d = 0 \n", c->heights[height].z);
				Log("afR %d = %g \n", c->heights[height].z, Perc_fine * pR_CTEM);
				Log("acR %d = %g \n", c->heights[height].z, Perc_coarse * pR_CTEM);
				Log("aS %d = %g \n", c->heights[height].z, pS_CTEM);
				Log("aRes %d = %g \n", c->heights[height].z, pF_CTEM);

				Log("delta_F %d = %g \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
				Log("delta_fR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
				Log("delta_cR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
				Log("delta_S %d = %g \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
				Log("delta_Res %d = %g \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
				Log("delta_BB %d = %g \n", c->heights[height].z, s->value[DEL_BB]);


				break;

			case 4:

				Log("Unvegetative period \n");

				Log("aF %d = 0 \n", c->heights[height].z);
				Log("afR %d = 0 \n", c->heights[height].z);
				Log("acR %d = 0 \n", c->heights[height].z);
				Log("aS %d = 0 \n", c->heights[height].z);
				Log("aRes %d = 0 \n", c->heights[height].z);

				s->value[DEL_FOLIAGE_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
				s->value[DEL_STEMS_CTEM]= 0;
				s->value[DEL_RESERVE_CTEM]= 0;
				s->value[DEL_BB]= 0;

				Log("delta_F %d = 0 \n", c->heights[height].z);
				Log("delta_fR %d = 0 \n", c->heights[height].z);
				Log("delta_cR %d = 0 \n", c->heights[height].z);
				Log("delta_S %d = 0 \n", c->heights[height].z);
				Log("delta_Res %d = 0 \n", c->heights[height].z);

				Log("-Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);
				Log("-Reserve Biomass (Wres) = %g tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);
				Log("-Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);
				Log("-Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				Log("-Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				break;
				/**********************************************************************/
			}
		}
		/*budburst phase*/
		if (s->counter[VEG_DAYS] <= s->counter[BUD_BURST_COUNTER] && s->value[LAI] < s->value[PEAK_Y_LAI] && s->value[BIOMASS_RESERVE_CTEM] > 0)
		{
			s->value[DEL_ROOTS_COARSE_CTEM] = 0;
			s->value[DEL_STEMS_CTEM]= 0;
			s->value[DEL_BB]= 0;
		}
	}
	else
	{

		/*budburst phase*/
		if (s->counter[VEG_DAYS] <= s->counter[BUD_BURST_COUNTER] && s->value[LAI] < s->value[PEAK_Y_LAI] && s->value[BIOMASS_RESERVE_CTEM] > 0)
		{
			s->value[DEL_ROOTS_COARSE_CTEM] = 0;
			s->value[DEL_STEMS_CTEM]= 0;
			s->value[DEL_BB]= 0;
		}
		else
		{
			s->value[DEL_FOLIAGE_CTEM] = 0;
			s->value[DEL_ROOTS_FINE_CTEM] = 0;
			s->value[DEL_ROOTS_COARSE_CTEM] = 0;
			s->value[DEL_STEMS_CTEM]= 0;
			s->value[DEL_RESERVE_CTEM]= -((s->value[TOTAL_AUT_RESP] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);;
			s->value[DEL_BB]= 0;
		}
		if (s->counter[VEG_UNVEG] == 0)
		{
			s->value[DEL_RESERVE_CTEM] = -((s->value[TOTAL_AUT_RESP] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell);
		}
		else
		{
			if (s->value[GPP_g_C] > 0.0)
			{
				s->value[DEL_RESERVE_CTEM] = -((s->value[TOTAL_AUT_RESP] * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell) + s->value[GPP_g_C];
			}
		}
		Log("delta_F %d = %g \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
		Log("delta_fR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
		Log("delta_cR %d = %g \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
		Log("delta_S %d = %g \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
		Log("delta_Res %d = %g \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
		Log("delta_BB %d = %g \n", c->heights[height].z, s->value[DEL_BB]);
		c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
		c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
		c->daily_delta_wbb[i] = s->value[DEL_BB];
		c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
		c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
		c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];
	}


	i = c->heights[height].z;




	c->daily_lai[i] = s->value[LAI];
	c->annual_delta_ws[i] += s->value[DEL_STEMS_CTEM];
	c->annual_ws[i] = s->value[BIOMASS_STEM_CTEM];
	c->annual_delta_wres[i] += s->value[DEL_RESERVE_CTEM];
	c->annual_wres[i] = s->value[BIOMASS_RESERVE_CTEM];
	Log("******************************\n");
}

/**/
