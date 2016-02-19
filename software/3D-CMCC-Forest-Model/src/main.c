/* main.c */



//TODO SIMULAZIONI DA bareground
//TODO RENDERE DINAMICO IL TREE_LAYER_LIMIT (all'aumentare del dbh aumenta il TREE_LAYER_LIMIT) see Montgomery & Chazdon, 2001)
//TODO INSERIRE UN MAX NUMBER OF LAYER IN FUNZIONE DEL TREE HEIGHT (chiedere Arianna)
//TODO NETCDF
//TODO VERSION FEM AND BGC
//TODO RIMETTERE PER L'ACQUA COMPETIZIONE SIMMETRICA




/* includes */
#ifdef _WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

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
#include "compiler.h"
#include "types.h"
#include "constants.h"
#include "netcdf/netcdf.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

/* constants */
#define PROGRAM_VERSION	"5.1"

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

//Last cumulative days in months
int MonthLength [] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 364};

//Days in Months
int DaysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *szMonth[MONTHS] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
		"AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };


/* global variables */
char 	*program_path		=	NULL,	// mandatory
		*input_dir			=	NULL,	// mandatory
		*input_path		=	NULL,	// mandatory
		*dataset_filename	=	NULL,	// mandatory
		*input_met_path	=	NULL,	// mandatory
		*site_path			=	NULL,	// mandatory
		*output_path		=	NULL,	// mandatory
		*out_filename		=	NULL,	// mandatory
		*output_file		= 	NULL,	// mandatory
		*daily_output_path		=	NULL,	// mandatory
		*daily_out_filename		=	NULL,	// mandatory
		*daily_output_file		= 	NULL,	// mandatory
		*monthly_output_path		=	NULL,	// mandatory
		*monthly_out_filename		=	NULL,	// mandatory
		*monthly_output_file		= 	NULL,	// mandatory
		*annual_output_path		=	NULL,	// mandatory
		*annual_out_filename		=	NULL,	// mandatory
		*annual_output_file		= 	NULL,	// mandatory
		*soil_output_path		= NULL, 	//mandatory
		*soil_out_filename		=	NULL,	// mandatory
		*soil_output_file		= 	NULL,	// mandatory
		*settings_path		=	NULL;	// mandatory
//*resolution		= 	NULL,	// mandatory
//*vers_arg			= 	NULL;	// mandatory

int log_enabled		=	1,	// default is on
		years_of_simulation	=	0;	// default is none

/* static global variables */
static FILES *files_founded;
static int files_founded_count;

/* strings */
static const char banner[] =	"\n\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n\n"
		"by Alessio Collalti - for contacts: a dot collalti at unitus dot it; alessio dot collalti at cmcc dot it\n"
		"using NetCDF %s\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"(use -h parameter for more information)\n\n\n";

static char copyright[] =
		"\n\n\n\n"
		"This version of 3D-CMCC Forest Ecosystem Model has been developed by:\n"
		"Alessio Collalti [a.collalti@unitus.it - alessio.collalti@cmcc.it],\n"
		"Alessio Ribeca,\n"
		"Sergio Marconi [sergio.marconi@cmcc.it] \n"
		"euroMediterranean Center on Climate Changes (CMCC),\n"
		"IAFENT division,\n"
		"Via A. Pacinotti 5, 01100 - Viterbo, Italy,\n"
		"and \nUniversity of Tuscia ,\n"
		"Department for innovation in biological, agro-food and forest systems (DIBAF),\n"
		"Forest Ecology Lab\n"
		"MEEO S.r.l.\n"
		"Via Saragat 9, 44122 Ferrara, Italy\n"
		"\n"
		"Programmers: Alessio Collalti - Alessio Ribeca - Sergio Marconi\n"
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
		"for more information see Collalti et al., 2014 Ecological Modelling \n"
		"\n";


