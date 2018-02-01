// dataset.h
//
// written by A.Ribeca
//
// please note that this file must be included only once!
//

typedef enum
{
	ANNUAL_DATASET_TYPE = 0
	, DAILY_DATASET_TYPE

} e_dataset_type;

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
	DAILY_GPP = 0
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
	char* sitename;
	char* clim_scenario;
	e_dataset_type type;
	int esm;
	int start_year;
	int end_year;
	int rows_count;
	int columns_count;
	double** vars;

	char* path;

} dataset_t;

const char *annual_vars[ANNUAL_VARS_COUNT] =
{
	"HEIGHT"
	, "DBH"
	, "AGE"
	, "LIVETREE"
	, "DEADTREE"
	, "THINNEDTREE"
	, "STEM_C"
	, "MAX_LEAF_C"
	, "MAX_FROOT_C"
	, "CROOT_C"
	, "BRANCH_C"
	, "FRUIT_C"
	, "BASAL_AREA"
	, "MAI"
	, "VOLUME"

	, "CMVB"
	, "CMW"
	, "CMR"
};

const char *daily_vars[DAILY_VARS_COUNT] =
{
	"GPP"
	, "NPP"
	, "RA"
	, "FAPAR"
	, "ET"
	, "INT"
	, "SOIL_EVAPO"
	, "TRA"
	, "ASW"
	, "DELTA_LEAF_C"
	, "DELTA_FROOT_C"
	, "DELTA_STEM_C"
	, "DELTA_BRANCH_C"
	, "DELTA_FRUIT_C"
	, "DELTA_CROOT_C"
	, "FROOT_AR"
	, "CROOT_AR"
	, "TSOIL"

	, "NPPAB"
	, "NPPBB"
	, "RAR"
};

void dataset_free(dataset_t* dataset)
{
	assert(dataset);

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

	if ( dataset->path )
	{
		free(dataset->path);
	}

	free(dataset);
}

dataset_t* dataset_import(const char* const filename)
{
#define BUF_SIZE (2048)		// should be enough
	char buf[BUF_SIZE];		

	e_dataset_type type;
	char site[32];			// should be enough
	char exp[16];			// should be enough
	int esm;				// should be enough
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
	dataset_t* dataset;

	const char delimiters[] = ",\r\n";

	const char* clim_scenarios[] = { "hist", "pico"
									, "rcp2p6", "rcp4p5", "rcp6p0", "rcp8p5" };

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
	if ( ! _strnicmp(filename + has_path, "annual", strlen("annual")) )
	{
		type = ANNUAL_DATASET_TYPE;
	}
	else if ( ! _strnicmp(filename + has_path, "daily", strlen("daily")) )
	{
		type = DAILY_DATASET_TYPE;
	}
	else
	{
		puts("invalid filename");
		goto quit;
	}

	// get site
	if ( 1 != sscanf(filename + has_path, "%*[^_]_%*[^_]_%[^_]_%n", site, &i) )
	{
		puts("unable to get site from filename");
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
			puts("unable to get site from filename");
			goto quit;
		}
	}

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
		puts("unable to parse filename");
		goto quit;
	}

	// check esm
	if ( (esm < 1) || (esm > 10) )
	{
		puts("invalid esm. range is 1-10");
		goto quit;
	}

	// check scenario
	for ( i = 0; i < SIZEOF_ARRAY(clim_scenarios); ++i )
	{
		if ( ! string_compare_i(clim_scenario, clim_scenarios[i]) )
		{
			break;
		}
	}
	if ( SIZEOF_ARRAY(clim_scenarios) == i )
	{
		printf("%s is an invalid clim-scenario\n", clim_scenario);
		goto quit;
	}


	f = fopen(filename, "r");
	if ( ! f )
	{
		puts("unable to open file");
		goto quit;
	}

	// parse header
	if ( ! fgets(buf, BUF_SIZE, f) )
	{
		puts("unable to get header");
		goto quit;
	}

	// get column
	columns_count = (ANNUAL_DATASET_TYPE == type) ?
						ANNUAL_IMPORT_VARS_COUNT : DAILY_IMPORT_VARS_COUNT;
	columns = malloc(columns_count*sizeof*columns);
	if ( ! columns )
	{
		puts(err_out_of_memory);
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
			if ( ! string_compare_i(token, (ANNUAL_DATASET_TYPE == type) ? annual_vars[j] : daily_vars[j]) )
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

					printf("duplicated var: %s already found on column %d\n", token, columns[j]); 
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
			printf("var %s not found\n", (ANNUAL_DATASET_TYPE == type) ? annual_vars[i] : daily_vars[i]);
			goto quit;
		}
	}
	
	// alloc memory
	dataset = malloc(sizeof*dataset);
	if ( ! dataset )
	{
		puts(err_out_of_memory);
		goto quit;
	}
	memset(dataset, 0, sizeof*dataset);
	dataset->sitename = string_copy(site);
	if ( ! dataset->sitename )
	{
		puts(err_out_of_memory);
		free(dataset);
		dataset = NULL;
		goto quit;
	}
	dataset->clim_scenario = string_copy(clim_scenario);
	if ( ! dataset->clim_scenario )
	{
		puts(err_out_of_memory);
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
			puts(err_out_of_memory);
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
		puts(err_out_of_memory);
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
			puts(err_out_of_memory);
			dataset_free(dataset);
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
					double value;

					value = convert_string_to_float(token, &err);
					if ( err )
					{
						printf("unable to convert %s at row %d\n", token, row+1);
						dataset_free(dataset);
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
			printf("imported %d columns instead of %d\n", column, dataset->columns_count);
			free(dataset);
			dataset = NULL;
			goto quit;
		}

		++row;
	}

	// count imported rows
	if ( row != dataset->rows_count )
	{
		printf("imported %d rows instead of %d\n", row, dataset->rows_count);
		free(dataset);
		dataset = NULL;
		goto quit;
	}

	// compute vars
	for ( i = 0; i < dataset->rows_count; ++i )
	{
		if ( ANNUAL_DATASET_TYPE == type )
		{
			double cmvb;
			double cmw;
			double cmr;

			double stem_c;
			double max_leaf_c;
			double max_froot_c;
			double croot_c;
			double branch_c;
			double fruit_c;

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
			double dws;
			double dwl;
			double dwbb;
			double dfruit;
			double dwfr;
			double dwcr;
			double frar;
			double crar;

			double nppab;
			double nppbb;
			double rar;

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
				dataset->vars[DAILY_TSOIL][i] += 273.13;

			

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