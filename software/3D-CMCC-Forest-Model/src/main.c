/* main.c */



//LIST OF THINGS TODO
//TODO SOIL LAYER ask simona
//TODO RUN MODEL FOR ROCCA
//TODO SIMULAZIONI DA bareground
//TODO RENDERE DINAMICO IL TREE_LAYER_LIMIT (all'aumentare del dbh aumenta il TREE_LAYER_LIMIT) see Montgomery & Chazdon, 2001)
//TODO INSERIRE UN MAX NUMBER OF LAYER IN FUNZIONE DEL TREE HEIGHT (chiedere Arianna)
//TODO NETCDF
//TODO VERSION FEM AND BGC
//TODO RIMETTERE PER L'ACQUA COMPETIZIONE SIMMETRICA




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

//#include <netcdf.h>


#include "compiler.h"
#include "types.h"
#include "constants.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

/* constants */
#define PROGRAM_VERSION	"5.0"

/* */
enum {	MONTH = 0,
	N_DAYS,
	RG_F,
	TA_F,
	VPD_F,
	TS_F,
	PRECIP,
	SWC,
	Ndvi_Lai,


	MET_COLUMNS };

//Last cumulative days in months
int MonthLength [] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 364};

//Days in Months
int DaysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *szMonth[MONTHS] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
		"AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };


/* global variables */
char *program_path		=	NULL,	// mandatory
		*input_dir			=	NULL,	// mandatory
		*input_path		=	NULL,	// mandatory
		*dataset_filename	=	NULL,	// mandatory
		*input_met_path	=	NULL,	// mandatory
		*site_path			=	NULL,	// mandatory
		*output_path		=	NULL,	// mandatory
		*out_filename		=	NULL,	// mandatory
		*output_file		= 	NULL,	// mandatory
		*settings_path		=	NULL;	// mandatory
//*resolution		= 	NULL,	// mandatory
//*vers_arg			= 	NULL;	// mandatory

int log_enabled		=	1,	// default is on
		years_of_simulation	=	0;	// default is none

/* static global variables */
static FILES *files_founded;

/* strings */
static const char banner[] =	"\n\n3D-CMCC Forest Model v."PROGRAM_VERSION"\n\n"
		"by Alessio Collalti - for contacts: a dot collalti at unitus dot it; alessio dot collalti at cmcc dot it\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"(use -h parameter for more information)\n\n\n";

static char copyright[] =
		"\n\n\n\n"
		"This version of 3D-CMCC Forest Model has been developed by:\n"
		"Alessio Collalti [a.collalti@unitus.it - alessio.collalti@cmcc.it],\n"
		"Alessio Ribeca,\n"
		"Alessandro Candini [candini@meeo.it].\n"
		"euroMediterranean Center on Climate Changes (CMCC),\n"
		"IAFENT division,\n"
		"Via A. Pacinotti 5, 01100 - Viterbo, Italy,\n"
		"and \nUniversity of Tuscia ,\n"
		"Department for innovation in biological, agro-food and forest systems (DIBAF),\n"
		"Forest Ecology Lab\n"
		"MEEO S.r.l.\n"
		"Via Saragat 9, 44122 Ferrara, Italy\n"
		"\n"
		"Programmers: Alessio Collalti - Alessio Ribeca - Alessandro Candini\n"
		"\n"
		"\"DISCLAIMER\" \n"
		"--------------------------------------------------------------------------------------------------------\n"
		"CMCC and University of Tuscia\n"
		"accepts no responsibility for the use of the 3D_CMCC FEM in\n"
		"the form supplied or as subsequently modified by third parties. CMCC and University of Tuscia disclaims\n"
		"liability for all losses, damages and costs incurred by any person as a result\n"
		"of relying on this software. Use of this software assumes agreement to this condition of use.\n"
		"Removal of this statement violates the spirit in which 3D-CMCC FEM,\n"
		"was released by CMCC-UNITUS. \n"
		"\n";


static const char comma_delimiter[] = ",\r\n";
static const char met_delimiter[] = " ,\t\r\n";
static const char *met_columns[MET_COLUMNS] = {	"Month",
		"n_days",
		"Rg_f",
		"Ta_f",
		//"Tamax",
		//"Tmin",
		"VPD_f",
		"Ts_f",
		"Precip",
		"SWC",
		"LAI"
};

/* messages */
/*
static const char msg_dataset_not_specified[] =
		"dataset not specified."
#if defined (_WIN32) || defined (linux)
		" searching..."
#endif
		"\n";
 */
static const char msg_dataset_path[]	=	"dataset path = %s\n";
static const char msg_site_path[]		=	"site path = %s\n";
static const char msg_settings_path[]	=	"settings path = %s\n";
//static const char msg_output_path[]		=	"output path = %s\n";
static const char msg_output_file[]		=	"output file = %s\n\n";
static const char msg_processing[]		=	"processing %s...\n";
static const char msg_ok[]				=	"ok";
static const char msg_summary[]			=	"\n%d file%s found: %d processed, %d skipped.\n\n";
static const char msg_usage[]			=	"usage: 3D-CMCC parameters\n\n"
		"  allowed parameters:\n\n"
		"    -dataset=XXXXX_YYYY.txt -> file to be processed"
		"    -met=XXXXX -> met file\n"
#if defined (_WIN32) || defined (linux)
		" (optional)"
#endif
		"\n"
		"    -output=path where result files are created\n"
		"    -outname=output filename\n"
		"    -site=site filename\n"
		"    -settings=settings filename\n"
		"    -log -> enable log to file\n";

