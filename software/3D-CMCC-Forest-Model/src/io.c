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

/* */
extern char *program_path;

/* */
//#define NC_CONV_FILENAME		"nc_conv.txt"
#define VPD_RANGE_MIN					-5
#define VPD_RANGE_MAX					120

/* do not change this order */
enum {
	YEAR = 0
	, MONTH
	, DAY
	, RG_F
	, TA_F
	, TMAX
	, TMIN
	, VPD_F
	, TS_F
	, PRECIP
	, SWC
	, NDVI_LAI
	, ET
	, WS_F
	, RH_F

	, MET_COLUMNS
};

/* */
static const char comma_delimiter[] = ",\r\n";
static const char met_delimiter[] = " ,\t\r\n";
static const char sz_err_out_of_memory[] = "out of memory.";

/* DO NOT CHANGE THIS ORDER */
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

/* do not change this order */
static const char *met_columns[MET_COLUMNS+2] = {
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
		, "WS_f"
		, "RH_f"

		/* hack */
		, "LAT"
		, "LON"
};

// ALESSIOR please note that leap years are handled correctly
// so do not change 28 (february) to 29!
static int days_per_month [] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const char *MonthName[MONTHS] = {
	"JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY"
	, "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
};

//
static int loadFileToMemory(const char *filename, char **result) {
	size_t size;
	FILE *f;

	size = 0;
	f = fopen(filename, "rb");
	if ( !f )  {
		*result = NULL;
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (char *)malloc((size+1)*sizeof**result);
	if ( !*result ) {
		fclose(f);
		return 0;
	}
	if ( size != fread(*result, sizeof(char), size, f) )  {
		free(*result);
		fclose(f);
		return 0;
	}
	fclose(f);
	(*result)[size] = '\0';

	return size;
}

void FreeOutputVars(OUTPUT_VARS *ov) {
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

OUTPUT_VARS *ImportOutputVarsFile(const char *const filename)
{
	char *token;
	char *p;
	char *buffer;
	int i;
	OUTPUT_VARS *ov;
	int *int_no_leak;
	int flag;

	const char delimiter[] = " ,\r\n";
	
	assert(filename);

	if ( ! loadFileToMemory(filename, &buffer) )
	{
		Log("unable to import output filename: %s\n", filename);
		return NULL;
	}

	ov = malloc(sizeof*ov);
	if ( ! ov ) {
		Log(sz_err_out_of_memory);
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

	for ( token = mystrtok(buffer, delimiter, &p); token; token = mystrtok(NULL, delimiter, &p) ) {
		if ( token[0] ) {
			flag = 0;
			for ( i = 0; i < OUTPUT_VARS_COUNT; ++i ) {
				if ( ! mystricmp(token, sz_output_vars[i]) ) {
					/* daily */
					if ( ('d' == token[0]) || ('D' == token[0]) ) {
						int_no_leak = realloc(ov->daily_vars, (ov->daily_vars_count+1)*sizeof*int_no_leak);
						if ( ! int_no_leak )
						{
							Log(sz_err_out_of_memory);
							FreeOutputVars(ov);
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
							Log(sz_err_out_of_memory);
							FreeOutputVars(ov);
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
							Log(sz_err_out_of_memory);
							FreeOutputVars(ov);
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
				Log("%s is an unknown output var. skipped\n", token);
			}
		}
	}

	free(buffer);

	return ov;
}

static void ResetYos(YOS *const yos)
{
	if ( yos )
	{
		int i;
		int y;

		yos->year = 0;
		for ( i = 0; i < MONTHS; ++i )
		{
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
				yos->m[i].d[y].prcp = INVALID_VALUE;
				yos->m[i].d[y].swc = INVALID_VALUE;
				yos->m[i].d[y].ndvi_lai = INVALID_VALUE;
				yos->m[i].d[y].daylength = INVALID_VALUE;
				yos->m[i].d[y].thermic_sum = INVALID_VALUE;
				yos->m[i].d[y].rho_air = INVALID_VALUE;
				yos->m[i].d[y].tsoil = INVALID_VALUE;
				yos->m[i].d[y].et = INVALID_VALUE;
				yos->m[i].d[y].windspeed = INVALID_VALUE;
				yos->m[i].d[y].rh_f = INVALID_VALUE;
				yos->m[i].d[y].lh_vap = INVALID_VALUE;
				yos->m[i].d[y].lh_vap_soil = INVALID_VALUE;
				yos->m[i].d[y].lh_fus = INVALID_VALUE;
				yos->m[i].d[y].lh_sub = INVALID_VALUE;
				yos->m[i].d[y].air_pressure = INVALID_VALUE;

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

/* private */
void compute_vpd(double *const values, const int rows_count, const int columns_count) {
#define VALUE_AT(r,c)	((r)+((c)*rows_count))
	int i;
	double value;

	for ( i = 0; i < rows_count; i++ ) {
		double ta = values[VALUE_AT(i, TA_F)];
		double rh = values[VALUE_AT(i, RH_F)];
		value = INVALID_VALUE;

		if ( ! IS_INVALID_VALUE(values[VALUE_AT(i, TA_F)]) && ! IS_INVALID_VALUE(values[VALUE_AT(i, RH_F)]) ) {
			/* 6.11 is for hPa */
			value = 6.1076 * exp(17.26938818 * values[VALUE_AT(i, TA_F)] / (237.3 + values[VALUE_AT(i, TA_F)]));
			value *= (1 - values[VALUE_AT(i, RH_F)] / 100.0);
			/* convert NaN to invalid value */
			if ( value != value ) {
				value = INVALID_VALUE;
			}

			/* check range */
			if ( ! IS_INVALID_VALUE(value) ) {
				if ( value < VPD_RANGE_MIN || value > VPD_RANGE_MAX ) {
					value = INVALID_VALUE;
				}
			}
		}
		values[VALUE_AT(i, VPD_F)] = value;
	}
#undef VALUE_AT
}



/* */
int yos_from_arr(const double *const values, const int rows_count, const int columns_count, YOS **p_yos, int *const yos_count) {
#define VALUE_AT(r,c)	((r)+((c)*rows_count))

	YOS *yos_no_leak;
	YOS *yos;
	int row;
	int year;
	int month;
	int day;
	int current_year;

	static double previous_solar_rad,
		previous_tavg,
		previous_tmax,
		previous_tmin,
		previous_vpd,
		previous_ts_f,
		previous_prcp,
		previous_swc,
		previous_ndvi_lai;
	
	assert(p_yos && yos_count);

	year = 0;
	current_year = -1;
	yos = *p_yos;

	for ( row = 0; row < rows_count; ++row ) {
		year = (int)values[VALUE_AT(row, YEAR)];
		if ( ! year ) {
			puts("year cannot be zero!\n");
			Log("year cannot be zero!\n");
			free(yos);
			return 0;
		}

		month = (int)values[VALUE_AT(row, MONTH)];
		if ( month < 1 || month > MONTHS ) {
			printf("bad month for year %d\n\n", year);
			Log("bad month for year %d\n\n", year);
			free(yos);
			return 0;

		}
		--month;

		day = (int)values[VALUE_AT(row, DAY)];
		if ( (day <= 0) || day > days_per_month[month] + (((1 == month) && IS_LEAP_YEAR(year)) ? 1 :0 ) ) {
			printf("bad day for %s %d\n\n", MonthName[month], year);
			Log("bad day for %s %d\n\n", MonthName[month], year);
			free(yos);
			return 0;
		}
		--day;

		if ( current_year != year ) {
			yos_no_leak = realloc(yos, (*yos_count+1)*sizeof*yos_no_leak);
			if ( ! yos_no_leak )
			{
				puts(sz_err_out_of_memory);
				free(yos);
				return 0;
			}
			yos = yos_no_leak;
			ResetYos(&yos[*yos_count]);

			yos[*yos_count].year = year;
			++*yos_count;
			
			current_year = year;
		}

		yos[*yos_count-1].m[month].d[day].n_days = day+1;
		if (yos[*yos_count-1].m[month].d[day].n_days > (int)settings->maxdays)
		{
			puts("ERROR IN N_DAYS DATA!!\n");
			Log("ERROR IN N_DAYS DATA!!\n");
			free(yos);
			return 0;
		}
		
		//switch ( i )
		//{								
		//case RG_F: //Rg_f - solar_rad -daily average solar radiation
		yos[*yos_count-1].m[month].d[day].solar_rad = values[VALUE_AT(row,RG_F)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad) && (!((day == 0) && (1 == *yos_count) && (month == 0))))
		{

			//the model gets the value of the day before
			//Log ("* SOLAR RAD -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
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

	//case TA_F: //Ta_f -  temperature average
		yos[*yos_count-1].m[month].d[day].tavg = values[VALUE_AT(row,TA_F)];
		if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tavg) && (!((day == 0) && (*yos_count == 1) && (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* TAVG -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);

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

	//case TMAX: //TMAX -  maximum temperature
		yos[*yos_count-1].m[month].d[day].tmax = values[VALUE_AT(row,TMAX)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* TMAX -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
			//Log("Getting previous day values.. !!\n");
			yos[*yos_count-1].m[month].d[day].tmax = yos[*yos_count-1].m[month].d[day].tavg;
			//Log("..using tavg = %f\n", yos[*yos_count-1].m[month].d[day].tavg);
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

	//case TMIN: //TMIN -  minimum temperature
		yos[*yos_count-1].m[month].d[day].tmin = values[VALUE_AT(row,TMIN)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmin) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* TMIN -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
			//Log("Getting previous day values.. !!\n");
			yos[*yos_count-1].m[month].d[day].tmin = yos[*yos_count-1].m[month].d[day].tavg;
			//Log("..using tavg = %f\n", yos[*yos_count-1].m[month].d[day].tavg);
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
		

	//case VPD_F: //RH_f - RH
		yos[*yos_count-1].m[month].d[day].vpd = values[VALUE_AT(row,VPD_F)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* VPD -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
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

	//case TS_F: // ts_f   Soil temperature
		yos[*yos_count-1].m[month].d[day].ts_f = values[VALUE_AT(row,TS_F)];
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
		
	//case PRECIP:  //Precip - rain
		yos[*yos_count-1].m[month].d[day].prcp = values[VALUE_AT(row,PRECIP)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].prcp) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* PRECIPITATION -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day+1);
			//Log("Getting previous day values.. !!\n");
			yos[*yos_count-1].m[month].d[day].prcp = previous_prcp;
			//Log("..value of the previous day = %f\n", yos[*yos_count-1].m[month].d[day].prcp);
			if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].prcp))
			{
				Log ("********* PRECIPITATION -NO DATA- in previous year!!!!\n" );

				//the model gets the value of the year before
				yos[*yos_count-1].m[month].d[day].prcp = yos[*yos_count-2].m[month].d[day].prcp;

				if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].prcp))
				{
					Log ("********* RAIN -NO DATA- in previous year!!!!\n" );
					yos[*yos_count-1].m[month].d[day].prcp = NO_DATA;
				}

			}
			//Log("precipitation of previous year = %f mm\n", yos[*yos_count-1].m[month].rain);
		}
		else
		{
			previous_prcp = yos[*yos_count-1].m[month].d[day].prcp;
		}

		//CONTROL
		if (yos[*yos_count-1].m[month].d[day].prcp > settings->maxprecip)
		{
			//Log("ERROR IN PRECIP DATA in year %d month %s!!!! %f\n", yos[*yos_count-1].year, MonthName[month], settings->maxprecip);
		}
		//Log("%d-%s-precip = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].rain);
	

	//case SWC: //Soil Water Content (%)

		yos[*yos_count-1].m[month].d[day].swc = values[VALUE_AT(row,SWC)];
		/*if ( error_flag )
		{
			printf("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
			Log("unable to convert value \"%s\" at column %d for %s day %d\n", token2, column+1, MonthName[month], day);
			free(yos);
			fclose(f);
			return 0;
		}*/
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
		//break;
	//case NDVI_LAI: //Get LAI in spatial version
		if (settings->spatial == 's')
		{
			yos[*yos_count-1].m[month].d[day].ndvi_lai = values[VALUE_AT(row,NDVI_LAI)];
			//if is not the first year the model get the previous year value
			if (*yos_count > 1)
			{

				//control in lai data if is an invalid value
				if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ndvi_lai) && (!((day == 0) && (month == 0))))
				{
					//the model gets the value of the day before
					//Log ("********* LAI -NO DATA in year %d month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day+1 );
					//Log("Getting previous years values !!\n");
					yos[*yos_count-1].m[month].d[day].ndvi_lai = previous_ndvi_lai;
					if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ndvi_lai))
					{
						//Log ("* LAI -NO DATA- in previous year!!!!\n" );
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
					//Log("********* INVALID DATA LAI > MAXLAI in year %d month %s!!!!\n", yos[*yos_count-1].year, MonthName[month] );
					//Log("Getting previous day values.. !!\n");
					yos[*yos_count-1].m[month].d[day].ndvi_lai = yos[*yos_count-2].m[month].d[day].ndvi_lai;
					//Log("..value of the previous day = %f\n", yos[*yos_count-1].m[month].d[day].ndvi_lai);
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
					//Log("**********First Year without a valid LAI value set to default value LAI\n");
					yos[*yos_count-1].m[month].d[day].ndvi_lai = settings->defaultlai;
					//Log("**DEFAULT LAI VALUE SET TO %d\n", settings->defaultlai);
				}
			}
		}
	
	//case ET: //ET for stand alone RothC
		yos[*yos_count-1].m[month].d[day].et = values[VALUE_AT(row,ET)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].et) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* ET -NO DATA in year %s month %s, day %d!!!!\n", year, MonthName[month], day);
		}
		//Log("%d-%s-tavg = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
		
	//case WS_F: //windspeed
		yos[*yos_count-1].m[month].d[day].windspeed = values[VALUE_AT(row,WS_F)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].windspeed) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			//the model gets the value of the day before
			//Log ("* windspeed -NO DATA in year %s month %s, day %d!!!!\n", yos[*yos_count-1].year, MonthName[month], day);
		}
		//Log("%d-%s-tavg = %f\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);

		// RH_f
		yos[*yos_count-1].m[month].d[day].rh_f = values[VALUE_AT(row,RH_F)];
		if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].rh_f) && (!((day == 0) && (*yos_count == 1)&& (month == 0))))
		{
			// ALESSIOR
			// TODO: ask ALESSIOC
		}
	}
	*p_yos = yos;
	return 1;
