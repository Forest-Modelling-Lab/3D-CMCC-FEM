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
#include "mpfit.h"
#include "common.h"
#include "leaf_fall.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
//extern logger_t* g_debug_log;

void littering ( cell_t *const c, species_t *const s )
{
	/* leaf, fine root, branch daily flux for litter production */

	/** carbon littering **/

	/* cell level litter carbon fluxes */
	c->daily_leaf_litrC        += s->value[C_LEAF_TO_LITR]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_litrC       += s->value[C_FROOT_TO_LITR] * 1e6 / g_settings->sizeCell;
	c->daily_fruit_litrC       += s->value[C_FRUIT_TO_LITR] * 1e6 / g_settings->sizeCell;
	c->daily_litrC             += (c->daily_leaf_litrC + c->daily_froot_litrC + c->daily_fruit_litrC);
	/* cell level cwd carbon fluxes */
	c->daily_branch_cwdC       += s->value[C_BRANCH_TO_CWD] * 1e6 / g_settings->sizeCell;
	c->daily_cwdC              += c->daily_branch_cwdC /* + ....*/;


	/* cell level litter carbon pools */
	c->leaf_litrC              += c->daily_leaf_litrC;
	c->froot_litrC             += c->daily_froot_litrC;
	c->fruit_litrC             += c->daily_fruit_litrC;
	c->litr_carbon             += c->daily_litrC;
	c->litr_tC                 += c->daily_litrC             / 1e6 * g_settings->sizeCell;
	/* cell level cwd carbon pools */
	c->cwd_carbon              += c->daily_cwdC;
	c->cwd_tC                  += c->daily_cwdC              / 1e6 * g_settings->sizeCell;

	/* leaf litter carbon fluxes */
	s->value[C_LEAF_TO_LITR1C]  = s->value[C_LEAF_TO_LITR]   * s->value[LEAF_LITR_LAB_FRAC];
	s->value[C_LEAF_TO_LITR2C]  = s->value[C_LEAF_TO_LITR]   * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[C_LEAF_TO_LITR3C]  = s->value[C_LEAF_TO_LITR]   * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[C_LEAF_TO_LITR4C]  = s->value[C_LEAF_TO_LITR]   * s->value[LEAF_LITR_LIGN_FRAC];

	/* fine root litter carbon fluxes */
	s->value[C_FROOT_TO_LITR1C] = s->value[C_FROOT_TO_LITR]  * s->value[FROOT_LITR_LAB_FRAC];
	s->value[C_FROOT_TO_LITR2C] = s->value[C_FROOT_TO_LITR]  * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[C_FROOT_TO_LITR3C] = s->value[C_FROOT_TO_LITR]  * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[C_FROOT_TO_LITR4C] = s->value[C_FROOT_TO_LITR]  * s->value[FROOT_LITR_LIGN_FRAC];

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
	//c->cwd_carbon              += ();

	/** nitrogen littering **/

	/* cell level litter nitrogen fluxes */
	c->daily_leaf_litrN        += s->value[N_LEAF_TO_LITR]    * 1e6 / g_settings->sizeCell;
	c->daily_froot_litrN       += s->value[N_FROOT_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->daily_fruit_litrN       += s->value[N_FRUIT_TO_LITR]   * 1e6 / g_settings->sizeCell;
	c->daily_litrN             += (c->daily_leaf_litrN + c->daily_froot_litrN + c->daily_fruit_litrN);
	/* cell level cwd nitrogen fluxes */
	c->daily_branch_cwdN       += s->value[N_BRANCH_TO_CWD]   * 1e6 / g_settings->sizeCell;
	c->daily_cwdN              += c->daily_branch_cwdN /* + ....*/;

	/* cell level cwd carbon pools */
	c->leaf_litrN              += c->daily_leaf_litrN;
	c->froot_litrN             += c->daily_froot_litrN;
	c->fruit_litrN             += c->daily_fruit_litrN;
	c->litr_nitrogen           += c->daily_litrN;
	c->litr_tN                 += c->daily_litrN              / 1e6 * g_settings->sizeCell;
	/* cell level cwd nitrogen pools */
	c->cwd_nitrogen            += c->daily_cwdN;
	c->cwd_tN                  += c->daily_cwdN               / 1e6 * g_settings->sizeCell;

	/* leaf litter nitrogen fluxes */
	s->value[N_LEAF_TO_LITR1N]  = s->value[N_LEAF_TO_LITR]    * s->value[LEAF_LITR_LAB_FRAC];
	s->value[N_LEAF_TO_LITR2N]  = s->value[N_LEAF_TO_LITR]    * s->value[LEAF_LITR_USCEL_FRAC];
	s->value[N_LEAF_TO_LITR3N]  = s->value[N_LEAF_TO_LITR]    * s->value[LEAF_LITR_SCEL_FRAC];
	s->value[N_LEAF_TO_LITR4N]  = s->value[N_LEAF_TO_LITR]    * s->value[LEAF_LITR_LIGN_FRAC];

	/* fine root litter carbon fluxes */
	s->value[N_FROOT_TO_LITR1N] = s->value[N_FROOT_TO_LITR]   * s->value[FROOT_LITR_LAB_FRAC];
	s->value[N_FROOT_TO_LITR2N] = s->value[N_FROOT_TO_LITR]   * s->value[FROOT_LITR_USCEL_FRAC];
	s->value[N_FROOT_TO_LITR3N] = s->value[N_FROOT_TO_LITR]   * s->value[FROOT_LITR_SCEL_FRAC];
	s->value[N_FROOT_TO_LITR4N] = s->value[N_FROOT_TO_LITR]   * s->value[FROOT_LITR_LIGN_FRAC];

	/* cell level litter nitrogen fluxes */
	c->daily_leaf_litr1N       += s->value[N_LEAF_TO_LITR1N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr2N       += s->value[N_LEAF_TO_LITR2N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr3N       += s->value[N_LEAF_TO_LITR3N]  * 1e6 / g_settings->sizeCell;
	c->daily_leaf_litr4N       += s->value[N_LEAF_TO_LITR4N]  * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr1N      += s->value[N_FROOT_TO_LITR1N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr2N      += s->value[N_FROOT_TO_LITR2N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr3N      += s->value[N_FROOT_TO_LITR3N] * 1e6 / g_settings->sizeCell;
	c->daily_froot_litr4N      += s->value[N_FROOT_TO_LITR4N] * 1e6 / g_settings->sizeCell;

	/* cell level litter nitrogen pools (tN/sizecell) */
	c->leaf_litr1N             += c->daily_leaf_litr1N;
	c->leaf_litr2N             += c->daily_leaf_litr2N;
	c->leaf_litr3N             += c->daily_leaf_litr3N;
	c->leaf_litr4N             += c->daily_leaf_litr4N;
	c->froot_litr1N            += c->daily_froot_litr1N;
	c->froot_litr2N            += c->daily_froot_litr2N;
	c->froot_litr3N            += c->daily_froot_litr3N;
	c->froot_litr4N            += c->daily_froot_litr4N;
	//todo
	//c->cwd_nitrogen

}
