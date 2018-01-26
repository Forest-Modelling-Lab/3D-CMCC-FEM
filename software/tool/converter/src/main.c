// main.c
//
// written by A. Ribeca
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include "netcdf.h"
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
			
int main(int argc, char* argv[])
{
	const char* folder;
	int ret;
	files_t* files;

	folder = NULL;
	files = NULL;
	ret = 1; // defaults to err

	// show banner
	printf(banner, nc_inq_libvers());

#if 1
	// parse args
	folder = "..";
	if  ( argc >= 2 )
	{
		// get folder name
		folder = argv[1];
	}

	// check if folder exists
	if ( folder && ! path_exists(folder) )
	{
		printf(err_invalid_input_folder, folder);
		goto quit;
	}

	// scan folder
	files = files_get(folder, "txt", NULL);
	if ( ! files )
	{
		printf("nothing found on %s\n", folder);
		goto quit;
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
			dataset_t* d;

			++parsed;

			printf("processing %s...", files->filename[i]);
			d = dataset_import(files->filename[i]);
			if ( ! d ) continue;
			if ( ! nc_conv(d) ) continue;
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

//ISIMIP_OUTPUT_v.5.4-FvCB-22-Dec-2017/FT/Soroe/output_5.4_2017_DECEMBER_22/annual/
