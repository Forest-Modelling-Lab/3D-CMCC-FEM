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
void E_Get_Partitioning_Allocation_CTEM (SPECIES *const s, CELL *const c, const MET_DATA *const met, int day, int month, int years, int DaysInMonth, int height, int age, int species)
{
	//CTEM VERSION

	Log("GET_ALLOCATION_ROUTINE\n\n");

	Log("Carbon allocation routine for evergreen\n");
	Log("Version = %c \n", settings->version);
	Log("Spatial = %c \n", settings->spatial);

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
	double Monthly_solar_radiation;
	double Light_trasm;
	double Par_over;

	double Perc_fine;
	double Perc_coarse;
	//double Perc_leaves;              //percentage of leaves in first growing season
	double oldW;
	//double gammaF;
	//double oldWf;

	double Biomass_exceeding;

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

	//7 May 2012
	//compute static ratio of allocation between fine
	//fixme see if chage with new parameters checked in "Get_biome_fraction"
	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
	//Log("Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
	//Log("Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
	Perc_coarse = 1- Perc_fine;
	//Log("Percentage of coarse root against total root= %f %%\n", Perc_coarse * 100 );


	if(s->value[NPP] > 0)
	{
		//Log("fracBB = %f\n", m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]);

		//I could try to get in instead F_SW the minimum value between F_SW and F_VPD  2 apr 2012
		//reductor = Minimum (s->value[F_SW], s->value[F_VPD]);
		//I could try to get in instead F_SW the minimum value between F_SW and F_NUTR  18 apr 2012
		//reductor = Minimum (s->value[F_SW], s->value[F_NUTR]);
		//reductor = s->value[F_SW];

		//fixme add also an F_LIGHT chiedi ad Angelo del suo F_LIGHT
		/*following Barbaroux et al., 2002 if there's a deficit in soil water plants allocate into reserve
		 * differently into foliage, so if F_LIGHT > F_SW then model should allocates into foliage pool otherwise
		 * into reserve pool
		 */
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

		//fixme if setted to 0, cones are negative during simulation
		if (day == 0 && month == 0 && years == 0)
		{
			s->value[BIOMASS_CONES_CTEM] = 0;
		}



		if (settings->spatial == 'u')
		{
			Log("Unspatial version \n");

			oldW = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM];


			//7 May 2012
			//compute static ratio of allocation between fine
			//fixme see if change with new parameters checked in "Get_biome_fraction"
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

			Log("PHENOLOGICAL PHASE = %d\n", s->phenology_phase);

			switch (s->phenology_phase)
			{
			case 1:
				Log("LAI = %f \n", s->value[LAI]);
				Log("**Maximum Growth**\n");
				Log("Allocating into foliage and stem pools\n");
				//fixme scegliere se usare Magnani o meno
			//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio

/*
			s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * (1.0 - s->value[STEM_LEAF_FRAC]);
			s->value[DEL_STEMS_CTEM] = (s->value[NPP] - s->value[DEL_FOLIAGE_CTEM]);


			s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
			Log("Biomass Foliage CTEM = %f tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);
			Log("Reserve Biomass = %f tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

			s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
			Log("Biomass Stem CTEM = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM] );
*/
				/*allocating into fine root and foliage*/
				s->value[DEL_FOLIAGE_CTEM] += (s->value[NPP] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
				Log("Biomass NPP allocated to foliage pool = %f\n", s->value[BIOMASS_FOLIAGE_CTEM]);


				s->value[DEL_ROOTS_FINE_CTEM] += (s->value[NPP] * s->value[FINE_ROOT_LEAF_FRAC]);
				Log("Biomass NPP allocated to fine root pool = %f\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);


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
			Log("++Lai = %f\n", s->value[LAI]);

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
				Log("LAI = %f \n", s->value[LAI]);


				Log("Re-allocating foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n");

				Biomass_exceeding = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MAX_BIOMASS_FOLIAGE_CTEM];
				Log("Foliar Biomass exceeding = %f tDM \n", Biomass_exceeding);
				Log("Biomass Foliage = %f tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);
				s->value[BIOMASS_FOLIAGE_CTEM] = s->value[MAX_BIOMASS_FOLIAGE_CTEM];
				Log("Max Biomass Foliage = %f tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]);
				Log("Biomass Foliage = %f tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);


				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Roots CTEM ratio = %f %%\n",  pR_CTEM * 100);
				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);

				//reserve ratio
				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);


				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = Biomass_exceeding * pR_CTEM;
				Log("BiomassRoots increment CTEM = %f tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


				//7 May 2012
				//compute fine and coarse root biomass increment
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_fine;
				Log("BiomassRoots into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
				Log("BiomassRoots into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


				s->value[DEL_STEMS_CTEM] = Biomass_exceeding *  pS_CTEM;
				Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS_CTEM]);

				s->value[DEL_RESERVE_CTEM] = Biomass_exceeding * pF_CTEM;
				Log("BiomassReserve increment CTEM = %f tDM/area\n", s->value[DEL_RESERVE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to non structural reserve pool
				s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				//check for live and dead tissues
				/*stem*/
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
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

				s->value[DEL_FOLIAGE_CTEM] = 0;

				Log("LAI setted to Peak Lai = %f \n", s->value[PEAK_Y_LAI]);
				s->value[LAI] = s->value[PEAK_Y_LAI];

				// Total Biomass Increment
				s->value[DEL_TOTAL_W] = s->value[DEL_FOLIAGE_CTEM] + s->value[DEL_ROOTS_TOT_CTEM] + s->value[DEL_STEMS_CTEM];
				Log("Increment Monthly Total Biomass  (delTotalW) = %f tDM/area\n", s->value[DEL_TOTAL_W]);


				// Total Biomass
				s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_TOT_CTEM];
				Log("Previous Total W = %f tDM/area\n", oldW);
				Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);


				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];
			}
			break;
			case 2:
				Log("Normal growth\n");
				Log("allocating into the three pools Ws+Wr+Wreserve\n");


				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Roots CTEM ratio layer %d = %f %%\n", z, pR_CTEM * 100);


				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				//Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);



				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				//Log("Foliage CTEM ratio = %f %%\n", pF_CTEM * 100);

				//REPRODUCTION ONLY FOR NEEDLE LEAF
				//fixme non sarà troppo!!!!
				if (s->value[PHENOLOGY] == 1.2)
				{
					//NPP for reproduction
					s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
					s->value[NPP] -= s->value[DEL_CONES_CTEM];
					s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
					Log("Biomass increment into cones = %f tDM/area\n", s->value[DEL_CONES_CTEM]);

					//reproductive life span
					s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
				}


				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
				Log("BiomassRoots increment CTEM = %f tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);

				//7 May 2012
				//comnpute fine and coarse root biomass
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
				Log("BiomassRoots increment into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  - s->value[DEL_ROOTS_FINE_CTEM] ;
				Log("BiomassRoots increment into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

				if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT_CTEM])
				{
					Log("ERROR IN ROOTS ALLOCATION del coarse + del fine = %f, del tot = %f \n", (s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]), s->value[DEL_ROOTS_TOT_CTEM]);
				}


				s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
				Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS_CTEM]);


				s->value[DEL_RESERVE_CTEM] = s->value[NPP] * pF_CTEM;
				Log("BiomassRESERVE increment CTEM = %f tDM/area\n", s->value[DEL_RESERVE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to foliage
				s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);



				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//check for live and dead tissues
				/*stem*/
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
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

				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];

				Log("aF %d = 0 \n", c->heights[height].z, pF_CTEM);
				Log("afR %d = %f \n", c->heights[height].z, Perc_fine * pR_CTEM);
				Log("acR %d = %f \n", c->heights[height].z, Perc_coarse * pR_CTEM);
				Log("aS %d = %f \n", c->heights[height].z, pS_CTEM);
				Log("aRes %d = %f \n", c->heights[height].z, pF_CTEM);

				Log("delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE_CTEM] );
				Log("delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE_CTEM]);
				Log("delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE_CTEM]);
				Log("delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS_CTEM]);
				Log("delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE_CTEM]);
				Log("delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);

				c->daily_delta_ws[i] = s->value[DEL_STEMS_CTEM];
				c->daily_delta_wf[i] = s->value[DEL_FOLIAGE_CTEM];
				c->daily_delta_wbb[i] = s->value[DEL_BB];
				c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE_CTEM];
				c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE_CTEM];
				c->daily_delta_wres[i] = s->value[DEL_RESERVE_CTEM];

				break;
			}



/*
			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Roots CTEM ratio  = %f %%\n", pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);

			//foliage ratio
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			Log("Foliage CTEM ratio = %f %%\n", pF_CTEM * 100);


			//REPRODUCTION ONLY FOR NEEDLE LEAF
			if (s->value[PHENOLOGY] == 1.2)
			{
				//NPP for reproduction
				s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
				s->value[NPP] -= s->value[DEL_CONES_CTEM];
				s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
				Log("Biomass increment into cones = %f tDM/area\n", s->value[DEL_CONES_CTEM]);

				//reproductive life span
				//s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
			}


			//7 May 2012
			//compute fine and coarse root biomass
			s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] * pR_CTEM;
			s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
			Log("BiomassRoots increment into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
			s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
			Log("BiomassRoots increment into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

			if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT_CTEM])
			{
				Log("ERROR IN ROOTS ALLOCATION del coarse + del fine = %f, del tot = %f \n", (s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]), s->value[DEL_ROOTS_TOT_CTEM]);
			}


			s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
			Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS_CTEM]);


			s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * pF_CTEM;
			Log("BiomassFoliage increment CTEM = %f tDM/area\n", s->value[DEL_FOLIAGE_CTEM] );

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
			Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
			//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);
			if (s->value[PHENOLOGY] == 1.2)
			{
				Log("Cones Biomass (Wc) = %f tDM/area\n", s->value[BIOMASS_CONES_CTEM]);
			}

			//allocation to stem
			s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

			//allocation to branch and bark
			s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

			//allocation to foliage
			s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);



			//allocation to roots
			s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

			s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
			s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

			//check for live and dead tissues
			//stem
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] * s->value[LIVE_TOTAL_WOOD]);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] * (1.0 -s->value[LIVE_TOTAL_WOOD]));
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
			//coarse root
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * s->value[LIVE_TOTAL_WOOD]);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * (1.0 -s->value[LIVE_TOTAL_WOOD]));
			Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			//branch and bark
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] * s->value[LIVE_TOTAL_WOOD]);
			Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] * (1.0 -s->value[LIVE_TOTAL_WOOD]));
			Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

			//recompute LAI after allocation
			//for dominant layer with sunlit foliage
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
			}
			//for dominated shaded foliage
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
			}
			Log("++Lai after allocation = %f\n", s->value[LAI]);
			//control if new Lai exceeds Peak Lai
			if (s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("ATTENTION LAI > PEAK_Y_LAI!!! reallocate foliage biomass exceeding\n");
				Log("LAI = %f \n", s->value[LAI]);


				Log("Re-allocating foliage exceeding biomass into the three pools Ws+Wr+Wreserve\n");

				Biomass_exceeding = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MAX_BIOMASS_FOLIAGE_CTEM];
				Log("Foliage Biomass exceeding = %f tDM \n", Biomass_exceeding);
				Log("Biomass Foliage = %f tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);
				s->value[BIOMASS_FOLIAGE_CTEM] = s->value[MAX_BIOMASS_FOLIAGE_CTEM];
				Log("Max Biomass Foliage = %f tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]);
				Log("Biomass Foliage = %f tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);


				pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Roots CTEM ratio = %f %%\n",  pR_CTEM * 100);
				pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
				Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);

				//reserve ratio
				pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
				Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);


				// Biomass allocation

				s->value[DEL_ROOTS_TOT_CTEM] = Biomass_exceeding * pR_CTEM;
				Log("BiomassRoots increment CTEM = %f tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


				//7 May 2012
				//compute fine and coarse root biomass increment
				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_fine;
				Log("BiomassRoots into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
				Log("BiomassRoots into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


				s->value[DEL_STEMS_CTEM] = Biomass_exceeding *  pS_CTEM;
				Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS_CTEM]);

				s->value[DEL_RESERVE_CTEM] = Biomass_exceeding * pF_CTEM;
				Log("BiomassReserve increment CTEM = %f tDM/area\n", s->value[DEL_RESERVE_CTEM] );

				//Total Stem Biomass
				//remove the part allocated to the branch and bark
				s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
				Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
				//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
				//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

				//allocation to stem
				s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
				s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
				Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

				//allocation to branch and bark
				s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
				Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

				//allocation to non structural reserve pool
				s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
				Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

				//allocation to roots
				s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
				Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

				s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
				Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
				s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
				Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

				//check for live and dead tissues
				//stem
				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
				s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
				//coarse root
				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * s->value[LIVE_TOTAL_WOOD_FRAC]);
				Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
				Log("Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
				//branch and bark
				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] * s->value[LIVE_TOTAL_WOOD_FRAC]);
				Log("Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
				Log("Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

				s->value[DEL_FOLIAGE_CTEM] = 0;

				Log("LAI setted to Peak Lai = %f \n", s->value[PEAK_Y_LAI]);
				s->value[LAI] = s->value[PEAK_Y_LAI];

				// Total Biomass Increment
				s->value[DEL_TOTAL_W] = s->value[DEL_FOLIAGE_CTEM] + s->value[DEL_ROOTS_TOT_CTEM] + s->value[DEL_STEMS_CTEM];
				Log("Increment Monthly Total Biomass  (delTotalW) = %f tDM/area\n", s->value[DEL_TOTAL_W]);


				// Total Biomass
				s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_TOT_CTEM];
				Log("Previous Total W = %f tDM/area\n", oldW);
				Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);


				s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
				s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
				s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
				s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
				s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
				s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
				s->value[DEL_Y_BB] += s->value[DEL_BB];
			}
			*/
		}

		if (settings->spatial == 's')
		{
			Log("Spatial version \n");


			oldW = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_COARSE_CTEM] + s->value[BIOMASS_ROOTS_FINE_CTEM];


			//7 May 2012
			//compute static ratio of allocation between fine and coarse root
			//deriving data from values reported for BIOME-BGC
			//fixme see if chage with new parameters checked in "Get_biome_fraction"
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


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Roots CTEM ratio layer %d = %f %%\n", z, pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			//Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);

			//reserve ratio
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			//Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);

			//REPRODUCTION ONLY FOR NEEDLE LEAF
			if (s->value[PHENOLOGY] == 1.2)
			{
				//NPP for reproduction
				s->value[DEL_CONES_CTEM] = s->value[NPP] * s->value[CONES_PERC];
				s->value[NPP] -= s->value[DEL_CONES_CTEM];
				s->value[BIOMASS_CONES_CTEM] += s->value[DEL_CONES_CTEM];
				Log("Biomass increment into cones = %f tDM/area\n", s->value[DEL_CONES_CTEM]);

				//reproductive life span
				//s->value[BIOMASS_CONES_CTEM] -= (s->value[BIOMASS_CONES_CTEM] * (1 / s->value[CONES_LIFE_SPAN]));
			}

			//7 May 2012
			//comnpute fine and coarse root biomass
			s->value[DEL_ROOTS_TOT_CTEM] = s->value[NPP] *  pR_CTEM;
			s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
			Log("BiomassRoots increment into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
			s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
			Log("BiomassRoots increment into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);

			if(( s->value[DEL_ROOTS_FINE_CTEM] + s->value[DEL_ROOTS_COARSE_CTEM]) != s->value[DEL_ROOTS_TOT_CTEM])
			{
				Log("ERROR IN ROOTS ALLOCATION  \n");
			}


			s->value[DEL_STEMS_CTEM] = s->value[NPP] *  pS_CTEM;
			Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS_CTEM]);


			s->value[DEL_FOLIAGE_CTEM] = s->value[NPP] * pF_CTEM;
			Log("BiomassFoliage increment CTEM = %f tDM/area\n", s->value[DEL_FOLIAGE_CTEM] );

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
			Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
			//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

			//allocation to branch and bark
			s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

			if (s->value[PHENOLOGY] == 1.2)
			{
				Log("Cones Biomass (Wc) = %f tDM/area\n", s->value[BIOMASS_CONES_CTEM]);
			}

			//allocation to stem
			s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

			//allocation to foliage
			s->value[BIOMASS_FOLIAGE_CTEM] +=  s->value[DEL_FOLIAGE_CTEM];
			Log("Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE_CTEM]);



			//allocation to roots
			s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

			s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
			s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

			//check for live and dead tissues
			/*stem*/
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
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

			s->value[DEL_LITTER] = /*gammaF*/ s->value[LITTERFALL_RATE] * s->value[BIOMASS_FOLIAGE_CTEM];
			Log("Foliage Biomass to litter from evergreen population = %f tDM/area\n", s->value[DEL_LITTER]);

			s->value[BIOMASS_FOLIAGE_CTEM] -=  s->value[DEL_LITTER];
			Log("Foliage Biomass at the end of year less Litterfall (Wf + oldWf) in tDM/area = %f\n", s->value[BIOMASS_FOLIAGE_CTEM]);

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
			Log("++Lai = %f\n", s->value[LAI]);
		}
	}
	else
	{
		Log("NPP < 0, no allocationoccurs!!\n");
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

		/*check if LAI > PEAK_Y_LAI*/
		if (s->value[LAI] > s->value[PEAK_Y_LAI])
		{
			Log("ATTENTION LAI > PEAK_Y_LAI!!! reallocate biomass exceeding\n");
			Log("LAI = %f \n", s->value[LAI]);


			Log("Re-allocating foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n");

			Biomass_exceeding = s->value[BIOMASS_FOLIAGE_CTEM] - s->value[MAX_BIOMASS_FOLIAGE_CTEM];
			Log("Foliar Biomass exceeding = %f tDM \n", Biomass_exceeding);
			Log("Biomass Foliage = %f tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);
			s->value[BIOMASS_FOLIAGE_CTEM] = s->value[MAX_BIOMASS_FOLIAGE_CTEM];
			Log("Max Biomass Foliage = %f tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]);
			Log("Biomass Foliage = %f tDM/area \n", s->value[BIOMASS_FOLIAGE_CTEM]);


			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Roots CTEM ratio = %f %%\n",  pR_CTEM * 100);
			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
			Log("Stem CTEM ratio = %f %%\n", pS_CTEM * 100);

			//reserve ratio
			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
			Log("Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);


			// Biomass allocation

			s->value[DEL_ROOTS_TOT_CTEM] = Biomass_exceeding * pR_CTEM;
			Log("BiomassRoots increment CTEM = %f tDM/area\n", s->value[DEL_ROOTS_TOT_CTEM]);


			//7 May 2012
			//compute fine and coarse root biomass increment
			s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_fine;
			Log("BiomassRoots into fine roots = %f tDM/area\n", s->value[DEL_ROOTS_FINE_CTEM]);
			s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
			Log("BiomassRoots into coarse roots = %f tDM/area\n", s->value[DEL_ROOTS_COARSE_CTEM]);


			s->value[DEL_STEMS_CTEM] = Biomass_exceeding *  pS_CTEM;
			Log("BiomassStem increment CTEM = %f tDM/area\n", s->value[DEL_STEMS_CTEM]);

			s->value[DEL_RESERVE_CTEM] = Biomass_exceeding * pF_CTEM;
			Log("BiomassReserve increment CTEM = %f tDM/area\n", s->value[DEL_RESERVE_CTEM] );

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
			Log("BiomassStemBB increment CTEM = %f tDM/area\n", s->value[DEL_BB]);
			//Log("Branch and bark fraction = %f %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %f tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
			s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
			Log("Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_CTEM]);

			//allocation to branch and bark
			s->value[BIOMASS_STEM_BRANCH_CTEM] += s->value[DEL_BB];
			Log("Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_CTEM]);

			//allocation to non structural reserve pool
			s->value[BIOMASS_RESERVE_CTEM] +=  s->value[DEL_RESERVE_CTEM];
			Log("Reserve Biomass (Wres) = %f tDM/area\n", s->value[BIOMASS_RESERVE_CTEM]);

			//allocation to roots
			s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
			Log("Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

			s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
			Log("Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
			s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
			Log("Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);

			//check for live and dead tissues
			/*stem*/
			s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
			Log("Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
			//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
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

			s->value[DEL_FOLIAGE_CTEM] = 0;

			Log("LAI setted to Peak Lai = %f \n", s->value[PEAK_Y_LAI]);
			s->value[LAI] = s->value[PEAK_Y_LAI];

			// Total Biomass Increment
			s->value[DEL_TOTAL_W] = s->value[DEL_FOLIAGE_CTEM] + s->value[DEL_ROOTS_TOT_CTEM] + s->value[DEL_STEMS_CTEM];
			Log("Increment Monthly Total Biomass  (delTotalW) = %f tDM/area\n", s->value[DEL_TOTAL_W]);


			// Total Biomass
			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_CTEM] + s->value[BIOMASS_STEM_CTEM] + s->value[BIOMASS_ROOTS_TOT_CTEM];
			Log("Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);


			s->value[DEL_Y_WS] += s->value[DEL_STEMS_CTEM];
			s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE_CTEM];
			s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE_CTEM];
			s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE_CTEM];
			s->value[DEL_Y_WRES] += s->value[DEL_RESERVE_CTEM];
			s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT_CTEM];
			s->value[DEL_Y_BB] += s->value[DEL_BB];
		}
	}

	//for daily_Log file only if there's one class
	/*
	if (c->heights_count -1  == 0 && c->heights[height].ages_count -1 == 0 && c->heights[height].ages[age].species_count -1 == 0)
	{
		c->daily_lai = s->value[LAI];
	}
	 */
	i = c->heights[height].z;

	c->daily_lai[i] = s->value[LAI];
	c->annual_delta_ws[i] += s->value[DEL_STEMS_CTEM];
	c->annual_ws[i] = s->value[BIOMASS_STEM_CTEM];
	Log("******************************\n");


}

/**/

