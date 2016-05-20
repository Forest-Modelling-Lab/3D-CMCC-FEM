/* topo.c */
#include "topo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

topo_t *topo_import(const char *const filename, int *const err) {
#define TOPO_BUFFER_SIZE	1024

	FILE *f;
	topo_t *t;

	assert(filename && err);

	*err = 0;

	f = fopen(filename, "r");
	if ( ! f ) {
		*err = 2;
		return NULL;
	}

	t = malloc(sizeof*t);
	if ( ! t ) {
		*err = 1;
		fclose(f);
		return NULL;
	}

	{
		int i = 0;
		float *p_float = t->values;
		char *buffer = malloc(TOPO_BUFFER_SIZE*sizeof*buffer);
		if ( ! buffer ) {
			*err = 1;
			free(t);
			fclose(f);
			return NULL;
		}

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
		free(buffer);

		/* are all value imported ? */
		if ( i != TOPO_VALUES_COUNT ) {
			*err = 3;
			free(t);			
			t = NULL;
		}
	}
	fclose(f);
	return t;

#undef TOPO_BUFFER_SIZE
}