/* error messages */
//extern const char err_out_of_memory[];
const char err_unable_open_file[] = "unable to open file.";
const char err_empty_file[] = "empty file ?";
const char err_window_size_too_big[] = "window size too big.";
static const char err_unable_get_current_directory[] = "unable to retrieve current directory.\n";
static const char err_unable_to_register_atexit[] = "unable to register clean-up routine.\n";
//static const char err_output_path_no_delimiter[] = "output path must end with a \"%c\"\n\n";
//static const char err_unable_open_output_path[] = "unable to open output path.\n";
static const char err_dataset_already_specified[] = "dataset already specified (%s)! \"%s\" skipped.\n";
static const char err_site_already_specified[] = "site already specified (%s)! \"%s\" skipped.\n";
static const char err_settings_already_specified[] = "settings already specified (%s)! \"%s\" skipped.\n";
//static const char err_resolution_already_specified[] = "resolution already specified (%s)! \"%s\" skipped.\n";
//static const char err_version_already_specified[] = "version already specified (%s)! \"%s\" skipped.\n";
static const char err_met_already_specified[] = "met already specified (%s)! \"%s\" skipped.\n";
static const char err_output_already_specified[] = "output path already specified (%s)! \"%s\" skipped.\n";
static const char err_outname_already_specified[] = "output filename specified without output path (%s)! \"%s\" skipped.\n";
static const char err_arg_needs_param[] = "%s parameter not specified.\n\n";
static const char err_arg_no_needs_param[] = "%s no needs parameter.\n\n";
static const char err_unable_convert_value_arg[] = "unable to convert value \"%s\" for %s.\n\n";
//static const char err_met_not_specified[] = "met not specified.";
//static const char err_zero_years_elaboration[] = "years of elaboration should be at least 1.\n";

