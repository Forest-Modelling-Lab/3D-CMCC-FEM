/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "turnover.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void Turnover(species_t *const s, cell_t *const c)
{
	static double daily_live_wood_turnover_rate;
	static double daily_leaf_fineroot_turnover_rate;
	//fixme fixme fixme fixme
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	logger(g_log, "\n*DAILY_TURNOVER*\n");

	/* compute rates */
	/* compute leaf and fine root turnover rate (ratio) */
	daily_leaf_fineroot_turnover_rate = s->value[LEAVES_FINERTTOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE];
	logger(g_log, "Daily leaf fine root turnover rate = %g (ratio)\n", daily_leaf_fineroot_turnover_rate);
	/* compute live-wood turnover rate (ratio) */
	daily_live_wood_turnover_rate = (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	logger(g_log, "Daily live-wood turnover rate = %g (ratio)\n", daily_live_wood_turnover_rate);

	/*following BIOME_BGC turnover occurs only during growing season */
	if (s->counter[VEG_UNVEG] == 1)
	{
		/*daily leaf turnover for evergreen */
		if (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2)
		{
			logger(g_log, "****leaf and fine root turnover for evergreen****\n");

			logger(g_log, "Leaf pool before turnover = %g tC/cell\n", s->value[LEAF_C]);
			logger(g_log, "Fine root pool before turnover = %g tC/cell\n", s->value[FINE_ROOT_C]);

			/* compute daily amount of leaf and fine root to remove */
			s->value[C_LEAF_TO_LITTER] = (s->value[LEAF_C] * daily_leaf_fineroot_turnover_rate);
			logger(g_log, "Daily leaf turnover = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);
			s->value[C_FINE_ROOT_TO_LITTER] = (s->value[FINE_ROOT_C] * daily_leaf_fineroot_turnover_rate);
			logger(g_log, "Daily fine root turnover = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_LITTER]);
			logger(g_log, "Daily biomass turnover to litter before retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);

			/* compute daily amount of C to re-translocate before remove leaf and fine root */
			s->value[C_LEAF_TO_RESERVE] = s->value[C_LEAF_TO_LITTER] * fraction_to_retransl;
			logger(g_log, "RETRANSL_C_LEAF_TO_RESERVE = %g tC/cell/day\n", s->value[C_LEAF_TO_RESERVE]);
			s->value[C_FINEROOT_TO_RESERVE] = s->value[C_FINE_ROOT_TO_LITTER] * fraction_to_retransl;
			logger(g_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %g tC/cell/day\n", s->value[C_FINEROOT_TO_RESERVE]);

			/* update considering retranslocation */
			s->value[C_LEAF_TO_LITTER] -= s->value[C_LEAF_TO_RESERVE];
			logger(g_log, "Daily leaf turnover after retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);
			s->value[C_FINE_ROOT_TO_LITTER] -= s->value[C_FINEROOT_TO_RESERVE];
			logger(g_log, "Daily fine root turnover after retranslocation = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_LITTER]);

			/**/
			s->value[C_TO_LEAF] -= s->value[C_LEAF_TO_LITTER];
			s->value[C_TO_FINEROOT] -= s->value[C_FINE_ROOT_TO_LITTER];

			/* considering that both leaf and fine root contribute to the litter pool */
			s->value[C_TO_LITTER] = (s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);
			logger(g_log, "biomass to litter after retranslocation = %g tC/cell/day\n", s->value[C_TO_LITTER]);

		}

		/* daily stem turnover live to dead wood*/
		logger(g_log, "\n****Stem turnover****\n");

		s->value[C_STEM_LIVEWOOD_TO_DEADWOOD] = (s->value[C_TO_STEM] * daily_live_wood_turnover_rate);
		logger(g_log, "C_STEM_LIVEWOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_STEM_LIVEWOOD_TO_DEADWOOD]);

		/* daily coarse root turnover live to dead wood */
		logger(g_log, "\n****Coarse root turnover****\n");

		s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD] = (s->value[C_TO_COARSEROOT] * daily_live_wood_turnover_rate);
		logger(g_log, "C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD = %g tC/cell\n", s->value[C_COARSEROOT_LIVE_WOOD_TO_DEADWOOD]);

		/* daily branch turnover live to dead wood */
		logger(g_log, "\n****Branch turnover****\n");

		s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD] = (s->value[C_TO_BRANCH] * daily_live_wood_turnover_rate);
		logger(g_log, "C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD = %g tC/cell\n", s->value[C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD]);

		/* daily fruit turnover */
		if(s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0)
		{
			s->value[C_FRUIT_TO_LITTER] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
			logger(g_log, "C_FRUIT_TO_LITTER = %g tC/cell\n", s->value[C_FRUIT_TO_LITTER]);
		}
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
//		logger(g_log, "****leaf turnover for evergreen****\n");
//		s->value[LITTERFALL_RATE] = s->value[LEAVES_FINERTTOVER]/365.0;
//		logger(g_log, "Daily litter fall rate = %g\n", s->value[LITTERFALL_RATE]);
//
//		/* reduce daily amount of leaf linearly */
//		s->value[LEAF_C] -= (s->value[LEAF_C] * s->value[LITTERFALL_RATE]);
//		s->value[C_TO_LITTER] = s->value[LEAF_C] * s->value[LITTERFALL_RATE];
//		logger(g_log, "Leaf to litter = %g tC/cell/day\n", s->value[C_TO_LITTER]);
//		logger(g_log, "Leaf poool after turnover = %g tC/cell\n", s->value[LEAF_C]);
//	}
//	/* end of veg period turnover */
//
//	if(s->counter[VEG_DAYS] == s->counter[DAY_VEG_FOR_LITTERFALL_RATE])
//	{
//		/* annual stem turnover live to dead wood*/
//		//test turnover fraction should be divided for day_veg or for the day in which model allocates to stem?
//		logger(g_log, "****Stem turnover****\n");
//		s->value[STEM_LIVE_WOOD_C] -= (s->value[STEM_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover));
//		logger(g_log, "Annual Live Stem Biomass = %g tC/cell\n", s->value[STEM_LIVE_WOOD_C]);
//		s->value[STEM_DEAD_WOOD_C] = s->value[STEM_C] - s->value[STEM_LIVE_WOOD_C];
//		//s->value[STEM_DEAD_WOOD_C] += (s->value[C_TO_STEM] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		logger(g_log, "AnnualDead Stem Biomass = %g tC/cell\n", s->value[STEM_DEAD_WOOD_C]);
//
//		logger(g_log, "Live stem + dead stem = %g tC/cell\n", s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]);
//		logger(g_log, "Total stem = %g tC/cell\n", s->value[STEM_C]);
//		logger(g_log, "difference in stem = %g tC/cell\n", (s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]);
//		//ALESSIOR
//		CHECK_CONDITION(fabs((s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C]) - s->value[STEM_C]), >1e-4)
//
//		/* annual coarse root turnover live to dead wood */
//		logger(g_log, "****Coarse root turnover****\n");
//		s->value[COARSE_ROOT_LIVE_WOOD_C] -= s->value[COARSE_ROOT_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover);
//		logger(g_log, "Live Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
//		s->value[COARSE_ROOT_DEAD_WOOD_C] = s->value[COARSE_ROOT_C] - s->value[COARSE_ROOT_LIVE_WOOD_C];
//		//s->value[COARSE_ROOT_DEAD_WOOD_C] += (s->value[C_TO_COARSEROOT] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		logger(g_log, "Dead Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
//
//		/* adding a daily turnover that goes to litter */
//		//		s->value[COARSE_ROOT_DEAD_WOOD_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_log, "Dead Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
//		//		s->value[COARSE_ROOT_C] -= (s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_log, "Coarse root Biomass = %g tC/cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
//		//		s->value[C_COARSE_ROOT_TO_LITTER] = s->value[COARSE_ROOT_C] * s->value[COARSERTTOVER]/s->counter[VEG_DAYS];
//		//		logger(g_log, "Coarse root Biomass to litter = %g tC/cell\n", s->value[C_COARSE_ROOT_TO_LITTER]);
//
//		logger(g_log, "Live coarse + dead coarse = %g tC/cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]);
//		logger(g_log, "Total coarse = %g tC/cell\n", s->value[COARSE_ROOT_C]);
//		logger(g_log, "difference in coarse root = %g tC/cell\n", (s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]);
//		//ALESSIOR
//		CHECK_CONDITION(fabs((s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C]) - s->value[COARSE_ROOT_C]), >1e-4)
//
//
//		/* annual branch turnover live to dead wood */
//		logger(g_log, "****Stem branch turnover****\n");
//		s->value[BRANCH_LIVE_WOOD_C] -= s->value[BRANCH_LIVE_WOOD_C] * (s->value[LIVE_WOOD_TURNOVER]/(int)counter_turnover);
//		logger(g_log, "Live Stem branch Biomass = %g tC/cell\n", s->value[BRANCH_LIVE_WOOD_C]);
//		s->value[BRANCH_DEAD_WOOD_C] = s->value[BRANCH_C] - s->value[BRANCH_LIVE_WOOD_C];
//		//s->value[BRANCH_DEAD_WOOD_C] += (s->value[C_TO_BRANCH] * (s->value[LIVE_WOOD_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LITTERFALL_RATE]));
//		logger(g_log, "Dead Stem Biomass = %g tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
//
//		/* adding a daily turnover that goes to litter */
//		//		s->value[BRANCH_DEAD_WOOD_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_log, "Dead Branch Biomass = %g tC/cell\n", s->value[BRANCH_DEAD_WOOD_C]);
//		//		s->value[BRANCH_C] -= (s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS]);
//		//		logger(g_log, "Branch Biomass = %g tC/cell\n", s->value[BRANCH_C]);
//		//		s->value[C_BRANCH_TO_LITTER] = s->value[C_TO_BRANCH] * s->value[BRANCHTTOVER]/s->counter[VEG_DAYS];
//		//		logger(g_log, "Branch Biomass to litter = %g tC/cell\n", s->value[C_BRANCH_TO_LITTER]);
//
//		logger(g_log, "Live branch + dead branch = %g tC/cell\n", s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]);
//		logger(g_log, "Total branch = %g tC/cell\n", s->value[BRANCH_C]);
//		logger(g_log, "difference in branch = %g tC/cell\n", (s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]);
//
//		CHECK_CONDITION(fabs((s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C]) - s->value[BRANCH_C]), >1e-4);
//	}
//	/* daily fruit turnover */
//	if(s->value[PHENOLOGY] == 1.2 && s->value[FRUIT_C] != 0.0)
//	{
//		s->value[FRUIT_C] -= (s->value[FRUIT_C] * (1 / s->value[CONES_LIFE_SPAN]));
//	}
//}