#undef VALUE_AT
}

//
int ImportNCFile(const char *const filename, YOS **pyos, int *const yos_count) {
/*
#define COLUMN_AT(x,y,c)	(((c)*dims_size[ROWS_DIM])+((x)*dims_size[ROWS_DIM]*(MET_COLUMNS+2))+((y)*dims_size[ROWS_DIM]*(MET_COLUMNS+2)*dims_size[X_DIM]))
#define VALUE_AT(x,y,r,c)	((r)+((c)*dims_size[ROWS_DIM])+((x)*dims_size[ROWS_DIM]*(MET_COLUMNS+2))+((y)*dims_size[ROWS_DIM]*(MET_COLUMNS+2)*dims_size[X_DIM]))
*/
#define COLUMN_AT(c)	((c)*dims_size[ROWS_DIM])
#define VALUE_AT(r,c)	((r)+(COLUMN_AT((c))))
	int i;
	int y;
	int z;
	int id_file;
	int ret;

	int dims_count;	/* dimensions */
	int vars_count;
	int atts_count;	/* attributes */
	int unl_count;	/* unlimited dimensions */
	char name[NC_MAX_NAME+1];
	nc_type type;
	size_t size;
	int *i_values = NULL; /* required */
	double *values = NULL; /* required */
	int columns[MET_COLUMNS];

	/* DO NOT CHANGE THIS ORDER */
	enum {
		ROWS_DIM,
		X_DIM,
		Y_DIM,

		DIMS_COUNT
	};

	int dims_size[DIMS_COUNT];
	const char *sz_dims[DIMS_COUNT] = { "row", "x", "y" }; /* DO NOT CHANGE THIS ORDER...please see top */

	/* */
	ret = nc_open(filename, NC_NOWRITE, &id_file);
    if ( ret != NC_NOERR ) goto quit;

	ret = nc_inq(id_file, &dims_count, &vars_count, &atts_count, &unl_count);
	if ( ret != NC_NOERR ) goto quit;

	if ( ! dims_count || ! vars_count ) {
		printf("bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		nc_close(id_file);
		return 0;
	}

	/* check if vars count are at least MET_COLUMNS+2 */
	if ( vars_count < MET_COLUMNS+2 ) {
		printf("bad nc file! Vars count should be %d at least.\n\n", vars_count);
		nc_close(id_file);
		return 0;
	}

	/* reset */
	for ( i = 0; i < MET_COLUMNS; ++i ) {
		columns[i] = 0;
	}
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		dims_size[i] = -1;
	}

	/* get dimensions */
	for ( i = 0; i < dims_count; ++i ) {
		ret = nc_inq_dim(id_file, i, name, &size);
		if ( ret != NC_NOERR ) goto quit;
		for ( y = 0; y < DIMS_COUNT; ++y ) {
			if ( ! mystricmp(sz_dims[y], name) ) {
				if ( dims_size[y] != -1 ) {
					printf("dimension %s already found!\n", sz_dims[y]);
					nc_close(id_file);
					return 0;
				}
				dims_size[y] = size;
				break;
			}
		}
	}

	/* check if we have all dimensions */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		if ( -1 == dims_size[i] ) {
			printf("dimension %s not found!\n", sz_dims[i]);
			nc_close(id_file);
			return 0;
		}
	}

	/* check x and y */
	if( (dims_size[X_DIM] != 1) || (dims_size[Y_DIM] != 1) ) {
		puts("x and y must be 1!");
		nc_close(id_file);
		return 0;
	}

	/* alloc memory for int values */
	i_values = malloc(dims_size[ROWS_DIM]*sizeof*values);
	if ( ! i_values ) {
		puts(sz_err_out_of_memory);
		nc_close(id_file);
		return 0;
	}

	/* alloc memory for double values */
	/* please note that we alloc double for year,month,day too and we add 2 columns for lat and lon ;) */
	/*values = malloc(dims_size[ROWS_DIM]*dims_size[X_DIM]*dims_size[Y_DIM]*(MET_COLUMNS+2)*sizeof*values);*/
	values = malloc(dims_size[ROWS_DIM]*MET_COLUMNS*sizeof*values);
	if ( ! values ) {
		puts(sz_err_out_of_memory);
		free(i_values);
		nc_close(id_file);
		return 0;
	}

	/* set all double values to -9999 */
	//for ( i = 0; i < dims_size[ROWS_DIM]*dims_size[X_DIM]*dims_size[Y_DIM]*(MET_COLUMNS+2); ++i ) {
	for ( i = 0; i < dims_size[ROWS_DIM]*MET_COLUMNS; ++i ) {
		values[i] = INVALID_VALUE;
	}

	/* get vars */
	for ( i = 0; i < vars_count; ++i ) {
		ret = nc_inq_var(id_file, i, name, &type, NULL, NULL, NULL);
		if ( ret != NC_NOERR ) goto quit;
		/* check if we need that var */
		for ( y = 0; y  < MET_COLUMNS /*+2*/; ++y ) {
			if ( ! mystricmp(name, met_columns[y]) ) {
				/* check if we've already get that var */
				if ( columns[y] ) {
					Log("column %s already imported!", name);
					free(values);
					free(i_values);
					nc_close(id_file);
					return 0;
				} else {
					columns[y] = 1;
				}
				/* get values */
				if ( NC_DOUBLE == type ) {
					//ret = nc_get_var_double(id_file, i, &values[COLUMN_AT(0, 0, y)]);
					ret = nc_get_var_double(id_file, i, &values[COLUMN_AT(y)]);
					if ( ret != NC_NOERR ) goto quit;
				} else if ( NC_INT == type ) {
					ret = nc_get_var_int(id_file, i, i_values);
					if ( ret != NC_NOERR ) goto quit;
					for ( z = 0; z < dims_size[ROWS_DIM]; ++z ) {
						values[VALUE_AT(z, y)] = (double)i_values[z]; 
					}
				} else {
					/* type format not supported! */
					Log("type format for %s column not supported", name);
					free(values);
					free(i_values);
					nc_close(id_file);
					return 0;
				}
				
				break;
			}
		}
	}
	free(i_values);
	i_values = NULL;

	/* check if we've all needed vars */
	for ( i = 0; i < MET_COLUMNS; ++i ) {
		if ( ((VPD_F == i) && (-1 == columns[RH_F])) || ((RH_F == i) && (-1 == columns[VPD_F])) ) {
			Log("met columns %s and %s are missing!\n\n", met_columns[VPD_F], met_columns[RH_F]);
			free(values);
			free(i_values);
			nc_close(id_file);
			return 0;
		} else if ( -1 == columns[i] ) {
			Log("met column %s not found.\n\n", met_columns[i]);
			free(values);
			free(i_values);
			nc_close(id_file);
			return 0;
		}
	}

	/* compute vpd ?*/
	if ( -1 == columns[VPD_F] ) {
		compute_vpd(values, dims_size[ROWS_DIM], MET_COLUMNS);
	}

	/* attributes */
	/*
	atts = malloc(atts_count*sizeof*atts);
	if ( ! atts ) {
		puts("out of memory!");
		nc_close(id_file);
		return 0;
	}
	for ( i = 0; i < atts_count; ++i ) {
		 ret = nc_inq_att(id_file, atts[i].name, &atts[i].type, &atts[i].size);
		 if ( ret != NC_NOERR ) goto quit;
	}
	*/

	/* save file */
