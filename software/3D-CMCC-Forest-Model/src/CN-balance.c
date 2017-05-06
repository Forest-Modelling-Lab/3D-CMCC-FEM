/*
 * balance.c
 *
 *  Created on: 13 apr 2017
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "settings.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void carbon_balance (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
//	species_t *s;
//	s = &c->heights[height].dbhs[dbh].ages[age].species[species];
//
//	/*** update class level carbon mass pools ***/
//	s->value[LEAF_C]     += (s->value[C_TO_LEAF]    - s->value[C_LEAF_TO_LITR]  + s->value[C_LEAF_TO_RESERVE]);
//	s->value[FROOT_C]    += (s->value[C_TO_FROOT]   - s->value[C_FROOT_TO_LITR] + s->value[C_FROOT_TO_RESERVE]);
//	s->value[STEM_C]     += (s->value[C_TO_STEM]    - s->value[C_STEM_TO_CWD]);
//	s->value[CROOT_C]    += (s->value[C_TO_CROOT]   - s->value[C_CROOT_TO_CWD]);
//	s->value[BRANCH_C]   += (s->value[C_TO_BRANCH]  - s->value[C_BRANCH_TO_CWD]);
//	s->value[RESERVE_C]  += (s->value[C_TO_RESERVE] - s->value[C_RESERVE_TO_CWD]);
//	s->value[FRUIT_C]    += (s->value[C_TO_FRUIT]   - s->value[C_FRUIT_TO_CWD]);
//
//	logger(g_debug_log, "\n* carbon pools (tree level) *\n");
//	logger(g_debug_log, "LEAF_C      = %f tC/cell\n", s->value[LEAF_C]);
//	logger(g_debug_log, "FROOT_C     = %f tC/cell\n", s->value[FROOT_C]);
//	logger(g_debug_log, "STEM_C      = %f tC/cell\n", s->value[STEM_C]);
//	logger(g_debug_log, "CROOT_C     = %f tC/cell\n", s->value[CROOT_C]);
//	logger(g_debug_log, "BRANCH_C    = %f tC/cell\n", s->value[BRANCH_C]);
//	logger(g_debug_log, "RESERVE_C   = %f tC/cell\n", s->value[RESERVE_C]);
//	logger(g_debug_log, "FRUIT_C     = %f tC/cell\n", s->value[FRUIT_C]);
//
//	/* check */
//	CHECK_CONDITION ( s->value[LEAF_C],     < , ZERO );
//	CHECK_CONDITION ( s->value[FROOT_C],    < , ZERO );
//	CHECK_CONDITION ( s->value[STEM_C],     < , ZERO );
//	CHECK_CONDITION ( s->value[BRANCH_C],   < , ZERO );
//	CHECK_CONDITION ( s->value[CROOT_C],    < , ZERO );
//	CHECK_CONDITION ( s->value[FRUIT_C],    < , ZERO );
//
//	/*** update cell level carbon pools ***/
//
//	/* tree */
//	c->leaf_carbon              += ((s->value[C_TO_LEAF]    - s->value[C_LEAF_TO_LITR]  + s->value[C_LEAF_TO_RESERVE])  * 1e6 / g_settings->sizeCell);
//	c->froot_carbon             += ((s->value[C_TO_FROOT]   - s->value[C_FROOT_TO_LITR] + s->value[C_FROOT_TO_RESERVE]) * 1e6 / g_settings->sizeCell);
//	c->stem_carbon              += ((s->value[C_TO_STEM]    - s->value[C_STEM_TO_CWD])                                  * 1e6 / g_settings->sizeCell);
//	c->branch_carbon            += ((s->value[C_TO_BRANCH]  - s->value[C_CROOT_TO_CWD])                                 * 1e6 / g_settings->sizeCell);
//	c->croot_carbon             += ((s->value[C_TO_CROOT]   - s->value[C_BRANCH_TO_CWD])                                * 1e6 / g_settings->sizeCell);
//	c->reserve_carbon           += ((s->value[C_TO_RESERVE] - s->value[C_BRANCH_TO_CWD])                                * 1e6 / g_settings->sizeCell);
//	c->fruit_carbon             += ((s->value[C_TO_FRUIT]   - s->value[C_FRUIT_TO_CWD])                                 * 1e6 / g_settings->sizeCell);
//
//	logger(g_debug_log, "\n* carbon pools (cell level) *\n");
//	logger(g_debug_log, "leaf_carbon          = %f gC/m2\n", c->leaf_carbon);
//	logger(g_debug_log, "froot_carbon         = %f gC/m2\n", c->froot_carbon);
//	logger(g_debug_log, "stem_carbon          = %f gC/m2\n", c->stem_carbon);
//	logger(g_debug_log, "branch_carbon        = %f gC/m2\n", c->branch_carbon);
//	logger(g_debug_log, "croot_carbon         = %f gC/m2\n", c->croot_carbon);
//	logger(g_debug_log, "reserve_carbon       = %f gC/m2\n", c->reserve_carbon);
//	logger(g_debug_log, "fruit_carbon         = %f gC/m2\n", c->fruit_carbon);
//
//	/* check */
//	CHECK_CONDITION ( c->leaf_carbon,    < , ZERO );
//	CHECK_CONDITION ( c->froot_carbon,   < , ZERO );
//	CHECK_CONDITION ( c->stem_carbon,    < , ZERO );
//	CHECK_CONDITION ( c->branch_carbon,  < , ZERO );
//	CHECK_CONDITION ( c->croot_carbon,   < , ZERO );
//	CHECK_CONDITION ( c->fruit_carbon,   < , ZERO );
//	/***************************************************************************************/
//
//	/* single tree average tree pools */
//	average_tree_pools ( s );
//
//	/***************************************************************************************/

}

