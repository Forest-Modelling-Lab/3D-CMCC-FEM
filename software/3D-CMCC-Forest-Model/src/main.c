/* main.c */



//TODO SIMULAZIONI DA bareground
//TODO RENDERE DINAMICO IL TREE_LAYER_LIMIT (all'aumentare del dbh aumenta il TREE_LAYER_LIMIT) see Montgomery & Chazdon, 2001)
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

/* constants */
#define PROGRAM_VERSION	"5.1.1"

//Last cumulative days in months
int MonthLength [] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 364};

//Days in Months
int DaysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const char *szMonth[MONTHS] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
		"AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };

/* global vars */
/* DO NOT REMOVE INITIALIZATION TO NULL, IT IS REQUIRED !! */
logger_t* g_log = NULL;
logger_t* g_daily_log = NULL;
logger_t* g_monthly_log = NULL;
logger_t* g_annual_log = NULL;
logger_t* g_soil_log = NULL;
soil_t* g_soil = NULL;
topo_t* g_topo = NULL;

/* DO NOT REMOVE INITIALIZATION TO NULL, IT IS REQUIRED !! */
char 	*g_sz_program_path = NULL

		, *input_path = NULL
		, *dataset_file = NULL
		, *input_met_path = NULL
		, *soil_path = NULL
		, *topo_path = NULL
		, *output_path = NULL

		, *g_sz_debug_output_filename = NULL
		, *g_sz_daily_output_filename =	NULL
		, *g_sz_monthly_output_filename = NULL
		, *g_sz_yearly_output_filename = NULL
		, *g_sz_soil_output_filename = NULL

		, *g_sz_settings_path =	NULL
		, *output_vars_path = NULL
;

/* DO NOT REMOVE INITIALIZATION TO NULL, IT IS REQUIRED !! */
OUTPUT_VARS *output_vars = NULL;	/* required */

static int years_of_simulation;	// default is none

// ALESSIOR: PORCATA fixme
int x_cells_count;
int y_cells_count;

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
static const char msg_debug_output_file[]		=	"debug output file path = %s\n";
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

