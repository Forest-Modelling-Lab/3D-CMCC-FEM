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
#include "soil.h"
#include "cumulative_balance.h"
#include "types.h"
#include "constants.h"
#include "topo.h"
#include "logger.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

/* constants */
#define PROGRAM_VERSION	"5.1.1"

//Last cumulative days in months
int MonthLength [] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 364};

//Days in Months
int DaysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *szMonth[MONTHS] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
		"AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };

/* global vars */
logger_t* g_log = NULL;
logger_t* g_daily_log = NULL;
logger_t* g_monthly_log = NULL;
logger_t* g_annual_log = NULL;
logger_t* g_soil_log = NULL;
soil_t* g_soil = NULL;
topo_t* g_topo = NULL;

/* global variables */
char 	*g_sz_program_path	=	NULL,	// mandatory
		*input_dir			=	NULL,	// mandatory
		*input_path		=	NULL,	// mandatory
		*dataset_filename	=	NULL,	// mandatory
		*input_met_path	=	NULL,	// mandatory
		*soil_path			=	NULL,	// mandatory
		*topo_path			=	NULL,	// mandatory
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
		*settings_path		=	NULL,	// mandatory
		*output_vars_path = NULL;
//*resolution		= 	NULL,	// mandatory
//*vers_arg			= 	NULL;	// mandatory

static int years_of_simulation	=	0;	// default is none

// ALESSIOR: PORCATA fixme
int x_cells_count;
int y_cells_count;

/* global variables */
OUTPUT_VARS *output_vars = NULL;	/* required */

/* strings */
static const char banner[] =	"\n\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n\n"
		"by Alessio Collalti - for contacts: a dot collalti at unitus dot it; alessio dot collalti at cmcc dot it\n"
		"using NetCDF %s\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"(use -h parameter for more information)\n\n\n";

static char copyright[] =
		"\n\n"
		"This version of 3D-CMCC FEM (v.5.1.1) has been developed by:\n"
		"Alessio Collalti [alessio.collalti@cmcc.it, a.collalti@unitus.it],\n"
		"Alessio Ribeca,\n"
		"Sergio Marconi [sergio.marconi@cmcc.it] \n"
		"Monia Santinii [monia.santini@cmcc.it] \n"
		"euroMediterranean Center on Climate Changes (CMCC),\n"
		"IAFES division,\n"
		"Viale Trieste 120, 01100 - Viterbo, Italy,\n"
		"and \nUniversity of Tuscia ,\n"
		"Department for innovation in biological, agro-food and forest systems (DIBAF),\n"
		"Forest Ecology Lab\n"
		"\n"
		"Programmers: Alessio Collalti - Alessio Ribeca - Sergio Marconi - Monia Santini\n"
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
		"for more information see Collalti et al., 2014 Ecological Modelling, \n"
		"Collalti et al., 2016 Geoscientific Model Development \n"
		"\n";


static const char msg_dataset_path[]			=	"dataset path = %s\n";
static const char msg_soil_path[]				=	"soil path = %s\n";
static const char msg_topo_path[]				=	"topo path = %s\n";
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
extern const char err_out_of_memory[];
const char err_unable_open_file[] = "unable to open file.";
const char err_empty_file[] = "empty file ?";
const char err_window_size_too_big[] = "window size too big.";
static const char err_unable_get_current_directory[] = "unable to retrieve current directory.\n";
static const char err_unable_to_register_atexit[] = "unable to register clean-up routine.\n";
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
	if ( input_dir )
		free(input_dir);

	if ( g_topo )
		free(g_topo);

	if ( g_soil )
		free(g_soil);

	if( settings )
		free(settings);

	if (g_sz_program_path)
		free(g_sz_program_path);

#ifdef _WIN32
#ifdef _DEBUG
	/*
		_CrtSetBreakAlloc(X);
	*/
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	_CrtDumpMemoryLeaks();

	// REMOVE THIS
	system("PAUSE");
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

