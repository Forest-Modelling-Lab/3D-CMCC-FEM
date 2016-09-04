/* main.c */
/*

	TODO:

	-SIMULAZIONI DA bareground
	-RENDERE DINAMICO IL TREE_LAYER_LIMIT (all'aumentare del dbh aumenta il TREE_LAYER_LIMIT) see Montgomery & Chazdon, 2001)
	-VERSION FEM AND BGC
	-RIMETTERE PER L'ACQUA COMPETIZIONE SIMMETRICA
 */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#ifdef _WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "compiler.h"
#include "soil_settings.h"
#include "print_output.h"
#include "common.h"
#include "constants.h"
#include "topo.h"
#include "settings.h"
#include "logger.h"
#include "output.h"
#include "matrix.h"
#include "yos.h"
#include "nc.h"
#include "met_data.h"
#include "cropmodel_daily.h"
#include "g-function.h"
#include "leaf_fall.h"
#include "treemodel_daily.h"
#include "soil_dndc.h"
#include "fluxes.h"
#include "print.h"
#include "utility.h"
#include "cell_model.h"
#include "soil_model.h"

/* Last cumulative days in months in non Leap years */
int MonthLength [] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

/* Last cumulative days in months in Leap years */
int MonthLength_Leap [] = { 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

/* Days in Months */
int DaysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *szMonth[MONTHS_COUNT] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
		"AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };

/* global vars */
/* DO NOT REMOVE INITIALIZATION TO NULL, IT IS REQUIRED !! */
logger_t* g_log = NULL;
logger_t* g_daily_log = NULL;
logger_t* g_monthly_log = NULL;
logger_t* g_annual_log = NULL;
logger_t* g_soil_log = NULL;
soil_settings_t* g_soil_settings = NULL;
topo_t* g_topo = NULL;
settings_t* g_settings = NULL;

/* DO NOT REMOVE INITIALIZATION TO NULL, IT IS REQUIRED !! */
char 	*g_sz_program_path = NULL
		, *g_sz_parameterization_path = NULL
		, *g_sz_input_path = NULL
		, *g_sz_dataset_file = NULL
		, *g_sz_input_met_file = NULL
		, *g_sz_soil_file = NULL
		, *g_sz_topo_file = NULL

		, *g_sz_debug_output_filename = NULL
		, *g_sz_daily_output_filename =	NULL
		, *g_sz_monthly_output_filename = NULL
		, *g_sz_yearly_output_filename = NULL
		, *g_sz_soil_output_filename = NULL

		, *g_sz_settings_file =	NULL
		, *g_sz_output_vars_file = NULL
		;

static int years_of_simulation;	// default is none

/* strings */
const char sz_launched[] = "\n"PROGRAM_NAME"\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"using NetCDF %s\n"
		"launched: %s\n"
		"--------------------------------------------------------------------------------\n";

static const char banner[] = "\n"PROGRAM_NAME"\n"
		"by Alessio Collalti [alessio.collalti@cmcc.it, a.collalti@unitus.it]\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"using NetCDF %s\n"
		"(use -h parameter for more information)\n\n";

static char copyright[] =
		"\n"
		"The 3D-CMCC Forest Ecosystem Model (v.5.2.2) has been developed by:\n"
		"Alessio Collalti [alessio.collalti@cmcc.it, a.collalti@unitus.it],\n"
		"Alessio Ribeca,\n"
		"Sergio Marconi [sergio.marconi@cmcc.it]\n"
		"Monia Santini [monia.santini@cmcc.it]\n"
		"euroMediterranean Center on Climate Changes (CMCC),\n"
		"IAFES division,\n"
		"Viale Trieste 120, 01100 - Viterbo, Italy,\n"
		"and \n"
		"University of Tuscia ,\n"
		"Department for innovation in biological, agro-food and forest systems (DIBAF),\n"
		"Forest Ecology Lab\n"
		"\n"
		"Programmers: Alessio Collalti - Alessio Ribeca - Sergio Marconi - Monia Santini\n"
		"\n"
		"\"DISCLAIMER\"\n\n"
		"CMCC and University of Tuscia\n"
		"accepts no responsibility for the use of the 3D_CMCC FEM in\n"
		"the form supplied or as subsequently modified by third parties.\n"
		"CMCC and University of Tuscia disclaims liability for all losses,\n"
		"damages and costs incurred by any person as a result of relying on this software.\n"
		"Use of this software assumes agreement to this condition of use.\n"
		"Removal of this statement violates the spirit in which 3D-CMCC FEM,\n"
		"was released by CMCC-UNITUS.\n"
		"for more information see:\n"
		"-Collalti et al., 2014 Ecological Modelling,\n"
		"-Collalti et al., 2016 Geoscientific Model Development\n"
		"--------------------------------------------------------------------------------\n"
		;


static const char msg_input_path[]				=	"input path = %s\n";
static const char msg_parameterization_path[]	=	"parameterization path = %s\n";
static const char msg_soil_file[]				=	"soil file = %s\n";
static const char msg_topo_file[]				=	"topo file = %s\n";
static const char msg_met_path[]				=	"met path = %s\n";
static const char msg_settings_path[]			=	"settings path = %s\n";
static const char msg_debug_output_file[]		=	"debug output file path = %s\n";
static const char msg_daily_output_file[]		=	"daily output file path = %s\n";
static const char msg_monthly_output_file[]		=	"monthly output file path = %s\n";
static const char msg_annual_output_file[]		=	"annual output file path = %s\n";
static const char msg_soil_output_file[]		=	"soil output file path = %s\n";
static const char msg_ok[]						=	"ok ";
static const char msg_ok_tree_model[]			=	"ok tree_model_daily";
static const char msg_ok_soil_model[]			=	"ok soil_model_daily";
static const char msg_ok_cell_model[]			=	"ok cell_model_daily";
static const char msg_usage[]					=	"\nusage:\n"
		"  3D-CMCC Model -i INPUT_DIR -o OUTPUT_FILENAME -d DATASET_FILENAME -m MET_FILE_LIST -s SITE_FILENAME -c SETTINGS_FILENAME [-h]\n"
		"  required options:\n"
#ifdef _WIN32
		"    -i input directory (i.e.: -i c:\\input\\directory\\)\n"
		"    -p parameterization directory (i.e.: -i c:\\parameterization\\directory\\)\n"
		"    -o output filename (i.e.: -o c:\\output\\CMCC.log)\n"
		"    -b daily output filename (i.e.: -o c:\\output\\daily_CMCC.log)\n"
		"    -f monthly output filename (i.e.: -o c:\\output\\monthly_CMCC.log)\n"
		"    -e annual output filename (i.e.: -o c:\\output\\annual_CMCC.log)\n"
#else
		"    -i input directory (i.e.: -i path/to/input/directory/)\n"
		"    -p parameterization directory (i.e.: -i path/to/parameterization/directory/)\n"
		"    -o output filename (i.e.: -o /path/to/CMCC.log)\n"
		"    -b daily output filename (i.e.: -o /path/to/CMCC.log)\n"
		"    -f monthly output filename (i.e.: -o /path/to/CMCC.log)\n"
		"    -e annual output filename (i.e.: -o /path/to/CMCC.log)\n"
#endif
		"    -d dataset filename stored into input directory (i.e.: -d input.txt)\n"
		"    -m met filename list stored into input directory (i.e.: -m 1999.txt,2003.txt,2009.txt)\n"
		"    -s soil filename stored into input directory (i.e.: -s soil.txt or soil.nc)\n"
		"    -t topo filename stored into input directory (i.e.: -t topo.txt or topo.nc)\n"
		"    -c settings filename stored into input directory (i.e.: -c settings.txt)\n"
		"    -r output vars list\t(i.e.: -r output_vars.lst)\n"
		"  optional options:\n"
		"    -h print this help\n"
		;

/* error messages */
extern const char sz_err_out_of_memory[];
const char err_unable_open_file[] = "unable to open file.";
const char err_empty_file[] = "empty file ?";
const char err_window_size_too_big[] = "window size too big.";
static const char err_unable_get_current_path[] = "unable to retrieve current path.\n";
static const char err_unable_to_register_atexit[] = "unable to register clean-up routine.\n";


static void clean_up(void) {
	if ( g_sz_output_vars_file ) free(g_sz_output_vars_file);
	if ( g_sz_settings_file ) free(g_sz_settings_file);
	if ( g_sz_debug_output_filename ) free(g_sz_debug_output_filename);
	if ( g_sz_daily_output_filename ) free(g_sz_daily_output_filename);
	if ( g_sz_monthly_output_filename ) free(g_sz_monthly_output_filename);
	if ( g_sz_yearly_output_filename ) free(g_sz_yearly_output_filename);
	if ( g_sz_soil_output_filename ) free(g_sz_soil_output_filename);
	if ( g_sz_parameterization_path ) free(g_sz_parameterization_path);
	if ( g_sz_input_path ) free(g_sz_input_path);
	if ( g_topo ) free(g_topo);
	if ( g_sz_topo_file ) free(g_sz_topo_file);
	if ( g_soil_settings ) free(g_soil_settings);
	if ( g_sz_soil_file ) free(g_sz_soil_file);
	if ( g_settings ) free(g_settings);
	if ( g_sz_input_met_file ) free(g_sz_input_met_file);
	if ( g_sz_dataset_file ) free(g_sz_dataset_file);
	if ( g_sz_program_path ) free(g_sz_program_path);

#ifdef _WIN32
#ifdef _DEBUG
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

static void show_usage(void) {
	puts(msg_usage);
}

static int log_start(const char* const sitename) {
	struct tm* data;
	time_t rawtime;
	char buffer[128]; /* should be enough */
	char buffer_2[32];

	time(&rawtime);
	data = gmtime(&rawtime);

	sprintf(buffer, "_%s", PROGRAM_VERSION);

	if ( sitename && sitename[0] ) {
		strcat(buffer, "_");
		strcat(buffer, sitename);
	}

	strcat(buffer, "_");
	sprintf(buffer_2, "%c", g_settings->version);
	strcat(buffer, buffer_2);
	strcat(buffer, "_CO2_mod_");
	if ( ! string_compare_i(g_settings->CO2_mod, "on") )
	{
		strcat(buffer, "ON");
	}
	else
	{
		strcat(buffer, "OFF");
	}
	strcat(buffer, "_Manag_");
	if ( ! string_compare_i(g_settings->management, "on") )
	{
		strcat(buffer, "ON");
	}
	else
	{
		strcat(buffer, "OFF");
	}
	strcat(buffer, "_");
	sprintf(buffer_2, "%c", g_settings->time);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%d", (int)g_settings->sizeCell);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%d", data->tm_year+1900);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%s", szMonth[data->tm_mon]);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%d", data->tm_mday);
	strcat(buffer, buffer_2);

	if ( ! string_compare_i(g_settings->dndc, "on") ) {
		strcat(buffer, "_");
		strcat(buffer, "DNDC");
	}

	/* add suffix for type files */
	{
		char *p;
		char *ext;
		p = strrchr(g_sz_input_met_file, '.');
		if ( ! p ) {
			puts("bad met file!\n");
			return -1;
		}
		++p;
		if ( ! string_compare_i(p, "nc") || ! string_compare_i(p, "nc4") ) {
			ext = "_nc";
		} else if ( ! string_compare_i(p, "lst") ) {
			ext = "_lst";
		} else if ( ! string_compare_i(p, "txt") ) {
			ext = "_txt";
		} else {
			puts("bad met file!\n");
			return 0;
		}
		strcat(buffer, ext);
	}

	strcat(buffer, ".txt");

	/* absolute path ? */
	g_log = logger_new("%s%s", g_sz_debug_output_filename, buffer);
	if ( ! g_log ) {
		puts("Unable to create log!");
		return 0;
	}

	g_daily_log = logger_new("%s%s", g_sz_daily_output_filename, buffer);
	if ( ! g_daily_log ) {
		puts("Unable to create daily log!");
		return 0;
	}

	g_monthly_log = logger_new("%s%s", g_sz_monthly_output_filename, buffer);
	if ( ! g_monthly_log ) {
		puts("Unable to create monthly log!");
		return 0;
	}

	g_annual_log = logger_new("%s%s", g_sz_yearly_output_filename, buffer);
	if ( ! g_annual_log ) {
		puts("Unable to create annual log!");
		return 0;
	}

	g_soil_log = logger_new("%s%s", g_sz_soil_output_filename, buffer);
	if ( ! g_soil_log ) {
		puts("Unable to create soil log!");
		return 0;
	}

	logger_disable_std(g_log);
	logger(g_log, copyright);
	logger_enable_std(g_log);

	logger(g_log, sz_launched, netcdf_get_version(), get_datetime());

	/* show paths */
	if ( g_sz_input_path )
		logger(g_log,"\nFILE PATHS\n");
	logger(g_log, msg_input_path, g_sz_input_path);
	logger(g_log, msg_parameterization_path, g_sz_parameterization_path);
	logger(g_log, msg_soil_file, g_sz_soil_file);
	logger(g_log, msg_topo_file, g_sz_topo_file);
	logger(g_log, msg_met_path, g_sz_input_met_file);
	logger(g_log, msg_settings_path, g_sz_settings_file);
	logger(g_log, msg_debug_output_file, g_sz_debug_output_filename);
	logger(g_log, msg_daily_output_file, g_sz_daily_output_filename);
	logger(g_log, msg_monthly_output_file, g_sz_monthly_output_filename);
	logger(g_log, msg_annual_output_file, g_sz_yearly_output_filename);
	logger(g_log, msg_soil_output_file, g_sz_soil_output_filename);

	return 1;
}

