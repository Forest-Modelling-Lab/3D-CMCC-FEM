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
	c->daily_leaf_litrC        += (s->value[C_LEAF_TO_LITR]  + s->value[DEAD_LEAF_C] )  * 1e6 / g_settings->sizeCell;
	c->daily_froot_litrC       += (s->value[C_FROOT_TO_LITR] + s->value[DEAD_FROOT_C] ) * 1e6 / g_settings->sizeCell;
	c->daily_litrC             += ((s->value[C_LEAF_TO_LITR] +
			s->value[C_FROOT_TO_LITR] +
			s->value[DEAD_LEAF_C]     +
			s->value[DEAD_FROOT_C])   *
			(1e6 / g_settings->sizeCell));

	/* cell level cwd carbon fluxes (gC/m2/day) */
	c->daily_stem_cwdC         += (s->value[C_STEM_TO_CWD]    + s->value[DEAD_STEM_C])    * 1e6 / g_settings->sizeCell;
	c->daily_croot_cwdC        += (s->value[C_CROOT_TO_CWD]   + s->value[DEAD_CROOT_C])   * 1e6 / g_settings->sizeCell;
	c->daily_branch_cwdC       += (s->value[C_BRANCH_TO_CWD]  + s->value[DEAD_BRANCH_C])  * 1e6 / g_settings->sizeCell;
	c->daily_reserve_cwdC      += (s->value[C_RESERVE_TO_CWD] + s->value[DEAD_RESERVE_C]) * 1e6 / g_settings->sizeCell;
	c->daily_fruit_cwdC        += (s->value[C_FRUIT_TO_CWD]   + s->value[DEAD_FRUIT_C])   * 1e6 / g_settings->sizeCell;
	c->daily_cwdC              += ((s->value[C_STEM_TO_CWD] +
			s->value[C_CROOT_TO_CWD]                        +
			s->value[C_BRANCH_TO_CWD]                       +
			s->value[C_RESERVE_TO_CWD]                      +
			s->value[C_FRUIT_TO_CWD]                        +
			s->value[DEAD_STEM_C]                           +
			s->value[DEAD_CROOT_C]                          +
			s->value[DEAD_BRANCH_C]                         +
			s->value[DEAD_RESERVE_C]                        +
			s->value[DEAD_FRUIT_C])                         *
			(1e6 / g_settings->sizeCell));

	/* cell level soil carbon fluxes (gC/m2/day) */
	c->daily_soilC       += 0. ;

	/* cell level litter carbon pools (gC/m2) */
	c->litrC                   += c->daily_litrC;
	c->cwdC                    += c->daily_cwdC ;
	c->soilC                   += c->daily_soilC ;

	/* leaf litter carbon fluxes */
	s->value[C_LEAF_TO_LITR1C]  = (s->value[C_LEAF_TO_LITR]  + s->value[DEAD_LEAF_C])  * s->value[LEAF_LITR_LAB_FRAC];
	s->value[C_LEAF_TO_LITR2C]  = (s->value[C_LEAF_TO_LITR]  + s->value[DEAD_LEAF_C])  * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[C_LEAF_TO_LITR3C]  = (s->value[C_LEAF_TO_LITR]  + s->value[DEAD_LEAF_C])  * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[C_LEAF_TO_LITR4C]  = (s->value[C_LEAF_TO_LITR]  + s->value[DEAD_LEAF_C])  * s->value[LEAF_LITR_LIGN_FRAC];

	/* fine root litter carbon fluxes */
	s->value[C_FROOT_TO_LITR1C] = (s->value[C_FROOT_TO_LITR] + s->value[DEAD_FROOT_C]) * s->value[FROOT_LITR_LAB_FRAC];
	s->value[C_FROOT_TO_LITR2C] = (s->value[C_FROOT_TO_LITR] + s->value[DEAD_FROOT_C]) * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[C_FROOT_TO_LITR3C] = (s->value[C_FROOT_TO_LITR] + s->value[DEAD_FROOT_C]) * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[C_FROOT_TO_LITR4C] = (s->value[C_FROOT_TO_LITR] + s->value[DEAD_FROOT_C]) * s->value[FROOT_LITR_LIGN_FRAC];

	CHECK_CONDITION ( fabs(s->value[C_LEAF_TO_LITR1C] +	s->value[C_LEAF_TO_LITR2C] + s->value[C_LEAF_TO_LITR3C] + s->value[C_LEAF_TO_LITR4C]) -	(s->value[C_LEAF_TO_LITR] +	s->value[DEAD_LEAF_C]), >, eps );
	CHECK_CONDITION ( fabs(s->value[C_FROOT_TO_LITR1C] + s->value[C_FROOT_TO_LITR2C] + s->value[C_FROOT_TO_LITR3C] + s->value[C_FROOT_TO_LITR4C]) -	(s->value[C_FROOT_TO_LITR] +	s->value[DEAD_FROOT_C]), >, eps );

	/* cell level litter carbon fluxes */
	c->daily_leaf_litr1C       += s->value[C_LEAF_TO_LITR1C]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr2C       += s->value[C_LEAF_TO_LITR2C]  * 1e6 / g_settings->sizeCell ;
	c->daily_leaf_litr3C       += s->value[C_LEAF_TO_LITR3C]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr4C       += s->value[C_LEAF_TO_LITR4C]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr1C      += s->value[C_FROOT_TO_LITR1C] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr2C      += s->value[C_FROOT_TO_LITR2C] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr3C      += s->value[C_FROOT_TO_LITR3C] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr4C      += s->value[C_FROOT_TO_LITR4C] * 1e6 / g_settings->sizeCell;

	/* cell level litter carbon pools (tC/sizecell) */
	c->leaf_litr1C             += c->daily_leaf_litr1C;
	c->leaf_litr2C             += c->daily_leaf_litr2C;
	c->leaf_litr3C             += c->daily_leaf_litr3C;
	c->leaf_litr4C             += c->daily_leaf_litr4C;
	c->froot_litr1C            += c->daily_froot_litr1C;
	c->froot_litr2C            += c->daily_froot_litr2C;
	c->froot_litr3C            += c->daily_froot_litr3C;
	c->froot_litr4C            += c->daily_froot_litr4C;
	//todo
	//c->daily_cwd1C              += ();

	/****************************************************************************************************************/

	/** nitrogen littering **/

	/* cell level litter nitrogen fluxes (gN/m2/day) */
	c->daily_leaf_litrN        += (s->value[N_LEAF_TO_LITR]  + s->value[DEAD_LEAF_N] )  * 1e6 / g_settings->sizeCell;
	c->daily_froot_litrN       += (s->value[N_FROOT_TO_LITR] + s->value[DEAD_FROOT_N] ) * 1e6 / g_settings->sizeCell;
	c->daily_litrN             += ((s->value[N_LEAF_TO_LITR] +
			s->value[N_FROOT_TO_LITR] +
			s->value[DEAD_LEAF_N]     +
			s->value[DEAD_FROOT_N])   *
			(1e6 / g_settings->sizeCell));

	/* cell level cwd nitrogen fluxes (gN/m2/day) */
	c->daily_stem_cwdN         += (s->value[N_STEM_TO_CWD]    + s->value[DEAD_STEM_N])    * 1e6 / g_settings->sizeCell;
	c->daily_croot_cwdN        += (s->value[N_CROOT_TO_CWD]   + s->value[DEAD_CROOT_N])   * 1e6 / g_settings->sizeCell;
	c->daily_branch_cwdN       += (s->value[N_BRANCH_TO_CWD]  + s->value[DEAD_BRANCH_N])  * 1e6 / g_settings->sizeCell;
	c->daily_reserve_cwdN      += (s->value[N_RESERVE_TO_CWD] + s->value[DEAD_RESERVE_N]) * 1e6 / g_settings->sizeCell;
	c->daily_fruit_cwdN        += (s->value[N_FRUIT_TO_CWD]   + s->value[DEAD_FRUIT_N])   * 1e6 / g_settings->sizeCell;
	c->daily_cwdN              += ((s->value[N_STEM_TO_CWD] +
			s->value[N_CROOT_TO_CWD]                        +
			s->value[N_BRANCH_TO_CWD]                       +
			s->value[N_RESERVE_TO_CWD]                      +
			s->value[N_FRUIT_TO_CWD]                        +
			s->value[DEAD_STEM_N]                           +
			s->value[DEAD_CROOT_N]                          +
			s->value[DEAD_BRANCH_N]                         +
			s->value[DEAD_RESERVE_N]                        +
			s->value[DEAD_FRUIT_N])                         *
			(1e6 / g_settings->sizeCell));

	/* cell level soil nitrogen fluxes (gN/m2/day) */
	c->daily_soilN       += 0. ;

	/* cell level litter nitrogen pools (gN/m2) */
	c->litrN                   += c->daily_litrN;
	c->cwdN                    += c->daily_cwdN ;
	c->soilN                   += c->daily_soilN ;

	/* leaf litter nitrogenfluxes */
	s->value[N_LEAF_TO_LITR1N]  = (s->value[N_LEAF_TO_LITR]  + s->value[DEAD_LEAF_N])  * s->value[LEAF_LITR_LAB_FRAC];
	s->value[N_LEAF_TO_LITR2N]  = (s->value[N_LEAF_TO_LITR]  + s->value[DEAD_LEAF_N])  * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[N_LEAF_TO_LITR3N]  = (s->value[N_LEAF_TO_LITR]  + s->value[DEAD_LEAF_N])  * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[N_LEAF_TO_LITR4N]  = (s->value[N_LEAF_TO_LITR]  + s->value[DEAD_LEAF_N])  * s->value[LEAF_LITR_LIGN_FRAC];

	/* fine root litter nitrogenfluxes */
	s->value[N_FROOT_TO_LITR1N] = (s->value[N_FROOT_TO_LITR] + s->value[DEAD_FROOT_N]) * s->value[FROOT_LITR_LAB_FRAC];
	s->value[N_FROOT_TO_LITR2N] = (s->value[N_FROOT_TO_LITR] + s->value[DEAD_FROOT_N]) * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[N_FROOT_TO_LITR3N] = (s->value[N_FROOT_TO_LITR] + s->value[DEAD_FROOT_N]) * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[N_FROOT_TO_LITR4N] = (s->value[N_FROOT_TO_LITR] + s->value[DEAD_FROOT_N]) * s->value[FROOT_LITR_LIGN_FRAC];

	/* cell level litter nitrogenfluxes */
	c->daily_leaf_litr1N       += s->value[N_LEAF_TO_LITR1N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr2N       += s->value[N_LEAF_TO_LITR2N]  * 1e6 / g_settings->sizeCell ;
	c->daily_leaf_litr3N       += s->value[N_LEAF_TO_LITR3N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr4N       += s->value[N_LEAF_TO_LITR4N]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr1N      += s->value[N_FROOT_TO_LITR1N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr2N      += s->value[N_FROOT_TO_LITR2N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr3N      += s->value[N_FROOT_TO_LITR3N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr4N      += s->value[N_FROOT_TO_LITR4N] * 1e6 / g_settings->sizeCell;

	/* cell level litter nitrogenpools (tC/sizecell) */
	c->leaf_litr1N             += c->daily_leaf_litr1N;
	c->leaf_litr2N             += c->daily_leaf_litr2N;
	c->leaf_litr3N             += c->daily_leaf_litr3N;
	c->leaf_litr4N             += c->daily_leaf_litr4N;
	c->froot_litr1N            += c->daily_froot_litr1N;
	c->froot_litr2N            += c->daily_froot_litr2N;
	c->froot_litr3N            += c->daily_froot_litr3N;
	c->froot_litr4N            += c->daily_froot_litr4N;
	//todo
	//c->daily_cwd1C              += ();

}