static const char msg_dataset_path[]			=	"dataset path = %s\n";
static const char msg_site_path[]				=	"site path = %s\n";
static const char msg_met_path[]				=	"met path = %s\n";
static const char msg_settings_path[]			=	"settings path = %s\n";
static const char msg_output_file[]				=	"output file path = %s\n";
static const char msg_daily_output_file[]		=	"daily output file path = %s\n";
static const char msg_monthly_output_file[]		=	"monthly output file path = %s\n";
static const char msg_annual_output_file[]		=	"annual output file path = %s\n";
static const char msg_soil_output_file[]		=	"soil output file path = %s\n";
static const char msg_processing[]				=	"processing %s...\n";
static const char msg_ok[]						=	"ok";
static const char msg_summary[]					=	"\n%d input file%s found: %d processed, %d skipped.\n\n";
static const char msg_usage[]					=	"usage: 3D-CMCC parameters\n\n"
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
static const char err_dataset_already_specified[] = "dataset already specified (%s)! \"%s\" skipped.\n";
static const char err_site_already_specified[] = "site already specified (%s)! \"%s\" skipped.\n";
static const char err_settings_already_specified[] = "settings already specified (%s)! \"%s\" skipped.\n";
static const char err_met_already_specified[] = "met already specified (%s)! \"%s\" skipped.\n";
static const char err_output_already_specified[] = "output path already specified (%s)! \"%s\" skipped.\n";
static const char err_outname_already_specified[] = "output filename specified without output path (%s)! \"%s\" skipped.\n";
static const char err_arg_needs_param[] = "%s parameter not specified.\n\n";
static const char err_arg_no_needs_param[] = "%s no needs parameter.\n\n";
static const char err_unable_convert_value_arg[] = "unable to convert value \"%s\" for %s.\n\n";





static void clean_up(void)
{
	if ( site )
		free(site);

	if( settings )
		free(settings);

	if (program_path)
		free(program_path);

	if ( files_founded ) {
		free_files(files_founded, files_founded_count);
	}
	
#ifdef _WIN32
#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	_CrtDumpMemoryLeaks();
#endif
#endif
}

//int get_input_path(char *arg, char *param, void *p) {
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( input_path )
//	{
//		printf(err_dataset_already_specified, input_path, param);
//	}
//	else
//	{
//		input_path = param;
//	}
//
//	/* ok */
//	return 1;
//}

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

//int get_output_filename(char *arg, char *param, void *p)
//{
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( out_filename )
//		printf(err_outname_already_specified, out_filename, param);
//	else
//	{
//		out_filename = param;
//
//		if( output_path )
//			strcat(output_file, out_filename);
//		else
//			printf("With -outname flag set -outpath not set: using default output file (prog_path/output.txt)");
//	}
//	Log("output file name = %s\n", out_filename);
//	return 1;
//}

int get_daily_output_filename(char *arg, char *param, void *p)
{
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if (daily_out_filename )
		printf(err_outname_already_specified, daily_out_filename, param);
	else
	{
		daily_out_filename = param;

		if( output_path )
			strcat(daily_output_file, daily_out_filename);
		else
			printf("With -daily_outname flag set -monthly_outpath not set: using default output file (prog_path/daily_output.txt)");
	}
	Log("daily_output file name = %s\n", daily_out_filename);
	return 1;
}

int get_monthly_output_filename(char *arg, char *param, void *p)
{
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if (monthly_out_filename )
		printf(err_outname_already_specified, monthly_out_filename, param);
	else
	{
		monthly_out_filename = param;

		if( output_path )
			strcat(monthly_output_file, monthly_out_filename);
		else
			printf("With -monthly_outname flag set -monthly_outpath not set: using default output file (prog_path/monthly_output.txt)");
	}
	Log("monthly_output file name = %s\n", monthly_out_filename);
	return 1;
}

