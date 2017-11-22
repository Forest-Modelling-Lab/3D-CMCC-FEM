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

#define BUGS 0

void littering ( cell_t *const c, species_t *const s )
{
	logger(g_debug_log, "\nLITTERING\n");

	/*********************************************************************************************************************************/
	/*** deadwood carbon ***/

	/*** compute class-level deadwood carbon fluxes (tC/sizecell/day) ****/
	s->value[DEADWOOD_TO_LITRC]         = s->value[C_TO_DEADWOOD];
	s->value[DEADWOOD_TO_LITR2C]        = s->value[C_TO_DEADWOOD]       * s->value[DEADWOOD_USCEL_FRAC];
	s->value[DEADWOOD_TO_LITR3C]        = s->value[C_TO_DEADWOOD]       * s->value[DEADWOOD_SCEL_FRAC];
	s->value[DEADWOOD_TO_LITR4C]        = s->value[C_TO_DEADWOOD]       * s->value[DEADWOOD_LIGN_FRAC];
	/* check */
	CHECK_CONDITION ( s->value[DEADWOOD_TO_LITR2C] + s->value[DEADWOOD_TO_LITR3C] + s->value[DEADWOOD_TO_LITR4C] , == , s->value[DEADWOOD_TO_LITRC] + eps );

	/*** update cell-level deadwood carbon fluxes (gC/m2/day) ****/
	c->daily_deadwood_to_litrC         += s->value[DEADWOOD_TO_LITRC]   * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr2C        += s->value[DEADWOOD_TO_LITR2C]  * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr3C        += s->value[DEADWOOD_TO_LITR3C]  * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr4C        += s->value[DEADWOOD_TO_LITR4C]  * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->daily_deadwood_to_litr2C + c->daily_deadwood_to_litr3C + c->daily_deadwood_to_litr4C , == , c->daily_deadwood_to_litrC + eps );

	/*** compute cell-level deadwood carbon pools (gC/m2) ***/
	c->deadwood_C                      += s->value[DEADWOOD_TO_LITRC]   * 1e6 / g_settings->sizeCell;
	c->deadwood_2C                     += s->value[DEADWOOD_TO_LITR2C]  * 1e6 / g_settings->sizeCell;
	c->deadwood_3C                     += s->value[DEADWOOD_TO_LITR3C]  * 1e6 / g_settings->sizeCell;
	c->deadwood_4C                     += s->value[DEADWOOD_TO_LITR4C]  * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->deadwood_2C + c->deadwood_3C + c->deadwood_4C , == , c->deadwood_C + eps );

	/*********************************************************************************************************************************/
	/*** leaf litter carbon ***/

	/*** compute leaf litter carbon fluxes (tC/sizecell/day) ****/
	s->value[LEAF_TO_LITRC]             = s->value[C_LEAF_TO_LITR];
	s->value[LEAF_TO_LITR1C]            = s->value[C_LEAF_TO_LITR]      * s->value[LEAF_LITR_LAB_FRAC] ;
	s->value[LEAF_TO_LITR2C]            = s->value[C_LEAF_TO_LITR]      * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[LEAF_TO_LITR3C]            = s->value[C_LEAF_TO_LITR]      * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[LEAF_TO_LITR4C]            = s->value[C_LEAF_TO_LITR]      * s->value[LEAF_LITR_LIGN_FRAC];
	/* check */
	CHECK_CONDITION ( s->value[LEAF_TO_LITR1C] + s->value[LEAF_TO_LITR2C] + s->value[LEAF_TO_LITR3C] + s->value[LEAF_TO_LITR4C] , == , s->value[LEAF_TO_LITRC] + eps );

	/*** update cell-level leaf litter carbon fluxes (gC/m2/day) ****/
	c->daily_leaf_to_litrC             += s->value[LEAF_TO_LITRC]       * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr1C            += s->value[LEAF_TO_LITR1C]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr2C            += s->value[LEAF_TO_LITR2C]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr3C            += s->value[LEAF_TO_LITR3C]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr4C            += s->value[LEAF_TO_LITR4C]      * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->daily_leaf_to_litr1C + c->daily_leaf_to_litr2C + c->daily_leaf_to_litr3C + c->daily_leaf_to_litr4C , == , c->daily_leaf_to_litrC + eps );

	/*** compute cell-level leaf litter carbon pools (gC/m2) ***/
	c->leaf_litrC                      += s->value[LEAF_TO_LITRC]       * 1e6 / g_settings->sizeCell;
	c->leaf_litr1C                     += s->value[LEAF_TO_LITR1C]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr2C                     += s->value[LEAF_TO_LITR2C]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr3C                     += s->value[LEAF_TO_LITR3C]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr4C                     += s->value[LEAF_TO_LITR4C]      * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->leaf_litr1C + c->leaf_litr2C + c->leaf_litr3C + c->leaf_litr4C , == , c->leaf_litrC + eps );

	/*********************************************************************************************************************************/
	/*** fine root litter carbon ***/

	/*** compute fine root litter carbon fluxes (tC/sizecell/day) ****/
	s->value[FROOT_TO_LITRC]            = s->value[C_FROOT_TO_LITR];
	s->value[FROOT_TO_LITR1C]           = s->value[C_FROOT_TO_LITR]     * s->value[FROOT_LITR_LAB_FRAC] ;
	s->value[FROOT_TO_LITR2C]           = s->value[C_FROOT_TO_LITR]     * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[FROOT_TO_LITR3C]           = s->value[C_FROOT_TO_LITR]     * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[FROOT_TO_LITR4C]           = s->value[C_FROOT_TO_LITR]     * s->value[FROOT_LITR_LIGN_FRAC];
	/* check */
	CHECK_CONDITION ( s->value[LEAF_TO_LITR1C] + s->value[LEAF_TO_LITR2C] + s->value[LEAF_TO_LITR3C] + s->value[LEAF_TO_LITR4C], == , s->value[LEAF_TO_LITRC] + eps );

	/*** update cell-level fine root litter carbon fluxes (gC/m2/day) ****/
	c->daily_froot_to_litrC            += s->value[FROOT_TO_LITRC]      * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr1C           += s->value[FROOT_TO_LITR1C]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr2C           += s->value[FROOT_TO_LITR2C]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr3C           += s->value[FROOT_TO_LITR3C]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr4C           += s->value[FROOT_TO_LITR4C]     * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->daily_froot_to_litr1C + c->daily_froot_to_litr2C + c->daily_froot_to_litr3C + c->daily_froot_to_litr4C , == , c->daily_froot_to_litrC + eps );

	/*** compute cell-level fine root litter carbon pools (gC/m2) ***/
	c->froot_litrC                     += s->value[FROOT_TO_LITRC]      * 1e6 / g_settings->sizeCell;
	c->froot_litr1C                    += s->value[FROOT_TO_LITR1C]     * 1e6 / g_settings->sizeCell;
	c->froot_litr2C                    += s->value[FROOT_TO_LITR2C]     * 1e6 / g_settings->sizeCell;
	c->froot_litr3C                    += s->value[FROOT_TO_LITR3C]     * 1e6 / g_settings->sizeCell;
	c->froot_litr4C                    += s->value[FROOT_TO_LITR4C]     * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->froot_litr1C + c->froot_litr2C + c->froot_litr3C + c->froot_litr4C , == , c->froot_litrC + eps );

	/*********************************************************************************************************************************/
	/*** deadwood nitrogen ***/

	/*** compute class-level deadwood nitrogen fluxes (tN/sizecell/day) ****/
	s->value[DEADWOOD_TO_LITRN]         = s->value[N_TO_DEADWOOD];
	s->value[DEADWOOD_TO_LITR2N]        = s->value[N_TO_DEADWOOD]       * s->value[DEADWOOD_USCEL_FRAC];
	s->value[DEADWOOD_TO_LITR3N]        = s->value[N_TO_DEADWOOD]       * s->value[DEADWOOD_SCEL_FRAC];
	s->value[DEADWOOD_TO_LITR4N]        = s->value[N_TO_DEADWOOD]       * s->value[DEADWOOD_LIGN_FRAC];
	/* check */
	CHECK_CONDITION ( s->value[DEADWOOD_TO_LITR2N] + s->value[DEADWOOD_TO_LITR3N] + s->value[DEADWOOD_TO_LITR4N], == , s->value[DEADWOOD_TO_LITRN] + eps );

	/*** update cell-level deadwood nitrogen fluxes (gN/m2/day) ****/
	c->daily_deadwood_to_litrN         += s->value[DEADWOOD_TO_LITRN]   * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr2N        += s->value[DEADWOOD_TO_LITR2N]  * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr3N        += s->value[DEADWOOD_TO_LITR3N]  * 1e6 / g_settings->sizeCell;
	c->daily_deadwood_to_litr4N        += s->value[DEADWOOD_TO_LITR4N]  * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->daily_deadwood_to_litr2N + c->daily_deadwood_to_litr3N + c->daily_deadwood_to_litr4N , == , c->daily_deadwood_to_litrN + eps );

	/*** compute cell-level deadwood nitrogen pools (gN/m2) ***/
	c->deadwood_N                      += s->value[DEADWOOD_TO_LITRN]   * 1e6 / g_settings->sizeCell;
	c->deadwood_2N                     += s->value[DEADWOOD_TO_LITR2N]  * 1e6 / g_settings->sizeCell;
	c->deadwood_3N                     += s->value[DEADWOOD_TO_LITR3N]  * 1e6 / g_settings->sizeCell;
	c->deadwood_4N                     += s->value[DEADWOOD_TO_LITR4N]  * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( c->deadwood_2N + c->deadwood_3N + c->deadwood_4N , == , c->deadwood_N + eps );

	/*********************************************************************************************************************************/
	/*** leaf litter nitrogen ***/

	/*** compute leaf litter nitrogen fluxes (tN/sizecell/day) ****/
	s->value[LEAF_TO_LITRN]             = s->value[N_LEAF_TO_LITR];
	s->value[LEAF_TO_LITR1N]            = s->value[N_LEAF_TO_LITR]      * s->value[LEAF_LITR_LAB_FRAC] ;
	s->value[LEAF_TO_LITR2N]            = s->value[N_LEAF_TO_LITR]      * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[LEAF_TO_LITR3N]            = s->value[N_LEAF_TO_LITR]      * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[LEAF_TO_LITR4N]            = s->value[N_LEAF_TO_LITR]      * s->value[LEAF_LITR_LIGN_FRAC];
	/* check */
	CHECK_CONDITION ( s->value[LEAF_TO_LITR1N] + s->value[LEAF_TO_LITR2N] + s->value[LEAF_TO_LITR3N] + s->value[LEAF_TO_LITR4N], == , s->value[LEAF_TO_LITRN] + eps );

	/*** update cell-level leaf litter nitrogen fluxes (gN/m2/day) ****/
	c->daily_leaf_to_litrN             += s->value[LEAF_TO_LITRN]       * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr1N            += s->value[LEAF_TO_LITR1N]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr2N            += s->value[LEAF_TO_LITR2N]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr3N            += s->value[LEAF_TO_LITR3N]      * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr4N            += s->value[LEAF_TO_LITR4N]      * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->daily_leaf_to_litr1N + c->daily_leaf_to_litr2N + c->daily_leaf_to_litr3N + c->daily_leaf_to_litr4N , == , c->daily_leaf_to_litrN + eps );

	/*** compute cell-level leaf litter nitrogen pools (gN/m2) ***/
	c->leaf_litrN                      += s->value[LEAF_TO_LITRN]       * 1e6 / g_settings->sizeCell;
	c->leaf_litr1N                     += s->value[LEAF_TO_LITR1N]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr2N                     += s->value[LEAF_TO_LITR2N]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr3N                     += s->value[LEAF_TO_LITR3N]      * 1e6 / g_settings->sizeCell;
	c->leaf_litr4N                     += s->value[LEAF_TO_LITR4N]      * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->leaf_litr1N + c->leaf_litr2N + c->leaf_litr3N + c->leaf_litr4N , == , c->leaf_litrN + eps );

	/*********************************************************************************************************************************/
	/*** fine root litter nitrogen ***/

	/*** compute fine root litter carbon fluxes (tN/sizecell/day) ****/
	s->value[FROOT_TO_LITRN]            = s->value[N_FROOT_TO_LITR];
	s->value[FROOT_TO_LITR1N]           = s->value[N_FROOT_TO_LITR]     * s->value[FROOT_LITR_LAB_FRAC] ;
	s->value[FROOT_TO_LITR2N]           = s->value[N_FROOT_TO_LITR]     * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[FROOT_TO_LITR3N]           = s->value[N_FROOT_TO_LITR]     * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[FROOT_TO_LITR4N]           = s->value[N_FROOT_TO_LITR]     * s->value[FROOT_LITR_LIGN_FRAC];
	/* check */
	CHECK_CONDITION ( s->value[FROOT_TO_LITR1N] + s->value[FROOT_TO_LITR2N] + s->value[FROOT_TO_LITR3N] + s->value[FROOT_TO_LITR4N], == , s->value[FROOT_TO_LITRN] + eps );

	/*** update cell-level fine root litter nitrogen fluxes (gN/m2/day) ****/
	c->daily_froot_to_litrN            += s->value[FROOT_TO_LITRN]      * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr1N           += s->value[FROOT_TO_LITR1N]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr2N           += s->value[FROOT_TO_LITR2N]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr3N           += s->value[FROOT_TO_LITR3N]     * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr4N           += s->value[FROOT_TO_LITR4N]     * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->daily_froot_to_litr1N + c->daily_froot_to_litr2N + c->daily_froot_to_litr3N + c->daily_froot_to_litr4N , == , c->daily_froot_to_litrN + eps );

	/*** compute cell-level fine root litter nitrogen pools (gN/m2) ***/
	c->froot_litrN                     += s->value[FROOT_TO_LITRN]      * 1e6 / g_settings->sizeCell;
	c->froot_litr1N                    += s->value[FROOT_TO_LITR1N]     * 1e6 / g_settings->sizeCell;
	c->froot_litr2N                    += s->value[FROOT_TO_LITR2N]     * 1e6 / g_settings->sizeCell;
	c->froot_litr3N                    += s->value[FROOT_TO_LITR3N]     * 1e6 / g_settings->sizeCell;
	c->froot_litr4N                    += s->value[FROOT_TO_LITR4N]     * 1e6 / g_settings->sizeCell;
	/* check */
	CHECK_CONDITION ( c->froot_litr1N + c->froot_litr2N + c->froot_litr3N + c->froot_litr4N , == , c->froot_litrN + eps );

}
