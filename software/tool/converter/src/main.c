// main.c
//
// written by A. Ribeca
//

// os dependant stuff
#ifdef _WIN32
#ifndef STRICT
#define STRICT
#endif
#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#include <direct.h>
// for memory leak
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif /* _DEBUG */
#pragma comment(lib, "kernel32.lib")
#else
#include <dirent.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netcdf.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma comment(lib, "netcdf")
#endif

#define PROG_VERSION "v1.0a"

const char banner[] =	"3D-CMCC-Forest-Model to NetCDF Converter " PROG_VERSION "\n"
						"by A. Ribeca <a.ribeca@unitus.it>\n"
						"compiled on "__DATE__" at "__TIME__"\n"
						"using netcdf v%s\n\n"
;

const char err_out_of_memory[] = "out of memory";
const char err_unable_to_register_cleanup_func[] = "warning - unable to register cleanup func";
const char err_no_input_folder[] = "error - input folder not specified. usage: conv [FOLDER]";
const char err_invalid_input_folder[] = "error - invalid input folder specified: %s";

#include "util.h"
#include "files.h"
#include "dataset.h"
#include "nc.h"

void clean_up(void)
{
#ifdef _WIN32
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
#endif
}
			
int main(int argc, char* argv[])
{
	const char* input_folder;
	const char* output_folder;
	int ret;
	int is_file;
	files_t* files;

	input_folder = NULL;
	output_folder = NULL;
	files = NULL;
	is_file = 0;
	ret = 1; // defaults to err

	//_CrtSetBreakAlloc();

	// show banner
	printf(banner, nc_inq_libvers());

#if 1
	// parse args
	input_folder = "..";
	if  ( argc >= 2 )
	{
		// get folder name
		input_folder = argv[1];
		if ( argc > 2 )
		{
			output_folder = argv[2];
		}
	}

	// register clean_up func
	if ( -1 == atexit(clean_up) )
	{
		puts(err_unable_to_register_cleanup_func);
	}

	// check if folder exists
	if ( input_folder && ! path_exists(input_folder) )
	{
		// check if is a file
		if ( file_exists(input_folder) )
		{
			is_file = 1;
		}
		else
		{
			printf(err_invalid_input_folder, input_folder);
			goto quit;
		}
	}

	if ( output_folder )
	{
		if ( ! path_exists(output_folder) )
		{
		#if defined(_WIN32)
			_mkdir(output_folder);
		#else 
			mkdir(output_folder, 0777);
		#endif
		}
	}

	// scan folder
	if ( is_file )
	{
		files = files_new_ex(NULL, input_folder);
		if ( ! files )
		{
			puts(err_out_of_memory);
			goto quit;
		}
	}
	else
	{
		files = files_get(input_folder, "txt", NULL);
		if ( ! files )
		{
			printf("nothing found on %s\n", input_folder);
			goto quit;
		}
	}

	// process
	{
		int i;
		int parsed;
		int processed;

		parsed = 0;
		processed = 0;
		for ( i = 0; i < files->count; ++i )
		{
			int r;
			dataset_t* d;

			++parsed;

			printf("processing %s...", files->filename[i]);
			d = dataset_import(files->filename[i]);
			if ( ! d ) continue;
			r = nc_conv(d, output_folder);
			dataset_free(d);
			if ( ! r ) continue;
			puts("ok");
			++processed;
		}

		printf("\nparsed %d files, processed %d files\n\n", parsed, processed);
	}
#else
	{
		dataset_t* d;

		const char filename[] = "annual_5.4_Solling_beech_2A_ESM10_hist.txt_(1967-2010)_CO2_ON_CO2_hist.txt_Man_OFF_d_10000_txt.txt";

		printf("importing %s...", filename);
		d = dataset_import(filename);
		if ( ! d ) goto quit;
		puts("ok");

		// convert to nc4
		if ( ! nc_conv(d) ) goto quit;	
	}
#endif
	ret = 0; // ok

quit:
	if ( files ) files_free(files);
	return ret;
}