#if 0
	{
		FILE *f;

		f = fopen(NC_CONV_FILENAME, "w");
		if ( ! f ) {
			puts("unable to create output file!");
			nc_close(id_file);
			free(values);
			free(i_values);
			return 0;
		}
		/* write header */
		fputs("Year\tMonth\tn_days\tRg_f\tTa_f\tTmax\tTmin\tVPD_f\tTs_f\tPrecip\tSWC\tLAI\tET\WS_F\n", f);
		for ( row = 0; row < dims_size[ROWS_DIM]; ++row ) {
				fprintf(f, "%d\t%d\t%d\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n",
						/*
						(int)values[VALUE_AT(0,0,row,YEAR)]
						, (int)values[VALUE_AT(0,0,row,MONTH)]
						, (int)values[VALUE_AT(0,0,row,DAY)]
						, values[VALUE_AT(0,0,row,RG_F)]
						, values[VALUE_AT(0,0,row,TA_F)]
						, values[VALUE_AT(0,0,row,TMAX)]
						, values[VALUE_AT(0,0,row,TMIN)]
						, values[VALUE_AT(0,0,row,VPD_F)]
						, values[VALUE_AT(0,0,row,TS_F)]
						, values[VALUE_AT(0,0,row,PRECIP)]
						, values[VALUE_AT(0,0,row,NDVI_LAI)]
						, values[VALUE_AT(0,0,row,ET)]
						, values[VALUE_AT(0,0,row,WS)]
						*/
						(int)values[VALUE_AT(row,YEAR)]
						, (int)values[VALUE_AT(row,MONTH)]
						, (int)values[VALUE_AT(row,DAY)]
						, values[VALUE_AT(row,RG_F)]
						, values[VALUE_AT(row,TA_F)]
						, values[VALUE_AT(row,TMAX)]
						, values[VALUE_AT(row,TMIN)]
						, values[VALUE_AT(row,VPD_F)]
						, values[VALUE_AT(row,TS_F)]
						, values[VALUE_AT(row,PRECIP)]
						, values[VALUE_AT(row,SWC)]
						, values[VALUE_AT(row,NDVI_LAI)]
						, values[VALUE_AT(row,ET)]
						//ALESSIOC
						, values[VALUE_AT(row,WS_F)]

				);
		}
		fclose(f);
	}
