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

extern logger_t* g_debug_log;

static const char err_bad_landuse_length[] =" bad landuse length at row %d, landuse must be 1 character.\n";
static const char err_bad_landuse[] ="bad landuse %c at row %d\n";

static const char *sz_vars[SOIL_VARS_COUNT] =
{
	"X"
	, "Y"
	, "LAT"
	, "LON"
	, "CLAY_PERC"
	, "SILT_PERC"
	, "SAND_PERC"
	, "SOIL_DEPTH"
	, "FR"
	, "FN0"
	, "FNN"
	, "M0"
	, "LITTERC"
	, "LITTERN"
	, "SOILC"
	, "SOILN"
	, "LITTERCWDC"

	, "LANDUSE"
};

soil_settings_t* import_txt(const char *const filename, int* const p_settings_count) {
#define SOIL_BUFFER_SIZE 1024
	char* p;
	char* token;
	char buffer[SOIL_BUFFER_SIZE];
	int i;
	int columns[SOIL_VARS_COUNT];
	FILE *f;
	soil_settings_t *ps;
	soil_settings_t s;
	soil_settings_t *ps_no_leak;

	const char delimiters[] = ",\r\n";

	assert(filename && p_settings_count);

	/* reset stuff */
	ps = NULL; /* required! for realloc stuff */
	*p_settings_count = 0;

	/* open file */
	f = fopen(filename, "r");
	if ( ! f ) {
		logger_error(g_debug_log, "unable to open %s, file not found.\n", filename);
		return 0;
	}

	/* get header */
	if ( ! fgets(buffer, SOIL_BUFFER_SIZE, f) )
	{
		logger_error(g_debug_log, "header not found in %s\n", filename);
		fclose(f);
		return NULL;
	}

	/* reset columns */
	for ( i = 0; i < SOIL_VARS_COUNT; i++ )
	{
		columns[i] = -1;
	}

	/* parse header */
	for ( i = 0, token = string_tokenizer(buffer, delimiters, &p); token; token = string_tokenizer(NULL, delimiters, &p), ++i )
	{
		int y;
		for ( y = 0; y < SOIL_VARS_COUNT; ++y )
		{
			if ( ! string_compare_i(token, sz_vars[y]) )
			{
				/* check if column is not already assigned */
				if ( columns[i] != -1 )
				{
					logger_error(g_debug_log, "%s already assigned at column %d in %s\n", sz_vars[y], columns[y], filename);
					fclose(f);
					return NULL;
				}
				columns[i] = y;
			}
		}
	}

	/* check missing columns */
	for ( i = 0; i < SOIL_VARS_COUNT; ++i )
	{
		if ( -1 == columns[i] )
		{
			logger_error(g_debug_log, "columm %s not found in %s\n", sz_vars[i], filename);
			fclose(f);
			return NULL;
		}
	}

	/* import values */
	while ( fgets(buffer, SOIL_BUFFER_SIZE, f) ) {
		int y; /* keep track of imported values */

		/* skip empty rows */
		if ( ('\r' == buffer[0]) || ('\n' == buffer[0]) )
		{
			continue;
		}

		/* parse values */
		y = 0;
		for ( i = 0, token = string_tokenizer(buffer, delimiters, &p); token; token = string_tokenizer(NULL, delimiters, &p), ++i )
		{
			int err;
			double value;

			// check landuse
			if ( SOIL_LANDUSE == columns[i] ) {
				/* check landuse length */
				if ( 1 != strlen(token) ) {
					printf(err_bad_landuse_length, *p_settings_count + 1);
					if ( ps ) free(ps);
					fclose(f);
					return NULL;
				}
				/* check landuse */
				if ( ('F' == token[0]) || ('f' == token[0]) ) {
					s.landuse = LANDUSE_F;
					//dataset->rows[dataset->rows_count-1].landuse = F ;
				} else if ( ('Z' == token[0]) || ('z' == token[0]) ) {
					s.landuse = LANDUSE_Z;
					//dataset->rows[dataset->rows_count-1].landuse = Z;
				} else {
					printf(err_bad_landuse, token[0], *p_settings_count + 1);
					if ( ps ) free(ps);
					fclose(f);
					return NULL;
				}
			} else {
				value = convert_string_to_float(token, &err);
				if ( err )
				{
					//if ( columns[SOIL_LANDUSE] != i) {
					logger_error(g_debug_log, "unable to convert '%s' at columm %d in %s\n", token, y+1, filename);
					if ( ps ) free(ps);
					fclose(f);
					return NULL;
					//}
				}
				s.values[columns[i]] = value;
			}

			if ( ++y > SOIL_VARS_COUNT )
			{
				logger_error(g_debug_log, "too many columns in %s\n", filename);
				if ( ps ) free(ps);
				fclose(f);
				return 0;
			}
		}

		/* check imported stuff */
		if ( y != SOIL_VARS_COUNT )
		{
			logger_error(g_debug_log, "imported values for row %d in %s should be %d not %d\n"
									, *p_settings_count + 1
									, filename
									, SOIL_VARS_COUNT
									, y
			);
			if ( ps ) free(ps);
			fclose(f);
			return 0;
		}

		/* check for X and Y */
		for ( y = 0; y < *p_settings_count; ++y )
		{
			if ( ((int)ps[y].values[SOIL_X] == (int)s.values[SOIL_X])
					&& ((int)ps[y].values[SOIL_Y] == (int)s.values[SOIL_Y]) )
			{
				logger_error(g_debug_log, "duplicated settings for %d,%d cell in\n"
																		, (int)ps[y].values[SOIL_X]
																		, (int)ps[y].values[SOIL_Y]
																		, filename
				);
				if ( ps ) free(ps);
				fclose(f);
				return 0;
			}
		}

		/* alloc new row */
		ps_no_leak = realloc(ps, (*p_settings_count+1)*sizeof*ps_no_leak);
		if ( ! ps_no_leak )
		{
			logger_error(g_debug_log, "out of memory during import of %s\n", filename);
			if ( ps ) free(ps);
			fclose(f);
			return 0;
		}
		ps = ps_no_leak;
		ps[*p_settings_count].landuse = s.landuse;
		for ( y = 0; y < SOIL_VARS_COUNT-1; ++y ) // -1 remove landuse
		{
			ps[*p_settings_count].values[y] = s.values[y];
		}
		
		++*p_settings_count;
	}
	fclose(f);

	if (  ! *p_settings_count )
	{
		logger_error(g_debug_log, "no values found in %s\n", filename);
		return NULL;
	}

	return ps;
#undef SOIL_BUFFER_SIZE
}

