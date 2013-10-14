/*partitionign_allocation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

/**/


//VERSION CURRENTLY USED
//Deciduous carbon allocation routine
void D_Get_Partitioning_Allocation_CTEM (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, int years, int height, int age, int species)
{
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


	Log("Cazzo %g\n", met[month].d[day].solar_rad);

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
				Log("allocating only into foliage and stem pools\n");


				//fixme scegliere se usare Magnani o meno
				//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
				//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
				/*
				s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * (1.0 / s->value[STEM_LEAF]);
				s->value[DEL_STEMS_CTEM] = (s->value[NPP]-s->value[DEL_FOLIAGE_CTEM]);
				 */

				s->value[DEL_FOLIAGE_CTEM] = s->value[NPP];

				//old version
				//s->value[DEL_FOLIAGE_CTEM] = s->value[NPP];

				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("Biomass Foliage CTEM = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM] );

				//s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				//Log("Biomass Stem CTEM = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM] );

				//recompute LAI
				//todo LAI is computed from biomass in DM while SLA is in C!!! probably SLA has to be converted into DM multiplying it per 2
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				Log("++Lai = %g\n", s->value[LAI]);

				//s->value[DEL_STEMS_CTEM] = 0;
				s->value[DEL_ROOTS_FINE_CTEM] = 0;
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
					//compute fine and coarse root biomass
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
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

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


				//recompute LAI
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * 2.0);
				Log("day = %d month %d ++Lai = %g\n", day, month, s->value[LAI]);


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
					//Log("Roots CTEM ratio layer %d = %g %%\n", z, pR_CTEM * 100);
					pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
					//Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);

					//reserve ratio
					pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
					//Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);


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

				break;
				/************************************************************************/
			case 3:
				Log("(LAI == PEAK LAI)\n");
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
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
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




				break;
				/**********************************************************************/
			case 0:
				Log("(DayLength < MINDAYLENGTH Abscission DayLength)\n");
				Log("allocating into the three pools Ws+Wr+Wreserve \nwith leaf fall\n");

				//leaf fall counter to compute in the first day of leaf fall the amount of biomass to remove to
				//have a linear decrease of foliage biomass and then LAI values

				if (s->counter[LEAF_FALL_COUNTER] == 1)
				{
					s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE] = s->value[BIOMASS_FOLIAGE_CTEM] * s->value[FOLIAGE_REDUCTION_RATE];
					Log("Daily amount of foliage biomass to remove = %g\n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
				}


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
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
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


				Log("***LEAF FALL**\n");
				//COMPUTE LITTERFALL using BIOME_BGC approach

				/*
					//todo check if move it into structure.c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					if (settings->time == 'm')
					{
						//compute months of leaf fall taking an integer value
						s->value[MONTH_FRAC_FOLIAGE_REMOVE] =  ( s->value[LEAF_FALL_FRAC_GROWING]  * s->counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						Log("Months of leaf fall for deciduous = %g \n", s->value[MONTH_FRAC_FOLIAGE_REMOVE]);
						//monthly rate of foliage reduction

						//currently the model considers a linear reduction in leaf fall
						//it should be a negative sigmoid function
						//todo: create a sigmoid function
						foliage_reduction_rate = 1.0 / (s->value[MONTH_FRAC_FOLIAGE_REMOVE] + 1);
						Log("foliage reduction rate = %g \n", foliage_reduction_rate);
						s->value[BIOMASS_FOLIAGE_CTEM] *= (1.0 - foliage_reduction_rate);
						Log("Biomass foliage = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
					}
					else
					{
						s->value[DAY_FRAC_FOLIAGE_REMOVE] =  ( s->value[LEAF_FALL_FRAC_GROWING]  * s->counter[DAY_VEG_FOR_LITTERFALL_RATE]);
						Log("Days of leaf fall for deciduous = %g \n", s->value[DAY_FRAC_FOLIAGE_REMOVE]);
						//monthly rate of foliage reduction

						//currently the model considers a linear reduction in leaf fall
						//it should be a negative sigmoid function
						//todo: create a sigmoid function
						foliage_reduction_rate = 1.0 / (s->value[FRAC_DAY_FOLIAGE_REMOVE] + 1);
						Log("foliage reduction rate = %g \n", foliage_reduction_rate);

					}
				 */
				Log("++Lai before Leaf fall= %g\n", s->value[LAI]);
				Log("Biomass foliage = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);
				Log("foliage reduction rate %g \n", s->value[FOLIAGE_REDUCTION_RATE]);
				Log("biomass foliage to remove %g \n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
				s->value[BIOMASS_FOLIAGE_CTEM] -= s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE];;
				Log("Biomass foliage = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);


				//recompute LAI
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * 2);
				Log("++Lai = %g\n", s->value[LAI]);


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
				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("BiomassFoliage CTEM = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM] );

				//recompute LAI
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * s->value[SLAmkg];
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
				//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to foliage
				s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
				Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);

				//recompute LAI
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * s->value[SLAmkg];
				Log("++Lai = %g\n", s->value[LAI]);



				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %g tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

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


				s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
				Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

				s->value[DEL_RESERVE_CTEM] = s->value[NPP] * pF_CTEM;
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

		//for daily_Log file only if there's one class
		/*
	if (c->heights_count -1  == 0 && c->heights[height].ages_count -1 == 0 && c->heights[height].ages[age].species_count -1 == 0)
	{
		c->daily_lai = s->value[LAI];
	}
		 */
	}
	else
	{
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
	}
	if (c->annual_layer_number == 1)
	{
		c->daily_lai[0] = s->value[LAI];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->daily_lai[1] = s->value[LAI];
		}
		else
		{
			c->daily_lai[0] = s->value[LAI];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->daily_lai[2] = s->value[LAI];
		}
		if (c->heights[height].z == 1)
		{
			c->daily_lai[1] = s->value[LAI];
		}
		if (c->heights[height].z == 0)
		{
			c->daily_lai[0] = s->value[LAI];
		}
	}

	/*compute annual stem increment*/
	if (c->annual_layer_number == 1)
	{
		c->annual_delta_ws[0] += s->value[DEL_STEMS_CTEM];
		c->annual_ws[0] = s->value[BIOMASS_STEM_CTEM];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->annual_delta_ws[1] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[1] = s->value[BIOMASS_STEM_CTEM];
		}
		else
		{
			c->annual_delta_ws[0] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[0] = s->value[BIOMASS_STEM_CTEM];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->annual_delta_ws[2] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[2] = s->value[BIOMASS_STEM_CTEM];
		}
		if (c->heights[height].z == 1)
		{
			c->annual_delta_ws[1] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[1] = s->value[BIOMASS_STEM_CTEM];
		}
		if (c->heights[height].z == 0)
		{
			c->annual_delta_ws[0] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[0] = s->value[BIOMASS_STEM_CTEM];
		}
	}


	Log("******************************\n");
}

