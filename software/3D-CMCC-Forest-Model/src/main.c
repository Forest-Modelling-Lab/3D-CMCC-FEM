/* main.c */

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
#include "compare.h"

//#define BENCHMARK_ONLY

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
logger_t* g_debug_log = NULL;
logger_t* g_daily_log = NULL;
logger_t* g_monthly_log = NULL;
logger_t* g_annual_log = NULL;
logger_t* g_soil_log = NULL;
soil_settings_t* g_soil_settings = NULL;
topo_t* g_topo = NULL;
settings_t* g_settings = NULL;

/* DO NOT REMOVE INITIALIZATION TO NULL, IT IS REQUIRED !! */
char	*g_sz_parameterization_path = NULL
		, *g_sz_input_path = NULL
		, *g_sz_output_path = NULL
		, *g_sz_dataset_file = NULL
		, *g_sz_input_met_file = NULL
		, *g_sz_soil_file = NULL
		, *g_sz_topo_file = NULL
		, *g_sz_settings_file =	NULL
		, *g_sz_ndep_file = NULL
		, *g_sz_co2_conc_file = NULL
		, *g_sz_output_vars_file = NULL
		, *g_sz_benchmark_path = NULL
		;

int g_year_start_index;
char g_sz_parameterization_output_path[256];
char g_sz_output_fullpath[256];

static int years_of_simulation;	// default is none
/* strings */
const char sz_launched[] = "\n"PROGRAM_FULL_NAME"\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"using NetCDF %s\n"
		"launched: %s\n"
		"--------------------------------------------------------------------------------\n";

static const char banner[] = "\n"PROGRAM_FULL_NAME"\n"
		"compiled using "COMPILER" on "__DATE__" at "__TIME__"\n"
		"using NetCDF %s\n"
		/*
#if defined _WIN32
		"previous commit hash: "COMMIT_HASH"\n"
#endif
		 */
		"(use -h parameter for more information)\n\n"
		"The "PROGRAM_FULL_NAME" has been developed by:\n"
		"Alessio Collalti [alessio.collalti@cmcc.it, a.collalti@unitus.it],\n"
		"Alessio Ribeca [alessio.ribeca@cmcc.it]\n"
		"Carlo Trotta [trottacarlo@unitus.it]"
		"Monia Santini [monia.santini@cmcc.it]\n"
		"euroMediterranean Center on Climate Changes (CMCC),\n"
		"IAFES division,\n"
		"Viale Trieste 120, 01100 - Viterbo, Italy,\n"
		"and \n"
		"Tuscia University (UNITUS),\n"
		"Department for innovation in biological, agro-food and forest systems (DIBAF),\n"
		"Forest Ecology Lab\n"
		"\n"
		"Programmers: Alessio Collalti - Alessio Ribeca - Carlo Trotta - Monia Santini\n"
		"\n"
		"\"DISCLAIMER\"\n"
		"CMCC and UNITUS\n"
		"accepts no responsibility for the use of the 3D_CMCC FEM in\n"
		"the form supplied or as subsequently modified by third parties.\n"
		"CMCC and UNITUS disclaims liability for all losses,\n"
		"damages and costs incurred by any person as a result of relying on this software.\n"
		"Use of this software assumes agreement to this condition of use.\n"
		"Removal of this statement violates the spirit in which 3D-CMCC FEM,\n"
		"was released by CMCC-UNITUS.\n"
		"for more information see:\n"
		"-Collalti et al., 2014 Ecological Modelling,\n"
		"-Collalti et al., 2016 Geoscientific Model Development\n"
		"--------------------------------------------------------------------------------\n";
static const char msg_input_path[]				=	"input path = %s\n";
static const char msg_parameterization_path[]	=	"parameterization path = %s\n";
static const char msg_soil_file[]				=	"soil file = %s\n";
static const char msg_topo_file[]				=	"topo file = %s\n";
static const char msg_met_path[]				=	"met path = %s\n";
static const char msg_settings_path[]			=	"settings path = %s\n";
static const char msg_co2_conc_file[]			=	"co2 conc file path = %s\n";
static const char msg_ndep_file[]				=	"ndep file path = %s\n";
static const char msg_debug_output_file[]		=	"debug output file path = %s\n";
static const char msg_daily_output_file[]		=	"daily output file path = %s\n";
static const char msg_monthly_output_file[]		=	"monthly output file path = %s\n";
static const char msg_annual_output_file[]		=	"annual output file path = %s\n";
static const char msg_soil_output_file[]		=	"soil output file path = %s\n";
static const char msg_ok[]						=	"ok ";
static const char msg_usage[]					=	"\nusage:\n"
		"  3D-CMCC-Forest-Model -i INPUT_DIR -d DATASET_FILENAME -m MET_FILE_LIST -s SITE_FILENAME -c SETTINGS_FILENAME [-o OUTPUT_FILENAME] [-h]\n"
		"  required options:\n"
