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

	if ( ! s->value[LEAF_C] )
	{
		s->value[LEAF_N]            = 0.;
		s->value[LEAF_SUN_N]        = 0.;
		s->value[LEAF_SHADE_N]      = 0.;

	}
	else
	{
		s->value[LEAF_N]            = s->value[LEAF_C]            / s->value[CN_LEAVES];
		s->value[LEAF_SUN_N]        = s->value[LEAF_SUN_C]        / s->value[CN_LEAVES];
		s->value[LEAF_SHADE_N]      = s->value[LEAF_SHADE_C]      / s->value[CN_LEAVES];
	}

	if ( ! s->value[FROOT_C] )
	{
		s->value[FROOT_N]           = 0.;
	}
	else
	{
		s->value[FROOT_N]           = s->value[FROOT_C]           / s->value[CN_FINE_ROOTS];
	}
	//s->value[LEAF_FALLING_N]  = s->value[LEAF_FALLING_C]    / s->value[CN_FALLING_LEAVES];
	s->value[STEM_LIVEWOOD_N]   = s->value[STEM_LIVEWOOD_C]   / s->value[CN_LIVEWOOD];
	s->value[STEM_DEADWOOD_N]   = s->value[STEM_DEADWOOD_C]   / s->value[CN_DEADWOOD];
	s->value[STEM_N]            = s->value[STEM_LIVEWOOD_N]   + s->value[STEM_DEADWOOD_N];
	s->value[CROOT_LIVEWOOD_N]  = s->value[CROOT_LIVEWOOD_C]  / s->value[CN_LIVEWOOD];
	s->value[CROOT_DEADWOOD_N]  = s->value[CROOT_DEADWOOD_C]  / s->value[CN_DEADWOOD];
	s->value[CROOT_N]           = s->value[CROOT_LIVEWOOD_N]  + s->value[CROOT_DEADWOOD_N];
	s->value[BRANCH_LIVEWOOD_N] = s->value[BRANCH_LIVEWOOD_C] / s->value[CN_LIVEWOOD];
	s->value[BRANCH_DEADWOOD_N] = s->value[BRANCH_DEADWOOD_C] / s->value[CN_DEADWOOD];
	s->value[BRANCH_N]          = s->value[BRANCH_LIVEWOOD_N] + s->value[BRANCH_DEADWOOD_N];

}
