/* matrix.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "soil_settings.h"
#include "allometry.h"
#include "topo.h"
#include "matrix.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "common.h"
#include "g-function.h"
#include "initialization.h"
#include "structure.h"
#include "netcdf.h"

extern logger_t* g_log;
extern settings_t* g_settings;
extern soil_settings_t *g_soil_settings;
extern topo_t *g_topo;
extern char *g_sz_program_path;
extern char *g_sz_input_path;
extern char *g_sz_parameterization_path;

/* ---------- dataset stuff ---------- */
enum {
	X_COLUMN
	, Y_COLUMN
	, LANDUSE_COLUMN
	, AGE_COLUMN
	, SPECIES_COLUMN
	, MANAGEMENT_COLUMN
	, N_COLUMN
	, STOOL_COLUMN
	, AVDBH_COLUMN
	, HEIGHT_COLUMN
	, WF_COLUMN
	, WRC_COLUMN
	, WRF_COLUMN
	, WS_COLUMN
	, WBB_COLUMN
	, WRES_COLUMN
	, LAI_COLUMN

	, COLUMNS_COUNT
};

typedef struct {
	int x;
	int y;
	e_landuse landuse;
	int age;
	char *species;
	e_management management;
	int n;
	int stool;
	double avdbh;
	double height;
	double wf;
	double wrc;
	double wrf;
	double ws;
	double wbb;
	double wres;
	double lai;
} row_t;

typedef struct {
	row_t* rows;
	int rows_count;
} dataset_t;

static const char* sz_vars[COLUMNS_COUNT] = {
	"X"
	, "Y"
	, "LANDUSE"
	, "AGE"
	, "SPECIES"
	, "MANAGEMENT"
	, "N"
	, "STOOL"
	, "AVDBH"
	, "HEIGHT"
	, "WF"
	, "WRC"
	, "WRF"
	, "WS"
	, "WBB"
	, "WRES"
	, "LAI"
};

static const char sz_species[] = "species.txt";

static const char header_delimiter[] = ", \r\n";
static const char dataset_delimiter[] = ",\r\n";

static const char err_redundancy[] = "redundancy: var \"%s\" already founded at column %d.\n";
static const char err_unable_find_column[] = "unable to find column for \"%s\" var.\n";
static const char err_conversion[] = "error during conversion of \"%s\" value at row %d, column %d.\n";
static const char err_bad_landuse_length[] =" bad landuse length at row %d, landuse must be 1 character.\n";
static const char err_bad_landuse[] ="bad landuse %c at row %d\n";
static const char err_bad_management_length[] =" bad management length at row %d, management must be 1 character.\n";
static const char err_bad_management[] = "bad management %c at row %d\n";
static const char err_too_many_column[] = "too many columns at row %d\n";

extern const char err_empty_file[];
extern const char sz_err_out_of_memory[];
extern const char err_unable_open_file[];

static char* species_get(const char* const filename, const int id) {
	char *token;
	char *p;
	char buffer[256];
	int _id;
	int err;
	FILE *f;

	f = fopen(filename, "r");
	if ( ! f ) return NULL;

	p = NULL;
	while ( fgets(buffer, 256, f) ) {
		/* get id */
		token = string_tokenizer(buffer, ",\r\n", &p);
		_id = convert_string_to_int(token, &err);
		if ( err ) goto err;
		if ( id == _id ) {
			token = string_tokenizer(NULL, ",\r\n", &p);
			p = string_copy(token);
			break;
		}
	}
err:
	fclose(f);
	return p;
}

static void dataset_free(dataset_t *p) {
	if ( p ) {
		if ( p->rows_count ) {
			int i;
			for ( i = 0; i < p->rows_count; ++i ) {
				if ( p->rows[i].species ) {
					free(p->rows[i].species);
				}
			}
			free(p->rows);
		}
		free(p);
	}
}

