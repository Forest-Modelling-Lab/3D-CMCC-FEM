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
#include <string.h>
#include "matrix.h"
#include "constants.h"
#include "logger.h"
#include "common.h"
#include "settings.h"
#include "regeneration.h"
#include "new_forest_tree_class.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;

void regeneration (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	double fruit_gDM;
	int seeds_number;
	int saplings_number;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**REGENERATION**\n");

	/* compute number of seeds */
	/*convert fruit pool from tC to gDM */
	fruit_gDM = s->value[FRUIT_C] * 1000000 * GC_GDM;
	logger(g_debug_log, "fruit_gDM = %g gDM\n", fruit_gDM);

	seeds_number = (int)(fruit_gDM / s->value[WEIGHTSEED]);
	logger(g_debug_log, "fruit biomass= %g tC\n", s->value[FRUIT_C]);
	logger(g_debug_log, "number of seeds = %d\n", seeds_number);

	/* reset annually fruit pool */
	s->value[FRUIT_C] = 0.;

	/* compute number of saplings based on germination capacity */
	saplings_number = (int)(seeds_number * s->value[GERMCAPACITY]);
	logger(g_debug_log, "number of saplings = %d\n", saplings_number);

	logger(g_debug_log, "name = %s\n", s->name);

	/* assign values */
	//ALESSIOC ALESSIOR FIXME PORCATA
	g_settings->regeneration_n_tree = saplings_number;

	/* it gets name of species that produces seeds */
	//ALESSIOC ALESSIOR FIXME PORCATA
	strncpy(g_settings->regeneration_species, ((const char*)s->name), SETTINGS_REGENERATION_SPECIES_MAX_SIZE-1);

	/* replanting tree class */
	if( g_settings->regeneration_n_tree )
	{
		if ( ! add_tree_class_for_regeneration( c ) )
		{
			logger(g_debug_log, "unable to add new regeneration class! (exit)\n");
			exit(1);
		}
	}
}




