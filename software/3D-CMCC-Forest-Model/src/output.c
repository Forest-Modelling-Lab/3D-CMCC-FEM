/* output.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for tolower
#include <assert.h>
#include "common.h"
#include "logger.h"
#include "output.h"
#include "netcdf.h"


#ifdef _WIN32
#define string_compare_n _strnicmp
#else
#define string_compare_n strncasecmp
#endif

extern logger_t* g_debug_log;
extern const char sz_err_out_of_memory[];
extern const char* log_types[];

typedef enum
{
	ANNUAL_DATASET_TYPE = 0
	, DAILY_DATASET_TYPE

} e_reimported_dataset_type;

typedef enum
{
	ANNUAL_HEIGHT = 0
	, ANNUAL_DBH
	, ANNUAL_AGE
	, ANNUAL_LIVETREE
	, ANNUAL_DEADTREE
	, ANNUAL_THINNEDTREE
	, ANNUAL_STEM_C
	, ANNUAL_MAX_LEAF_C
	, ANNUAL_MAX_FROOT_C
	, ANNUAL_CROOT_C
	, ANNUAL_BRANCH_C
	, ANNUAL_FRUIT_C
	, ANNUAL_BASAL_AREA
	, ANNUAL_MAI
	, ANNUAL_VOLUME
		
	, ANNUAL_IMPORT_VARS_COUNT

	, ANNUAL_CMVB = ANNUAL_IMPORT_VARS_COUNT
	, ANNUAL_CMW
	, ANNUAL_CMR

	, ANNUAL_VARS_COUNT

} e_annual_vars;

typedef enum
{
	DAILY_LAI = 0
	, DAILY_GPP
	, DAILY_NPP
	, DAILY_RA
	, DAILY_FAPAR
	, DAILY_ET
	, DAILY_INT
	, DAILY_SOIL_EVAPO
	, DAILY_TRA
	, DAILY_ASW
	, DAILY_DELTA_LEAF_C
	, DAILY_DELTA_FROOT_C

	, DAILY_DELTA_STEM_C
	, DAILY_DELTA_BRANCH_C
	, DAILY_DELTA_FRUIT_C
	, DAILY_DELTA_CROOT_C

	, DAILY_FROOT_AR
	, DAILY_CROOT_AR
	, DAILY_TSOIL

	, DAILY_IMPORT_VARS_COUNT

	, DAILY_NPPAB = DAILY_IMPORT_VARS_COUNT
	, DAILY_NPPBB
	, DAILY_RAR

	, DAILY_VARS_COUNT

} e_daily_vars;

typedef struct
{
	char* filename;
	char* sitename;
	char* clim_scenario;
	char* co2_type;
	char* co2_file;
	char* exp;
	e_reimported_dataset_type type;
	int esm;
	int start_year;
	int end_year;
	int rows_count;
	int columns_count;
	float** vars;

	char* path;

} reimported_dataset_t;

struct
{
	char* name;
	char* standard_name;
	char* long_name;
	char* unit;
}
annual_vars[ANNUAL_VARS_COUNT] =
{
	{ "HEIGHT", "height", "Stand Height", "m" }
	, { "DBH", "dbh", "Mean DBH", "cm" }
	, { "AGE", "age", "Tree Age By Dbh Class", "years" }
	, { "LIVETREE", "density", "Stand Density", "trees ha-1" }
	, { "DEADTREE", "mortstemno", "Removed Stem Numbers By Size Class By Natural Mortality", "trees ha-1" }
	, { "THINNEDTREE", "harvstemno", "Removed Stem Numbers BY Size Class By Management", "trees ha-1" }
	, { "STEM_C" }			// NOT EXPORTED
	, { "MAX_LEAF_C", "cleaf", "Carbon Mass In Leaves", "kg m-2" }
	, { "MAX_FROOT_C" }		// NOT EXPORTED
	, { "CROOT_C" }			// NOT EXPORTED
	, { "BRANCH_C" }		// NOT EXPORTED
	, { "FRUIT_C" }			// NOT EXPORTED
	, { "BASAL_AREA", "ba", "Basal Area", "m2 ha-1" }
	, { "MAI", "mai", "Mean Annual Increment", "m3 ha-1" }
	, { "VOLUME", "vol", "Stand Volume", "m3 ha-1" }

	, { "CMVB", "cveg", "Carbon Mass In Vegetation Biomass", "kg m-2" }
	, { "CMW", "cwood", "Carbon Mass In Wood", "kg m-2" }
	, { "CMR", "croot", "Carbon Mass in Roots", "kg m-2" }
};

struct
{
	char* name;
	char* standard_name;
	char* long_name;
	char* unit;
	int add_species;
}
daily_vars[DAILY_VARS_COUNT] =
{
	{ "LAI_PROJ", "lai", "Leaf Area Index", "m2 m-2", 1 } // monthly
	, { "GPP", "gpp", "Gross Primary Production", "kg m-2 s-1", 1 }
	, { "NPP", "npp", "Net Primary Production", "kg m-2 s-1", 1 }
	, { "RA", "ra", "Autotrophic (Plant) Respiration", "kg m-2 s-1", 1 }
	, { "FAPAR", "fapar", "Fraction Of Absorbed Photosynthetically Active Radiation", "%", 1 }
	, { "ET", "evap", "Total Evapotranspiration", "kg m-2 s-1", 0 }
	, { "INT", "intercept", "Evaporation From Canopy (Interception)", "kg m-2 s-1", 1 }
	, { "SOIL_EVAPO", "esoil", "Water Evaporation From Soil", "kg m-2 s-1", 0 }
	, { "TRA", "trans", "Transpiration", "kg m-2 s-1", 1 }
	, { "ASW", "soilmost", "Soil Mosture", "kg m-2", 0 }
	, { "DELTA_LEAF_C", "npp-landleaf", "Net Primary Production Allocated To Leaf Biomass", "kg m-2", 1 }
	, { "DELTA_FROOT_C", "npp-landroot", "Net Primary Production Allocated To Fine Root Biomass", "kg m-2", 1 }
	, { "DELTA_STEM_C" }	// NOT EXPORTED
	, { "DELTA_BRANCH_C" }	// NOT EXPORTED
	, { "DELTA_FRUIT_C" }	// NOT EXPORTED
	, { "DELTA_CROOT_C" }	// NOT EXPORTED
	, { "FROOT_AR" }		// NOT EXPORTED
	, { "CROOT_AR" }		// NOT EXPORTED
	, { "TSOIL", "tsl", "Temperature Of Soil", "k", 0 }

	, { "NPPAB", "npp_abovegroundwood", "Net Primary Production Allocated To Above Ground Wood Biomass", "kg m-2", 1 }
	, { "NPPBB", "npp_belowgroundwood", "Net Primary Production Allocated To Below Ground Wood Biomass", "kg m-2", 1 }
	, { "RAR", "rr", "Root Autotrophic Respiration", "kg m-2", 1 }
};

void reimported_dataset_free(reimported_dataset_t* dataset)
{
	assert(dataset);

	if ( dataset->exp )
	{
		free(dataset->exp);
	}

	if ( dataset->co2_type )
	{
		free(dataset->co2_type);
	}

	if ( dataset->co2_file )
	{
		free(dataset->co2_file);
	}

	if ( dataset->vars )
	{
		int i;

		for ( i = 0; i < dataset->columns_count; ++i )
		{
			free(dataset->vars[i]);
		}
		free(dataset->vars);
	}
	if ( dataset->clim_scenario )
	{
		free(dataset->clim_scenario);
	}
	if ( dataset->sitename )
	{
		free(dataset->sitename);
	}

	if ( dataset->filename )
	{
		free(dataset->filename);
	}

	if ( dataset->path )
	{
		free(dataset->path);
	}

	free(dataset);
}

static reimported_dataset_t* reimport_dataset(const char* const filename)
{
#define BUF_SIZE (2048)		// should be enough
	char buf[BUF_SIZE];		

	e_reimported_dataset_type type;
	char site[32];			// should be enough
	char exp[16];			// should be enough
	int esm;
	char clim_scenario[16];	// should be enough
	int start_year;
	int end_year;
	char co2_type[8];		// should be enough
	char co2_file[16];		// should be enough
	char man[8];			// should be enough
	int* columns;
	int columns_count;

	char* p;
	char* token;
	
	int i;
	int has_path;
	FILE* f;
	reimported_dataset_t* dataset;

	const char delimiters[] = ",\r\n";

	assert(filename);

	dataset = NULL;
	columns = NULL;
	f = NULL;

	// check for path
	{
		char* p;
		char* p2;

		has_path = 0;

		p = strrchr(filename, '\\'); 
		p2 = strrchr(filename, '/');
		if ( p2 > p ) p = p2;
		if (  p )
			has_path = p - filename + 1;
	}

	// check type
	if ( ! string_compare_n (filename + has_path, "annual", strlen("annual")) )
	{
		type = ANNUAL_DATASET_TYPE;
	}
	else if ( ! string_compare_n(filename + has_path, "daily", strlen("daily")) )
	{
		type = DAILY_DATASET_TYPE;
	}
	else
	{
		printf("%s is an invalid filename\n", filename+has_path);
		goto quit;
	}

	// get site
	if ( 1 != sscanf(filename + has_path, "%*[^_]_%*[^_]_%[^_]_%n", site, &i) )
	{
		printf("unable to get site from %s\n", filename+has_path);
		goto quit;
	}

	// we can have site with underscore in name
	// e.g: Solling_spruce, Solling_beech and Bily_Kriz
	// so we must fix it here
	if ( ! string_compare_i(site, "Solling") || ! string_compare_i(site, "Bily") )
	{
		int n;

		strcat(site, "_");
		n = strlen(site);
		if ( 1 != sscanf(filename + has_path, "%*[^_]_%*[^_]_%*[^_]_%[^_]_%n", site+n, &i) )
		{
			printf("unable to get site from %s\n", filename+has_path);
			goto quit;
		}
	}

	// check for local
	if ( ! string_compare_n(filename + has_path + i, "local", 5) )
	{
		strcpy(exp, "local");
		esm = 11;

		// get stuff
		if ( 6 != sscanf(filename + has_path + i + 5 + 1, "%[^.].txt_(%d-%d)_CO2_%[^_]_%[^.].txt_Man_%[^_]_"
															, clim_scenario
															, &start_year
															, &end_year
															, co2_type
															, co2_file
															, man
															) )
		{
			printf("unable to parse %s\n", filename+has_path);
			goto quit;
		}
	}
	else
	{
		// get stuff
		if ( 8 != sscanf(filename + has_path + i, "%[^_]_ESM%d_%[^.].txt_(%d-%d)_CO2_%[^_]_%[^.].txt_Man_%[^_]_"
															, exp
															, &esm
															, clim_scenario
															, &start_year
															, &end_year
															, co2_type
															, co2_file
															, man
															) )
		{
			printf("unable to parse %s\n", filename+has_path);
			goto quit;
		}

		if ( (esm < 1) || (esm > 10) )
		{
			printf("%s has invalid esm. range is 1-10", filename+has_path);
			goto quit;
		}
	}

	--esm; // zero based index

	f = fopen(filename, "r");
	if ( ! f )
	{
		printf("unable to open %s\n", filename+has_path);
		goto quit;
	}

	// parse header
	if ( ! fgets(buf, BUF_SIZE, f) )
	{
		printf("unable to get header from %s\n", filename+has_path);
		goto quit;
	}

	// get column
	columns_count = (ANNUAL_DATASET_TYPE == type) ?
						ANNUAL_IMPORT_VARS_COUNT : DAILY_IMPORT_VARS_COUNT;
	columns = malloc(columns_count*sizeof*columns);
	if ( ! columns )
	{
		puts(sz_err_out_of_memory);
		goto quit;
	}

	for ( i = 0; i < columns_count; ++i )
	{
		columns[i] = -1;
	}

	for ( i = 0, token = string_tokenizer(buf, delimiters, &p); token; token = string_tokenizer(NULL, delimiters, &p), ++i )
	{
		int j;

		if ( ! token || ! token[0] ) continue;

		for ( j = 0; j < columns_count; ++j )
		{
			if ( ! string_compare_i(token, (ANNUAL_DATASET_TYPE == type) ? annual_vars[j].name : daily_vars[j].name) )
			{
				if ( columns[j] != -1 ) 
				{
					// fix for gpp, npp
					if ( DAILY_DATASET_TYPE == type )
					{
						if ( ! strcmp(token, "gpp")
							|| ! strcmp(token, "npp")
							|| ! strcmp(token, "et")
							)
						{
							continue;
						}
					}

					printf("duplicated var in %s: %s already found on column %d\n", filename+has_path, token, columns[j]); 
					goto quit;
				}

				columns[j] = i;
				// do not break so we can check for duplicated names
			}
		}
	}

	for ( i = 0; i < columns_count; ++i )
	{
		if ( -1 == columns[i] )
		{
			printf("var %s not found in %s\n"
						, (ANNUAL_DATASET_TYPE == type) ? annual_vars[i].name : daily_vars[i].name
						, filename+has_path
			);
			goto quit;
		}
	}
	
	// alloc memory
	dataset = malloc(sizeof*dataset);
	if ( ! dataset )
	{
		puts(sz_err_out_of_memory);
		goto quit;
	}
	memset(dataset, 0, sizeof*dataset);
	dataset->filename = string_copy(filename + has_path);
	dataset->sitename = string_copy(site);
	if ( ! dataset->sitename )
	{
		puts(sz_err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}
	dataset->clim_scenario = string_copy(clim_scenario);
	if ( ! dataset->clim_scenario )
	{
		puts(sz_err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}
	dataset->exp = string_copy(exp);
	if ( ! dataset->exp )
	{
		puts(sz_err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}
	dataset->co2_type = string_copy(co2_type);
	if ( ! dataset->co2_type )
	{
		puts(sz_err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}
	dataset->co2_file = string_copy(co2_file);
	if ( ! dataset->co2_file )
	{
		puts(sz_err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}

	dataset->type = type;
	dataset->esm = esm;
	dataset->start_year = start_year;
	dataset->end_year = end_year;

	if ( has_path )
	{
		dataset->path = malloc((has_path+1)*sizeof*dataset->path);
		if ( ! dataset->path )
		{
			puts(sz_err_out_of_memory);
			goto quit;
		}
		strncpy(dataset->path, filename, has_path);
		dataset->path[has_path] = '\0';
	}
	
	// compute rows count
	if ( ANNUAL_DATASET_TYPE == type )
	{
		dataset->rows_count = end_year - start_year + 1;
		dataset->columns_count = ANNUAL_VARS_COUNT;
	}
	else
	{
		int year;

		dataset->rows_count = 0;
		for ( year = start_year; year <= end_year; ++year )
		{
			dataset->rows_count += IS_LEAP_YEAR(year) ? 366 : 365;
		}
		dataset->columns_count = DAILY_VARS_COUNT;
	}

	// alloc memory for vars
	dataset->vars = malloc(dataset->columns_count*sizeof*dataset->vars);
	if ( ! dataset->vars )
	{
		puts(sz_err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}
	memset(dataset->vars, 0, sizeof*dataset->vars);

	// alloc memory for values on each var
	for ( i = 0; i < dataset->columns_count; ++i )
	{
		dataset->vars[i] = malloc(dataset->rows_count*sizeof*dataset->vars[i]);
		if ( ! dataset->vars[i] )
		{
			puts(sz_err_out_of_memory);
			reimported_dataset_free(dataset);
			dataset = NULL;
			goto quit;
		}
	}

	// import values
{
	int row; // keep track of current row

	row = 0;
	while ( fgets(buf, BUF_SIZE, f) )
	{
		char* token;
		char* p;
		int column;

		// break on empty row
		// it means we have reach end of values to import
		if ( ('\r' == buf[0]) || ('\n' == buf[0]) )
		{
			break;
		}

		column = 0;
		for ( i = 0, token = string_tokenizer(buf, delimiters, &p); token; token = string_tokenizer(NULL, delimiters, &p), ++i )
		{
			int j;

			for ( j = 0; j < columns_count; ++j )
			{
				if ( i == columns[j] )
				{
					int err;
					float value;

					value =(float) convert_string_to_float(token, &err);
					if ( err )
					{
						printf("unable to convert %s at row %d in %s\n", token, row+1, filename+has_path);
						reimported_dataset_free(dataset);
						dataset = NULL;
						goto quit;
					}
					dataset->vars[j][row] = value;
					++column;
					break;
				}
			}
		}

		if ( column != columns_count )
		{
			printf("imported %d columns instead of %d at row %d in %s\n", column, dataset->columns_count, row+1, filename+has_path);
			reimported_dataset_free(dataset);
			dataset = NULL;
			goto quit;
		}

		++row;
	}

	// count imported rows
	if ( row != dataset->rows_count )
	{
		printf("imported %d rows instead of %d in %s\n", row, dataset->rows_count, filename+has_path);
		reimported_dataset_free(dataset);
		dataset = NULL;
		goto quit;
	}

	// compute vars
	for ( i = 0; i < dataset->rows_count; ++i )
	{
		if ( ANNUAL_DATASET_TYPE == type )
		{
			float cmvb;
			float cmw;
			float cmr;

			float stem_c;
			float max_leaf_c;
			float max_froot_c;
			float croot_c;
			float branch_c;
			float fruit_c;

			cmvb = INVALID_VALUE;
			cmw = INVALID_VALUE;
			cmr = INVALID_VALUE;

			stem_c = dataset->vars[ANNUAL_STEM_C][i];
			max_leaf_c = dataset->vars[ANNUAL_MAX_LEAF_C][i];
			max_froot_c = dataset->vars[ANNUAL_MAX_FROOT_C][i];
			croot_c = dataset->vars[ANNUAL_CROOT_C][i];
			branch_c = dataset->vars[ANNUAL_BRANCH_C][i];
			fruit_c = dataset->vars[ANNUAL_FRUIT_C][i];

			if ( ! IS_INVALID_VALUE(croot_c) )
			{
				if ( ! IS_INVALID_VALUE(max_froot_c) )
				{
					cmr = max_froot_c + croot_c;
				}

				if ( ! IS_INVALID_VALUE(stem_c) && ! IS_INVALID_VALUE(branch_c) )
				{
					cmw = stem_c + branch_c + croot_c;

					if ( ! IS_INVALID_VALUE(max_leaf_c) && ! IS_INVALID_VALUE(fruit_c) && ! IS_INVALID_VALUE(max_froot_c) )
					{
						cmvb = stem_c + max_leaf_c + max_froot_c + croot_c + branch_c + fruit_c;
					}
				}
			}

			dataset->vars[ANNUAL_CMR][i] = cmr;
			dataset->vars[ANNUAL_CMW][i] = cmw;
			dataset->vars[ANNUAL_CMVB][i] = cmvb;

			/* apply conversion */
			if ( ! IS_INVALID_VALUE(dataset->vars[ANNUAL_CMVB][i]) )
				dataset->vars[ANNUAL_CMVB][i] /= 10.;
			if ( ! IS_INVALID_VALUE(dataset->vars[ANNUAL_MAX_LEAF_C][i]) )
				dataset->vars[ANNUAL_MAX_LEAF_C][i] /= 10.;
			if ( ! IS_INVALID_VALUE(dataset->vars[ANNUAL_CMW][i]) )
				dataset->vars[ANNUAL_CMW][i] /= 10.;
			if ( ! IS_INVALID_VALUE(dataset->vars[ANNUAL_CMR][i]) )
				dataset->vars[ANNUAL_CMR][i] /= 10.;
		}
		else // daily
		{
			float dws;
			float dwl;
			float dwbb;
			float dfruit;
			float dwfr;
			float dwcr;
			float frar;
			float crar;

			float nppab;
			float nppbb;
			float rar;

			nppab = INVALID_VALUE;
			nppbb = INVALID_VALUE;
			rar = INVALID_VALUE;

			dws = dataset->vars[DAILY_DELTA_STEM_C][i];
			dwl = dataset->vars[DAILY_DELTA_LEAF_C][i];
			dwbb = dataset->vars[DAILY_DELTA_BRANCH_C][i];
			dfruit = dataset->vars[DAILY_DELTA_FRUIT_C][i];
			dwfr = dataset->vars[DAILY_DELTA_FROOT_C][i];
			dwcr = dataset->vars[DAILY_DELTA_CROOT_C][i];
			frar = dataset->vars[DAILY_FROOT_AR][i];
			crar = dataset->vars[DAILY_CROOT_AR][i];

			if ( ! IS_INVALID_VALUE(frar) && ! IS_INVALID_VALUE(crar) )
			{
				rar = frar + crar;
			}

			if ( ! IS_INVALID_VALUE(dwfr) && ! IS_INVALID_VALUE(dwcr) )
			{
				nppbb = dwfr + dwcr;
			}

			if ( ! IS_INVALID_VALUE(dws)
					&& ! IS_INVALID_VALUE(dwl)
					&& ! IS_INVALID_VALUE(dwbb)
					&& ! IS_INVALID_VALUE(dfruit) )
			{
				nppab = dws + dwl + dwbb + dfruit;
			}

			dataset->vars[DAILY_RAR][i] = rar;
			dataset->vars[DAILY_NPPBB][i] = nppbb;
			dataset->vars[DAILY_NPPAB][i] = nppab;

			/* apply conversion */
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_GPP][i]) )
				dataset->vars[DAILY_GPP][i] /= (1000*86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_NPP][i]) )
				dataset->vars[DAILY_NPP][i] /= (1000*86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_RA][i]) )
				dataset->vars[DAILY_RA][i] /= (1000*86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_ET][i]) )
				dataset->vars[DAILY_ET][i] /= 86400;
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_INT][i]) )
				dataset->vars[DAILY_INT][i] /= 86400;
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_SOIL_EVAPO][i]) )
				dataset->vars[DAILY_SOIL_EVAPO][i] /= 86400;	
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_TRA][i]) )
				dataset->vars[DAILY_TRA][i] /= 86400;
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_DELTA_LEAF_C][i]) )
				dataset->vars[DAILY_DELTA_LEAF_C][i] /= (10 * 86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_DELTA_FROOT_C][i]) )
				dataset->vars[DAILY_DELTA_FROOT_C][i] /= (10 * 86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_NPPAB][i]) )
				dataset->vars[DAILY_NPPAB][i] /= (10 * 86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_NPPBB][i]) )
				dataset->vars[DAILY_NPPBB][i] /= (10 * 86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_RAR][i]) )
				dataset->vars[DAILY_RAR][i] /= (1000 * 86400);
			if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_TSOIL][i]) )
				dataset->vars[DAILY_TSOIL][i] += 273.13f;
		}
	}
