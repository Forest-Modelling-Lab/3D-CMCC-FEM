/*
 * litter_model.c
 *
 *  Created on: 22 mar 2017
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "settings.h"
#include "decomposition.h"

int Litter_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	/* shortcuts */
	cell_t *c;
	meteo_daily_t *meteo_daily;

	/* assign shortcuts */
	c = &m->cells[cell];
	meteo_daily = &m->cells[cell].years[year].m[month].d[day];

	/* check parameters */
	assert(m);

	/* reconciling litter pools */

	/*** LITTER CARBON POOLS ***/
	/* cell level litter carbon pools (tC/sizecell) */
	c->leaf_litrC              += c->daily_leaf_litrC;
	c->froot_litrC             += c->daily_froot_litrC;
	c->leaf_litr1C             += c->daily_leaf_litr1C;
	c->leaf_litr2C             += c->daily_leaf_litr2C;
	c->leaf_litr3C             += c->daily_leaf_litr3C;
	c->leaf_litr4C             += c->daily_leaf_litr4C;
	c->froot_litr1C            += c->daily_froot_litr1C;
	c->froot_litr2C            += c->daily_froot_litr2C;
	c->froot_litr3C            += c->daily_froot_litr3C;
	c->froot_litr4C            += c->daily_froot_litr4C;
	c->litrC                   += (c->daily_leaf_litrC + c->daily_froot_litrC);
	//todo include cwdC

	/*** LITTER NITROGEN POOLS ***/
	/* cell level litter nitrogen pools (tN/sizecell) */
	c->leaf_litrN              += c->daily_leaf_litrN;
	c->leaf_litr1N             += c->daily_leaf_litr1N;
	c->leaf_litr2N             += c->daily_leaf_litr2N;
	c->leaf_litr3N             += c->daily_leaf_litr3N;
	c->leaf_litr4N             += c->daily_leaf_litr4N;
	c->froot_litrN             += c->daily_froot_litrN;
	c->froot_litr1N            += c->daily_froot_litr1N;
	c->froot_litr2N            += c->daily_froot_litr2N;
	c->froot_litr3N            += c->daily_froot_litr3N;
	c->froot_litr4N            += c->daily_froot_litr4N;
	c->litrN                   += (c->daily_leaf_litrN + c->daily_froot_litrN);
	//todo include cwdN


	/* decomposition */
	decomposition ( c, meteo_daily );

	/* ok */
	return 1;
}