static dataset_t* dataset_import_nc(const char* const filename) {
	enum {
		X_DIM
		, Y_DIM

		, DIMS_COUNT
	};
	char buffer[256];
	char sz_nc_filename[256];
	int i;
	int vars[COLUMNS_COUNT];
	float f_value;
	double value;
	int dims_size[DIMS_COUNT];
	const char *sz_dims[DIMS_COUNT] = { "x", "y" };
	int _x;
	int _y;
	int y;
	int id_file;
	int ret;
	int x_id;
	int dims_count;	/* dimensions */
	int vars_count;
	int atts_count;	/* attributes */
	int unl_count;	/* unlimited dimensions */
	char name[NC_MAX_NAME+1];
	nc_type type;
	size_t size;
	int n_dims;
	int ids[NC_MAX_VAR_DIMS];
	dataset_t *d;

	size_t start[DIMS_COUNT] = { 0, 0 };
	size_t count[DIMS_COUNT] = { 1, 1 };

	d = NULL;
	sz_nc_filename[0] = '\0';
	if ( filename[1] != ':' ) {
		strncpy(sz_nc_filename, g_sz_program_path, 256);
	}
	strcat(sz_nc_filename, filename);
	ret = nc_open(sz_nc_filename, NC_NOWRITE, &id_file);
	if ( ret != NC_NOERR ) goto quit;

	ret = nc_inq(id_file, &dims_count, &vars_count, &atts_count, &unl_count);
	if ( ret != NC_NOERR ) goto quit;

	if ( ! dims_count || ! vars_count ) {
		logger(g_log, "bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		goto quit_no_nc_err;
	}

	if ( DIMS_COUNT != dims_count ) {
		logger(g_log, "bad dimension size. It should be %d not %d\n", DIMS_COUNT, dims_count);
		goto quit_no_nc_err;
	}

	/* reset */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		dims_size[i] = -1;
	}

	/* get dimensions */
	x_id = 0;
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		ret = nc_inq_dim(id_file, i, name, &size);
		if ( ret != NC_NOERR ) goto quit;
		for ( y = 0; y < DIMS_COUNT; ++y ) {
			if ( ! string_compare_i(sz_dims[y], name) ) {
				if ( dims_size[y] != -1 ) {
					logger(g_log, "dimension %s already found!\n", sz_dims[y]);
					goto quit_no_nc_err;
				}
				dims_size[y] = size;
				if ( X_DIM == y ) x_id = i;
				break;			
			}
		}
	}

	/* check if we have all dimensions */
	for ( i = 0; i < DIMS_COUNT; ++i ) {
		if ( -1 == dims_size[i] ) {
			logger(g_log, "dimension %s not found!\n", sz_dims[i]);
			goto quit_no_nc_err;
		}
	}
	
	d = malloc(sizeof*d);
	if ( ! d ) {
		logger(g_log, "%s\n", sz_err_out_of_memory);
		goto quit_no_nc_err;
	}
	d->rows_count = dims_size[X_DIM]*dims_size[Y_DIM];
	d->rows = malloc(d->rows_count*sizeof*d->rows);
	if ( ! d->rows ) {
		logger(g_log, "%s\n", sz_err_out_of_memory);
		free(d);
		goto quit_no_nc_err;
	}
	memset(d->rows, 0, sizeof(row_t)*d->rows_count);
	
	for ( _x = 0; _x < dims_size[X_DIM]; ++_x ) {
		for ( _y = 0; _y < dims_size[Y_DIM]; ++_y ) {
			for ( i = 0; i < COLUMNS_COUNT; ++i ) {
				vars[i] = 0;
			}
			for ( i = 0; i < vars_count; ++i ) {
				ret = nc_inq_var(id_file, i, name, &type, &n_dims, ids, NULL);
				if ( ret != NC_NOERR ) goto quit;
				/* who cames first ? x or y ? */
				if ( x_id == ids[0] ) {
					/* x first */
					start[0] = _x;
					start[1] = _y;			
				} else {
					/* y first */
					start[0] = _y;
					start[1] = _x;
				}
				for ( y = 0; y < COLUMNS_COUNT; ++y ) {
					if ( ! string_compare_i(name, sz_vars[y]) ) {
						int index;
						int flag_value;
						/* check if we already have imported that var */
						if ( vars[y] ) {
							logger(g_log, "var %s already imported\n", sz_vars[y]);
							goto quit_no_nc_err;
						}
						if ( DIMS_COUNT != n_dims ) {
							logger(g_log, "bad %s dimension size. It should be 2 not %d\n", sz_vars[y], n_dims);
							goto quit_no_nc_err;
						}
						/* get values */
						flag_value = 0;
						if ( NC_FLOAT == type ) {
							ret = nc_get_vara_float(id_file, i, start, count, &f_value);
							if ( ret != NC_NOERR ) goto quit;
							flag_value = 1;
						} else if ( NC_DOUBLE == type ) {
							ret = nc_get_vara_double(id_file, i, start, count, &value);
							if ( ret != NC_NOERR ) goto quit;
						} else {
							/* type format not supported! */
							logger(g_log, "type format in %s for %s column not supported\n\n", buffer, sz_vars[y]);
							goto quit_no_nc_err;
						}
						if ( flag_value ) {
							value = f_value;
						}
						index = _y * dims_size[X_DIM] + _x;
						/* ALESSIOR: those fill filled multiple times...fix it */
						d->rows[index].x = _x;
						d->rows[index].y = _y;
						switch ( y ) {
							case LANDUSE_COLUMN:
								if ( 0 == (int)value ) {
									d->rows[index].landuse = F;
								} else {
									d->rows[index].landuse = Z;
								}
							break;

							case AGE_COLUMN:
								d->rows[index].age = (int)value;
							break;

							case SPECIES_COLUMN: {
								char temp[256];
								sprintf(temp, "%s%s", g_sz_parameterization_path, sz_species);
								d->rows[index].species = species_get(temp, (int)value);
								if ( ! d->rows[index].species ) {
									logger(g_log, "unable to get species from %s\n", temp);
									goto quit_no_nc_err;
								}
							}
							break;

							case MANAGEMENT_COLUMN:
								if ( 0 == (int)value ) {
									d->rows[index].management = T;
								} else {
									d->rows[index].management = C;
								}
							break;

							case N_COLUMN:
								d->rows[index].n = (int)value;
							break;

							case STOOL_COLUMN:
								d->rows[index].stool = (int)value;
							break;

							case AVDBH_COLUMN:
								d->rows[index].avdbh = value;
							break;

							case HEIGHT_COLUMN:
								d->rows[index].height = value;
							break;

							case WF_COLUMN:
								d->rows[index].wf = value;
							break;

							case WRC_COLUMN:
								d->rows[index].wrc = value;
							break;

							case WRF_COLUMN:
								d->rows[index].wrf = value;
							break;

							case WS_COLUMN:
								d->rows[index].ws = value;
							break;

							case WBB_COLUMN:
								d->rows[index].wbb = value;
							break;

							case WRES_COLUMN:
								d->rows[index].wres = value;
							break;

							case LAI_COLUMN:
								d->rows[index].lai = value;
							break;
						}

						vars[y] = 1;
						break;
					}
				}
			}
		}
	}
	nc_close(id_file);
	return d;