#if 0
	// dump dataset
	{
		FILE *s;

		s = fopen("debug_import.csv", "w");
		if ( s )
		{
			// write header
			if ( ANNUAL_DATASET_TYPE == dataset->type )
			{
				fputs("YEAR,", s);
				for ( i = 0; i < SIZEOF_ARRAY(annual_vars); ++i )
				{
					fputs(annual_vars[i], s);
					if ( i < SIZEOF_ARRAY(annual_vars)-1 )
					{
						fputs(",", s);
					}
				}
			}
			else // daily
			{
				for ( i = 0; i < SIZEOF_ARRAY(daily_vars); ++i )
				{
					fputs(daily_vars[i], s);
					if ( i < SIZEOF_ARRAY(daily_vars)-1 )
					{
						fputs(",", s);
					}
				}
			}
			fputs("\n", s);

			if ( ANNUAL_DATASET_TYPE == dataset->type )
			{
				for ( i = 0; i < dataset->rows_count; ++i )
				{
					int j;

					fprintf(s, "%d,", start_year+i);
					for ( j = 0; j < dataset->columns_count; ++j )
					{
						fprintf(s, "%f", dataset->vars[j][i]);

						if ( j < dataset->columns_count - 1 )
						{
							fputs(",", s);
						}
					}
					fputs("\n", s);
				}
			}
			else
			{
				int day;
				int year;

				day = 0;
				year = start_year;
				for ( i = 0; i < dataset->rows_count; ++i )
				{
					int j;

					if ( ++day > (IS_LEAP_YEAR(year) ? 366 : 365) )
					{
						day = 1;
						++year;
					}

					fprintf(s, "%d,%d,", year, day);
					for ( j = 0; j < dataset->columns_count; ++j )
					{
						fprintf(s, "%f", dataset->vars[j][i]);

						if ( j < dataset->columns_count - 1 )
						{
							fputs(",", s);
						}
					}
					fputs("\n", s);
				}
			}
			fclose(s);
		}
		else
		{
			puts("unable to create debug file!");
		}
	}
