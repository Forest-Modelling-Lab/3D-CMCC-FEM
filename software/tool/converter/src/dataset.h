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
							// index order
	ANNUAL_HEIGHT = 0		// 2
	, ANNUAL_DBH			// 3
	, ANNUAL_AGE			// 4
	, ANNUAL_LIVE_TREE		// 25
	, ANNUAL_DEAD_TREE		// 26
	, ANNUAL_THINNED_TREE	// 27
	, ANNUAL_STEM_C			// 36
	, ANNUAL_MAX_LEAF_C		// 43
	, ANNUAL_MAX_FROOT_C	// 44
	, ANNUAL_CROOT_C		// 45
	, ANNUAL_BRANCH_C		// 49
	, ANNUAL_FRUIT_C		// 53
	, ANNUAL_BASAL_AREA		// 69
	, ANNUAL_MAI			// 73
	, ANNUAL_VOLUME			// 74
		
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
	, DAILY_C_INT
	, DAILY_SOIL_EVALPO
	, DAILY_C_TRA
	, DAILY_ASW
	, DAILY_DWL
	, DAILY_DWFR

	, DAILY_DWS
	, DAILY_DWBB
	, DAILY_DFRUIT
	, DAILY_NWFR
	, DAILY_DWCR

	, DAILY_FRAR
	, DAILY_CRAR
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
	, "LIVE_TREE"
	, "DEAD_TREE"
	, "THINNED_TREE"
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
	, "C_INT"
	, "SOIL_EVALPO"
	, "C_TRA"
	, "ASW"
	, "DWL"
	, "DWFR"
	, "DWS"
	, "DWBB"
	, "DFRUIT"
	, "NWFR"
	, "DWCR"
	, "FRAR"
	, "CRAR"
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
	
	int i;
	int has_path;
	FILE* f;
	dataset_t* dataset;

	const int annual_vars_index[ANNUAL_IMPORT_VARS_COUNT] = { 2, 3, 4, 25, 26, 27, 36, 43, 44, 45, 49, 53, 69, 73, 74 };
	const int daily_vars_index[DAILY_IMPORT_VARS_COUNT] = { 0 }; // TODO

	const char* clim_scenarios[] = { "hist", "pico", "rcp2p6", "rcp4p5", "rcp6p0", "rcp8p5" };
	
	const char annual_import_header[] = "YEAR,LAYER,HEIGHT,DBH,AGE,SPECIES,MANAGEMENT,GPP,GROSS_ASS,GR,MR,RA,NPP,CUE,Y(perc),PeakLAI,MaxLAI,SLA,SAPWOOD-AREA,CC-Proj,DBHDC,CROWN_DIAMETER,CROWN_HEIGHT,CROWN_AREA_PROJ,APAR,LIVETREE,DEADTREE,THINNEDTREE,VEG_D,FIRST_VEG_DAY,CTRANSP,CINT,CLE,WUE,MIN_RESERVE_C,RESERVE_C,STEM_C,STEMSAP_C,STEMHEART_C,STEMSAP_PERC,STEMLIVE_C,STEMDEAD_C,STEMLIVE_PERC,MAX_LEAF_C,MAX_FROOT_C,CROOT_C,CROOTLIVE_C,CROOTDEAD_C,CROOTLIVE_PERC,BRANCH_C,BRANCHLIVE_C,BRANCHDEAD_C,BRANCHLIVE_PERC,FRUIT_C,MAX_FRUIT_C,RESERVE_N,STEM_N,STEMLIVE_N,STEMDEAD_N,CROOT_N,CROOTLIVE_N,CROOTDEAD_N,BRANCH_N,BRANCHLIVE_N,BRANCHDEAD_N,FRUIT_N,STANDING_WOOD,DELTA_WOOD,CUM_DELTA_WOOD,BASAL_AREA,TREE_CAI,TREE_MAI,CAI,MAI,VOLUME,TREE_VOLUME,DELTA_TREE_VOL(perc),DELTA_AGB,DELTA_BGB,AGB,BGB,BGB:AGB,DELTA_TREE_AGB,DELTA_TREE_BGB,C_HWP,VOLUME_HWP,STEM_RA,LEAF_RA,FROOT_RA,CROOT_RA,BRANCH_RA,gpp,npp,ar,hr,rsoil,rsoilCO2,reco,nee,nep,et,le,soil-evapo,asw,iWue,vol,cum_vol,run_off,litrC,litr1C,litr2C,litr3C,litr4C,cwdC,cwd2C,cwd3C,cwd4C,soilC,soil1C,soil2C,soil3C,soil4C,litrN,litr1N,litr2N,litr3N,litr4N,cwdN,cwd2N,cwd3N,cwd4N,soilN,soil1N,soil2N,soil3N,soil4N,solar_rad,tavg,tmax,tmin,tday,tnight,vpd,prcp,tsoil,rh,avg_asw,[CO2]";
	const char daily_import_header[] = "YEAR,MONTH,DAY,LAYER,HEIGHT,DBH,AGE,SPECIES,MANAGEMENT,GPP,ALPHA_EFF,ALPHA_EFF_SUN,ALPHA_EFF_SHADE,RG,RM,RA,NPP,CUE,LAI_PROJ,PEAK-LAI_PROJ,LAI_EXP,D-CC_P,DBHDC,CROWN_AREA_PROJ,CROWN_AREA_EXP,PAR,APAR,fAPAR,NTREE,VEG_D,INT,WAT,EVA,TRA,ET,LE,WUE,RESERVE_C,STEM_C,STEMSAP_C,STEMLIVE_C,STEMDEAD_C,LEAF_C,FROOT_C,CROOT_C,CROOTSAP_C,CROOTLIVE_C,CROOTDEAD_C,BRANCH_C,BRANCH_C,BRANCHLIVE_C,BRANCHDEAD_C,FRUIT_C,DELTARESERVE_C,DELTA_STEM_C,DELTA_LEAF_C,DELTA_FROOT_C,DELTA_CROOT_C,DELTA_BRANCH_C,DELTA_FRUIT_C,RESERVE_N,STEM_N,STEMLIVE_N,STEMDEAD_N,LEAF_N,FROOT_N,CROOT_N,CROOTLIVE_N,CROOTDEAD_N,BRANCH_N,BRANCHLIVE_N,BRANCHDEAD_N,FRUIT_N,DELTA_RESERVE_N,DELTA_STEM_N,DELTA_LEAF_N,DELTA_FROOT_N,DELTA_CROOT_N,DELTA_BRANCH_N,DELTA_FRUIT_N,STEM_AR,LEAF_AR,FROOT_AR,CROOT_AR,BRANCH_AR,F_CO2_VER,FCO2_TR,FLIGHT,FAGE,FT,FVPD,FN,FSW,LITR_C,CWD_C,gpp,npp,ar,hr,rsoil,reco,nee,nep,et,le,soil_evapo,snow_pack,asw,moist_ratio,iWue,litrC,litr1C,litr2C,litr3C,litr4C,deadwoodC,deadwood2C,deadwood3C,deadwood4C,soilC,soil1C,soil2C,soil3C,soil4C,litrN,litr1N,litr2N,litr3N,litr4N,deadwoodN,deadwood2N,deadwood3N,deadwood4N,soilN,soil1N,soil2N,soil3N,soil4N,Tsoil,Daylength";

	assert(filename);

	dataset = NULL;
	f = NULL;

	// check for path
	{
		char* p;
		char* p2;

		has_path = 0;

		p = strrchr(filename, '\\'); 
		p2 = strrchr(filename, '/');
		if ( p2 > p ) p = p2;
		has_path = p - filename + 1;
	}

	// check type
	if ( ! strncasecmp(filename + has_path, "annual", strlen("annual")) )
	{
		type = ANNUAL_DATASET_TYPE;
	}
	else if ( ! strncasecmp(filename + has_path, "daily", strlen("daily")) )
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

	// remove line endings
	buf[strcspn(buf, "\r\n")] = '\0';

	// check for header
	if ( string_compare_i(buf, (ANNUAL_DATASET_TYPE == type) ? annual_import_header :  daily_import_header) )
	{
		puts("invalid header");
		goto quit;
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
		for ( year = start_year; year < end_year; ++year )
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
		const int* indexes;
		int indexes_count;
		
		if ( ANNUAL_DATASET_TYPE == dataset->type )
		{
			indexes = annual_vars_index;
			indexes_count = ANNUAL_IMPORT_VARS_COUNT;
		}
		else // daily
		{
			indexes = daily_vars_index;
			indexes_count = DAILY_IMPORT_VARS_COUNT;
		}

		row = 0;
		while ( fgets(buf, BUF_SIZE, f) )
		{
			char* token;
			char* p;
			int column;

			const char delimiters[] = ",\r\n";

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

				for ( j = 0; j < indexes_count; ++j )
				{
					if ( i == indexes[j] )
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

			if ( column != indexes_count )
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
			}
			else // daily
			{
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

				for ( i = 0; i < dataset->rows_count; ++i )
				{
					if ( ANNUAL_DATASET_TYPE == dataset->type )
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
					else // daily
					{
						// TODO
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
	if ( f ) fclose(f);
	return dataset;

#undef BUF_SIZE
}