//static int get_soil_path(char *arg, char *param, void *p) {
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( soil_path )
//	{
//		printf(err_site_already_specified, soil_path, param);
//	}
//	else
//	{
//		soil_path = param;
//	}
//
//	/* ok */
//	return 1;
//}
//
//int get_settings_path(char *arg, char *param, void *p) {
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( settings_path )
//	{
//		printf(err_settings_already_specified, settings_path, param);
//	}
//	else
//	{
//		settings_path = param;
//	}
//
//	/* ok */
//	return 1;
//}
//
///* */
//int get_met_path(char *arg, char *param, void *p)
//{
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( input_met_path )
//	{
//		printf(err_met_already_specified, input_met_path, param);
//	}
//	else
//	{
//		input_met_path = param;
//	}
//
//	/* ok */
//	return 1;
//}
//
///* */
//int get_output_path(char *arg, char *param, void *p)
//{
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	if ( output_path )
//		printf(err_output_already_specified, output_path, param);
//	else
//	{
//		output_path = malloc(sizeof(*output_path)*BUFFER_SIZE);
//		output_file = malloc(sizeof(*output_file)*BUFFER_SIZE);
//
//		output_path = param;
//		strcpy(output_file, output_path);
//		//		strcat(output_file, LOGFILE);
//	}
//
//	return 1;
//}

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
//	logger(g_log, "output file name = %s\n", out_filename);
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
	logger(g_log, "daily_output file name = %s\n", daily_out_filename);
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
	logger(g_log, "monthly_output file name = %s\n", monthly_out_filename);
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
	logger(g_log, "annual_output file name = %s\n", annual_out_filename);
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
//	logger(g_log, "soil_output file name = %s\n", soil_out_filename);
//	return 1;
//}

///* */
//int set_prec_value(char *arg, char *param, void *p)
//{
//	int error;
//
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	/* convert param */
//	*(PREC *)p = convert_string_to_prec(param, &error);
//	if ( error )
//	{
//		printf(err_unable_convert_value_arg, param, arg);
//		return 0;
//	}
//
//	/* ok */
//	return 1;
//}
//
///* */
//int set_int_value(char *arg, char *param, void *p)
//{
//	int error;
//
//	if ( !param )
//	{
//		printf(err_arg_needs_param, arg);
//		return 0;
//	}
//
//	/* convert param */
//	*(int *)p = convert_string_to_int(param, &error);
//	if ( error )
//	{
//		printf(err_unable_convert_value_arg, param, arg);
//		return 0;
//	}
//
//	/* ok */
//	return 1;
//}
//
///* */
//int set_log_file(char *arg, char *param, void *p)
//{
//	if ( param )
//	{
//		printf(err_arg_no_needs_param, arg);
//		return 0;
//	}
//
//	log_enabled = 1;
//
//	/* ok */
//	return 1;
//}
//
///* */
//int show_help(char *arg, char *param, void *p)
//{
//	if ( param )
//	{
//		printf(err_arg_no_needs_param, arg);
//		return 0;
//	}
//
//	puts(msg_usage);
//
//	/* must return error */
//	return 0;
//}

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
	fprintf(stderr, "\t-s\tsoil filename stored into input directory\t(i.e.: -s soil.txt or soil.nc)\n");
	fprintf(stderr, "\t-t\ttopo filename stored into input directory\t(i.e.: -t topo.txt or topo.nc)\n");
	fprintf(stderr, "\t-c\tsettings filename stored into input directory\t(i.e.: -c settings.txt)\n");
	fprintf(stderr, "\t-r\toutput vars list\t(i.e.: -r output_vars.lst)\n");
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


/*
	please note: mm and dd must starts from 0 not 1
*/
int get_daily_row_from_date(const int yyyy, const int mm, const int dd) {
	int i;
	int days;
	int days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if ( IS_LEAP_YEAR(yyyy) )
		++days_per_month[1];

	/* check args */
	assert(((mm >= 0) && (mm < 12)) && ((dd >= 0) && (dd < days_per_month[mm])));

	days = 0;
	for ( i = 0; i < mm; ++i ) {
		days += days_per_month[i];
	}
	return (days+dd);
}


