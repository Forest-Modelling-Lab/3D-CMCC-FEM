/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Turnover (SPECIES *s, CELL *c, int DaysInMonth, int height)
{
	Log("\nGET_DAILY_TURNOVER\n\n");

	/*following biome turnover occurs only during growing season*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		//fixme see if add foliage turnover!!!!!

		/*daily leaf turnover for EVERGREEN, leaffall for deciduous is managed by "leaffall" function*/
		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
		{
			Log("****leaf turnover for evergreen****\n");
			s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
			Log("Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);
			Log("Daily litter fall = %f\n", s->value[LITTERFALL_RATE]);
			s->value[BIOMASS_FOLIAGE] -= (s->value[BIOMASS_FOLIAGE] * s->value[LITTERFALL_RATE]);
			Log("Foliage Biomass after turnover = %f\n", s->value[BIOMASS_FOLIAGE]);
			s->value[DAILY_DEL_LITTER] += (s->value[BIOMASS_FOLIAGE] * s->value[LITTERFALL_RATE]);
			/*recompute LAI after turnover*/
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
			}
			Log("++Lai from foliage or reserve = %f\n", s->value[LAI]);

		}


		/*daily stem turnover*/
		//Log("****Stem turnover****\n");
		//turnover of live stem wood to dead stem wood
//		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
//		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);

		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[DEL_STEMS] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);

		if (fabs((s->value[BIOMASS_STEM_LIVE_WOOD] + s->value[BIOMASS_STEM_DEAD_WOOD]) - s->value[BIOMASS_STEM])>1e-1)
		{
			Log("Live stem + dead stem = %f\n", s->value[BIOMASS_STEM_LIVE_WOOD] + s->value[BIOMASS_STEM_DEAD_WOOD]);
			Log("Total stem = %f\n", s->value[BIOMASS_STEM]);
			exit(1);
		}

		/*daily coarse root turnover*/
		//Log("****Coarse root turnover****\n");
		//turnover of live coarse root wood to coarse root dead wood
//		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Live Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
//		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);

		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[DEL_ROOTS_COARSE_CTEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Live Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);

		if (fabs((s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] + s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]) - s->value[BIOMASS_ROOTS_COARSE])>1e-4)
		{
			Log("Live coarse + dead coarse = %f\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] + s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
			Log("Total coarse = %f\n", s->value[BIOMASS_ROOTS_COARSE]);
			exit(1);
		}

		/*daily stem branch turnover*/
		//Log("****Stem branch turnover****\n");
		//turnover of live stem branch wood to dead stem branch wood
//		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Live Stem branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
//		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[DEL_BB] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Live Stem branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);

		if (fabs((s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] + s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]) - s->value[BIOMASS_BRANCH])>1e-4)
		{
			Log("Live branch + dead branch = %f\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] + s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
			Log("Total branch = %f\n", s->value[BIOMASS_BRANCH]);
			exit(1);
		}

		/* LITTERFALL COMPUTATION AT CELL LEVEL */
		c->daily_litterfall += s->value[DAILY_DEL_LITTER];
		Log("Daily Litterfall = %f\n", c->daily_litterfall);
		c->monthly_litterfall += c->daily_litterfall;
		Log("Monthly Litterfall = %f\n", c->monthly_litterfall);
		c->annual_litterfall += c->daily_litterfall;
		Log("Annual Litterfall = %f\n", c->annual_litterfall);
	}
}