#endif
	if ( ! yos_from_arr(values, dims_size[ROWS_DIM], MET_COLUMNS, pyos, yos_count) ) {
		free(values);
		return 0;
	}
	/* hack */
	ret = 0;

quit:
	nc_close(id_file);
	free(values);
	free(i_values);
	if ( ret ) {
		puts(nc_strerror(ret));
		ret = 1;
	}
	return ! ret;

#undef COLUMN_AT
#undef VALUE_AT
}

/* private */
size_t file_load_in_memory(const char *const filename, char **result) {
	size_t size;
	FILE *f;

	size = 0;
	f = fopen(filename, "rb");
	if ( !f )  {
		*result = NULL;
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (char *)malloc((size+1)*sizeof**result);
	if ( !*result ) {
		fclose(f);
		return 0;
	}
	if ( size != fread(*result, sizeof(char), size, f) )  {
		free(*result);
		fclose(f);
		return 0;
	}
	fclose(f);
	(*result)[size] = '\0';

	return size;
}


/* private */
static int file_get_rows_count(const char *const filename) {
	size_t i;
	size_t size;
	int rows_count;
	char *p;

	if ( !filename || ('\0' == filename[0]) ) {
		return -1;
	}

	size = file_load_in_memory(filename, &p);
	if ( ! size ) return -1;

	//
	rows_count = 0;
	for ( i = 0; i < size; i++ ) {
		if ( '\n' == p[i] ) {
			++rows_count;
		}
	}
	if ( p[size-1] != '\n' ) {
		++rows_count;
	}
	free(p);
	return rows_count;
}

/* private */
static void timestamp_split(const double value, int *const YYYY, int *const MM, int *const DD) {
	*YYYY = (int)value/10000;
	*MM = ((int)value - *YYYY*10000)/100;
	*DD = (int)value - (*YYYY*10000) - (*MM*100);
}

/* private */
static int ImportListFile(const char *const filename, YOS **p_yos, int *const yos_count) {
#define VARS_COUNT		((MET_COLUMNS)-3)	/* we remove first 3 columns: year, month and day */

#define COLUMN_AT(c)	((c)*rows_count)
#define VALUE_AT(r,c)	((r)+(COLUMN_AT(c)))

	/* TODO: fix buffers length...it can't have same length */
	char buffer[256];
	char sz_path[256];
	char sz_nc_filename[256];
	char *p;
	char *p2;
	int i;
	int z;
	FILE *f;
	int rows_count;
	int vars[VARS_COUNT] = { 0 }; /* required */
	float *f_values;
	double *values;
		enum {
		X_DIM,
		Y_DIM,
		TIME_DIM,

		DIMS_COUNT
	};

	int dims_size[DIMS_COUNT];
	const char *sz_lat = "lat";
	const char *sz_lon = "lon";
	const char *sz_time = "time";
	const char *sz_dims[DIMS_COUNT] = { "x", "y", "time" }; /* DO NOT CHANGE THIS ORDER...please see top */
	const char *sz_vars[VARS_COUNT] = { "RADS"
										, "T_2M"
										, "TMAX_2M"
										, "TMIN_2M"
										, "VPD"
										, "TSOIL"
										, "TOT_PREC"
										, "SWC"
										, "LAI"
										, "ET"
										, "WS_F"
										, "RH"
	};

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
	int flag;

	/* init */
	rows_count = 0;
	values = NULL;
	f_values = NULL;

	/* open lst file */
	f = fopen(filename, "r");
	if ( ! f ) {
		Log("unable to open met data file !\n");
		return 0;
	}

	/* get filename path */
	sz_path[0] = '\0';
	if ( filename[1] != ':' ) {
		strncpy(sz_path, program_path, 256);
	}
	/* check for buffer overflow */
	strcat(sz_path, filename);
	p = (strrchr(sz_path, '\\'));
	if ( p ) ++p;
	p2 = (strrchr(sz_path, '/'));
	if ( p2 ) ++p2;
	if  ( p2 > p ) p = p2;
	*p = '\0';

	/* parse lst file */
	while ( fgets(buffer, 256, f) ) {
		/* remove \r\n and skip blank line */
		for ( i = 0; buffer[i]; ++i ) {
			if ( '\r' == buffer[i] || '\n' == buffer[i] ) {
				buffer[i] = '\0';
				break;
			}
		}
		if ( ! buffer[0] ) {
			continue;
		}

		/* check for buffer overflow */
		sprintf(sz_nc_filename, "%s%s", sz_path, buffer);

		/* try to open nc file */
		ret = nc_open(sz_nc_filename, NC_NOWRITE, &id_file);
		if ( ret != NC_NOERR ) goto quit;

		ret = nc_inq(id_file, &dims_count, &vars_count, &atts_count, &unl_count);
		if ( ret != NC_NOERR ) goto quit;

		if ( ! dims_count || ! vars_count ) {
			printf("bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
			nc_close(id_file);
			fclose(f);
			free(f_values);
			free(values);
			return 0;
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
				if ( ! mystricmp(sz_dims[y], name) ) {
					if ( dims_size[y] != -1 ) {
						printf("dimension %s already found!\n", sz_dims[y]);
						nc_close(id_file);
						fclose(f);
						free(f_values);
						free(values);
						return 0;
					}
					dims_size[y] = size;
					break;
				}
			}
		}

		/* check if we have all dimensions */
		for ( i = 0; i < DIMS_COUNT; ++i ) {
			if ( -1 == dims_size[i] ) {
				printf("dimension %s not found!\n", sz_dims[i]);
				nc_close(id_file);
				fclose(f);
				free(f_values);
				free(values);
				return 0;
			}
		}

		/* check rows_count */
		if ( ! rows_count ) {
			rows_count = dims_size[TIME_DIM];
			/* alloc memory for double values */
			values = malloc(rows_count*MET_COLUMNS*sizeof*values);
			if ( ! values ) {
				puts(sz_err_out_of_memory);
				nc_close(id_file);
				fclose(f);
				return 0;
			}
			/* set all double values to -9999 */
			for ( i = 0; i < rows_count*MET_COLUMNS; ++i ) {
				values[i] = INVALID_VALUE;
			}
			/* alloc memory for float values */
			f_values = malloc(rows_count*sizeof*f_values);
			if ( ! f_values ) {
				puts(sz_err_out_of_memory);
				nc_close(id_file);
				fclose(f);
				free(values);
				return 0;
			}
			/* set all float values to -9999..not really needed */
			for ( i = 0; i < rows_count; ++i ) {
				f_values[i] = INVALID_VALUE;
			}
		} else {
			if ( rows_count != dims_size[TIME_DIM] ) {
				printf("rows count inside %s should be %d not %d\n\n", buffer, rows_count, dims_size[TIME_DIM]);
				nc_close(id_file);
				fclose(f);
				free(f_values);
				free(values);
				return 0;
			}
		}

		/* check x and y */
		if( (dims_size[X_DIM] != 1) || (dims_size[Y_DIM] != 1) ) {
			printf("x and y inside %s, must be 1!", buffer);
			nc_close(id_file);
			fclose(f);
			free(f_values);
			free(values);
			return 0;
		}

		/* get var */
		flag = 0;
		for ( i = 0; i < vars_count; ++i ) {
			ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
			if ( ret != NC_NOERR ) goto quit;
			/* check for lat */
			if ( ! mystricmp(name, sz_lat) ) {
				/* we get only one size */
				if ( (n_dims < 2) || (ids[0] != 1) ) {
					printf("sorry, only one lat is supported in %s\n\n", buffer);
					nc_close(id_file);
					fclose(f);
					free(values);
					free(f_values);
					return 0;
				}
			} else if ( ! mystricmp(name, sz_lon) ) {
				/* we get only one size */
				if ( (n_dims < 2) || (ids[0] != 1) ) {
					printf("sorry, only one lon is supported in %s\n\n", buffer);
					nc_close(id_file);
					fclose(f);
					free(values);
					free(f_values);
					return 0;
				}
			} else if ( ! mystricmp(name, sz_time) ) {
				if ( type != NC_DOUBLE ) {
					printf("type format in %s for time column not supported\n\n", buffer);
					nc_close(id_file);
					fclose(f);
					free(values);
					free(f_values);
					return 0;
				}
				ret = nc_get_var_double(id_file, i, &values[COLUMN_AT(YEAR)]);
				if ( ret != NC_NOERR ) goto quit;

				/* adjust time to YYYY,MM,DD */
				for ( z = 0; z < rows_count; ++z ) {
					int YYYY;
					int MM;
					int DD;
					timestamp_split(values[VALUE_AT(z, YEAR)], &YYYY, &MM, &DD);
					values[VALUE_AT(z, YEAR)] = YYYY;
					values[VALUE_AT(z, MONTH)] = MM;
					values[VALUE_AT(z, DAY)] = DD;
				}
			} else {
				for ( y = 0; y  < VARS_COUNT; ++y ) {
					if ( ! mystricmp(name, sz_vars[y]) ) {
						/* check if we already have imported that var */
						if ( vars[y] ) {
							Log("var %s already imported\n", sz_vars[y]);
							nc_close(id_file);
							fclose(f);
							free(values);
							free(f_values);
							return 0;
						}

						/* get values */
						if ( NC_FLOAT == type ) {
							ret = nc_get_var_float(id_file, i, f_values);
							if ( ret != NC_NOERR ) goto quit;
							for ( z = 0; z < rows_count; ++z ) {
								values[VALUE_AT(z, y + 3)] = f_values[z];
							}
						} else if ( NC_DOUBLE == type ) {
							ret = nc_get_var_double(id_file, i, &values[COLUMN_AT(y + 3)]);
							if ( ret != NC_NOERR ) goto quit;
						} else {
							/* type format not supported! */
							Log("type format in %s for %s column not supported\n\n", buffer, sz_vars[y]);
							nc_close(id_file);
							fclose(f);
							free(values);
							free(f_values);
							return 0;
						}
						vars[y] = 1;
						flag = 1;
						break;
					}
				}
			}
		}
		if ( ! flag ) {
			printf("var not found inside %s\n\n", buffer);
			nc_close(id_file);
			fclose(f);
			free(values);
			free(f_values);
			return 0;
		}
		nc_close(id_file);
	}

	/* check for missing vars */
	for ( i = 0; i < VARS_COUNT; ++i ) {
		if ( ((VPD_F-3 == i) && (-1 == vars[RH_F-3])) || ((RH_F-3 == i) && (-1 == vars[VPD_F-3])) ) {
			Log("met columns %s and %s are missing!\n\n", sz_vars[VPD_F-3], sz_vars[RH_F-3]);
			fclose(f);
			free(values);
			free(f_values);
			return 0;
		} else if ( -1 == vars[i] ) {
			Log("var %s not found\n", sz_vars[i]);
			fclose(f);
			free(values);
			free(f_values);
			return 0;
		}
	}

	/* close file */
	fclose(f);

	/* free memory */
	free(f_values);

	/* compute vpd ?*/
	if ( -1 == vars[VPD_F-3] ) {
		compute_vpd(values, rows_count, MET_COLUMNS);
	}
	
	i = yos_from_arr(values, rows_count, MET_COLUMNS, p_yos, yos_count);
	free(values);
	return i;

quit:
	puts(nc_strerror(ret));
	nc_close(id_file);
	free(f_values);
	free(values);
	return 0;

#undef VALUE_AT
#undef COLUMN_AT
#undef COLUMNS_COUNT
}

//
int ImportStandardFile(const char *const filename, YOS **p_yos, int *const yos_count) {
#define VALUE_AT(r,c)	((r)+((c)*rows_count))
	int i = 0,
		column = 0,
		month = 0,
		day = 0,
		year = 0,
		error_flag = 0,
		columns[MET_COLUMNS];

	int no_year_column;
	int current_year = 0;

	int current_row;

	char *token = NULL,
	*token2 = NULL,
	*p,
	buffer[BUFFER_SIZE] = { 0 };
	int rows_count;
	double *values;

	FILE *f;

	assert(p_yos);

	// get rows count
	rows_count = file_get_rows_count(filename);
	if ( rows_count <= 0 ) {
		Log("unable to open met data file !\n");
		return 0;
	}

	// remove header
	--rows_count;

	// alloc memory for values
	values = malloc(rows_count*MET_COLUMNS*sizeof*values);
	if ( ! values ) {
		Log(sz_err_out_of_memory);
		return 0;
	}

	// clear values with INVALID_VALUES
	for ( i = 0; i < rows_count*MET_COLUMNS; ++i ) {
		values[i] = INVALID_VALUE;
	}

	// open file
	f = fopen(filename, "r");
	if ( !f )
	{
		Log("unable to open met data file !\n");
		free(values);
		return 0;
	}

	// get header
	if ( ! fgets(buffer, BUFFER_SIZE, f) )
	{
		Log ("empty met data file ?\n");
		free(values);
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
					free(values);
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
			else if ( ((VPD_F == i) && (-1 != columns[RH_F])) || ((RH_F == i) && (-1 != columns[VPD_F])) ) {
				continue;
			}
			printf("met column %s not found.\n\n", met_columns[i]);
			free(values);
			fclose(f);
			return 0;
		}
	}
	
	if ( (! no_year_column && (YEAR != columns[YEAR])) || (MONTH-no_year_column != columns[MONTH]) || (DAY-no_year_column != columns[DAY]) )
	{
		puts("date must be on first columns!\n\n");
		free(values);
		fclose(f);
		return 0;
	}

	// parse rows
	current_row = 0;
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
		if ( ++current_row > rows_count ) {
			puts("too many rows found!");
			Log("too many rows found!");
			free(values);
			fclose(f);
			return 0;
		}

		// parse string
		for ( column = no_year_column, token2 = mystrtok(buffer, met_delimiter, &p); token2; token2 = mystrtok(NULL, met_delimiter, &p), ++column )
		{
			if ( column >= MET_COLUMNS ) {
				printf("too many columns at row %d\n", current_row+1);
				free(values);
				fclose(f);
				return 0;
			}

			for ( i = 0; i < MET_COLUMNS; ++i ) {
				if ( column == columns[i] ) break;
			}

			values[VALUE_AT(current_row-1, i)] = convert_string_to_prec(token2, &error_flag);
			if ( error_flag )
			{
				printf("unable to convert value \"%s\" for %s column\n", token2, met_columns[i+no_year_column]);
				Log("unable to convert value \"%s\" for %s column\n", token2, met_columns[i+no_year_column]);
				free(values);
				fclose(f);
				return 0;
			}
		}
	}

	Log ("ok met data\n");

	fclose(f);

	if ( rows_count != current_row ) {
		printf("rows count should be %d not %d\n", rows_count, current_row);
		Log("rows count should be %d not %d\n", rows_count, current_row);
		free(values);
		return 0;
	}

	/* fix year */
	if ( no_year_column ) {
		for ( i = 0; i < rows_count; ++i ) {
			values[VALUE_AT(i, YEAR)] = year;
		}
	}

	/* compute vpd ?*/
	if ( -1 == columns[VPD_F] ) {
		compute_vpd(values, rows_count, MET_COLUMNS);
	}

	if ( ! yos_from_arr(values, rows_count, MET_COLUMNS, p_yos, yos_count) ) {
		free(values);
		return 0;
	}

	free(values);

	/* ok */
	return 1;