/**/

//VERSION CURRENTLY USED
//Evergreen carbon allocation routine
void E_Get_Partitioning_Allocation_CTEM (SPECIES *const s, AGE * const a, CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, int years, int height, int age)
{
	//CTEM VERSION

	Log("GET_ALLOCATION_ROUTINE\n\n");

	Log("Carbon allocation routine for evergreen\n");
	Log("Version = %c \n", settings->version);
	Log("Spatial = %c \n", settings->spatial);




	//allocation parameter. their sum must be = 1
	float  s0Ctem = s->value[S0CTEM];
	float  r0Ctem = s->value[R0CTEM];
	//float  f0Ctem = s->value[F0CTEM];
	float const omegaCtem = s->value[OMEGA_CTEM];

	//determines the maximum amount of drymatter to allocate to foliage
	//CURRENTLY NOT USED
	//float const epsilon = s->value[EPSILON_CTEM];      //in KgC/m^2)^-0.6  allocation parameter see also Frankfurt biosphere model
	//float const kappa = s->value[KAPPA];               //(dimensionless) allocation parameter

	float pS_CTEM;
	float pR_CTEM;
	float pF_CTEM;
	//float max_DM_foliage;
	//float reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR

	float Daily_solar_radiation;
	float Monthly_solar_radiation;
	float Light_trasm;
	float Par_over;

	float Perc_fine;
	float Perc_coarse;
	//float Perc_leaves;              //percentage of leaves in first growing season
	float oldW;
	float gammaF;
	float oldWf;


	if(s->value[NPP] > 0)
	{

		if (settings->time == 'm')
		{
			Monthly_solar_radiation = met[month].solar_rad * MOLPAR_MJ * DaysInMonth;
			Par_over = c->par - s->value[APAR];
			Light_trasm = Par_over / Monthly_solar_radiation;
		}
		else
		{
			Daily_solar_radiation = met[month].d[day].solar_rad * MOLPAR_MJ;
			Par_over = c->par - s->value[APAR];
			Light_trasm = Par_over /Daily_solar_radiation;
		}

		s->value[FRACBB] = s->value[FRACBB1] + (s->value[FRACBB0] - s->value[FRACBB1]) * exp(-ln2 * (c->heights[height].ages[age].value / s->value[TBB]));
		//Log("fracBB = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]);

		//I could try to get in instead F_SW the minimum value between F_SW and F_VPD  2 apr 2012
		//reductor = Minimum (s->value[F_SW], s->value[F_VPD]);
		//I could try to get in instead F_SW the minimum value between F_SW and F_NUTR  18 apr 2012
		//reductor = Minimum (s->value[F_SW], s->value[F_NUTR]);
		//reductor = s->value[F_SW];
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

		if (day == 0 && month == 0 && years == 0)
		{
			s->value[BIOMASS_CONES_CTEM] = 0;
		}



		if (settings->spatial == 'u')
		{
			Log("Unspatial version \n");

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



			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Roots CTEM ratio  = %g %%\n", pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);

			//foliage ratio
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			Log("Foliage CTEM ratio = %g %%\n", pF_CTEM * 100);


			//REPRODUCTION ONLY FOR NEEDLE LEAF
			if (s->value[PHENOLOGY] == 1.2)
			{
				//NPP for reproduction
				s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
				s->value[NPP] -= s->value[DEL_CONES_CTEM];
				s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
				Log("Biomass increment into cones = %g tDM/area\n", s->value[DEL_CONES_CTEM]);

				//reproductive life span
				//s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
			}


			//7 May 2012
			//compute fine and coarse root biomass
			s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
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
			//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);
			if (s->value[PHENOLOGY] == 1.2)
			{
				Log("Cones Biomass (Wc) = %g tDM/area\n", s->value[BIOMASS_CONES_CTEM]);
			}

			//allocation to stem
			s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
			Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

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


			//leaf litterfall from 3-PG
			//gammaF = s->value[GAMMAFX] * s->value[GAMMAF0] / (s->value[GAMMAF0] + (s->value[GAMMAFX] - s->value[GAMMAF0]) * exp(-12 * log(1 + s->value[GAMMAFX] / s->value[GAMMAF0]) * a->value / s->value[TGAMMAF]));
			//Log("Litterfall rate = %g\n", gammaF);

			//oldWf = s->value[BIOMASS_FOLIAGE_CTEM];
			Log("++Lai = %g\n", s->value[LAI]);
			Log("++DBHDC = %g\n", s->value[CANOPY_COVER_DBHDC]);

			//Log("OldWf = %g\n", oldWf);

			Log("-LITTERFALL FOR EVEGREEN-\n");
			s->value[DEL_LITTER] = /*gammaF*/ s->value[LITTERFALL_RATE] * s->value[BIOMASS_FOLIAGE_CTEM];
			Log("Litterfall rate = %g\n", s->value[LITTERFALL_RATE]);
			Log("Foliage Biomass to litter from evergreen population = %g tDM/area\n", s->value[DEL_LITTER]);

			Log("BALANCE BETWEEN BIOM FOLIAGE INC - LITTFALL = %g\n", s->value[DEL_FOLIAGE_CTEM] - s->value[DEL_LITTER]);
			if (s->value[DEL_FOLIAGE_CTEM]<s->value[DEL_LITTER])
			{
				Log("Del_litter exceeds del_foliage!!\n");
				s->value[DEL_FOLIAGE_CTEM] = s->value[DEL_LITTER];
			}

			s->value[BIOMASS_FOLIAGE_CTEM] -=  s->value[DEL_LITTER];
			Log("Foliage Biomass at the end of year less Litterfall (Wf + oldWf) in tDM/area = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

			//recompute LAI
			//Log("SLA in mod = %g KgC/m^2 \n", s->value[SLAmkg]);

			s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * 2.0);
			Log("Lai after litterfall = %g\n", s->value[LAI]);


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


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Roots CTEM ratio layer %d = %g %%\n", z, pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);

			//reserve ratio
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			//Log("Reserve CTEM ratio = %g %%\n", pF_CTEM * 100);

			//REPRODUCTION ONLY FOR NEEDLE LEAF
			if (s->value[PHENOLOGY] == 1.2)
			{
				//NPP for reproduction
				s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
				s->value[NPP] -= s->value[DEL_CONES_CTEM];
				s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
				Log("Biomass increment into cones = %g tDM/area\n", s->value[DEL_CONES_CTEM]);

				//reproductive life span
				//s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
			}

			//7 May 2012
			//comnpute fine and coarse root biomass
			s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] *  pR_CTEM;
			s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
			Log("BiomassRoots increment into fine roots = %g tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
			s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
			Log("BiomassRoots increment into coarse roots = %g tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

			if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT_CTEM])
			{
				Log("ERROR IN ROOTS ALLOCATION  \n");
			}


			s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
			Log("BiomassStem increment CTEM = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);


			s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * pF_CTEM;
			Log("BiomassFoliage increment CTEM = %g tDM/area\n", s->value[DEL_FOLIAGE_CTEM] );

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
			//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

			if (s->value[PHENOLOGY] == 1.2)
			{
				Log("Cones Biomass (Wc) = %g tDM/area\n", s->value[BIOMASS_CONES_CTEM]);
			}

			//allocation to stem
			s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
			Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

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

			//leaf litterfall

			//gammaF = s->value[GAMMAFX] * s->value[GAMMAF0] / (s->value[GAMMAF0] + (s->value[GAMMAFX] - s->value[GAMMAF0])* exp(-12 * log(1 + s->value[GAMMAFX] / s->value[GAMMAF0]) * a->value / s->value[TGAMMAF]));
			//Log("Litterfall rate = %g\n", gammaF);

			//oldWf = s->value[BIOMASS_FOLIAGE_CTEM];

			//Log("OldWf = %g\n", oldWf);
			s->value[DEL_LITTER] = /*gammaF*/ s->value[LITTERFALL_RATE] * s->value[BIOMASS_FOLIAGE_CTEM];
			Log("Foliage Biomass to litter from evergreen population = %g tDM/area\n", s->value[DEL_LITTER]);

			s->value[BIOMASS_FOLIAGE_CTEM] -=  s->value[DEL_LITTER];
			Log("Foliage Biomass at the end of year less Litterfall (Wf + oldWf) in tDM/area = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

			//recompute LAI
			Log("SLA in mod= %g KgC/m^2 \n", s->value[SLAmkg]);
			s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] *  1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * 2.0);
			Log("++Lai = %g\n", s->value[LAI]);
		}
	}
	else
	{
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
	}

	//for daily_Log file only if there's one class
	/*
	if (c->heights_count -1  == 0 && c->heights[height].ages_count -1 == 0 && c->heights[height].ages[age].species_count -1 == 0)
	{
		c->daily_lai = s->value[LAI];
	}
	 */
	if (c->annual_layer_number == 1)
	{
		c->daily_lai[0] = s->value[LAI];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->daily_lai[1] = s->value[LAI];
		}
		else
		{
			c->daily_lai[0] = s->value[LAI];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->daily_lai[2] = s->value[LAI];
		}
		if (c->heights[height].z == 1)
		{
			c->daily_lai[1] = s->value[LAI];
		}
		if (c->heights[height].z == 0)
		{
			c->daily_lai[0] = s->value[LAI];
		}
	}

	/*compute annual stem increment*/
	if (c->annual_layer_number == 1)
	{
		c->annual_delta_ws[0] += s->value[DEL_STEMS_CTEM];
		c->annual_ws[0] = s->value[BIOMASS_STEM_CTEM];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->annual_delta_ws[1] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[1] = s->value[BIOMASS_STEM_CTEM];
		}
		else
		{
			c->annual_delta_ws[0] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[0] = s->value[BIOMASS_STEM_CTEM];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->annual_delta_ws[2] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[2] = s->value[BIOMASS_STEM_CTEM];
		}
		if (c->heights[height].z == 1)
		{
			c->annual_delta_ws[1] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[1] = s->value[BIOMASS_STEM_CTEM];
		}
		if (c->heights[height].z == 0)
		{
			c->annual_delta_ws[0] += s->value[DEL_STEMS_CTEM];
			c->annual_ws[0] = s->value[BIOMASS_STEM_CTEM];
		}
	}



	Log("******************************\n");


}

