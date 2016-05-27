/* soil.c */
#include "soil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "types.h"
#include "netcdf.h"
#include "logger.h"

extern logger_t* g_log;

extern char *g_sz_program_path;

static const char *sz_vars[SOIL_VARS_COUNT] = {
	"LAT"
	, "LON"
	, "CLAY_PERC"
	, "SILT_PERC"
	, "SAND_PERC"
	, "SOIL_DEPTH"
	, "FR"
	, "FN0"
	, "FNN"
	, "M0"
	, "SN"
	, "PH"
	, "NO3"
	, "NH4"
	, "HYDRAULIC_CONDUCTIVITY"
	, "SOC"
	, "LIT_FRACT"
	, "HUMA_FRACT"
	, "HUMU_FRACT"
	, "BIO_FRACT"
	, "RCNRVL"
	, "RCNRL"
	, "RCNRR"
	, "RCNB"
	, "RCNH"
	, "RCNM"
	, "RCNH2"
	, "DC_LITTER"
	, "DC_HUMADS"
	, "DC_HUMUS"
};

static void reset(soil_t* const s) {
	int i;
	assert(s);
	s->sitename[0] = '\0';
	for ( i = 0; i < SOIL_VARS_COUNT; ++i ) {
		s->values[i] = INVALID_VALUE;
	}
}

static int import_txt(soil_t *const s, const char *const filename, const int x, const int y) {
#define SOIL_BUFFER_SIZE 1024
	char buffer[SOIL_BUFFER_SIZE];
	int i;
	double *p_float;
	FILE *f;

	assert(s && filename);

	f = fopen(filename, "r");
	if ( ! f ) {
		return 0;
	}

	i = 0;
	p_float = s->values;	
	while ( fgets(buffer, SOIL_BUFFER_SIZE, f) ) {
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
	/*return ( i == SOIL_VALUES_COUNT ) ? 0 : 2;*/
	return 1;
#undef SOIL_BUFFER_SIZE
}

static int import_nc(soil_t *const s, const char *const filename, const int x_cell, const int y_cell) {
	enum {
		X_DIM = 0
		, Y_DIM
		, TIME_DIM
		, DIMS_COUNT
	};

	char buffer[256];
	char sz_nc_filename[256];
	int i;
	int rows_count;
	int vars[SOIL_VARS_COUNT];
	float value;
	int dims_size[DIMS_COUNT];
	const char *sz_lat = "lat";
	const char *sz_lon = "lon";
	const char *sz_time = "time";
	const char *sz_dims[DIMS_COUNT] = { "x", "y", "time" };

	int y;
	int id_file;
	int ret;
	int dims_count;	/* dimensions */
	int vars_count;
	int atts_count;	/* attributes */
	int unl_count;	/* unlimited dimensions */
	char name[NC_MAX_NAME+1];
	nc_type type;
	size_t size;
	int n_dims;
	int ids[NC_MAX_VAR_DIMS];

	size_t start[] = { 0, 0, 0 };
	size_t count[] = { 0, 1, 1 };

	rows_count = 0;
	for ( i = 0; i < SOIL_VARS_COUNT; i++ ) {
		vars[i] = 0;
	}

	sz_nc_filename[0] = '\0';
	if ( filename[1] != ':' ) {
		strncpy(sz_nc_filename, g_sz_program_path, 256);
	}
	strcat(sz_nc_filename, filename);

	ret = nc_open(sz_nc_filename, NC_NOWRITE, &id_file);
	if ( ret != NC_NOERR ) goto quit;

	ret = nc_inq(id_file, &dims_count, &vars_count, &atts_count, &unl_count);
	if ( ret != NC_NOERR ) goto quit;

	if ( ! dims_count || ! vars_count ) {
		logger(g_log, "bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		goto quit2;
	}

	/* reset */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		dims_size[i] = -1;
	}

	/* get dimensions */
	for ( i = 0; i < dims_count; ++i ) {
		ret = nc_inq_dim(id_file, i, name, &size);
		if ( ret != NC_NOERR ) goto quit;
		for ( y = 0; y < DIMS_COUNT; ++y ) {
			if ( ! string_compare_i(sz_dims[y], name) ) {
				if ( dims_size[y] != -1 ) {
					logger(g_log, "dimension %s already found!\n", sz_dims[y]);
					goto quit2;
				}
				dims_size[y] = size;
				break;
			}
		}
	}

	/* check if we have all dimensions */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		if ( -1 == dims_size[i] ) {
			logger(g_log, "dimension %s not found!\n", sz_dims[i]);
			goto quit2;
		}
	}

	/* check if time is 1 */
	if ( dims_size[TIME_DIM] != 1 ) {
		logger(g_log, "time size must be 1\n");
		goto quit2;
	}

	/* check if x_cell is >= x_dim */
	if ( x_cell >= dims_size[X_DIM] ) {
		logger(g_log, "x_cell >= x_dim: %d,%d\n", x_cell, dims_size[X_DIM]);
		goto quit2;
	}

	/* check if y_cell is >= y_dim */
	if ( y_cell >= dims_size[Y_DIM] ) {
		logger(g_log, "y_cell >= y_dim: %d,%d\n", y_cell, dims_size[Y_DIM]);
		goto quit2;
	}

	start[1] = y_cell;
	start[2] = x_cell;
	count[0] = dims_size[TIME_DIM];

	/* get vars */
	for ( i = 0; i < vars_count; ++i ) {
		ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
		if ( ret != NC_NOERR ) goto quit;
		for ( y = 0; y  < SOIL_VARS_COUNT; ++y ) {
			if ( ! string_compare_i(name, sz_vars[y]) ) {
				/* check if we already have imported that var */
				if ( vars[y] ) {
					logger(g_log, "var %s already imported\n", sz_vars[y]);
					goto quit2;
				}
				/* get values */
				if ( NC_FLOAT == type ) {
					ret = nc_get_vara_float(id_file, i, start, count, &value);
					if ( ret != NC_NOERR ) goto quit;
					s->values[y] = value;
				} else if ( NC_DOUBLE == type ) {
					ret = nc_get_vara_double(id_file, i, start, count, &s->values[y]);
					if ( ret != NC_NOERR ) goto quit;
				} else {
					/* type format not supported! */
					logger(g_log, "type format in %s for %s column not supported\n\n", buffer, sz_vars[y]);
					goto quit2;
				}
				vars[y] = 1;
				break;
			}
		}
	}
	nc_close(id_file);
	return 1;

quit:
	logger(g_log, nc_strerror(ret));
quit2:
	nc_close(id_file);
	return 0;
}

soil_t* soil_new(void) {
	soil_t* s = malloc(sizeof*s);
	if ( s ) reset(s);
	return s;
}

int soil_import(soil_t *const s, const char *const filename, const int x, const int y) {
	char *p;
	assert(s);
	reset(s);
	p = strrchr(filename, '.');
	if ( p ) {
		if ( ! string_compare_i(++p, "nc") ) {
			return import_nc(s, filename, x, y);
		}
	}
	return import_txt(s, filename, x, y);
}
