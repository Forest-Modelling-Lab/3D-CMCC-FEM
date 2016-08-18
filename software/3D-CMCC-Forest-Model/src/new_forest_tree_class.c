/*new_forest_tree_class.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "management.h"
#include "constants.h"
#include "settings.h"
#include "common.h"
#include "logger.h"
#include "new_forest_tree_class.h"

extern settings_t* g_settings;
extern logger_t* g_log;

int add_new_tree_class (cell_t *const c, const int height, const int age, const int species)
{
	int i;
	int y;
	int z;
	int flag;
	height_t *h;
	age_t *a;
	species_t *s;

	logger(g_log, "**ADD NEW TREE CLASS**\n");

	/* it is used only with "human" regeneration */
	logger(g_log, "Human management\n");

	/* add height */
//ALESSIOR
//	if (!alloc_struct((void **)&c->heights, &c->heights_count, sizeof(height_t)) )
//	{
//		return 0;
//	}
	h = &c->heights[c->heights_count-1];
	h->value = g_settings->height_sapling;

	/* NON TOCCARE ! */
	h->ages = NULL;
	h->ages_count = 0;

	/* add age */
//ALESSIOR
//	if ( !alloc_struct((void **)&h->ages, &h->ages_count, sizeof(age_t)) )
//	{
//		return 0;
//	}
	a = &h->ages[h->ages_count-1];
	a->value = g_settings->age_sapling;
	a->species = NULL;
	a->species_count = 0;

	/* add species */
//ALESSIOR
//	if ( !alloc_struct((void **)&a->species, &a->species_count, sizeof(species_t)) )
//	{
//		return 0;
//	}
	s = &a->species[a->species_count-1];
	s->name = string_copy(g_settings->replanted_species);
	if ( ! s->name )
	{
		logger(g_log, "unable to copy replanted species from settins. out of memory.");
		return 0;
	}

	/* reset all values */
	for ( i = 0; i < VALUES; ++i ) {
		s->value[i] = INVALID_VALUE;
	}

	/* check number of species */
	flag = 0;
	for ( i = c->heights_count - 2; i >= 0 ; --i )
	{
		/* check for same species */
		for ( y = 0; y < c->heights[i].ages_count; ++y )
		{
			for ( z = 0; z < c->heights[y].ages[y].species_count; ++z )
			{
				if ( ! string_compare_i(c->heights[y].ages[y].species[z].name, s->name) )
				{
					for ( i = 0; i < AVDBH; ++i )
					{
						s->value[i] = c->heights[y].ages[y].species[z].value[i];
					}
					flag = 1;
					break;
				}
			}
			if ( flag ) break;
		}
		if ( flag ) break;
	}

	/* load from file */
	if ( ! flag )
	{
//ALESSIOR
//		if ( ! fill_species_from_file(s) )
//		{
//			return 0;
//		}
	}
	/* ALESSIOC fixme */
	s->value[DENMIN] = INVALID_VALUE;

	/* fill the structs with new variables incrementing counters */
	s->value[AVDBH] = g_settings->avdbh_sapling;
	s->counter[N_TREE] = g_settings->replanted_tree;

	logger(g_log, "**heights_count = %d \n", c->heights_count);
	logger(g_log, "**ages_count = %d \n", h->ages_count);
	logger(g_log, "**species_count = %d \n", a->species_count);
	logger(g_log, "**height_sapling = %f\n", h->value);
	logger(g_log, "**age_sapling = %d\n", a->value);
	logger(g_log, "**avdbh sampling = %f\n", s->value[AVDBH]);
	logger(g_log, "**n tree %d of %s\n", s->counter[N_TREE], s->name);

	//FIXME add initialisation data

	return 1;
}