/**/

//CURRENTLY NOT USED
void M_Get_Partitioning_Allocation_NASACASA (SPECIES *const s, int z, float Stand_NPP)
{
	/*NASA-CASA VERSION*/

	/*NASACASA*/
	float const r0nasacasa = 0.3;
	float const s0nasacasa = 0.3;
	float pS_NASACASA;
	float pR_NASACASA;
	float pF_NASACASA;

	Log("(NASA-CASA) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z);


	//Partitioning Ratio

	//Log("Light Trasmitted for this layer = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TRASM]);

	//Roots Ratio
	pR_NASACASA = (3 * r0nasacasa) * ( s->value[LIGHT_TRASM] / ( s->value[LIGHT_TRASM] + (2 * s->value[F_SW])));
	Log("Roots NASA-CASA ratio = %g %%\n", pR_NASACASA * 100);

	//Stem Ratio
	pS_NASACASA = ( 3 * s0nasacasa) * (s->value[F_SW] / (( 2 * s->value[LIGHT_TRASM] ) + s->value[F_SW]));
	Log("Stem NASA-CASA ratio = %g %%\n", pS_NASACASA * 100);

	//Foliage Ratio
	pF_NASACASA = 1 - ( pR_NASACASA + pS_NASACASA);
	Log("Foliage NASA-CASA ratio = %g %%\n", pF_NASACASA * 100);



	//Biomass Allocation

	//Log("Yearly Cumulated NPP for this layer  = %g tDM/area\n",  s->value[YEARLY_NPP]);

	s->value[BIOMASS_ROOTS_NASA_CASA] = Stand_NPP * pR_NASACASA;
	//Log("BiomassRoots NASA-CASA = %g tDM/area\n", s->value[BIOMASS_ROOTS_NASA_CASA]);

	s->value[BIOMASS_STEM_NASA_CASA] = Stand_NPP * pS_NASACASA;
	//Log("BiomassStem NASA-CASA = %g tDM/area\n", s->value[BIOMASS_STEM_NASA_CASA]);

	s->value[BIOMASS_FOLIAGE_NASA_CASA] = Stand_NPP * pF_NASACASA;
	//Log("BiomassFoliage NASA-CASA = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_NASA_CASA]);

	Log("******************************\n");
}
/**/


//CURRENTLY NOT USED
void M_Get_Partitioning_Allocation_3PG (SPECIES *const s, int z, int years, int month, int management, float Stand_NPP, const MET_DATA *const met)
{
	/*3PG VERSION*/
	float pfsPower;
	float pfsConst;
	static float emme;
	float pFS;
	float pR;
	float pS;
	float pF;
	float oldW;

	// control
	float RatioSum;

	//Log("\n-- (3PG) MONTHLY BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z, years);

	if (management == 0)
	{
		//Log("Management type = TIMBER\n");
	}
	else
	{
		//Log("Management type = COPPICE\n");
	}


	oldW = s->value[WF] + s->value[WS] + s->value[WRC] + s->value[WRF];


	//NOT USED IN MONTHLY PARTITIONING-ALLOCATION

	/*annual average of physiological modifier for partitioning*/
	/*
	s->value[AVERAGE_PHYS_MOD] = s->value[YEARLY_PHYS_MOD] / s->counter[VEG_MONTHS];
	Log("Average Physmod = %g \n", s->value[AVERAGE_PHYS_MOD]);
	 */

	emme = site->m0 + (1 - site->m0) * site->fr;
	//Log("emme = %g\n", emme );

	/* Calculate Partitioning Coefficients */
	// il driver del partitioning sono: fSW-fVPD-fAGE
	// 'emme' è omogeneo per tutte le celle

	if ( s->value[LAI] <= (s->value[PEAK_Y_LAI] / 2.0 ))
	{
		//Log("**Maximum Growth**\n");
		//Log("allocating only into foliage pools\n");
		s->value[WF] += Stand_NPP;

		//Log("BiomassFoliage 3PG = %g tDM/area\n", s->value[WF] );
	}
	else
	{
		if (met[month].daylength > s->value[MINDAYLENGTH])
		{

			/*timber routine*/
			if (management == 0)
			{
				pfsPower = log (s->value[PFS20] / s->value[PFS2]) / log (10.0);
				//Log("PFS20 = %g\n", s->value[PFS20]);
				//Log("PFS2 = %g\n", s->value[PFS2]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);
				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle con ugual specie fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				//IN MONTHLY RUOUTINE USE THE MONTHLY PHYSMOD

				// % di NPP to roots
				pR = s->value[PRX] * s->value[PRN] / (s->value[PRN] + (s->value[PRX] - s->value[PRN]) * s->value[PHYS_MOD] * emme);
				//Log("pR = %g\n", pR);
				//Log("PHYSMOD = %g\n", s->value[PHYS_MOD]);

				//Log("PRX = %g\n", s->value[PRX]);
				//Log("PRN = %g\n", s->value[PRN]);
				//Log("3PG ratio to roots layer %d = %g %%\n", z, pR * 100);
			}
			//end of timber routine
			/*coppice routine*/
			else
			{
				//per ora la routine per i cedui non è dinamica, non considera infatti che
				//all'aumentare degli anni dal taglio i valori di PF2, PF20, PRX e PRN si
				//devono avvicinare a quelli della routine della fustaia
				//manca una variabile che tenga conto degli anni dal taglio tra i dati di
				//inizializzazione, forse si puo ovviare considerando gli anni dal taglio
				//considerandola come se fosse l'età

				pfsPower = log (s->value[PFS20_C] / s->value[PFS2_C]) / log (10.0);
				//Log("PFS20_C = %g\n", s->value[PFS20_C]);
				//Log("PFS2_C = %g\n", s->value[PFS2_C]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);

				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2_C] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				// % di NPP to roots
				pR = s->value[PRX_C] * s->value[PRN_C] / (s->value[PRN_C] + (s->value[PRX_C] - s->value[PRN_C]) * s->value[PHYS_MOD] * emme);
				//Log("pR = %g\n", pR);
				//Log("PRX_C = %g\n", s->value[PRX_C]);
				//Log("PRN_C = %g\n", s->value[PRN_C]);
				//Log("3PG ratio to roots for coppice = %g %%\n", pR * 100);
			}

			//end of coppice routine


			//if (fabs(APAR) < 0.000001) APAR = 0.000001  da 3PG ma che è????????????



			// % di NPP to stem
			pS = (1 - pR) / (1 + pFS);
			//Log("pS = %g\n", pS);
			//Log("3PG ratio to stem = %g %%\n", pS * 100);

			// % di NPP to foliage
			pF = 1 - pR - pS;
			//Log("pF = %g\n", m->lpCell[index].pF);
			//Log("3PG ratio to foliage = %g %%\n", pF * 100);

			RatioSum = pF + pS + pR;
			//Log("pF + pS + pR = %d\n", (int)RatioSum);
			if (RatioSum != 1)
			{
				//Log("Error in Biomass Partitioning!!\n");
			}

			//Log("End of month NPP for this layer  = %g tDM/area\n",  Stand_NPP);

			//Foliage compart
			//Log("Initial Foliage Biomass (Wf) = %g tDM/area\n", s->value[INITIAL_WF]);

			// Fraction of NPP to Foliage
			s->value[DEL_WF] = Stand_NPP * pF;
			//Log("Increment Yearly Biomass allocated (delWf) = %g tDM/area\n", s->value[DEL_WF]);


			// Total Foliage Biomass


			s->value[WF] = s->value[DEL_WF] + s->value[WF];

			/*
            if (!years)
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }
            }
            else
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }

            }
			 */



			//Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[WF]);



			//Roots compart
			//Log("Initial Root Biomass (Wr) = %g tDM/area\n", s->value[INITIAL_WR]);

			// Fraction of NPP to Roots
			s->value[DEL_WR] = Stand_NPP * pR;
			//Log("Increment Yearly Biomass allocated (delWr) = %g tDM/area\n", s->value[DEL_WR]);

			// Total Roots Biomass

			s->value[WRT] = s->value[WRT] + s->value[DEL_WR];
			//Log("Root Biomass (Wr) = %g tDM/area\n", s->value[WR]);



			//Stem compart
			//Log("Initial Stem Biomass (Ws) = %g tDM/area\n", s->value[INITIAL_WS]);

			// Fraction of NPP to Stem
			s->value[DEL_WS] = Stand_NPP * pS;
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_WS] * s->value[FRACBB];
			//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_WS] -= s->value[DEL_BB];
			s->value[WS] = s->value[WS] + s->value[DEL_WS];
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_WS]);
			//Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[WS]);
		}
		else
		{
			/*timber routine*/
			if (management == 0)
			{
				pfsPower = log (s->value[PFS20] / s->value[PFS2]) / log (10.0);
				//Log("PFS20 = %g\n", s->value[PFS20]);
				//Log("PFS2 = %g\n", s->value[PFS2]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);
				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle con ugual specie fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				//IN MONTHLY RUOUTINE USE THE MONTHLY PHYSMOD

				// % di NPP to roots
				pR = s->value[PRX] * s->value[PRN] / (s->value[PRN] + (s->value[PRX] - s->value[PRN]) * s->value[PHYS_MOD] * emme);
				//Log("pR = %g\n", pR);
				//Log("PHYSMOD = %g\n", s->value[PHYS_MOD]);

				//Log("PRX = %g\n", s->value[PRX]);
				//Log("PRN = %g\n", s->value[PRN]);
				//Log("3PG ratio to roots layer %d = %g %%\n", z, pR * 100);
			}
			//end of timber routine
			/*coppice routine*/
			else
			{
				//per ora la routine per i cedui non è dinamica, non considera infatti che
				//all'aumentare degli anni dal taglio i valori di PF2, PF20, PRX e PRN si
				//devono avvicinare a quelli della routine della fustaia
				//manca una variabile che tenga conto degli anni dal taglio tra i dati di
				//inizializzazione, forse si puo ovviare considerando gli anni dal taglio
				//considerandola come se fosse l'età

				pfsPower = log (s->value[PFS20_C] / s->value[PFS2_C]) / log (10.0);
				//Log("PFS20_C = %g\n", s->value[PFS20_C]);
				//Log("PFS2_C = %g\n", s->value[PFS2_C]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);

				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2_C] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				// % di NPP to roots
				pR = s->value[PRX_C] * s->value[PRN_C] / (s->value[PRN_C] + ((s->value[PRX_C] - s->value[PRN_C]) * s->value[PHYS_MOD] * emme));
				//Log("pR = %g\n", pR);
				//Log("PRX_C = %g\n", s->value[PRX_C]);
				//Log("PRN_C = %g\n", s->value[PRN_C]);
				//Log("3PG ratio to roots for coppice = %g %%\n", pR * 100);
			}

			//end of coppice routine


			//if (fabs(APAR) < 0.000001) APAR = 0.000001  da 3PG ma che è????????????



			// % di NPP to stem
			pS = (1 - pR) / (1 + pFS);
			//Log("pS = %g\n", pS);
			//Log("3PG ratio to stem = %g %%\n", pS * 100);

			// % di NPP to foliage
			pF = 1 - pR - pS;
			//Log("pF = %g\n", m->lpCell[index].pF);
			//Log("3PG ratio to foliage = %g %%\n", pF * 100);

			pS += (pF / 2.0);
			pR += (pF / 2.0);
			pF = 0;

			RatioSum = pF + pS + pR;
			//Log("pF + pS + pR = %d\n", (int)RatioSum);
			if (RatioSum != 1)
			{
				//Log("Error in Biomass Partitioning!!\n");
			}

			//Log("End of month NPP for this layer  = %g tDM/area\n",  Stand_NPP);

			//Foliage compart
			//Log("Initial Foliage Biomass (Wf) = %g tDM/area\n", s->value[INITIAL_WF]);

			// Fraction of NPP to Foliage
			s->value[DEL_WF] = Stand_NPP * pF;
			//Log("Increment Yearly Biomass allocated (delWf) = %g tDM/area\n", s->value[DEL_WF]);


			// Total Foliage Biomass


			s->value[WF] = s->value[DEL_WF] + s->value[WF];

			/*
            if (!years)
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }
            }
            else
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }

            }
			 */



			//Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[WF]);



			//Roots compart
			//Log("Initial Root Biomass (Wr) = %g tDM/area\n", s->value[INITIAL_WR]);

			// Fraction of NPP to Roots
			s->value[DEL_WR] = Stand_NPP * pR;
			//Log("Increment Yearly Biomass allocated (delWr) = %g tDM/area\n", s->value[DEL_WR]);

			// Total Roots Biomass

			s->value[WRT] = s->value[WRT] + s->value[DEL_WR];
			//Log("Root Biomass (Wr) = %g tDM/area\n", s->value[WR]);



			//Stem compart
			//Log("Initial Stem Biomass (Ws) = %g tDM/area\n", s->value[INITIAL_WS]);

			// Fraction of NPP to Stem
			s->value[DEL_WS] = Stand_NPP * pS;
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_WS] * s->value[FRACBB];
			//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_WS] -= s->value[DEL_BB];
			s->value[WS] = s->value[WS] + s->value[DEL_WS];
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_WS]);
			//Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[WS]);

		}
	}


	// Total Biomass Increment
	s->value[DEL_TOTAL_W] = s->value[DEL_WF] + s->value[DEL_WR] + s->value[DEL_WS];
	//Log("Increment Yearly Total Biomass  (delTotalW) = %g tDM/area\n", s->value[DEL_TOTAL_W]);


	// Total Biomass
	s->value[TOTAL_W] = s->value[WF] + s->value[WS] + s->value[WRT];
	//Log("Previous Total W = %g tDM/area\n", oldW);
	//Log("Total Biomass = %g tDM/area\n", s->value[TOTAL_W]);



	//cumulated yearly biomass increment
	//s->value[DEL_Y_WS] += s->value[DEL_WS];
	//Log("Increment Yearly STEM Biomass = %g tDM/area\n", s->value[DEL_Y_WS]);
	//s->value[DEL_Y_WR] += s->value[DEL_WR];
	//Log("Increment Yearly ROOT Biomass = %g tDM/area\n", s->value[DEL_Y_WR]);
	//s->value[DEL_Y_WF] += s->value[DEL_WF];
	//Log("Increment Yearly FOLIAGE Biomass = %g tDM/area\n", s->value[DEL_Y_WF]);






	/*control*/
	if (oldW > s->value[TOTAL_W])
	{
		//Log("ERROR in Partitioning-Allocation!!!\n");

		//Log("Previous Total W = %g tDM/area\n", oldW);

	}




	//Log("******************************\n");


}