quit:
	logger(g_log, nc_strerror(ret));
quit_no_nc_err:
	if ( d ) dataset_free(d);
	nc_close(id_file);
	return NULL;
}

static dataset_t* dataset_import_txt(const char* const filename) {
#define BUFFER_SIZE	1024
	int i = 0;
	int y = 0;
	char buffer[BUFFER_SIZE];
	char *token;
	char *p;
	int error;
	FILE *f;
	int *columns;
	double value;
	row_t* rows_no_leak;
	dataset_t* dataset;
	int assigned;

	if ( ! filename || '\0' == filename[0] ) {
		return NULL;
	}

	f = fopen(filename, "r");
	if ( ! f ) {
		puts(err_unable_open_file);
		return NULL;
	}
	
	if ( ! fgets(buffer, BUFFER_SIZE, f) ) {
		puts(err_empty_file);
		fclose(f);
		return 0;
	}
	
	columns = malloc(COLUMNS_COUNT*sizeof*columns);
	if ( ! columns ) {
		puts(sz_err_out_of_memory);
		fclose(f);
		return NULL;
	}
	
	for ( i = 0; i < COLUMNS_COUNT; i++ ) {
		columns[i] = -1;
	}
	
	for ( y = 0, token = string_tokenizer(buffer, header_delimiter, &p); token; token = string_tokenizer(NULL, header_delimiter, &p), ++y ) {
		for ( i = 0; i < COLUMNS_COUNT; i++ ) {
			if ( 0 == string_compare_i(token, sz_vars[i]) ) {
				/* check for duplicated columns */
				if ( -1 != columns[i] ) {
					printf(err_redundancy, token, columns[i]+1);
					free(columns);
					fclose(f);
					return NULL;
				}

				/* column founded, assign to array and skip to next token */
				columns[i] = y;
				break;
			}
		}
	}
	/* check for missing values */
	for ( i = 0; i < COLUMNS_COUNT; i++ ) {
		if ( -1 == columns[i] ) {
			printf(err_unable_find_column, sz_vars[i]);
			free(columns);
			fclose(f);
			return 0;
		}
	}

	dataset = malloc(sizeof*dataset);
	if ( ! dataset ) {
		puts(sz_err_out_of_memory);
		free(columns);
		fclose(f);
		return 0;
	}
	dataset->rows = NULL;
	dataset->rows_count = 0;

	/* get data */
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		/* remove carriage return and newline */
		for ( i = 0; buffer[i]; ++i ) {
			if ( ('\n' == buffer[i]) || ('\r' == buffer[i]) ) {
				buffer[i] = '\0';
				break;
			}
		}
		/* skip empty lines */
		if ( '\0' == buffer[0] ) {
			continue;
		}
		/* alloc memory */
		rows_no_leak = realloc(dataset->rows, (dataset->rows_count+1)*sizeof*rows_no_leak);
		if ( !rows_no_leak ) {
			puts(sz_err_out_of_memory);
			free(columns);
			dataset_free(dataset);
			fclose(f);
			return NULL;
		}

		/* assign pointer */
		dataset->rows = rows_no_leak;
		dataset->rows[dataset->rows_count++].species = NULL;

		/* get data */
		assigned = 0;
		for ( token = string_tokenizer(buffer, dataset_delimiter, &p), y = 0; token; token = string_tokenizer(NULL, dataset_delimiter, &p), ++y ) {
			/* put value at specified columns */
			for ( i = 0; i < COLUMNS_COUNT; i++ ) {
				if ( y == columns[i] ) {
					/* assigned */
					++assigned;
					/* check columns */
					if ( i == LANDUSE_COLUMN ) {
						/* check landuse length */
						if ( 1 != strlen(token) ) {
							printf(err_bad_landuse_length, dataset->rows_count);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
						/* check landuse */
						if ( ('F' == token[0]) || ('f' == token[0]) ) {
							dataset->rows[dataset->rows_count-1].landuse = F ;
						} else if ( ('Z' == token[0]) || ('z' == token[0]) ) {
							dataset->rows[dataset->rows_count-1].landuse = Z;
						} else {
							printf(err_bad_landuse, token[0], dataset->rows_count);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
					} else if ( SPECIES_COLUMN == i ) {
						dataset->rows[dataset->rows_count-1].species = string_copy(token);
						if ( ! dataset->rows[dataset->rows_count-1].species ) {
							puts(sz_err_out_of_memory);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
					} else if ( MANAGEMENT_COLUMN == i ) {
						/* check management length */
						if ( 1 != strlen(token) ) {
							printf(err_bad_management_length, dataset->rows_count);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
						/* check management */
						if ( ('T' == token[0]) || ('t' == token[0]) ) {
							dataset->rows[dataset->rows_count-1].management = T;
						} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
							dataset->rows[dataset->rows_count-1].management = C;
						} else {
							printf(err_bad_management, token[0], dataset->rows_count);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
					} else {
						value = convert_string_to_float(token, &error);
						if ( error ) {
							printf(err_conversion, token, dataset->rows_count, y+1);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}

						/* convert nan to invalid value */
						if ( value != value ) {
							value = INVALID_VALUE;
						}

						/* assign value */
						switch ( i ) {
							case X_COLUMN:
								dataset->rows[dataset->rows_count-1].x = (int)value;
							break;

							case Y_COLUMN:
								dataset->rows[dataset->rows_count-1].y = (int)value;
							break;

							case AGE_COLUMN:
								dataset->rows[dataset->rows_count-1].age = (int)value;
							break;

							case N_COLUMN:
								dataset->rows[dataset->rows_count-1].n = (int)value;
							break;

							case STOOL_COLUMN:
								dataset->rows[dataset->rows_count-1].stool = (int)value;
							break;

							case AVDBH_COLUMN:
								dataset->rows[dataset->rows_count-1].avdbh = value;
							break;

							case HEIGHT_COLUMN:
								dataset->rows[dataset->rows_count-1].height = value;
							break;

							case WF_COLUMN:
								dataset->rows[dataset->rows_count-1].wf = value;
							break;

							case WRC_COLUMN:
								dataset->rows[dataset->rows_count-1].wrc = value;
							break;

							case WRF_COLUMN:
								dataset->rows[dataset->rows_count-1].wrf = value;
							break;

							case WS_COLUMN:
								dataset->rows[dataset->rows_count-1].ws = value;
							break;

							case WBB_COLUMN:
								dataset->rows[dataset->rows_count-1].wbb = value;
							break;

							case WRES_COLUMN:
								dataset->rows[dataset->rows_count-1].wres = value;
							break;

							case LAI_COLUMN:
								dataset->rows[dataset->rows_count-1].lai = value;
							break;

							default:
								printf(err_too_many_column, dataset->rows_count);
								free(columns);
								dataset_free(dataset);
								fclose(f);
								return NULL;
						}
					}
					/* skip to next token */
					break;
				}
			}
		}

		/* check columns */
		if ( assigned != COLUMNS_COUNT ) {
			puts("not all values has been imported!");
			free(columns);
			dataset_free(dataset);
			fclose(f);
			return NULL;
		}
	}
	free(columns);
	fclose(f);
	return dataset;
#undef BUFFER_SIZE
}

/* ------- end dataset stuff ------- */

/* NON CAMBIARE IL LORO ORDINE */
static const char *sz_species_values[] = {
		/*valori relativi alla specie*/

		"LIGHT_TOL",                  //light Tolerance

		"PHENOLOGY",				//PHENOLOGY 0.1 = deciduous broadleaf, 0.2 = deciduous needle leaf, 1.1 = broad leaf evergreen, 1.2 = needle leaf evergreen

		"ALPHA",                      // Canopy quantum efficiency (molC/molPAR)
		"EPSILONgCMJ",               // Light Use Efficiency  (gC/MJ)(used if ALPHA is not available)
		//"Y",                          // Assimilate use efficiency-Respiration rate-GPP/NP
		//"EPSILONgCMJ",                // = ALPHA * GC_MOL / MOLPAR_MJ = gC/MJ


		"K",                          //Extinction coefficient for absorption of PAR by canopy for Quercus cerris L. (A. Cutini, Ann Sci For, 1996)
		"ALBEDO",
		"GAMMA_LIGHT",

		"INT_COEFF",                  //Precipitation interception coefficient

		//SPECIFIC LEAF AREA
		"SLA_AVG",                    //AVERAGE Specific Leaf Area m^2/KgC for sunlit/shaded leaves
		"SLA_RATIO",                  //(DIM) ratio of shaded to sunlit projected SLA
		"LAI_RATIO",                  //(DIM) all-sided to projected leaf area ratio

		//FRACTION BRANCH-BARK
		"FRACBB0",                    //Branch and Bark fraction at age 0 (m^2/kg)
		"FRACBB1",                    //Branch and Bark fraction for mature stands (m^2/kg)
		"TBB",                        //Age at which fracBB = (FRACBB0 + FRACBB1 )/ 2

		//MASS DENSITY
		"RHOMIN",                     //Minimum Basic Density for young Trees
		"RHOMAX",                     //Maximum Basic Density for young Trees (Ferrara-Nolè)
		"TRHO",                       //Age at which rho = (RHOMIN + RHOMAX )/2

		//VPD
		"COEFFCOND",                  //Define stomatal responsee to VPD in m/sec
		"BLCOND",                     //Canopy Boundary Layer conductance
		"MAXCOND",                    //Maximum leaf Conductance in m/sec
		"CUTCOND",                    //cuticular conductance in m/sec

		//AGE for timber
		"MAXAGE",
		"RAGE",                       //Relative Age to give fAGE = 0.5
		"NAGE",                       //Power of relative Age in function for Age
		//AGE for SHOOTS
		"MAXAGE_S",
		"RAGE_S",                       //Relative Age to give fAGE = 0.5
		"NAGE_S",                       //Power of relative Age in function for Age


		//TEMPERATURE
		"GROWTHTMIN",                 //Minimum temperature for growth
		"GROWTHTMAX",                 //Maximum temperature for growth
		"GROWTHTOPT",                 //Optimum temperature for growth
		"GROWTHSTART",                //Thermic sum  value for starting growth in °C
		"GROWTHEND",                  //Thermic sum  value for ending growth in °C
		"MINDAYLENGTH",               //minimum day length for phenology

		//SOIL WATER
		"SWPOPEN",
		"SWPCLOSE",
		"SWCONST",                    //Costant in Soil Water modifier vs Moist Ratio
		"SWPOWER",                    //Power in Soil Water modifier vs Moist Ratio

		//BIOMASS PARTITIONING CTEM
		"OMEGA_CTEM",                        //ALLOCATION PARAMETER
		"S0CTEM",                           //PARAMETER CONTROLLING ALLOCATION TO STEM
		"R0CTEM",                           //PARAMETER CONTROLLING ALLOCATION TO ROOT
		"F0CTEM",                           //PARAMETER CONTROLLING ALLOCATION TO FOLIAGE
		"RES0CTEM",                         //PARAMETER CONTROLLING ALLOCATION TO RESERVE
		"EPSILON_CTEM",                          //ALLOCATION PARAMETER IN (Kgcm^2)^-0.6
		"KAPPA",                            //ALLOCATION PARAMETER DIMENSIONLESS
		"MIN_R0CTEM",                       //MINIMUM RATE TO ROOT AT THE FIRST YEAR AFTER COPPICING
		"MAX_S0CTEM",                       //MAXIMUM RATE TO STEM AT THE FIRST YEAR AFTER COPPICING
		"YEARS_FOR_CONVERSION",        //years from coppicing to consider tree as a timber

		"FRUIT_PERC",
		"CONES_LIFE_SPAN",



		//BIOME ALLOCATION RATIOS
		"FINE_ROOT_LEAF",	//allocation new fine root C:new leaf (ratio)
		"STEM_LEAF",		//allocation new stem C:new leaf (ratio)
		"COARSE_ROOT_STEM",	//allocation new coarse root C:new stem (ratio)
		"LIVE_TOTAL_WOOD",	//allocation new live wood C:new total wood C (ratio)

		//BIOME C:N RATIOS
		"CN_LEAVES",  //CN of leaves (kgC/kgN)
		"CN_LITTER",  //CN of leaf litter (kgC/kgN)
		"CN_FINE_ROOTS",  // CN of fine roots (kgC/kgN)
		"CN_LIVE_WOODS",  //CN of live woods (kgC/kgN)
		"CN_DEAD_WOODS",  //CN of dead woods (kgC/kgN)

		//per specie caducifoglie LITTERFALL RATE = 1 !!!!!!!!!!!!!!
		//LITTERFALL
		"BUD_BURST",					//days of bud burst at the beginning of growing season (only for deciduous)
		//FOLLOWING BIOME-BGC
		"LEAF_FALL_FRAC_GROWING",		//proportions of the growing season of leaf fall
		"LEAF_LIFE_SPAN",				//Leaf life span

		//ROOT TURNOVER
		"LEAVES_FINERTTOVER",             //Average yearly leaves and fine root turnover rate
		"COARSERTTOVER",                  //Average yearly coarse root turnover rate
		"SAPWOODTTOVER",	              //Average yearly sapwood turnover rate
		"BRANCHTTOVER",	                  //Average yearly branch turnover rate
		"LIVE_WOOD_TURNOVER",             //Average yearly live wood turnover rate


		//MORTALITY
		"WSX1000",                    //Max stem mass (kg) per tree at 1000 trees/hectare
		"THINPOWER",                  //Power in self-thinning rule
		"MF",                         //Fraction mean single tree foliage biomass lost per dead tree
		"MR",                         //Fraction mean single tree root biomass lost per dead tree
		"MS",                         //Fraction mean single tree stem biomass lost per dead tree

		//ALLOMETRIC RELATIONSHIPS

		//"DBHDC",                      //dbh (cm)- crown diameter (e) ratio  from cm to meter cm-->m
		//DBHDC = 20/100
		"DBHDCMAX",                   //Low Density
		"DBHDCMIN",                   //High Density
		"SAP_A",                      //a coefficient for sapwood
		"SAP_B",                      //b coefficient for sapwood
		"SAP_LEAF",                   //sapwood_max leaf area ratio in pipe model
		"SAP_WRES",					  //Sapwood-Reserve biomass ratio used if no Wres data are available
		"HMAX",                       //Max Height in m
		"DMAX",                       //Max Diameter in cm
		"HPOWER",                     //Slope of Asymptotic Height from Sortie
		"RPOWER",                     //Slope of Asymptotic Crown-Radius from Sortie
		"b_RPOWER",
		"CHPOWER",                    //Slope of Asymptotic Crown-Height from Sortie
		"b_CHPOWER",

		"STEMCONST_P",
		"STEMPOWER_P",



		//CHAPMAN-RICHARDS relationships
		"CRA",
		"CRB",
		"CRC",

		//CROWDING COMPETITION FUNCTION
		"HDMAX",                      //Height to Base diameter ratio MAX
		"HDMIN",                      //Height to Base diameter ratio MIN

		//DENSITY FUNCTION
		"DENMAX",                     //Maximum density (trees/10000 m^2)
		"DENMIN",                     //Minimum density (trees/10000 m^2)

		//ESTABLISHMENT
		"MINPAREST",                  //Minimum Monthly PAR (W/m^2 hour) for Establishment for the Dominated Layer
		"MINRAIN",                    //Minimum annual Rain Precipitation for Establishment

		//SEEDS PRODUCTION
		"ADULT_AGE",
		"MAXSEED",                    //numero massimo semi prodotti dalla pianta (da TREEMIG)
		"MASTSEED",                   //ricorrenza anni di pasciona (da TREEMIG)
		"WEIGHTSEED",                 //peso frutto in g
		"SEXAGE",
		"GERMCAPACITY",               //Geminability (Lischke H. & Loffler T. J.)
		"MINTEMP",                    //Minimum temperature for germination in °C
		"ESTMAX",                     //Potential Establishment rate in the absence of competition

		//SEEDS PRODUCTION FROM LPJ
		"FRACFRUIT",                  //Fraction of NPP to Fruit Production


		//MANAGMENT
		"ROTATION",
		"MINAGEMANAG",
		"MINDBHMANAG",                //Minimum DBH for Managment
		"AV_SHOOT",                   //Average number of shoots produced after coppicing
};

/* error strings */
extern const char sz_err_out_of_memory[];


/* */
static int alloc_struct(void** t, int* count, unsigned int size) {
	void *no_leak;

	no_leak = realloc(*t, ++*count*size);
	if ( ! no_leak ) {
		--*count;
		return 0;
	}
	*t = no_leak;
	return 1;
}

static int fill_cell_from_species(age_t* const a, const row_t* const row) {
	static species_t species = { 0 };
	char *p;

	assert(a && row);

	p = string_copy(row->species);
	if ( ! p ) return 0;

	if ( ! alloc_struct((void **)&a->species, &a->species_count, sizeof(species_t)) ) {
		return 0;
	}

	a->species[a->species_count-1] = species;
	a->species[a->species_count-1].management = row->management;
	a->species[a->species_count-1].name = p;
	a->species[a->species_count-1].counter[N_TREE] = row->n;
	a->species[a->species_count-1].counter[N_STUMP] = row->stool;
	a->species[a->species_count-1].value[AVDBH] = row->avdbh;
	a->species[a->species_count-1].value[BIOMASS_FOLIAGE_tDM] = row->wf;
	a->species[a->species_count-1].value[BIOMASS_COARSE_ROOT_tDM] = row->wrc;
	a->species[a->species_count-1].value[BIOMASS_FINE_ROOT_tDM] = row->wrf;
	a->species[a->species_count-1].value[BIOMASS_STEM_tDM] = row->ws;
	a->species[a->species_count-1].value[BIOMASS_BRANCH_tDM] = row->wbb;
	a->species[a->species_count-1].value[RESERVE_tDM] = row->wres;
	a->species[a->species_count-1].value[LAI] = row->lai;
	a->species[a->species_count-1].turnover = malloc(a->species_count*sizeof*a->species[a->species_count-1].turnover);
	if ( ! a->species[a->species_count-1].turnover ) return 0;

	return 1;
}

static int fill_cell_from_ages(height_t* const h, const row_t* const row) {
	static age_t age = { 0 };
	assert ( h && row);
	if ( !alloc_struct((void **)&h->ages, &h->ages_count, sizeof(age_t)) ) {
		return 0;
	}
	h->ages[h->ages_count-1] = age;
	h->ages[h->ages_count-1].value = row->age;
	return fill_cell_from_species(&h->ages[h->ages_count-1], row);
}

static int fill_cell_from_heights_and_soils(cell_t *const c, const row_t *const row) {
	static height_t height = { 0 };
	static soil_t soil = { 0 };

	//check parameter
	assert(c && row);

	//alloc memory for heights
	if (!alloc_struct((void **)&c->heights, &c->heights_count, sizeof(height_t)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1] = height;

	//alloc memory for soils
	if (!alloc_struct((void **)&c->soils, &c->soils_count, sizeof(soil_t)) )
	{
		return 0;
	}
	c->soils[c->soils_count-1] = soil;

	// set values
	c->heights[c->heights_count-1].value = row->height;

	// add age
	return fill_cell_from_ages(&c->heights[c->heights_count-1], row);
}

static int fill_cell(matrix_t* const m, const row_t* const row) {
	static cell_t cell = { 0 };
	assert(m && row);

	if ( ! alloc_struct((void **)&m->cells, &m->cells_count, sizeof(cell_t)) ) {
		return 0;
	}
	m->cells[m->cells_count-1] = cell;
	m->cells[m->cells_count-1].landuse = row->landuse;
	m->cells[m->cells_count-1].x = row->x;
	m->cells[m->cells_count-1].y = row->y;
	/* ALESSIOR TODO ask ALESSIOC: fixme without -1 the model gets 1 more!! */
	m->cells[m->cells_count-1].soils_count = (int)g_settings->soil_layer -1;

	/* add species */
	return fill_cell_from_heights_and_soils(&m->cells[m->cells_count-1], row);
}

static int fill_species_from_file(species_t *const s) {
#define BUFFER_SIZE	256
	char *p;
	char *token;
	char *token2;
	char filename[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	int i;
	int y;
	int result;
	FILE *f;
	double value;
	const char species_values_delimiter[] = " \t\r\n";
	int species_count;
	int *species_flags;

	assert(s);

	species_count = SIZE_OF_ARRAY(sz_species_values);
	species_flags = malloc(sizeof*species_flags*species_count);
	if ( ! species_flags ) {
		puts(sz_err_out_of_memory);
		return 0;
	}

	for ( i = 0; i < species_count; ++i ) {
		species_flags[i] = 0;
	}
	sprintf(filename, "%s%s.txt", g_sz_parameterization_path, s->name);
	f = fopen(filename, "r");
	if ( ! f ){
		printf("unable to open %s!", filename);
		free(species_flags);
		return 0;
	}

	y = 0;
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		for ( i = 0; buffer[i]; ++i ) {
			if ( ('\r' == buffer[i])
					|| ('\n' == buffer[i]) ) {
				buffer[i] = '\0';
			}
		}
		if ( '\0' == buffer[0] ) {
			continue;
		}

		/* skip comments */
		if ( ('\\' == buffer[0]) && ('\\' == buffer[1]) ) {
			continue;
		}

		token = string_tokenizer(buffer, species_values_delimiter, &p);
		if ( !token ) {
			printf("unable to get value token in file \"%s\", line %s.\n", filename, buffer);
			free(species_flags);
			fclose(f);
			return 0;
		}

		for ( i = 0; i < species_count; ++i ) {
			if ( ! string_compare_i(sz_species_values[i], token) ) {
				token2 = string_tokenizer(NULL, species_values_delimiter, &p);
				if ( ! token2 ) {
					printf("unable to get value for \"%s\" in \"%s\".\n", token, filename);
					free(species_flags);
					fclose(f);
					return 0;
				}
				value = convert_string_to_float(token2, &result);
				if ( result ) {
					printf("unable to convert value \"%s\" for \"%s\" in \"%s\".\n", token2, token, filename);
					free(species_flags);
					fclose(f);
					return 0;
				}
				s->value[i] = value;
				species_flags[i] = 1;

				/* keep track of assigned value */
				++y;
				break;
			}
		}
	}

	/* check assigned values */
	if ( y != species_count ) {
		for ( i = 0; i < species_count; ++i ) {
			if ( ! species_flags[i] ) break;
		}
		assert(i < species_count);
		
		printf("error: %s value missing in %s\n", sz_species_values[i], filename);
		free(species_flags);
		fclose(f);
		return 0;
	}
	free(species_flags);
	fclose(f);

	return 1;
#undef BUFFER_SIZE
}

matrix_t* matrix_create(const char* const filename) {
	int row;
	int cell;
	int species;
	int age;
	int height;
	dataset_t *d;
	matrix_t* m;

	assert(filename);

	/* import txt or nc ? */
	{
	char *p;
	p = strrchr(filename, '.');
	if ( p ) {
		if ( ! string_compare_i(++p, "nc") ) {
			d = dataset_import_nc(filename);
		} else {
			d = dataset_import_txt(filename);
		}
	} else {
		printf("bad filename!");
		return NULL;
	}
	}
	if ( ! d ) return NULL;
#ifdef _DEBUG
	{
		FILE *f;
		f = fopen("debug_input.txt", "w");
		if ( f ) {
			int i;
			fputs("x,y,landuse,age,species,management,n,stool,avdbg,height,wf,wrc,wrf,ws,wbb,wres,lai\n", f);
			for ( i = 0; i < d->rows_count; ++i ) {
				fprintf(f, "%d,%d,%c,%d,%s,%c,%d,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g\n"
							, d->rows[i].x
							, d->rows[i].y
							, (F == d->rows[i].landuse) ? 'F' : 'Z'
							, d->rows[i].age
							, d->rows[i].species
							, (T == d->rows[i].management) ? 'T' : 'C'
							, d->rows[i].n
							, d->rows[i].stool
							, d->rows[i].avdbh
							, d->rows[i].height
							, d->rows[i].wf
							, d->rows[i].wrc
							, d->rows[i].wrf
							, d->rows[i].ws
							, d->rows[i].wbb
							, d->rows[i].wres
							, d->rows[i].lai
				);
			}
			fclose(f);
		}
	}
#endif

	m = malloc(sizeof*m);
	if ( ! m ) {
		dataset_free(d);
		return NULL;
	}
	m->cells = NULL;
	m->cells_count = 0;

	for ( row = 0; row < d->rows_count; row++ ) {
		if ( ! fill_cell(m, &d->rows[row]) ) {
			dataset_free(d);
			matrix_free(m);
			return NULL;
		}
	}
	dataset_free(d);

	/* fill with species values */
	for ( cell = 0; cell < m->cells_count; ++cell ) {
		for (height = 0; height < m->cells[cell].heights_count; ++height) {
			for (age = 0;  age < m->cells[cell].heights[height].ages_count; ++age ) {
				for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; ++species ) {
					if ( ! fill_species_from_file(&m->cells[cell].heights[height].ages[age].species[species]) ) {
						matrix_free(m);
						return NULL;
					}
				}
			}
		}
	}
	return m;
}

void matrix_summary(const matrix_t* const m/*, const int day, const int month, const int year*/)
{
	int cell;
	int species;
	int age;
	int height;
	int resol;
	int day = 0;
	int month = 0;
	int year = 0;

	assert (m);

	logger(g_log, "RUN COMPSET\n");

	//cell MUST be squares
	resol = (int)sqrt(g_settings->sizeCell);

	logger(g_log, "Cell resolution = %d x %d = %f m^2\n", resol, resol, g_settings->sizeCell);
	if (g_settings->version == 'f')
	{
		logger(g_log, "Model version = FEM \n");
	}
	else
	{
		logger(g_log, "Model version = BGC \n");
	}

	if (g_settings->spatial == 's')
	{
		logger(g_log, "Model spatial = spatial \n");
	}
	else
	{
		logger(g_log, "Model spatial = un-spatial \n");
	}
	logger(g_log, "Temporal scale = daily \n");
	if (g_settings->symmetric_water_competition == 'y')
	{
		//Log ("Symmetric water competition\n");
	}
	else
	{
		//Log ("Asymmetric water competition\n");
	}

	/*Site definition*/
	logger(g_log, "***************************************************\n");
	logger(g_log, "SITE DATASET\n");
	logger(g_log, "Site Name = %s\n", g_soil_settings->sitename);
	logger(g_log, "Latitude = %f° \n", g_soil_settings->values[SOIL_LAT]);
	logger(g_log, "Longitude = %f° \n", g_soil_settings->values[SOIL_LON]);
	logger(g_log, "Elevation = %g m\n", g_topo->values[TOPO_ELEV]);
	if (g_soil_settings->values[SOIL_LAT] > 0) logger(g_log, "North hemisphere\n");
	else logger(g_log, "South hemisphere\n");
	logger(g_log, "***************************************************\n");


	//loop on each cell
	for ( cell = 0; cell< m->cells_count; cell++)
	{
		if (m->cells[cell].landuse == F)
		{
			logger(g_log, "FOREST DATASET\n");
			logger(g_log, "matrix has %d cell%s\n", m->cells_count, (m->cells_count > 1 ? "s" : ""));
			logger(g_log, "****GET FOREST CHARACTERISTICS for cell  (%d, %d)****\n", m->cells[cell].x, m->cells[cell].y);
			logger(g_log, "- cell n.%02d is at %d, %d and has %d height classes \n",
					cell+1,
					m->cells[cell].x,
					m->cells[cell].y,
					m->cells[cell].heights_count);

			//loop on each height
			for ( height = 0; height < m->cells[cell].heights_count; height++ )
			{
				logger(g_log, "**(%d)\n", height + 1);
				logger(g_log, "-- height n.%02d is %f m and has %d age classes\n",
						height + 1,
						m->cells[cell].heights[height].value,
						m->cells[cell].heights[height].ages_count);

				//loop on each age
				for ( age = 0; age < m->cells[cell].heights[height].ages_count; age++ )
				{
					logger(g_log, "--- age n.%02d is %d yrs and has %d species \n\n",
							age + 1,
							m->cells[cell].heights[height].ages[age].value,
							m->cells[cell].heights[height].ages[age].species_count
					/*m->cells[cell].heights[height].ages[age].species[species].name*/);

					/* loop on each species */
					for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species ++)
					{
						Pool_fraction (&m->cells[cell].heights[height].ages[age].species[species]);

						/*************FOREST INITIALIZATION DATA***********/
						Allometry_Power_Function (&m->cells[cell].heights[height].ages[age], &m->cells[cell].heights[height].ages[age].species[species]);

						/* compute annual number of different layers */
						Annual_numbers_of_layers (&m->cells[cell]);

						/* compute forest structure */
						Daily_Forest_structure (&m->cells[cell], day, month, year);

						/* IF NO BIOMASS INITIALIZATION DATA OR TREE HEIGHTS ARE AVAILABLE FOR STAND BUT JUST DENDROMETRIC VARIABLES (i.e. AVDBH, HEIGHT, THESE ARE MANDATORY) */
						Initialization_biomass_data (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], &m->cells[cell], day, month, year);
						logger(g_log, 
								"\n\n----- CLASS DATASET-----\n"
								"----- height = %f\n"
								"----- age = %d\n"
								"----- species n.%02d is %s\n"
								"----- n = %d trees\n"
								"----- n stumps = %d stumps\n"
								"----- avdbh = %f cm\n"
								"----- wf = %f tDM/ha\n"
								"----- wr coarse = %f tDM/area\n"
								"----- wr fine = %f tDM/area\n"
								"----- wr tot = %f tDM/area\n"
								"----- ws = %f tDM/area\n"
								"----- wbb = %f tDM/area\n"
								"----- wres = %f tDM/area\n"
								"----- ws live = %f tDM/area\n"
								"----- wrc live = %f tDM/area\n"
								"----- wbb live = %f tDM/area\n"
								"----- w tot live = %f tDM/area\n"
								"----- w tot dead = %f tDM/area\n"
								"----- lai = %f tDM/area\n",

								m->cells[cell].heights[height].value,
								m->cells[cell].heights[height].ages[age].value,
								species + 1,
								m->cells[cell].heights[height].ages[age].species[species].name,
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE],
								m->cells[cell].heights[height].ages[age].species[species].counter[N_STUMP],
								m->cells[cell].heights[height].ages[age].species[species].value[AVDBH],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_BRANCH_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[RESERVE_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_LIVE_WOOD_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_LIVE_WOOD_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_DEAD_WOOD_tDM],
								m->cells[cell].heights[height].ages[age].species[species].value[LAI]);

					}
				}
			}

			/*Soil definition*/
			logger(g_log, "***************************************************\n");
			logger(g_log, "SOIL DATASET\n");
			logger(g_log, "Number of soil layers = %.0f\n", g_settings->soil_layer);
			logger(g_log, "***************************************************\n");

			Initialization_site_data (&m->cells[cell]);

		}
		else if (m->cells[cell].landuse == Z)
		{
			logger(g_log, "*********************\n\n\n");
			logger(g_log, "CROP DATASET\n");
			logger(g_log, "*(%d)\n", cell + 1);

		}
	}
}

