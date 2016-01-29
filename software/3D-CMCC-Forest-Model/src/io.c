/* io.c 

re-written by Alessio Ribeca on January 2016
please ASK before modify it!

*/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "common.h"
#include "netcdf/netcdf.h"


#ifdef _WIN32
#pragma comment(lib, "lib/netcdf")
#endif

/* do not change this order */
enum {
	YEAR = 0,
	MONTH,
	DAY,
	RG_F,
	TA_F,
	TMAX,
	TMIN,
	VPD_F,
	TS_F,
	PRECIP,
	SWC,
	NDVI_LAI,
	ET,
	LITTFALL,

	MET_COLUMNS
};

/* */
static const char comma_delimiter[] = ",\r\n";
static const char met_delimiter[] = " ,\t\r\n";
static const char *met_columns[MET_COLUMNS] = {
		"Year"
		, "Month"
		, "n_days"
		, "Rg_f"
		, "Ta_f"
		, "Tmax"
		, "Tmin"
		, "VPD_f"
		, "Ts_f"
		, "Precip"
		, "SWC"
		, "LAI"
		, "ET"
		, "LITTERFALL"
};

// ALESSIOR please note that leaf years are handled correctly
// so do not change 28 for february to 29!
static int days_per_month [] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const char *MonthName[MONTHS] = {
	"JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY"
	, "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
};

static const char err_out_of_memory[] = "out of memory.";


static void ResetYos(YOS *const yos)
{
	if ( yos )
	{
		int i;
		int y;

		yos->year = 0;
		for ( i = 0; i < MONTHS; ++i )
		{
			yos->m[i].n_days = INVALID_VALUE;
			yos->m[i].month = INVALID_VALUE;
			yos->m[i].solar_rad = INVALID_VALUE;
			yos->m[i].tavg = INVALID_VALUE;
			yos->m[i].tmax = INVALID_VALUE;
			yos->m[i].tmin = INVALID_VALUE;
			yos->m[i].tday = INVALID_VALUE;
			yos->m[i].tnight = INVALID_VALUE;
			yos->m[i].vpd = INVALID_VALUE;
			yos->m[i].ts_f = INVALID_VALUE;
			yos->m[i].rain = INVALID_VALUE;
			yos->m[i].swc = INVALID_VALUE;
			yos->m[i].ndvi_lai = INVALID_VALUE;
			yos->m[i].daylength = INVALID_VALUE;
			yos->m[i].thermic_sum = INVALID_VALUE;
			yos->m[i].avg_monthly_temp = INVALID_VALUE;
			yos->m[i].cum_monthly_rain = INVALID_VALUE;
			yos->m[i].rho_air = INVALID_VALUE;
			yos->m[i].et = INVALID_VALUE;
			yos->m[i].littfall = INVALID_VALUE;
			for ( y = 0; y < 31; ++y )
			{
				yos->m[i].d[y].n_days = INVALID_VALUE;
				yos->m[i].d[y].solar_rad = INVALID_VALUE;
				yos->m[i].d[y].tavg = INVALID_VALUE;
				yos->m[i].d[y].tmax = INVALID_VALUE;
				yos->m[i].d[y].tmin = INVALID_VALUE;
				yos->m[i].d[y].tday = INVALID_VALUE;
				yos->m[i].d[y].tnight = INVALID_VALUE;
				yos->m[i].d[y].vpd = INVALID_VALUE;
				yos->m[i].d[y].ts_f = INVALID_VALUE;
				yos->m[i].d[y].rain = INVALID_VALUE;
				yos->m[i].d[y].swc = INVALID_VALUE;
				yos->m[i].d[y].ndvi_lai = INVALID_VALUE;
				yos->m[i].d[y].daylength = INVALID_VALUE;
				yos->m[i].d[y].thermic_sum = INVALID_VALUE;
				yos->m[i].d[y].rho_air = INVALID_VALUE;
				yos->m[i].d[y].tsoil = INVALID_VALUE;
				yos->m[i].d[y].et = INVALID_VALUE;
				yos->m[i].d[y].littfall = INVALID_VALUE;

			}
		}
	}
}

typedef struct {
	char *name;
	size_t size;
} DIM;

void dims_free(DIM *dim, const int dims_count) {
	int i;

	if  ( dim ) {
		for ( i = 0; i < dims_count; ++i ) {
			free(dim[i].name);
		}
		free(dim);
	}
}

typedef struct {
	char *name;
	nc_type type;
	size_t size;
} ATT;