#undef VALUE_AT
}


/*import met data file*/
//------------------------------------------------------------------------------
// *file is the comma separated files list!!! not a single file
// initially yos_count is equal to 0
YOS *ImportYosFiles(char *file, int *const yos_count, const int x, const int y)
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
		if ( i )
		{
			i = ImportNCFile(token, &yos, yos_count);
		}
		else if ( ! mystricmp(p2, "lst") )
		{
			i = ImportListFile(token, &yos, yos_count);
		}
		else /* txt */
		{
			i = ImportStandardFile(token, &yos, yos_count);
		}
		if ( ! i )
		{
			free(yos);
			return NULL;
		}	
	}

	// sort
	if ( *yos_count > 1 )
	{
		qsort(yos, *yos_count, sizeof*yos, sort_by_years);
	}

	Log ("ok met data importing\n");

#if 0
	/* save imported yos for debugging purposes */
	{
		char buffer[64];
		int i;
		int month;
		int z;
		FILE *f;

		for ( i = 0; i < *yos_count; ++i ) {
			sprintf(buffer, "debug_met_daily_%d.csv", yos[i].year);
			f = fopen(buffer, "w");
			if ( ! f ) {
				printf("unable to create %s\n", buffer);
				free(yos);
				return NULL;
			}
			fputs("year,month,day,n_days,solar_rad,tavg,tmax,tmin,tday,tnight,vpd,ts_f"
					",rain,swc,ndvi_lai,daylength,thermic_sum"
					",rho_air,tsoil,et,windspeed\n", f);

			for ( month = 0; month < 12; ++month ) {
				for ( z = 0; z < 31; ++z ) {
					if ( z == yos[i].m[month].d[z].n_days )
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
							//ALESSIOC
							, yos[i].m[month].d[z].windspeed);
				}
			}		
			fclose(f);
		}
	}