/*
	ALESSIOR
	simply remove all after last / or \
	NOT GUARANTEED THAT IS A REAL PATH ;)
 */
static char* get_path(const char *const s) {
	char *p;
	char *bs;
	char *temp;

	temp = string_copy(s);
	if ( ! temp ) return NULL;

	p = strrchr(temp, '/');
	if ( p ) ++p;
	bs = strrchr(temp, '\\');
	if ( bs ) ++bs;
	if ( bs > p ) p = bs;
	if ( p ) *p = '\0';

	return temp;
}

/*
	copy path and add a / at end if missing
 */

char* path_copy(const char *const s) {
	if ( s ) {
		int i;
		i = strlen(s);
		if ( ('/' == s[i-1]) || ('\\' == s[i-1]) ) {
			return string_copy(s);
		} else {
			char *p = malloc(i+1+1);
			if ( p ) {
				if ( strcpy(p, s) ) {
					strcat(p, "/");
					return p;
				} else {
					free(p);
					p = NULL;
				}
			}
		}
	}
	return NULL;
}

/*
	parse and check passed args
 */
static int parse_args(int argc, char *argv[]) {
	int i;

	g_sz_input_path = NULL;
	g_sz_parameterization_path = NULL;
	g_sz_debug_output_filename = NULL;
	g_sz_daily_output_filename = NULL;
	g_sz_monthly_output_filename = NULL;
	g_sz_yearly_output_filename = NULL;
	g_sz_soil_output_filename = NULL;
	g_sz_dataset_file = NULL;
	g_sz_input_met_file = NULL;
	g_sz_soil_file = NULL;
	g_sz_topo_file = NULL;
	g_sz_settings_file = NULL;
	g_sz_output_vars_file = NULL;

	for ( i = 1; i < argc; ++i ) {
		if ( argv[i][0] != '-' ) {
			continue;
		}

		switch ( argv[i][1] ) {
		case 'i': /* folder where input files are stored */
			if ( ! argv[i+1] ) {
				puts("input path not specified!");
				goto err;
			}
			g_sz_input_path = path_copy(argv[i+1]);
			if ( ! g_sz_input_path ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'p': /* folder where parameterization files are stored */
			if ( ! argv[i+1] ) {
				puts("parameterization path not specified!");
				goto err;
			}
			g_sz_parameterization_path = path_copy(argv[i+1]);
			if ( ! g_sz_parameterization_path ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'o': /* debug file path */
			if ( ! argv[i+1] ) {
				puts("debug file path not specified!");
				goto err;
			}
			g_sz_debug_output_filename = string_copy(argv[i+1]);
			if ( ! g_sz_debug_output_filename ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'b': /* daily file path */
			if ( ! argv[i+1] ) {
				puts("daily file not specified!");
				goto err;
			}
			g_sz_daily_output_filename = string_copy(argv[i+1]);
			if( ! g_sz_daily_output_filename ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'f': /* monthly file path */
			if ( ! argv[i+1] ) {
				puts("monthly file not specified!");
				goto err;
			}
			g_sz_monthly_output_filename = string_copy(argv[i+1]);
			if( ! g_sz_monthly_output_filename ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'e': /* yearly file path */
			if ( ! argv[i+1] ) {
				puts("annual file not specified!");
				goto err;
			}
			g_sz_yearly_output_filename = string_copy(argv[i+1]);
			if( ! g_sz_yearly_output_filename ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'n': /* soil file path */
			if ( ! argv[i+1] ) {
				puts("soil file not specified!");
				goto err;
			}
			g_sz_soil_output_filename = string_copy(argv[i+1]);
			if( ! g_sz_soil_output_filename ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'd': /* dataset file path */
			if ( ! argv[i+1] ) {
				puts("dataset file not specified!");
				goto err;
			}
			g_sz_dataset_file = string_copy(argv[i+1]);
			if( ! g_sz_dataset_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'm': /* met filename */
			if ( ! argv[i+1] ) {
				puts("met file not specified!");
				goto err;
			}
			g_sz_input_met_file = string_copy(argv[i+1]);
			if ( ! g_sz_input_met_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 's': /*soil file */
			if ( ! argv[i+1] ) {
				puts("soil file not specified!");
				goto err;
			}
			g_sz_soil_file = string_copy(argv[i+1]);
			if ( ! g_sz_soil_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 't': /* topo file */
			if ( ! argv[i+1] ) {
				puts("topo file not specified!");
				goto err;
			}
			g_sz_topo_file = string_copy(argv[i+1]);
			if( ! g_sz_topo_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'c': /* settings file */
			if ( ! argv[i+1] ) {
				puts("settings file not specified!");
				goto err;
			}
			g_sz_settings_file = string_copy(argv[i+1]);
			if( ! g_sz_settings_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'r': /* output filename */
			if ( ! argv[i+1] ) {
				puts("output path not specified!");
				goto err;
			}
			g_sz_output_vars_file = string_copy(argv[i+1]);
			if( ! g_sz_output_vars_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'h': /* show help */
			goto err_show_usage;
			break;

		default:
			printf("invalid option (%s)!\n", argv[i]);
			goto err_show_usage;
		}
	}

	/* check that each mandatory parameter has been used */
	/*
	if ( ! g_sz_input_path ) {
		g_sz_input_path = string_copy(g_sz_program_path);
		if ( ! g_sz_input_path ) {
			puts(sz_err_out_of_memory);
			goto err;
		}
	}
	 */

	if ( ! g_sz_parameterization_path ) {
		puts("parameterization path not specified!");
		goto err_show_usage;
	}

	if ( ! g_sz_debug_output_filename ) {
		puts("output filename option is missing!");
		goto err_show_usage;
	}

	if ( ! g_sz_monthly_output_filename ) {
		puts("monthly output filename option is missing!");
		goto err_show_usage;
	}

	if ( ! g_sz_yearly_output_filename ) {
		puts("annual output filename option is missing!");
		goto err_show_usage;
	}

	if ( ! g_sz_soil_output_filename ) {
		puts("soil output filename option is missing!");
		goto err_show_usage;
	}

	if ( ! g_sz_dataset_file ) {
		puts("dataset filename not specified!");
		goto err_show_usage;
	}

	if ( ! g_sz_input_met_file ) {
		puts("met file list is missing!");
		goto err_show_usage;
	}

	if ( ! g_sz_soil_file ) {
		puts("soil filename not specified!");
		goto err_show_usage;
	}

	if ( ! g_sz_topo_file ) {
		puts("topo filename option is missing!");
		goto err_show_usage;
	}

	if ( ! g_sz_settings_file ) {
		puts("settings filename option is missing!");
		goto err_show_usage;
	}

	return 1;

err_show_usage:
	show_usage();

	err:
	return 0;
}

int main(int argc, char *argv[]) {
	char temp[256];
	int ret;
	int year;
	int month;
	int day;
	int cell;
	int prog_ret;
	int flag;
	int start_year;
	matrix_t* matrix;
	output_t* output_vars;
	
	//_CrtSetBreakAlloc(109);
	
	/* initialize */
	matrix = NULL;
	output_vars = NULL;
	prog_ret = 1;

	/* get program path */
	g_sz_program_path = get_current_path();
	if ( ! g_sz_program_path ) {
		puts(err_unable_get_current_path);
		return 1;
	}

	/* register atexit */
	if ( -1 == atexit(clean_up) ) {
		puts(err_unable_to_register_atexit);
		return 1;
	}

	/* show copyright */
	puts(copyright);

	/* show banner */
	printf(banner, netcdf_get_version());

	/* parse args */
	printf("parsing arguments...");
	if ( ! parse_args(argc, argv) ) {
		return 1;
	}
	puts(msg_ok);

	/* some check for fix import path */
	flag = 0;
	if ( g_sz_input_path ) {
		int len = strlen(g_sz_input_path);
		flag = (('/' == g_sz_input_path[len-1]) || ('\\' == g_sz_input_path[len-1]));
	}

	/* import output vars file ? */
	if ( g_sz_output_vars_file ) {
		printf("import output file...");
		if ( g_sz_input_path ) {
			strcpy(temp, g_sz_input_path);
			if ( ! flag ) strcat(temp, FOLDER_DELIMITER);
			strcat(temp, g_sz_output_vars_file);
			output_vars = output_import(temp);
		} else {
			output_vars = output_import(g_sz_output_vars_file);
		}
		free(g_sz_output_vars_file);
		g_sz_output_vars_file = NULL;
		if ( ! output_vars ) {
			return 1;
		}
		puts(msg_ok);
	}

	printf("import settings file %s...", g_sz_settings_file);
	if ( g_sz_input_path ) {
		strcpy(temp, g_sz_input_path);
		if ( ! flag ) strcat(temp, FOLDER_DELIMITER);
		strcat(temp, g_sz_settings_file);
		g_settings = settings_import(temp);
	} else {
		g_settings = settings_import(g_sz_settings_file);
	}
	if ( ! g_settings ) {
		goto err;
	}
	if ( g_settings->time != 'd' ) {
		puts("uncorrect time step choiced!");
		goto err;
	} else {
		puts(msg_ok);
	}

	printf("build matrix using %s...", g_sz_dataset_file);
	if ( g_sz_input_path ) {
		strcpy(temp, g_sz_input_path);
		if ( ! flag ) strcat(temp, FOLDER_DELIMITER);
		strcat(temp, g_sz_dataset_file);
		matrix = matrix_create(temp);
	} else {
		matrix = matrix_create(g_sz_dataset_file);
	}
	free(g_sz_dataset_file); g_sz_dataset_file = NULL;
	if ( ! matrix ) goto err;
	puts(msg_ok);

	printf("soil allocation...");
	g_soil_settings = soil_settings_new();
	if ( ! g_soil_settings ) {
		logger(g_log, sz_err_out_of_memory);
		goto err;
	}
	puts(msg_ok);
	printf("topo allocation...");
	g_topo = topo_new();
	if ( ! g_topo ) {
		logger(g_log, sz_err_out_of_memory);
		goto err;
	}
	puts(msg_ok);


	start_year = -1;
	logger(g_log, "\n3D-CMCC FEM START....\n\n");
	for ( cell = 0; cell < matrix->cells_count; ++cell )
	{
		/* Marconi: the variable i needs to be a for private variable, used to fill the vpsat vector v(365;1) */
		/* ALESSIOR: i renamed to index_vpsat */
		int index_vpsat;

		logger(g_log, "Processing met data files for cell at %d,%d...\n", matrix->cells[cell].x, matrix->cells[cell].y);
		logger(g_log, "input_met_path = %s\n", g_sz_input_met_file);

		/* import soil values */
		logger(g_log, "importing soil settings...");
		if ( g_sz_input_path ) {
			strcpy(temp, g_sz_input_path);
			if ( ! flag ) strcat(temp, FOLDER_DELIMITER);
			strcat(temp, g_sz_soil_file);
			ret = soil_settings_import(g_soil_settings, temp, matrix->cells[cell].x, matrix->cells[cell].y);
		} else {
			ret = soil_settings_import(g_soil_settings, g_sz_soil_file, matrix->cells[cell].x, matrix->cells[cell].y);
		}
		if ( ! ret ) {
			goto err;
		}
		logger(g_log, "ok\n");

		/* only for first cell */
		if ( 0 == cell ) {
			if ( ! log_start(g_soil_settings->sitename) ) {
				goto err;
			}
		}

		if (	IS_INVALID_VALUE(g_soil_settings->values[SOIL_SAND_PERC])||
				IS_INVALID_VALUE(g_soil_settings->values[SOIL_CLAY_PERC])||
				IS_INVALID_VALUE(g_soil_settings->values[SOIL_SILT_PERC])||
				IS_INVALID_VALUE(g_soil_settings->values[SOIL_DEPTH]) ) {
			logger(g_log, "NO SOIL DATA AVAILABLE\n");
			goto err;
		}

		/* import topo values */
		logger(g_log, "importing topo settings...");
		if ( g_sz_input_path ) {
			strcpy(temp, g_sz_input_path);
			if ( ! flag ) strcat(temp, FOLDER_DELIMITER);
			strcat(temp, g_sz_topo_file);
			ret = topo_import(g_topo, temp, matrix->cells[cell].x, matrix->cells[cell].y);
		} else {
			ret = topo_import(g_topo, g_sz_topo_file, matrix->cells[cell].x, matrix->cells[cell].y);
		}
		if ( ! ret ) {
			goto err;
		}
		logger(g_log, "ok\n");

		/* check hemisphere */
		if ( g_soil_settings->values[SOIL_LAT] > 0 ) {
			matrix->cells[cell].north = 0;
		} else {
			matrix->cells[cell].north = 1;
		}

		logger(g_log, "importing met data...");
		if ( g_sz_input_path ) {
			strcpy(temp, g_sz_input_path);
			if ( ! flag ) strcat(temp, FOLDER_DELIMITER);
			strcat(temp, g_sz_input_met_file);
			matrix->cells[cell].years = yos_import(temp, &years_of_simulation, matrix->cells[cell].x, matrix->cells[cell].y);
		} else {
			matrix->cells[cell].years = yos_import(g_sz_input_met_file, &years_of_simulation, matrix->cells[cell].x, matrix->cells[cell].y);
		}
		if ( ! matrix->cells[cell].years ) goto err;
		logger(g_log, "ok\n");

		/* get start year */
		if ( -1 == start_year ) {
			start_year =  matrix->cells[0].years[0].year;
		}

		/* alloc memory for daily output netcdf vars (if any) */
		if ( output_vars && output_vars->daily_vars_count && ! output_vars->daily_vars_value ) {
			int ii;
			int rows_count = matrix->cells_count*years_of_simulation*366*output_vars->daily_vars_count;
			output_vars->daily_vars_value = malloc(rows_count*sizeof*output_vars->daily_vars_value);
			if ( ! output_vars->daily_vars_value ) {
				logger(g_log, sz_err_out_of_memory);
				goto err;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->daily_vars_value[ii] = INVALID_VALUE;
			}
		}

		/* alloc memory for monthly output netcdf vars (if any) */
		if ( output_vars && output_vars->monthly_vars_count && ! output_vars->monthly_vars_value ) {
			int ii;
			int rows_count = matrix->cells_count*years_of_simulation*12*output_vars->monthly_vars_count;
			output_vars->monthly_vars_value = malloc(rows_count*sizeof*output_vars->monthly_vars_value);
			if ( ! output_vars->monthly_vars_value ) {
				logger(g_log, sz_err_out_of_memory);
				goto err;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->monthly_vars_value[ii] = INVALID_VALUE;
			}
		}

		/* alloc memory for yearly output netcdf vars (if any) */
		if ( output_vars && output_vars->yearly_vars_count && ! output_vars->yearly_vars_value ) {
			int ii;
			int rows_count = matrix->cells_count*years_of_simulation*output_vars->yearly_vars_count;
			output_vars->yearly_vars_value = malloc(rows_count*sizeof*output_vars->yearly_vars_value);
			if ( ! output_vars->yearly_vars_value ) {
				logger(g_log, sz_err_out_of_memory);
				goto err;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->yearly_vars_value[ii] = INVALID_VALUE;
			}
		}

		logger(g_log, "Total years_of_simulation = %d\n", years_of_simulation);
		logger(g_log, "***************************************************\n\n");

		for ( year = 0; year < years_of_simulation; ++year )
		{
			/* for handling leap years */
			int days_per_month;

			index_vpsat = 0;
			for ( month = 0; month < MONTHS_COUNT; ++month )
			{
				days_per_month = DaysInMonth[month];
				if ( (FEBRUARY == month) && IS_LEAP_YEAR(matrix->cells[cell].years[year].year) )
				{
					++days_per_month;
				}

				for ( day = 0; day < days_per_month; ++day )
				{
					if( !day && !month && !year)
					{
						/* general summary on model simulation */
						simulation_summary(matrix);

						/* summary on site */
						site_summary(matrix);

						/* summary on topography */
						topo_summary(matrix);

						/* summary on soil */
						soil_summary(matrix, matrix->cells);

						if( F == matrix->cells[cell].landuse )
						{
							/* note: this happens just the first day of simulation */
							/* forest summary */
							forest_summary(matrix, cell, day, month, year);
						}
						else
						{
							/* include summary for other land uses */
						}
					}

					/* compute daily climate variables not coming from met data */
					Avg_temperature(matrix->cells[cell].years[year].m, day, month);
					Daylight_avg_temperature(matrix->cells[cell].years[year].m, day, month);
					Nightime_avg_temperature(matrix->cells[cell].years[year].m, day, month);
					Soil_temperature(matrix->cells[cell].years[year].m, day, month);
					Thermic_sum(matrix->cells[cell].years[year].m, day, month);
					Air_density(matrix->cells[cell].years[year].m, day, month);
					Day_Length(&matrix->cells[cell], day, month, year);
					Latent_heat(matrix->cells[cell].years[year].m, day, month);
					Air_pressure(matrix->cells[cell].years[year].m, day, month);
					Psychrometric(matrix->cells[cell].years[year].m, day, month);
					Sat_vapour_pressure(&matrix->cells[cell], day, month, year);
					Dew_temperature(matrix->cells[cell].years[year].m, day, month);
					Radiation(&matrix->cells[cell], day, month, year);
					Check_prcp(&matrix->cells[cell], day, month, year);
					//Annual_met_values(&matrix->cells[cell], day, month, year);
					Annual_CO2_concentration(matrix->cells[cell].years[year].m, day, month, year);

					if ( F == matrix->cells[cell].landuse )
					{
						/* compute annually the days for the growing season before any other process */
						Veg_Days (&matrix->cells[cell], day, month, year);

						//Marconi 18/06: function used to calculate VPsat from Tsoil following Hashimoto et al., 2011
						get_vpsat(&matrix->cells[cell], day, month, year, index_vpsat);
						++index_vpsat;
					}
					else
					{
						/* include other land use */
					}
				}
			}
			for ( month = 0; month < MONTHS_COUNT; ++month )
			{
				days_per_month = DaysInMonth[month];
				if ( (FEBRUARY == month) && IS_LEAP_YEAR(matrix->cells[cell].years[year].year) )
				{
					++days_per_month;
				}
				for ( day = 0; day < days_per_month; ++day )
				{
					/* print daily met data */
					print_daily_met_data (&matrix->cells[cell], day, month, year);

					/************************************************************************/
					if ( F == matrix->cells[cell].landuse && matrix->cells[cell].heights_count != 0 )
					{
						if ( 'f' == g_settings->version )
						{
							if ( !Tree_model_daily(matrix, cell, day, month, year) )
							{
								logger(g_log, "tree model daily failed!!!");
							}
							else
							{
								puts(msg_ok_tree_model);

								if (!string_compare_i(g_settings->dndc, "on"))
								{
									logger(g_log, "RUNNING DNDC.....\n");
									//TODO SERGIO
									//soil_dndc_sgm (matrix, cell, year, month, day, years_of_simulation);
									//soil_dndc......................................
								}
								else
								{
									//logger(g_log, "No soil simulation!!!\n");
								}
								get_net_ecosystem_exchange(&matrix->cells[cell]);
							}
						}
						else
						{
							/* run for BGC version */
						}
					}
					else
					{
						//						if ( ! crop_model_D(matrix, cell, year, month, day, years_of_simulation) ) {
						//							logger(g_log, "crop model failed.");
						//						}
						//						else
						//						{
						//							puts(msg_ok);
						//							//look if put it here or move before tree_model at the beginning of each month simulation
						//							//	soil_model (matrix, yos, years, month, years_of_simulation);
						//						}
					}
					//ALESSIOC
					/************************************************************************/
					if ( !Soil_model_daily(matrix, cell, day, month, year) )
					{
						logger(g_log, "soil model daily failed!!!");
					}
					else
					{
						puts(msg_ok_soil_model);
					}
					
					/*************************************************************************/

					//ALESSIOC
					/************************************************************************/
					
					if ( !Cell_model_daily(matrix, cell, day, month, year) )
					{
						logger(g_log, "cell model daily failed!!!");
					}
					else
					{
						puts(msg_ok_cell_model);
					}
					/*************************************************************************/

					// save values for put in output netcdf
					if ( output_vars && output_vars->daily_vars_count ) {
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
						/*
						#define YS					(matrix->y_cells_count)
						#define XS					(matrix->x_cells_count)
						#define ROWS				(366*years_of_simulation)
						#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
							int i;
							for ( i = 0; i < output_vars->daily_vars_count; ++i )
							{
								int row = get_daily_row_from_date(matrix->cells[cell].years[year].year, month, day) + (year*366);
								int index = VALUE_AT(matrix->cells[cell].x, matrix->cells[cell].y, row, i);
								if ( AR_DAILY_OUT == output_vars->daily_vars[i] )	output_vars->daily_vars_value[index] = matrix->cells[cell].daily_aut_resp;
								if ( GPP_DAILY_OUT == output_vars->daily_vars[i] )	output_vars->daily_vars_value[index] = matrix->cells[cell].daily_gpp;
								if ( NPP_DAILY_OUT == output_vars->daily_vars[i] )	output_vars->daily_vars_value[index] = matrix->cells[cell].daily_npp_gC;
							}
						#undef VALUE_AT
						#undef ROWS
						#undef XS
						#undef YS
						 */
						output_push_values(output_vars
								, &matrix->cells[cell]
								                 , month
								                 , day
								                 , year
								                 , years_of_simulation
								                 , matrix->x_cells_count
								                 , matrix->y_cells_count
								                 , OUTPUT_TYPE_DAILY
						);
					}
					/******************************************************************************/
					/* print daily output */
					EOD_print_cumulative_balance_cell_level (&matrix->cells[cell], day, month, year, years_of_simulation );

					/* reset daily variables once printed */
					reset_daily_class_variables ( &matrix->cells[cell] );
					reset_daily_layer_variables ( &matrix->cells[cell] );
					reset_daily_cell_variables  ( &matrix->cells[cell] );

					//					if ( ! string_compare_i(g_settings->dndc, "on") )
					//					{
					//						Get_EOD_soil_balance_cell_level (&matrix->cells[cell], year, month, day);
					//					}
					logger(g_log, "****************END OF DAY (%d)*******************\n", day + 1 );
				}

				/* save values for put in output netcdf */
				if ( output_vars && output_vars->monthly_vars_count ) {
					/*
				#define YS					(matrix->y_cells_count)
				#define XS					(matrix->x_cells_count)
				#define ROWS				(12*years_of_simulation)
				#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
					int i;
					for ( i = 0; i < output_vars->monthly_vars_count; ++i )
					{
						int row = month + (year*12);
						int index = VALUE_AT(matrix->cells[cell].x, matrix->cells[cell].y, row, i);
						if ( AR_MONTHLY_OUT == output_vars->monthly_vars[i] ) output_vars->monthly_vars_value[index] = matrix->cells[cell].monthly_aut_resp;
						if ( GPP_MONTHLY_OUT == output_vars->monthly_vars[i] ) output_vars->monthly_vars_value[index] = matrix->cells[cell].monthly_gpp;
						if ( NPP_MONTHLY_OUT == output_vars->monthly_vars[i] ) output_vars->monthly_vars_value[index] = matrix->cells[cell].monthly_npp_gC;
					}
				#undef VALUE_AT
				#undef ROWS
				#undef XS
				#undef YS
					 */
					output_push_values(output_vars
							, &matrix->cells[cell]
							                 , month
							                 , day
							                 , year
							                 , years_of_simulation
							                 , matrix->x_cells_count
							                 , matrix->y_cells_count
							                 , OUTPUT_TYPE_MONTHLY
					);
				}
				/******************************************************************************/
				/* print monthly output */
				EOM_print_cumulative_balance_cell_level( &matrix->cells[cell], month, year, years_of_simulation );

				/* reset monthly variables once printed */
				if ( ( IS_LEAP_YEAR( matrix->cells[cell].years[year].year ) ? (MonthLength_Leap[month]) : (MonthLength[month] )) == matrix->cells[cell].doy )
				{
					reset_monthly_class_variables ( &matrix->cells[cell] );
					reset_monthly_layer_variables ( &matrix->cells[cell] );
					reset_monthly_cell_variables  ( &matrix->cells[cell] );
				}
				logger(g_log, "****************END OF MONTH (%d)*******************\n\n", month + 1 );
			}

			// save values for put in output netcdf
			if ( output_vars && output_vars->yearly_vars_count ) {
				/*
			#define YS					(matrix->y_cells_count)
			#define XS					(matrix->x_cells_count)
			#define ROWS				(years_of_simulation)
			#define VALUE_AT(x,y,r,c)	((x)+(XS)*((y)+(YS)*((r)+(ROWS)*(c))))
				int i;
				for ( i = 0; i < output_vars->yearly_vars_count; ++i )
				{
					int index = VALUE_AT(matrix->cells[cell].x, matrix->cells[cell].y, year, i);
					if ( AR_YEARLY_OUT == output_vars->yearly_vars[i] ) output_vars->yearly_vars_value[index] = matrix->cells[cell].annual_aut_resp;
					if ( GPP_YEARLY_OUT == output_vars->yearly_vars[i] ) output_vars->yearly_vars_value[index] = matrix->cells[cell].annual_gpp;
					if ( NPP_YEARLY_OUT == output_vars->yearly_vars[i] ) output_vars->yearly_vars_value[index] = matrix->cells[cell].annual_npp_gC;
				}
			#undef VALUE_AT
			#undef ROWS
			#undef XS
			#undef YS
				 */
				output_push_values(output_vars
						, &matrix->cells[cell]
						                 , month
						                 , day
						                 , year
						                 , years_of_simulation
						                 , matrix->x_cells_count
						                 , matrix->y_cells_count
						                 , OUTPUT_TYPE_YEARLY
				);
			}
			/******************************************************************************/
			/* print annual output */
			EOY_print_cumulative_balance_cell_level( &matrix->cells[cell], year, years_of_simulation );

			/* reset annual variables once printed */
			if ( ( IS_LEAP_YEAR( matrix->cells[cell].years[year].year ) ? (MonthLength_Leap[DECEMBER]) : (MonthLength[DECEMBER] )) == matrix->cells[cell].doy )
			{
				reset_annual_class_variables ( &matrix->cells[cell] );
				reset_annual_layer_variables ( &matrix->cells[cell] );
				reset_annual_cell_variables  ( &matrix->cells[cell] );
			}
			logger(g_log, "****************END OF YEAR (%d)*******************\n", matrix->cells[cell].years[year].year );
		}

		index_vpsat = matrix->cells[cell].years[0].year;
		free(matrix->cells[cell].years);
		matrix->cells[cell].years = NULL; /* required */
	}

	/* NETCDF output */
	if ( output_vars && output_vars->daily_vars_value ) {
		char *path = get_path(g_sz_daily_output_filename);
		if ( ! path && g_sz_daily_output_filename  ) {
			logger(g_log, sz_err_out_of_memory);
			goto err;
		}
		ret = output_write(output_vars, path, start_year, years_of_simulation, matrix->x_cells_count, matrix->y_cells_count, 0);
		free(path);
		if ( ! ret ) {
			logger(g_log, sz_err_out_of_memory);
			goto err;
		}
		free(output_vars->daily_vars_value);
		output_vars->daily_vars_value = NULL;
	}

	if ( output_vars && output_vars->monthly_vars_value ) {
		char *path = get_path(g_sz_monthly_output_filename);
		if ( ! path && g_sz_monthly_output_filename ) {
			logger(g_log, sz_err_out_of_memory);
			goto err;
		}
		ret = output_write(output_vars, path, start_year, years_of_simulation, matrix->x_cells_count, matrix->y_cells_count, 1);
		free(path);
		if ( ! ret ) {
			logger(g_log, sz_err_out_of_memory);
			goto err;
		}
		free(output_vars->monthly_vars_value);
		output_vars->monthly_vars_value = NULL;
	}

	if ( output_vars && output_vars->yearly_vars_value ) {
		char *path = get_path(g_sz_yearly_output_filename);
		if ( ! path && g_sz_yearly_output_filename) {
			logger(g_log, sz_err_out_of_memory);
			goto err;
		}
		ret = output_write(output_vars, path, start_year, years_of_simulation, matrix->x_cells_count, matrix->y_cells_count, 2);
		free(path);
		if ( ! ret ) {
			logger(g_log, sz_err_out_of_memory);
			goto err;
		}
		free(output_vars->yearly_vars_value);
		output_vars->yearly_vars_value = NULL;
	}

	/* free memory */
	matrix_free(matrix); matrix = NULL;

	/* ok ! */
	prog_ret = 0;

	err:
	/* close logger */
	logger_close(g_soil_log); g_soil_log = NULL;
	logger_close(g_annual_log); g_annual_log = NULL;
	logger_close(g_monthly_log); g_monthly_log = NULL;
	logger_close(g_daily_log); g_daily_log = NULL;
	logger_close(g_log); g_log = NULL;

	/* free memory */
	if ( output_vars ) output_free(output_vars);
	if ( matrix ) matrix_free(matrix);

	free(g_sz_input_met_file); g_sz_input_met_file = NULL;
	free(g_sz_debug_output_filename); g_sz_debug_output_filename = NULL;
	free(g_sz_daily_output_filename); g_sz_daily_output_filename = NULL;
	free(g_sz_monthly_output_filename); g_sz_monthly_output_filename = NULL;
	free(g_sz_yearly_output_filename); g_sz_yearly_output_filename = NULL;
	free(g_sz_soil_output_filename); g_sz_soil_output_filename = NULL;
	free(g_sz_settings_file); g_sz_settings_file = NULL;

	/* this should be freed before */
	free(g_sz_topo_file); g_sz_topo_file = NULL;
	free(g_sz_soil_file); g_sz_soil_file = NULL;

	return prog_ret;
}