#endif

}
quit:
	if ( columns ) free(columns);
	if ( f ) fclose(f);
	return dataset;

#undef BUF_SIZE
}

static void lowercase(char *s)
{
	int i;

	assert(s);

	for ( i = 0; s[i]; ++i )
	{
		s[i] = tolower(s[i]);
	}
}

static int nc_conv(reimported_dataset_t* dataset, char* folder)
{
#define BUF_SIZE			(1024)	// should be enough
#define NC_MISSING_VALUE	(1e20)
	char* sitename;
	char buf[BUF_SIZE];
	int i;
	int ret;
	int site_index;
	double* index;

	// for compression
	const int shuffle = 1;
    const int deflate = 1;
    const int deflate_level = 9;

	const size_t latlon_index[] = { 0 };

	const char modelname[] = "3D-CMCC-CNR-BGC";

	enum
	{
		PIAB = 0
		, FASY
		, PISY
		, PIPI

		, SPECIES_COUNT
	};

	struct
	{
		char* standard_name;
		char* long_name;
	}
	species [SPECIES_COUNT] =
	{
		{ "piab", "Picea Abies" }
		, { "fasy", "Fagus Sylvatica" }
		, { "pisy", "Pinus Sylvestris" }
		, { "pipi", "Pinus Pinaster" }
	};

	struct
	{
		char* model_name;
		char* isimip_name;
		double lat;
		double lon;
		int species;
	}
	sites [] =
	{
		{ "Bily_Kriz", "bily_kriz", 49.3f, 18.32f, PIAB  }
		, { "Collelongo", "collelongo", 41.85f, 13.59f, FASY }
		, { "Hyytiala", "hyytiala", 61.84f, 24.29f, PISY }
		, { "Kroof", "kroof", 48.25f, 11.10f, FASY }
		, { "LeBray", "le_bray", 44.72f, -0.77f, PIPI  }
		, { "Peitz", "peitz", 51.92f, 14.35f, PISY  }
		, { "Solling_beech", "solling_beech", 51.77f, 9.57f, FASY  }
		, { "Solling_spruce", "solling_spruce", 51.76f, 9.58f, PIAB  }
		, { "Soroe", "soro", 55.29f, 11.38f, FASY  }
	};

	const char *gcms[] = // is ecm for model
	{
		"hadgem2_es"
		, "ipsl_cm5a_lr"
		, "miroc_esm_chem"
		, "gfdl_esm2m"
		, "noresm1_m"
		, "gswp3"
		, "princeton"
		, "watch"
		, "wfdei"
		, "localclim"
	};

	const char socio_econ_scenario[] = "man";

	const char sz_standard_name[] = "standard_name";
	const char sz_long_name[] = "long_name";
	const char sz_units[] = "units";
	const char sz_axis[] = "axis";

	const char sz_lat[] = "lat";
	const char sz_lon[] = "lon";
	const char sz_time[] = "time";

	const char sz_lat_long[] = "latitude";
	const char sz_lon_long[] = "longitude";

	const char sz_lat_degrees[] = "degrees_north";
	const char sz_lon_degrees[] = "degrees_east";

	const char sz_calendar[]= "calendar";
	const char sz_calendar_type[]= "proleptic_gregorian";

	const char sz_months_since[] = "months since 1661-01-01 00:00:00";

	const char sz_missing_value[] = "missing_value";
	
	const char sz_title[] = "title";
	const char sz_title_desc[] = "3D-CMCC-CNR-BGC";

	const char sz_contact[] = "contact";
	const char sz_contact_desc[] = "Alessio Collalti (alessio.collalti@cmcc.it, alessio.collalti@isafom.cnr.it) - "
									"Alessio Ribeca (alessio.ribeca@cmcc.it, a.ribeca@unitus.it)"
	;

	const char sz_institution[] = "institution";
	const char sz_institution_desc[] =	"Foundation euroMediterranean Center on Climate Change (CMCC) Division Impacts on Agriculture, Forest and Ecosystem Services (IAFES) - "
										"CNR-ISAFOM National Research Council of Italy Institute for Agricultural and Forestry Systems in the Mediterranean(ISAFOM)"
	;

	ret = 0; // default to err
	index = NULL;
	
	assert(dataset);

	index = malloc(dataset->rows_count*sizeof*index);
	if ( ! index )
	{
		puts(sz_err_out_of_memory);
		return 0;
	}

	if ( ANNUAL_DATASET_TYPE == dataset->type )
	{
		for ( i = 0; i < dataset->rows_count; ++i )
		{
			index[i] = dataset->start_year - 1661 + i;
		}
	}

	// get sitename
	sitename = dataset->sitename;
	site_index = -1;
	for ( i = 0; i < SIZE_OF_ARRAY(sites); ++i )
	{
		if ( ! string_compare_i(sitename, sites[i].model_name) )
		{
			sitename = sites[i].isimip_name;
			site_index = i;
			break;
		}
	}

	for ( i = 0; i < dataset->columns_count; ++i )
	{
		char buf2[BUF_SIZE];
		int j;
		int nc_id, lon_dim_id, lat_dim_id, time_dim_id;
		int lon_id, lat_id, time_id, var_id;
		int var_dim_ids[3];
		int year;
		int years_loop;
		double lat;
		double lon;
		
		// skip var
		if ( ANNUAL_DATASET_TYPE == dataset->type )
		{
			if (	(ANNUAL_STEM_C == i)
					|| (ANNUAL_MAX_FROOT_C == i)
					|| (ANNUAL_CROOT_C == i)
					|| (ANNUAL_BRANCH_C == i)
					|| (ANNUAL_FRUIT_C == i)
				)
			{
				continue;
			}			
		}
		// DAILY_DATASET_TYPE
		else
		{
			if (	
					(DAILY_LAI == i)
					|| (DAILY_DELTA_STEM_C == i)
					|| (DAILY_DELTA_BRANCH_C == i)
					|| (DAILY_DELTA_FRUIT_C == i)
					|| (DAILY_DELTA_CROOT_C == i)
					|| (DAILY_FROOT_AR == i)
					|| (DAILY_CROOT_AR == i)
				)
			{
				continue;
			}
		}

		if ( ANNUAL_DATASET_TYPE == dataset->type )
		{
			years_loop = 1;
		}
		else
		// DAILY_DATASET_TYPE
		{
			years_loop = ((dataset->end_year - dataset->start_year + 1) / 10) + 1;
		}

		for ( year = 0; year < years_loop; ++year )
		{
			int start_year;
			int end_year;
			int rows_count;
			int start_data;
			
			if ( ANNUAL_DATASET_TYPE == dataset->type )
			{
				start_year = dataset->start_year;
				end_year = dataset->end_year;
				rows_count = dataset->rows_count;
				start_data = 0;
			}
			else
			// DAILY_DATASET_TYPE
			{			
				// disabled 10 years split
			#if 0
				int days_count;

				start_year = dataset->start_year + year * 10;
				end_year = start_year + 10;
				if ( end_year > dataset->end_year )
				{
					end_year = dataset->end_year;
					++end_year;
				}
			
				// compute days from 1661
				days_count = 0;
				for ( j = 1661; j < start_year; ++j )
				{
					days_count += (IS_LEAP_YEAR(j) ? 366: 365);
				}

				// compute rows_count
				rows_count = 0;
				for ( j = start_year; j < end_year; ++j )
				{
					rows_count += (IS_LEAP_YEAR(j) ? 366 : 365); 
				}			
				
				for ( j = 0; j < rows_count; ++j )
				{
					index[j] = days_count + j;
				}
				
				// compute start data
				start_data = 0;
				for ( j = dataset->start_year; j < start_year; ++j )
				{
					start_data += (IS_LEAP_YEAR(j) ? 366 : 365); 
				}
				--end_year;
			#else
				start_year = dataset->start_year;
				end_year = dataset->end_year;
				rows_count = dataset->rows_count;
				start_data = 0;
			#endif
			}
					
			// compute filename
			if ( ! folder )
			{
				folder = dataset->path ? dataset->path : "";
			}
			sprintf(buf, "%s%.*s_%s.nc"
								, folder
								, strlen(dataset->filename)-4, dataset->filename
								, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].standard_name : daily_vars[i].standard_name
			);
									
			// convert INVALID_VALUE TO NC_MISSING_VALUE
 			for ( j = 0; j < dataset->rows_count; ++j )
			{
				if ( IS_INVALID_VALUE(dataset->vars[i][j]) )
				{
					dataset->vars[i][j] = (float)NC_MISSING_VALUE;
				}
			}

			// create nc file
			if ( (ret = nc_create(buf, NC_CLOBBER | NC_NETCDF4 | NC_CLASSIC_MODEL, &nc_id)) )
				goto err;

			// define dims
			if ( (ret = nc_def_dim(nc_id, sz_time, rows_count, &time_dim_id)) )
				goto err;
			if ( (ret = nc_def_dim(nc_id, sz_lon, 1, &lon_dim_id)) )
				goto err;
			if ( (ret = nc_def_dim(nc_id, sz_lat, 1, &lat_dim_id)) )
				goto err;
			
			// define vars
			if ( (ret = nc_def_var(nc_id, sz_time, NC_DOUBLE, 1, &time_dim_id, &time_id)) )
				goto err;
			if ( (ret = nc_def_var(nc_id, sz_lon, NC_DOUBLE, 1, &lon_dim_id, &lon_id)) )
				goto err;
			if ( (ret = nc_def_var(nc_id, sz_lat, NC_DOUBLE, 1, &lat_dim_id, &lat_id)) )
				goto err;
			
			var_dim_ids[0] = time_dim_id;
			var_dim_ids[1] = lat_dim_id;
			var_dim_ids[2] = lon_dim_id;
			if ( (ret = nc_def_var(nc_id, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].standard_name : daily_vars[i].standard_name, NC_FLOAT, 3, var_dim_ids, &var_id)) )
				goto err;

			// enable compression
		#if 0
			if ( (ret = nc_def_var_deflate(nc_id, lat_id, shuffle, deflate, deflate_level)) )
				goto err;
			if ( (ret = nc_def_var_deflate(nc_id, lon_id, shuffle, deflate, deflate_level)) )
				goto err;
			if ( (ret = nc_def_var_deflate(nc_id, var_id, shuffle, deflate, deflate_level)) )
				goto err;
			if ( (ret = nc_def_var_deflate(nc_id, time_id, shuffle, deflate, deflate_level)) )
				goto err;
		#endif

			// put attributes for lat
			if ( (ret = nc_put_att_text(nc_id, lat_id, sz_standard_name, strlen(sz_lat_long), sz_lat_long)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, lat_id, sz_long_name, strlen(sz_lat_long), sz_lat_long)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, lat_id, sz_units, strlen(sz_lat_degrees), sz_lat_degrees)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, lat_id, sz_axis, 1, "Y")) )
				goto err;

			// put attributes for lon
			if ( (ret = nc_put_att_text(nc_id, lon_id, sz_standard_name, strlen(sz_lon_long), sz_lon_long)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, lon_id, sz_long_name, strlen(sz_lon_long), sz_lon_long)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, lon_id, sz_units, strlen(sz_lon_degrees), sz_lon_degrees)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, lon_id, sz_axis, 1, "X")) )
				goto err;

			// put attributes for time
			if ( (ret = nc_put_att_text(nc_id, time_id, sz_standard_name, strlen(sz_time), sz_time)) )
				goto err;
			sprintf(buf2, "%s since 1661-01-01 00:00:00", (ANNUAL_DATASET_TYPE == dataset->type) ? "years" : "days"); 
			if ( (ret = nc_put_att_text(nc_id, time_id, sz_units, strlen(buf2), buf2)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, time_id, sz_calendar, strlen(sz_calendar_type), sz_calendar_type)) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, time_id, sz_axis, 1, "T")) )
				goto err;
			if ( (ret = nc_put_att_text(nc_id, time_id, sz_long_name, strlen(sz_time), sz_time)) )
				goto err;

			// put attributes for var
			j = sprintf(buf2, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].standard_name : daily_vars[i].standard_name);
			if ( (ret = nc_put_att_text(nc_id, var_id, sz_standard_name, strlen(buf2), buf2)) )
				goto err;

			j = sprintf(buf2, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].standard_name : daily_vars[i].long_name);
			if ( (ret = nc_put_att_text(nc_id, var_id, sz_long_name, strlen(buf2), buf2)) )
				goto err;

			sprintf(buf2, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].unit : daily_vars[i].unit);
			if ( (ret = nc_put_att_text(nc_id, var_id, sz_units, strlen(buf2), buf2)) )
				goto err;

			{
				float v;

				v = (float)NC_MISSING_VALUE;
				if ( (ret = nc_put_att_float(nc_id, var_id, sz_missing_value, NC_FLOAT, 1, &v)) )
					goto err;
				if ( (ret = nc_put_att_float(nc_id, var_id, _FillValue, NC_FLOAT, 1, &v)) )
					goto err;
			}

			// set global attributes
			if ( (ret = nc_put_att_text(nc_id, NC_GLOBAL, sz_title, strlen(sz_title_desc), sz_title_desc)) )
				goto err;

			if ( (ret = nc_put_att_text(nc_id, NC_GLOBAL, sz_contact, strlen(sz_contact_desc), sz_contact_desc)) )
				goto err;

			if ( (ret = nc_put_att_text(nc_id, NC_GLOBAL, sz_institution, strlen(sz_institution_desc), sz_institution_desc)) )
				goto err;

			// end "define" mode
			if ( (ret = nc_enddef(nc_id)) )
				goto err;

			if ( -1 == site_index )
			{
				lat = (double)NC_MISSING_VALUE;
				lon = (double)NC_MISSING_VALUE;
			}
			else
			{
				lat = sites[site_index].lat;
				lon = sites[site_index].lon;
			}

			// put data
			if ( (ret = nc_put_var_double(nc_id, time_id, index)) )
				goto err;
			if ( (ret = nc_put_var1_double(nc_id, lon_id, latlon_index, &lon)) )
				goto err;
			if ( (ret = nc_put_var1_double(nc_id, lat_id, latlon_index, &lat)) )
				goto err;
		
			if ( (ret = nc_put_var_float(nc_id, var_id, dataset->vars[i]+start_data)) )
				goto err;

			// close file
			if ( (ret = nc_close(nc_id)) )
				goto err;

			// create monthly LAI_PROJ on last loop
			// so we can reuse vars !
			if (	(i == dataset->columns_count-1)
					&& (DAILY_DATASET_TYPE == dataset->type)
					&& (year == years_loop - 1) )
			{
				rows_count = (dataset->end_year - dataset->start_year + 1) * 12;
				
				sprintf(buf, "%smonthly%.*s_LAI.nc", folder, strlen(dataset->filename+5)-4, dataset->filename+5);

			#if 0
				{
					char* climate_scenario;
					char* soc_scenario;
					char* co2sens_scenarios;

					if ( string_compare_i(dataset->clim_scenario, "hist") )
					{
						climate_scenario = "historical";
					}
					else
					{
						climate_scenario = dataset->clim_scenario;
					}

					if ( ! string_compare_i(dataset->exp, "2a")
							|| ! string_compare_i(dataset->exp, "local")
							|| ! string_compare_i(dataset->exp, "ft")
						)
					{
						if ( ! string_compare_i(dataset->co2_type, "var") )
						{
							soc_scenario = "man";
						}
						else
						{
							soc_scenario = "nat";
						}
					}
					else
					{
						if ( ! string_compare_i(dataset->co2_type, "var") )
						{
							soc_scenario = "2005soc";
						}
						else
						{
							soc_scenario = "off";
						}
					}

					if ( ! string_compare_i(dataset->exp, "2a")
							|| ! string_compare_i(dataset->exp, "local")
							|| ! string_compare_i(dataset->exp, "ft")
						)
					{
						if ( ! string_compare_i(dataset->co2_type, "var") )
						{
							co2sens_scenarios = "noco2";
						}
						else
						{
							co2sens_scenarios = "co2";
						}
					}
					else
					{
						if ( ! string_compare_i(dataset->co2_type, "var") )
						{
							co2sens_scenarios = "2005c02";
						}
						else
						{
							co2sens_scenarios = "c02";
						}
					}

					// get var name
					j = sprintf(buf2, "lai_");
					sprintf(buf2+j, "_%s", species[sites[site_index].species].standard_name);

					// compute filename
					if ( folder )
						j = sprintf(buf, "%s/", folder);
					else
						j = sprintf(buf, "%s", dataset->path ? dataset->path : "");
					sprintf(buf+j, "%s_%s_%s_%s_%s_%s_%s_%s_%s_%d_%d.nc"
										, modelname
										, gcms[dataset->esm]
										, "localbc"
										, climate_scenario
										, soc_scenario
										, co2sens_scenarios
										, buf2 // var
										, dataset->sitename
										, "monthly"
										, dataset->start_year
										, dataset->end_year
					);
				}
				lowercase(buf);
			#endif

				// compute mean re-using a dataset var
			{	
				int year;
				
				j = 0; // index
				for ( year = 0; year < rows_count; ++year ) 
				{
					int month;

					index[0] = (dataset->start_year - 1661) * 12;
					for ( month = 0; month < 12; ++month )
					{
						int days;
						int row;
						int n;
						float mean;

						const int days_per_month[] =
								{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

						days = days_per_month[month];

						// fix for leap february
						if ( (1 == month) && IS_LEAP_YEAR(dataset->start_year + year) )
						{
							++days;
						}

						mean = 0.;
						n = 0;
						for ( row = 0; row < days; ++row, ++j )
						{
							if ( ! IS_INVALID_VALUE(dataset->vars[DAILY_LAI][j]) )
							{
								mean += dataset->vars[DAILY_LAI][j];
								++n;
							}
						}

						if ( n )
						{
							mean /= n;
						}
						else
						{
							mean = (float)NC_MISSING_VALUE;
						}
						dataset->vars[DAILY_RAR][(year*12)+month] = mean;

						index[month] = index[0] + month;
					}
				}
			}
				
				//printf("- creating %s...", buf);		

				// create nc file
				if ( (ret = nc_create(buf, NC_CLOBBER | NC_NETCDF4 | NC_CLASSIC_MODEL, &nc_id)) )
					goto err;

				// define dims
				if ( (ret = nc_def_dim(nc_id, sz_time, rows_count, &time_dim_id)) )
					goto err;
				if ( (ret = nc_def_dim(nc_id, sz_lon, 1, &lon_dim_id)) )
					goto err;
				if ( (ret = nc_def_dim(nc_id, sz_lat, 1, &lat_dim_id)) )
					goto err;		
				
				// define vars
				if ( (ret = nc_def_var(nc_id, sz_time, NC_DOUBLE, 1, &time_dim_id, &time_id)) )
					goto err;
				if ( (ret = nc_def_var(nc_id, sz_lon, NC_DOUBLE, 1, &lon_dim_id, &lon_id)) )
					goto err;
				if ( (ret = nc_def_var(nc_id, sz_lat, NC_DOUBLE, 1, &lat_dim_id, &lat_id)) )
					goto err;
				

				var_dim_ids[0] = time_dim_id; // time goes first
				var_dim_ids[1] = lat_dim_id;
				var_dim_ids[2] = lon_dim_id;

				if ( (ret = nc_def_var(nc_id, daily_vars[DAILY_LAI].standard_name, NC_FLOAT, 3, var_dim_ids, &var_id)) )
					goto err;

				// enable compression
			#if 0
				if ( (ret = nc_def_var_deflate(nc_id, lat_id, shuffle, deflate, deflate_level)) )
					goto err;
				if ( (ret = nc_def_var_deflate(nc_id, lon_id, shuffle, deflate, deflate_level)) )
					goto err;
				if ( (ret = nc_def_var_deflate(nc_id, var_id, shuffle, deflate, deflate_level)) )
					goto err;
				if ( (ret = nc_def_var_deflate(nc_id, time_id, shuffle, deflate, deflate_level)) )
					goto err;
			#endif

				// put attributes for lat
				if ( (ret = nc_put_att_text(nc_id, lat_id, sz_standard_name, strlen(sz_lat_long), sz_lat_long)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, lat_id, sz_long_name, strlen(sz_lat_long), sz_lat_long)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, lat_id, sz_units, strlen(sz_lat_degrees), sz_lat_degrees)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, lat_id, sz_axis, 1, "Y")) )
					goto err;

				// put attributes for lon
				if ( (ret = nc_put_att_text(nc_id, lon_id, sz_standard_name, strlen(sz_lon_long), sz_lon_long)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, lon_id, sz_long_name, strlen(sz_lon_long), sz_lon_long)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, lon_id, sz_units, strlen(sz_lon_degrees), sz_lon_degrees)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, lon_id, sz_axis, 1, "X")) )
					goto err;

				// put attributes for time
				if ( (ret = nc_put_att_text(nc_id, time_id, sz_standard_name, strlen(sz_time), sz_time)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, time_id, sz_units, strlen(sz_months_since), sz_months_since)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, time_id, sz_calendar, strlen(sz_calendar_type), sz_calendar_type)) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, time_id, sz_axis, 1, "T")) )
					goto err;
				if ( (ret = nc_put_att_text(nc_id, time_id, sz_long_name, strlen(sz_time), sz_time)) )
					goto err;

				// put attributes for var
				j = sprintf(buf2, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].standard_name : daily_vars[i].standard_name);
				if ( (ret = nc_put_att_text(nc_id, var_id, sz_standard_name, strlen(buf2), buf2)) )
					goto err;

				j = sprintf(buf2, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].standard_name : daily_vars[i].long_name);
				if ( (ret = nc_put_att_text(nc_id, var_id, sz_long_name, strlen(buf2), buf2)) )
					goto err;

				sprintf(buf2, (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i].unit : daily_vars[i].unit);
				if ( (ret = nc_put_att_text(nc_id, var_id, sz_units, strlen(buf2), buf2)) )
					goto err;

				{
					float v;

					v = (float)NC_MISSING_VALUE;
					if ( (ret = nc_put_att_float(nc_id, var_id, sz_missing_value, NC_FLOAT, 1, &v)) )
						goto err;
					if ( (ret = nc_put_att_float(nc_id, var_id, _FillValue, NC_FLOAT, 1, &v)) )
						goto err;
				}

				// set global attributes
				if ( (ret = nc_put_att_text(nc_id, NC_GLOBAL, sz_title, strlen(sz_title_desc), sz_title_desc)) )
					goto err;

				if ( (ret = nc_put_att_text(nc_id, NC_GLOBAL, sz_contact, strlen(sz_contact_desc), sz_contact_desc)) )
					goto err;

				if ( (ret = nc_put_att_text(nc_id, NC_GLOBAL, sz_institution, strlen(sz_institution_desc), sz_institution_desc)) )
					goto err;

				// end "define" mode
				if ( (ret = nc_enddef(nc_id)) )
					goto err;

				// put data
				if ( (ret = nc_put_var_double(nc_id, time_id, index)) )
					goto err;
				if ( (ret = nc_put_var1_double(nc_id, lon_id, latlon_index, &lon)) )
					goto err;
				if ( (ret = nc_put_var1_double(nc_id, lat_id, latlon_index, &lat)) )
					goto err;
				if ( (ret = nc_put_var_float(nc_id, var_id, dataset->vars[DAILY_RAR])) )
					goto err;

				// close file
				if ( (ret = nc_close(nc_id)) )
					goto err;
			}
		}
	}

	if ( index ) free(index);
	return 1;

err:
	printf("error %s in %s\n", nc_strerror(ret), dataset->filename);
	if ( index ) free(index);
	return 0;
#undef NC_MISSING_VALUE
#undef BUF_SIZE
}

int convert_to_nc(const char* const filename)
{
#define BUF_SIZE (1024) // should be enough
	char folder[BUF_SIZE];
	int ret;
	reimported_dataset_t* d;

	assert(filename);

	ret = 0; //default to err

	d = reimport_dataset(filename);
	if ( ! d ) goto quit;

	/* get path */
	{
		char* p;
		char* p2;
		int path_len;

		path_len = 0;
		p = strrchr(filename, '\\'); 
		p2 = strrchr(filename, '/');
		if ( p2 > p ) p = p2;
		if (  p )
			path_len = p - filename + 1;

		if ( path_len ) {
			strncpy(folder, filename, path_len);
		}
		folder[path_len] = '\0';
	}

	if ( nc_conv(d, folder) )
	{
		ret = 1;
	}

quit:
	if ( d ) reimported_dataset_free(d);
	return ret;
#undef BUF_SIZE
}
