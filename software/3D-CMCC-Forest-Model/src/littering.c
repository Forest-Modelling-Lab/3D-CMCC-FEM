/*
 * littering.c
 *
 *  Created on: 12 apr 2017
 *      Author: alessio
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "common.h"
#include "leaf_fall.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

void littering ( cell_t *const c, species_t *const s )
{
	logger(g_debug_log, "\nLITTERING\n");

	/*********************************************************************************************************************************/
	/*** deadwood carbon ***/

	//CORRADO fixme fixme fixme fixme fixme fixme fixme fixme fixme
	//set to zero for bugs!!!
	s->value[C_TO_DEADWOOD] = 0.;

	/*** compute class-level deadwood carbon pools (tC/sizecell) ****/
	s->value[DEADWOOD_2C]               = ( s->value[C_TO_DEADWOOD] * s->value[DEADWOOD_USCEL_FRAC]);
	s->value[DEADWOOD_3C]               = ( s->value[C_TO_DEADWOOD] * s->value[DEADWOOD_SCEL_FRAC]) ;
	s->value[DEADWOOD_4C]               = ( s->value[C_TO_DEADWOOD] * s->value[DEADWOOD_LIGN_FRAC]) ;
//	/* check */
//	CHECK_CONDITION ( fabs ( s->value[DEADWOOD_2C] + s->value[DEADWOOD_3C] + s->value[DEADWOOD_4C] ), >,  s->value[C_TO_DEADWOOD] + eps);

	/*** update cell-level deadwood carbon fluxes (gC/m2/day) ****/
	c->daily_deadwood_to_litr2C        += s->value[DEADWOOD_2C]     * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr3C        += s->value[DEADWOOD_3C]     * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr4C        += s->value[DEADWOOD_4C]     * 1e6 / g_settings->sizeCell;
//	c->daily_deadwood_to_litrC         += (s->value[DEADWOOD_2C]    +
//			s->value[DEADWOOD_3C]                                   +
//			s->value[DEADWOOD_4C])                                  *
//					1e6 / g_settings->sizeCell;

	/*** compute cell-level deadwood carbon pools (gC/m2) ***/
//	c->deadwood_C                      += s->value[C_TO_DEADWOOD]   * 1e6 / g_settings->sizeCell;
	c->deadwood_2C                     += s->value[DEADWOOD_2C]     * 1e6 / g_settings->sizeCell;
	c->deadwood_3C                     += s->value[DEADWOOD_3C]     * 1e6 / g_settings->sizeCell;
	c->deadwood_4C                     += s->value[DEADWOOD_4C]     * 1e6 / g_settings->sizeCell;
//	/* check */
//	CHECK_CONDITION ( fabs ( c->deadwood_2C + c->deadwood_3C + c->deadwood_4C ) , > , c->deadwood_C + eps );

	/*********************************************************************************************************************************/
	/*** leaf litter carbon ***/

	/*** compute leaf litter carbon pools (tC/sizecell) ****/
	s->value[LEAF_LITR1C]               = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_LAB_FRAC]   );
	s->value[LEAF_LITR2C]               = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_USCEL_FRAC] );
	s->value[LEAF_LITR3C]               = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_SCEL_FRAC]  );
	s->value[LEAF_LITR4C]               = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_LIGN_FRAC]  );
//	/* check */
//	CHECK_CONDITION ( fabs ( s->value[LEAF_LITR1C] + s->value[LEAF_LITR2C] + s->value[LEAF_LITR3C] + s->value[LEAF_LITR4C]), >,  s->value[LEAF_LITTER_C] + eps);

	/*** update cell-level leaf litter carbon fluxes (gC/m2/day) ****/
	c->daily_leaf_to_litr1C            += s->value[LEAF_LITR1C]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr2C            += s->value[LEAF_LITR2C]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr3C            += s->value[LEAF_LITR3C]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr4C            += s->value[LEAF_LITR4C]      * 1e6 / g_settings->sizeCell;
//	c->daily_leaf_to_litrC             += (s->value[LEAF_LITR1C]     +
//			s->value[LEAF_LITR2C]                                    +
//			s->value[LEAF_LITR3C]                                    +
//			s->value[LEAF_LITR4C])                                   *
//					1e6 / g_settings->sizeCell;

	/*** compute cell-level leaf litter carbon pools (gC/m2) ***/