static soil_settings_t* import_nc(const char *const sz_filename, int*const p_settings_count) {
	enum {
		X_DIM = 0
		, Y_DIM
		, DIMS_COUNT
	};

	char name[NC_MAX_NAME+1];
	int i;
	int x;
	int y;
	int z;
	int id_file;
	int dims_count;
	int vars_count;
	int dims_size[2];
	int x_id;
	int ret;
	int n_dims;
	int ids[NC_MAX_VAR_DIMS];
	size_t size;
		
	soil_settings_t *ps;
	soil_settings_t *ps_no_leak;

	nc_type type;

	size_t start[DIMS_COUNT] = { 0, 0 };
	size_t count[DIMS_COUNT] = { 1, 1 };

	const char *sz_dims[DIMS_COUNT] = { "x", "y" };

	assert(sz_filename && p_settings_count);

	/* reset stuff */
	ps = NULL; /* required! for realloc stuff */
	*p_settings_count = 0;
		
	ret = nc_open(sz_filename, NC_NOWRITE, &id_file);
	if ( ret != NC_NOERR ) goto quit;

	ret = nc_inq(id_file, &dims_count, &vars_count, NULL, NULL);
	if ( ret != NC_NOERR ) goto quit;

	if ( ! dims_count || ! vars_count ) {
		logger_error(g_debug_log, "bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		goto quit_no_nc_err;
	}

	if ( vars_count < SOIL_VARS_COUNT ) {
		logger_error(g_debug_log, "bad nc file! %d vars but expected %d\n\n", vars_count, SOIL_VARS_COUNT);
		goto quit_no_nc_err;
	}

	/* dims_count can be only 2 and ids only x and y */
	if ( 2 != dims_count ) {
		logger_error(g_debug_log, "bad dimension size. It should be 2 not %d\n", dims_count);
		goto quit_no_nc_err;
	}

	/* reset */
	dims_size[X_DIM] = -1;
	dims_size[Y_DIM] = -1;

	/* get dimensions */
	x_id = 0;
	for ( i = 0; i < dims_count; ++i ) {
		ret = nc_inq_dim(id_file, i, name, &size);
		if ( ret != NC_NOERR ) goto quit;
		for ( y = 0; y < DIMS_COUNT; ++y ) {
			if ( ! string_compare_i(sz_dims[y], name) ) {
				if ( dims_size[y] != -1 ) {
					logger_error(g_debug_log, "dimension %s already found!\n", sz_dims[y]);
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
			logger_error(g_debug_log, "dimension %s not found!\n", sz_dims[i]);
			goto quit_no_nc_err;
		}
	}

	/* check if we have all columns */
	{
		int vars[SOIL_VARS_COUNT] = { 0 };
		for ( i = 0; i < vars_count; ++i ) {
			ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
			for ( z = 0; z  < SOIL_VARS_COUNT; ++z ) {
				if ( ! string_compare_i(name, sz_vars[z]) ) {
					vars[z] = 1;
					break;
				}
			}
		}
		for ( i = 0; i < SOIL_VARS_COUNT; ++i ) {
			if ( ! vars[i] ) {
				logger_error(g_debug_log, "%s column not found!\n", sz_vars[i]);
				goto quit_no_nc_err;
			}
		}
	}
	

	/* get vars */
	for ( x = 0; x < dims_size[X_DIM]; x++ ) {
		for ( y = 0; y < dims_size[Y_DIM]; y++ ) {
			int assigned;

			/* alloc new row */
			ps_no_leak = realloc(ps, (*p_settings_count+1)*sizeof*ps_no_leak);
			if ( ! ps_no_leak )
			{
				logger_error(g_debug_log, "out of memory during import of %s\n", sz_filename);
				if ( ps ) free(ps);
				return 0;
			}
			ps = ps_no_leak;

			assigned = 0;
			for ( i = 0; i < vars_count; ++i ) {
				ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
				if ( ret != NC_NOERR ) goto quit;
				/* who cames first ? x or y ? */
				if ( x_id == ids[0] ) {
					/* x first */
					start[0] = x;
					start[1] = y;			
				} else {
					/* y first */
					start[0] = y;
					start[1] = x;
				}

				for ( z = 0; z  < SOIL_VARS_COUNT; ++z ) {
					float f;
					double d;

					if ( ! string_compare_i(name, sz_vars[z]) ) {
						/* n_dims can be only 2 and ids only x and y */
						if ( 2 != n_dims ) {
							logger_error(g_debug_log, "bad %s dimension size. It should be 2 not %d\n", sz_vars[z], n_dims);
							goto quit_no_nc_err;
						}
						/* get values */
						if ( NC_FLOAT == type ) {
							ret = nc_get_vara_float(id_file, i, start, count, &f);
							if ( ret != NC_NOERR ) goto quit;
							if ( z == SOIL_LANDUSE ) {
								ps[*p_settings_count].landuse = f;
							} else {
								ps[*p_settings_count].values[z] = f;
							}
							++assigned;
						} else if ( NC_DOUBLE == type ) {
							ret = nc_get_vara_double(id_file, i, start, count, &d);
							if ( ret != NC_NOERR ) goto quit;
							if ( z == SOIL_LANDUSE ) {
								ps[*p_settings_count].landuse = d;
							} else {
								ps[*p_settings_count].values[z] = d;
							}
							++assigned;
						} else {
							/* type format not supported! */
							logger_error(g_debug_log, "type format in %s for %s column not supported\n\n", sz_filename, sz_vars[z]);
							goto quit_no_nc_err;
						}
						break;
					}
				}
			}

			if ( assigned != SOIL_VARS_COUNT ) {
				logger_error(g_debug_log, "imported %s columns instead of %d\n\n", assigned, SOIL_VARS_COUNT);
				goto quit_no_nc_err;
			}

			++*p_settings_count;
		}
	}
	
	nc_close(id_file);
	return ps;

quit:
	logger(g_debug_log, nc_strerror(ret));
quit_no_nc_err:
	if ( ps ) free(ps);
	nc_close(id_file);
	return NULL;
}

soil_settings_t* soil_settings_import(const char *const filename, int*const soil_settings_count) {
	char *p;

	assert(filename && soil_settings_count);

	p = strrchr(filename, '.');
	if ( p ) {
		++p;
		if ( ! string_compare_i(p, "nc") || ! string_compare_i(p, "nc4") ) {
			return import_nc(filename, soil_settings_count);
		}
	}
	return import_txt(filename, soil_settings_count);
}
