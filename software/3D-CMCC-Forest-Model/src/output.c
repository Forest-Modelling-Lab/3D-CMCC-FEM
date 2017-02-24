/* output.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "logger.h"
#include "output.h"
#include "netcdf.h"

extern logger_t* g_debug_log;
extern const char sz_err_out_of_memory[];
extern const char* log_types[];

/*
	DO NOT CHANGE THIS ORDER
	please see output.h
*/
static const char *sz_output_vars[OUTPUT_VARS_COUNT] = {
		"ar"
		, "gpp"
		, "npp"
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

	enum { DAILY_FREQ, MONTHLY_FREQ, ANNUAL_FREQ, FREQ_COUNT };
	const char *sz_freq[FREQ_COUNT] = { "daily_", "monthly_", "annual_" };

	assert(filename);

	i = file_load_in_memory(filename, &buffer);
	if ( i <= 0 ) {
		char *err;
		printf("unable to import output filename '%s': ", filename);
		switch ( i )
		{
			case 0:
				err = "file is empty!";
			break;

			case -1:
				err = "file not found!";
			break;

			case -2:
				err = "out of memory!";
			break;

			case -3:
				err = "read error!";
			break;
		}
		puts(err);
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
			char* var;
			int freq;

			// get frequency
			var = NULL;
			for ( i = 0; i < FREQ_COUNT; ++i ) {
				var = strstr_i(token, sz_freq[i]);
				if ( var ) {
					freq = i;
					break;
				}
			}
			if ( ! var ) {
				printf("%s is an unknown output var. skipped\n", token);
				continue;
			}
			
			// get var name
			if ( var - token ) {
				printf("bad output var specified: %s. skipped\n", token);
				continue;
			}
			var += strlen(sz_freq[freq]);

			flag = 0;
			for ( i = 0; i < OUTPUT_VARS_COUNT; ++i ) {
				if ( ! string_compare_i(var, sz_output_vars[i]) ) {
					switch ( freq ) {
						case DAILY_FREQ:
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
						break;

						case MONTHLY_FREQ:
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
						break;

						case ANNUAL_FREQ:
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
						break;

						default:
							assert(0);
					}
				}
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

/* please note: mm and dd must starts from 0 not 1 */
static int get_daily_row_from_date(const int yyyy, const int mm, const int dd) {
	int i;
	int days;
	int days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if ( IS_LEAP_YEAR(yyyy) ) {
		++days_per_month[1];
	}

	/* check args */
	assert(((mm >= 0) && (mm < 12)) && ((dd >= 0) && (dd < days_per_month[mm])));

	days = 0;
	for ( i = 0; i < mm; ++i ) {
		days += days_per_month[i];
	}
	return (days+dd);
}

static void daily_push_values(const output_t* const o, const cell_t* const c, const int year, const int month, const int day, const int year_index, const int years_count, const int x_cells_count, const int y_cells_count) {
/*
	la memoria e' stata allocata come C*R*Y*X

	C = colonne ( variabili )
	R = righe ( anni di elaborazione * 366 )
	Y = numero y celle
	X = numero x celle

	quindi il valore [v1][v2][v3][v4] e' indicizzato a

	[v1 * n1 * n2 *n3 + v2 * n2 * n3 + v3 * n3 + v4]

	ossia

	[v4 + n3 * (v3 + n2 * (v2 + n1 * v1))]
*/
#define YS					(y_cells_count)
#define XS					(x_cells_count)
#define ROWS				(366*years_count)
#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
	int i;
	for ( i = 0; i < o->daily_vars_count; ++i ) {
		int row = get_daily_row_from_date(year, month, day) + (year_index*366);
		int index = VALUE_AT(c->x, c->y, row, i);
		if ( AR_OUT == o->daily_vars[i] )		o->daily_vars_value[index] = c->daily_aut_resp;
		if ( GPP_OUT == o->daily_vars[i] )	o->daily_vars_value[index] = c->daily_gpp;
		if ( NPP_OUT == o->daily_vars[i] )	o->daily_vars_value[index] = c->daily_npp_gC;
	}
#undef VALUE_AT
#undef ROWS
#undef XS
#undef YS
}

static void monthly_push_values(const output_t* const o, const cell_t* const c, const int year, const int month, const int day, const int year_index, const int years_count, const int x_cells_count, const int y_cells_count) {
#define YS					(y_cells_count)
#define XS					(x_cells_count)
#define ROWS				(12*years_count)
#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
	int i;
	for ( i = 0; i < o->monthly_vars_count; ++i ) {
		int row = month + (year_index*12);
		int index = VALUE_AT(c->x, c->y, row, i);
		if ( AR_OUT == o->monthly_vars[i] ) o->monthly_vars_value[index] = c->monthly_aut_resp;
		if ( GPP_OUT == o->monthly_vars[i] ) o->monthly_vars_value[index] = c->monthly_gpp;
		if ( NPP_OUT == o->monthly_vars[i] ) o->monthly_vars_value[index] = c->monthly_npp_gC;
	}
#undef VALUE_AT
#undef ROWS
#undef XS
#undef YS
}

static void yearly_push_values(const output_t* const o, const cell_t* const c, const int year, const int month, const int day, const int year_index, const int years_count, const int x_cells_count, const int y_cells_count) {
#define YS					(y_cells_count)
#define XS					(x_cells_count)
#define ROWS				(years_count)
#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
	int i;
	for ( i = 0; i < o->yearly_vars_count; ++i )
	{
		int index = VALUE_AT(c->x, c->y, year_index, i);
		if ( AR_OUT == o->yearly_vars[i] ) o->yearly_vars_value[index] = c->annual_aut_resp;
		if ( GPP_OUT == o->yearly_vars[i] ) o->yearly_vars_value[index] = c->annual_gpp;
		if ( NPP_OUT == o->yearly_vars[i] ) o->yearly_vars_value[index] = c->annual_npp_gC;
	}
#undef VALUE_AT
#undef ROWS
#undef XS
#undef YS
}

void output_push_values(const output_t* const o, const cell_t* const c, const int month, const int day, const int year_index, const int years_count, const int x_cells_count, const int y_cells_count, const e_output_types type) {
	int year;
	assert(o && c);
	assert((type >= OUTPUT_TYPE_DAILY) && (type < OUTPUT_TYPES_COUNT));
	year = c->years[year_index].year;
	if ( OUTPUT_TYPE_DAILY == type ) {
		daily_push_values(o, c, year, month, day, year_index, years_count, x_cells_count, y_cells_count);
	} else if ( OUTPUT_TYPE_MONTHLY == type ) {
		monthly_push_values(o, c, year, month, day, year_index, years_count, x_cells_count, y_cells_count);
	} else {
		yearly_push_values(o, c, year, month, day, year_index, years_count, x_cells_count, y_cells_count);
	}
}

static int output_write_nc(const output_t* const vars, const char *const path, const int year_start, const int years_count, const int x_cells_count, const int y_cells_count, const e_output_types type) {
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
	const char *var;
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

	/* init */
	time_rows = NULL;
	values = NULL;
	var = NULL;
	n = 0;
	index = 0;

	/* create time rows */
	if ( 0 == type ) time_rows = malloc(years_count*366*sizeof*time_rows);
	else if ( 1 == type ) time_rows = malloc(years_count*12*sizeof*time_rows);
	else if ( 2 == type ) time_rows = malloc(years_count*sizeof*time_rows);
	if ( ! time_rows ) {
		logger(g_debug_log, sz_err_out_of_memory);
		return 0;
	}

	if ( OUTPUT_TYPE_DAILY == type ) {
		rows_count = 0;
		for ( n = 0; n < years_count; ++n ) {
			ret = 365 + IS_LEAP_YEAR(year_start+n);
			for ( i = 0; i < ret; i++ ) {
				time_rows[i+n*366] = get_daily_date_from_row(i, year_start+n);
			}
			rows_count += ret;
		}
	} else if ( OUTPUT_TYPE_MONTHLY == type ) {
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

	if ( OUTPUT_TYPE_DAILY == type ) n = vars->daily_vars_count;
	else if ( OUTPUT_TYPE_MONTHLY == type ) n = vars->monthly_vars_count;
	else n = vars->yearly_vars_count;

	for ( i = 0; i < n; ++i ) {
		/* create output filename */
		if ( OUTPUT_TYPE_DAILY == type )
			sprintf(sz_buffer, "%s%s%s%s.nc" , path, log_types[DAILY_LOG], FOLDER_DELIMITER, sz_output_vars[vars->daily_vars[i]]);
		else if ( OUTPUT_TYPE_MONTHLY == type )
			sprintf(sz_buffer, "%s%s%s%s.nc", path, log_types[MONTHLY_LOG], FOLDER_DELIMITER, sz_output_vars[vars->monthly_vars[i]]);
		else
			sprintf(sz_buffer, "%s%s%s%s.nc", path, log_types[YEARLY_LOG], FOLDER_DELIMITER, sz_output_vars[vars->yearly_vars[i]]);

		/* get var name */
		if ( OUTPUT_TYPE_DAILY == type)
			var = sz_output_vars[vars->daily_vars[i]];
		else if ( OUTPUT_TYPE_MONTHLY == type)
			var = sz_output_vars[vars->monthly_vars[i]];
		else if ( OUTPUT_TYPE_YEARLY == type)
			var = sz_output_vars[vars->yearly_vars[i]];
		else
			//logger(g_debug_log, "unable to create output netcdf file %s: internal error (bad var name)", sz_buffer);
			//free(time_rows);
			//return 0;
			assert(0);
		
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

		ret = nc_def_var(id_file, var, NC_DOUBLE, 3, id_dims, &id_var);
		if ( ret != NC_NOERR ) goto quit;

		/* close definition */
		ret = nc_enddef(id_file);
		if ( ret != NC_NOERR ) goto quit;

		/* put time row */
		ret = nc_put_var_double(id_file, id_time, time_rows);
		if ( ret != NC_NOERR ) goto quit;

		/* puts values */
		if ( OUTPUT_TYPE_DAILY == type ) {
			values = vars->daily_vars_value;
			index = x_cells_count*y_cells_count*366*years_count*i;
		}
		else if ( OUTPUT_TYPE_MONTHLY == type ) {
			values = vars->monthly_vars_value;
			index = x_cells_count*y_cells_count*12*years_count*i;
		}
		else  {
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
	logger(g_debug_log, "unable to create output netcdf file %s: %s", sz_buffer, nc_strerror(ret));
	free(time_rows);
	nc_close(id_file);

	return 0;
}

#if 0
static int output_write_txt(const output_t* const vars, const char *const path, const int year_start, const int years_count, const int x_cells_count, const int y_cells_count, const e_output_types type) {
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
	int var;
	int vars_count;
	int row;
	int rows_count;

	if ( OUTPUT_TYPE_DAILY == type ) vars_count = vars->daily_vars_count;
	else if ( OUTPUT_TYPE_MONTHLY == type ) vars_count = vars->monthly_vars_count;
	else vars_count = vars->yearly_vars_count;
	
	/* loop each vars */
	for ( var = 0; var < vars_count; ++var )
	{
		char sz_buffer[256];
		int x_cell;
		int y_cell;
		int year;
		FILE *f;

		/* create output filename */
		if ( OUTPUT_TYPE_DAILY == type )
		{
			sprintf(sz_buffer, "%s%s.txt", path, sz_output_vars[vars->daily_vars[var]]);			
		}
		else if ( OUTPUT_TYPE_MONTHLY == type )
		{
			sprintf(sz_buffer, "%s%s.txt", path, sz_output_vars[vars->monthly_vars[var]]);
		}
		else
		{
			sprintf(sz_buffer, "%s%s.txt", path, sz_output_vars[vars->yearly_vars[var]]);
		}

		/* create file */
		f = fopen(sz_buffer, "w");
		if ( ! f )
		{
			logger(g_debug_log, "unable to create output file: %s", sz_buffer);
			return 0;
		}

		/* loop each x */
		for ( x_cell = 0; x_cell < x_cells_count; ++x_cell )
		{
			/* loop each y */
			for ( y_cell = 0; y_cell < y_cells_count; ++y_cell )
			{
				fprintf(f, "cell: %d, %d\n", x_cell, y_cell);
				fputs("time,", f);
				if ( OUTPUT_TYPE_DAILY == type )
				{
					fprintf(f, "%s\n", sz_output_vars[vars->daily_vars[var]]);
					fprintf(f, "%d,", get_daily_date_from_row(var, year));
				}
				else if ( OUTPUT_TYPE_MONTHLY == type )
				{
					fprintf(f, "%s\n", sz_output_vars[vars->monthly_vars[var]]);
					fprintf(f, "%d,", get_monthly_date_from_row(var, year));
				}
				else
				{
					fprintf(f, "%s\n", sz_output_vars[vars->yearly_vars[var]]);
					fprintf(f, "%d,", year);
				}

				/* loop each year */
				for ( year = year_start; year < year_start+years_count; ++year )
				{		
					if ( OUTPUT_TYPE_DAILY == type ) rows_count = 365 + IS_LEAP_YEAR(year);
					else if ( OUTPUT_TYPE_MONTHLY == type ) rows_count = 12;
					else rows_count = years_count;

					for ( row = 0; row < rows_count; ++row )
					{
						int index;

						if ( OUTPUT_TYPE_DAILY == type )
						{
							index = (x_cell+x_cells_count*(y_cell+y_cells_count*((row)+(366*(year-year_start))*(var))));

							fprintf(f, "%d,%g\n", get_daily_date_from_row(row, year), vars->daily_vars_value[index]);
						}
						else if ( OUTPUT_TYPE_MONTHLY == type )
						{
							index = (x_cell+x_cells_count*(y_cell+y_cells_count*((row)+(12*(year-year_start))*(var))));

							fprintf(f, "%d,%g\n", get_monthly_date_from_row(row, year), vars->monthly_vars_value[index]);
						}
						else
						{
							index = (x_cell+x_cells_count*(y_cell+y_cells_count*(row+year-year_start*(var))));

							fprintf(f, "%d,%g\n", year, vars->yearly_vars_value[index]);
						}
					}
				}
			}
		}

		fclose(f);
	}

	return 1;
}
#endif

/* path must terminate with a backslash! */
int output_write(const output_t* const vars, const char *const path, const int year_start, const int years_count, const int x_cells_count, const int y_cells_count, const e_output_types type) {

	assert(vars && years_count && x_cells_count && y_cells_count && ((type >= OUTPUT_TYPE_DAILY) && (type < OUTPUT_TYPES_COUNT)));

	return output_write_nc(vars, path, year_start, years_count, x_cells_count, y_cells_count, type);
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