//	c->leaf_litrC                      += s->value[C_LEAF_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->leaf_litr1C                     += s->value[LEAF_LITR1C]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr2C                     += s->value[LEAF_LITR2C]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr3C                     += s->value[LEAF_LITR3C]      * 1e6 / g_settings->sizeCell;
//	c->leaf_litr4C                     += s->value[LEAF_LITR4C]      * 1e6 / g_settings->sizeCell;
//	/* check */
//	CHECK_CONDITION ( fabs ( c->leaf_litr1C + c->leaf_litr2C + c->leaf_litr3C + c->leaf_litr4C ) , > , c->leaf_litrC + eps );

	/*********************************************************************************************************************************/
	/*** fine root litter carbon ***/

	//CORRADO fixme fixme fixme fixme fixme fixme fixme fixme fixme
	//set to zero for bugs!!!
	s->value[N_TO_DEADWOOD] = 0.;

	/*** compute class-level fine root carbon pools (tC/sizecell) ****/
	s->value[FROOT_LITR1C]              = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_LAB_FRAC]   );
	s->value[FROOT_LITR2C]              = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_USCEL_FRAC] );
	s->value[FROOT_LITR3C]              = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_SCEL_FRAC]  );
	s->value[FROOT_LITR4C]              = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_LIGN_FRAC]  );
//	/* check */
//	CHECK_CONDITION ( fabs ( s->value[FROOT_LITR1C] + s->value[FROOT_LITR2C] + s->value[FROOT_LITR3C] + s->value[FROOT_LITR4C]), >,  s->value[FROOT_LITTER_C] + eps);

	/*** update cell-level fine root litter carbon fluxes (gC/m2/day) ****/
	c->daily_froot_to_litr1C           += s->value[FROOT_LITR1C]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr2C           += s->value[FROOT_LITR2C]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr3C           += s->value[FROOT_LITR3C]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr4C           += s->value[FROOT_LITR4C]     * 1e6 / g_settings->sizeCell;
//	c->daily_froot_to_litrC            += (s->value[FROOT_LITR1C]    +
//			s->value[FROOT_LITR2C]                                   +
//			s->value[FROOT_LITR3C]                                   +
//			s->value[FROOT_LITR4C])                                  *
//					1e6 / g_settings->sizeCell;

	/*** compute cell-level fine root litter carbon pools (gC/m2) ***/
//	c->froot_litrC                     += s->value[C_FROOT_TO_LITR]  * 1e6 / g_settings->sizeCell;
	c->froot_litr1C                    += s->value[FROOT_LITR1C]     * 1e6 / g_settings->sizeCell;
	c->froot_litr2C                    += s->value[FROOT_LITR2C]     * 1e6 / g_settings->sizeCell;
	c->froot_litr3C                    += s->value[FROOT_LITR3C]     * 1e6 / g_settings->sizeCell;
	c->froot_litr4C                    += s->value[FROOT_LITR4C]     * 1e6 / g_settings->sizeCell;
//	/* check */
//	CHECK_CONDITION ( fabs ( c->froot_litr1C + c->froot_litr2C + c->froot_litr3C + c->froot_litr4C ) , > , c->froot_litrC + eps );

	/*********************************************************************************************************************************/
	/*** coarse woody debris nitrogen ***/

	/*** compute class-level deadwood nitrogen pools (tN/sizecell) ****/
	s->value[DEADWOOD_2N]               = ( s->value[N_TO_DEADWOOD] * s->value[DEADWOOD_USCEL_FRAC]);
	s->value[DEADWOOD_3N]               = ( s->value[N_TO_DEADWOOD] * s->value[DEADWOOD_SCEL_FRAC]) ;
	s->value[DEADWOOD_4N]               = ( s->value[N_TO_DEADWOOD] * s->value[DEADWOOD_LIGN_FRAC]) ;
//	/* check */
//	CHECK_CONDITION ( fabs ( s->value[DEADWOOD_2N] + s->value[DEADWOOD_3N] + s->value[DEADWOOD_4N] ), >,  s->value[N_TO_DEADWOOD] + eps);

	/*** update cell-level deadwood nitrogen fluxes (gN/m2/day) ****/
	c->daily_deadwood_to_litr2N        += s->value[DEADWOOD_2N]      * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr3N        += s->value[DEADWOOD_3N]      * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr4N        += s->value[DEADWOOD_4N]      * 1e6 / g_settings->sizeCell;
//	c->daily_deadwood_to_litrN         += (s->value[DEADWOOD_2N]     +
//			s->value[DEADWOOD_3N]                                    +
//			s->value[DEADWOOD_4N])                                   *
//					1e6 / g_settings->sizeCell;

	/*** compute cell-level deadwood nitrogen pools (gN/m2) ***/
//	c->deadwood_N                      += s->value[N_TO_DEADWOOD]    * 1e6 / g_settings->sizeCell;
	c->deadwood_2N                     += s->value[DEADWOOD_2N]      * 1e6 / g_settings->sizeCell;
	c->deadwood_3N                     += s->value[DEADWOOD_3N]      * 1e6 / g_settings->sizeCell;
	c->deadwood_4N                     += s->value[DEADWOOD_4N]      * 1e6 / g_settings->sizeCell;