void M_Get_Fruit_Allocation_LPJ (SPECIES *const s, int z, int years, float Yearly_Rain, float canopy_cover_dominant )
{
	//static float WseedTree;    //fruit biomass per Tree
	static int NseedTree;      //Number of fruits per Tree

	//static float FProCov;   //LPJ Foliage Projective  Cover for Seed Establishment
	//static float LightAbsorb;
	//static float EstabRate;   //Seed establishment rate Under Dominant Canopy
	//static int Nsapling;      //Number of saplings Under and Outside Dominant Canopy


	Log("**SEEDS-ALLOCATION**\n");

	Log("\n--LPJ FRUIT ALLOCATION--\n");





	//il 10% lo do ai frutti
	//fraction of total NPP to Fruit compart
	//biomass to seeds
	s->value[W_SEED] = s->value[YEARLY_NPP] * s->value[FRACFRUIT];
	Log("Costant Fraction Rate of Annual NPP for Fruit Production using LPJ = %g %%\n", s->value[FRACFRUIT] * 100);
	Log("Annual NPP to Seeds Biomass Compart = %g tDM/area/year\n", s->value[W_SEED]);


	//WseedTree = s->value[W_SEED] / s->counter[N_TREE];



	//Log("Annual Biomass for Seeds Compart for Tree  = %g in tDM/tree/year\n", WseedTree);
	//Log("Annual Biomass for Seeds Compart for Tree  = %g in Kg/tree/year\n", WseedTree * 1000);
	//Number of seeds from tDM to grammes
	s->counter[N_SEED] = (s->value[W_SEED] * 1000000)/ s->value[WEIGHTSEED];
	//Log("Annual Number of seeds using LPJ  = %d seeds/area/year\n", s->counter[N_SEED]);
	/*
	   Log("NSEED %g\n", s->counter[N_SEED]);
	   Log("NTREE %g\n", s->counter[N_TREE]);
	   NseedTree = s->counter[N_SEED] / s->counter[N_TREE];
	 */
	Log("Annual Number of Seeds Produced for Tree using LPJ = %d seeds/area/year\n", s->counter[N_SEED]);
	Log("Annual Number of Seeds Produced for Tree using LPJ = %d seeds/tree/year\n", NseedTree);

	/*
	   Log("Intrinsic Germinability Rate = %g %% \n", s->value[GERMCAPACITY] * 100);


	   if (Yearly_Rain > s->value[MINRAIN])
	   {

	   Log("Annual Number of seeds using LPJ  = %d seeds/area/year\n", s->counter[N_SEED]);

	//da ricontrollare per vedere quale Lai prende di quale layer e semmai non utilizzare il FProCov





	LightAbsorb = 1 -  (exp(- s->value[K] * s->value[LAI]));

	Log("Light Absorb in Dominant Canopy = %g \n", LightAbsorb);
	FProCov = canopy_cover_dominant * LightAbsorb;
	Log("Canopy Cover in  Dominant layer with DBHDC function = %g \n", canopy_cover_dominant);

	Log("LPJ Fractional Projective Cover FPC = %g \n", FProCov);


	EstabRate = s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - FProCov))) * (1 - FProCov);
	Log("Seed Establishment Rate from LPJ = %g saplings/m^2 \n", EstabRate);

	Nsapling = s->counter[N_SEED] * EstabRate ;
	Log("Annual Number of Saplings per hectare using LPJ = %d Saplings/year hectare\n", Nsapling);
	Log("Annual Number of Saplings using LPJ = %g Saplings/year m^2\n", (float) Nsapling / SIZECELL );
	Log("Percentage of seeds survived using LPJ = %g %% seeds/year hectare\n", ((float)Nsapling * 100)/(float)s->counter[N_SEED] );
	//Log("------THE MODEL STILL DOESN'T TAKE INTO ACCOUNT THE PRESENTS OF SUBDOMINANTS SPECIES!!!!------\n");
	}
	else
	{
	Log("Not enough Rain for Seeds Establishment!!!!\n");
	}

	//s->value[YEARLY_NPP] = s->value[YEARLY_NPP] - s->value[W_SEED];
	//Log("Annual NPP  less Seeds Biomass using LPJ = %g tDM/area/year\n", s->value[YEARLY_NPP]);
	 */

}


