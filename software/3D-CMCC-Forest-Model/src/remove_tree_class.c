/*
 * remove_tree_class.c
 *
 *  Created on: 02/set/2016
 *      Author: alessio-cmcc
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "constants.h"
#include "common.h"
#include "forest_tree_class.h"
#include "treemodel_daily.h"
#include "remove_tree_class.h"

static int species_remove(age_t *a, const int index) {
	int i;
	int y;

	assert(a);

	if ( index > a->species_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < a->species_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		a->species[y++] = a->species[i];
	}
	--a->species_count;
	++a->species_avail;

	return 1;
}

static int age_remove(dbh_t *d, const int index) {
	int i;
	int y;

	assert(d);

	if ( index > d->ages_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < d->ages_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		d->ages[y++] = d->ages[i];
	}
	--d->ages_count;
	++d->ages_avail;

	return 1;
}

static int dbh_remove(height_t *h, const int index) {
	int i;
	int y;

	assert(h);

	if ( index > h->dbhs_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < h->dbhs_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		h->dbhs[y++] = h->dbhs[i];
	}
	--h->dbhs_count;
	++h->dbhs_avail;

	return 1;
}

static int height_remove(cell_t *c, const int index) {
	int i;
	int y;

	assert(c);

	if ( index > c->heights_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < c->heights_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		c->heights[y++] = c->heights[i];
	}
	--c->heights_count;
	++c->heights_avail;

	return 1;
}

int tree_class_remove(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	height_t* h;
	dbh_t* d;
	age_t* a;
	species_t* s;

	assert(c);

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &d->ages[age];
	s = &a->species[species];

	if ( ! s->counter[N_TREE] )
	{
		if ( ! species_remove(a, species) )	return 0;
		if ( ! age_remove(d, age) )			return 0;
		if ( ! dbh_remove(h, dbh) )			return 0;
		if ( ! height_remove(c, height) )	return 0;
	}

	return 1;
}

