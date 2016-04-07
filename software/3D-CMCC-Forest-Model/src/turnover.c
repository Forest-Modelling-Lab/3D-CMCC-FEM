/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Turnover(SPECIES *s, CELL *c)
{
	static double daily_live_turnover_rate;

	daily_live_turnover_rate = (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]);

	/*following biome turnover occurs only during growing season */
	if (s->counter[VEG_UNVEG] == 1)
	{
		Log("\n*DAILY_TURNOVER*\n\n");
		/*daily leaf turnover for evergreen */
		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
		{
			Log("****leaf turnover for evergreen****\n");
			s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
			Log("Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);

			/* reduce daily amount of leaf linearly */
			s->value[LEAF_C] -= (s->value[LEAF_C] * s->value[LITTERFALL_RATE]);
			s->value[C_TO_LITTER] = s->value[LEAF_C] * s->value[LITTERFALL_RATE];
			Log("Leaf to litter = %f tC/cell/day\n", s->value[C_TO_LITTER]);
			Log("Leaf poool after turnover = %f tC/cell\n", s->value[LEAF_C]);
		}
		/* daily stem turnover live to dead wood*/
		Log("Live:Dead turnover rate = %f tC/day\n", daily_live_turnover_rate);
		Log("****Stem turnover****\n");
		Log("Previous Live Stem Biomass = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C]);
		Log("Live Stem Biomass to turnover= %.10f tC/cell/day\n", s->value[STEM_LIVE_WOOD_C] * daily_live_turnover_rate);
		s->value[STEM_LIVE_WOOD_C] -= (s->value[C_TO_STEM] * daily_live_turnover_rate);
		//s->value[STEM_LIVE_WOOD_C] -= (s->value[STEM_LIVE_WOOD_C] * daily_live_turnover_rate);
		Log("Live Stem Biomass = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C]);
		s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
		Log("Dead Stem Biomass = %f tC/cell\n", s->value[STEM_DEAD_WOOD_C]);
		Log("Live stem + dead stem = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
		Log("Total stem = %f tC/cell\n", s->value[STEM_C]);
		CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]), >1e-4)
		//exit(1);

		/* daily coarse root turnover live to dead wood */
		Log("****Coarse root turnover****\n");
		Log("Previous Live Coarse Root  Biomass = %f tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
		s->value[COARSE_ROOT_LIVE_WOOD_C] -= (s->value[C_TO_COARSEROOT] * daily_live_turnover_rate);
		//s->value[COARSE_ROOT_LIVE_WOOD_C] -= (s->value[COARSE_ROOT_LIVE_WOOD_C] * daily_live_turnover_rate);
		Log("Live Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
		s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
		Log("Dead Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

		/* adding a daily turnover that goes to litter */
		//		s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Dead Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
		//		s->value[COARSE_ROOT_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
		//		s->value[C_COARSE_ROOT_TO_LITTER] = s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS];
		//		Log("Coarse root Biomass to litter = %f tC/cell\n", s->value[C_COARSE_ROOT_TO_LITTER]);

		Log("Live coarse + dead coarse = %f tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
		Log("Total coarse = %f tC/cell\n", s->value[COARSE_ROOT_C]);
		Log("difference in coarse root = %f tC/cell\n", (s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]);
		CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]), >1e-4)
		//exit(1);

		/* daily branch turnover live to dead wood */
		Log("****Branch turnover****\n");
		Log("Previous Live Branch  Biomass = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C]);
		s->value[BRANCH_LIVE_WOOD_C] -= (s->value[C_TO_BRANCH] * daily_live_turnover_rate);
		//s->value[BRANCH_LIVE_WOOD_C] -= (s->value[BRANCH_LIVE_WOOD_C] * daily_live_turnover_rate);
		Log("Live Stem branch Biomass = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C]);
		s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
		Log("Dead Stem Biomass = %f tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
		//exit(1);

		/* adding a daily turnover that goes to litter */
		//		s->value[BRANCH_DEAD_WOOD_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Dead Branch Biomass = %f tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
		//		s->value[BRANCH_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Branch Biomass = %f tC/cell\n", s->value[BRANCH_C]);
		//		s->value[C_BRANCH_TO_LITTER] = s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS];
		//		Log("Branch Biomass to litter = %f tC/cell\n", s->value[C_BRANCH_TO_LITTER]);

		Log("Live branch + dead branch = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
		Log("Total branch = %f\n tC/cell", s->value[BRANCH_C]);
		Log("difference in branch = %f tC/cell\n", (s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]);
		CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]), >1e-4);

		/* daily fruit turnover */
		if(s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0)
		{
			s->value[FRUIT_C] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
		}
	}
}

void EOY_Turnover(SPECIES *s)
{
	static int counter_turnover;
	static double c_to_stem;
	static double c_to_branch;
	static double c_to_coarseroot;

	/* reset previous annual values */
	if (s->counter[VEG_DAYS] == 1)
	{
		counter_turnover = 0.0;
		c_to_stem = 0.0;
		c_to_branch = 0.0;
		c_to_coarseroot = 0.0;
	}
	/* upgrade current annual values */
	if(s->value[C_TO_STEM] != 0.0 || s->value[C_TO_COARSEROOT] != 0.0 || s->value[C_TO_BRANCH] != 0.0)
	{
		counter_turnover++;
		c_to_stem += s->value[C_TO_STEM];
		c_to_branch += s->value[C_TO_BRANCH];
		c_to_coarseroot += s->value[C_TO_COARSEROOT];
	}
	/*daily leaf turnover for evergreen leaffall*/
	if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
	{
		Log("****leaf turnover for evergreen****\n");
		s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
		Log("Daily litter fall rate = %f\n", s->value[LITTERFALL_RATE]);

		/* reduce daily amount of leaf linearly */
		s->value[LEAF_C] -= (s->value[LEAF_C] * s->value[LITTERFALL_RATE]);
		s->value[C_TO_LITTER] = s->value[LEAF_C] * s->value[LITTERFALL_RATE];
		Log("Leaf to litter = %f tC/cell/day\n", s->value[C_TO_LITTER]);
		Log("Leaf poool after turnover = %f tC/cell\n", s->value[LEAF_C]);
	}
	/* end of veg period turnover */

	if(s->counter[VEG_DAYS] == s->counter[DAY_VEG_FOR_LITTERFALL_RATE])
	{
		/* annual stem turnover live to dead wood*/
		//test turnover fraction should be divided for day_veg or for the day in which model allocates to stem?
		Log("****Stem turnover****\n");
		s->value[STEM_LIVE_WOOD_C] -= (s->value[STEM_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover));
		Log("prova %f\n", (c_to_stem * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover)));
		Log("Annual Live Stem Biomass = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C]);
		s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
		//s->value[STEM_DEAD_WOOD_C] += (s->value[C_TO_STEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("AnnualDead Stem Biomass = %f tC/cell\n", s->value[STEM_DEAD_WOOD_C]);

		Log("Live stem + dead stem = %f tC/cell\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
		Log("Total stem = %f tC/cell\n", s->value[STEM_C]);
		Log("difference in stem = %f tC/cell\n", (s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]);
		//ALESSIOR
		CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]), >1e-4)

		/* annual coarse root turnover live to dead wood */
		Log("****Coarse root turnover****\n");
		s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[COARSE_ROOT_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover);
		Log("Live Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
		s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
		//s->value[COARSE_ROOT_DEAD_WOOD_C] += (s->value[C_TO_COARSEROOT] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);

		/* adding a daily turnover that goes to litter */
		//		s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Dead Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
		//		s->value[COARSE_ROOT_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Coarse root Biomass = %f tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
		//		s->value[C_COARSE_ROOT_TO_LITTER] = s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS];
		//		Log("Coarse root Biomass to litter = %f tC/cell\n", s->value[C_COARSE_ROOT_TO_LITTER]);

		Log("Live coarse + dead coarse = %f tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
		Log("Total coarse = %f tC/cell\n", s->value[COARSE_ROOT_C]);
		Log("difference in coarse root = %f tC/cell\n", (s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]);
		//ALESSIOR
		CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]), >1e-4)


		/* annual branch turnover live to dead wood */
		Log("****Stem branch turnover****\n");
		s->value[BRANCH_LIVE_WOOD_C] -= s->value[BRANCH_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover);
		Log("Live Stem branch Biomass = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C]);
		s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
		//s->value[BRANCH_DEAD_WOOD_C] += (s->value[C_TO_BRANCH] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
		Log("Dead Stem Biomass = %f tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);

		/* adding a daily turnover that goes to litter */
		//		s->value[BRANCH_DEAD_WOOD_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Dead Branch Biomass = %f tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
		//		s->value[BRANCH_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
		//		Log("Branch Biomass = %f tC/cell\n", s->value[BRANCH_C]);
		//		s->value[C_BRANCH_TO_LITTER] = s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS];
		//		Log("Branch Biomass to litter = %f tC/cell\n", s->value[C_BRANCH_TO_LITTER]);

		Log("Live branch + dead branch = %f tC/cell\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
		Log("Total branch = %f\n tC/cell", s->value[BRANCH_C]);
		Log("difference in branch = %f tC/cell\n", (s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]);
		//ALESSIOR
		CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]), >1e-4);
	}
	/* daily fruit turnover */
	if(s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0)
	{
		s->value[FRUIT_C] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
	}
}