int get_annual_output_filename(char *arg, char *param, void *p)
{
	if ( !param )
	{
		printf(err_arg_needs_param, arg);
		return 0;
	}

	if ( annual_out_filename )
		printf(err_outname_already_specified, annual_out_filename, param);
	else
	{
		annual_out_filename = param;

		if( output_path )
			strcat(annual_output_file, annual_out_filename);
		else
			printf("With -annual_outname flag set -anual_outpath not set: using default output file (prog_path/annual_output.txt)");
	}
	Log("annual_output file name = %s\n", annual_out_filename);
	return 1;
}
//int get_soil_output_filename(char *arg, char *param, void *p)
//{
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( soil_out_filename )
//		printf(err_outname_already_specified, soil_out_filename, param);
//	else
//	{
//		soil_out_filename = param;
//
//		if( output_path )
//			strcat(soil_output_file, soil_out_filename);
//		else
//			printf("With -soil_outname flag set -soil_outpath not set: using default output file (prog_path/soil_output.txt)");
//	}
//	Log("soil_output file name = %s\n", soil_out_filename);
//	return 1;
//}

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
	fprintf(stderr, "\t-b\tdaily output filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stderr, "\t-e\tmonthly output filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stderr, "\t-f\tannual output filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stderr, "\t-d\tdataset filename stored into input directory\t(i.e.: -d input.txt)\n");
	fprintf(stderr, "\t-m\tmet filename list stored into input directory\t(i.e.: -m 1999.txt,2003.txt,2009.txt)\n");
	fprintf(stderr, "\t-s\tsite filename stored into input directory\t(i.e.: -s site.txt)\n");
	fprintf(stderr, "\t-c\tsettings filename stored into input directory\t(i.e.: -c settings.txt)\n");
	fprintf(stderr, "\nOptional options:\n");
	fprintf(stderr, "\t-h\tprint this help\n");
	fprintf(stderr, "\nLaunch example:\n");
	fprintf(stderr, "\t./3D-CMCC Model -i /path/to/input/directory/ -o /path/to/CMCC.log -f /path/to/CMCC.log -e /path/to/CMCC.log -d /path/to/input.txt -m /path/to/1999.txt,/path/to/2003.txt,/path/to/2009.txt -s /path/to/site.txt -c /path/to/settings.txt\n");
	exit(1);
}

