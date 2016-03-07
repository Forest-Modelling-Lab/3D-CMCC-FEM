/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Get_turnover (SPECIES *s, CELL *c, int DaysInMonth, int height)
{
	Log("\nGET_DAILY_TURNOVER\n\n");

	/*following biome turnover occurs only during growing season*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		//fixme see if add foliage turnover!!!!!

		/*daily leaf turnover for EVERGREEN*/
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


		//fine root lost for turnover are considered lost at all?
		/*daily fine root turnover*/
		//Log("****Fine root turnover****\n");
		//turnover of fine root
		if (s->value[BIOMASS_ROOTS_FINE] > 0.0)
		{
			s->value[BIOMASS_ROOTS_FINE] -= (s->value[BIOMASS_ROOTS_FINE] * (s->value[LEAVES_FINERTTOVER]/365.0));
			Log("Daily fine root turnover = %f tDM/cell\n", s->value[BIOMASS_ROOTS_FINE] * (s->value[LEAVES_FINERTTOVER]/365.0));
			Log("Fine root Biomass after turnover = %f\n", s->value[BIOMASS_FOLIAGE]);
		}

		/*daily stem turnover*/
		//Log("****Stem turnover****\n");
		//turnover of live stem wood to dead stem wood
		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live stem wood passing to dead stem wood = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);

		/*daily coarse root turnover*/
		//Log("****Coarse root turnover****\n");
		//turnover of live coarse root wood to coarse root dead wood
		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Coarse root Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);

		/*daily stem branch turnover*/
		//Log("****Stem branch turnover****\n");
		//turnover of live stem branch wood to dead stem branch wood
		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Stem branch Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
	}
}