#endif

	return yos;
}

/* 
	please note that row start from 0
*/
static int get_daily_date_from_row(const int row, const int yyyy) {
	int dd;
	int mm;
	int days;
	int days_per_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if ( IS_LEAP_YEAR(yyyy) ) {
		++days_per_month[1];
	}

	assert(row>=0 && row<365+IS_LEAP_YEAR(yyyy));

	dd = 0;
	days = 0;
	for ( mm = 0; mm < 12; ++mm ) {
		days += days_per_month[mm];
		if ( row < days ) {
			dd = row-(days-days_per_month[mm])+1;
			break;
		}
	}
	++mm;
	
	return yyyy*10000+mm*100+dd;
}
/* 
	please note that row start from 0
*/
static int get_monthly_date_from_row(const int row, const int yyyy) {
	assert(row>=0 && row<12);
	return yyyy*100+(row+1);
}

//
// if type is 0, write daily
// if type is 1, write monthly
// if type is 2, write yearly
//
int WriteNetCDFOutput(const OUTPUT_VARS *const vars, const int year_start, const int years_count, const int x_cells_count, const int y_cells_count, const int type) {
	int i;
	int ret;
	char *p;
	char sz_buffer[256];
	int n;

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
	if ( 1 == type ) time_rows = malloc(years_count*12*sizeof*time_rows);
	if ( 2 == type ) time_rows = malloc(years_count*sizeof*time_rows);
	if ( ! time_rows ) {
		Log(sz_err_out_of_memory);
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
			sprintf(sz_buffer, "%s.nc", sz_output_vars[vars->daily_vars[i]]);

		if ( 1 == type )
			sprintf(sz_buffer, "%s.nc", sz_output_vars[vars->monthly_vars[i]]);

		if ( 2 == type )
			sprintf(sz_buffer, "%s.nc", sz_output_vars[vars->yearly_vars[i]]);

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
		id_dims[1] = id_x;
		id_dims[2] = id_y;
				
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
		if ( 0 == type)
			values = vars->daily_vars_value;
		if ( 1 == type)
			values = vars->monthly_vars_value;
		if ( 2 == type)
			values = vars->yearly_vars_value;
		
		ret = nc_put_var_double(id_file, id_var, values);
		if ( ret != NC_NOERR ) goto quit;

				
		nc_close(id_file);		
	}

	return 1;

quit:
	Log("unable to create output netcdf file %s: %s", sz_buffer, nc_strerror(ret));
	free(time_rows);
	nc_close(id_file);

	return 0;
}
