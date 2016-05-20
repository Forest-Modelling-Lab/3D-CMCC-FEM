/* topo.c */
#include "topo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "types.h"
#include "netcdf.h"

static int import_txt(topo_t *const t, const char *const filename, const int x, const int y) {
#define TOPO_BUFFER_SIZE 1024
	char buffer[TOPO_BUFFER_SIZE];
	int i;
	float *p_float;
	FILE *f;

	assert(t && filename);

	f = fopen(filename, "r");
	if ( ! f ) {
		return 1;
	}

	i = 0;
	p_float = t->values;	
	while ( fgets(buffer, TOPO_BUFFER_SIZE, f) ) {
		/* skip empty line */
		if ( ('\n' == buffer[0]) || ('/' == buffer[0]) ) {
			continue;
		} else {
			char *p = strtok(buffer, " \"");
			p = strtok(NULL, "\"");

			*p_float = (float)atof(p);
			p_float++;
			i++;
		}
	}

	/* are all value imported ? */
	/*return ( i == TOPO_VALUES_COUNT ) ? 0 : 2;*/
	return 0;
#undef TOPO_BUFFER_SIZE
}

static int import_nc(topo_t *const t, const char *const filename, const int x, const int y) {
	return 0;
}

topo_t* topo_new(void) {
	topo_t * t;

	t = malloc(sizeof*t);
	if ( t ) {
		topo_reset(t);
	}
	return t;
}

void topo_reset(topo_t* const t) {
	int i;
	assert(t);
	for ( i = 0; i < TOPO_VALUES_COUNT; ++i ) {
		t->values[i] = INVALID_VALUE;
	}
}

/*
	return 0 if ok
	return 1 if file do not exist
	return 2 if file is not imported correctly
*/
int topo_import(topo_t *const t, const char *const filename, const int x, const int y) {
	char *p;
	assert(t);
	p = strrchr(filename, '.');
	if ( p ) {
		if ( ! string_compare_i(++p, "nc") ) {
			return import_nc(t, filename, x, y);
		}
	}
	return import_txt(t, filename, x, y);
}