void rows_free(ROW *rows, int rows_count ) {
	int i;

	if ( rows_count  ) {
		for ( i = 0; i < rows_count; ++i ) {
			free(rows[i].species);
		}
		free(rows);
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
	files_processed_count,
	files_not_processed_count,
	total_files_count;

	struct tm* data;

	int years,
	month,
	day;

	int cell;

	char strSitename[20] = "";
	char strTmp[3], strTmp2[4], strTmp3[3];
	char strSizeCell[10] = "";
	char strData[30] = "";



	YOS *yos;
	ROW *rows;
	MATRIX *m;
	time_t rawtime;

	// ALESSIOR
	// this vars are declared in types.h
	// so we need to initialized here
	site = NULL;
	settings = NULL;

	/* register atexit */
	if ( -1 == atexit(clean_up) )
	{
		puts(err_unable_to_register_atexit);
		return 1;
	}

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
		case 'b': // Daily Output file name (with path)
			daily_out_filename = malloc(sizeof(*daily_out_filename)*BUFFER_SIZE);
			if( !daily_out_filename )
			{
				fprintf(stderr, "Cannot allocate memory for daily_out_filename.\n");
				return 1;
			}
			bzero(daily_out_filename, BUFFER_SIZE-1);
			strcpy(daily_out_filename, argv[i+1]);
			break;
		case 'f': // Monthly Output file name (with path)
			monthly_out_filename = malloc(sizeof(*monthly_out_filename)*BUFFER_SIZE);
			if( !monthly_out_filename )
			{
				fprintf(stderr, "Cannot allocate memory for monthly_out_filename.\n");
				return 1;
			}
			bzero(monthly_out_filename, BUFFER_SIZE-1);
			strcpy(monthly_out_filename, argv[i+1]);
			break;
		case 'e': // Annual Output file name (with path)
			annual_out_filename = malloc(sizeof(*annual_out_filename)*BUFFER_SIZE);
			if( !annual_out_filename )
			{
				fprintf(stderr, "Cannot allocate memory for annual_out_filename.\n");
				return 1;
			}
			bzero(annual_out_filename, BUFFER_SIZE-1);
			strcpy(annual_out_filename, argv[i+1]);
			break;
		case 'n': // Soil Output file name (with path)
			soil_out_filename = malloc(sizeof(*soil_out_filename)*BUFFER_SIZE);
			if( !soil_out_filename )
			{
				fprintf(stderr, "Cannot allocate memory for soil_out_filename.\n");
				return 1;
			}
			bzero(soil_out_filename, BUFFER_SIZE-1);
			strcpy(soil_out_filename, argv[i+1]);
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

	if(monthly_out_filename == NULL )
	{
		fprintf(stderr, "Error: monthly output filename option is missing!\n");
		usage();
	}
	else
	{
		monthly_output_file = malloc(sizeof(*monthly_output_file)*BUFFER_SIZE);
		if( !monthly_output_file )
		{
			fprintf(stderr, "Cannot allocate memory for monthly output_file.\n");
			return 1;
		}
		bzero(monthly_output_file, BUFFER_SIZE-1);
		strcpy(monthly_output_file, monthly_out_filename);

	}


	if( annual_out_filename == NULL )
	{
		fprintf(stderr, "Error: annual output filename option is missing!\n");
		usage();
	}
	else
	{
		annual_output_file = malloc(sizeof(*annual_output_file)*BUFFER_SIZE);
		if( !annual_output_file )
		{
			fprintf(stderr, "Cannot allocate memory for annual output_file.\n");
			return 1;
		}
		bzero(annual_output_file, BUFFER_SIZE-1);
		strcpy(annual_output_file, annual_out_filename);

	}
	if( soil_out_filename == NULL )
	{
		fprintf(stderr, "Error: soil output filename option is missing!\n");
		usage();
	}
	else
	{
		soil_output_file = malloc(sizeof(*soil_output_file)*BUFFER_SIZE);
		if( !soil_output_file )
		{
			fprintf(stderr, "Cannot allocate memory for annual output_file.\n");
			return 1;
		}
		bzero(soil_output_file, BUFFER_SIZE-1);
		strcpy(soil_output_file, soil_out_filename);

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
		free(dataset_filename); dataset_filename = NULL;
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


	/* get program path */
	program_path = get_current_directory();
	if ( !program_path )
	{
		puts(err_unable_get_current_directory);
		return 1;
	}


	// Import settings.txt file (before logInit(), because I choose output filename from settings)
	error = importSettingsFile(settings_path);
	if ( error )
	{
		Log("Settings File not imported!!\n\n");
		return -1;
	}
	else
	{
		printf ("...Settings file imported!!\n");
	}

	// Import site.txt file
	error = importSiteFile(site_path);
	if ( error )
	{
		Log("Site file not imported!!\n\n");
		return -1;
	}
	else
	{
		Log("site path = %s\n", site_path);
		Log("...Site file imported!!\n\n");
	}

	//add site name to output files
	
	sprintf(strSitename, "%s", site->sitename);

	strcat (out_filename, "_");
	strcat (out_filename, strSitename);

	strcat (daily_out_filename, "_");
	strcat (daily_out_filename, strSitename);

	strcat (monthly_out_filename, "_");
	strcat (monthly_out_filename, strSitename);

	strcat (annual_out_filename, "_");
	strcat (annual_out_filename, strSitename);

	strcat (soil_out_filename, "_");
	strcat (soil_out_filename, strSitename);



	//define output file name in function of model settings
	
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

	strcat (daily_out_filename, strTmp);
	strcat (daily_out_filename, strTmp2);
	strcat (daily_out_filename, strTmp3);

	strcat (monthly_out_filename, strTmp);
	strcat (monthly_out_filename, strTmp2);
	strcat (monthly_out_filename, strTmp3);

	strcat (annual_out_filename, strTmp);
	strcat (annual_out_filename, strTmp2);
	strcat (annual_out_filename, strTmp3);

	strcat (soil_out_filename, strTmp);
	strcat (soil_out_filename, strTmp2);
	strcat (soil_out_filename, strTmp3);

	
	sprintf(strSizeCell, "%d", (int)settings->sizeCell);

	strcat (out_filename, strSizeCell);
	strcat (out_filename, "_");

	strcat (daily_out_filename, strSizeCell);
	strcat (daily_out_filename, "_");

	strcat (monthly_out_filename, strSizeCell);
	strcat (monthly_out_filename, "_");

	strcat (annual_out_filename, strSizeCell);
	strcat (annual_out_filename, "_");

	strcat (soil_out_filename, strSizeCell);
	strcat (soil_out_filename, "_");

	//add data to output.txt

	time (&rawtime);
	data = gmtime(&rawtime);

	
	sprintf(strData, "%d", data->tm_year+1900);
	strcat (out_filename, strData);
	strcat (out_filename, "_");

	strcat (daily_out_filename, strData);
	strcat (daily_out_filename, "_");

	strcat (monthly_out_filename, strData);
	strcat (monthly_out_filename, "_");

	strcat (annual_out_filename, strData);
	strcat (annual_out_filename, "_");

	strcat (soil_out_filename, strData);
	strcat (soil_out_filename, "_");

	sprintf(strData, "%s", szMonth[data->tm_mon]);
	strcat (out_filename, strData);
	strcat (out_filename, "_");

	strcat (daily_out_filename, strData);
	strcat (daily_out_filename, "_");

	strcat (monthly_out_filename, strData);
	strcat (monthly_out_filename, "_");

	strcat (annual_out_filename, strData);
	strcat (annual_out_filename, "_");

	strcat (soil_out_filename, strData);
	strcat (soil_out_filename, "_");

	sprintf(strData, "%d", data->tm_mday);

	strcat (out_filename, strData);

	strcat (daily_out_filename, strData);
	strcat (monthly_out_filename, strData);
	strcat (annual_out_filename, strData);
	strcat (soil_out_filename, strData);

	if (!mystricmp(settings->dndc, "on"))
	{
		strcat (out_filename, "_");
		strcat (daily_out_filename, "_");
		strcat (monthly_out_filename, "_");
		strcat (annual_out_filename, "_");
		strcat (soil_out_filename, "_");
		sprintf(strData, "DNDC");
		strcat (daily_out_filename, strData);
		strcat (monthly_out_filename, strData);
		strcat (annual_out_filename, strData);
		strcat (soil_out_filename, strData);
		strcat (out_filename, strData);

	}
	if (!mystricmp(settings->rothC, "on"))
	{
		strcat (out_filename, "_");
		strcat (daily_out_filename, "_");
		strcat (monthly_out_filename, "_");
		strcat (annual_out_filename, "_");
		strcat (soil_out_filename, "_");
		sprintf(strData, "RothC");
		strcat (daily_out_filename, strData);
		strcat (monthly_out_filename, strData);
		strcat (annual_out_filename, strData);
		strcat (soil_out_filename, strData);
		strcat (out_filename, strData);

	}

	/* add suffix for type files */
	{
		char *p;
		char *ext;
		p = strrchr(input_met_path, '.');
		if ( ! p ) {
			puts("bad met file!\n");
			return -1;
		}
		++p;
		if ( ! mystricmp(p, "nc") ) {
			ext = "_nc";
		} else if ( ! mystricmp(p, "lst") ) {
			ext = "_lst";
		} else if ( ! mystricmp(p, "txt") ) {
			ext = "_txt";
		} else {
			puts("bad met file!\n");
			return -1;
		}
		strcat(out_filename, ext);
		strcat(daily_out_filename, ext);
		strcat(monthly_out_filename, ext);
		strcat(annual_out_filename, ext);
		strcat(soil_out_filename, ext);
	}

	strcat (out_filename, ".txt");
	strcat (daily_out_filename, ".txt");
	strcat (monthly_out_filename, ".txt");
	strcat (annual_out_filename, ".txt");
	strcat (soil_out_filename, ".txt");


	//Create output files
	if ( !logInit(out_filename) )
	{
		log_enabled = 0;
		puts("Unable to log to file: check logfile path!");
	}
	free(out_filename); out_filename = NULL;
	daily_logInit (daily_out_filename);
	Daily_Log ("daily output file at cell level\n\n");
	free(daily_out_filename); daily_out_filename = NULL;

	monthly_logInit (monthly_out_filename);
	Monthly_Log ("monthly output file at cell level\n\n");
	free(monthly_out_filename); monthly_out_filename = NULL;

	annual_logInit (annual_out_filename);
	Annual_Log ("annual output file at cell level\n\n");
	free(annual_out_filename); annual_out_filename = NULL;

	soil_logInit (soil_out_filename);
	free(soil_out_filename);
	soil_out_filename = NULL;
	soil_Log ("soil output file at cell level\n\n");
	/* show copyright*/
	Log(copyright);

	/* show banner */
	Log(banner, nc_inq_libvers());

	/* show paths */
	printf(msg_dataset_path, input_path);
	printf(msg_site_path, site_path);
	printf(msg_met_path, input_met_path);
	printf(msg_settings_path, settings_path);
	printf(msg_output_file, output_file);
	printf(msg_daily_output_file, annual_output_file);
	printf(msg_monthly_output_file, annual_output_file);
	printf(msg_annual_output_file, annual_output_file);
	printf(msg_soil_output_file, soil_output_file);

	free(site_path); site_path = NULL;
	free(settings_path); settings_path = NULL;

	/* get files */
	files_founded = get_files(program_path, input_path, &files_founded_count, &error);
	if ( error )
	{
		Log("Error reading input files!\n\n");
		free(input_path);
		return 1;
	}
	Log("input path = %s\n", input_path);
	Log("...input file imported\n\n");
	free(input_path);
	input_path = NULL;

	/* reset */
	files_processed_count = 0;
	files_not_processed_count = 0;
	total_files_count = 0;
	/*
	// Import site.txt file
	error = importSiteFile(site_path);
	if ( error )
	{
		Log("Site file not imported!!\n\n");
		return -1;
	}
	else
	{
		Log("site path = %s\n", site_path);
		Log("...Site file imported!!\n\n");
	}
	 */
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
		free(input_dir); input_dir = NULL;

		/* free rows */
		rows_free(rows, rows_count);

		/* check matrix */
		if ( !m )
		{
			Log("Matrix not created!!\n\n");
			return 1;
		}
		else
		{
			Log("....Matrix created!!\n\n");
		}

		// import Years Of Simulation (years met files)
		Log("Processing met data files...\n");
		Log("input_met_path = %s\n", input_met_path);
		yos = ImportYosFiles(input_met_path, &years_of_simulation);

		if ( !yos )
		{
			Log("Met File %s not imported !!\n", input_met_path);
			matrix_free(m);
			return -1;
		}
		else
		{
			Log("....Met files imported!\n\n");
		}

		Log("\n3D-CMCC MODEL START....\n\n\n\n");
		Log("Total years_of_simulation = %d\n", years_of_simulation);
		Log("***************************************************\n");

		for (years = 0; years < years_of_simulation; years++)
		{
			Log("\n-Year simulated = %d\n", yos[years].year);

			if (!years)
				matrix_summary (m, years, yos);


			if (settings->time == 'd')
			{
				for ( cell = 0; cell < m->cells_count; cell++)
				{
					if (	IS_INVALID_VALUE(site->sand_perc)
							|| IS_INVALID_VALUE(site->clay_perc)
							|| IS_INVALID_VALUE(site->silt_perc)
							|| IS_INVALID_VALUE(site->bulk_dens)
							|| IS_INVALID_VALUE(site->soil_depth) )
					{
						Log("NO SOIL DATA AVAILABLE\n");
						//exit
						return 0;
					}

					//Get air pressure
					Get_air_pressure (&m->cells[cell]);
					//check hemisphere
					if (site->lat > 0)
					{
						m->cells[cell].north = 0;
					}
					else
					{
						m->cells[cell].north = 1;
					}
				}
				//run for all cells to check land use
				for ( cell = 0; cell < m->cells_count; cell++)
				{
					//Marconi: the variable i needs to be a for private variable, used to fill the vpsat vector v(365;1)
					int i;
					// ALESSIOR for handling leap years
					int days_per_month;
					i =0;
					for (month = 0; month < MONTHS; month++)
					{
						days_per_month = DaysInMonth[month];
						if ( (FEBRUARY == month) && IS_LEAP_YEAR(yos[years].year) )
						{
							++days_per_month;
						}

						for (day = 0; day < days_per_month; day++)
						{
							//Check for daily temperatures
							Get_avg_temperature (&m->cells[cell], day, month, years, yos);
							Get_daylight_avg_temperature (&m->cells[cell], day, month, years, yos);
							Get_nightime_avg_temperature (&m->cells[cell], day, month, years, yos);
							Get_soil_temperature (&m->cells[cell], day, month, years, yos);
							//for RothC
							//Get_avg_monthly_temp (&m->cells[cell], day, month, years, days_per_month, yos);
							//Get_cum_monthly_rain (&m->cells[cell], day, month, years, days_per_month, yos);

							//
							Get_thermic_sum (&m->cells[cell], day, month, years, yos);
							Get_rho_air (&m->cells[cell], day, month, years, yos);
							//Get day length
							Get_Day_Length (&m->cells[cell], day, month, years, yos);
							//GetDayLength_3PG (&m->cells[cell], day, month, years, MonthLength[month], yos);

							if(m->cells[cell].landuse == F)
							{
								//Get vegetative days
								Get_Veg_Days (&m->cells[cell], yos, day, month, years, days_per_month);
								//Marconi 18/06: function used to calculate VPsat from Tsoil following Hashimoto et al., 2011
								get_vpsat(&m->cells[cell], day, month, years, yos, i);
								i++;
							}
							else if (m->cells[cell].landuse == Z)
							{
								//sergio
							}

						}
						/*
						for (day = 0; day < days_per_month; day++)
						{
							Print_met_daily_data (yos, day, month, years);
						}
						 */
					}
					for (month = 0; month < MONTHS; month++)
					{
						days_per_month = DaysInMonth[month];
						if ( (FEBRUARY == month) && IS_LEAP_YEAR(yos[years].year) )
						{
							++days_per_month;
						}
						for (day = 0; day < days_per_month; day++ )
						{
							if(m->cells[cell].landuse == F)
							{
								if (settings->version == 'f')
								{
									//Marconi: 18/06: fitting vpSat on gaussian curve to asses peak value (parameter b1)
									//if(day == 0 && month == 0) leaffalMarconi(&m->cells[cell]);
									//run for FEM version
									if (!tree_model_daily (m, yos, years, month, day, years_of_simulation) )
									{
										Log("tree model daily failed.");
									}
									else
									{
										puts(msg_ok);
										//										soil_Log("\nsoilLog prova");
										//run for SOIL functions
										//soil_model (m, yos, years, month, day, years_of_simulation);

										if (!mystricmp(settings->dndc, "on"))
										{
											Log("RUNNING DNDC.....\n");
											//TODO SERGIO
											soil_dndc_sgm (m, yos, years, month, day, years_of_simulation);
											//soil_dndc......................................
										}
										//else if (!mystricmp(settings->rothC, "on"))
										//{
										//	Log("RUNNING ROTHC.....\n");
										//	//TODO SERGIO
										//	soil_rothC (m, yos, years, month, day, years_of_simulation);
										//}
										else
										{
											Log("No soil simulation!!!\n");
										}
										get_net_ecosystem_exchange(&m->cells[cell]);
									}
								}
								else
								{
									//run for BGC version
								}
							}
							if(m->cells[cell].landuse == Z)
							{
								if ( !crop_model_D (m, yos, years, month, day, years_of_simulation) )
								{
									Log("crop model failed.");
								}
								else
								{
									puts(msg_ok);
									soil_Log("\nsoilLog prova");
									//look if put it here or move before tree_model  at the beginning of each month simulation
									//	soil_model (m, yos, years, month, years_of_simulation);
								}
							}
							Log("****************END OF DAY (%d)*******************\n\n\n", day+1);
							Get_EOD_cumulative_balance_cell_level (&m->cells[cell], yos, years, month, day);
							if (!mystricmp(settings->dndc, "on"))
							{
								Get_EOD_soil_balance_cell_level (&m->cells[cell], yos, years, month, day);
							}

						}
						Log("****************END OF MONTH (%d)*******************\n", month+1);
						if (!mystricmp(settings->rothC, "on"))
						{
							Get_EOD_soil_balance_cell_level (&m->cells[cell], yos, years, month, day);
						}
						Get_EOM_cumulative_balance_cell_level (&m->cells[cell], yos, years, month);
					}
					Log("****************END OF YEAR (%d)*******************\n\n\n\n\n\n\n\n\n\n\n", yos[years].year);
					Get_EOY_cumulative_balance_cell_level (&m->cells[cell], yos, years, years_of_simulation);

				}
			}
			//else if (settings->time == 'm')
			//{
			//	for ( cell = 0; cell < m->cells_count; cell++)
			//	{
			//		//Get air pressure
			//		Get_air_pressure (&m->cells[cell]);

			//		if (	IS_INVALID_VALUE(site->sand_perc)
			//				|| IS_INVALID_VALUE(site->clay_perc)
			//				|| IS_INVALID_VALUE(site->silt_perc)
			//				|| IS_INVALID_VALUE(site->bulk_dens)
			//				|| IS_INVALID_VALUE(site->soil_depth) )
			//		{
			//			Log("NO SOIL DATA AVAILABLE\n");
			//			return 0;
			//		}
			//		//check hemisphere
			//		if (site->lat > 0)
			//		{
			//			m->cells[cell].north = 0;
			//		}
			//		else
			//		{
			//			m->cells[cell].north = 1;
			//		}

			//		//run for all cells to check land use
			//		for ( cell = 0; cell < m->cells_count; cell++)
			//		{
			//			//Get air pressure
			//			Get_air_pressure (&m->cells[cell]);

			//			//run for forests
			//			if (m->cells[cell].landuse == F)
			//			{
			//				Log("RUN FOR FORESTS\n");

			//				//control version 's' or 'u' and change if asked
			//				if (settings->spatial == 's' && yos[years].year >= (int)(settings->switchtounspatial))
			//				{
			//					settings->version = 'u';
			//					Log("--Years to switch from s to u = %f\n\n\n\n\n", settings->switchtounspatial);
			//					Log("\n\n\n************CHANGING VERSION..........***************\n");
			//					Log("year %d...changing version from spatial to unspatial\n", yos[years].year);
			//					Log("Model version = %c\n\n\n\n", settings->version);
			//					Log("Model spatial = %c\n\n\n\n", settings->spatial);
			//					Log("************************************************************\n");
			//				}

			//				//compute number of vegetative months
			//				for (month = 0; month < MONTHS; month++)
			//				{
			//					//Check for temperatures
			//					Get_avg_temperature (&m->cells[cell], day, month, years, yos);
			//					Get_daylight_avg_temperature (&m->cells[cell], day, month, years, yos);
			//					Get_nightime_avg_temperature (&m->cells[cell], day, month, years, yos);

			//					//todo add Get_thermic_sum if used

			//					//Get vegetative months
			//					Get_Veg_Months (&m->cells[cell], yos, month, years);
			//				}
			//				//run tree_model_M
			//				for (month = 0; month < MONTHS; month++)
			//				{
			//					if ( !tree_model (m, yos, years, month, years_of_simulation) )
			//					{
			//						Log("tree model failed.");
			//					}
			//					else
			//					{
			//						puts(msg_ok);
			//						//look if put it here or move before tree_model  at the beginning of each month simulation
			//						//currently soil_model uses equals values for all cells
			//						//a struct is anyway defined in types.h for soil data
			//						//soil_model (m, yos, years, month, years_of_simulation);
			//					}
			//				}
			//			}
			//			//run for crops
			//			if  (m->cells[cell].landuse == Z)
			//			{
			//				Log("RUN FOR CROPS\n");
			//				//run tree_model
			//				for (month = 0; month < MONTHS; month++)
			//				{
			//					if (!crop_model_M (m, yos, years, month, years_of_simulation) )
			//					{
			//						Log("crop model failed.");
			//					}
			//					else
			//					{
			//						puts(msg_ok);
			//						//look if put it here or move before tree_model  at the beginning of each month simulation
			//						//currently soil_model uses equals values for all cells
			//						//a struct is anyway defined in types.h for soil data
			//						//soil_model (m, yos, years, month, years_of_simulation);
			//					}
			//				}
			//			}
			//		}
			//		Log("****************END OF MONTH*******************\n\n\n\n\n\n\n\n\n\n\n\n\n");
			//	}
			//}
			else
			{
				Log("UNCORRECT TIME STEP CHOICED!!!\n");
				return 0;
			}


			Log("...%d finished to simulate\n\n\n\n\n\n", yos[years].year);
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
	daily_logClose();
	monthly_logClose();
	annual_logClose();
	soil_logClose();

	// Free memory
	free(input_met_path); input_met_path = NULL;
	free(output_file); output_file = NULL;
	free(daily_output_file); daily_output_file = NULL;
	free(monthly_output_file); monthly_output_file = NULL;
	free(annual_output_file); annual_output_file = NULL;
	free(soil_output_file); soil_output_file = NULL;

	/* free memory at exit */
	return 0;
}