//	/* check */
//	CHECK_CONDITION ( fabs ( c->deadwood_2N + c->deadwood_3N + c->deadwood_4N ) , > , c->deadwood_N + eps );

	/*********************************************************************************************************************************/
	/*** leaf litter nitrogen ***/

	/*** compute leaf litter nitrogen pools (tN/sizecell) ****/
	s->value[LEAF_LITR1N]               = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_LAB_FRAC]   );
	s->value[LEAF_LITR2N]               = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_USCEL_FRAC] );
	s->value[LEAF_LITR3N]               = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_SCEL_FRAC]  );
	s->value[LEAF_LITR4N]               = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_LIGN_FRAC]  );
//	/* check */
//	CHECK_CONDITION ( fabs ( s->value[LEAF_LITR1N] + s->value[LEAF_LITR2N] + s->value[LEAF_LITR3N] + s->value[LEAF_LITR4N]), >,  s->value[LEAF_LITTER_N] + eps);

	/*** update cell-level leaf litter nitrogen fluxes (gN/m2/day) ****/
	c->daily_leaf_to_litr1N            += s->value[LEAF_LITR1N]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr2N            += s->value[LEAF_LITR2N]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr3N            += s->value[LEAF_LITR3N]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr4N            += s->value[LEAF_LITR4N]      * 1e6 / g_settings->sizeCell;
//	c->daily_leaf_to_litrN             += (s->value[LEAF_LITR1N]     +
//			s->value[LEAF_LITR2N]                                    +
//			s->value[LEAF_LITR3N]                                    +
//			s->value[LEAF_LITR4N])                                   *
//					1e6 / g_settings->sizeCell;

	/*** compute cell-level leaf litter nitrogen pools (gN/m2) ***/
//	c->leaf_litrN                      += s->value[N_LEAF_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->leaf_litr1N                     += s->value[LEAF_LITR1N]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr2N                     += s->value[LEAF_LITR2N]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr3N                     += s->value[LEAF_LITR3N]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr4N                     += s->value[LEAF_LITR4N]      * 1e6 / g_settings->sizeCell;
//	/* check */
//	CHECK_CONDITION ( fabs ( c->leaf_litr1N + c->leaf_litr2N + c->leaf_litr3N + c->leaf_litr4N ) , > , c->leaf_litrN + eps );

	/*********************************************************************************************************************************/
	/*** fine root litter nitrogen ***/

	/*** compute class-level fine root nitrogen pools (tC/sizecell) ****/
	s->value[FROOT_LITR1N]              = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_LAB_FRAC]   );
	s->value[FROOT_LITR2N]              = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_USCEL_FRAC] );
	s->value[FROOT_LITR3N]              = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_SCEL_FRAC]  );
	s->value[FROOT_LITR4N]              = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_LIGN_FRAC]  );
//	/* check */
//	CHECK_CONDITION ( fabs ( s->value[FROOT_LITR1N] + s->value[FROOT_LITR2N] + s->value[FROOT_LITR3N] + s->value[FROOT_LITR4N]), >,  s->value[FROOT_LITTER_N] + eps);

	/*** update cell-level fine root litter nitrogen fluxes (gN/m2/day) ****/
	c->daily_froot_to_litr1N           += s->value[FROOT_LITR1N]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr2N           += s->value[FROOT_LITR2N]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr3N           += s->value[FROOT_LITR3N]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr4N           += s->value[FROOT_LITR4N]     * 1e6 / g_settings->sizeCell;
//	c->daily_froot_to_litrN            += (s->value[FROOT_LITR1N]    +
//			s->value[FROOT_LITR2N]                                   +
//			s->value[FROOT_LITR3N]                                   +
//			s->value[FROOT_LITR4N])                                  *
//					1e6 / g_settings->sizeCell;

	/*** compute cell-level fine root litter nitrogen pools (gN/m2) ***/
//	c->froot_litrN                     += s->value[N_FROOT_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->froot_litr1N                    += s->value[FROOT_LITR1N]      * 1e6 / g_settings->sizeCell;
	c->froot_litr2N                    += s->value[FROOT_LITR2N]      * 1e6 / g_settings->sizeCell;
	c->froot_litr3N                    += s->value[FROOT_LITR3N]      * 1e6 / g_settings->sizeCell;
	c->froot_litr4N                    += s->value[FROOT_LITR4N]      * 1e6 / g_settings->sizeCell;
//	/* check */
//	CHECK_CONDITION ( fabs ( c->froot_litr1N + c->froot_litr2N + c->froot_litr3N + c->froot_litr4N ) , > , c->froot_litrN + eps );


}
