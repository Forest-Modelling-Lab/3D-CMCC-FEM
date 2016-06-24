/* output.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "logger.h"
#include "output.h"
#include "netcdf.h"

extern logger_t* g_log;
extern const char sz_err_out_of_memory[];

/* DO NOT CHANGE THIS ORDER
	please see output.h
*/
static const char *sz_output_vars[OUTPUT_VARS_COUNT] = {
		"daily_ar"
		, "monthly_ar"
		, "annual_ar"

		, "daily_gpp"
		, "monthly_gpp"
		, "annual_gpp"

		, "daily_npp"
		, "monthly_npp"
		, "annual_npp"
};

output_t* output_import(const char *const filename) {
	char *token;
	char *p;
	char *buffer;
	int i;
	output_t *ov;
	int *int_no_leak;
	int flag;

	const char delimiter[] = " ,\r\n";

	assert(filename);

	if ( ! file_load_in_memory(filename, &buffer) ) {
		printf("unable to import output filename: %s\n", filename);
		return NULL;
	}

	ov = malloc(sizeof*ov);
	if ( ! ov ) {
		puts(sz_err_out_of_memory);
		free(buffer);
		return NULL;
	}
	ov->daily_vars = NULL;
	ov->daily_vars_value = NULL;
	ov->daily_vars_count = 0;
	ov->monthly_vars = NULL;
	ov->monthly_vars_value = NULL;
	ov->monthly_vars_count = 0;
	ov->yearly_vars = NULL;
	ov->yearly_vars_value = NULL;
	ov->yearly_vars_count = 0;

	for ( token = string_tokenizer(buffer, delimiter, &p); token; token = string_tokenizer(NULL, delimiter, &p) ) {
		if ( token[0] ) {
			flag = 0;
			for ( i = 0; i < OUTPUT_VARS_COUNT; ++i ) {
				if ( ! string_compare_i(token, sz_output_vars[i]) ) {
					/* daily */
					if ( ('d' == token[0]) || ('D' == token[0]) ) {
						int_no_leak = realloc(ov->daily_vars, (ov->daily_vars_count+1)*sizeof*int_no_leak);
						if ( ! int_no_leak )
						{
							puts(sz_err_out_of_memory);
							output_free(ov);
							free(buffer);
							return NULL;
						}
						ov->daily_vars = int_no_leak;
						ov->daily_vars[ov->daily_vars_count++] = i;
						flag = 1;
						break;
					}
					else /* monthly */
						if ( ('m' == token[0]) || ('M' == token[0]) ) {
							int_no_leak = realloc(ov->monthly_vars, (ov->monthly_vars_count+1)*sizeof*int_no_leak);
							if ( ! int_no_leak )
							{
								puts(sz_err_out_of_memory);
								output_free(ov);
								free(buffer);
								return NULL;
							}
							ov->monthly_vars = int_no_leak;
							ov->monthly_vars[ov->monthly_vars_count++] = i;
							flag = 1;
							break;
						}
						else /* yearly */
							if ( ('a' == token[0]) || ('A' == token[0]) ) { /* a/A means annual */
								int_no_leak = realloc(ov->yearly_vars, (ov->yearly_vars_count+1)*sizeof*int_no_leak);
								if ( ! int_no_leak )
								{
									puts(sz_err_out_of_memory);
									output_free(ov);
									free(buffer);
									return NULL;
								}
								ov->yearly_vars = int_no_leak;
								ov->yearly_vars[ov->yearly_vars_count++] = i;
								flag = 1;
								break;
							}
				}
			}
			if ( ! flag ) {
				printf("%s is an unknown output var. skipped\n", token);
			}
		}
	}

	free(buffer);

	return ov;
}

static int get_monthly_date_from_row(const int row, const int yyyy) {
	assert(row>=0 && row<12);
	return yyyy*100+(row+1);
}
static int get_daily_date_from_row(const int doy, int yyyy) {
	int i;
	int dd;
	int mm;
	int days_per_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	assert((doy >=0 ) && (doy < 365+IS_LEAP_YEAR(yyyy)));

	if ( IS_LEAP_YEAR(yyyy) ) {
		++days_per_month[1];
	}
	dd = doy;
	for ( i = 0, mm = 0; mm < 12; ++mm ) {
		i += days_per_month[mm];
		if ( dd <= i ) {
			dd -= i - days_per_month[mm];
			break;
		}
	}
	++dd;
	++mm;

	return (yyyy*10000)+(mm*100)+dd;
}