static void clean_up(void) {
	if ( g_sz_settings_path ) free(g_sz_settings_path);
	if ( g_sz_debug_output_filename ) free(g_sz_debug_output_filename);
	if ( g_sz_daily_output_filename ) free(g_sz_daily_output_filename);
	if ( g_sz_monthly_output_filename ) free(g_sz_monthly_output_filename);
	if ( g_sz_yearly_output_filename ) free(g_sz_yearly_output_filename);
	if ( g_sz_soil_output_filename ) free(g_sz_soil_output_filename);
	if ( input_path ) free(input_path);
	if ( g_topo ) free(g_topo);
	if ( g_soil ) free(g_soil);
	if( settings ) free(settings);
	if ( g_sz_program_path ) free(g_sz_program_path);

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

static void show_usage(void) {
	fprintf(stdout, "\nUsage:\n");
	fprintf(stdout, "\t3D-CMCC Model -i INPUT_DIR -o OUTPUT_FILENAME -d DATASET_FILENAME -m MET_FILE_LIST -s SITE_FILENAME -c SETTINGS_FILENAME [-h]\n");
	fprintf(stdout, "\nMandatory options:\n");
	fprintf(stdout, "\t-i\tinput directory\t\t\t\t\t(i.e.: -i /path/to/input/directory/)\n");
	fprintf(stdout, "\t-o\toutput filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stdout, "\t-b\tdaily output filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stdout, "\t-e\tmonthly output filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stdout, "\t-f\tannual output filename\t\t\t\t\t(i.e.: -o /path/to/CMCC.log)\n");
	fprintf(stdout, "\t-d\tdataset filename stored into input directory\t(i.e.: -d input.txt)\n");
	fprintf(stdout, "\t-m\tmet filename list stored into input directory\t(i.e.: -m 1999.txt,2003.txt,2009.txt)\n");
	fprintf(stdout, "\t-s\tsoil filename stored into input directory\t(i.e.: -s soil.txt or soil.nc)\n");
	fprintf(stdout, "\t-t\ttopo filename stored into input directory\t(i.e.: -t topo.txt or topo.nc)\n");
	fprintf(stdout, "\t-c\tsettings filename stored into input directory\t(i.e.: -c settings.txt)\n");
	fprintf(stdout, "\t-r\toutput vars list\t(i.e.: -r output_vars.lst)\n");
	fprintf(stdout, "\nOptional options:\n");
	fprintf(stdout, "\t-h\tprint this help\n");
	fprintf(stdout, "\nLaunch example:\n");
	fprintf(stdout, "\t3D-CMCC Model -i /path/to/input/directory/ -o /path/to/CMCC.log -f /path/to/CMCC.log -e /path/to/CMCC.log -d /path/to/input.txt -m /path/to/1999.txt,/path/to/2003.txt,/path/to/2009.txt -s /path/to/site.txt -c /path/to/settings.txt\n");
}

void rows_free(ROW *rows, int rows_count ) {
	if ( rows_count  ) {
		int i;
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


static int log_start(const char* const sitename) {
	struct tm* data;
	time_t rawtime;
	char buffer[64]; /* should be enough */
	char buffer_2[16];

	time(&rawtime);
	data = gmtime(&rawtime);

	buffer[0] = '\0';
	if ( sitename && sitename[0] ) {
		strcat(buffer, "_");
		strcat(buffer, sitename);
	}
	strcat(buffer, "_");
	sprintf(buffer_2, "%c", settings->version);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%c", settings->spatial);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%c", settings->time);
	strcat(buffer, buffer_2);
	strcat(buffer, "_");
	sprintf(buffer_2, "%d", (int)settings->sizeCell);
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

	if ( ! string_compare_i(settings->dndc, "on") ) {
		strcat(buffer, "_");
		strcat(buffer, "DNDC");
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
		strcat(buffer, ext);
	}

	strcat(buffer, ".txt");
	
	/* create output files */
	g_log = logger_new_ex(g_sz_debug_output_filename, buffer);
	if ( ! g_log ) puts("Unable to log to file: check logfile path!");
	
	g_daily_log = logger_new_ex(g_sz_daily_output_filename, buffer);
	if ( ! g_daily_log ) puts("Unable to create daily log!");
	
	g_monthly_log = logger_new_ex(g_sz_monthly_output_filename, buffer);
	if ( ! g_monthly_log ) puts("Unable to create monthly log!");

	g_annual_log = logger_new_ex(g_sz_yearly_output_filename, buffer);
	if ( ! g_annual_log ) puts("Unable to create annual log!");
	
	g_soil_log = logger_new_ex(g_sz_soil_output_filename, buffer);
	if ( ! g_soil_log ) puts("Unable to create soil log!");

	/* show paths */
	printf(msg_dataset_path, input_path);
	printf(msg_soil_path, soil_path);
	printf(msg_topo_path, topo_path);
	printf(msg_met_path, input_met_path);
	printf(msg_settings_path, g_sz_settings_path);
	printf(msg_debug_output_file, g_sz_debug_output_filename);
	printf(msg_daily_output_file, g_sz_daily_output_filename);
	printf(msg_monthly_output_file, g_sz_monthly_output_filename);
	printf(msg_annual_output_file, g_sz_yearly_output_filename);
	printf(msg_soil_output_file, g_sz_soil_output_filename);

	free(g_sz_settings_path); g_sz_settings_path = NULL;

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

	input_path = NULL;
	g_sz_debug_output_filename = NULL;
	g_sz_daily_output_filename = NULL;
	g_sz_monthly_output_filename = NULL;
	g_sz_yearly_output_filename = NULL;
	g_sz_soil_output_filename = NULL;
	dataset_file = NULL;
	input_met_path = NULL;
	soil_path = NULL;
	topo_path = NULL;
	g_sz_settings_path = NULL;
	output_vars_path = NULL;

	for ( i = 1; i < argc; ++i ) {
		if ( argv[i][0] != '-' ) {
			continue;
		}

		switch ( argv[i][1] ) {
			case 'i': /* folder where input files are stored */
				input_path = path_copy(argv[i+1]);
				if ( ! input_path ) {
					fprintf(stderr, "cannot allocate memory for input_path.\n");
					goto err;
				}
			break;

			case 'o': /* debug file path */
				g_sz_debug_output_filename = string_copy(argv[i+1]);
				if ( ! g_sz_debug_output_filename ) {
					fprintf(stderr, "cannot allocate memory for g_sz_debug_output_filename.\n");
					goto err;
				}
			break;

			case 'b': /* daily file path */
				g_sz_daily_output_filename = string_copy(argv[i+1]);
				if( ! g_sz_daily_output_filename ) {
					fprintf(stderr, "cannot allocate memory for g_sz_daily_output_filename.\n");
					goto err;
				}
			break;

			case 'f': /* monthly file path */
				g_sz_monthly_output_filename = string_copy(argv[i+1]);
				if( ! g_sz_monthly_output_filename ) {
					fprintf(stderr, "cannot allocate memory for g_sz_monthly_output_filename.\n");
					goto err;
				}
			break;

			case 'e': /* yearly file path */
				g_sz_yearly_output_filename = string_copy(argv[i+1]);
				if( ! g_sz_yearly_output_filename ) {
					fprintf(stderr, "cannot allocate memory for g_sz_yearly_output_filename.\n");
					goto err;
				}
			break;

			case 'n': /* soil file path */
				g_sz_soil_output_filename = string_copy(argv[i+1]);
				if( ! g_sz_soil_output_filename ) {
					fprintf(stderr, "cannot allocate memory for g_sz_soil_output_filename.\n");
					goto err;
				}
			break;

			case 'd': /* dataset file path */
				dataset_file = string_copy(argv[i+1]);
				if( ! dataset_file ) {
					fprintf(stderr, "cannot allocate memory for dataset_file.\n");
					goto err;
				}
			break;

			case 'm': /* met filename */
				input_met_path = string_copy(argv[i+1]);
				if( ! input_met_path ) {
					fprintf(stderr, "cannot allocate memory for input_met_path.\n");
					goto err;
				}
			break;

			case 's': /*soil file */
				soil_path = string_copy(argv[i+1]);
				if( ! soil_path ) {
					fprintf(stderr, "cannot allocate memory for soil_path.\n");
					goto err;
				}
			break;

			case 't': /* topo file */
				topo_path = string_copy(argv[i+1]);
				if( ! topo_path ) {
					fprintf(stderr, "cannot allocate memory for topo_path.\n");
					goto err;
				}
			break;

			case 'c': /* settings file */
				g_sz_settings_path = string_copy(argv[i+1]);
				if( ! g_sz_settings_path ) {
					fprintf(stderr, "cannot allocate memory for g_sz_settings_path.\n");
					goto err;
				}
			break;

			case 'r': /* outputfilename */
				output_vars_path = string_copy(argv[i+1]);
				if( ! output_vars_path ) {
					fprintf(stderr, "cannot allocate memory for output_vars_path.\n");
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
	if ( ! input_path ) {
		fprintf(stderr, "error: input dir option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! g_sz_debug_output_filename ) {
		fprintf(stderr, "error: output filename option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! g_sz_monthly_output_filename ) {
		fprintf(stderr, "error: monthly output filename option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! g_sz_yearly_output_filename ) {
		fprintf(stderr, "error: annual output filename option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! g_sz_soil_output_filename ) {
		fprintf(stderr, "error: soil output filename option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! dataset_file ) {
		fprintf(stderr, "error: dataset filename option is missing!\n");
		goto err_show_usage;
	}

	if ( ! input_met_path ) {
		fprintf(stderr, "error: met file list is missing!\n");
		goto err_show_usage;
	}

	if ( ! soil_path ) {
		fprintf(stderr, "error: soil filename option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! topo_path ) {
		fprintf(stderr, "error: topo filename option is missing!\n");
		goto err_show_usage;
	}
	
	if ( ! g_sz_settings_path ) {
		fprintf(stderr, "error: settings filename option is missing!\n");
		goto err_show_usage;
	}
	
	return 1;

err_show_usage:
	show_usage();

err:
	return 0;
}

int main(int argc, char *argv[]) {
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
	// this var is declared in types.h
	// so we need to initialize here
	settings = NULL;

	/* get program path */
	g_sz_program_path = get_current_directory();
	if ( ! g_sz_program_path ) {
		puts(err_unable_get_current_directory);
		return 1;
	}

	/* register atexit */
	if ( -1 == atexit(clean_up) ) {
		puts(err_unable_to_register_atexit);
		return 1;
	}

	/* parse args */
	if ( ! parse_args(argc, argv) ) {
		return 1;
	}

	/* show copyright */
	logger(g_log, copyright);

	/* show banner */
	logger(g_log, banner, get_netcdf_version());

	if ( output_vars_path ) {
		output_vars = ImportOutputVarsFile(output_vars_path);
		free(output_vars_path);
		output_vars_path = NULL;
		if ( ! output_vars ) {
			return 1;
		}
	}

	// Import settings.txt file (before logInit(), because I choose output filename from settings)
	error = importSettingsFile(g_sz_settings_path);
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
	printf(msg_processing, dataset_file);

	/* import dataset */
	rows = import_dataset(dataset_file, &rows_count);
	free(dataset_file); dataset_file = NULL;
	if ( !rows )
	{
		logger(g_log, "unable to get dataset files!");
		return 1;
	}
	puts(msg_ok);

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
					Psychrometric (&m->cells[cell], day, month, year, yos);
					Sat_vapour_pressure (&m->cells[cell], day, month, year, yos);
					Dew_temperature (&m->cells[cell], day, month, year, yos);
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
								/* run for SOIL */
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
		int ret;
		char *path = get_path(g_sz_daily_output_filename);
		if ( ! path && g_sz_daily_output_filename  ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		ret = WriteNetCDFOutput(path, output_vars, i, years_of_simulation, x_cells_count, y_cells_count, 0);
		free(path);
		if ( ! ret ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		free(output_vars->daily_vars_value);
		output_vars->daily_vars_value = NULL;
	}

	if ( output_vars && output_vars->monthly_vars_value ) {
		int ret;
		char *path = get_path(g_sz_monthly_output_filename);
		if ( ! path && g_sz_monthly_output_filename ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		ret = WriteNetCDFOutput(path, output_vars, i, years_of_simulation, x_cells_count, y_cells_count, 1);
		free(path);
		if ( ! ret ) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		free(output_vars->monthly_vars_value);
		output_vars->monthly_vars_value = NULL;
	}

	if ( output_vars && output_vars->yearly_vars_value ) {
		int ret;
		char *path = get_path(g_sz_yearly_output_filename);
		if ( ! path && g_sz_yearly_output_filename) {
			logger(g_log, err_out_of_memory);
			matrix_free(m);
			return 1;
		}
		ret = WriteNetCDFOutput(path, output_vars, i, years_of_simulation, x_cells_count, y_cells_count, 2);
		free(path);
		if ( ! ret ) {
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
	free(g_sz_debug_output_filename); g_sz_debug_output_filename = NULL;
	free(g_sz_daily_output_filename); g_sz_daily_output_filename = NULL;
	free(g_sz_monthly_output_filename); g_sz_monthly_output_filename = NULL;
	free(g_sz_yearly_output_filename); g_sz_yearly_output_filename = NULL;
	free(g_sz_soil_output_filename); g_sz_soil_output_filename = NULL;

	/* this should be freed before */
	free(topo_path); topo_path = NULL;
	free(soil_path); soil_path = NULL;

	/* free memory at exit */
	return 0;
}
