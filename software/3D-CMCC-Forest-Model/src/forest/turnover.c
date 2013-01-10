/*turnover.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"



void Get_turnover (SPECIES *const s)
{
	//int oldWs;
	//int oldWrf;
	//int oldWrc;

	Log("****TURNOVER****\n");
	Log("****fine root turnover****\n");
	Log("Fine Root Biomass  = %g tDM/area\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
	s->value[DEL_ROSS_FINE] =  s->value[BIOMASS_ROOTS_FINE_CTEM] * s->value[FINERTTOVER];
	Log("Average yearly fine Root turnover rate  = %g\n", s->value[FINERTTOVER]);
	Log("Fine Root Biomass lost = %g tDM/area\n", s->value[DEL_ROSS_FINE]);

	if (s->value[DEL_ROSS_FINE] > s->value[BIOMASS_ROOTS_FINE_CTEM])
	{
		Log("ATTENTION fine root turnover exceed DEL_WR !!! \n");
		s->value[BIOMASS_ROOTS_FINE_CTEM] -= s->value[DEL_ROOTS_FINE_CTEM];
	}

	//oldWrf = s->value[BIOMASS_ROOTS_FINE_CTEM];

	// Fine Root Biomass less root turnover

	s->value[BIOMASS_ROOTS_FINE_CTEM] -= s->value[DEL_ROSS_FINE];

	/*

    Log("****   coarse root turnover****\n");
    s->value[DEL_ROSS_COARSE] =  s->value[BIOMASS_ROOTS_COARSE_CTEM] * s->value[COARSERTTOVER];
    Log("Average yearly coarse Root turnover rate  = %g\n", s->value[COARSERTTOVER]);
    Log("Coarse Root Biomass lost = %g tDM/ha\n", s->value[DEL_ROSS_COARSE]);


    if (s->value[DEL_ROSS_COARSE] > s->value[BIOMASS_ROOTS_COARSE_CTEM])
    {
         Log("ATTENTION coarse root turnover exceed DEL_WR !!! \n");
         s->value[BIOMASS_ROOTS_COARSE_CTEM] -= s->value[DEL_ROOTS_COARSE_CTEM];
    }

    oldWrc = s->value[BIOMASS_ROOTS_COARSE_CTEM];


    //Coarse Root Biomass less root turnover

    s->value[BIOMASS_ROOTS_COARSE_CTEM] -= s->value[DEL_ROSS_COARSE];


    Log("****Stem turnover****\n");
    //in LPJ the turnover of the stem in considered as the ampunt of sapwood that pass to the heartwood

    s->value[DEL_ROSS_STEM] =  s->value[BIOMASS_STEM_CTEM]* s->value[SAPWOODTTOVER];
    Log("Average yearly Stem turnover rate  = %g\n", s->value[SAPWOODTTOVER]);
    Log("Stem Biomass lost = %g tDM/ha\n", s->value[DEL_ROSS_STEM]);

    if (s->value[DEL_ROSS_STEM] > s->value[BIOMASS_STEM_CTEM])
    {
         Log("ATTENTION stem turnover exceed DEL_WS !!! \n");
         s->value[BIOMASS_STEM_CTEM] -= s->value[DEL_ROSS_STEM];
    }

    oldWs = s->value[BIOMASS_STEM_CTEM];

    // Fine Root Biomass less root turnover

    s->value[BIOMASS_STEM_CTEM] -= s->value[DEL_ROSS_STEM];
	 */

}