/*
	if type is 0, write daily
	if type is 1, write monthly
	if type is 2, write yearly

	path must terminate with a backslash!
*/
int output_write(const output_t* const vars, const char *const path, const int year_start, const int years_count, const int x_cells_count, const int y_cells_count, const int type) {
/*
	la memoria e' stata allocata come C*R*Y*X

	C = colonne ( variabili )
	R = righe ( anni di elaborazione * 366 )
	X = numero x celle
	Y = numero y celle

	quindi il valore a [v1][v2][v3][v4] va indicizzato a 

	[v1 * n1 * n2 *n3 + v2 * n2 * n3 + v3 * n3 + v4]

	ossia

	[v4 + n3 * (v3 + n2 * (v2 + n1 * v1))]
	 */

	int i;
	int ret;
	char *p;
	char sz_buffer[256];
	int n;
	int index;

	int id_file;
	int id_x;
	int id_y;
	int id_lat;
	int id_lon;
	int id_time;
	int id_var;
	int id_dims[3];
	double *time_rows;
	int rows_count;

	double *values;

	const char sz_x[] = "x";
	const char sz_y[] = "y";
	const char sz_lat[] = "lat";
	const char sz_lon[] = "lon";
	const char sz_time[] = "time";

	assert(vars && years_count && x_cells_count && y_cells_count && ((type >=0) && (type<=3)));

	/* create time rows */
	if ( 0 == type ) time_rows = malloc(years_count*366*sizeof*time_rows);
	else if ( 1 == type ) time_rows = malloc(years_count*12*sizeof*time_rows);
	else if ( 2 == type ) time_rows = malloc(years_count*sizeof*time_rows);
	if ( ! time_rows ) {
		logger(g_log, sz_err_out_of_memory);
		return 0;
	}

	if ( 0 == type ) {
		rows_count = 0;
		for ( n = 0; n < years_count; ++n ) {
			ret = 365 + IS_LEAP_YEAR(year_start+n);
			for ( i = 0; i < ret; i++ ) {
				time_rows[i+n*366] = get_daily_date_from_row(i, year_start+n);
			}
			rows_count += ret;
		}
	} else if ( 1 == type ) {
		for ( n = 0; n < years_count; ++n ) {
			for ( i = 0; i < 12; i++ ) {
				time_rows[i+n*12] = get_monthly_date_from_row(i, year_start+n);
			}
		}
		rows_count = 12*years_count;
	} else {
		for ( i = 0; i < years_count; ++i ) {
			time_rows[i] = year_start+i;
		}
		rows_count = years_count;
	}

	if ( 0 == type ) n = vars->daily_vars_count;
	if ( 1 == type ) n = vars->monthly_vars_count;
	if ( 2 == type ) n = vars->yearly_vars_count;

	for ( i = 0; i < n; ++i ) {
		/* create output filename */
		if ( 0 == type )
			sprintf(sz_buffer, "%s%s.nc", path, sz_output_vars[vars->daily_vars[i]]);

		if ( 1 == type )
			sprintf(sz_buffer, "%s%s.nc", path, sz_output_vars[vars->monthly_vars[i]]);

		if ( 2 == type )
			sprintf(sz_buffer, "%s%s.nc", path, sz_output_vars[vars->yearly_vars[i]]);

		/* create file */
		ret = nc_create(sz_buffer, NC_CLOBBER, &id_file);
		if ( ret != NC_NOERR ) goto quit;

		/* define dimensions */
		ret = nc_def_dim(id_file, sz_x, x_cells_count, &id_x);
		if ( ret != NC_NOERR ) goto quit;

		ret = nc_def_dim(id_file, sz_y, y_cells_count, &id_y);
		if ( ret != NC_NOERR ) goto quit;

		ret = nc_def_dim(id_file, sz_time, rows_count /*NC_UNLIMITED*/, &id_time);
		if ( ret != NC_NOERR ) goto quit;

		/* NC_UNLIMITED must be first dimension (or the left-most dimension) */
		id_dims[0] = id_time;
		id_dims[1] = id_y;
		id_dims[2] = id_x;

		/* define variables */
		ret = nc_def_var(id_file, sz_lat, NC_FLOAT, 2, id_dims+1, &id_lat);
		if ( ret != NC_NOERR ) goto quit;

		ret = nc_def_var(id_file, sz_lon, NC_FLOAT, 2, id_dims+1, &id_lon);
		if ( ret != NC_NOERR ) goto quit;

		ret = nc_def_var(id_file, sz_time, NC_DOUBLE, 1, id_dims, &id_time);
		if ( ret != NC_NOERR ) goto quit;

		/* remove suffix from var name(daily, month or annual) */
		if ( 0 == type)
			p = strchr(sz_output_vars[vars->daily_vars[i]], '_');
		if ( 1 == type)
			p = strchr(sz_output_vars[vars->monthly_vars[i]], '_');
		if ( 2 == type)
			p = strchr(sz_output_vars[vars->yearly_vars[i]], '_');
		assert(p);
		++p;

		ret = nc_def_var(id_file, p, NC_DOUBLE, 3, id_dims, &id_var);
		if ( ret != NC_NOERR ) goto quit;

		/* close definition */
		ret = nc_enddef(id_file);
		if ( ret != NC_NOERR ) goto quit;

		/* put time row */
		ret = nc_put_var_double(id_file, id_time, time_rows);
		if ( ret != NC_NOERR ) goto quit;

		/* puts values */
		if ( 0 == type )
		{
			values = vars->daily_vars_value;
			index = x_cells_count*y_cells_count*366*years_count*i;
		}
		if ( 1 == type )
		{
			values = vars->monthly_vars_value;
			index = x_cells_count*y_cells_count*12*years_count*i;
		}
		if ( 2 == type )
		{
			values = vars->yearly_vars_value;
			index = x_cells_count*y_cells_count*years_count*i;
		}

		ret = nc_put_var_double(id_file, id_var, values+index);
		if ( ret != NC_NOERR ) goto quit;

		nc_close(id_file);
	}
	free(time_rows);
	return 1;

	quit:
	logger(g_log, "unable to create output netcdf file %s: %s", sz_buffer, nc_strerror(ret));
	free(time_rows);
	nc_close(id_file);

	return 0;
}

void output_free(output_t *ov) {
	assert(ov);
	if ( ov->yearly_vars_count ) {
		free(ov->yearly_vars_value);
		free(ov->yearly_vars);
	}
	if ( ov->monthly_vars_count ) {
		free(ov->monthly_vars_value);
		free(ov->monthly_vars);
	}
	if ( ov->daily_vars_count ) {
		free(ov->daily_vars_value);
		free(ov->daily_vars);
	}
	free(ov);
}
