/*
 * growth_respiration.c
 *
 *  Created on: 03/ott/2013
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



//FOLLOWING BIOME-BGC

void Get_growth_respiration (SPECIES *s, CELL *const c)
{

	Log("\nGET_GROWTH_RESPIRATION\n");

	//COMPUTE GROWTH RESPIRATION
	s->value[LEAF_GROWTH_RESP] = s->value[DEL_FOLIAGE_CTEM] * GRPERC;
	Log("daily leaf growth respiration = %g gC/day m^2\n", s->value[LEAF_GROWTH_RESP]);

	s->value[FINE_ROOT_GROWTH_RESP] = s->value[DEL_ROOTS_FINE_CTEM]* GRPERC;
	Log("daily fine root growth respiration = %g gC/day m^2\n", s->value[FINE_ROOT_GROWTH_RESP]);

	s->value[STEM_GROWTH_RESP] = s->value[DEL_STEMS_CTEM]* GRPERC;
	Log("daily stem growth respiration = %g gC/day m^2\n", s->value[STEM_GROWTH_RESP]);

	s->value[COARSE_ROOT_GROWTH_RESP] = s->value[DEL_ROOTS_COARSE_CTEM]* GRPERC;
	Log("daily coarse root growth respiration = %g gC/day m^2\n", s->value[COARSE_ROOT_GROWTH_RESP]);

	s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] + s->value[FINE_ROOT_GROWTH_RESP] + s->value[STEM_GROWTH_RESP] + s->value[COARSE_ROOT_GROWTH_RESP];
	Log("daily total growth respiration = %g gC/day m^2\n", s->value[TOTAL_GROWTH_RESP]);

	//THEN REMOVE FROM ASSIMILATED BIOMASS
	//TODO CHECK IF USE
	/*
	 *s->value[BIOMASS_FOLIAGE_CTEM] -=  s->value[LEAF_GROWTH_RESP];
	 *s->value[BIOMASS_ROOTS_FINE_CTEM] -=  s->value[FINE_ROOT_GROWTH_RESP];
	 *s->value[BIOMASS_STEM_CTEM] -=  s->value[STEM_GROWTH_RESP];
	 *s->value[BIOMASS_ROOTS_COARSE_CTEM] -=  s->value[COARSE_ROOT_GROWTH_RESP];
	 *
	 */
}