int sort_by_years(const void *a, const void *b)
{
	if ( ((YOS *)a)->year < ((YOS *)b)->year )
	{
		return -1;
	}
	else if ( ((YOS *)a)->year > ((YOS *)b)->year )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


static void clean_up(void)
{
	if (program_path)
		free(program_path);

	if (files_founded)
		free(files_founded);
}

int get_input_path(char *arg, char *param, void *p) {
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( input_path )
	{
		printf(err_dataset_already_specified, input_path, param);
	}
	else
	{
		input_path = param;
	}

	/* ok */
	return 1;
}

int get_site_path(char *arg, char *param, void *p) {
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( site_path )
	{
		printf(err_site_already_specified, site_path, param);
	}
	else
	{
		site_path = param;
	}

	/* ok */
	return 1;
}

int get_settings_path(char *arg, char *param, void *p) {
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( settings_path )
	{
		printf(err_settings_already_specified, settings_path, param);
	}
	else
	{
		settings_path = param;
	}

	/* ok */
	return 1;
}

/*int get_resolution(char *arg, char *param, void *p) {
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( resolution )
	{
		printf(err_resolution_already_specified, resolution, param);
	}
	else
	{
		resolution = param;
	}

	// ok
	return 1;
}

int get_version(char *arg, char *param, void *p) {
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( vers_arg )
	{
		printf(err_version_already_specified, vers_arg, param);
	}
	else
	{
		vers_arg = param;
	}

	// ok
	return 1;
}*/

/* */
int get_met_path(char *arg, char *param, void *p)
{
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( input_met_path )
	{
		printf(err_met_already_specified, input_met_path, param);
	}
	else
	{
		input_met_path = param;
	}

	/* ok */
	return 1;
}

/* */
int get_output_path(char *arg, char *param, void *p)
{
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( output_path )
		printf(err_output_already_specified, output_path, param);
	else
	{
		output_path = malloc(sizeof(*output_path)*BUFFER_SIZE);
		output_file = malloc(sizeof(*output_file)*BUFFER_SIZE);

		output_path = param;
		strcpy(output_file, output_path);
		//		strcat(output_file, LOGFILE);
	}

	return 1;
}

int get_output_filename(char *arg, char *param, void *p)
{
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( out_filename )
		printf(err_outname_already_specified, out_filename, param);
	else
	{
		out_filename = param;

		if( output_path )
			strcat(output_file, out_filename);
		else
			printf("With -outname flag set -outpath not set: using default output file (prog_path/output.txt)");
	}
	Log("output file name = %s\n", out_filename);
	return 1;
}

/* */
int set_prec_value(char *arg, char *param, void *p)
{
	int error;

	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	/* convert param */
	*(PREC *)p = convert_string_to_prec(param, &error);
	if ( error )
	{
		printf(err_unable_convert_value_arg, param, arg);
		return 0;
	}

	/* ok */
	return 1;
}

/* */
int set_int_value(char *arg, char *param, void *p)
{
	int error;

	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	/* convert param */
	*(int *)p = convert_string_to_int(param, &error);
	if ( error )
	{
		printf(err_unable_convert_value_arg, param, arg);
		return 0;
	}

	/* ok */
	return 1;
}

/* */
int set_log_file(char *arg, char *param, void *p)
{
	if ( param )
	{
		printf(err_arg_no_needs_param, arg);
		return 0;
	}

	log_enabled = 1;

	/* ok */
	return 1;
}

/* */
int show_help(char *arg, char *param, void *p)
{
	if ( param )
	{
		printf(err_arg_no_needs_param, arg);
		return 0;
	}

	puts(msg_usage);

	/* must return error */
	return 0;
}

void usage(void)
{
	fprintf(stderr, "\nUsage:\n");
	fprintf(stderr, "\t./3D-CMCC Model -i INPUT_DIR -o OUTPUT_FILENAME -d DATASET_FILENAME -m MET_FILE_LIST -s SITE_FILENAME -c SETTINGS_FILENAME [-h]\n");
	fprintf(stderr, "\nMandatory options:\n");
	fprintf(stderr, "\t-i\tinput directory\t\t\t\t\t(i.e.: -i /path/to/input/directory/)\n");
	fprintf(stderr, "\t-o\toutput filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stderr, "\t-d\tdataset filename stored into input directory\t(i.e.: -d input.txt)\n");
	fprintf(stderr, "\t-m\tmet filename list stored into input directory\t(i.e.: -m 1999.txt,2003.txt,2009.txt)\n");
	fprintf(stderr, "\t-s\tsite filename stored into input directory\t(i.e.: -s site.txt)\n");
	fprintf(stderr, "\t-s\tsettings filename stored into input directory\t(i.e.: -c settings.txt)\n");
	fprintf(stderr, "\nOptional options:\n");
	fprintf(stderr, "\t-h\tprint this help\n");
	fprintf(stderr, "\nLaunch example:\n");
	fprintf(stderr, "\t./3D-CMCC Model -i /path/to/input/directory/ -o /path/to/CMCC.log -d /path/to/input.txt -m /path/to/1999.txt,/path/to/2003.txt,/path/to/2009.txt -s /path/to/site.txt -c /path/to/settings.txt\n");
	exit(1);
}

/*import met data file*/
//------------------------------------------------------------------------------


int is_valid_met(const char *const input_file)
{
	// met file should be MET_FILENAME_LEN chars len
	if ( MET_FILENAME_LEN != strlen(input_file) )
		return 0;

	// 01234567890123456789
	// ATNeu2002_L4_m.txt
	// 10th and 12th chars must be an underscore
	if ( '_' != input_file[9] || '_' != input_file[12] )
		return 0;

	// from 5th to 8th chars a digit must be present ( year )
	if ( 0 == isdigit(input_file[5]) ) return 0;
	if ( 0 == isdigit(input_file[6]) ) return 0;
	if ( 0 == isdigit(input_file[7]) ) return 0;
	if ( 0 == isdigit(input_file[8]) ) return 0;

	/* ok */
	return 1;
}

//
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

	if ( !file ) return NULL;

	// reset
	*yos_count = 0;

	//
	for ( token = mystrtok(file, comma_delimiter, &p); token; token = mystrtok(NULL, comma_delimiter, &p) )
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
		if ( !yos_no_leak )
		{
			//
			Log("out of memory.\n");
			free(yos);
			return NULL;
		}

		// assign memory
		yos = yos_no_leak;

		filename = malloc(sizeof(*filename)*BUFFER_SIZE);
		if( !filename )
		{
			fprintf(stderr, "Cannot allocate memory for filename.\n");
			return NULL;
		}
		bzero(filename, BUFFER_SIZE-1);
		// Get only filename.txt from /this/is/the/complete/path/of/filename.txt
		filename = (strrchr(token, '/'))+1;
		// Get year from filename: file name is like 123_4567_2007.txt where
		// 123 is the x coordinate of the cell,
		// 4567 is the y coordinate of the cell,
		// 2007 is the year

		char *pch,
		*tmp_filename;

		tmp_filename = (char*)malloc(sizeof(char)*1024);
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
							switch ( i )
							{
							//unused in monthly version just used in daily version
							case N_DAYS:
								yos[*yos_count-1].m[month].n_days = convert_string_to_int(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
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
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].solar_rad))
								{
									Log ("* SOLAR RAD -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
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
									Log("ERROR IN RG DATA in year %s month %s!!!!\n", year, szMonth[month] );
								}
								//convert daily average solar radiation to monthly solar radiation
								//m[month].solar_rad *= m[month].n_days;
								break;

							case TA_F: //Ta_f -  temperature average
								yos[*yos_count-1].m[month].tavg = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].tavg))
								{
									Log ("* T_AVG -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
									Log("Getting previous years values !!\n");
									yos[*yos_count-1].m[month].tavg = yos[*yos_count-2].m[month].tavg;
									if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].tavg))
									{
										Log ("********* T_AVG -NO DATA- in previous year!!!!\n" );
									}
								}
								//CONTROL
								if (yos[*yos_count-1].m[month].tavg > settings->maxtavg)
								{
									Log("ERROR IN TAV DATA in year %s month %s!!!!\n", year, szMonth[month] );
								}
								break;

							case VPD_F: //RH_f - RH
								yos[*yos_count-1].m[month].vpd = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].vpd))
								{
									Log ("* VPD -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
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
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].ts_f))
								{
									//Log ("* TS_F -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
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
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].rain))
								{
									Log ("* PRECIPITATION -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
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
									Log("ERROR IN PRECIP DATA in year %s month %s!!!!\n", year, szMonth[month] );
								}
								break;

							case SWC: //Soil Water Content (%)
								yos[*yos_count-1].m[month].swc = convert_string_to_prec(token2, &error_flag);
								if ( error_flag )
								{
									printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
									free(yos);
									fclose(f);
									return NULL;
								}
								if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].swc))
								{
									//Log ("********* SWC -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
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
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
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
											Log ("********* LAI -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
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
											Log("********* INVALID DATA LAI > MAXLAI in year %s month %s!!!!\n", year, szMonth[month] );
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
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].n_days > (int)settings->maxdays)
									{
										Log("ERROR IN N_DAYS DATA!!\n");
									}
									//Log("month %s day = %d\n",szMonth[month], yos[*yos_count-1].m[month].d[day].n_days);
									break;

								case RG_F: //Rg_f - solar_rad -daily average solar radiation
									yos[*yos_count-1].m[month].d[day].solar_rad = convert_string_to_prec(token2, &error_flag);
									//convert KJ to MJ
									//yos[*yos_count-1].m[month].solar_rad = yos[*yos_count-1].m[month].solar_rad / 1000;
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].solar_rad))
									{
										Log ("* SOLAR RAD -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
										Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].solar_rad = yos[*yos_count-2].m[month].d[day].solar_rad;
										if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].d[day].solar_rad))
										{
											Log ("********* SOLAR RAD -NO DATA- in previous year!!!!\n" );
										}
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].solar_rad > settings->maxrg )
									{
										Log("ERROR IN RG DATA in year %s month %s!!!!\n", year, szMonth[month] );
									}
									//convert daily average solar radiation to monthly solar radiation
									//m[month].solar_rad *= m[month].n_days;
									//Log("%d-%s-solar rad = %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].solar_rad);
									break;

								case TA_F: //Ta_f -  temperature average
									yos[*yos_count-1].m[month].d[day].tavg = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].tavg))
									{
										Log ("* TAV -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
										Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].tavg = yos[*yos_count-2].m[month].d[day].tavg;
										if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].d[day].tavg))
										{
											Log ("********* T_AVG -NO DATA- in previous year!!!!\n" );
										}
									}
									//CONTROL
									if (yos[*yos_count-1].m[month].d[day].tavg > settings->maxtavg)
									{
										Log("ERROR IN TAV DATA in year %s month %s!!!!\n", year, szMonth[month] );
									}
									//Log("%d-%s-tavg = %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].tavg);
									break;

								case VPD_F: //RH_f - RH
									yos[*yos_count-1].m[month].d[day].vpd = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].vpd))
									{
										Log ("* VPD -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
										yos[*yos_count-1].m[month].d[day].vpd = yos[*yos_count-2].m[month].d[day].vpd;
										if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].d[day].vpd))
										{
											Log ("********* VPD -NO DATA- in previous year!!!!\n" );
										}
									}
									//Log("%d-%s-vpd = %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].vpd);
									break;

								case TS_F: // ts_f   Soil temperature
									yos[*yos_count-1].m[month].d[day].ts_f = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].ts_f))
									{
										//Log ("* TS_F -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
										yos[*yos_count-1].m[month].d[day].ts_f = yos[*yos_count-2].m[month].d[day].ts_f;
										if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].ts_f))
										{
											//Log ("********* TS_F -NO DATA- in previous year!!!!\n" );
										}
									}
									//Log("%d-%s-ts_f = %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].ts_f);
									break;

								case PRECIP:  //Precip - rain
									yos[*yos_count-1].m[month].d[day].rain = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].rain))
									{
										Log ("* PRECIPITATION -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
										Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].rain = yos[*yos_count-2].m[month].d[day].rain;
										if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].d[day].rain))
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
									if (yos[*yos_count-1].m[month].d[day].rain > settings->maxprecip)
									{
										Log("ERROR IN PRECIP DATA in year %s month %s!!!!\n", year, szMonth[month] );
									}
									//Log("%d-%s-precip = %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].rain);
									break;

								case SWC: //Soil Water Content (%)
									yos[*yos_count-1].m[month].d[day].swc = convert_string_to_prec(token2, &error_flag);
									if ( error_flag )
									{
										printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
										free(yos);
										fclose(f);
										return NULL;
									}
									if ( IS_INVALID_VALUE (yos[*yos_count-1].m[month].d[day].swc))
									{
										//Log ("********* SWC -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
										//Log("Getting previous years values !!\n");
										yos[*yos_count-1].m[month].d[day].swc = yos[*yos_count-2].m[month].d[day].swc;
										if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].d[day].swc))
										{
											//Log ("* SWC -NO DATA- in previous year!!!!\n" );
											yos[*yos_count-1].m[month].d[day].swc = NO_DATA;
										}
									}
									//Log("%d-%s-swc= %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].swc);
									break;
								case Ndvi_Lai: //Get LAI in spatial version
									if (settings->spatial == 's')
									{
										yos[*yos_count-1].m[month].d[day].ndvi_lai = convert_string_to_prec(token2, &error_flag);


										if ( error_flag )
										{
											printf("unable to convert value \"%s\" at column %d for %s\n", token2, column+1, szMonth[month]);
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
												Log ("********* LAI -NO DATA in year %s month %s!!!!\n", year, szMonth[month] );
												//Log("Getting previous years values !!\n");
												yos[*yos_count-1].m[month].d[day].ndvi_lai = yos[*yos_count-2].m[month].d[day].ndvi_lai;
												if ( IS_INVALID_VALUE (yos[*yos_count-2].m[month].d[day].ndvi_lai))
												{
													Log ("* LAI -NO DATA- in previous year!!!!\n" );
													yos[*yos_count-1].m[month].d[day].ndvi_lai = NO_DATA;
												}
											}
											//control lai data in spatial version if value is higher than MAXLAI
											if(yos[*yos_count-1].m[month].d[day].ndvi_lai > settings->maxlai)
											{
												Log("********* INVALID DATA LAI > MAXLAI in year %s month %s!!!!\n", year, szMonth[month] );
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
									//Log("%d-%s-ndvi_lai = %g\n",yos[*yos_count-1].m[month].d[day].n_days, szMonth[month], yos[*yos_count-1].m[month].d[day].ndvi_lai);
									//Log("mese = %d\n", yos[*yos_count -1].m);
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
				if (day == DaysInMonth[month])
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

void met_summary(MET_DATA *met) {
	int i;

	/* check parameter */
	assert(met);

	/* */
	for ( i=0; i<MONTHS; i++ )
	{
		Log("MET_SUMMARY FUNCTION\n");
		Log(	"MET DATA - month %02d:\n"
				"n_days = %d\n"
				"solar_rad = %g\n"
				"tav = %g\n"
				"rh = %g\n"
				"ts_f = %g\n"
				"rain = %g\n"
				"swc = %g\n",
				"lai = %g\n",
				i+1,
				met[i].n_days,
				met[i].solar_rad,
				met[i].tavg,
				met[i].vpd,
				met[i].ts_f,
				met[i].rain,
				met[i].swc,
				met[i].ndvi_lai
		);
	}
}


//----------------------------------------------------------------------------//
//                                                                            //
//                                 MAIN.C                                     //
//                                                                            //
//----------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
	int i,
	error,
	rows_count,
	files_founded_count,
	files_processed_count,
	files_not_processed_count,
	total_files_count;

	struct tm* data;

	int years,
	month,
	day;

	int cell;

	//int site_data;  // if site data ok = 1, else = 0

	MET_DATA *met;

	YOS *yos;
	ROW *rows;
	MATRIX *m;
	//	const ARGUMENT args[] = {
	//		{ "dataset",	get_input_path,		NULL },
	//		{ "site",	get_site_path,		NULL },
	//		{ "met",	get_met_path,		NULL },
	//		{ "output",	get_output_path,	NULL },
	//		{ "outname",	get_output_filename,	NULL },
	//		{ "log",	set_log_file,		NULL },
	//		{ "h",		show_help,		NULL },
	//		{ "?",		show_help,		NULL },
	//		{ "help",	show_help,		NULL },
	//	};
	//
	/* register atexit */
	if ( -1 == atexit(clean_up) )
	{
		puts(err_unable_to_register_atexit);
		return 1;
	}
	//
	//	/* parse arguments */
	//	if ( !parse_arguments(argc, argv, args, SIZE_OF_ARRAY(args)) )
	//	{
	//		return 1;
	//	}

	// Parsing arguments
	for(i=1; i<argc; i++)
	{
		if ( argv[i][0] != '-' ) continue;
		switch(argv[i][1])
		{
		case 'i': // Directory where input files are stored
			input_dir = malloc(sizeof(*input_dir)*BUFFER_SIZE);
			if( !input_dir )
			{
				fprintf(stderr, "Cannot allocate memory for input_dir.\n");
				return 1;
			}
			bzero(input_dir, BUFFER_SIZE-1);
			strcpy(input_dir, argv[i+1]);
			break;
		case 'o': // Output file name (with path)
			out_filename = malloc(sizeof(*out_filename)*BUFFER_SIZE);
			if( !out_filename )
			{
				fprintf(stderr, "Cannot allocate memory for out_filename.\n");
				return 1;
			}
			bzero(out_filename, BUFFER_SIZE-1);
			strcpy(out_filename, argv[i+1]);
			break;
		case 'd': // Dataset filename
			dataset_filename = malloc(sizeof(*dataset_filename)*BUFFER_SIZE);
			if( !dataset_filename )
			{
				fprintf(stderr, "Cannot allocate memory for dataset_filename.\n");
				return 1;
			}
			bzero(dataset_filename, BUFFER_SIZE-1);
			strcpy(dataset_filename, argv[i+1]);
			break;
		case 'm': // Met filename list
			input_met_path = malloc(sizeof(*input_met_path)*BUFFER_SIZE);
			if( !input_met_path )
			{
				fprintf(stderr, "Cannot allocate memory for input_met_path.\n");
				return 1;
			}
			bzero(input_met_path, BUFFER_SIZE-1);
			strcpy(input_met_path, argv[i+1]);
			break;
		case 's': // Site filename
			site_path = malloc(sizeof(*site_path)*BUFFER_SIZE);
			if( !site_path )
			{
				fprintf(stderr, "Cannot allocate memory for site_path.\n");
				return 1;
			}
			bzero(site_path, BUFFER_SIZE-1);
			strcpy(site_path, argv[i+1]);
			break;
		case 'c': // Settings filename
			settings_path = malloc(sizeof(*settings_path)*BUFFER_SIZE);
			if( !settings_path )
			{
				fprintf(stderr, "Cannot allocate memory for settings_path.\n");
				return 1;
			}
			bzero(settings_path, BUFFER_SIZE-1);
			strcpy(settings_path, argv[i+1]);
			break;
			/*case 'r': // Resolution (must be 10 or 100)
			resolution = malloc(sizeof(*resolution)*BUFFER_SIZE);
			if( !resolution )
			{
				fprintf(stderr, "Cannot allocate memory for resolution.\n");
				return 1;
			}
			bzero(resolution, BUFFER_SIZE-1);
			strcpy(resolution, argv[i+1]);
			break;
		case 'v': // Version (must be 'u' or 's')
			vers_arg = malloc(sizeof(*vers_arg)*BUFFER_SIZE);
			if( !vers_arg )
			{
				fprintf(stderr, "Cannot allocate memory for version.\n");
				return 1;
			}
			bzero(vers_arg, BUFFER_SIZE-1);
			strcpy(vers_arg, argv[i+1]);
			break;*/
		case 'h': // Print help
			usage();
			break;
		default:
			printf("Invalid option (%s)!\n", argv[i]);
			usage();
			return 1;
		}

	}

	// Check if every mandatory parameter has been used
	if( input_dir == NULL )
	{
		fprintf(stderr, "Error: input dir option is missing!\n");
		usage();
	}
	else
	{
		strcat(input_dir, "/");
	}

	if( out_filename == NULL )
	{
		fprintf(stderr, "Error: output filename option is missing!\n");
		usage();
	}
	else
	{
		output_file = malloc(sizeof(*output_file)*BUFFER_SIZE);
		if( !output_file )
		{
			fprintf(stderr, "Cannot allocate memory for output_file.\n");
			return 1;
		}
		bzero(output_file, BUFFER_SIZE-1);
		strcpy(output_file, out_filename);

	}


	if( dataset_filename == NULL )
	{
		fprintf(stderr, "Error: dataset filename option is missing!\n");
		usage();
	}
	else
	{
		input_path = malloc(sizeof(*input_path)*BUFFER_SIZE);
		if( !input_path )
		{
			fprintf(stderr, "Cannot allocate memory for input_path.\n");
			return 1;
		}
		bzero(input_path, BUFFER_SIZE-1);

		strcat(input_path, dataset_filename);
	}

	if( input_met_path == NULL )
	{
		fprintf(stderr, "Error: met file list is missing!\n");
		usage();
	}
	else
	{
		char *pch = NULL,
				*tmp_input_met_path = NULL;

		tmp_input_met_path = malloc(sizeof(*tmp_input_met_path)*BUFFER_SIZE);
		if( !tmp_input_met_path )
		{
			fprintf(stderr, "Cannot allocate memory for tmp_input_met_path.\n");
			return 1;
		}
		bzero(tmp_input_met_path, BUFFER_SIZE-1);

		// Add to every met filename its relative path
		for(pch = strtok(input_met_path, ","); pch != NULL; pch = strtok (NULL, ","))
		{
			sprintf(tmp_input_met_path, "%s%s,", tmp_input_met_path, pch); // line 1
		}


		// Delete the last ',' inserted
		tmp_input_met_path[strlen(tmp_input_met_path)-1] = '\0';

		// Copy temporary value into the correct variable
		strcpy(input_met_path, tmp_input_met_path);

		free(tmp_input_met_path);
	}

	if( site_path == NULL )
	{
		fprintf(stderr, "Error: site filename option is missing!\n");
		usage();
	}
	else
	{
		char *tmp = NULL;
		tmp = malloc(sizeof(*tmp)*BUFFER_SIZE);
		if( !tmp )
		{
			fprintf(stderr, "Cannot allocate memory for tmp.\n");
			return 1;
		}
		bzero(tmp, BUFFER_SIZE-1);
		strcat(tmp, site_path);
		strcpy(site_path, tmp);

		free(tmp);
	}

	if( settings_path == NULL )
	{
		fprintf(stderr, "Error: settings filename option is missing!\n");
		usage();
	}
	else
	{
		char *tmp = NULL;
		tmp = malloc(sizeof(*tmp)*BUFFER_SIZE);
		if( !tmp )
		{
			fprintf(stderr, "Cannot allocate memory for tmp.\n");
			return 1;
		}
		bzero(tmp, BUFFER_SIZE-1);
		strcat(tmp, settings_path);
		strcpy(settings_path, tmp);

		free(tmp);
	}

	/*	if( resolution == NULL )
	{
		fprintf(stderr, "Error: resolution option is missing!\n");
		usage();
	}
	else
	{
		char *tmp = NULL;
		tmp = malloc(sizeof(*tmp)*BUFFER_SIZE);
		if( !tmp )
		{
			fprintf(stderr, "Cannot allocate memory for tmp.\n");
			return 1;
		}
		bzero(tmp, BUFFER_SIZE-1);
		strcat(tmp, resolution);
		strcpy(resolution, tmp);
		sizeCell = atoi(resolution);


		if( (sizeCell < 10) || (sizeCell > 100) )
		{
			fprintf(stderr, "Error: resolution of the point must be included 10 or 100 meters!\n");
			exit(2);
		}


		// DEFINE SIZE CELL IN SQUARE METERS (10000 m^2 = 1ha); its value is:
		// 100   for pixels of 10x10   meters resolutionsu
		// 10000 for pixels of 100x100 meters resolution
		sizeCell = sizeCell*sizeCell;


		free(tmp);
	}


	if( vers_arg == NULL )
	{
		fprintf(stderr, "Error: version option is missing!\n");
		usage();
	}
	else
	{
		char *tmp = NULL;
		tmp = malloc(sizeof(*tmp)*BUFFER_SIZE);
		if( !tmp )
		{
			fprintf(stderr, "Cannot allocate memory for version.\n");
			return 1;
		}
		bzero(tmp, BUFFER_SIZE-1);
		strcat(tmp, vers_arg);
		strcpy(vers_arg, tmp);
		version = *vers_arg;
		if( (version != 's') && (version != 'u') )
		{
			fprintf(stderr, "Error: version must be 's' or 'u'!\n");
			exit(2);
			free(tmp);
		}
	}*/



	/* get program path */
	program_path = get_current_directory();
	if ( !program_path )
	{
		puts(err_unable_get_current_directory);
		return 1;
	}

	//	/* dataset specified ? */
	//	if ( !input_path )
	//	{
	//		puts(msg_dataset_not_specified);
	//		input_path = program_path;
	//	}
	//
	//	/* output path specified ? */
	//	if ( output_path )
	//	{
	//		/* check if last char is a FOLDER_DELIMITER */
	//		if ( output_path[strlen(output_path)-1] != FOLDER_DELIMITER )
	//		{
	//			printf(err_output_path_no_delimiter, FOLDER_DELIMITER);
	//			return 1;
	//		}
	//
	//		/* check if output path exists */
	//		if ( !path_exists(output_path) )
	//		{
	//			puts(err_unable_open_output_path);
	//			return 1;
	//		}
	//	}
	//	else // if output path is not specified in CMCC launch
	//	{
	//		output_path = malloc(sizeof(*output_path)*BUFFER_SIZE);
	//		output_file = malloc(sizeof(*output_file)*BUFFER_SIZE);
	//
	//		output_path = program_path;
	//		strcpy(output_file, output_path);
	//		strcat(output_file, LOGFILE);
	//	}
	//
	//	if ( !out_filename )
	//	{
	//		printf("No output filename specified!\n");
	//		return 1;
	//	}
	//
	//	/* met path specified ? */
	//	if ( !input_met_path )
	//	{
	//		puts(err_met_not_specified);
	//		return 1;
	//	}
	//

	// Import settings.txt file (before logInit(), because I choose output filename from settings)
	error = importSettingsFile(settings_path);
	if ( error )
	{
		Log("Settings File not imported!!\n\n");
		return -1;
	}

	//define output file name in function of model settings
	char strTmp[3], strTmp2[4], strTmp3[3];
	strTmp[0] = '_';
	strTmp[1] = settings->version;
	strTmp[2] = '\0';

	strTmp2[0] = '_';
	strTmp2[1] = settings->spatial;
	strTmp2[2] = '_';
	strTmp2[3] = '\0';

	strTmp3[0] = settings->time;
	strTmp3[1] = '_';
	strTmp3[2] = '\0';


	strcat (out_filename, strTmp);
	strcat (out_filename, strTmp2);
	strcat (out_filename, strTmp3);



	char strSizeCell[10] = "";
	sprintf(strSizeCell, "%d", (int)settings->sizeCell);

	strcat (out_filename, strSizeCell);
	strcat (out_filename, "_");

	//add data to output.txt
	time_t rawtime;
	time (&rawtime);
	data = gmtime(&rawtime);

	char strData[30] = "";
	sprintf(strData, "%d", data->tm_year+1900);
	strcat (out_filename, strData);
	strcat (out_filename, "_");

	//sprintf(strData, "%d", data->tm_mon+1);
	//strcat (out_filename, strData);
	//strcat (out_filename, "_");

	sprintf(strData, "%s", szMonth[data->tm_mon]);
	strcat (out_filename, strData);
	strcat (out_filename, "_");

	sprintf(strData, "%d", data->tm_mday);
	strcat (out_filename, strData);
	strcat (out_filename, "_");


	/*
	//check if daylight savings time
	if (data->tm_isdst == 0)
	{
		sprintf(strData, "%d", data->tm_hour+1);
		strcat (out_filename, strData);
		strcat (out_filename, ":");
		sprintf(strData, "%d", data->tm_min);
		strcat (out_filename, strData);
	}
	else if (data->tm_isdst > 0)
	{
		sprintf(strData, "%d", data->tm_hour);
		strcat (out_filename, strData);
		strcat (out_filename, ":");
		sprintf(strData, "%d", data->tm_min);
		strcat (out_filename, strData);
	}
	else
	{
		strcat (out_filename, "no_data_available");
	}
	*/




	//sprintf(strData, "%d", data->tm_sec);
	//strcat (out_filename, strData);

	strcat (out_filename, ".txt");



	if ( !logInit(out_filename) )
	{
		log_enabled = 0;
		puts("Unable to log to file: check logfile path!");
	}

	/* show copyright*/
	Log(copyright);

	/* show banner */
	Log(banner);

	/* show paths */
	printf(msg_dataset_path, input_path);
	printf(msg_site_path, site_path);
	printf(msg_settings_path, settings_path);
	printf(msg_output_file, output_file);

	/* get files */
	files_founded = get_files(program_path, input_path, &files_founded_count, &error);
	if ( error )
	{
		Log("Error reading input files!\n\n");

		return 1;
	}


	/* reset */
	files_processed_count = 0;
	files_not_processed_count = 0;
	total_files_count = 0;

	// Import site.txt file
	error = importSiteFile(site_path);
	if ( error )
	{
		Log("Site File not imported!!\n\n");
		return -1;
	}



	/* loop for searching file */
	for ( i = 0; i < files_founded_count; i++)
	{
		/* inc */
		++total_files_count;

		/* processing */
		printf(msg_processing, files_founded[i].list[0].name);

		/* import dataset */
		rows = import_dataset(files_founded[i].list[0].fullpath, &rows_count);
		if ( !rows )
		{
			++files_not_processed_count;
			continue;
		}
		puts(msg_ok);

		/* build matrix */
		m = matrix_create(rows, rows_count, input_dir);

		/* free rows */
		free(rows);

		/* check matrix */
		if ( !m )
		{
			Log("Matrix not created!!\n\n");
			return 1;
		}

		// import Years Of Simulation (years met files)
		yos = ImportYosFiles(input_met_path, &years_of_simulation);
		if ( !yos )
		{
			Log("Met File not imported!!\n\n");
			matrix_free(m);
			return -1;
		}



		Log("out_filename = %s\n", out_filename);

		Log("\n3D-CMCC MODEL START\n");
		Log("***************************************************\n");

		for (years = 0; years < years_of_simulation; years++)
		{
			matrix_summary (m, years, yos);

			Log("\n-YEARS OF SIMULATION = %d\n", yos[years].year);

			if (settings->time == 'm')
			{
				//check if soil data are available
				for ( cell = 0; cell < m->cells_count; cell++)
				{
					if ((site->sand_perc == -999.0) ||
						(site->clay_perc == -999.0) ||
						(site->silt_perc == -999.0) ||
						(site->bulk_dens == -999.0) ||
						(site->soil_depth == -999.0) )
					{
						Log("NO SOIL DATA AVAILABLE\n");
						return 0;
					}
				}
				//run for all cells to check land use
				for ( cell = 0; cell < m->cells_count; cell++)
				{
					//run for forests
					if (m->cells[cell].landuse == F)
					{
						Log("RUN FOR FORESTS\n");

						//control version 's' or 'u' and change if asked
						if (settings->spatial == 's' && yos[years].year >= (int)(settings->switchtounspatial))
						{
							settings->version = 'u';
							Log("--Years to switch from s to u = %g\n\n\n\n\n", settings->switchtounspatial);
							Log("\n\n\n************CHANGING VERSION..........***************\n");
							Log("year %d...changing version from spatial to unspatial\n", yos[years].year);
							Log("Model version = %c\n\n\n\n", settings->version);
							Log("Model spatial = %c\n\n\n\n", settings->spatial);
							Log("************************************************************\n");
						}

						/*compute number of vegetative months*/
						for (month = 0; month < MONTHS; month++)
						{
							Get_Veg_Months (&m->cells[cell], yos, month, years);
						}
						//run tree_model_M
						for (month = 0; month < MONTHS; month++)
						{
							if ( !tree_model (m, yos, years, month, years_of_simulation) )
							{
								Log("tree model failed.");
							}
							else
							{
								puts(msg_ok);
								//look if put it here or move before tree_model  at the beginning of each month simulation
								//currently soil_model uses equals values for all cells
								//a struct is anyway defined in types.h for soil data
								//soil_model (m, yos, years, month, years_of_simulation);
							}
						}
					}
					//run for crops
					if  (m->cells[cell].landuse == Z)
					{
						Log("RUN FOR CROPS\n");
						//run tree_model
						for (month = 0; month < MONTHS; month++)
						{
							if (!crop_model_M (m, yos, years, month, years_of_simulation) )
							{
								Log("crop model failed.");
							}
							else
							{
								puts(msg_ok);
								//look if put it here or move before tree_model  at the beginning of each month simulation
								//currently soil_model uses equals values for all cells
								//a struct is anyway defined in types.h for soil data
								//soil_model (m, yos, years, month, years_of_simulation);
							}
						}
					}
				}
				Log("****************END OF MONTH*******************\n\n\n\n\n\n\n\n\n\n\n\n\n");
			}
			else if (settings->time == 'd')//run for daily version
			{
				//check if soil data are available
				for ( cell = 0; cell < m->cells_count; cell++)
				{
					if ((site->sand_perc == -999.0) ||
						(site->clay_perc == -999.0) ||
						(site->silt_perc == -999.0) ||
						(site->bulk_dens == -999.0) ||
						(site->soil_depth == -999.0) )
					{
						Log("NO SOIL DATA AVAILABLE\n");
						return 0;
					}
				}
				//run for all cells to check land use
				for ( cell = 0; cell < m->cells_count; cell++)
				{
					//compute days of veg
					for (month = 0; month < MONTHS; month++)
					{
						for (day = 0; day < DaysInMonth[month]; day++)
						{
							GetDayLength (&m->cells[cell], day, month, years, MonthLength[month], yos);
							//GetDayLength_3PG (&m->cells[cell], met, month, day);
							Get_Veg_Days (&m->cells[cell], yos, day, month, years, MonthLength[month], DaysInMonth[month]);
						}
					}
					for (month = 0; month < MONTHS; month++)
					{
						for (day = 0; day < DaysInMonth[month]; day++ )
						{
							if(m->cells[cell].landuse == F)
							{
								if (settings->version == 'f')
								{
									//run for FEM version
									if ( !tree_model_daily (m, yos, years, month, day, years_of_simulation) )
									{
										Log("tree model daily failed.");
									}
									else
									{
										puts(msg_ok);
										//run for SOIL function
										soil_model_daily (m, yos, years, month, years_of_simulation);
									}


								}
								else
								{
									//run for BGC version

								}
							}
							if(m->cells[cell].landuse == Z)
							{

								//if ( !crop_model_D (m, yos, years, month, years_of_simulation) )
								//{
								//	Log("crop model failed.");
								//}
								//else
								//{
								//	puts(msg_ok);
								//look if put it here or move before tree_model  at the beginning of each month simulation
								//	soil_model (m, yos, years, month, years_of_simulation);
								//}

							}
							Log("****************END OF DAY*******************\n");
						}
						Log("****************END OF MONTH*******************\n");
					}
				}
			}
			else
			{
				Log("NO TIME STEP CHOICED!!!\n");
			}

			Log("****************END OF YEAR (%d)*******************\n", yos[years].year);

			Log("*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n");
		}

		/* free memory */
		free(yos);
		matrix_free(m);

		/* increment processed files count */
		++files_processed_count;
	}

	/* summary */
	printf(	msg_summary,
			total_files_count,
			total_files_count > 1 ? "s" : "",
					files_processed_count,
					files_not_processed_count );

	logClose();

	// Free memory
	free(output_file);

	/* free memory at exit */
	return 0;
}
