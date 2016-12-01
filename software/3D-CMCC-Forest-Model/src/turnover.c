/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "turnover.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

void turnover(species_t *const s)
{
	static double daily_live_wood_turnover_rate;

	logger(g_debug_log, "\n**TURNOVER**\n");

	/* compute live-wood turnover rate (ratio) */
	daily_live_wood_turnover_rate = (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL]);
	//logger(g_debug_log, "Daily live-wood turnover rate = %g (ratio)\n", daily_live_wood_turnover_rate);

	/*following BIOME_BGC turnover occurs only during growing season */
	if ( s->counter[VEG_UNVEG] == 1 )
	{
		//test useless if allocation.c uses * s->value[EFF_LIVE_TOTAL_WOOD_FRAC] to compute live wood
		/* daily stem turnover live to dead wood*/
		logger(g_debug_log, "\n****Stem turnover****\n");

		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD] = (s->value[C_TO_STEM] * daily_live_wood_turnover_rate);
		logger(g_debug_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);

		/* daily coarse root turnover live to dead wood */
		logger(g_debug_log, "\n****Coarse root turnover****\n");

		s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD] = (s->value[C_TO_COARSEROOT] * daily_live_wood_turnover_rate);
		logger(g_debug_log, "C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD]);

		/* daily branch turnover live to dead wood */
		logger(g_debug_log, "\n****Branch turnover****\n");

		s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD] = (s->value[C_TO_BRANCH] * daily_live_wood_turnover_rate);
		logger(g_debug_log, "C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD = %g tC/cell\n", s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD]);

		/* daily fruit turnover */
		if( s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0 )
		{
			s->value[C_FRUIT_TO_LITTER] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
			logger(g_debug_log, "C_FRUIT_TO_LITTER = %g tC/cell\n", s->value[C_FRUIT_TO_LITTER]);
		}
	}
	else
	{
		logger(g_debug_log,"No turnover\n");
	}
}

