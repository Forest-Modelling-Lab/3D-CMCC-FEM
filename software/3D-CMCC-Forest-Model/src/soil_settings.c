/* soil_settings.c */
#include "soil_settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "matrix.h"
#include "common.h"
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

static void reset(soil_settings_t* const s) {
	int i;
	assert(s);
	s->sitename[0] = '\0';
	for ( i = 0; i < SOIL_VARS_COUNT; ++i ) {
		s->values[i] = INVALID_VALUE;
	}
}

static int import_txt(soil_settings_t *const s, const char *const filename, const int x, const int y) {
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

static int import_nc(soil_settings_t *const s, const char *const filename, const int x_cell, const int y_cell) {
	enum {
		X_DIM = 0
		, Y_DIM
		, DIMS_COUNT
	};

	char buffer[256];
	char sz_nc_filename[256];
	int i;
	int vars[SOIL_VARS_COUNT];
	float value;
	int dims_size[DIMS_COUNT];
	int x_id;
	const char *sz_dims[DIMS_COUNT] = { "x", "y" };

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

	size_t start[DIMS_COUNT] = { 0, 0 };
	size_t count[DIMS_COUNT] = { 1, 1 };

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
		goto quit_no_nc_err;
	}

	/* dims_count can be only 2 and ids only x and y */
	if ( 2 != dims_count ) {
		logger(g_log, "bad dimension size. It should be 2 not %d\n", dims_count);
		goto quit_no_nc_err;
	}

	/* reset */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		dims_size[i] = -1;
	}

	/* get dimensions */
	x_id = 0;
	for ( i = 0; i < dims_count; ++i ) {
		ret = nc_inq_dim(id_file, i, name, &size);
		if ( ret != NC_NOERR ) goto quit;
		for ( y = 0; y < DIMS_COUNT; ++y ) {
			if ( ! string_compare_i(sz_dims[y], name) ) {
				if ( dims_size[y] != -1 ) {
					logger(g_log, "dimension %s already found!\n", sz_dims[y]);
					goto quit_no_nc_err;
				}
				dims_size[y] = size;
				if ( X_DIM == y ) x_id = i;
				break;			
			}
		}
	}

	/* check if we have all dimensions */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		if ( -1 == dims_size[i] ) {
			logger(g_log, "dimension %s not found!\n", sz_dims[i]);
			goto quit_no_nc_err;
		}
	}

	/* check if x_cell is >= x_dim */
	if ( x_cell >= dims_size[X_DIM] ) {
		logger(g_log, "x_cell >= x_dim: %d,%d\n", x_cell, dims_size[X_DIM]);
		goto quit_no_nc_err;
	}

	/* check if y_cell is >= y_dim */
	if ( y_cell >= dims_size[Y_DIM] ) {
		logger(g_log, "y_cell >= y_dim: %d,%d\n", y_cell, dims_size[Y_DIM]);
		goto quit_no_nc_err;
	}

	/* get vars */
	for ( i = 0; i < vars_count; ++i ) {
		ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
		if ( ret != NC_NOERR ) goto quit;
		/* who cames first ? x or y ? */
		if ( x_id == ids[0] ) {
			/* x first */
			start[0] = x_cell;
			start[1] = y_cell;			
		} else {
			/* y first */
			start[0] = y_cell;
			start[1] = x_cell;
		}
		for ( y = 0; y  < SOIL_VARS_COUNT; ++y ) {
			if ( ! string_compare_i(name, sz_vars[y]) ) {
				/* check if we already have imported that var */
				if ( vars[y] ) {
					logger(g_log, "var %s already imported\n", sz_vars[y]);
					goto quit_no_nc_err;
				}
				/* n_dims can be only 2 and ids only x and y */
				if ( 2 != n_dims ) {
					logger(g_log, "bad %s dimension size. It should be 2 not %d\n", sz_vars[y], n_dims);
					goto quit_no_nc_err;
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
					goto quit_no_nc_err;
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
quit_no_nc_err:
	nc_close(id_file);
	return 0;
}

soil_settings_t* soil_settings_new(void) {
	soil_settings_t* s = malloc(sizeof*s);
	if ( s ) reset(s);
	return s;
}

int soil_settings_import(soil_settings_t *const s, const char *const filename, const int x, const int y) {
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
