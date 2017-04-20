/*
 * N-assimilation.c
 *
 *  Created on: 28/ott/2013
 *      Author: alessio
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <N-assimilation.h>
#include "constants.h"
#include "settings.h"
#include "logger.h"

void nitrogen_assimilation(species_t *const s)
{
	/* computing Nitrogen pools (tC/ha) */

	s->value[LEAF_N]            = s->value[LEAF_C]             / s->value[CN_LEAVES];
	//s->value[LEAF_FALLING_N]  = s->value[LEAF_FALLING_C]     / s->value[CN_FALLING_LEAVES];
	s->value[FROOT_N]           = s->value[FROOT_C]            / s->value[CN_FINE_ROOTS];
	s->value[STEM_LIVE_WOOD_N]  = s->value[STEM_LIVE_WOOD_C]   / s->value[CN_LIVE_WOODS];
	s->value[STEM_DEAD_WOOD_N]  = s->value[STEM_DEAD_WOOD_C]   / s->value[CN_DEAD_WOODS];
	s->value[STEM_N]            = s->value[STEM_LIVE_WOOD_N]   + s->value[STEM_DEAD_WOOD_N];
	s->value[CROOT_LIVE_WOOD_N] = s->value[CROOT_LIVE_WOOD_C]  / s->value[CN_LIVE_WOODS];
	s->value[CROOT_DEAD_WOOD_N] = s->value[CROOT_DEAD_WOOD_C]  / s->value[CN_DEAD_WOODS];
	s->value[CROOT_N]           = s->value[CROOT_LIVE_WOOD_N]  + s->value[CROOT_DEAD_WOOD_N];
	s->value[BRANCH_LIVE_WOOD_N]= s->value[BRANCH_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	s->value[BRANCH_DEAD_WOOD_N]= s->value[BRANCH_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	s->value[BRANCH_N]          = s->value[BRANCH_LIVE_WOOD_N] + s->value[BRANCH_DEAD_WOOD_N];

}
