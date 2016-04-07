/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Get_turnover_Marconi (SPECIES *s, CELL *c, int DaysInMonth, int height)
{

	double dStemBiomass, dCoarseBiomass, dBranchBiomass;

		Log("****DAILY TURNOVER following Marconi flowChart****\n");

	//The first cycle of turnover for the simulation the module follows a modification of Biome's routine. That is, being the first year,
	//the model can't calculate the dBiomass for the previous year, and uses the current increments to define the quantity of tissues
	//death (expected a slight underestimation though).
	// from the second cycle on, the model takes the quantity of tissue developed the antecedent cycle and expects it to be dying. That
	// assuming the average period of turnover for the specific tissue of the specific species and eventually of age a.
	// If that's so, the model assumes from the second cycle on, that the quantity of turnover is the dot[cycle-1].dCarbonPool
	//Marconi 17/06: at the very begining of growing season these variables are set to the value of Biomass so that dC is reliable (solves the first YOS Litter jump)
	//fixsergio: what if a flag years == 0 would be added too?

	/*following biome turnover occurs only during growing season*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		//fixme see if add foliage turnover!!!!!

		/*daily leaf turnover for EVERGREEN*/
		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
		{
			if(c->dos  < s->turnover->FINERTOVER)
			{
				Log("****leaf turnover for evergreen****\n");
				s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
				Log("Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);
				s->value[BIOMASS_FOLIAGE_tDM] -= (s->value[OLD_BIOMASS_LEAVES] * s->value[LITTERFALL_RATE]);
				/*recompute LAI after turnover*/
				/*for dominant layer with sunlit foliage*/
				Log("Foliage Biomass after turnover = %f\n", s->value[BIOMASS_FOLIAGE_tDM]);
				c->leafLittering += (s->value[OLD_BIOMASS_LEAVES] * s->value[LITTERFALL_RATE]) / GC_GDM * 1000 / settings->sizeCell;
				c->leaflitN += (s->value[OLD_BIOMASS_LEAVES] * s->value[LITTERFALL_RATE]) / GC_GDM * 1000 / settings->sizeCell /s->value[CN_LEAVES];
				c->leafBiomass += s->value[BIOMASS_FOLIAGE_tDM];

				/*recompute LAI after turnover*/
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
				}
				Log("++Lai from foliage or reserve = %f\n", s->value[LAI]);
				//marconi: 21/6 --> removed the 365 factor; using the dBiomass it the turnover factors may not be divided by 365 days of the year
				//			s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE] = s->value[DEL_ROOTS_FINE_CTEM] * (s->value[LEAVES_FINERTTOVER]);

				//soil_Log("fine root senescence time: from %d for %d days\n", c->doy, s->counter[FROM_SEN_TO_EOY] );
				if (s->value[BIOMASS_FINE_ROOT_tDM] > 0.0)
				{
					//fine root lost for turnover are considered lost at all?
					/*daily fine root turnover*/
					Log("****Fine root turnover****\n");
					//turnover of fine root
					s->value[BIOMASS_FINE_ROOT_tDM] -= Maximum(0, s->value[OLD_BIOMASS_FINE_ROOT_tDM] * (s->value[LEAVES_FINERTTOVER])/365.0);
					s->value[BIOMASS_FINE_ROOT_tDM] = 1.2;
					c->fineRootLittering += Maximum(0, s->value[OLD_BIOMASS_FINE_ROOT_tDM] * (s->value[LEAVES_FINERTTOVER])/365.0) /
							GC_GDM * 1000 / settings->sizeCell;
					c->fineRootlitN += Maximum(0,s->value[OLD_BIOMASS_FINE_ROOT_tDM] * (s->value[LEAVES_FINERTTOVER])/365.0) /
							GC_GDM * 1000 / settings->sizeCell  /s->value[CN_FINE_ROOTS];
				}
			}
			else
			{
				if (s->value[BIOMASS_FINE_ROOT_tDM] > 0.0)
				{
					//fineroots turnover following Marconi's idea
					Log("****Fine root turnover****\n");
					s->value[BIOMASS_FINE_ROOT_tDM] -=s->turnover->fineroot[c->dos % s->turnover->FINERTOVER];
					c->fineRootLittering +=  s->turnover->fineroot[c->dos % s->turnover->FINERTOVER] / GC_GDM * 1000 / settings->sizeCell;
					c->fineRootlitN += s->turnover->fineroot[c->dos % s->turnover->FINERTOVER]/ GC_GDM * 1000 / settings->sizeCell  /s->value[CN_FINE_ROOTS];
				}
				//leaves turnover following Marconi's idea:
				c->leafLittering += (s->turnover->leaves[c->dos % s->turnover->FINERTOVER]) / GC_GDM * 1000 / settings->sizeCell;
				s->value[BIOMASS_FOLIAGE_tDM] -= (s->turnover->leaves[c->dos % s->turnover->FINERTOVER]);
				c->leaflitN += (s->turnover->leaves[c->dos % s->turnover->FINERTOVER]) / GC_GDM * 1000 / settings->sizeCell /s->value[CN_LEAVES];
				c->leafBiomass = s->value[BIOMASS_FOLIAGE_tDM];

				/*recompute LAI after turnover*/
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_tDM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
				}
				Log("++Lai from foliage or reserve = %f\n", s->value[LAI]);
			}

			s->turnover->fineroot[c->dos % s->turnover->FINERTOVER] = Maximum(0, s->value[DEL_ROOTS_FINE]);
			s->turnover->leaves[c->dos % s->turnover->FINERTOVER] = Maximum(0, s->value[DEL_FOLIAGE]);
			//	soil_Log("\narray cell:\t %d \nfineroot:\t %g\nleaves:\t %g",c->dos % s->turnover->FINERTOVER,
			//			s->turnover->fineroot[c->dos % s->turnover->FINERTOVER], s->turnover->leaves[c->dos % s->turnover->FINERTOVER]);
			//soil_Log("\n\n prova resto: %d\n", dayOfTurnover);

		}
		else /* deciduous */
		{

		}


		/*		daily stem turnover
		Log("****Stem turnover****\n");
		//turnover of live stem wood to dead stem wood
		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[OLD_BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Stem Biomass = %f tDM/cell\n", s->value[OLD_BIOMASS_STEM_LIVE_WOOD]);
		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[OLD_BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);

		daily coarse root turnover
		Log("****Coarse root turnover****\n");
		//turnover of live coarse root wood to coarse root dead wood
		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Coarse root Biomass = %f tDM/cell\n", s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD]);
		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);

		daily stem branch turnover
		Log("****Stem branch turnover****\n");
		//turnover of live stem branch wood to dead stem branch wood
		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Stem branch Biomass = %f tDM/cell\n", s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD]);
		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);*/
	}
	//fixsergio: still used biome for stem, branches and coarse roots; change it with the new turnover version
	/*daily stem turnover*/
	Log("****Stem turnover****\n");
	//turnover of live stem wood to dead stem wood
	//Marconi version, 15/06: decreasing the proportional quantity for each day of simulation
	dStemBiomass = Maximum(0,(s->value[OLD_BIOMASS_STEM_LIVE_WOOD]) * s->value[LIVE_WOOD_TURNOVER] / 365.0);
	s->value[BIOMASS_STEM_LIVE_WOOD_tDM] -= dStemBiomass;
	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] += dStemBiomass;

	dStemBiomass = Maximum(0,(s->value[OLD_BIOMASS_STEM]) * s->value[SAPWOODTTOVER] / 365.0); //old biomass all stem
	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] -= dStemBiomass; //Marconi
	c->stemLittering =c->stemLittering + dStemBiomass / GC_GDM * 1000 / settings->sizeCell;
	c->stemlitN = c->stemlitN + dStemBiomass / GC_GDM * 1000 / settings->sizeCell /s->value[CN_DEAD_WOODS];
	s->value[BIOMASS_STEM_tDM] -= dStemBiomass;
	c->stemBiomass = s->value[BIOMASS_STEM_tDM];

	//		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
	//		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
	//		c->stemLittering += s->value[BIOMASS_STEM_LIVE_WOOD]  /20 * s->value[LIVE_WOOD_TURNOVER]/365;
	//		c->stemLittering = 0;
	//		c->stemlitN = 0;

	/*daily coarse root turnover*/
	Log("****Coarse root turnover****\n");
	dCoarseBiomass = Maximum(0.0, s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] / 365.0 * s->value[LIVE_WOOD_TURNOVER]);
	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] -= dCoarseBiomass;
	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] += dCoarseBiomass;

	dCoarseBiomass = Maximum(0.0, s->value[OLD_BIOMASS_ROOTS_COARSE] / 365.0 * s->value[COARSERTTOVER]);
	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] -= dCoarseBiomass;	//Marconi
	c->coarseRootLittering += dCoarseBiomass / GC_GDM * 1000 / settings->sizeCell;
	c->coarseRootlitN += dCoarseBiomass / GC_GDM * 1000 / settings->sizeCell /s->value[CN_DEAD_WOODS];
	s->value[BIOMASS_COARSE_ROOT_tDM] -= dStemBiomass;
	c->coarseRootBiomass = s->value[BIOMASS_COARSE_ROOT_tDM];


	//turnover of live coarse root wood to coarse root dead wood
	//		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Live Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
	//		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
	//		c->coarseRootLittering += s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]/20 * s->value[LIVE_WOOD_TURNOVER]/365;


	/*daily stem branch turnover*/
	Log("****Stem branch turnover****\n");
	//Marconi version, 15/06: decreasing the proportional quantity for each day of simulation
	dBranchBiomass = Maximum(0.0, s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD]/365.0 * s->value[LIVE_WOOD_TURNOVER]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] -= dBranchBiomass;
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] += dBranchBiomass;

	dBranchBiomass = Maximum(0.0, s->value[OLD_BIOMASS_BRANCH]/365.0 * s->value[BRANCHTTOVER]);
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] -= dBranchBiomass; //Marconi
	c->stemBrancLittering += dBranchBiomass / GC_GDM * 1000 / settings->sizeCell;
	c->stemBranclitN += dBranchBiomass / GC_GDM * 1000 / settings->sizeCell /s->value[CN_DEAD_WOODS];
	s->value[BIOMASS_BRANCH_tDM] -= dStemBiomass;

	c->stemBranchBiomass = s->value[BIOMASS_BRANCH_tDM];


	//turnover of live stem branch wood to dead stem branch wood
	//		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Live Stem branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	//		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
	Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);

	//
	//	s->turnover->fineroot[c->dos % s->turnover->FINERTOVER] = Maximum(0, s->value[DEL_ROOTS_FINE_CTEM]);
	//	s->turnover->leaves[c->dos % s->turnover->FINERTOVER] = Maximum(0, s->value[DEL_FOLIAGE]);
	////	soil_Log("\narray cell:\t %d \nfineroot:\t %g\nleaves:\t %g",c->dos % s->turnover->FINERTOVER,
	////			s->turnover->fineroot[c->dos % s->turnover->FINERTOVER], s->turnover->leaves[c->dos % s->turnover->FINERTOVER]);
	//	//soil_Log("\n\n prova resto: %d\n", dayOfTurnover);
}
