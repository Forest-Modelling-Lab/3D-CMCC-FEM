/* netcdf_stuff.c */
#include "netcdf_stuff.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "netcdf.h"
#include "common.h"

///* */
//#define INVALID_VALUE		-9999
//
///* */
//#define IS_LEAP_YEAR(x)		(((x) % 4 == 0 && (x) % 100 != 0) || (x) % 400 == 0)
//
///* */
//#ifdef _WIN32
//#pragma comment(lib, "lib/netcdf")
//#endif /* _WIN32 */
//
//enum {
//	YEAR,
//	MONTH,
//	DAY,
//
//	DATE_SIZE
//};
//
///* */
//static const char *sz_date[DATE_SIZE] = { "year", "month", "day" };
//
///* */
//static char g_netcdf_err[256];
//
///* */
//static char netcdf_err_no_error[] = "no error";
//static char netcdf_err_out_of_memory[] = "out of memory";
//static char netcdf_err_no_year_found[] = "no year found in filename";
//static char netcdf_err_unable_open_file[] = "unable to open file";
//static char netcdf_err_no_header_found[] = "no header found";
//
///* private */
//static unsigned int get_date(int year, int month, int day) {
//	return year*10000+month*100+day;
//}
//
///* private */
//static void netcdf_err_set(const char *const err, ...) {
//	va_list args;
//
//	va_start(args, err); 
//    vsprintf(g_netcdf_err, err, args);
//}
//
///* private */
//static void free_columns(char **columns, int columns_count) {
//	int i;
//
//	if ( columns ) {
//		for ( i = 0; i < columns_count; ++i ) {
//			if ( columns[i] ) {
//				free(columns[i]);
//			}
//		}
//		free(columns);
//	}
//}
//
///* private */
//static int meteo_get_row_from_date(int year, int month, int day) {
//	int i;
//	int row;
//
//	const int days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//
//	row = 0;
//	for ( i = 0; i < month-1; i++ ) {
//		row += days_per_month[i];
//	}
//	row += day-1; /* 'cause we start from 0...is a zero based index */
//
//	if ( IS_LEAP_YEAR(year) && month > 2 ) {
//		++row;
//	}
//
//	return row;
//}
//
///* private */
//int meteo_get_var_index(const METEO *const m, const char *const var) {
//	int i;
//
//	assert(m&& var);
//
//	for ( i = 0; i < m->columns_count; ++i ) {
//		if ( ! stricmp(m->header[i], var) ) {
//			return i;
//		}
//	}
//	return -1;
//}
//
//
///* */
//int netcdf_create(const DATASET *const dataset, const char *const filename) {
//	int i;
//	int y;
//	int z;
//	int k;
//	int file_id;
//	int lat_id;
//	int lon_id;
//	int ret;
//	char fake_string[2] = { 0 };
//	char **columns_no_leak;
//	PARAMETERS *ps;
//
//	char **columns;
//	int rows_count;
//	int columns_count;
//	int doy_dim;
//	int lat_dim;
//	int lon_dim;
//	int doy_id;
//	int var_dim[3];
//	int *var_id;
//	int *i_values;
//	double *values;
//	int index;
//	int func_ret;
//
//	const double missing_value = INVALID_VALUE;
//
//	const char sz_doy[] = "doy";
//	const char sz_year[] = "year";
//	const char sz_month[] = "month";
//	const char sz_day[] = "day";
//	const char sz_lat[] = "lat";
//	const char sz_lon[] = "lon";
//	const char sz_err_out_of_memory[] = "out of memory";
//	
//	/* */
//	assert(dataset && filename);
//
//	func_ret = 0;
//
//	/* */
//	netcdf_err_set(netcdf_err_no_error);
//
//	/* check dataset */
//	if ( ! dataset->site ) {
//		netcdf_err_set("site file not found");
//		return 0;
//	}
//
//	//if ( ! dataset->settings ) {
//	//	netcdf_err_set("settings file not found");
//	//	return 0;
//	//}
//
//	//if ( ! dataset->details_count ) {
//	//	netcdf_err_set("details file not found");
//	//	return 0;
//	//}
//
//	if ( ! dataset->meteo_count ) {
//		netcdf_err_set("meteo file not found");
//		return 0;
//	}
//
//	if ( ! dataset->init_count ) {
//		netcdf_err_set("input file not found");
//		return 0;
//	}
//
//	/* get rows and columns count */
//	rows_count = 0;
//	for ( i = 0; i < dataset->meteo_count; ++i ) {
//		rows_count += dataset->meteo[i]->rows_count;
//	}
//
//	/* we insert columns of first meteo dataset plus year column
//		this is to prevent to insert duplicate columns
//		and insert other columns
//	*/
//
//	columns_count = dataset->meteo[0]->columns_count + 1;
//	columns = malloc(columns_count*sizeof*columns);
//	if ( ! columns ) {
//		netcdf_err_set(sz_err_out_of_memory);
//		return 0;
//	}
//	for ( i = 0; i < columns_count; ++i ) {
//		columns[i] = NULL;
//	}
//	columns[0] = strdup(sz_year);
//	if ( ! columns[0] ) {
//		netcdf_err_set(sz_err_out_of_memory);
//		free(columns);
//		return 0;
//	}
//	for ( i = 0; i < dataset->meteo[0]->columns_count; ++i ) {
//		columns[i+1] = strdup(dataset->meteo[0]->header[i]);
//		if ( ! columns[i+1] ) {
//			netcdf_err_set(sz_err_out_of_memory);
//			free_columns(columns, columns_count);
//			return 0;
//		}
//	}
//
//	for ( i = 1; i < dataset->meteo_count; ++i ) {
//		for ( y = 0; y < dataset->meteo[i]->columns_count; ++y ) {
//			ret = 0;
//			for ( z = 0; z < columns_count; ++z ) {
//				if ( ! stricmp(columns[z], dataset->meteo[i]->header[y]) ) {
//					ret = 1;
//					break;
//				}
//			}
//			if ( ! ret ) {
//				columns_no_leak = realloc(columns, (columns_count+1)*sizeof*columns_no_leak);
//				if ( ! columns_no_leak ) {
//					netcdf_err_set(sz_err_out_of_memory);
//					free_columns(columns, columns_count);
//					return 0;
//				}
//				columns = columns_no_leak;
//				columns[columns_count] = strdup(dataset->meteo[i]->header[y]);
//				if ( ! columns[columns_count] ) {
//					netcdf_err_set(sz_err_out_of_memory);
//					free_columns(columns, columns_count);
//					return 0;
//				}
//				++columns_count;
//			}
//		}
//	}
//
//	var_id = malloc(columns_count*sizeof*var_id);
//	if ( ! var_id ) {
//		netcdf_err_set(sz_err_out_of_memory);
//		free_columns(columns, columns_count);
//		return 0;
//	}
//
//	values = malloc(rows_count*sizeof*values);
//	if ( ! values ) {
//		netcdf_err_set(sz_err_out_of_memory);
//		free(var_id);
//		free_columns(columns, columns_count);
//		return 0;
//	}
//
//	i_values = malloc(rows_count*sizeof*i_values);
//	if ( ! i_values ) {
//		netcdf_err_set(sz_err_out_of_memory);
//		free_columns(columns, columns_count);
//		free(var_id);
//		free(columns);
//		return 0;
//	}
//	
//	/* create file */
//	ret = nc_create(filename, NC_CLOBBER, &file_id);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* define dimensions */
//	ret = nc_def_dim(file_id, sz_doy, rows_count, &doy_dim);
//	if ( ret != NC_NOERR ) goto quit;
//	
//	ret = nc_def_dim(file_id, sz_lat, 1, &lat_dim);
//	if ( ret != NC_NOERR ) goto quit;
//
//	ret = nc_def_dim(file_id, sz_lon, 1, &lon_dim);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* define variables */
//	ret = nc_def_var(file_id, sz_doy, NC_INT, 1, &doy_dim, &doy_id);
//	if ( ret != NC_NOERR ) goto quit;
//
//	ret = nc_def_var(file_id, sz_lat, NC_DOUBLE, 1, &lat_dim, &lat_id);
//	if ( ret != NC_NOERR ) goto quit;
//
//	ret = nc_def_var(file_id, sz_lon, NC_DOUBLE, 1, &lon_dim, &lon_id);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* insert vars */
//	var_dim[0] = lat_dim;
//	var_dim[1] = lon_dim;
//	/* MANDATORY: this must be last */
//	var_dim[2] = doy_dim;
//
//	for ( z = 0; z < DATE_SIZE; ++z ) {
//		ret = nc_def_var(file_id, sz_date[z], NC_INT, 3, var_dim, &var_id[z]);
//		if ( ret != NC_NOERR ) goto quit;
//	}
//
//	/* we start from z 'cause we already added year, month and day */
//	for ( i = z; i < columns_count; i++ ) {
//		ret = nc_def_var(file_id, columns[i], NC_DOUBLE, 3, var_dim, &var_id[i]);
//		if ( ret != NC_NOERR ) goto quit;
//		/* set INVALID_VALUE */
//		ret = nc_put_att_double(file_id, var_id[i], "missing_value", NC_DOUBLE, 1, &missing_value);
//		if ( ret != NC_NOERR ) goto quit;
//	}
//
//	/* write site stuff */
//	if ( (ret != NC_NOERR) && (ret != NC_EINDEFINE) ) goto quit;
//	ps = dataset->site;
//	for ( i = 0; i < ps->count; ++i ) {
//		switch ( ps->value[i]->type ) {
//			case TYPE_CHAR:	
//				fake_string[0] = ps->value[i]->value.c;
//				ret = nc_put_att_text(file_id
//						, NC_GLOBAL, ps->name[i]
//						, 2, fake_string);
//			break;
//
//			case TYPE_PCHAR:
//				ret = nc_put_att_text(file_id
//						, NC_GLOBAL, ps->name[i]
//						, strlen(ps->value[i]->value.p)
//						, ps->value[i]->value.p);
//			break;
//
//			case TYPE_INT:	
//				ret = nc_put_att_int(file_id
//						, NC_GLOBAL, ps->name[i], NC_INT
//						, 1, &ps->value[i]->value.i);
//			break;
//
//			case TYPE_FLOAT:	
//				ret = nc_put_att_float(file_id
//						, NC_GLOBAL, ps->name[i], NC_FLOAT
//						, 1, &ps->value[i]->value.f);
//			break;
//
//			case TYPE_DOUBLE:	
//				ret = nc_put_att_double(file_id
//						, NC_GLOBAL, ps->name[i], NC_DOUBLE
//						, 1, &ps->value[i]->value.d);
//			break;
//
//			case TYPE_UNKNOWN:
//			default:
//				netcdf_err_set("unknown type of var \"%s\" in settings file.", ps->name[i]);
//				func_ret = 2;
//				goto quit;
//			break;
//		}
//
//		if ( ret != NC_NOERR ) {
//			goto quit;
//		}
//	}
//	nc_enddef(file_id);
//
//	/* insert datetime */
//	/*current_time = time(NULL);
//	strcpy(description,ctime(&current_time));
//	strlen(description);
//	description[strlen(description)-1] = '\0';
//	stat = nc_put_att_text(sr_ncid, NC_GLOBAL, "Created", strlen(description)+1, description*/
//
//	/* insert values, doy */
//	//z = 0;
//	//for ( i = 0; i < dataset->meteo_count; ++i ) {
//	//	for ( y = 0; y < dataset->meteo[i]->rows_count; ++y ) {
//	//		i_values[y+z] = y+1;
//	//	}
//	//	z += dataset->meteo[i]->rows_count;
//	//}
//	for ( i = 0; i < rows_count; ++i ) {
//		i_values[i]=i+1;
//	}
//	ret = nc_put_var_int(file_id, doy_id, i_values);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* insert values, year */
//	z = 0;
//	for ( i = 0; i < dataset->meteo_count; ++i ) {
//		for ( y = 0; y < dataset->meteo[i]->rows_count; ++y ) {
//			i_values[y+z] = dataset->meteo[i]->year;
//		}
//		z += dataset->meteo[i]->rows_count;
//	}
//	ret = nc_put_var_int(file_id, var_id[YEAR], i_values);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* insert values, month */
//	z = 0;
//	for ( i = 0; i < dataset->meteo_count; ++i ) {
//		k = meteo_get_var_index(dataset->meteo[i], sz_month);
//		if ( -1 == k ) {
//			netcdf_err_set("missing month column for meteo file, year %d",
//				dataset->meteo[i]->year);
//			goto quit;
//		}
//		for ( y = 0; y < dataset->meteo[i]->rows_count; ++y ) {
//			i_values[y+z] = (int)dataset->meteo[i]->rows[y][k];
//		}
//		z += dataset->meteo[i]->rows_count;
//	}
//	ret = nc_put_var_int(file_id, var_id[MONTH], i_values);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* insert values, day */
//	z = 0;
//	for ( i = 0; i < dataset->meteo_count; ++i ) {
//		k = meteo_get_var_index(dataset->meteo[i], "n_days");
//		if ( -1 == k ) {
//			netcdf_err_set("missing day column for meteo file, year %d",
//				dataset->meteo[i]->year);
//			goto quit;
//		}
//		for ( y = 0; y < dataset->meteo[i]->rows_count; ++y ) {
//			i_values[y+z] = (int)dataset->meteo[i]->rows[y][k];
//		}
//		z += dataset->meteo[i]->rows_count;
//	}
//	ret = nc_put_var_int(file_id, var_id[DAY], i_values);
//	if ( ret != NC_NOERR ) goto quit;
//
//	/* fill arr with missing_value */
//	for ( i = 0; i < rows_count; ++i ) {
//		values[i] = missing_value;
//	}
//
//	for ( y = DATE_SIZE; y < columns_count; ++y ) {
//		index = 0;	
//		for ( i = 0; i < dataset->meteo_count; ++i ) {
//			k = meteo_get_var_index(dataset->meteo[i], columns[y]);
//			if (  k != -1 ) {
//				for ( z = 0; z < dataset->meteo[i]->rows_count; ++z ) {
//					
//					values[index + meteo_get_row_from_date(dataset->meteo[i]->year
//											, dataset->meteo[i]->rows[z][0]
//											, dataset->meteo[i]->rows[z][1])
//					] = dataset->meteo[i]->rows[z][k];
//				}
//			}
//			index += dataset->meteo[i]->rows_count;
//		}
//		ret = nc_put_var_double(file_id, var_id[y], values);
//		if ( ret != NC_NOERR ) goto quit;
//	}
//
//	func_ret = 1;
//
//quit:
//	free(i_values);
//	free(values);
//	free_columns(columns, columns_count);
//	free(var_id);
//	if ( ! func_ret ) {
//		netcdf_err_set(nc_strerror(ret));
//	} else if ( 2 == func_ret ) {
//		func_ret = 0;
//	}
//	nc_close(file_id);
//	return func_ret;
//}
//
///* */
//const char *netcdf_err(void) {
//	return g_netcdf_err;
//}

const char * netcdf_get_version(void) {
	return nc_inq_libvers();
}