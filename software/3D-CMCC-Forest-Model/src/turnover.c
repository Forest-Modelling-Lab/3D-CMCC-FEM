/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Get_turnover (SPECIES *const s, int DaysInMonth)
{
	//int oldWs;
	//int oldWrf;
	//int oldWrc;
	/*
	float fine_root_turnover_rate;
	float coarse_root_turnover_rate;
	float stem_turnover_rate;
	float live_wood_turnover;
	 */

	Log("****DAILY TURNOVER****\n");

	/*
	fine_root_turnover_rate = s->value[FINERTTOVER];
	coarse_root_turnover_rate = s->value[COARSERTTOVER];
	stem_turnover_rate = s->value[SAPWOODTTOVER];
	live_wood_turnover = s->value[LIVE_WOOD_TURNOVER];
	 */
	/*
	Log("****fine root turnover****\n");
	Log("Fine Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
	s->value[DEL_ROSS_FINE] =  s->value[BIOMASS_ROOTS_FINE_CTEM] * fine_root_turnover_rate;
	Log("Average monthly fine Root turnover rate  = %g\n",fine_root_turnover_rate);
	Log("Fine Root Biomass lost = %g tDM/area\n", s->value[DEL_ROSS_FINE]);

	if (s->value[DEL_ROSS_FINE] > s->value[BIOMASS_ROOTS_FINE_CTEM])
	{
		Log("ATTENTION fine root turnover exceed DEL_WR !!! \n");
		s->value[BIOMASS_ROOTS_FINE_CTEM] -= s->value[DEL_ROOTS_FINE_CTEM];
	}

	//oldWrf = s->value[BIOMASS_ROOTS_FINE_CTEM];

	// Fine Root Biomass less root turnover

	s->value[BIOMASS_ROOTS_FINE_CTEM] -= s->value[DEL_ROSS_FINE];
	Log("Fine Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
	 */

	// fixme remove if use live wood turnover that takes into account all live tissues
	//fixme do not use 3-PG parameters value (it takes into account fine + coarse)
	/*
	Log("****   coarse root turnover****\n");
	Log("Coarse Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);
	s->value[DEL_ROSS_COARSE] =  s->value[BIOMASS_ROOTS_COARSE_CTEM] * coarse_root_turnover_rate;
	Log("Average monthly coarse Root turnover rate  = %g\n", coarse_root_turnover_rate);
	Log("Coarse Root Biomass lost = %g tDM/ha\n", s->value[DEL_ROSS_COARSE]);


	if (s->value[DEL_ROSS_COARSE] > s->value[BIOMASS_ROOTS_COARSE_CTEM])
	{
		Log("ATTENTION coarse root turnover exceed DEL_WR !!! \n");
		s->value[BIOMASS_ROOTS_COARSE_CTEM] -= s->value[DEL_ROOTS_COARSE_CTEM];
	}
	 */
	//oldWrc = s->value[BIOMASS_ROOTS_COARSE_CTEM];


	/*following biome turnover occurs only in vegetative period*/
	if (s->counter[VEG_UNVEG] == 1)
	{
		//fixme see if add foliage turnover!!!!!

		/*daily leaf turnover*/
		Log("****leaf turnover****\n");
		Log("compute it??\n");

		//fine root lost for turnover are considered lost at all?
		/*daily fine root turnover*/
		Log("****Fine root turnover****\n");
		//turnover of fine root
		s->value[BIOMASS_ROOTS_FINE_CTEM] -= (s->value[BIOMASS_ROOTS_FINE_CTEM] * (s->value[FINERTTOVER]/365.0));
		Log("Daily fine root turnover = %g tDM/cell\n", s->value[BIOMASS_ROOTS_FINE_CTEM] * (s->value[FINERTTOVER]/365.0));
		Log("Live Coarse root Biomass = %g tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);



		/*daily stem turnover*/
		Log("****Stem turnover****\n");
		//turnover of live stem wood to dead stem wood
		s->value[BIOMASS_STEM_LIVE_WOOD] -= (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live stem wood passing to dead stem wood = %g tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Stem Biomass = %g tDM/cell\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
		s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[BIOMASS_STEM_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Stem Biomass = %g tDM/cell\n", s->value[BIOMASS_STEM_DEAD_WOOD]);

		/*daily coarse root turnover*/
		Log("****Coarse root turnover****\n");
		//turnover of live coarse root wood to coarse root dead wood
		s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] -= (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live coarse root wood passing to dead wood = %g tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Coarse root Biomass = %g tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
		s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Coarse root Biomass = %g tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);

		/*daily stem branch turnover*/
		Log("****Stem branch turnover****\n");
		//turnover of live stem branch wood to dead stem branch wood
		s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] -= (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Daily live stem branch wood passing to dead stem branch wood = %g tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Live Stem branch Biomass = %g tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
		s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] * (s->value[LIVE_WOOD_TURNOVER]/365.0));
		Log("Dead Stem Biomass = %g tDM/cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
	}


	/*
    s->value[DEL_ROSS_STEM] =  s->value[BIOMASS_STEM_CTEM]* stem_turnover_rate;
    Log("Average monthly Stem turnover rate  = %g\n", stem_turnover_rate);
    Log("Stem Biomass lost = %g tDM/ha\n", s->value[DEL_ROSS_STEM]);

    if (s->value[DEL_ROSS_STEM] > s->value[BIOMASS_STEM_CTEM])
    {
         Log("ATTENTION stem turnover exceed DEL_WS !!! \n");
         s->value[BIOMASS_STEM_CTEM] -= s->value[DEL_ROSS_STEM];
    }

    //oldWs = s->value[BIOMASS_STEM_CTEM];

    // Fine Root Biomass less root turnover

    s->value[BIOMASS_STEM_CTEM] -= s->value[DEL_ROSS_STEM];
	 */


	//fixme new turnover for stem
	//turnover must be only for live stem??? (in tech guide it says that i for live and dead)


}