#ifdef _WIN32
		"    -i input path (i.e.: -i c:\\input\\directory\\)\n"
		"    -o output path (i.e.: -o c:\\output\\directory\\)\n"
		"    -p parameterization directory (i.e.: -i c:\\parameterization\\directory\\)\n"
#else
		"    -i input directory (i.e.: -i path/to/input/folder/)\n"
		"    -o output directory (i.e.: -i path/to/output/folder/)\n"
		"    -p parameterization directory (i.e.: -i path/to/parameterization/directory/)\n"
#endif
		"    -d dataset filename stored into input directory (i.e.: -d input.txt)\n"
		"    -m met filename list stored into input directory (i.e.: -m 1999.txt,2003.txt,2009.txt)\n"
		"    -s soil filename stored into input directory (i.e.: -s soil.txt or soil.nc)\n"
		"    -t topo filename stored into input directory (i.e.: -t topo.txt or topo.nc)\n"
		"    -c settings filename stored into input directory (i.e.: -c settings.txt)\n"
		"    -k co2 concentration file (i.e.: -k co2_conc.txt)\n"
		"    -n ndep file (i.e.: -n ndep.txt)\n"
		"    -r output vars list (i.e.: -r output_vars.lst)\n"
		"    -u benchmark path\n"
		"    -h print this help\n"
		;

/* error messages */
extern const char sz_err_out_of_memory[];
const char err_unable_open_file[] = "unable to open file.";
const char err_empty_file[] = "empty file ?";
const char err_window_size_too_big[] = "window size too big.";
//static const char err_unable_get_current_path[] = "unable to retrieve current path.\n";
static const char err_unable_to_register_atexit[] = "unable to register clean-up routine.\n";

static const char* get_filename(const char *const s)
{
	const char *p;
	const char *p2;

	p = NULL;

	if ( s ) {
		p = strrchr(s, '/');
		if ( p ) ++p;
		p2 = strrchr(s, '\\');
		if ( p2 ) ++p2;
		if ( p2 > p ) p = p2;
		if ( ! p ) p = s;
	}

	return p;
}


static void clean_up(void)
{
	if ( g_sz_benchmark_path ) free(g_sz_benchmark_path);
	if ( g_sz_output_vars_file ) free(g_sz_output_vars_file);
	if ( g_sz_ndep_file ) free(g_sz_ndep_file);
	if ( g_sz_co2_conc_file ) free(g_sz_co2_conc_file);
	if ( g_sz_settings_file ) free(g_sz_settings_file);
	/*
	if ( g_sz_debug_output_filename ) free(g_sz_debug_output_filename);
	if ( g_sz_daily_output_filename ) free(g_sz_daily_output_filename);
	if ( g_sz_monthly_output_filename ) free(g_sz_monthly_output_filename);
	if ( g_sz_yearly_output_filename ) free(g_sz_yearly_output_filename);
	if ( g_sz_soil_output_filename ) free(g_sz_soil_output_filename);
	 */
	if ( g_sz_parameterization_path ) free(g_sz_parameterization_path);
	if ( g_sz_output_path ) free(g_sz_output_path);
	if ( g_sz_input_path ) free(g_sz_input_path);
	//if ( g_topo ) free(g_topo);
	if ( g_sz_topo_file ) free(g_sz_topo_file);
	//if ( g_soil_settings ) free(g_soil_settings);
	if ( g_sz_soil_file ) free(g_sz_soil_file);
	if ( g_settings ) free(g_settings);
	if ( g_sz_input_met_file ) free(g_sz_input_met_file);
	if ( g_sz_dataset_file ) free(g_sz_dataset_file);

#ifdef _WIN32
#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	_CrtDumpMemoryLeaks();

	system("PAUSE");
#endif
#endif
}

static void show_usage(void)
{
	puts(msg_usage);
}

static int parameterization_output_create(const char* const sz_date) {
	int i;

	assert(sz_date);

	i = has_path_delimiter(g_sz_output_path);
	
	/* create parameterization folder */
	sprintf(g_sz_parameterization_output_path, "%s%soutput_%s_%s%sparameterization%s"
												, g_sz_output_path
												, i ? "" : FOLDER_DELIMITER
												, PROGRAM_VERSION
												, sz_date
												, FOLDER_DELIMITER
												, FOLDER_DELIMITER
	);

	return path_create(g_sz_parameterization_output_path);
}