/**/
int M_Get_Fruit_Allocation_Logistic_Equation (SPECIES *const s, AGE *const a)
{
	/*USING A LOGISTIC EQUATION*/
	static int NumberSeed;                  //Number of Seeds per tree
	static int PopNumberSeeds;              //Number of Seeds per Population
	static int MaxSeed = 2000;              //Maximum seeds number
	static int OptSexAge = 100;             //Age at maximum seeds production
	static int MinSexAge = 20;              //Minimum age for sex maturity
	static float WseedLE ;                  //Weight of seeds of population from Logistic Equation

	Log("------LOGISTIC EQUATION FRUIT ALLOCATION------\n");



	NumberSeed = (MaxSeed/ (1 + OptSexAge * exp (-0.1 * (a->value - MinSexAge))));
	Log("Annual Number of Seeds for Tree from Logistic Equation = %d seeds/tree/year\n", NumberSeed);

	PopNumberSeeds = NumberSeed * s->counter[N_TREE];
	Log("Annual Number of Seeds for Population from Logistic Equation = %d seeds/area/year\n", PopNumberSeeds);

	WseedLE = ((float)PopNumberSeeds * s->value[WEIGHTSEED]) / 1000000 /* to convert in tonnes*/;
	Log("Biomass for Seed from Logistic Equation = %g tDM/area\n", WseedLE);
	Log("Fraction of Biomass allocated for Seed from Logistic Equation = %g tDM/area\n", WseedLE);
	Log("Fraction of NPP allocated for Seed from Logistic Equation = %.4g %%\n", (WseedLE * 100) /s->value[YEARLY_NPP] );

	return NumberSeed;
}
/**/