//void EOY_Turnover(species_t *s)
//{
//	static int counter_turnover;
//	static double c_to_stem;
//	static double c_to_branch;
//	static double c_to_coarseroot;
//
//	/* reset previous annual values */
//	if (s->counter[VEG_DAYS] == 1)
//	{
//		counter_turnover = 0;
//		c_to_stem = 0.0;
//		c_to_branch = 0.0;
//		c_to_coarseroot = 0.0;
//	}
//	/* upgrade current annual values */
//	if(s->value[C_TO_STEM] != 0.0 || s->value[C_TO_COARSEROOT] != 0.0 || s->value[C_TO_BRANCH] != 0.0)
//	{
//		counter_turnover++;
//		c_to_stem += s->value[C_TO_STEM];
//		c_to_branch += s->value[C_TO_BRANCH];
//		c_to_coarseroot += s->value[C_TO_COARSEROOT];
//	}
//	/*daily leaf turnover for evergreen leaffall*/
//	if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
//	{
//		logger(g_debug_log, "****leaf turnover for evergreen****\n");
//		s->value[LITTERFALL_RATE] = s->value[LEAF_FINEROOT_TURNOVER]/365.0;
//		logger(g_debug_log, "Daily litter fall rate = %g\n", s->value[LITTERFALL_RATE]);
//
//		/* reduce daily amount of leaf linearly */
//		s->value[LEAF_C] -= (s->value[LEAF_C] * s->value[LITTERFALL_RATE]);
//		s->value[C_TO_LITTER] = s->value[LEAF_C] * s->value[LITTERFALL_RATE];
//		logger(g_debug_log, "Leaf to litter = %g tC/cell/day\n", s->value[C_TO_LITTER]);
//		logger(g_debug_log, "Leaf poool after turnover = %g tC/cell\n", s->value[LEAF_C]);
//	}
//	/* end of veg period turnover */
//
//	if(s->counter[VEG_DAYS] == s->counter[DAY_VEG_FOR_LEAF_FALL])
//	{
//		/* annual stem turnover live to dead wood*/
//		//test turnover fraction should be divided for day_veg or for the day in which model allocates to stem?
//		logger(g_debug_log, "****Stem turnover****\n");
//		s->value[STEM_LIVE_WOOD_C] -= (s->value[STEM_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover));
//		logger(g_debug_log, "Annual Live Stem Biomass = %g tC/cell\n", s->value[STEM_LIVE_WOOD_C]);
//		s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
//		//s->value[STEM_DEAD_WOOD_C] += (s->value[C_TO_STEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL]));
//		logger(g_debug_log, "AnnualDead Stem Biomass = %g tC/cell\n", s->value[STEM_DEAD_WOOD_C]);
//
//		logger(g_debug_log, "Live stem + dead stem = %g tC/cell\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
//		logger(g_debug_log, "Total stem = %g tC/cell\n", s->value[STEM_C]);
//		logger(g_debug_log, "difference in stem = %g tC/cell\n", (s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]);
//		//ALESSIOR
//		CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]), >,eps)
//
//		/* annual coarse root turnover live to dead wood */
//		logger(g_debug_log, "****Coarse root turnover****\n");
//		s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[COARSE_ROOT_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover);
//		logger(g_debug_log, "Live Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
//		s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
//		//s->value[COARSE_ROOT_DEAD_WOOD_C] += (s->value[C_TO_COARSEROOT] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL]));
//		logger(g_debug_log, "Dead Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
//
//		/* adding a daily turnover that goes to litter */
//		//		s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSEROOT_TURNOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_debug_log, "Dead Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
//		//		s->value[COARSE_ROOT_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSEROOT_TURNOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_debug_log, "Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
//		//		s->value[C_COARSE_ROOT_TO_LITTER] = s->value[COARSE_ROOT_C] * s->value[COARSEROOT_TURNOVER]/s->counter[VEG_DAYS];
//		//		logger(g_debug_log, "Coarse root Biomass to litter = %g tC/cell\n", s->value[C_COARSE_ROOT_TO_LITTER]);
//
//		logger(g_debug_log, "Live coarse + dead coarse = %g tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
//		logger(g_debug_log, "Total coarse = %g tC/cell\n", s->value[COARSE_ROOT_C]);
//		logger(g_debug_log, "difference in coarse root = %g tC/cell\n", (s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]);
//		//ALESSIOR
//		CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]), >,eps)
//
//
//		/* annual branch turnover live to dead wood */
//		logger(g_debug_log, "****Stem branch turnover****\n");
//		s->value[BRANCH_LIVE_WOOD_C] -= s->value[BRANCH_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover);
//		logger(g_debug_log, "Live Stem branch Biomass = %g tC/cell\n", s->value[BRANCH_LIVE_WOOD_C]);
//		s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
//		//s->value[BRANCH_DEAD_WOOD_C] += (s->value[C_TO_BRANCH] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL]));
//		logger(g_debug_log, "Dead Stem Biomass = %g tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
//
//		/* adding a daily turnover that goes to litter */
//		//		s->value[BRANCH_DEAD_WOOD_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_debug_log, "Dead Branch Biomass = %g tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
//		//		s->value[BRANCH_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_debug_log, "Branch Biomass = %g tC/cell\n", s->value[BRANCH_C]);
//		//		s->value[C_BRANCH_TO_LITTER] = s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS];
//		//		logger(g_debug_log, "Branch Biomass to litter = %g tC/cell\n", s->value[C_BRANCH_TO_LITTER]);
//
//		logger(g_debug_log, "Live branch + dead branch = %g tC/cell\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
//		logger(g_debug_log, "Total branch = %g tC/cell\n", s->value[BRANCH_C]);
//		logger(g_debug_log, "difference in branch = %g tC/cell\n", (s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]);
//
//		CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]), >,eps);
//	}
//	/* daily fruit turnover */
//	if(s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0)
//	{
//		s->value[FRUIT_C] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
//	}
//}