static int log_start(const char* const sz_date, const char* const sitename)
{
	char *p;
	char buffer[128];	/* should be enough */
	char date[32];		/* should be enough */
	int len;
	struct tm* ptm;
	time_t t;

	/* get current date */
	time(&t);
	ptm = gmtime(&t);
	sprintf(date, "%04d_%s_%02d"
			, ptm->tm_year+1900
			, szMonth[ptm->tm_mon]
			          , ptm->tm_mday
	);

	/* build log filename */

	/* program version */
	len = sprintf(buffer, "_%s", PROGRAM_VERSION);

	/* sitename */
	if ( sitename && sitename[0] ) {
		len += sprintf(buffer+len, "_%s", sitename);
	}

	/* setting version */
	len += sprintf(buffer+len, "_%s", get_filename(g_sz_input_met_file));

	/* co2_fixed */
	len += sprintf(buffer+len, "_(%d", g_settings->year_start);

	/* end simulation */
	len += sprintf(buffer+len, "-%d)", g_settings->year_end);

	/* setting version */
	//len += sprintf(buffer+len, "_%c", g_settings->version);

	/* co2_transient */
	switch ( g_settings->CO2_trans ) {
		case CO2_TRANS_ON:
			p = "ON";
			break;

		case CO2_TRANS_OFF:
			p = "OFF";
			break;

		case CO2_TRANS_VAR:
			p = "VAR";
			break;

		default:
			puts("bad CO2_trans value in settings file!\n");
			return 0;
	}
	len += sprintf(buffer+len, "_CO2_%s", p);

	/* management */
	if ( g_settings->management )
	{
		p = "ON";
	}
	else
	{
		p = "OFF";
	}
	len += sprintf(buffer+len, "_Manag_%s", p);

	/* time */
	len += sprintf(buffer+len, "_%c", g_settings->time);

	/* sizeCell */
	len += sprintf(buffer+len, "_%d", (int)g_settings->sizeCell);

	/* date */
	//len += sprintf(buffer+len, "_%s", date);

	/* dndc */
	if ( g_settings->dndc )
	{
		len += sprintf(buffer+len, "_DNDC");
	}

	/* files type */
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
		len += sprintf(buffer+len, "%s", ext);
	}

	/* extension */
	sprintf(buffer+len, ".txt");

	/* create log files and parameterization folder */
	{
		int i;
		int flag;
		int log_flag[5];
		char* log_name[5] = {	"debug"
				, "daily"
				, "monthly"
				, "annual"
				, "soil"
		};

		logger_t** logs[5] = {
				&g_debug_log
				, &g_daily_log
				, &g_monthly_log
				, &g_annual_log
				, &g_soil_log
		};

		log_flag[0] = g_settings->debug_output;
		log_flag[1] = g_settings->daily_output;
		log_flag[2] = g_settings->monthly_output;
		log_flag[3] = g_settings->yearly_output;
		log_flag[4] = g_settings->soil_output;

		flag = 0;
		if ( g_sz_output_path ) {
			int len = strlen(g_sz_output_path);
			flag = (('/' == g_sz_output_path[len-1]) || ('\\' == g_sz_output_path[len-1]));
		}

		for ( i = 0 ; i < 5; ++i ) {
			if ( log_flag[i] ) {
				*logs[i] = logger_new("%s%soutput_%s_%s%s%s%s%s%s"
										, g_sz_output_path
										, flag ? "" : FOLDER_DELIMITER
										, PROGRAM_VERSION
										, date
										, FOLDER_DELIMITER
										, log_name[i]
										, FOLDER_DELIMITER
										, log_name[i]
										, buffer
				);
				if ( ! *logs[i] ) {
					printf("Unable to create %s log!\n\n",log_name[i]);
					return 0;
				}
			}
		}
	}
		
	/* disable screen output when "off" */
	if ( ! g_settings->screen_output ) { 
		logger_disable_std(g_debug_log);
	}

	logger(g_debug_log, sz_launched, netcdf_get_version(), datetime_current());

	/* show paths */
	logger(g_debug_log,"\nFILE NAMES AND PATHS\n");
	if ( g_sz_input_path )
	{
		logger(g_debug_log, msg_input_path, g_sz_input_path);
	}
	logger(g_debug_log, msg_parameterization_path, g_sz_parameterization_path);
	logger(g_debug_log, msg_soil_file, g_sz_soil_file);
	logger(g_debug_log, msg_topo_file, g_sz_topo_file);
	logger(g_debug_log, msg_met_path, g_sz_input_met_file);

	if ( g_sz_co2_conc_file ) 
		logger(g_debug_log, msg_co2_conc_file, g_sz_co2_conc_file);

	if ( g_sz_ndep_file ) 
		logger(g_debug_log, msg_ndep_file, g_sz_ndep_file);

	logger(g_debug_log, msg_settings_path, g_sz_settings_file);
	if ( g_debug_log )						logger(g_debug_log, msg_debug_output_file, g_debug_log->filename);
	if ( g_debug_log && g_daily_log )		logger(g_debug_log, msg_daily_output_file, g_daily_log->filename);
	if ( g_debug_log && g_monthly_log )		logger(g_debug_log, msg_monthly_output_file, g_monthly_log->filename);
	if ( g_debug_log && g_annual_log )		logger(g_debug_log, msg_annual_output_file, g_annual_log->filename);
	if ( g_debug_log && g_soil_log )		logger(g_debug_log, msg_soil_output_file, g_soil_log->filename);

	if ( g_daily_log ) g_daily_log->std_output = 0;
	if ( g_monthly_log ) g_monthly_log->std_output = 0;
	if ( g_annual_log ) g_annual_log->std_output = 0;

	return 1;
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