void atts_free(ATT *att, const int atts_count) {
	int i;

	if  ( att ) {
		for ( i = 0; i < atts_count; ++i ) {
			free(att[i].name);
		}
		free(att);
	}
}

//
int ImportNCFile(const char *const filename, YOS **pyos, int *const yos_count) {
	int i;
	int id_file;
	int ret;
	int dims_count;	/* dimensions */
	int vars_count;
	int atts_count;	/* attributes */
	int unl_count;	/* unlimited dimensions */
	char name[NC_MAX_NAME+1];
	char *p;
	nc_type type;
	size_t size;

	DIM *dims = NULL; /* mandatory */
	ATT *atts = NULL; /* mandatory */

	ret = nc_open(filename, NC_NOWRITE, &id_file);
    if ( ret != NC_NOERR ) goto quit;

	ret = nc_inq(id_file, &dims_count, &vars_count, &atts_count, &unl_count);
	if ( ret != NC_NOERR ) goto quit;

	if ( ! dims_count || ! vars_count ) {
		printf("bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		nc_close(id_file);
		return 0;
	}

	/* dimensions */
	dims = malloc(dims_count*sizeof*dims);
	if ( ! dims ) {
		puts("out of memory!");
		nc_close(id_file);
		return 0;
	}
	for ( i = 0; i < dims_count; ++i ) {
		ret = nc_inq_dim(id_file, i, name, &dims[i].size);
		if ( ret != NC_NOERR ) goto quit;
		dims[i].name = strdup(name);
		if  ( ! dims[i].name ) {
			puts("out of memory!");
			nc_close(id_file);
			dims_free(dims, dims_count);
			return 0;
		}
	}

	/* attributes */
	//atts = malloc(atts_count*sizeof*atts);
	//if ( ! atts ) {
	//	puts("out of memory!");
	//	nc_close(id_file);
	//	return 0;
	//}
	//for ( i = 0; i < atts_count; ++i ) {
	//	 ret = nc_inq_att(id_file, atts[i].name, &atts[i].type, &atts[i].size);
	//	 if ( ret != NC_NOERR ) goto quit;
	//}

quit:
	nc_close(id_file);
	dims_free(dims, dims_count);
	atts_free(atts, atts_count);
	if ( ret ) {
		puts(nc_strerror(ret));
		ret = 1;
	}
	return ! ret;
}

//
int ImportStandardFile(const char *const filename, YOS **pyos, int *const yos_count) {
	int i = 0,
		column = 0,
		month = 0,
		day = 0,
		year = 0,
		error_flag = 0,
		columns[MET_COLUMNS];

	int no_year_column;
	int current_year = 0;

	char *token = NULL,
	*token2 = NULL,
	*p,
	buffer[BUFFER_SIZE] = { 0 };

	static double previous_solar_rad,
		previous_tavg,
		previous_tmax,
		previous_tmin,
		previous_vpd,
		previous_ts_f,
		previous_rain,
		previous_swc,
		previous_ndvi_lai;

	FILE *f;
	YOS *yos;
	YOS *yos_no_leak;

	assert(pyos);

	//
	yos = *pyos;

	// open file
	f = fopen(filename, "r");
	if ( !f )
	{
		puts("unable to open met data file !");
		Log("unable to open met data file !\n");
		free(yos);
		return 0;
	}

	// get header
	if ( ! fgets(buffer, BUFFER_SIZE, f) )
	{
		puts("empty met data file ?");
		Log ("empty met data file ?\n");
		free(yos);
		fclose(f);
		return 0;
	}

	// reset
	year = 0;
	month = INVALID_VALUE;
	day = INVALID_VALUE;
	current_year = 0;
	for ( i = 0; i < MET_COLUMNS; ++i )
	{
		columns[i] = -1;
	}

	// parse header
	for ( column = 0, token2 = mystrtok(buffer, met_delimiter, &p); token2; token2 = mystrtok(NULL, met_delimiter, &p), column++ )
	{
		for ( i = 0; i < MET_COLUMNS; ++i )
		{
			if ( ! mystricmp(token2, met_columns[i]) )
			{
				if  ( -1 != columns[i] )
				{
					printf("met column %s already assigned.\n\n", token2);
					Log("met column %s already assigned.\n\n", token2);
					free(yos);
					fclose(f);
					return 0;
				}
				columns[i] = column;
				break;
			}
		}
	}

	// check if each columns was assigned
	no_year_column = 0;
	for ( i = 0; i < MET_COLUMNS; ++i )
	{
		if ( -1 == columns[i] )
		{
			/* get year from filename */
			if ( YEAR == i )
			{
				char *p = strrchr(filename, '_');
				if ( p )
				{
					int y;

					++p;
					for ( y = 0; p[y]; ++y );
					if ( y > 4 )
					{
						p[4] = '\0';
						year = convert_string_to_int(p, &error_flag);
						if ( ! error_flag )
						{
							no_year_column = 1;
							continue;
						}
					}
				}
			}
			printf("met column %s not found.\n\n", met_columns[i]);
			fclose(f);
			return 0;
		}
	}
	
	if ( (! no_year_column && (YEAR != columns[YEAR])) || (MONTH-no_year_column != columns[MONTH]) || (DAY-no_year_column != columns[DAY]) )
	{
		puts("date must be on first columns!\n\n");
		free(yos);
		fclose(f);
		return 0;
	}

	// parse rows
	while ( fgets(buffer, BUFFER_SIZE, f) )
	{
		// remove carriage return and newline ( if any )
		for ( i = 0; buffer[i]; i++ )
		{
			if ( ('\n' == buffer[i]) || ('\r' == buffer[i]) )
			{
				buffer[i] = '\0';
				break;
			}
		}

		// skip empty lines
		if ( '\0' == buffer[0] )
		{
			continue;
		}

		// parse string
		for ( column = no_year_column, token2 = mystrtok(buffer, met_delimiter, &p); token2; token2 = mystrtok(NULL, met_delimiter, &p), ++column )
		{
			if ( column <= DAY )
			{
				i = convert_string_to_int(token2, &error_flag);
				if ( error_flag )
				{
					printf("unable to convert value \"%s\" for %s column\n", token2, met_columns[column+no_year_column]);
					Log("unable to convert value \"%s\" for %s column\n", token2, met_columns[column+no_year_column]);
					free(yos);
					fclose(f);
					return 0;
				}

				switch ( column )
				{
					case YEAR:
						if ( ! i )
						{
							puts("year cannot be zero!\n");
							Log("year cannot be zero!\n");
							free(yos);
							fclose(f);
							return 0;
						}
						year = i;
					break;

					case MONTH:
						if ( i < 1 || i > MONTHS )
						{
							printf("bad %s month for year %d\n\n", MonthName[i-1], year);
							Log("bad %s month for year %d\n\n", MonthName[i-1], year);
							free(yos);
							fclose(f);
							return 0;

						}
						month = --i;
					break;

					case DAY:
						if ( (i <= 0) || i > days_per_month[month] + (((1 == month) && IS_LEAP_YEAR(year)) ? 1 :0 ) )
						{
							printf("bad day for %s %d\n\n", MonthName[month], year);
							Log("bad day for %s %d\n\n", MonthName[month], year);
							free(yos);
							fclose(f);
							return 0;
						}
						day = --i;
					break;
				}

				if ( current_year != year )
				{
					yos_no_leak = realloc(yos, (*yos_count+1)*sizeof*yos_no_leak);
					if ( ! yos_no_leak )
					{
						puts(err_out_of_memory);
						free(yos);
						fclose(f);
						return 0;
					}
					yos = yos_no_leak;
					ResetYos(&yos[*yos_count]);

					yos[*yos_count].year = year;
					++*yos_count;
					
					current_year = year;
				}

				/* check */
				if ( (DAY == column) )
				{
					// monthly version
					if ( 'm' == settings->time ) {
						yos[*yos_count-1].m[month].n_days = day+1;
						if (yos[*yos_count-1].m[month].n_days > (int)settings->maxdays)
						{
							puts("ERROR IN N_DAYS DATA!!\n");
							Log("ERROR IN N_DAYS DATA!!\n");
							free(yos);
							fclose(f);
							return 0;
						}
					// daily
					} else if ( ! strncmp (settings->daymet, "off", 3) ) {
						yos[*yos_count-1].m[month].d[day].n_days = day+1;
						if (yos[*yos_count-1].m[month].d[day].n_days > (int)settings->maxdays)
						{
							puts("ERROR IN N_DAYS DATA!!\n");
							Log("ERROR IN N_DAYS DATA!!\n");
							free(yos);
							fclose(f);
							return 0;
						}
					}
					else if (strncmp (settings->daymet, "on", 3)== 0)
					{
						//todo insert daymet functions
						Log("DAYMET\n");
						free(yos);
						fclose(f);
						return 0;
					}
					else
					{
						Log("ERROR NO CORRECT CHOICE\n");
						free(yos);
						fclose(f);
						return 0;
					}
				}
			}
			else
			{
				for ( i = DAY+1; i < MET_COLUMNS; i++ )
				{
					if ( column == (columns[i] + no_year_column))
					{
						//set value for monthly version
						if (settings->time == 'm')
						{
							//printf("opening met file year %d \n", year);
							switch ( i )
							{
								
							case RG_F: //Rg_f - solar_rad -daily average solar radiation
								yos[*yos_count-1].m[month].solar_rad = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].solar_rad) && *yos_count > 1 )
								{
									Log ("* SOLAR RAD -NO DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
									Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].solar_rad = yos[*yos_count-2].m[month].solar_rad;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].solar_rad))
									{
										Log ("********* SOLAR RAD -NO DATA- in previous year!!!!\n" );
									}
								}
								//CONTROL
								if (yos[*yos_count-1].m[month].solar_rad > settings->maxrg )
								{
									Log("ERROR IN RG DATA Rg > MAXRg in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
								}
								//convert daily average solar radiation to monthly solar radiation
								//m[month].solar_rad *= m[month].n_days;
								break;

							case TA_F: //Ta_f -  temperature average
								yos[*yos_count-1].m[month].tavg = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tavg) && *yos_count > 1 )
								{
									//Log ("* T_AVG -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tavg = yos[*yos_count-2].m[month].tavg;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tavg))
									{
										//Log ("********* T_AVG -NO DATA- in previous year!!!!\n" );
									}
								}
								//CONTROL
								if (yos[*yos_count-1].m[month].tavg > settings->maxtavg)
								{
									Log("ERROR IN TAV DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
								}
								break;

							case TMAX: //Tmax -  maximum temperature
								yos[*yos_count-1].m[month].tmax = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tmax) && *yos_count > 1 )
								{
									Log ("* T_MAX -NO DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tmax = yos[*yos_count-2].m[month].tmax;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tmax))
									{
										//Log ("********* T_MAX -NO DATA- in previous year!!!!\n" );
									}
								}

								break;


							case TMIN: //Tmin -  minimum temperature
								yos[*yos_count-1].m[month].tmin = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tmin) && *yos_count > 1 )
								{
									Log ("* TMIN -NO DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tmin = yos[*yos_count-2].m[month].tmin;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tmin))
									{
										//Log ("********* T_MIN -NO DATA- in previous year!!!!\n" );
									}
								}
								break;

							case VPD_F: //RH_f - RH
								yos[*yos_count-1].m[month].vpd = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].vpd) && *yos_count > 1 )
								{
									Log ("* VPD -NO DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
									yos[*yos_count-1].m[month].vpd = yos[*yos_count-2].m[month].vpd;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].vpd))
									{
										Log ("********* VPD -NO DATA- in previous year!!!!\n" );
									}
								}
								break;

							case TS_F: // ts_f   Soil temperature
								yos[*yos_count-1].m[month].ts_f = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ts_f) && *yos_count > 1 )
								{
									//Log ("* TS_F -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									yos[*yos_count-1].m[month].ts_f = yos[*yos_count-2].m[month].ts_f;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ts_f))
									{
										//Log ("********* TS_F -NO DATA- in previous year!!!!\n" );
									}
								}
								break;

							case PRECIP:  //Precip - rain
								yos[*yos_count-1].m[month].rain = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].rain) && *yos_count > 1 )
								{
									Log ("* PRECIPITATION -NO DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
									Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].rain = yos[*yos_count-2].m[month].rain;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].rain))
									{
										Log ("********* PRECIPITATION -NO DATA- in previous year!!!!\n" );

									}
									//Log("precipitation of previous year = %f mm\n", yos[*yos_count-1].m[month].rain);
								}
								
								//CONTROL
								if (yos[*yos_count-1].m[month].rain > settings->maxprecip)
								{
									Log("ERROR IN PRECIP DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
								}
								break;

							case SWC: //Soil Water Content (%)
								yos[*yos_count-1].m[month].swc = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return 0;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].swc) && *yos_count > 1)
								{
									//Log ("********* SWC -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].swc = yos[*yos_count-2].m[month].swc;
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].swc))
									{
										//Log ("* SWC -NO DATA- in previous year!!!!\n" );
										yos[*yos_count-1].m[month].swc = NO_DATA;
									}
								}
								break;
							case NDVI_LAI: //Get LAI in spatial version
								if (settings->spatial == 's')
								{
									yos[*yos_count-1].m[month].ndvi_lai = convert_string_to_prec(token2, &error_flag);


									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return 0;
									}
									//if is not the first year the model get the previous year value
									if ( *yos_count > 1 )
									{

										//control in lai data if is an invalid value
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ndvi_lai))
										{
											Log ("********* LAI -NO DATA in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
											//Log("Getting previous years values !!\n");
											yos[*yos_count-1].m[month].ndvi_lai = yos[*yos_count-2].m[month].ndvi_lai;
											if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ndvi_lai))
											{
												Log ("* LAI -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].ndvi_lai = NO_DATA;
											}
										}
										//control lai data in spatial version if value is higher than MAXLAI
										if(yos[*yos_count-1].m[month].ndvi_lai > settings->maxlai)
										{
											Log("********* INVALID DATA LAI > MAXLAI in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
											Log("Getting previous years values !!\n");
											yos[*yos_count-1].m[month].ndvi_lai = yos[*yos_count-2].m[month].ndvi_lai;
										}
									}
									//for the first year if LAI is an invalid value set LAI to a default value DEFAULTLAI
									else
									{
										if(yos[*yos_count-1].m[month].ndvi_lai > settings->maxlai)
										{
											//RISOLVERE QUESTO PROBLEMA PER NON AVERE UN DEFUALT LAI!!!!!!!!!!!!!
											//
											//
											//
											Log("**********First Year without a valid LAI value set to default value LAI\n");
											yos[*yos_count-1].m[month].ndvi_lai = settings->defaultlai;
											Log("**DEFAULT LAI VALUE SET TO %d\n", settings->defaultlai);
										}
									}
								}
								break;
							case ET: //Evapotranspiration for stand alone RothC
									yos[*yos_count-1].m[month].et = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].et) && *yos_count > 1 )
									{
										//Log ("* ET-NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										//Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].et = yos[*yos_count-2].m[month].et;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].et))
										{
											//Log ("********* ET -NO DATA- in previous year!!!!\n" );
										}
									}
									break;
							case LITTFALL: //Litterfall  for stand alone RothC
									yos[*yos_count-1].m[month].littfall = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										Log("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].littfall) && *yos_count > 1 )
									{
										//Log ("* littfall-NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										//Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].littfall = yos[*yos_count-2].m[month].littfall;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].littfall))
										{
											//Log ("********* LITTFALL -NO DATA- in previous year!!!!\n" );
										}
									}
									break;
							}
						}
						//set values for daily version
						else
						{
							if (strncmp (settings->daymet, "off", 3)== 0)
							{
								switch ( i )
								{								
								case RG_F: //Rg_f - solar_rad -daily average solar radiation
									yos[*yos_count-1].m[month].d[day].solar_rad = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									else
									{
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad) && (!((day == 0) && (1 == *yos_count) && (month == 0))))
									{

										//the model gets the value of the day before
										Log ("* SOLAR RAD -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
										//Log("Getting previous day values.. !!\n");
										yos[*yos_count-1].m[month].d[day].solar_rad = previous_solar_rad;
										//Log("..value of the previous day = %f\n", yos[*yos_count-1].m[month].d[day].solar_rad);
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad))
										{
											//Log ("********* SOLAR RAD -NO DATA- in previous day!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].solar_rad = yos[*yos_count-2].m[month].d[day].solar_rad;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad))
											{
												//Log ("********* SOLAR RAD -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].solar_rad = NO_DATA;
											}
										}
									}
									else
									{
										previous_solar_rad = yos[*yos_count-1].m[month].d[day].solar_rad;
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].solar_rad > settings->maxrg )
									{
										//Log("ERROR IN RG DATA Rg > MAXRg in year %s month %s day %d!!!!\n", year, MonthName[month], day );
									}
									break;

								case TA_F: //Ta_f -  temperature average
									yos[*yos_count-1].m[month].d[day].tavg = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}

									if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tavg) && (!((day == 0) && (*yos_count == 1) && (month == 0))))
									{
										//the model gets the value of the day before
										Log ("* TAVG -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);

										yos[*yos_count-1].m[month].d[day].tavg = previous_tavg;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tavg))
										{
											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].tavg = yos[*yos_count-2].m[month].d[day].tavg;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tavg))
											{
												//Log ("********* TAVG -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].tavg = NO_DATA;
											}
										}
									}
									else
									{
										previous_tavg = yos[*yos_count-1].m[month].d[day].tavg;
									}

									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].tavg > settings->maxtavg)
									{
										//Log("ERROR IN TAV DATA in year %s month %s!!!!\n", year, MonthName[month] );
									}
									//Log("%d-%s-tavg = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;

								case TMAX: //TMAX -  maximum temperature
									yos[*yos_count-1].m[month].d[day].tmax = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									else
									{
										//Log("tmax = %f\n", yos[*yos_count-1].m[month].d[day].tmax);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										Log ("* TMAX -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
										//Log("Getting previous day values.. !!\n");
										yos[*yos_count-1].m[month].d[day].tmax = yos[*yos_count-1].m[month].d[day].tavg;
										Log("..using tavg = %f\n", yos[*yos_count-1].m[month].d[day].tavg);
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax))
										{
											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].tmax = yos[*yos_count-2].m[month].d[day].tmax;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax))
											{
												//Log ("********* TMAX -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].tmax = NO_DATA;
											}
										}
									}
									else
									{
										previous_tmax = yos[*yos_count-1].m[month].d[day].tmax;
									}
									//Log("%d-%s-tavg = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;

								case TMIN: //TMIN -  minimum temperature
									yos[*yos_count-1].m[month].d[day].tmin = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmin) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										Log ("* TMIN -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
										//Log("Getting previous day values.. !!\n");
										yos[*yos_count-1].m[month].d[day].tmin = yos[*yos_count-1].m[month].d[day].tavg;
										Log("..using tavg = %f\n", yos[*yos_count-1].m[month].d[day].tavg);
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmin))
										{
											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].tmin = yos[*yos_count-2].m[month].d[day].tmin;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmin))
											{
												//Log ("********* TMIN -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].tmin = NO_DATA;
											}
										}
									}
									else
									{
										previous_tmin = yos[*yos_count-1].m[month].d[day].tmin;
									}
									break;

								case VPD_F: //RH_f - RH
									yos[*yos_count-1].m[month].d[day].vpd = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}

									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										Log ("* VPD -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
										//Log("Getting previous day values.. !!\n");
										yos[*yos_count-1].m[month].d[day].vpd = previous_vpd;
										//Log("..value of the previous day = %f\n", yos[*yos_count-1].m[month].d[day].vpd);
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd))
										{
											//Log ("********* VPD -NO DATA- in previous year!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].vpd = yos[*yos_count-2].m[month].d[day].vpd;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd))
											{
												//Log ("********* VPD -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].vpd = NO_DATA;
											}

										}
									}
									else
									{
										previous_vpd = yos[*yos_count-1].m[month].d[day].vpd;
									}
									//Log("%d-%s-vpd = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].vpd);
									break;

								case TS_F: // ts_f   Soil temperature
									yos[*yos_count-1].m[month].d[day].ts_f = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										//Log ("* TS_F -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										yos[*yos_count-1].m[month].d[day].ts_f = previous_ts_f;
										/*
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f))
										{
											//Log ("********* TS_F -NO DATA- in previous year!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].ts_f = yos[*yos_count-1].m[month].d[day].ts_f;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f))
											{
												//Log ("********* TS_F -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].ts_f = NO_DATA;
											}
										}
										*/
									}
									else
									{
										previous_ts_f = yos[*yos_count-1].m[month].d[day].ts_f;
									}
									//Log("%d-%s-ts_f = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].ts_f);
									break;

								case PRECIP:  //Precip - rain
									yos[*yos_count-1].m[month].d[day].rain = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].rain) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										Log ("* PRECIPITATION -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day+1);
										//Log("Getting previous day values.. !!\n");
										yos[*yos_count-1].m[month].d[day].rain = previous_rain;
										Log("..value of the previous day = %f\n", yos[*yos_count-1].m[month].d[day].rain);
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].rain))
										{
											Log ("********* PRECIPITATION -NO DATA- in previous year!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].rain = yos[*yos_count-2].m[month].d[day].rain;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].rain))
											{
												Log ("********* RAIN -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].rain = NO_DATA;
											}

										}
										//Log("precipitation of previous year = %f mm\n", yos[*yos_count-1].m[month].rain);
									}
									else
									{
										previous_rain = yos[*yos_count-1].m[month].d[day].rain;
									}

									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].rain > settings->maxprecip)
									{
										Log("ERROR IN PRECIP DATA in year %d month %s!!!! %f\n", yos[*yos_count-1].year, MonthName[month], settings->maxprecip);
									}
									//Log("%d-%s-precip = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].rain);
									break;

								case SWC: //Soil Water Content (%)

									yos[*yos_count-1].m[month].d[day].swc = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										//Log ("********* SWC -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										//Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].swc = previous_swc;
										/*
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc))
										{
											//Log ("* SWC -NO DATA- in previous year!!!!\n" );
											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].swc = yos[*yos_count-1].m[month].d[day].swc;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc))
											{
												//Log ("********* RAIN -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].swc = NO_DATA;
											}
										}
										*/
									}
									else
									{
										previous_swc = yos[*yos_count-1].m[month].d[day].swc;
									}

									//Log("%d-%s-swc= %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].swc);
									break;
								case NDVI_LAI: //Get LAI in spatial version
									if (settings->spatial == 's')
									{
										yos[*yos_count-1].m[month].d[day].ndvi_lai = convert_string_to_prec(token2, &error_flag);


										if ( error_flag )
										{
											printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
											Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
											free(yos);
											fclose(f);
											return 0;
										}
										//if is not the first year the model get the previous year value
										if (*yos_count > 1)
										{

											//control in lai data if is an invalid value
											if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ndvi_lai) && (!((day == 0) && (month == 0))))
											{
												//the model gets the value of the day before
												Log ("********* LAI -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day+1 );
												//Log("Getting previous years values !!\n");
												yos[*yos_count-1].m[month].d[day].ndvi_lai = previous_ndvi_lai;
												if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ndvi_lai))
												{
													Log ("* LAI -NO DATA- in previous year!!!!\n" );
													yos[*yos_count-1].m[month].d[day].ndvi_lai = NO_DATA;
												}
											}
											else
											{
												previous_ndvi_lai = yos[*yos_count-1].m[month].d[day].ndvi_lai;
											}
											//control lai data in spatial version if value is higher than MAXLAI
											if(yos[*yos_count-1].m[month].d[day].ndvi_lai > settings->maxlai)
											{
												Log("********* INVALID DATA LAI > MAXLAI in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
												Log("Getting previous day values.. !!\n");
												yos[*yos_count-1].m[month].d[day].ndvi_lai = yos[*yos_count-2].m[month].d[day].ndvi_lai;
												Log("..value of the previous day = %f\n", yos[*yos_count-1].m[month].d[day].ndvi_lai);
											}
										}
										//for the first year if LAI is an invalid value set LAI to a default value DEFAULTLAI
										else
										{
											if(yos[*yos_count-1].m[month].d[day].ndvi_lai > settings->maxlai)
											{
												//todo RISOLVERE QUESTO PROBLEMA PER NON AVERE UN DEFUALT LAI!!!!!!!!!!!!!
												//
												//
												//
												Log("**********First Year without a valid LAI value set to default value LAI\n");
												yos[*yos_count-1].m[month].d[day].ndvi_lai = settings->defaultlai;
												Log("**DEFAULT LAI VALUE SET TO %d\n", settings->defaultlai);
											}
										}
									}
									break;
								case ET: //ET for stand alone RothC
									yos[*yos_count-1].m[month].d[day].et = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}

									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].et) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										//Log ("* ET -NO DATA in year %s month %s, day %d!!!!\n", year, MonthName[month], day);
									}
									//Log("%d-%s-tavg = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;
								case LITTFALL: //Littfall for stand alone RothC
									yos[*yos_count-1].m[month].d[day].littfall = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
										free(yos);
										fclose(f);
										return 0;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].littfall) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
									{
										//the model gets the value of the day before
										//Log ("* litterfall -NO DATA in year %s month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
									}
									//Log("%d-%s-tavg = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;
								}
							}
							else if (strncmp (settings->daymet, "on", 3)== 0)
							{
								//todo insert daymet functions
								Log("DAYMET\n");
							}
							else
							{
								Log("ERROR NO CORRECT CHOICE\n");
							}
						}
					}
				}
			}
		}
	}


	// check for month
	if (settings->time == 'm')
	{
		if ( month != MONTHS )
		{
			printf("missing values in met data file, %d months imported instead of %d !\n", month+1, MONTHS);
			Log("missing values in met data file, %d months imported instead of %d !\n", month+1, MONTHS);
			free(yos);
			return 0;
		}
	}
	Log ("ok met data\n");

	fclose(f);

	*pyos = yos;

	/* ok */
	return 1;
}