static int log_start(const char* const sitename) {
	char strTmp[3], strTmp2[4], strTmp3[3];
	char strSizeCell[10] = "";
	char strData[30] = "";
	struct tm* data;
	time_t rawtime;
	//add site name to output files

	if ( sitename && sitename[0] ) {
		strcat (out_filename, "_");
		strcat (out_filename, sitename);

		strcat (daily_out_filename, "_");
		strcat (daily_out_filename, sitename);

		strcat (monthly_out_filename, "_");
		strcat (monthly_out_filename, sitename);

		strcat (annual_out_filename, "_");
		strcat (annual_out_filename, sitename);

		strcat (soil_out_filename, "_");
		strcat (soil_out_filename, sitename);
	}



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

	if (!string_compare_i(settings->dndc, "on"))
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
		if ( ! string_compare_i(p, "nc") ) {
			ext = "_nc";
		} else if ( ! string_compare_i(p, "lst") ) {
			ext = "_lst";
		} else if ( ! string_compare_i(p, "txt") ) {
			ext = "_txt";
		} else {
			puts("bad met file!\n");
			return 0;
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

	/* create output files */
	g_log = logger_new(out_filename);
	if ( ! g_log ) puts("Unable to log to file: check logfile path!");
	free(out_filename); out_filename = NULL;

	g_daily_log = logger_new(daily_out_filename);
	if ( ! g_daily_log ) puts("Unable to create daily log!");
	free(daily_out_filename); daily_out_filename = NULL;

	g_monthly_log = logger_new(monthly_out_filename);
	if ( ! g_monthly_log ) puts("Unable to create monthly log!");
	free(monthly_out_filename); monthly_out_filename = NULL;

	g_annual_log = logger_new(annual_out_filename);
	if ( ! g_annual_log ) puts("Unable to create annual log!");
	free(annual_out_filename); annual_out_filename = NULL;

	g_soil_log = logger_new(soil_out_filename);
	if ( ! g_soil_log ) puts("Unable to create soil log!");
	free(soil_out_filename); soil_out_filename = NULL;
	
	/* show paths */
	printf(msg_dataset_path, input_path);
	printf(msg_soil_path, soil_path);
	printf(msg_topo_path, topo_path);
	printf(msg_met_path, input_met_path);
	printf(msg_settings_path, settings_path);
	printf(msg_output_file, output_file);
	printf(msg_daily_output_file, annual_output_file);
	printf(msg_monthly_output_file, annual_output_file);
	printf(msg_annual_output_file, annual_output_file);
	printf(msg_soil_output_file, soil_output_file);

	free(settings_path); settings_path = NULL;

	return 1;
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
	rows_count;

	int year,
	month,
	day;

	int cell;
	YOS *yos;
	ROW *rows;
	MATRIX *m;

	// ALESSIOR
	// this var are declared in types.h
	// so we need to initialize here
	settings = NULL;

	/* register atexit */
	if ( -1 == atexit(clean_up) )
	{
		puts(err_unable_to_register_atexit);
		return 1;
	}

	/* show copyright*/
	logger(g_log, copyright);

	/* show banner */
	logger(g_log, banner, GetNetCDFVersion());

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
		case 's': // Soil filename
			soil_path = malloc(sizeof(*soil_path)*BUFFER_SIZE);
			if( !soil_path )
			{
				fprintf(stderr, "Cannot allocate memory for soil_path.\n");
				return 1;
			}
			bzero(soil_path, BUFFER_SIZE-1);
			strcpy(soil_path, argv[i+1]);
			break;
		case 't': // Topo filename
			topo_path = malloc(sizeof(*topo_path)*BUFFER_SIZE);
			if( !topo_path )
			{
				fprintf(stderr, "Cannot allocate memory for topo_path.\n");
				return 1;
			}
			bzero(topo_path, BUFFER_SIZE-1);
			strcpy(topo_path, argv[i+1]);
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

		case 'r': // outputfilename
			output_vars_path = mystrdup(argv[i+1]);
			if( ! output_vars_path )
			{
				fprintf(stderr, "Cannot allocate memory for output_vars_path.\n");
				return 1;
			}
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

	if( soil_path == NULL )
	{
		fprintf(stderr, "Error: soil filename option is missing!\n");
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
		strcat(tmp, soil_path);
		strcpy(soil_path, tmp);

		free(tmp);
	}

	if( topo_path == NULL )
	{
		fprintf(stderr, "Error: topo filename option is missing!\n");
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
		strcat(tmp, topo_path);
		strcpy(topo_path, tmp);

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

	if ( output_vars_path ) {
		output_vars = ImportOutputVarsFile(output_vars_path);
		if ( ! output_vars ) {
			exit(1);
		}
		free(output_vars_path);
		output_vars_path = NULL;
	}

	/* get program path */
	g_sz_program_path = get_current_directory();
	if ( ! g_sz_program_path ) {
		puts(err_unable_get_current_directory);
		return 1;
	}


	// Import settings.txt file (before logInit(), because I choose output filename from settings)
	error = importSettingsFile(settings_path);
	if ( error )
	{
		logger(g_log, "Settings File not imported!!\n\n");
		return -1;
	}
	else
	{
		printf ("...Settings file imported!!\n");
	}

	if ( settings->time != 'd' ) {
		logger(g_log, "UNCORRECT TIME STEP CHOICED!!!\n");
		return -1;
	}

	/* processing */
	printf(msg_processing, input_path);

	/* import dataset */
	rows = import_dataset(input_path, &rows_count);
	if ( !rows )
	{
		logger(g_log, "unable to get dataset files!");
		return 1;
	}
	puts(msg_ok);
	free(input_path);
	input_path = NULL;


	/* build matrix */
	m = matrix_create(rows, rows_count);

	/* free rows */
	rows_free(rows, rows_count);

	/* check matrix */
	logger(g_log, "Matrix %s created!!\n\n", m ? "" : "not ");
	if ( ! m ) return 1;

	/* fixme ALESSIOR: a porcata, maybe one day will be fixed */
	/* reset */
	x_cells_count = 0;
	y_cells_count = 0;

	g_soil = soil_new();
	if ( ! g_soil ) {
		matrix_free(m);
		return 1;
	}

	g_topo = topo_new();
	if ( ! g_topo ) {
		logger(g_log, "unable to allocate memory for topo struct\n");
		matrix_free(m);
		return 1;
	}

	logger(g_log, "\n3D-CMCC MODEL START....\n\n\n\n");
	for ( cell = 0; cell < m->cells_count; ++cell )
	{
		logger(g_log, "Processing met data files for cell at %d,%d...\n", m->cells[cell].x, m->cells[cell].y);
		logger(g_log, "input_met_path = %s\n", input_met_path);

		/* import soil values */
		if ( ! soil_import(g_soil, soil_path, m->cells[cell].x, m->cells[cell].y) ) {
			matrix_free(m);
			return 1;
		}

		/* only for first cell */
		if ( 0 == cell ) {
			if ( ! log_start(g_soil->sitename) ) {
				matrix_free(m);
				return 1;
			}
		}

		if (	IS_INVALID_VALUE(g_soil->values[SOIL_SAND_PERC])
				|| IS_INVALID_VALUE(g_soil->values[SOIL_CLAY_PERC])
				|| IS_INVALID_VALUE(g_soil->values[SOIL_SILT_PERC])
				|| IS_INVALID_VALUE(g_soil->values[SOIL_DEPTH]) ) {
			logger(g_log, "NO SOIL DATA AVAILABLE\n");
			matrix_free(m);
			return 1;
		}

		/* import topo values */
		if ( ! topo_import(g_topo, topo_path, m->cells[cell].x, m->cells[cell].y) ) {
			matrix_free(m);
			return 1;
		}

		//check hemisphere
		if ( g_soil->values[SOIL_LAT] > 0 ) {
			m->cells[cell].north = 0;
		} else {
			m->cells[cell].north = 1;
		}

		m->cells[cell].years = ImportYosFiles(input_met_path, &years_of_simulation, m->cells[cell].x, m->cells[cell].y);
		logger(g_log, "Met file %simported!!\n\n", m->cells[cell].years ? "": "not ");
		if ( ! m->cells[cell].years ) {
			matrix_free(m);
			return 1;
		}

		// alloc memory for daily output netcdf vars (if any)
		if ( output_vars && output_vars->daily_vars_count && ! output_vars->daily_vars_value ) {
			int ii;
			int rows_count = m->cells_count*years_of_simulation*366*output_vars->daily_vars_count;
			output_vars->daily_vars_value = malloc(rows_count*sizeof*output_vars->daily_vars_value);
			if ( ! output_vars->daily_vars_value ) {
				logger(g_log, err_out_of_memory);
				matrix_free(m);
				return 1;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->daily_vars_value[ii] = INVALID_VALUE;
			}
		}

		// alloc memory for monthly output netcdf vars (if any)
		if ( output_vars && output_vars->monthly_vars_count && ! output_vars->monthly_vars_value ) {
			int ii;
			int rows_count = m->cells_count*years_of_simulation*12*output_vars->monthly_vars_count;
			output_vars->monthly_vars_value = malloc(rows_count*sizeof*output_vars->monthly_vars_value);
			if ( ! output_vars->monthly_vars_value ) {
				logger(g_log, err_out_of_memory);
				FreeOutputVars(output_vars);
				matrix_free(m);
				return 1;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->monthly_vars_value[ii] = INVALID_VALUE;
			}
		}

		// alloc memory for yearly output netcdf vars (if any)
		if ( output_vars && output_vars->yearly_vars_count && ! output_vars->yearly_vars_value ) {
			int ii;
			int rows_count = m->cells_count*years_of_simulation*output_vars->yearly_vars_count;
			output_vars->yearly_vars_value = malloc(rows_count*sizeof*output_vars->yearly_vars_value);
			if ( ! output_vars->yearly_vars_value ) {
				logger(g_log, err_out_of_memory);
				FreeOutputVars(output_vars);
				matrix_free(m);
				return 1;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->yearly_vars_value[ii] = INVALID_VALUE;
			}
		}

		// very ugly hack :(
		yos = m->cells[cell].years;

		logger(g_log, "Total years_of_simulation = %d\n", years_of_simulation);
		logger(g_log, "***************************************************\n");

		for ( year = 0; year < years_of_simulation; ++year )
		{
			//Marconi: the variable i needs to be a for private variable, used to fill the vpsat vector v(365;1)
			int i;
			// ALESSIOR for handling leap years
			int days_per_month;

			logger(g_log, "\n-Year simulated = %d\n", m->cells[cell].years[year].year);

			/* only for first year */
			if ( ! year ) matrix_summary (m);

			i =0;
			for (month = 0; month < MONTHS; month++)
			{
				days_per_month = DaysInMonth[month];
				if ( (FEBRUARY == month) && IS_LEAP_YEAR(m->cells[cell].years[year].year) )
				{
					++days_per_month;
				}

				for (day = 0; day < days_per_month; day++)
				{
					/* daily climate variables */
					Avg_temperature (&m->cells[cell], day, month, year);
					Daylight_avg_temperature (&m->cells[cell], day, month, year, yos);
					Nightime_avg_temperature (&m->cells[cell], day, month, year, yos);
					Soil_temperature (&m->cells[cell], day, month, year, yos);
					Thermic_sum (&m->cells[cell], day, month, year, yos);
					Air_density (&m->cells[cell], day, month, year, yos);
					Day_Length (&m->cells[cell], day, month, year, yos);
					Latent_heat (&m->cells[cell], day, month, year, yos);
					Air_pressure (&m->cells[cell], day, month, year, yos);
					Annual_met_values (&m->cells[cell], day, month, year, yos);
					Annual_CO2_concentration (&m->cells[cell], day, month, year, yos);

					if(m->cells[cell].landuse == F)
					{
						/* compute before any other processes annually the days for the growing season */
						Veg_Days (&m->cells[cell], yos, day, month, year);
						//Marconi 18/06: function used to calculate VPsat from Tsoil following Hashimoto et al., 2011
						get_vpsat(&m->cells[cell], day, month, year, yos, i);
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
				if ( (FEBRUARY == month) && IS_LEAP_YEAR(yos[year].year) )
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
							//if(day == 0 && month == 0) leaffall(&m->cells[cell]);
							//run for FEM version
							if (!Tree_model_daily (m, yos, year, month, day, years_of_simulation, cell) )
							{
								logger(g_log, "tree model daily failed.");
							}
							else
							{
								puts(msg_ok);
								//										logger(g_soil_log, "\nsoilLog prova");
								//run for SOIL functions
								//soil_model (m, yos, years, month, day, years_of_simulation);

								if (!string_compare_i(settings->dndc, "on"))
								{
									logger(g_log, "RUNNING DNDC.....\n");
									//TODO SERGIO
									soil_dndc_sgm (m, yos, year, month, day, years_of_simulation);
									//soil_dndc......................................
								}
								else
								{
									logger(g_log, "No soil simulation!!!\n");
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
						if ( !crop_model_D (m, yos, year, month, day, years_of_simulation) )
						{
							logger(g_log, "crop model failed.");
						}
						else
						{
							puts(msg_ok);
							//look if put it here or move before tree_model  at the beginning of each month simulation
							//	soil_model (m, yos, years, month, years_of_simulation);
						}
					}
					logger(g_log, "****************END OF DAY (%d)*******************\n\n\n", day+1);

					// save values for put in output netcdf
					if ( output_vars && output_vars->daily_vars_count )
					{
					/*
						la memoria è stata allocata come C*R*Y*X
						
						C = colonne ( variabili )
						R = righe ( anni di elaborazione * 366 )
						Y = numero y celle
						X = numero x celle
						
						quindi il valore [v1][v2][v3][v4] è indicizzato a 
						
						[v1 * n1 * n2 *n3 + v2 * n2 * n3 + v3 * n3 + v4]

						ossia

						[v4 + n3 * (v3 + n2 * (v2 + n1 * v1))]
					*/

					#define YS					(y_cells_count)
					#define XS					(x_cells_count)
					#define ROWS				(366*years_of_simulation)
					#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
						int i;
						for ( i = 0; i < output_vars->daily_vars_count; ++i )
						{
							int row = get_daily_row_from_date(yos[year].year, month, day) + (year*366);
							int index = VALUE_AT(m->cells[cell].x, m->cells[cell].y, row, i);
							if ( AR_DAILY_OUT == output_vars->daily_vars[i] )	output_vars->daily_vars_value[index] = m->cells[cell].daily_aut_resp;
							if ( GPP_DAILY_OUT == output_vars->daily_vars[i] )	output_vars->daily_vars_value[index] = m->cells[cell].daily_gpp;
							if ( NPP_DAILY_OUT == output_vars->daily_vars[i] )	output_vars->daily_vars_value[index] = m->cells[cell].daily_npp_gC;
						}
					#undef VALUE_AT
					#undef ROWS
					#undef XS
					#undef YS
					}

					EOD_cumulative_balance_cell_level (&m->cells[cell], yos, year, month, day, cell);
					if (!string_compare_i(settings->dndc, "on"))
					{
						Get_EOD_soil_balance_cell_level (&m->cells[cell], yos, year, month, day);
					}

				}
				logger(g_log, "****************END OF MONTH (%d)*******************\n", month+1);

				// save values for put in output netcdf
				if ( output_vars && output_vars->monthly_vars_count )
				{
				#define YS					(y_cells_count)
				#define XS					(x_cells_count)
				#define ROWS				(12*years_of_simulation)
				#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
					int i;
					for ( i = 0; i < output_vars->monthly_vars_count; ++i )
					{
						int row = month + (year*12);
						int index = VALUE_AT(m->cells[cell].x, m->cells[cell].y, row, i);
						if ( AR_MONTHLY_OUT == output_vars->monthly_vars[i] ) output_vars->monthly_vars_value[index] = m->cells[cell].monthly_aut_resp;
						if ( GPP_MONTHLY_OUT == output_vars->monthly_vars[i] ) output_vars->monthly_vars_value[index] = m->cells[cell].monthly_gpp;
						if ( NPP_MONTHLY_OUT == output_vars->monthly_vars[i] ) output_vars->monthly_vars_value[index] = m->cells[cell].monthly_npp_gC;
					}
				#undef VALUE_AT
				#undef ROWS
				#undef XS
				#undef YS
				}

				EOM_cumulative_balance_cell_level (&m->cells[cell], yos, year, month, cell);
			}

			logger(g_log, "****************END OF YEAR (%d)*******************\n\n", yos[year].year);

			// save values for put in output netcdf
			if ( output_vars && output_vars->yearly_vars_count )
			{
			#define YS					(y_cells_count)
			#define XS					(x_cells_count)
			#define ROWS				(years_of_simulation)
			#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
				int i;
				for ( i = 0; i < output_vars->yearly_vars_count; ++i )
				{
					int index = VALUE_AT(m->cells[cell].x, m->cells[cell].y, year, i);
					if ( AR_YEARLY_OUT == output_vars->yearly_vars[i] ) output_vars->yearly_vars_value[index] = m->cells[cell].annual_aut_resp;
					if ( GPP_YEARLY_OUT == output_vars->yearly_vars[i] ) output_vars->yearly_vars_value[index] = m->cells[cell].annual_gpp;
					if ( NPP_YEARLY_OUT == output_vars->yearly_vars[i] ) output_vars->yearly_vars_value[index] = m->cells[cell].annual_npp_gC;
				}
			#undef VALUE_AT
			#undef ROWS
			#undef XS
			#undef YS
			}

			EOY_cumulative_balance_cell_level (m, &m->cells[cell], yos, year, years_of_simulation, cell);
			logger(g_log, "...%d finished to simulate\n\n\n\n\n\n", yos[year].year);
		}
		i = yos[0].year;
		free(yos);
		yos = NULL;
		m->cells[cell].years = NULL; /* required */
	}

	/* free memory */
	matrix_free(m);

	/* NETCDF output */
	if ( output_vars && output_vars->daily_vars_value ) {
		if ( ! WriteNetCDFOutput(output_vars, i, years_of_simulation, x_cells_count, y_cells_count, 0) ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		free(output_vars->daily_vars_value);
		output_vars->daily_vars_value = NULL;
	}

	if ( output_vars && output_vars->monthly_vars_value ) {
		if ( ! WriteNetCDFOutput(output_vars, i, years_of_simulation, x_cells_count, y_cells_count, 1) ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		free(output_vars->monthly_vars_value);
		output_vars->monthly_vars_value = NULL;
	}

	if ( output_vars && output_vars->yearly_vars_value ) {
		if ( ! WriteNetCDFOutput(output_vars, i, years_of_simulation, x_cells_count, y_cells_count, 2) ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		free(output_vars->yearly_vars_value);
		output_vars->yearly_vars_value = NULL;
	}

	logger_close(g_soil_log); g_soil_log = NULL;
	logger_close(g_annual_log); g_annual_log = NULL;
	logger_close(g_monthly_log); g_monthly_log = NULL;
	logger_close(g_daily_log); g_daily_log = NULL;
	logger_close(g_log); g_log = NULL;

	// Free memory
	if ( output_vars ) FreeOutputVars(output_vars);
	free(input_met_path); input_met_path = NULL;
	free(output_file); output_file = NULL;
	free(daily_output_file); daily_output_file = NULL;
	free(monthly_output_file); monthly_output_file = NULL;
	free(annual_output_file); annual_output_file = NULL;
	free(soil_output_file); soil_output_file = NULL;

	/* this should be freed before */
	free(topo_path); topo_path = NULL;
	free(soil_path); soil_path = NULL;

	/* free memory at exit */
	return 0;
}
