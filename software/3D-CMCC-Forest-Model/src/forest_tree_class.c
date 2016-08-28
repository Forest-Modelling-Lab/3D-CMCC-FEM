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
#include "initialization.h"
#include "structure.h"
#include "forest_tree_class.h"

extern settings_t* g_settings;
extern logger_t* g_log;

static int fill_cell(cell_t *const c)
{
	char* p;
	height_t* h;
	age_t* a;

	static height_t height = { 0 };
	static age_t age = { 0 };
	static species_t species = { 0 };

	assert(c);

	CHECK_CONDITION(g_settings->replanted_management, != 0);
	
	//alloc memory for heights
	if ( ! alloc_struct((void **)&c->heights, &c->heights_count, sizeof(height_t)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1] = height;
	//ALESSIOC change height_sapling in replanted_height
	c->heights[c->heights_count-1].value = g_settings->replanted_height;

	h = &c->heights[c->heights_count-1];
			
	if ( ! alloc_struct((void **)&h->ages, &h->ages_count, sizeof(age_t)) )
	{
		return 0;
	}
	h->ages[h->ages_count-1] = age;
	h->ages[h->ages_count-1].value = g_settings->replanted_age;
	a = &h->ages[h->ages_count-1];

	if ( ! alloc_struct((void **)&a->species, &a->species_count, sizeof(species_t)) )
	{
		return 0;
	}

	p = string_copy(g_settings->replanted_species);
	if ( ! p ) return 0;

	a->species[a->species_count-1] = species;
	// ALESSIOR fix, must use e_management 
	a->species[a->species_count-1].management = 0; /* T */
	a->species[a->species_count-1].name = p;
	a->species[a->species_count-1].counter[N_TREE] = g_settings->replanted_n_tree;
	a->species[a->species_count-1].counter[N_STUMP] = 0;
	a->species[a->species_count-1].value[AVDBH] = g_settings->replanted_avdbh;
	a->species[a->species_count-1].value[LAI] = g_settings->replanted_lai;
	a->species[a->species_count-1].turnover = NULL; //malloc(a->species_count*sizeof*a->species[a->species_count-1].turnover);
	//if ( ! a->species[a->species_count-1].turnover ) return 0;

	return 1;
}

int add_tree_class (cell_t *const c, const int height, const int age, const int species)
{
//	int i;
//	int y;
//	int z;
//	int flag;
//	height_t *h;
//	age_t *a;
//	species_t *s;

	logger(g_log, "**ADD NEW TREE CLASS**\n");

	/* it is used only with "human" regeneration */
	logger(g_log, "Human management\n");

	if ( ! fill_cell(c) ) return 0;

	/*
	logger(g_log, "**heights_count = %d \n", c->heights_count);
	logger(g_log, "**ages_count = %d \n", h->ages_count);
	logger(g_log, "**species_count = %d \n", a->species_count);
	logger(g_log, "**height_sapling = %f\n", h->value);
	logger(g_log, "**replanted_age = %d\n", a->value);
	logger(g_log, "**avdbh sampling = %f\n", s->value[AVDBH]);
	logger(g_log, "**n tree %d of %s\n", s->counter[N_TREE], s->name);
	*/

	/* initialise new forest class pools */
	initialization_forest_C_biomass( c, height, age, species );

	return 1;
}
