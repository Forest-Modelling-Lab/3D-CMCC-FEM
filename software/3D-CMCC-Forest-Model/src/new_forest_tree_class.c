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
extern logger_t* g_debug_log;

static int fill_cell_for_replanting(cell_t *const c, const int height, const int dbh, const int age, const int species, const int species_index)
{

	char* p;
	height_t* h;
	dbh_t* d;
	age_t* a;
#if 0
	//old
	static height_t height   = { 0 };
	static dbh_t dbh         = { 0 };
	static age_t age         = { 0 };
	static species_t species = { 0 };

	assert(c);

	/* alloc memory for heights */
	if ( ! alloc_struct((void **)&c->heights, &c->heights_count, &c->heights_avail, sizeof(height_t)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1]       = height;
	c->heights[c->heights_count-1].value = g_settings->replanted[species_index].height;
	h = &c->heights[c->heights_count-1];

	/* alloc memory for dbhs */
	if ( ! alloc_struct((void **)&h->dbhs, &h->dbhs_count, &h->dbhs_avail, sizeof(dbh_t)) )
	{
		return 0;
	}
	h->dbhs[h->dbhs_count-1]       = dbh;
	h->dbhs[h->dbhs_count-1].value = g_settings->replanted[species_index].avdbh;
	d = &h->dbhs[h->dbhs_count-1];

	/* alloc memory for ages */
	if ( ! alloc_struct((void **)&d->ages, &d->ages_count, &d->ages_avail, sizeof(age_t)) )
	{
		return 0;
	}
	d->ages[d->ages_count-1]       = age;
	d->ages[d->ages_count-1].value = (int)g_settings->replanted[species_index].age;
	a = &d->ages[d->ages_count-1];

	/* alloc memory for species */
	if ( ! alloc_struct((void **)&a->species, &a->species_count, &a->species_avail, sizeof(species_t)) )
	{
		return 0;
	}

	p = string_copy(g_settings->replanted[species_index].species);
	if ( ! p ) return 0;

	a->species[a->species_count-1]                  = species;
	a->species[a->species_count-1].management       = T;
	a->species[a->species_count-1].name             = p;
	a->species[a->species_count-1].counter[N_TREE]  = (int)g_settings->replanted[species_index].n_tree;
	a->species[a->species_count-1].counter[N_STUMP] = 0;
	a->species[a->species_count-1].value[LAI_PROJ]  = g_settings->replanted[species_index].lai;
#else

	//new
	assert(c);

	printf("Replanted species %s\n", g_settings->replanted[species_index].species);

	/* alloc memory for heights */
//	if ( ! alloc_struct((void **)&c->heights, &c->heights_count, &c->heights_avail, sizeof(height_t)) )
//	{
//		return 0;
//	}

	c->heights[height].value = g_settings->replanted[species_index].height;
	h = &c->heights[height];

	/* alloc memory for dbhs */
//	if ( ! alloc_struct((void **)&h->dbhs, &h->dbhs_count, &h->dbhs_avail, sizeof(dbh_t)) )
//	{
//		return 0;
//	}
	h->dbhs[dbh].value = g_settings->replanted[species_index].avdbh;
	d = &h->dbhs[dbh];

	/* alloc memory for ages */
//	if ( ! alloc_struct((void **)&d->ages, &d->ages_count, &d->ages_avail, sizeof(age_t)) )
//	{
//		return 0;
//	}
	d->ages[age].value = (int)g_settings->replanted[species_index].age;
	a = &d->ages[age];

	/* alloc memory for species */
//	if ( ! alloc_struct((void **)&a->species, &a->species_count, &a->species_avail, sizeof(species_t)) )
//	{
//		return 0;
//	}

	p = string_copy(g_settings->replanted[species_index].species);
	if ( ! p ) return 0;


	a->species[species].management       = T;
	a->species[species].name             = p;
	a->species[species].counter[N_TREE]  = (int)g_settings->replanted[species_index].n_tree;
	a->species[species].counter[N_STUMP] = 0;
	a->species[species].value[LAI_PROJ]  = g_settings->replanted[species_index].lai;

#endif

	return 1;
}

int add_tree_class_for_replanting (cell_t *const c, const int height, const int dbh, const int age, const int species, const int rsi, const int day, const int month, const int year)
{
	int day_temp;
	int month_temp;
	int DaysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	logger(g_debug_log, "**New tree class (REPLANTATION)**\n");

	/* it is used only with "human" regeneration */
	logger(g_debug_log, "Human management\n");

	if ( ! fill_cell_for_replanting ( c, height, dbh, age, species, rsi ) ) return 0;

	/* fill with species values from parameterization file */
	if ( ! fill_species_from_file ( &c->heights[height].dbhs[dbh].ages[age].species[species]) )
	{
		return 0;
	}

	/* check for veg days */
	for (month_temp = 0; month_temp < 12; ++month_temp)
	{
		/* for handling leap years */
		int days_per_month;

		days_per_month = DaysInMonth[month_temp];
		if ( (FEBRUARY == month_temp) && IS_LEAP_YEAR(c->years[year].year) )
		{
			++days_per_month;
		}

		for ( day_temp = 0; day_temp < days_per_month; ++day_temp )
		{
			/* compute annually the days for the growing season before any other process */
			Veg_Days ( c , day_temp, month_temp, year );
		}
	}

	/* reset years of simulation for new class */
	c->heights[height].dbhs[dbh].ages[age].species[species].counter[DOS] = 0;
	c->heights[height].dbhs[dbh].ages[age].species[species].counter[MOS] = 0;
	c->heights[height].dbhs[dbh].ages[age].species[species].counter[YOS] = 0;

	/* initialize new power function */
	allometry_power_function ( c );

	/* initialize new carbon pool fraction */
	carbon_pool_fraction ( c );

	/* initialize new forest structure */
	initialization_forest_structure (c , day, month, year);

	/* initialize new forest class pools */
	initialization_forest_class_C ( c, height, dbh, age, species );

	/* initialize new nitrogen pools */
	initialization_forest_class_N ( c, height, dbh, age, species );

	/* initialize new litter pools */
	initialization_forest_class_litter_soil ( c, height, dbh, age, species );

	/* print new forest class dataset */
	print_new_daily_forest_class_data( c, height, dbh, age, species );

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

	logger(g_debug_log, "\n**Fill cell from regeneration**\n");

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
	d->ages[d->ages_count-1].value = (int)g_settings->regeneration_age;
	a = &d->ages[d->ages_count-1];

	/* alloc memory for species */
	if ( ! alloc_struct((void **)&a->species, &a->species_count, &a->species_avail, sizeof(species_t)) )
	{
		return 0;
	}

	p = string_copy(g_settings->regeneration_species);
	if ( ! p ) return 0;

	a->species[a->species_count-1] = species;
	a->species[a->species_count-1].management = T;
	a->species[a->species_count-1].name = p;
	a->species[a->species_count-1].counter[N_TREE] = (int)g_settings->regeneration_n_tree;
	a->species[a->species_count-1].counter[N_STUMP] = 0;
	a->species[a->species_count-1].value[LAI_PROJ] = g_settings->regeneration_lai;
	logger(g_debug_log, "-height = %f\n", c->heights[c->heights_count-1].value);
	logger(g_debug_log, "-dbh = %f\n", h->dbhs[h->dbhs_count-1].value);
	logger(g_debug_log, "-age = %d\n", d->ages[d->ages_count-1].value);
	logger(g_debug_log, "-name = %s\n", a->species[a->species_count-1].name);
	logger(g_debug_log, "-N-tree = %d\n", a->species[a->species_count-1].counter[N_TREE]);
	logger(g_debug_log, "-lai = %f\n", a->species[a->species_count-1].value[LAI_PROJ]);


	return 1;
}

int add_tree_class_for_regeneration ( cell_t *const c )
{
	logger(g_debug_log, "**ADD NEW TREE CLASS (REGENERATION)**\n");

	/* it is used only with natural regeneration */
	if ( ! fill_cell_for_regeneration( c ) ) return 0;

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
	initialization_forest_class_C ( c, c->heights_count-1, 0, 0, 0 );

	/* initialize nitrogen pools */
	initialization_forest_class_N ( c, c->heights_count-1, 0, 0, 0 );

	/* initialize litter and soil pools */
	initialization_forest_class_litter_soil ( c, c->heights_count-1, 0, 0, 0 );

	/* print new forest class dataset */
	print_new_daily_forest_class_data( c, c->heights_count-1, 0, 0, 0 );

	return 1;
}