/*import met data file*/
//------------------------------------------------------------------------------
// *file is the comma separated files list!!! not a single file
// initially yos_count is equal to 0
YOS *ImportYosFiles(char *file, int *const yos_count)
{
	int i;
	char *token;
	char *p;
	char *p2;
	char *filename;
	YOS *yos;

	assert(file && yos_count);

	// reset
	yos = NULL; // MANDATORY!!!!!!!!!
	*yos_count = 0;

	//
	Log("Processing met data files...\n");

	// process met files
	for (token = mystrtok(file, comma_delimiter, &p); token; token = mystrtok(NULL, comma_delimiter, &p) )
	{
		// get token length
		for ( i = 0; token[i]; ++i );

		// if length is 0 skip to next token
		if ( ! i )
		{
			continue;
		}

		// Get only filename.txt from X:\folder\filename.txt
		// or /this/is/the/complete/path/of/filename.txt
		filename = (strrchr(token, '\\'));
		if ( filename ) ++filename;
		p2 = (strrchr(token, '/'));
		if ( p2 ) ++p2;
		if  ( p2 > filename )
		{
			filename = p2;
		}
		if ( ! filename )
		{
			filename = token;
		}
		Log("opening met file '%s' \n", filename);

		i = 0; // flag for netcdf file
		p2 = strrchr(filename, '.');
		if ( p2 ) {
			++p2;
			if ( ! mystricmp(p2, "nc") )
			{
				i = 1; // is a netcdf file!
			}
		}
		if ( ! i )
		{
			i = ImportStandardFile(token, &yos, yos_count);
		}
		else
		{
			i = ImportNCFile(token, &yos, yos_count);
		}
		if ( ! i )
		{
			return NULL;
		}	
	}

	// sort
	if ( *yos_count > 1 )
	{
		qsort(yos, *yos_count, sizeof*yos, sort_by_years);
	}

	Log ("ok met data importing\n");

#if 1
	/* save imported yos for debugging purposes */
	{
		char buffer[64];
		int i;
		int month;
		int z;
		FILE *f;

		for ( i = 0; i < *yos_count; ++i ) {
			sprintf(buffer, "debug_met_monthly_%d.csv", yos[i].year);
			f = fopen(buffer, "w");
			if ( ! f ) {
				printf("unable to create %s\n", buffer);
				free(yos);
				return NULL;
			}
			fputs("year,month,n_days,month,solar_rad,tavg,tmax,tmin,tday,tnight,vpd,ts_f"
					",rain,swc,ndvi_lai,daylength,thermic_sum,avg_monthly_temp"
					",cum_monthly_rain,rho_air,et,littfall\n", f);
			for ( month = 0; month < 12; ++month ) {
				fprintf(f, "%d,%d,%d,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n"
							, yos[i].year
							, month+1
							, yos[i].m[month].n_days
							, yos[i].m[month].month
							, yos[i].m[month].solar_rad
							, yos[i].m[month].tavg
							, yos[i].m[month].tmax
							, yos[i].m[month].tmin
							, yos[i].m[month].tday
							, yos[i].m[month].tnight
							, yos[i].m[month].vpd
							, yos[i].m[month].ts_f
							, yos[i].m[month].rain
							, yos[i].m[month].swc
							, yos[i].m[month].ndvi_lai
							, yos[i].m[month].daylength
							, yos[i].m[month].thermic_sum
							, yos[i].m[month].avg_monthly_temp
							, yos[i].m[month].cum_monthly_rain
							, yos[i].m[month].rho_air
							, yos[i].m[month].et
							, yos[i].m[month].littfall);
			}		
			fclose(f);

			sprintf(buffer, "debug_met_daily_%d.csv", yos[i].year);
			f = fopen(buffer, "w");
			if ( ! f ) {
				printf("unable to create %s\n", buffer);
				free(yos);
				return NULL;
			}
			fputs("year,month,day,n_days,solar_rad,tavg,tmax,tmin,tday,tnight,vpd,ts_f"
					",rain,swc,ndvi_lai,daylength,thermic_sum"
					",rho_air,tsoil,et,littfall\n", f);

			for ( month = 0; month < 12; ++month ) {
				for ( z = 0; z < 31; ++z ) {
					if ( z == yos[i].m[month].n_days )
					{
						break;
					}
					fprintf(f, "%d,%d,%d,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n"
							, yos[i].year
							, month+1
							, z+1
							, yos[i].m[month].d[z].n_days
							, yos[i].m[month].d[z].solar_rad
							, yos[i].m[month].d[z].tavg
							, yos[i].m[month].d[z].tmax
							, yos[i].m[month].d[z].tmin
							, yos[i].m[month].d[z].tday
							, yos[i].m[month].d[z].tnight
							, yos[i].m[month].d[z].vpd
							, yos[i].m[month].d[z].ts_f
							, yos[i].m[month].d[z].rain
							, yos[i].m[month].d[z].swc
							, yos[i].m[month].d[z].ndvi_lai
							, yos[i].m[month].d[z].daylength
							, yos[i].m[month].d[z].thermic_sum
							, yos[i].m[month].d[z].rho_air
							, yos[i].m[month].d[z].tsoil
							, yos[i].m[month].d[z].et
							, yos[i].m[month].d[z].littfall);
				}
			}		
			fclose(f);
		}
	}
#endif

	return yos;
}