int M_Get_Fruit_Allocation_TREEMIG (SPECIES *const s, AGE *const a)
{
	static int NumberSeed;
	static float heigthdependence;
	static float WseedT;            //height dependence factor

	Log("------TREEMIG FRUIT ALLOCATION------\n");

	heigthdependence = s->value[LAI] / s->value[LAIGCX] ;//sarebbe Lai  / Lai max;
	Log("heigthdependence = %g \n", heigthdependence);

	//numero semi prodotti
	NumberSeed = (float)s->counter[N_TREE] * s->value[MAXSEED] * heigthdependence * 0.51 *
			( 1 + sin((2 * Pi * (float)a->value ) / s->value[MASTSEED]));
	Log("Nseed per cell at the End of the This Year = %d seeds per cell\n", NumberSeed);

	//Biomassa allocata nei semi in tDM/area
	WseedT = (float)NumberSeed * s->value[WEIGHTSEED] / 1000000;  //per convertire in tonnellate biomassa allocata per i semi
	Log("Seeds Biomass per cell at the End of the This Year = %g tonnes of seeds/area \n", WseedT);
	Log("Fraction of NPP allocated for Seed from TREEMIG = %.4g %%\n", (WseedT * 100) /s->value[YEARLY_NPP] );

	return NumberSeed;

}



