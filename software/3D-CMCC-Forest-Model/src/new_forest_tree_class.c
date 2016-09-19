/*new_forest_tree_class.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <new_forest_tree_class.h>
#include "matrix.h"
#include "management.h"
#include "constants.h"
#include "settings.h"
#include "common.h"
#include "logger.h"
#include "initialization.h"
#include "structure.h"
#include "biomass.h"
#include "allometry.h"
#include "g-function.h"
#include "print.h"

extern settings_t* g_settings;
extern logger_t* g_log;

static int fill_cell_for_replanting(cell_t *const c)
{
	char* p;
	height_t* h;
	dbh_t* d;
	age_t* a;

	static height_t height = { 0 };
	static dbh_t dbh = { 0 };
	static age_t age = { 0 };
	static species_t species = { 0 };

	assert(c);
	
	/* alloc memory for heights */
	if ( ! alloc_struct((void **)&c->heights, &c->heights_count, &c->heights_avail, sizeof(height_t)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1] = height;
	c->heights[c->heights_count-1].value = g_settings->replanted_height;
	h = &c->heights[c->heights_count-1];

	/* alloc memory for dbhs */
	if ( ! alloc_struct((void **)&h->dbhs, &h->dbhs_count, &h->dbhs_avail, sizeof(dbh_t)) )
	{
		return 0;
	}
	h->dbhs[h->dbhs_count-1] = dbh;
	h->dbhs[h->dbhs_count-1].value = g_settings->replanted_avdbh;
	d = &h->dbhs[h->dbhs_count-1];

	/* alloc memory for ages */
	if ( ! alloc_struct((void **)&d->ages, &d->ages_count, &d->ages_avail, sizeof(age_t)) )
	{
		return 0;
	}
	d->ages[d->ages_count-1] = age;
	d->ages[d->ages_count-1].value = (int)g_settings->replanted_age;
	a = &d->ages[d->ages_count-1];

	/* alloc memory for species */
	if ( ! alloc_struct((void **)&a->species, &a->species_count, &a->species_avail, sizeof(species_t)) )
	{
		return 0;
	}

	p = string_copy(g_settings->replanted_species);
	if ( ! p ) return 0;

	a->species[a->species_count-1] = species;
	// ALESSIOR fix, must use e_management 
	a->species[a->species_count-1].management = 0; /* T */
	a->species[a->species_count-1].name = p;
	a->species[a->species_count-1].counter[N_TREE] = (int)g_settings->replanted_n_tree;
	a->species[a->species_count-1].counter[N_STUMP] = 0;
	a->species[a->species_count-1].value[LAI] = g_settings->replanted_lai;

	return 1;
}

int add_tree_class_for_replanting (cell_t *const c)
{
	logger(g_log, "**ADD NEW TREE CLASS (REPLANTATION)**\n");

	/* it is used only with "human" regeneration */
	logger(g_log, "Human management\n");

	if ( ! fill_cell_for_replanting(c) ) return 0;

	/* fill with species values from parameterization file */
	if ( ! fill_species_from_file(&c->heights[c->heights_count-1].dbhs[0].ages[0].species[0]) )
	{
		return 0;
	}

	/* initialize power function */
	allometry_power_function ( c );

	/* initialize carbon pool fraction */
	carbon_pool_fraction ( c );

	/* initialize new forest class pools */
	initialization_forest_class_C_biomass( c, c->heights_count-1, 0, 0, 0 );

	/* initialise nitrogen pools */
	initialization_forest_class_N_biomass ( c, c->heights_count-1, 0, 0, 0 );

	/* print new forest class dataset */
	print_new_daily_forest_class_data( c, c->heights_count-1, 0, 0, 0 );

	return 1;
}
/************************************************************************************************************/

/************************************************************************************************************/

static int fill_cell_for_regeneration( cell_t *const c )
{
	char* p;

	height_t* h;
	dbh_t* d;
	age_t* a;

	static height_t height = { 0 };
	static dbh_t dbh = { 0 };
	static age_t age = { 0 };
	static species_t species = { 0 };


	assert(c);

	logger(g_log, "\n**Fill cell from regeneration**\n");


	/* alloc memory for heights */
	if ( ! alloc_struct((void **)&c->heights, &c->heights_count, &c->heights_avail, sizeof(height_t)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1] = height;
	c->heights[c->heights_count-1].value = g_settings->regeneration_height;
	h = &c->heights[c->heights_count-1];

	/* alloc memory for dbhs */
	if ( ! alloc_struct((void **)&h->dbhs, &h->dbhs_count, &h->dbhs_avail, sizeof(dbh_t)) )
	{
		return 0;
	}
	h->dbhs[h->dbhs_count-1] = dbh;
	h->dbhs[h->dbhs_count-1].value = g_settings->regeneration_avdbh;
	d = &h->dbhs[h->dbhs_count-1];

	/* alloc memory for ages */
	if ( ! alloc_struct((void **)&d->ages, &d->ages_count, &d->ages_avail, sizeof(age_t)) )
	{
		return 0;
	}
	d->ages[d->ages_count-1] = age;
	d->ages[d->ages_count-1].value = g_settings->regeneration_age;
	a = &d->ages[d->ages_count-1];

	/* alloc memory for species */
	if ( ! alloc_struct((void **)&a->species, &a->species_count, &a->species_avail, sizeof(species_t)) )
	{
		return 0;
	}

	p = string_copy(g_settings->regeneration_species);
	if ( ! p ) return 0;

	a->species[a->species_count-1] = species;
	// ALESSIOR fix, must use e_management
	a->species[a->species_count-1].management = 0; /* T */
	a->species[a->species_count-1].name = p;
	a->species[a->species_count-1].counter[N_TREE] = g_settings->regeneration_n_tree;
	a->species[a->species_count-1].counter[N_STUMP] = 0;
	a->species[a->species_count-1].value[LAI] = g_settings->regeneration_lai;
	logger(g_log, "-height = %f\n", c->heights[c->heights_count-1].value);
	logger(g_log, "-dbh = %f\n", h->dbhs[h->dbhs_count-1].value);
	logger(g_log, "-age = %d\n", d->ages[d->ages_count-1].value);
	logger(g_log, "-name = %s\n", a->species[a->species_count-1].name);
	logger(g_log, "-N-tree = %d\n", a->species[a->species_count-1].counter[N_TREE]);
	logger(g_log, "-lai = %f\n", a->species[a->species_count-1].value[LAI]);


	return 1;
}

int add_tree_class_for_regeneration ( cell_t *const c )
{
	logger(g_log, "**ADD NEW TREE CLASS (REGENERATION)**\n");


	/* it is used only with natural regeneration */
	if ( ! fill_cell_for_regeneration( c ) ) return 0;getchar();

	/* fill with species values from parameterization file */
	if ( ! fill_species_from_file(&c->heights[c->heights_count-1].dbhs[0].ages[0].species[0]) )
	{
		return 0;
	}

	/* initialize power function */
	allometry_power_function ( c );

	/* initialize carbon pool fraction */
	carbon_pool_fraction ( c );

	/* initialize new forest class pools */
	initialization_forest_class_C_biomass( c, c->heights_count-1, 0, 0, 0 );

	/* initialize nitrogen pools */
	initialization_forest_class_N_biomass ( c, c->heights_count-1, 0, 0, 0 );

	/* print new forest class dataset */
	print_new_daily_forest_class_data( c, c->heights_count-1, 0, 0, 0 );

	return 1;
}