void matrix_free(matrix_t *m) {
	int i;
	int cell;
	int age;
	int height;


	if ( m ) {
		if ( m->cells_count ) {
			for ( cell = 0 ; cell < m->cells_count; ++cell ) {
				if ( m->cells[cell].heights_count ) {
					for ( height = 0; height < m->cells[cell].heights_count; ++height ) {
						if ( m->cells[cell].heights[height].ages_count ) {
							for ( age = 0; age < m->cells[cell].heights[height].ages_count; ++age ) {
								if ( m->cells[cell].heights[height].ages[age].species ) {
									for ( i = 0; i < m->cells[cell].heights[height].ages[age].species_count; ++i ) {
										if ( m->cells[cell].heights[height].ages[age].species[i].name ) {
											free(m->cells[cell].heights[height].ages[age].species[i].name);
										}
										if ( m->cells[cell].heights[height].ages[age].species[i].turnover ) {
											free(m->cells[cell].heights[height].ages[age].species[i].turnover);
										}
									}
									free ( m->cells[cell].heights[height].ages[age].species);
								}
							}
							free ( m->cells[cell].heights[height].ages);
						}
					}
					free ( m->cells[cell].heights);
					free ( m->cells[cell].soils);
				}
				free (m->cells[cell].years);
			}
			free (m->cells);
		}
		free (m);
	}
}
