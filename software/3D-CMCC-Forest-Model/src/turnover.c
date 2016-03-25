/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Turnover(SPECIES *s)
{
	Log("\n**DAILY_TURNOVER**\n\n");

	/*following biome turnover occurs only during growing season*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		/*daily leaf turnover for DECIDUOUS, leaffall*/
		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			Log("****leaf turnover for evergreen****\n");
			s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
			Log("Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);

			//new
			/* reduce daily amount of leaf linearly */
			s->value[LEAF_C] -= (s->value[LEAF_C] * s->value[LITTERFALL_RATE]);
			s->value[C_TO_LITTER] = s->value[LEAF_C] * s->value[LITTERFALL_RATE];
			Log("Leaf to litter = %f tC/cell/day\n", s->value[C_TO_LITTER]);
			Log("Leaf poool after turnover = %f tC/cell\n", s->value[LEAF_C]);
		}
		/* daily stem turnover */
		Log("****Stem turnover****\n");
		s->value[STEM_LIVE_WOOD_C] -= (s->value[C_TO_STEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Daily live stem wood passing to dead stem wood = %f tC/cell\n", s->value[LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Live Stem Biomass = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C]);
		s->value[STEM_DEAD_WOOD_C] += (s->value[C_TO_STEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Stem Biomass = %f tC/cell\n", s->value[STEM_DEAD_WOOD_C]);

		if (fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C])>1e-1)
		{
			Log("Live stem + dead stem = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
			Log("Total stem = %f tC/cell\n", s->value[STEM_C]);
			exit(1);
		}

		/*daily coarse root turnover*/
		Log("****Coarse root turnover****\n");
		s->value[COARSE_ROOT_LIVE_WOOD_C] -= (s->value[C_TO_COARSEROOT] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Daily live coarse root wood passing to dead wood = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Live Coarse root Biomass = %f tC/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
		s->value[COARSE_ROOT_DEAD_WOOD_C] += (s->value[C_TO_COARSEROOT] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

		if (fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C])>1e-4)
		{
			Log("Live coarse + dead coarse = %f tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
			Log("Total coarse = %f tC/cell\n", s->value[COARSE_ROOT_C]);
			exit(1);
		}

		/*daily stem branch turnover*/
		Log("****Stem branch turnover****\n");
		s->value[BRANCH_LIVE_WOOD_C] -= (s->value[C_TO_BRANCH] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Daily live stem branch wood passing to dead stem branch wood = %f tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Live Stem branch Biomass = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C]);
		s->value[BRANCH_DEAD_WOOD_C] += (s->value[C_TO_BRANCH] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Stem Biomass = %f tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);

		if (fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C])>1e-4)
		{
			Log("Live branch + dead branch = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
			Log("Total branch = %f\n tC/cell", s->value[BRANCH_C]);
			exit(1);
		}
	}
}