char* concatenate_path(char* s1, char* s2) {
	char *p;
	int i;
	int ii;
	int flag;

	assert(s1 && s2);

	i = strlen(s1);
	flag = ! (('\\' == s1[i-1]) || ('/' == s1[i-1]));
	ii = strlen(s2);

	i += flag + ii + 1;

	p = malloc(i*sizeof*p);
	if ( p ) {
		sprintf(p, "%s%s%s", s1
							, flag ? FOLDER_DELIMITER : ""
							, s2);
	}
	return p;
}

/*
	parse and check passed args
 */
static int parse_args(int argc, char *argv[])
{
	char* p;
	int i;

	g_sz_input_path = NULL;
	g_sz_output_path = NULL;
	g_sz_parameterization_path = NULL;
	g_sz_dataset_file = NULL;
	g_sz_input_met_file = NULL;
	g_sz_soil_file = NULL;
	g_sz_topo_file = NULL;
	g_sz_settings_file = NULL;
	g_sz_output_vars_file = NULL;
	g_sz_benchmark_path = NULL;

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

		case 'o': /* output folder*/
			if ( ! argv[i+1] ) {
				puts("output folder not specified");
				goto err;
			}
			g_sz_output_path = string_copy(argv[i+1]);
			if ( ! g_sz_output_path ) {
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

		case 'k': /* co2 file */
			if ( ! argv[i+1] ) {
				puts("co2 conc file not specified!");
				goto err;
			}
			g_sz_co2_conc_file = string_copy(argv[i+1]);
			if( ! g_sz_co2_conc_file ) {
				puts(sz_err_out_of_memory);
				goto err;
			}
			break;

		case 'n': /* ndep file */
			if ( ! argv[i+1] ) {
				puts("ndep file not specified!");
				goto err;
			}
			g_sz_ndep_file = string_copy(argv[i+1]);
			if( ! g_sz_ndep_file ) {
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

		case 'u': /* compare path */
			if ( ! argv[i+1] ) {
				puts("compare path not specified!");
				goto err;
			}
			g_sz_benchmark_path = string_copy(argv[i+1]);
			if( ! g_sz_benchmark_path ) {
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
	
	/* output path specified ? */
	if ( ! g_sz_output_path ) {
		g_sz_output_path = string_copy("output");
		if ( ! g_sz_output_path ) {
			puts(sz_err_out_of_memory);
			goto err;
		}
	}

	if ( ! g_sz_parameterization_path ) {
		puts("parameterization path not specified!");
		goto err_show_usage;
	}

	if ( g_sz_dataset_file && g_sz_input_path ) {
		p = concatenate_path(g_sz_input_path, g_sz_dataset_file); 
		if ( ! p )
		{
			puts(sz_err_out_of_memory);
			goto err;
		}
		free(g_sz_dataset_file);
		g_sz_dataset_file = p;
	}

	if ( ! g_sz_input_met_file ) {
		puts("met file list is missing!");
		goto err_show_usage;
	}  else if ( g_sz_input_path ) {
		p = concatenate_path(g_sz_input_path, g_sz_input_met_file); 
		if ( ! p )
		{
			puts(sz_err_out_of_memory);
			goto err;
		}
		free(g_sz_input_met_file);
		g_sz_input_met_file = p;
	}

	if ( ! g_sz_soil_file ) {
		puts("soil filename not specified!");
		goto err_show_usage;
	} else if ( g_sz_input_path ) {
		p = concatenate_path(g_sz_input_path, g_sz_soil_file); 
		if ( ! p )
		{
			puts(sz_err_out_of_memory);
			goto err;
		}
		free(g_sz_soil_file);
		g_sz_soil_file = p;
	}

	if ( ! g_sz_topo_file ) {
		puts("topo filename option is missing!");
		goto err_show_usage;
	 } else if ( g_sz_input_path ) {
		p = concatenate_path(g_sz_input_path, g_sz_topo_file); 
		if ( ! p )
		{
			puts(sz_err_out_of_memory);
			goto err;
		}
		free(g_sz_topo_file);
		g_sz_topo_file = p;
	}

	if ( ! g_sz_settings_file ) {
		puts("settings filename option is missing!");
		goto err_show_usage;
	} else if ( g_sz_input_path ) {
		p = concatenate_path(g_sz_input_path, g_sz_settings_file); 
		if ( ! p )
		{
			puts(sz_err_out_of_memory);
			goto err;
		}
		free(g_sz_settings_file);
		g_sz_settings_file = p;
	}

	if ( g_sz_output_vars_file ) {
		p = concatenate_path(g_sz_input_path, g_sz_output_vars_file); 
		if ( ! p )
		{
			puts(sz_err_out_of_memory);
			goto err;
		}
		free(g_sz_output_vars_file);
		g_sz_output_vars_file = p;
	}

	return 1;

err_show_usage:
	show_usage();

	err:
	return 0;
}

int main(int argc, char *argv[]) {
	char sz_date[32]; // should be enough
	int ret;
	int year;
	int month;
	int day;
	int cell;
	int prog_ret;
	int soil_settings_count;
	int topos_count;
	double timer;
	double start_timer;
	double end_timer;
	matrix_t* matrix;
	output_t* output_vars;
	soil_settings_t* s;
	topo_t* t;

	//_CrtSetBreakAlloc();

	/* initialize */
	matrix = NULL;
	output_vars = NULL;
	t = NULL;
	s = NULL;
	prog_ret = 1;

	/* start timer */
	timer_init();
	start_timer = timer_get();

	/* get current date */
	{
		struct tm* ptm;
		time_t t;

		/* get current date */
		time(&t);
		ptm = gmtime(&t);
		sprintf(sz_date, "%04d_%s_%02d"
					, ptm->tm_year + 1900
					, szMonth[ptm->tm_mon]
					, ptm->tm_mday
		);
	}

	/* register atexit */
	if ( -1 == atexit(clean_up) ) {
		puts(err_unable_to_register_atexit);
		return 1;
	}

	/* show banner */
	printf(banner, netcdf_get_version());

	/* parse arguments */
	printf("parsing arguments...");
	if ( ! parse_args(argc, argv) ) {
		return 1;
	}
	puts(msg_ok);

	if ( ! parameterization_output_create(sz_date) ) {
		puts("Unable to create parameterization output path\n");
		return 0;
	}

	/* import output vars file ? */
	if ( g_sz_output_vars_file ) {
		printf("import output file...");
		output_vars = output_import(g_sz_output_vars_file);
		free(g_sz_output_vars_file);
		g_sz_output_vars_file = NULL;
		if ( ! output_vars ) {
			return 1;
		}
		puts(msg_ok);
	}

	if ( g_sz_benchmark_path && ! g_sz_output_path ) {
		puts("unable to benchmark. output path not specified!");
		return 0;
	}

#ifdef BENCHMARK_ONLY
	goto benchmark;
#endif

	printf("import settings file %s...", g_sz_settings_file);
	g_settings = settings_import(g_sz_settings_file);
	if ( ! g_settings )
	{
		goto err;
	}
	if ( g_settings->time != 'd' ) {
		puts("uncorrect time step choiced!");
		goto err;
	} else {
		puts(msg_ok);
	}

	if ( ! g_settings->Ndep_fixed && ! g_sz_ndep_file ) {
		puts("ndep file not specified for ndep_fixed off");
		goto err;
	}
	
	printf("soil import...");
	s = soil_settings_import(g_sz_soil_file, &soil_settings_count);
	if ( ! s ) {
		goto err;
	}
	puts(msg_ok);

	printf("topo import...");
	t = topo_import(g_sz_topo_file, &topos_count);
	if ( ! t ) {
		goto err;
	}
	puts(msg_ok);

	printf("build matrix");
	if ( g_sz_dataset_file )
		 printf("using %s...", g_sz_dataset_file);
	else
		printf("...");
	matrix = matrix_create(s, soil_settings_count, g_sz_dataset_file);
	if ( ! matrix ) goto err;
	puts(msg_ok);

	g_year_start_index = -1;
	
	logger(g_debug_log, "\n3D-CMCC FEM START....\n\n");
	
	for ( cell = 0; cell < matrix->cells_count; ++cell )
	{
		logger(g_debug_log, "Processing met data files for cell at %d,%d...\n", matrix->cells[cell].x, matrix->cells[cell].y);
		logger(g_debug_log, "input_met_path = %s\n", g_sz_input_met_file);

		/* set g_soil_settings and g_topo */
		{
			int i;
			int index = -1;

			/* soil settings */
			for ( i = 0; i < soil_settings_count; ++i )
			{
				if ( (matrix->cells[cell].x == (int)s[i].values[SOIL_X])
						&& (matrix->cells[cell].y == (int)s[i].values[SOIL_Y]) )
				{
					index = i;
					break;
				}
			}

			if ( -1 == index )
			{
				logger_error(g_debug_log, "no soil settings found for cell at %d,%d\n"
												, matrix->cells[cell].x
												, matrix->cells[cell].y
				);
				continue;
			}
			g_soil_settings = &s[index];

			/* topo */
			for ( i = 0; i < topos_count; ++i )
			{
				if ( (matrix->cells[cell].x == (int)t[i].values[TOPO_X])
						&& (matrix->cells[cell].y == (int)t[i].values[TOPO_Y]) )
				{
					index = i;
					break;
				}
			}

			if ( -1 == index )
			{
				logger_error(g_debug_log, "no topo settings found for cell at %d,%d\n"
												, matrix->cells[cell].x
												, matrix->cells[cell].y
				);
				continue;
			}
			g_topo = &t[index];
		}

		/* only for first cell */
		if ( 0 == cell ) {
			if ( ! log_start(sz_date, g_settings->sitename) ) {
				goto err;
			}
		}

		if (	IS_INVALID_VALUE(g_soil_settings->values[SOIL_SAND_PERC])
				|| IS_INVALID_VALUE(g_soil_settings->values[SOIL_CLAY_PERC])
				|| IS_INVALID_VALUE(g_soil_settings->values[SOIL_SILT_PERC])
				|| IS_INVALID_VALUE(g_soil_settings->values[SOIL_DEPTH]) ) {
			logger_error(g_debug_log, "NO SOIL DATA AVAILABLE");
			goto err;
		}

		/* check hemisphere */
		if ( g_soil_settings->values[SOIL_LAT] > 0 ) {
			matrix->cells[cell].north = 0;
		} else {
			matrix->cells[cell].north = 1;
		}

		logger_error(g_debug_log, "importing met data...");
		matrix->cells[cell].years = yos_import(g_sz_input_met_file, &years_of_simulation, matrix->cells[cell].x, matrix->cells[cell].y);
		if ( ! matrix->cells[cell].years ) goto err;
		logger_error(g_debug_log, "ok\n");

		/* set start year index */
		if ( -1 == g_year_start_index ) {
			int i;
			int ii;

			for ( i = 0; i < years_of_simulation; ++i ) {
				if ( g_settings->year_start == matrix->cells[0].years[i].year ) {
					g_year_start_index = i;
					break;
				}
			}
			if ( -1 == g_year_start_index ) {
				logger_error(g_debug_log, "start year (%d) not found. range is %d-%d\n"
												, g_settings->year_start
												, matrix->cells[0].years[0].year
												, matrix->cells[0].years[years_of_simulation-1].year
				);
				goto err;
			}

			/* set end year (adjusting years_of_simulation) */
			ii = -1;
			for ( i = 0; i < years_of_simulation; ++i ) {
				if ( g_settings->year_end == matrix->cells[0].years[i].year ) {
					if ( g_year_start_index > i ) {
						logger_error(g_debug_log, "start year (%d) cannot be > end year (%d)\n"
												, g_settings->year_start
												, g_settings->year_end
						);
						goto err;
					}
					ii = i;
					break;
				}
			}
			if ( -1 == ii ) {
				logger_error(g_debug_log, "end year (%d) not found. range is %d-%d\n"
												, g_settings->year_end
												, matrix->cells[0].years[0].year
												, matrix->cells[0].years[years_of_simulation-1].year
				);
				goto err;
			}
			/* i is year_end_index */
			i = ii + 1;

			years_of_simulation = i - g_year_start_index;
		}

		/* move pointer for year */
		matrix->cells[cell].years += g_year_start_index;

		/* alloc memory for daily output netcdf vars (if any) */
		if ( output_vars && output_vars->daily_vars_count && ! output_vars->daily_vars_value ) {
			int ii;
			int rows_count = matrix->cells_count*years_of_simulation*366*output_vars->daily_vars_count;
			output_vars->daily_vars_value = malloc(rows_count*sizeof*output_vars->daily_vars_value);
			if ( ! output_vars->daily_vars_value ) {
				logger_error(g_debug_log, sz_err_out_of_memory);
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
				logger_error(g_debug_log, sz_err_out_of_memory);
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
				logger_error(g_debug_log, sz_err_out_of_memory);
				goto err;
			}
			for ( ii = 0; ii < rows_count; ++ii ) {
				output_vars->yearly_vars_value[ii] = INVALID_VALUE;
			}
		}

		logger(g_debug_log, "Total years_of_simulation = %d\n", years_of_simulation);
		logger(g_debug_log, "***************************************************\n\n");

		for ( year = 0; year < years_of_simulation; ++year )
		{
			/* for handling leap years */
			int days_per_month;

			for ( month = 0; month < MONTHS_COUNT; ++month )
			{
				days_per_month = DaysInMonth[month];
				if ( (FEBRUARY == month) && IS_LEAP_YEAR(matrix->cells[cell].years[year].year) )
				{
					++days_per_month;
				}

				for ( day = 0; day < days_per_month; ++day )
				{
					if( !day && !month && ! year )
					{
						/* general summary on model simulation */
						simulation_summary(matrix);

						/* summary on site */
						site_summary(matrix);

						/* summary on topography */
						topo_summary(matrix);

						/* summary on soil */
						soil_summary(matrix, matrix->cells);

						if( LANDUSE_F == g_soil_settings->landuse )
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
					Thermic_sum(matrix->cells[cell].years[year].m, day, month, year);
					Air_density(matrix->cells[cell].years[year].m, day, month);
					Day_Length(&matrix->cells[cell], day, month, year);
					Latent_heat(matrix->cells[cell].years[year].m, day, month);
					Air_pressure(matrix->cells[cell].years[year].m, day, month);
					Psychrometric(matrix->cells[cell].years[year].m, day, month);
					Sat_vapour_pressure(&matrix->cells[cell], day, month, year);
					Dew_temperature(matrix->cells[cell].years[year].m, day, month);
					Radiation(&matrix->cells[cell], day, month, year);
					Check_prcp(&matrix->cells[cell], day, month, year);

					if ( LANDUSE_F == g_soil_settings->landuse )
					{
						/* compute annually the days for the growing season before any other process */
						Veg_Days (&matrix->cells[cell], day, month, year);
					}
					else
					{
						/* include other land use ???? */
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

					/* print cell data */
					print_daily_cell_data ( &matrix->cells[cell] );

					/************************************************************************/
					if ( g_sz_dataset_file )
					{
						if ( (LANDUSE_F == g_soil_settings->landuse) && (matrix->cells[cell].heights_count != 0) )
						{
							if ( 'f' == g_settings->version )
							{
								if ( !Tree_model_daily( matrix, cell, day, month, year ) )
								{
									logger(g_debug_log, "tree model daily failed!!!");
								}
								else
								{
									printf("ok tree_model (%02d-%02d-%d)\n", day+1, month+1, year+g_settings->year_start);
								}
							}
							else
							{
								/* run for BGC version */
							}
						}
						else
						{
							//if ( ! crop_model_D(matrix, cell, year, month, day, years_of_simulation) )
							//{
							//	logger(g_debug_log, "crop model failed.");
							//}
							//else
							//{
							//	puts(msg_ok);
							//	//look if put it here or move before tree_model at the beginning of each month simulation
							//	soil_model (matrix, yos, years, month, years_of_simulation);
							//}
						}
					}

					/************************************************************************/
					/* run for soil model */
					if ( !Soil_model_daily(matrix, cell, day, month, year) )
					{
						logger_error(g_debug_log, "soil model daily failed!!!");
					}
					else
					{
						printf("ok soil_model (%02d-%02d-%d)\n", day+1, month+1, year+g_settings->year_start);
					}
					/************************************************************************/
					/* run for cell model */
					if ( !Cell_model_daily(matrix, cell, day, month, year) )
					{
						logger_error(g_debug_log, "cell model daily failed!!!");
					}
					else
					{
						printf("ok cell_model (%02d-%02d-%d)\n", day+1, month+1, year+g_settings->year_start);
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
					logger(g_debug_log, "****************END OF DAY (%d)*******************\n\n\n", day + 1 );
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
				logger(g_debug_log, "****************END OF MONTH (%d)*******************\n\n", month + 1 );
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
			logger(g_debug_log, "****************END OF YEAR (%d)*******************\n", matrix->cells[cell].years[year].year );
		}
		
		if ( ! matrix->cells_count ) break;

		if ( g_year_start_index != -1 ) {
			free(matrix->cells[cell].years-g_year_start_index);
		} else {
			free(matrix->cells[cell].years);
		}
		matrix->cells[cell].years = NULL; /* required */
	}

	/* NETCDF output */
	if ( output_vars && output_vars->daily_vars_value ) {
		ret = output_write(output_vars, g_sz_output_path, g_settings->year_start, years_of_simulation, matrix->x_cells_count, matrix->y_cells_count, 0);
		//free(path);
		if ( ! ret ) {
			logger(g_debug_log, sz_err_out_of_memory);
			goto err;
		}
		free(output_vars->daily_vars_value);
		output_vars->daily_vars_value = NULL;
	}

	if ( output_vars && output_vars->monthly_vars_value ) {
		ret = output_write(output_vars, g_sz_output_path, g_settings->year_start, years_of_simulation, matrix->x_cells_count, matrix->y_cells_count, 1);
		//free(path);
		if ( ! ret ) {
			logger(g_debug_log, sz_err_out_of_memory);
			goto err;
		}
		free(output_vars->monthly_vars_value);
		output_vars->monthly_vars_value = NULL;
	}

	if ( output_vars && output_vars->yearly_vars_value ) {
		ret = output_write(output_vars, g_sz_output_path, g_settings->year_start, years_of_simulation, matrix->x_cells_count, matrix->y_cells_count, 2);
		//free(path);
		if ( ! ret ) {
			logger(g_debug_log, sz_err_out_of_memory);
			goto err;
		}
		free(output_vars->yearly_vars_value);
		output_vars->yearly_vars_value = NULL;
	}

	/* free memory */
	matrix_free(matrix); matrix = NULL;

	/* ok ! */
	prog_ret = 0;

#ifdef BENCHMARK_ONLY
benchmark:
#endif

	// TODO: FIX THIS
	/* close logger */
	logger_close(g_soil_log); g_soil_log = NULL;
	logger_close(g_annual_log); g_annual_log = NULL;
	logger_close(g_monthly_log); g_monthly_log = NULL;
	logger_close(g_daily_log); g_daily_log = NULL;

	/* benchmark ? */
	if ( g_sz_benchmark_path ) {
		int i;
		char temp[256];
		i = has_path_delimiter(g_sz_output_path);
		sprintf(temp, "%s%soutput_%s_%s"
						, g_sz_output_path
						, i ? "" : FOLDER_DELIMITER
						, PROGRAM_VERSION
						, sz_date
		);
		logger_error(g_debug_log, "benchmark with '%s'...", g_sz_benchmark_path);
		if ( compare(temp, g_sz_benchmark_path) ) {
			puts("ok!");
		}
	}

err:

	/* cleanup memory...
		do not remove null pointer to prevent
		double free with clean_up func
	*/

	/* close logger */
	logger_close(g_soil_log); g_soil_log = NULL;
	logger_close(g_annual_log); g_annual_log = NULL;
	logger_close(g_monthly_log); g_monthly_log = NULL;
	logger_close(g_daily_log); g_daily_log = NULL;

	/* free memory */
	if ( t ) free(t);
	if ( s ) free(s);
	if ( output_vars ) output_free(output_vars);
	if ( matrix ) matrix_free(matrix);

	free(g_sz_input_met_file); g_sz_input_met_file = NULL;
	free(g_sz_settings_file); g_sz_settings_file = NULL;

	/* this should be freed before */
	free(g_sz_topo_file); g_sz_topo_file = NULL;
	free(g_sz_soil_file); g_sz_soil_file = NULL;

	/* print elapsed time */
	end_timer = timer_get();
	timer = end_timer - start_timer;
	logger_error(g_debug_log, "%.2f secs / %.2f mins / %.2f hours elapsed\n", timer, timer / 60.0, timer / 3600.0);
	logger_close(g_debug_log); g_debug_log = NULL;

	return prog_ret;
}
