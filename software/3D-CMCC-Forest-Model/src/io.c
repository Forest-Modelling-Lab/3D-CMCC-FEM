

/* includes */
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

static const char comma_delimiter[] = ",\r\n";
static const char met_delimiter[] = " ,\t\r\n";


/* */
enum {	MONTH = 0,
	N_DAYS,
	RG_F,
	TA_F,
	TMAX,
	TMIN,
	VPD_F,
	TS_F,
	PRECIP,
	SWC,
	Ndvi_Lai,


	MET_COLUMNS };

static const char *met_columns[MET_COLUMNS] = {
		"Month",
		"n_days",
		"Rg_f",
		"Ta_f",
		"Tmax",
		"Tmin",
		"VPD_f",
		"Ts_f",
		"Precip",
		"SWC",
		"LAI"
};



int giorninelmese [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *MonthName[MONTHS] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
		"AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };


/*import met data file*/
//------------------------------------------------------------------------------
// *file is the comma separated files list!!! not a single file
// initially yos_count is equal to 0
YOS *ImportYosFiles(char *file, int *const yos_count)
{
	int i = 0,
		column = 0,
		month = 0,
		day = 0,
		error_flag = 0,
		error,
		columns[MET_COLUMNS];

	char year[5],
	*filename,
	*token = NULL,
	*token2 = NULL,
	*p,
	*p2,
	buffer[BUFFER_SIZE] = { 0 };

	FILE *f = NULL;

	YOS *yos = NULL, // mandatory
			*yos_no_leak;

	Log("Processing met data files...\n");


	if (!file)
	{
		return NULL;
		Log("NULL Met file!!\n");
	}


	// reset
	*yos_count = 0;

	//
	for (token = mystrtok(file, comma_delimiter, &p); token; token = mystrtok(NULL, comma_delimiter, &p) )
	{
		// get token length
		i = strlen(token);

		// if length is 0 skip to next token
		if ( !i ) continue;

		/*
		// validate name
		if ( !is_valid_met(token) )
		{
		Log("Invalid met file specified: %s\n", token);
		free(yos);
		return NULL;
		}
		*/

		// alloc memory for yos
		yos_no_leak = realloc(yos, (++*yos_count)*sizeof*yos_no_leak);
		//italo yos_no_leak = realloc(yos, (++*yos_count)*sizeof(YOS));
		if ( !yos_no_leak )
		{
			Log("yos_no_leak out of memory.\n");
			free(yos);
			return NULL;
		}

		// assign memory
		yos = yos_no_leak;


		filename = malloc(sizeof(*filename)*BUFFER_SIZE);

		if( !filename )
		{
			fprintf(stderr, "Cannot allocate memory for %s\n", file);
			return NULL;
		}
		bzero(filename, BUFFER_SIZE-1);
		// Get only filename.txt from /this/is/the/complete/path/of/filename.txt
		filename = (strrchr(token, '/'))+1;
		// Get year from filename: file name is like 123_4567_2007.txt where
		// 123 is the x coordinate of the cell,
		// 4567 is the y coordinate of the cell,
		// 2007 is the year

		Log("..opening met file '%s' \n", filename);

		char *pch,
		*tmp_filename;

		tmp_filename = (char*)malloc(sizeof(char)*BUFFER_SIZE);
		if( !tmp_filename )
		{
			fprintf(stderr, "Cannot allocate memory for tmp_filename.\n");
			exit(1);
		}

		// Copy of filename because strtok cut it
		strcpy(tmp_filename, filename);

		pch = strtok (tmp_filename, "_");
		pch = strtok (NULL, "_");
		pch = strtok (NULL, "_.");

		strcpy(year, pch);
		year[4] = '\0';

		yos[*yos_count-1].year = convert_string_to_int(year, &error);
		if ( error )
		{
			Log("error during conversion of %s\n", year);
			free(yos);
			return NULL;
		}
		// open file
		f = fopen(token, "r");
		if ( !f )
		{
			printf("unable to open met data file !\n");
			free(yos);
			return NULL;
		}

		// get header
		if ( !fgets(buffer, BUFFER_SIZE, f) )
		{
			printf("empty met data file ?\n");
			free(yos);
			fclose(f);
			return NULL;
		}

		// reset columns
		for ( i=0; i<MET_COLUMNS; i++ )
			columns[i] = -1;

		// parse header
		for ( column = 0, token2 = mystrtok(buffer, met_delimiter, &p2); token2; token2 = mystrtok(NULL, met_delimiter, &p2), column++ )
		{
			for ( i = 0; i < MET_COLUMNS; i++ )
			{
				if ( !mystricmp(token2, met_columns[i]) )
				{
					if  ( -1 != columns[i] )
					{
						printf("met column %s already assigned.\n\n", token2);
						free(yos);
						fclose(f);
						return NULL;
					}
					columns[i] = column;
					break;
				}
			}
		}

		// check if each columns was assigned
		for ( i = 0; i < MET_COLUMNS; i++ )
		{
			if ( -1 == columns[i] )
			{
				printf("met column %s not found.\n\n", met_columns[i]);
				free(yos);
				fclose(f);
				return NULL;
			}
		}

		// reset
		month = 0;

		//
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
			if ( '\0' == buffer[0] ) continue;

			for ( column = 0, token2 = mystrtok(buffer, met_delimiter, &p2); token2; token2 = mystrtok(NULL, met_delimiter, &p2), column++ )
			{
				//Log("day = %d\n", day);
				//Log("month = %d\n", month);
				//Log("MONTH = %d\n", MONTHS);
				//Log("column = %d\n", column);
				//Log("yos = %d\n", yos);
				//Log("MET_COLUMNS = %d\n", MET_COLUMNS);


				if (settings->time == 'm')
				{
					if ( month == MONTHS  )
					{
						printf("bad monthly format data for monthly met file, too many rows for a year.\n\n");
						free(yos);
						fclose(f);
						return NULL;
					}
				}
				else
				{
					if (day > 365)
					{
						printf("bad monthly format data for daily met file, too many rows for a year.\n\n");
						free(yos);
						fclose(f);
						return NULL;
					}
				}
				for ( i = 0; i < MET_COLUMNS; i++ )
				{
					if ( column == columns[i] )
					{
						//set value for monthly version
						if (settings->time == 'm')
						{

							//printf("opening met file year %d \n", year);
							switch ( i )
							{
							//unused in monthly version just used in daily version
							case N_DAYS:
								yos[*yos_count-1].m[month].n_days = convert_string_to_int(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								//CONTROL
								if (yos[*yos_count-1].m[month].n_days > (int)settings->maxdays)
								{
									Log("ERROR IN N_DAYS DATA!!\n");
								}
								break;

							case RG_F: //Rg_f - solar_rad -daily average solar radiation
								yos[*yos_count-1].m[month].solar_rad = convert_string_to_prec(token2, &error_flag);
								//convert KJ to MJ
								//yos[*yos_count-1].m[month].solar_rad = yos[*yos_count-1].m[month].solar_rad / 1000;
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].solar_rad) && *yos_count > 1)
								{
									Log ("* SOLAR RAD -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].solar_rad = yos[*yos_count-2].m[month].solar_rad;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].solar_rad))
									{
										Log ("********* SOLAR RAD -NO DATA- in previous year!!!!\n" );
									}
								}
								//CONTROL
								if (yos[*yos_count-1].m[month].solar_rad > settings->maxrg )
								{
									Log("ERROR IN RG DATA in year %s month %s!!!!\n", year, MonthName[month] );
								}
								//convert daily average solar radiation to monthly solar radiation
								//m[month].solar_rad *= m[month].n_days;
								break;

							case TA_F: //Ta_f -  temperature average
								yos[*yos_count-1].m[month].tavg = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tavg) && *yos_count > 1)
								{
									//Log ("* T_AVG -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tavg = yos[*yos_count-2].m[month].tavg;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].tavg))
									{
										//Log ("********* T_AVG -NO DATA- in previous year!!!!\n" );
									}
								}
								//CONTROL
								if (yos[*yos_count-1].m[month].tavg > settings->maxtavg)
								{
									Log("ERROR IN TAV DATA in year %s month %s!!!!\n", year, MonthName[month] );
								}
								break;

							case TMAX: //Tmax -  maximum temperature
								yos[*yos_count-1].m[month].tmax = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tmax) && *yos_count > 1)
								{
									//Log ("* T_MAX -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tmax = yos[*yos_count-2].m[month].tmax;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].tmax))
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
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tmin) && *yos_count > 1)
								{
									//Log ("* T_AVG -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tmin = yos[*yos_count-2].m[month].tmin;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].tmin))
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
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].vpd) && *yos_count > 1)
								{
									Log ("* VPD -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									yos[*yos_count-1].m[month].vpd = yos[*yos_count-2].m[month].vpd;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].vpd))
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
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ts_f) && *yos_count > 1)
								{
									//Log ("* TS_F -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									yos[*yos_count-1].m[month].ts_f = yos[*yos_count-2].m[month].ts_f;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].ts_f))
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
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].rain) && *yos_count > 1)
								{
									Log ("* PRECIPITATION -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].rain = yos[*yos_count-2].m[month].rain;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].rain))
									{
										Log ("********* PRECIPITATION -NO DATA- in previous year!!!!\n" );

									}
									//Log("precipitation of previous year = %g mm\n", yos[*yos_count-1].m[month].rain);
								}
								/*
								   else
								   {
								//convert daily average to monthly rain precipitation
								yos[*yos_count-1].m[month].rain *= yos[*yos_count-1].m[month].n_days;
								}
								 */
								//CONTROL
								if (yos[*yos_count-1].m[month].rain > settings->maxprecip)
								{
									Log("ERROR IN PRECIP DATA in year %s month %s!!!!\n", year, MonthName[month] );
								}
								break;

							case SWC: //Soil Water Content (%)
								yos[*yos_count-1].m[month].swc = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].swc) && *yos_count > 1)
								{
									//Log ("********* SWC -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
									//Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].swc = yos[*yos_count-2].m[month].swc;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].swc))
									{
										//Log ("* SWC -NO DATA- in previous year!!!!\n" );
										yos[*yos_count-1].m[month].swc = NO_DATA;
									}
								}
								break;
							case Ndvi_Lai: //Get LAI in spatial version
								if (settings->spatial == 's')
								{
									yos[*yos_count-1].m[month].ndvi_lai = convert_string_to_prec(token2, &error_flag);


									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									//if is not the first year the model get the previous year value
									if (*yos_count > 1)
									{

										//control in lai data if is an invalid value
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ndvi_lai))
										{
											Log ("********* LAI -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
											//Log("Getting previous years values !!\n");
											yos[*yos_count-1].m[month].ndvi_lai = yos[*yos_count-2].m[month].ndvi_lai;
											if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].ndvi_lai))
											{
												Log ("* LAI -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].ndvi_lai = NO_DATA;
											}
										}
										//control lai data in spatial version if value is higher than MAXLAI
										if(yos[*yos_count-1].m[month].ndvi_lai > settings->maxlai)
										{
											Log("********* INVALID DATA LAI > MAXLAI in year %s month %s!!!!\n", year, MonthName[month] );
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
							}
						}
						//set values for daily version
						else
						{
							if (strncmp (settings->daymet, "off", 3)== 0)
							{
								switch ( i )
								{
								//unused in monthly version just used in daily version
								case N_DAYS:
									yos[*yos_count-1].m[month].d[day].n_days = convert_string_to_int(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("n_days = %d\n", yos[*yos_count-1].m[month].d[day].n_days);
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].n_days > (int)settings->maxdays)
									{
										Log("ERROR IN N_DAYS DATA!!\n");
									}
									//Log("month %s day = %d\n",MonthName[month], yos[*yos_count-1].m[month].d[day].n_days);
									break;

								case RG_F: //Rg_f - solar_rad -daily average solar radiation
									yos[*yos_count-1].m[month].d[day].solar_rad = convert_string_to_prec(token2, &error_flag);
									//convert KJ to MJ
									//yos[*yos_count-1].m[month].solar_rad = yos[*yos_count-1].m[month].solar_rad / 1000;
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("solar_rad = %g\n", yos[*yos_count-1].m[month].d[day].solar_rad);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad)&& *yos_count > 1)
									{
										//the model gets the value of the day before
										Log ("* SOLAR RAD -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].solar_rad = yos[*yos_count].m[month].d[day-1].solar_rad;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad))
										{
											Log ("********* SOLAR RAD -NO DATA- in previous day!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].solar_rad = yos[*yos_count-2].m[month].d[day].solar_rad;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad))
											{
												Log ("********* SOLAR RAD -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].solar_rad = NO_DATA;
											}
										}
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].solar_rad > settings->maxrg )
									{
										Log("ERROR IN RG DATA in year %s month %s!!!!\n", year, MonthName[month] );
									}
									break;

								case TA_F: //Ta_f -  temperature average
									yos[*yos_count-1].m[month].d[day].tavg = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("tavg = %g\n", yos[*yos_count-1].m[month].d[day].tavg);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tavg)&& *yos_count > 1)
									{
										//the model gets the value of the day before
										//Log ("* TAVG -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										/*
										yos[*yos_count-1].m[month].d[day].tavg = yos[*yos_count-1].m[month].d[day-1].tavg;
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
										*/
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].tavg > settings->maxtavg)
									{
										Log("ERROR IN TAV DATA in year %s month %s!!!!\n", year, MonthName[month] );
									}
									//Log("%d-%s-tavg = %g\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;

								case TMAX: //TMAX -  maximum temperature
									yos[*yos_count-1].m[month].d[day].tmax = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("tmax = %g\n", yos[*yos_count-1].m[month].d[day].tmax);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax) && *yos_count > 1)
									{
										//the model gets the value of the day before
										//Log ("* TAV -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										yos[*yos_count-1].m[month].d[day].tmax = yos[*yos_count].m[month].d[day-1].tmax;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax))
										{
											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].tmax = yos[*yos_count-2].m[month].d[day].tmax;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmax))
											{
												Log ("********* TMAX -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].tmax = NO_DATA;
											}
										}
									}
									//Log("%d-%s-tavg = %g\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;

								case TMIN: //TMIN -  minimum temperature
									yos[*yos_count-1].m[month].d[day].tmin = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("tmin = %g\n", yos[*yos_count-1].m[month].d[day].tmin);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tmin) && *yos_count > 1)
									{
										//the model gets the value of the day before
										Log ("* TAV -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].tmin = yos[*yos_count].m[month].d[day-1].tmin;
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
									break;

								case VPD_F: //RH_f - RH
									yos[*yos_count-1].m[month].d[day].vpd = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("vpd = %g\n", yos[*yos_count-1].m[month].d[day].vpd);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd) && *yos_count > 1)
									{
										//the model gets the value of the day before
										Log ("* VPD -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										yos[*yos_count-1].m[month].d[day].vpd = yos[*yos_count].m[month].d[day-1].vpd;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd))
										{
											Log ("********* VPD -NO DATA- in previous year!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].vpd = yos[*yos_count-2].m[month].d[day].vpd;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd))
											{
												Log ("********* VPD -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].vpd = NO_DATA;
											}

										}
									}
									//Log("%d-%s-vpd = %g\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].vpd);
									break;

								case TS_F: // ts_f   Soil temperature
									yos[*yos_count-1].m[month].d[day].ts_f = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f) && *yos_count > 1)
									{
										//the model gets the value of the day before
										//Log ("* TS_F -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										yos[*yos_count-1].m[month].d[day].ts_f = yos[*yos_count].m[month].d[day-1].ts_f;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f))
										{
											//Log ("********* TS_F -NO DATA- in previous year!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].ts_f = yos[*yos_count-2].m[month].d[day].ts_f;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f))
											{
												//Log ("********* TS_F -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].ts_f = NO_DATA;
											}
										}
									}
									//Log("%d-%s-ts_f = %g\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].ts_f);
									break;

								case PRECIP:  //Precip - rain
									yos[*yos_count-1].m[month].d[day].rain = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									else
									{
										//Log("rain = %g\n", yos[*yos_count-1].m[month].d[day].rain);
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].rain) && *yos_count > 1)
									{
										//the model gets the value of the day before
										Log ("* PRECIPITATION -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].rain = yos[*yos_count].m[month].d[day-1].rain;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day-1].rain))
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
										//Log("precipitation of previous year = %g mm\n", yos[*yos_count-1].m[month].rain);
									}
									/*
								   else
								   {
								//convert daily average to monthly rain precipitation
								yos[*yos_count-1].m[month].rain *= yos[*yos_count-1].m[month].n_days;
								}
									 */
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].rain > settings->maxprecip)
									{
										Log("ERROR IN PRECIP DATA in year %s month %s!!!!\n", year, MonthName[month] );
									}
									//Log("%d-%s-precip = %g\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].rain);
									break;

								case SWC: //Soil Water Content (%)
									yos[*yos_count-1].m[month].d[day].swc = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc) && *yos_count > 1)
									{
										//the model gets the value of the day before
										//Log ("********* SWC -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
										//Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].swc = yos[*yos_count].m[month].d[day-1].swc;
										if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc))
										{
											//Log ("* SWC -NO DATA- in previous year!!!!\n" );

											//the model gets the value of the year before
											yos[*yos_count-1].m[month].d[day].swc = yos[*yos_count-2].m[month].d[day].swc;

											if (IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc))
											{
												//Log ("********* RAIN -NO DATA- in previous year!!!!\n" );
												yos[*yos_count-1].m[month].d[day].swc = NO_DATA;
											}
										}
									}
									//Log("%d-%s-swc= %g\n",yos[*yos_count-1].m[month].d[day].n_days, MonthName[month], yos[*yos_count-1].m[month].d[day].swc);
									break;
								case Ndvi_Lai: //Get LAI in spatial version
									if (settings->spatial == 's')
									{
										yos[*yos_count-1].m[month].d[day].ndvi_lai = convert_string_to_prec(token2, &error_flag);


										if ( error_flag )
										{
											printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, MonthName[month]);
											free(yos);
											fclose(f);
											return NULL;
										}
										//if is not the first year the model get the previous year value
										if (*yos_count > 1)
										{

											//control in lai data if is an invalid value
											if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ndvi_lai))
											{
												//the model gets the value of the day before
												Log ("********* LAI -NO DATA in year %s month %s!!!!\n", year, MonthName[month] );
												//Log("Getting previous years values !!\n");
												yos[*yos_count-1].m[month].d[day].ndvi_lai = yos[*yos_count].m[month].d[day-1].ndvi_lai;
												if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ndvi_lai))
												{
													Log ("* LAI -NO DATA- in previous year!!!!\n" );
													yos[*yos_count-1].m[month].d[day].ndvi_lai = NO_DATA;
												}
											}
											//control lai data in spatial version if value is higher than MAXLAI
											if(yos[*yos_count-1].m[month].d[day].ndvi_lai > settings->maxlai)
											{
												Log("********* INVALID DATA LAI > MAXLAI in year %s month %s!!!!\n", year, MonthName[month] );
												Log("Getting previous years values !!\n");
												yos[*yos_count-1].m[month].d[day].ndvi_lai = yos[*yos_count-2].m[month].d[day].ndvi_lai;
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
			if (settings->time == 'd')
			{
				++day;
				if (day == giorninelmese[month])
				{
					++month;
					day = 0;
				}
			}
			else
			{
				++month;
			}
		}

		// check for month
		if (settings->time == 'm')
		{
			if ( month != MONTHS )
			{
				printf("missing values in met data file, %d months imported instead of %d !\n", month+1, MONTHS);
				free(yos);
				return NULL;
			}
		}


		fclose(f);
	}

	// sort
	if ( *yos_count > 1 )
	{
		qsort(yos, *yos_count, sizeof*yos, sort_by_years);
	}

	// return pointer
	return yos;
}

