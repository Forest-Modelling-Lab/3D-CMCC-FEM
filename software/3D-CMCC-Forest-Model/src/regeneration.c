/*
 * regeneration.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "logger.h"
#include "common.h"
#include "settings.h"
#include "regeneration.h"

extern logger_t* g_log;

void regeneration (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	int seeds_number;
	int saplings_number;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_log, "\n**REGENERATION**\n");

	/* compute number of seeds */
	seeds_number = s->value[FRUIT_C] / s->value[WEIGHTSEED];
	logger(g_log, "number of seeds = %d\n", seeds_number);

	/* reset annually fruit pool */
	s->value[FRUIT_C] = 0.;

	/* compute number of saplings based on germination capacity */
	saplings_number = seeds_number * s->value[GERMCAPACITY];
	logger(g_log, "number of saplings = %d\n", saplings_number);

	add_tree_class_for_regeneration ( c, &s->name, saplings_number, s->value[PHENOLOGY] );

}




