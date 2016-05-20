/* soil.c */
#include "soil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "types.h"
#include "netcdf.h"

static int import_txt(soil_t *const s, const char *const filename, const int x, const int y) {
#define TOPO_BUFFER_SIZE 1024
	char buffer[TOPO_BUFFER_SIZE];
	int i;
	double *p_float;
	FILE *f;

	assert(s && filename);

	f = fopen(filename, "r");
	if ( ! f ) {
		return 1;
	}

	i = 0;
	p_float = s->values;	
	while ( fgets(buffer, TOPO_BUFFER_SIZE, f) ) {
		/* skip empty line */
		if ( ('\n' == buffer[0]) || ('/' == buffer[0]) ) {
			continue;
		} else {
			char *p = strtok(buffer, " \"");
			p = strtok(NULL, "\"");
			switch ( i++ ) {
				case 0:
					strcpy(s->sitename, p);
				break;

				default:
					*p_float = atof(p);
					p_float++;
				break;
			}
		}
	}

	/* are all value imported ? */
	/*return ( i == TOPO_VALUES_COUNT ) ? 0 : 2;*/
	return 0;
#undef TOPO_BUFFER_SIZE
}

static int import_nc(soil_t *const t, const char *const filename, const int x, const int y) {
	return 0;
}

soil_t* soil_new(void) {
	soil_t* s;

	s = malloc(sizeof*s);
	if ( s ) {
		soil_reset(s);
	}
	return s;
}

void soil_reset(soil_t* const s) {
	int i;
	assert(s);
	s->sitename[0] = '\0';
	for ( i = 0; i < SOIL_VALUES_COUNT; ++i ) {
		s->values[i] = INVALID_VALUE;
	}
}

/*
	return 0 if ok
	return 1 if file do not exist
	return 2 if file is not imported correctly
*/
int soil_import(soil_t *const s, const char *const filename, const int x, const int y) {
	char *p = strrchr(filename, '.');
	if ( p ) {
		if ( ! string_compare_i(++p, "nc") ) {
			return import_nc(s, filename, x, y);
		}
	}
	return import_txt(s, filename, x, y);
}