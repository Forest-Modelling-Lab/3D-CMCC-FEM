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

void littering ( cell_t *const c, species_t *const s )
{
	/* daily flux for litter production */

	/** carbon littering **/

	/* cell level litter carbon fluxes (gC/m2/day) */

	/*** compute coarse woody debris carbon pools ****/
	s->value[CWD_LITR2C] = ( s->value[C_TO_CWD] * s->value[DEAD_WOOD_USCEL_FRAC]);
	s->value[CWD_LITR3C] = ( s->value[C_TO_CWD] * s->value[DEAD_WOOD_SCEL_FRAC]) ;
	s->value[CWD_LITR4C] = ( s->value[C_TO_CWD] * s->value[DEAD_WOOD_LIGN_FRAC]) ;

	/* check */
	CHECK_CONDITION ( fabs ( s->value[CWD_LITR2C] + s->value[CWD_LITR3C] + s->value[CWD_LITR4C] ), >,  s->value[C_TO_CWD] + eps);

	/*** compute leaf litter carbon pools ****/
	s->value[LEAF_LITR1C] = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_LAB_FRAC]   );
	s->value[LEAF_LITR2C] = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_USCEL_FRAC] );
	s->value[LEAF_LITR3C] = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_SCEL_FRAC]  );
	s->value[LEAF_LITR4C] = ( s->value[C_LEAF_TO_LITR] * s->value[LEAF_LITR_LIGN_FRAC]  );

	/* check */
	CHECK_CONDITION ( fabs ( s->value[LEAF_LITR1C] + s->value[LEAF_LITR2C] + s->value[LEAF_LITR3C] + s->value[LEAF_LITR4C]), >,  s->value[LEAF_LITTER_C] + eps);

	/*** compute froot litter carbon pools ****/
	s->value[FROOT_LITR1C] = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_LAB_FRAC]   );
	s->value[FROOT_LITR2C] = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_USCEL_FRAC] );
	s->value[FROOT_LITR3C] = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_SCEL_FRAC]  );
	s->value[FROOT_LITR4C] = ( s->value[C_FROOT_TO_LITR] * s->value[FROOT_LITR_LIGN_FRAC]  );

	/* check */
	CHECK_CONDITION ( fabs ( s->value[FROOT_LITR1C] + s->value[FROOT_LITR2C] + s->value[FROOT_LITR3C] + s->value[FROOT_LITR4C]), >,  s->value[FROOT_LITTER_C] + eps);

	/*** compute coarse woody debris carbon pools (gC/m2) ***/
	c->cwd_litrC   += s->value[C_TO_CWD]           * 1e6 / g_settings->sizeCell;
	c->cwd_litr2C  += s->value[CWD_LITR2C]         * 1e6 / g_settings->sizeCell;
	c->cwd_litr3C  += s->value[CWD_LITR3C]         * 1e6 / g_settings->sizeCell;
	c->cwd_litr4C  += s->value[CWD_LITR4C]         * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( fabs ( c->cwd_litr2C + c->cwd_litr3C + c->cwd_litr4C ) , > , c->cwd_litrC + eps );

	/*** compute leaf litter carbon pools (gC/m2) ****/
	c->leaf_litrC  += s->value[C_LEAF_TO_LITR]     * 1e6 / g_settings->sizeCell;
	c->leaf_litr1C += s->value[LEAF_LITR1C]        * 1e6 / g_settings->sizeCell;
	c->leaf_litr2C += s->value[LEAF_LITR2C]        * 1e6 / g_settings->sizeCell;
	c->leaf_litr3C += s->value[LEAF_LITR3C]        * 1e6 / g_settings->sizeCell;
	c->leaf_litr4C += s->value[LEAF_LITR4C]        * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( fabs ( c->leaf_litr1C + c->leaf_litr2C + c->leaf_litr3C + c->leaf_litr4C ) , > , c->leaf_litrC + eps );

	/*** compute froot litter carbon pools (gN/m2) ****/
	c->froot_litrC  += s->value[C_FROOT_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->froot_litr1C += s->value[FROOT_LITR1C]      * 1e6 / g_settings->sizeCell;
	c->froot_litr2C += s->value[FROOT_LITR2C]      * 1e6 / g_settings->sizeCell;
	c->froot_litr3C += s->value[FROOT_LITR3C]      * 1e6 / g_settings->sizeCell;
	c->froot_litr4C += s->value[FROOT_LITR4C]      * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( fabs ( c->froot_litr1C + c->froot_litr2C + c->froot_litr3C + c->froot_litr4C ) , > , c->froot_litrC + eps );

	/* cumulate overall */
	c->litrC += c->leaf_litrC + c->froot_litrC + c->cwd_litrC;

	c->daily_leaf_to_litrC        += s->value[C_LEAF_TO_LITR]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litrC       += s->value[C_FROOT_TO_LITR] * 1e6 / g_settings->sizeCell;

	/* cell level cwd carbon fluxes (gC/m2/day) */
	c->daily_stem_cwdC            += s->value[C_STEM_TO_CWD]    * 1e6 / g_settings->sizeCell;
	c->daily_croot_cwdC           += s->value[C_CROOT_TO_CWD]   * 1e6 / g_settings->sizeCell;
	c->daily_branch_cwdC          += s->value[C_BRANCH_TO_CWD]  * 1e6 / g_settings->sizeCell;
	c->daily_reserve_cwdC         += s->value[C_RESERVE_TO_CWD] * 1e6 / g_settings->sizeCell;
	c->daily_fruit_cwdC           += s->value[C_FRUIT_TO_CWD]   * 1e6 / g_settings->sizeCell;
	c->daily_cwd_to_litrC         += (s->value[C_STEM_TO_CWD]   +
			s->value[C_CROOT_TO_CWD]                            +
			s->value[C_BRANCH_TO_CWD]                           +
			s->value[C_RESERVE_TO_CWD]                          +
			s->value[C_FRUIT_TO_CWD])                           *
					1e6 / g_settings->sizeCell;

	c->daily_to_litrC             += (s->value[C_LEAF_TO_LITR]  +
			s->value[C_FROOT_TO_LITR]                           +
			s->value[C_TO_CWD] )                                *
					1e6 / g_settings->sizeCell;

	/* cell level litter carbon fluxes */
	c->daily_cwd_to_litr2C        += s->value[CWD_LITR2C]   * 1e6 / g_settings->sizeCell;
	c->daily_cwd_to_litr3C        += s->value[CWD_LITR3C]   * 1e6 / g_settings->sizeCell;
	c->daily_cwd_to_litr4C        += s->value[CWD_LITR4C]   * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr1C       += s->value[LEAF_LITR1C]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr2C       += s->value[LEAF_LITR2C]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr3C       += s->value[LEAF_LITR3C]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr4C       += s->value[LEAF_LITR4C]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr1C      += s->value[FROOT_LITR1C] * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr2C      += s->value[FROOT_LITR2C] * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr3C      += s->value[FROOT_LITR3C] * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr4C      += s->value[FROOT_LITR4C] * 1e6 / g_settings->sizeCell;

	/* overall */
	c->daily_litr1C               += c->daily_leaf_to_litr1C + c->daily_froot_to_litr1C;
	c->daily_litr2C               += c->daily_leaf_to_litr2C + c->daily_froot_to_litr2C + c->daily_cwd_to_litr2C;
	c->daily_litr3C               += c->daily_leaf_to_litr3C + c->daily_froot_to_litr3C + c->daily_cwd_to_litr3C;
	c->daily_litr4C               += c->daily_leaf_to_litr4C + c->daily_froot_to_litr4C + c->daily_cwd_to_litr4C;


	/****************************************************************************************************************/

	/** nitrogen littering **/

	/* cell level litter carbon fluxes (gN/m2/day) */

	/*** compute coarse woody debris carbon pools ****/
	s->value[CWD_LITR2N] = ( s->value[N_TO_CWD] * s->value[DEAD_WOOD_USCEL_FRAC]);
	s->value[CWD_LITR3N] = ( s->value[N_TO_CWD] * s->value[DEAD_WOOD_SCEL_FRAC]) ;
	s->value[CWD_LITR4N] = ( s->value[N_TO_CWD] * s->value[DEAD_WOOD_LIGN_FRAC]) ;

	/* check */
	CHECK_CONDITION ( fabs ( s->value[CWD_LITR2N] + s->value[CWD_LITR3N] + s->value[CWD_LITR4N] ), >,  s->value[N_TO_CWD] + eps);

	/*** compute leaf litter carbon pools ****/
	s->value[LEAF_LITR1N] = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_LAB_FRAC]   );
	s->value[LEAF_LITR2N] = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_USCEL_FRAC] );
	s->value[LEAF_LITR3N] = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_SCEL_FRAC]  );
	s->value[LEAF_LITR4N] = ( s->value[N_LEAF_TO_LITR] * s->value[LEAF_LITR_LIGN_FRAC]  );

	/* check */
	CHECK_CONDITION ( fabs ( s->value[LEAF_LITR1N] + s->value[LEAF_LITR2N] + s->value[LEAF_LITR3N] + s->value[LEAF_LITR4N]), >,  s->value[LEAF_LITTER_N] + eps);

	/*** compute froot litter carbon pools ****/
	s->value[FROOT_LITR1N] = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_LAB_FRAC]   );
	s->value[FROOT_LITR2N] = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_USCEL_FRAC] );
	s->value[FROOT_LITR3N] = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_SCEL_FRAC]  );
	s->value[FROOT_LITR4N] = ( s->value[N_FROOT_TO_LITR] * s->value[FROOT_LITR_LIGN_FRAC]  );

	/* check */
	CHECK_CONDITION ( fabs ( s->value[FROOT_LITR1N] + s->value[FROOT_LITR2N] + s->value[FROOT_LITR3N] + s->value[FROOT_LITR4N]), >,  s->value[FROOT_LITTER_N] + eps);

	/*** compute coarse woody debris carbon pools (gN/m2) ***/
	c->cwd_litrN   += s->value[N_TO_CWD]           * 1e6 / g_settings->sizeCell;
	c->cwd_litr2N  += s->value[CWD_LITR2N]         * 1e6 / g_settings->sizeCell;
	c->cwd_litr3N  += s->value[CWD_LITR3N]         * 1e6 / g_settings->sizeCell;
	c->cwd_litr4N  += s->value[CWD_LITR4N]         * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( fabs ( c->cwd_litr2N + c->cwd_litr3N + c->cwd_litr4N ) , > , c->cwd_litrN + eps );

	/*** compute leaf litter carbon pools (gN/m2) ****/
	c->leaf_litrN  += s->value[N_LEAF_TO_LITR]     * 1e6 / g_settings->sizeCell;
	c->leaf_litr1N += s->value[LEAF_LITR1N]        * 1e6 / g_settings->sizeCell;
	c->leaf_litr2N += s->value[LEAF_LITR2N]        * 1e6 / g_settings->sizeCell;
	c->leaf_litr3N += s->value[LEAF_LITR3N]        * 1e6 / g_settings->sizeCell;
	c->leaf_litr4N += s->value[LEAF_LITR4N]        * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( fabs ( c->leaf_litr1N + c->leaf_litr2N + c->leaf_litr3N + c->leaf_litr4N ) , > , c->leaf_litrN + eps );

	/*** compute froot litter carbon pools (gN/m2) ****/
	c->froot_litrN  += s->value[N_FROOT_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->froot_litr1N += s->value[FROOT_LITR1N]      * 1e6 / g_settings->sizeCell;
	c->froot_litr2N += s->value[FROOT_LITR2N]      * 1e6 / g_settings->sizeCell;
	c->froot_litr3N += s->value[FROOT_LITR3N]      * 1e6 / g_settings->sizeCell;
	c->froot_litr4N += s->value[FROOT_LITR4N]      * 1e6 / g_settings->sizeCell;

	/* check */
	CHECK_CONDITION ( fabs ( c->froot_litr1N + c->froot_litr2N + c->froot_litr3N + c->froot_litr4N ) , > , c->froot_litrN + eps );

	/* cumulate overall */
	c->litrN += c->leaf_litrN + c->froot_litrN + c->cwd_litrN;

	c->daily_leaf_to_litrN        += s->value[N_LEAF_TO_LITR]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litrN       += s->value[N_FROOT_TO_LITR] * 1e6 / g_settings->sizeCell;

	/* cell level cwd carbon fluxes (gN/m2/day) */
	c->daily_stem_cwdN            += s->value[N_STEM_TO_CWD]    * 1e6 / g_settings->sizeCell;
	c->daily_croot_cwdN           += s->value[N_CROOT_TO_CWD]   * 1e6 / g_settings->sizeCell;
	c->daily_branch_cwdN          += s->value[N_BRANCH_TO_CWD]  * 1e6 / g_settings->sizeCell;
	c->daily_reserve_cwdN         += s->value[N_RESERVE_TO_CWD] * 1e6 / g_settings->sizeCell;
	c->daily_fruit_cwdN           += s->value[N_FRUIT_TO_CWD]   * 1e6 / g_settings->sizeCell;
	c->daily_cwd_to_litrN         += (s->value[N_STEM_TO_CWD]   +
			s->value[N_CROOT_TO_CWD]                            +
			s->value[N_BRANCH_TO_CWD]                           +
			s->value[N_RESERVE_TO_CWD]                          +
			s->value[N_FRUIT_TO_CWD])                           *
					1e6 / g_settings->sizeCell;

	c->daily_to_litrN             += (s->value[N_LEAF_TO_LITR]  +
			s->value[N_FROOT_TO_LITR]                           +
			s->value[N_TO_CWD])                                 *
			1e6 / g_settings->sizeCell;

	/* cell level litter carbon fluxes */
	c->daily_cwd_to_litr2N        += s->value[CWD_LITR2N]   * 1e6 / g_settings->sizeCell;
	c->daily_cwd_to_litr3N        += s->value[CWD_LITR3N]   * 1e6 / g_settings->sizeCell;
	c->daily_cwd_to_litr4N        += s->value[CWD_LITR4N]   * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr1N       += s->value[LEAF_LITR1N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr2N       += s->value[LEAF_LITR2N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr3N       += s->value[LEAF_LITR3N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_to_litr4N       += s->value[LEAF_LITR4N]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr1N      += s->value[FROOT_LITR1N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr2N      += s->value[FROOT_LITR2N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr3N      += s->value[FROOT_LITR3N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_to_litr4N      += s->value[FROOT_LITR4N] * 1e6 / g_settings->sizeCell;

	/* overall */
	c->daily_litr1N               += c->daily_leaf_to_litr1N + c->daily_froot_to_litr1N;
	c->daily_litr2N               += c->daily_leaf_to_litr2N + c->daily_froot_to_litr2N + c->daily_cwd_to_litr2N;
	c->daily_litr3N               += c->daily_leaf_to_litr3N + c->daily_froot_to_litr3N + c->daily_cwd_to_litr3N;
	c->daily_litr4N               += c->daily_leaf_to_litr4N + c->daily_froot_to_litr4N + c->daily_cwd_to_litr4N;


}
