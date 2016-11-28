/* topo.c */
#include "topo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "matrix.h"
#include "common.h"
#include "netcdf.h"
#include "logger.h"

extern logger_t* g_debug_log;

extern char *g_sz_input_path;

static const char *sz_vars[TOPO_VARS_COUNT] =
{
	"X"
	, "Y"
	, "ELEV"
};

topo_t* import_topo_txt(const char *const filename, int* const p_topos_count) {
#define TOPO_BUFFER_SIZE 1024
	char* p;
	char* token;
	char buffer[TOPO_BUFFER_SIZE];
	int i;
	int columns[TOPO_VARS_COUNT];
	FILE *f;
	topo_t *pt;
	topo_t t;
	topo_t *pt_no_leak;

	const char delimiters[] = ",\r\n";

	assert(filename && p_topos_count);

	/* reset stuff */
	pt = NULL; /* required! for realloc stuff */
	*p_topos_count = 0;

	/* open file */
	f = fopen(filename, "r");
	if ( ! f ) {
		logger_error(g_debug_log, "unable to open, file doesn't exist? %s\n", filename);
		return 0;
	}

	/* get header */
	if ( ! fgets(buffer, TOPO_BUFFER_SIZE, f) )
	{
		logger_error(g_debug_log, "header not found in %s\n", filename);
		return 0;
	}

	/* reset columns */
	for ( i = 0; i < TOPO_VARS_COUNT; i++ )
	{
		columns[i] = -1;
	}

	/* parse header */
	for ( i = 0, token = string_tokenizer(buffer, delimiters, &p); token; token = string_tokenizer(NULL, delimiters, &p), ++i )
	{
		int y;
		for ( y = 0; y < TOPO_VARS_COUNT; ++y )
		{
			if ( ! string_compare_i(token, sz_vars[y]) )
			{
				/* check if column is not already assigned */
				if ( columns[y] != -1 )
				{
					logger_error(g_debug_log, "%s already assigned at column %d in %s\n", sz_vars[y], columns[y], filename);
					fclose(f);
					return 0;
				}
				columns[y] = i;
			}
		}
	}

	/* check missing columns */
	for ( i = 0; i < TOPO_VARS_COUNT; i++ )
	{
		if ( -1 == columns[i] )
		{
			logger_error(g_debug_log, "columm %s not found in %s\n", sz_vars[i], filename);
			fclose(f);
			return 0;
		}
	}

	/* import values */
	while ( fgets(buffer, TOPO_BUFFER_SIZE, f) ) {
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
			float value;

			value = (float)convert_string_to_float(token, &err);
			if ( err )
			{
				logger_error(g_debug_log, "unable to convert '%s' at columm %d in %s\n", token, y+1, filename);
				if ( pt ) free(pt);
				fclose(f);
				return 0;
			}

			if ( ++y > TOPO_VARS_COUNT )
			{
				logger_error(g_debug_log, "too many columns in %s\n", filename);
				if ( pt ) free(pt);
				fclose(f);
				return 0;
			}
			
			t.values[columns[i]] = value;
		}

		/* check imported stuff */
		if ( y != TOPO_VARS_COUNT )
		{
			logger_error(g_debug_log, "imported values for row %d in %s should be %d not %d\n"
																	, *p_topos_count + 1
																	, filename
																	, TOPO_VARS_COUNT
																	, y
			);
			if ( pt ) free(pt);
			fclose(f);
			return 0;
		}

		/* check for X and Y */
		for ( y = 0; y < *p_topos_count; ++y )
		{
			if ( ((int)pt[y].values[TOPO_X] == (int)t.values[TOPO_X])
					&& ((int)pt[y].values[TOPO_Y] == (int)t.values[TOPO_Y]) )
			{
				logger_error(g_debug_log, "duplicated settings for %d,%d cell in\n"
																		, (int)pt[y].values[TOPO_X]
																		, (int)pt[y].values[TOPO_Y]
																		, filename
				);
				if ( pt ) free(pt);
				fclose(f);
				return 0;
			}
		}

		/* alloc new row */
		pt_no_leak = realloc(pt, (*p_topos_count+1)*sizeof*pt_no_leak);
		if ( ! pt_no_leak )
		{
			logger_error(g_debug_log, "out of memory during import of %s\n", filename);
			if ( pt ) free(pt);
			fclose(f);
			return 0;
		}
		pt = pt_no_leak;
		for ( y = 0; y < TOPO_VARS_COUNT; ++y )
		{
			pt[*p_topos_count].values[y] = t.values[y];
		}
		
		++*p_topos_count;
	}
	fclose(f);

	if (  ! *p_topos_count )
	{
		logger_error(g_debug_log, "no values found in %s\n", filename);
		return NULL;
	}

	return pt;
#undef TOPO_BUFFER_SIZE
}

static topo_t* import_nc(const char *const sz_filename, int*const p_topo_count) {
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
	//int y_id;
	int ret;
	int n_dims;
	int ids[NC_MAX_VAR_DIMS];
	size_t size;
		
	topo_t *ps;
	topo_t *ps_no_leak;

	nc_type type;

	size_t start[DIMS_COUNT] = { 0, 0 };
	size_t count[DIMS_COUNT] = { 1, 1 };

	const char *sz_dims[DIMS_COUNT] = { "x", "y" };

	assert(sz_filename && p_topo_count);

	/* reset stuff */
	ps = NULL; /* required! for realloc stuff */
	*p_topo_count = 0;

	ret = nc_open(sz_filename, NC_NOWRITE, &id_file);
	if ( ret != NC_NOERR ) goto quit;

	ret = nc_inq(id_file, &dims_count, &vars_count, NULL, NULL);
	if ( ret != NC_NOERR ) goto quit;

	if ( ! dims_count || ! vars_count ) {
		logger_error(g_debug_log, "bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		goto quit_no_nc_err;
	}

	if ( vars_count < TOPO_VARS_COUNT ) {
		logger_error(g_debug_log, "bad nc file! %d vars but expected %d\n\n", vars_count, TOPO_VARS_COUNT);
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
		int vars[TOPO_VARS_COUNT] = { 0 };
		for ( i = 0; i < vars_count; ++i ) {
			ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
			for ( z = 0; z  < TOPO_VARS_COUNT; ++z ) {
				if ( ! string_compare_i(name, sz_vars[z]) ) {
					vars[z] = 1;
					break;
				}
			}
		}
		for ( i = 0; i < TOPO_VARS_COUNT; ++i ) {
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
			ps_no_leak = realloc(ps, (*p_topo_count+1)*sizeof*ps_no_leak);
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

				for ( z = 0; z  < TOPO_VARS_COUNT; ++z ) {
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
							ps[*p_topo_count].values[z] = f;
							++assigned;
						} else if ( NC_DOUBLE == type ) {
							ret = nc_get_vara_double(id_file, i, start, count, &d);
							if ( ret != NC_NOERR ) goto quit;
							ps[*p_topo_count].values[z] = (float)d;
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

			if ( assigned != TOPO_VARS_COUNT ) {
				logger_error(g_debug_log, "imported %s columns instead of %d\n\n", assigned, TOPO_VARS_COUNT);
				goto quit_no_nc_err;
			}

			++*p_topo_count;
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

topo_t* topo_import(const char *const filename, int*const topos_count)
{
	char *p;

	assert(filename && topos_count);

	p = strrchr(filename, '.');
	if ( p ) {
		++p;
		if ( ! string_compare_i(p, "nc") || ! string_compare_i(p, "nc4") ) {
			return import_nc(filename, topos_count);
		}
	}
	return import_topo_txt(filename, topos_count);
}
