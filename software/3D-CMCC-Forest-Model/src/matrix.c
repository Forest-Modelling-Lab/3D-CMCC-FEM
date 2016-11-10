/* matrix.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

extern logger_t* g_debug_log;
extern settings_t* g_settings;
extern soil_settings_t *g_soil_settings;
extern topo_t *g_topo;
extern char *g_sz_program_path;
extern char *g_sz_parameterization_path;
extern char g_sz_parameterization_output_path[];
extern int g_year_start_index;

/* ---------- dataset stuff ---------- */
enum {
	YEAR_COLUMN
	, X_COLUMN
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
	int year_stand;
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
		"YEAR"
		, "X"
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

static const char sz_species[] = "species_code.txt";

static const char delimiter[] = " ,/\r\n";

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

static dataset_t* dataset_import_nc(const char* const filename, int* const px_cells_count, int* const py_cells_count) {
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
		logger(g_debug_log, "bad nc file! %d dimensions and %d vars\n\n", dims_count, vars_count);
		goto quit_no_nc_err;
	}

	if ( DIMS_COUNT != dims_count ) {
		logger(g_debug_log, "bad dimension size. It should be %d not %d\n", DIMS_COUNT, dims_count);
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
					logger(g_debug_log, "dimension %s already found!\n", sz_dims[y]);
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
			logger(g_debug_log, "dimension %s not found!\n", sz_dims[i]);
			goto quit_no_nc_err;
		}
	}

	/* assign xy size */
	*px_cells_count = dims_size[X_DIM];
	*py_cells_count = dims_size[Y_DIM];

	d = malloc(sizeof*d);
	if ( ! d ) {
		logger(g_debug_log, "%s\n", sz_err_out_of_memory);
		goto quit_no_nc_err;
	}
	d->rows_count = dims_size[X_DIM]*dims_size[Y_DIM];
	d->rows = malloc(d->rows_count*sizeof*d->rows);
	if ( ! d->rows ) {
		logger(g_debug_log, "%s\n", sz_err_out_of_memory);
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
							logger(g_debug_log, "var %s already imported\n", sz_vars[y]);
							goto quit_no_nc_err;
						}
						if ( DIMS_COUNT != n_dims ) {
							logger(g_debug_log, "bad %s dimension size. It should be 2 not %d\n", sz_vars[y], n_dims);
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
							logger(g_debug_log, "type format in %s for %s column not supported\n\n", buffer, sz_vars[y]);
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
								logger(g_debug_log, "unable to get species from %s\n", temp);
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
	logger(g_debug_log, nc_strerror(ret));
	quit_no_nc_err:
	if ( d ) dataset_free(d);
	nc_close(id_file);
	return NULL;
}

static dataset_t* dataset_import_txt(const char* const filename) {
#define BUFFER_SIZE	1024
	int i = 0;
	int y = 0;
	int rows_count;
	char buffer[BUFFER_SIZE];
	char *token;
	char *p;
	char *p2;
	int error;
	FILE *f;
	int *columns;
	double value;
	row_t* rows_no_leak;
	row_t row;
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

	/* get header ( skip comments ) */
	do {
		if ( ! fgets(buffer, BUFFER_SIZE, f) ) {
			puts(err_empty_file);
			fclose(f);
			return 0;
		}

		/* remove initial spaces (if any) */
		p2 = buffer;
		while ( isspace(*p2) ) ++p2;

		/* skip empty lines and comments */
	} while ( ('\r' == p2[0]) || ('\n' == p2[0]) || ('/' == p2[0]) );

	columns = malloc(COLUMNS_COUNT*sizeof*columns);
	if ( ! columns ) {
		puts(sz_err_out_of_memory);
		fclose(f);
		return NULL;
	}

	for ( i = 0; i < COLUMNS_COUNT; i++ ) {
		columns[i] = -1;
	}

	for ( y = 0, token = string_tokenizer(p2, delimiter, &p); token; token = string_tokenizer(NULL, delimiter, &p), ++y ) {
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
	rows_count = 0;
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		/* remove initial spaces (if any) */
		p = buffer;
		while ( isspace(*p) ) ++p;

		/* remove comment, carriage return and newline */
		for ( i = 0; p[i]; ++i ) {
			if ( ('/' == p[i]) || ('\n' == p[i]) || ('\r' == p[i]) ) {
				buffer[i] = '\0';
				break;
			}
		}

		++rows_count;

		/* skip empty lines */
		if ( '\0' == p[0] ) {
			continue;
		}

		/* get data */
		assigned = 0;
		p2 = p;
		row.species = NULL;
		for ( token = string_tokenizer(p2, delimiter, &p), y = 0; token; token = string_tokenizer(NULL, delimiter, &p), ++y ) {
			/* put value at specified columns */
			for ( i = 0; i < COLUMNS_COUNT; i++ ) {
				if ( y == columns[i] ) {
					/* assigned */
					++assigned;
					/* check columns */
					if ( i == LANDUSE_COLUMN ) {
						/* check landuse length */
						if ( 1 != strlen(token) ) {
							printf(err_bad_landuse_length, rows_count);
							free(row.species);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
						/* check landuse */
						if ( ('F' == token[0]) || ('f' == token[0]) ) {
							row.landuse = F;
							//dataset->rows[dataset->rows_count-1].landuse = F ;
						} else if ( ('Z' == token[0]) || ('z' == token[0]) ) {
							row.landuse = Z;
							//dataset->rows[dataset->rows_count-1].landuse = Z;
						} else {
							printf(err_bad_landuse, token[0], rows_count);
							free(row.species);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
					} else if ( SPECIES_COLUMN == i ) {
						row.species = string_copy(token);
						//dataset->rows[dataset->rows_count-1].species = string_copy(token);
						//if ( ! dataset->rows[dataset->rows_count-1].species ) {
						if ( ! row.species ) {
							puts(sz_err_out_of_memory);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
					} else if ( MANAGEMENT_COLUMN == i ) {
						/* check management length */
						if ( 1 != strlen(token) ) {
							printf(err_bad_management_length, rows_count);
							free(row.species);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
						/* check management */
						if ( ('T' == token[0]) || ('t' == token[0]) ) {
							//dataset->rows[dataset->rows_count-1].management = T;
							row.management = T;
						} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
							//dataset->rows[dataset->rows_count-1].management = C;
							row.management = C;
						} else {
							printf(err_bad_management, token[0], rows_count);
							free(row.species);
							free(columns);
							dataset_free(dataset);
							fclose(f);
							return NULL;
						}
					} else {
						value = convert_string_to_float(token, &error);
						if ( error ) {
							printf(err_conversion, token, rows_count, y+1);
							free(row.species);
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
							case YEAR_COLUMN:
								//dataset->rows[dataset->rows_count-1].year_stand = (int)value;
								row.year_stand = (int)value;
							break;

							case X_COLUMN:
								//dataset->rows[dataset->rows_count-1].x = (int)value;
								row.x = (int)value;
							break;

							case Y_COLUMN:
								//dataset->rows[dataset->rows_count-1].y = (int)value;
								row.y = (int)value;
							break;

							case AGE_COLUMN:
								//dataset->rows[dataset->rows_count-1].age = (int)value;
								row.age = (int)value;
							break;

							case N_COLUMN:
								//dataset->rows[dataset->rows_count-1].n = (int)value;
								row.n = (int)value;
							break;

							case STOOL_COLUMN:
								//dataset->rows[dataset->rows_count-1].stool = (int)value;
								row.stool = (int)value;
							break;

							case AVDBH_COLUMN:
								//dataset->rows[dataset->rows_count-1].avdbh = value;
								row.avdbh = value;
							break;

							case HEIGHT_COLUMN:
								//dataset->rows[dataset->rows_count-1].height = value;
								row.height = value;
							break;

							case WF_COLUMN:
								//dataset->rows[dataset->rows_count-1].wf = value;
								row.wf = value;
							break;

							case WRC_COLUMN:
								//dataset->rows[dataset->rows_count-1].wrc = value;
								row.wrc = value;
							break;

							case WRF_COLUMN:
								//dataset->rows[dataset->rows_count-1].wrf = value;
								row.wrf = value;
							break;

							case WS_COLUMN:
								//dataset->rows[dataset->rows_count-1].ws = value;
								row.ws = value;
							break;

							case WBB_COLUMN:
								//dataset->rows[dataset->rows_count-1].wbb = value;
								row.wbb = value;
							break;

							case WRES_COLUMN:
								//dataset->rows[dataset->rows_count-1].wres = value;
								row.wres = value;
							break;

							case LAI_COLUMN:
								//dataset->rows[dataset->rows_count-1].lai = value;
								row.lai = value;
							break;

							default:
								printf(err_too_many_column, rows_count);
								free(row.species);
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
			free(row.species);
			free(columns);
			dataset_free(dataset);
			fclose(f);
			return NULL;
		}

		/* check for year */
		if ( row.year_stand == g_settings->year_start ) {
			/* alloc memory */
			rows_no_leak = realloc(dataset->rows, (dataset->rows_count+1)*sizeof*rows_no_leak);
			if ( ! rows_no_leak ) {
				puts(sz_err_out_of_memory);
				free(row.species);
				free(columns);
				dataset_free(dataset);
				fclose(f);
				return NULL;
			}

			/* assign pointer */
			dataset->rows = rows_no_leak;
			dataset->rows[dataset->rows_count++] = row;
		} else {
			free(row.species);
			row.species = NULL;
		}
	}
	free(columns);
	fclose(f);

	/* how many rows we have ? */
	if ( ! dataset->rows_count ) {
		printf("starting year %d not found!!", g_settings->year_start);
		dataset_free(dataset);
		dataset = NULL;
	}

	return dataset;
#undef BUFFER_SIZE
}

/* ------- end dataset stuff ------- */


static const char *sz_species_values[] =
{
		/* SPECIES-SPECIFIC ECO-PHYSIOLOGICAL PARAMETER VALUES */
		/* NOTE: DON'T CHANGE THEIR ORDER!! */

		"LIGHT_TOL",                  /* Light Tolerance 4 = very shade intolerant (cc = 90%), 3 = shade intolerant (cc = 100%), 2 = shade tolerant (cc = 110%), 1 = very shade tolerant (cc = 120%)*/
		"PHENOLOGY",                  /* PHENOLOGY 0.1 = deciduous broadleaf, 0.2 = deciduous needle leaf, 1.1 = broad leaf evergreen, 1.2 = needle leaf evergreen*/
		"ALPHA",                      /* Canopy quantum efficiency (molC/molPAR) */
		"EPSILONgCMJ",                /* Light Use Efficiency  (gC/MJ)(used if ALPHA is not available) */
		"K",                          /* Extinction coefficient for absorption of PAR by canopy */
		"ALBEDO",                     /* Canopy albedo */
		"GAMMA_LIGHT",                /* Value for Light modifier */
		"INT_COEFF",                  /* Precipitation interception coefficient */
		"SLA_AVG",                    /* AVERAGE Specific Leaf Area m^2/KgC for sunlit/shaded leaves */
		"SLA_RATIO",                  /* (DIM) ratio of shaded to sunlit projected SLA */
		"LAI_RATIO",                  /* (DIM) all-sided to projected leaf area ratio */
		"FRACBB0",                    /* Branch and Bark fraction at age 0 (m^2/kg) */
		"FRACBB1",                    /* Branch and Bark fraction for mature stands (m^2/kg) */
		"TBB",                        /* Age at which fracBB = (FRACBB0 + FRACBB1 )/ 2 */
		"RHOMIN",                     /* Minimum Basic Density for young Trees */
		"RHOMAX",                     /* Maximum Basic Density for young Trees */
		"TRHO",                       /* Age at which rho = (RHOMIN + RHOMAX )/2 */
		"FORM_FACTOR",                /* Tree form factor (adim) */
		"COEFFCOND",                  /* Define stomatal responsee to VPD in m/sec */
		"BLCOND",                     /* Canopy Boundary Layer conductance */
		"MAXCOND",                    /* Maximum leaf Conductance in m/sec */
		"CUTCOND",                    /* Cuticular conductance in m/sec */
		"MAXAGE",                     /* Maximum tree age */
		"RAGE",                       /* Relative Age to give fAGE = 0.5 */
		"NAGE",                       /* Power of relative Age in function for Age */
		"MAXAGE_S",                   /* Maximum age for coppice */
		"RAGE_S",                     /* Relative Age to give fAGE = 0.5 */
		"NAGE_S",                     /* Power of relative Age in function for Age */
		"GROWTHTMIN",                 /* Minimum temperature for growth */
		"GROWTHTMAX",                 /* Maximum temperature for growth */
		"GROWTHTOPT",                 /* Optimum temperature for growth */
		"GROWTHSTART",                /* Thermic sum  value for starting growth in °C */
		"MINDAYLENGTH",               /* Minimum day length for phenology */
		"SWPOPEN",                    /* Soil water potential open */
		"SWPCLOSE",                   /* Soil water potential close */
		"SWCONST",                    /* Costant in Soil Water modifier vs Moist Ratio */
		"SWPOWER",                    /* Power in Soil Water modifier vs Moist Ratio */
		"OMEGA_CTEM",                 /* ALLOCATION PARAMETER */
		"S0CTEM",                     /* PARAMETER CONTROLLING ALLOCATION TO STEM */
		"R0CTEM",                     /* PARAMETER CONTROLLING ALLOCATION TO ROOT */
		"F0CTEM",                     /* PARAMETER CONTROLLING ALLOCATION TO FOLIAGE */
		"MIN_R0CTEM",                 /* MINIMUM RATE TO ROOT AT THE FIRST YEAR AFTER COPPICING */
		"MAX_S0CTEM",                 /* MAXIMUM RATE TO STEM AT THE FIRST YEAR AFTER COPPICING */
		"YEARS_FOR_CONVERSION",       /* years from coppicing to consider tree as a timber */
		"FRUIT_PERC",                 /* percentage of npp to fruit */
		"CONES_LIFE_SPAN",            /* cone life span */
		"FINE_ROOT_LEAF",             /* allocation new fine root C:new leaf (ratio) */
		"STEM_LEAF",                  /* allocation new stem C:new leaf (ratio) */
		"COARSE_ROOT_STEM",           /* allocation new coarse root C:new stem (ratio) */
		"LIVE_TOTAL_WOOD",            /* allocation new live wood C:new total wood C (ratio) */
		"CN_LEAVES",                  /* CN of leaves (kgC/kgN) */
		"CN_FALLING_LEAVES",          /* CN of leaf litter (kgC/kgN) */
		"CN_FINE_ROOTS",              /* CN of fine roots (kgC/kgN) */
		"CN_LIVE_WOODS",              /* CN of live woods (kgC/kgN) */
		"CN_DEAD_WOODS",              /* CN of dead woods (kgC/kgN) */
		"BUD_BURST",                  /* days of bud burst at the beginning of growing season (only for deciduous) */
		"LEAF_FALL_FRAC_GROWING",     /* proportions of the growing season of leaf fall */
		"LEAF_FINEROOT_TURNOVER",     /* Average yearly leaves and fine root turnover rate */
		"LIVE_WOOD_TURNOVER",         /* Average yearly live wood turnover rate */
		"DBHDCMAX",                   /* maximum dbh crown diameter relationship when minimum density */
		"DBHDCMIN",                   /* minimum dbh crown diameter relationship when maximum density */
		"SAP_A",                      /* a coefficient for sapwood */
		"SAP_B",                      /* b coefficient for sapwood */
		"SAP_LEAF",                   /* sapwood_max leaf area ratio in pipe model */
		"SAP_WRES",                   /* Sapwood-Reserve biomass ratio used if no Wres data are available */
		"STEMCONST_P",                /* constant in the stem mass vs. diameter relationship */
		"STEMPOWER_P",                /* power in the stem mass vs. diameter relationship */
		"CRA",                        /* Chapman-Richards maximum height */
		"CRB",                        /* Chapman_Richards b parameter */
		"CRC",                        /* Chapman_Richards c parameter */
		"HDMAX_A",                    /* A parameter for Height (m) to Base diameter (m) ratio MAX */
		"HDMAX_B",                    /* B parameter for Height (m) to Base diameter (m) ratio MAX */
		"HDMIN_A",                    /* A parameter for Height (m) to Base diameter (m) ratio MIN */
		"HDMIN_B",                    /* B parameter for Height (m) to Base diameter (m) ratio MIN */
		"CROWN_FORM_FACTOR",          /* Crown form factor (0 = cylinder, 1 = cone, 2 = sphere, 3 = ellipsoid) */
		"CROWN_A",                    /* Crown a parameter */
		"CROWN_B",                    /* Crown b parameter */
		"MAXSEED",                    /* numero massimo semi prodotti dalla pianta (da TREEMIG) */
		"MASTSEED",                   /* ricorrenza anni di pasciona (da TREEMIG) */
		"WEIGHTSEED",                 /* fruit wiight in g */
		"SEXAGE",                     /* Age for sexual maturity */
		"GERMCAPACITY",               /* Geminability */
		"ROTATION",                   /* rotation for final harvest (based on tree age) */
		"THINNING",                   /* thinning regime (based on year simulation) */
		"THINNING_REGIME",            /* thinning regime (0 = above, 1 = below) */
		"MINAGEMANAG",                /* Minimum age for Management */
		"MINDBHMANAG",                /* Minimum DBH for Management */
		"AV_SHOOT",                   /* Average number of shoots produced after coppicing */
};

/* error strings */
extern const char sz_err_out_of_memory[];

static int fill_cell_from_species(age_t* const a, const row_t* const row) {
	static species_t species = { 0 };
	char *p;

	/* check parameter */
	assert( a );

	p = NULL;
	if ( row )
	{
		p = string_copy(row->species);
		if ( ! p ) return 0;
	}

	if ( ! alloc_struct((void **)&a->species, &a->species_count, &a->species_avail, sizeof(species_t)) )
	{
		free(p);
		return 0;
	}

	a->species[a->species_count-1] = species;
	a->species[a->species_count-1].management = row ? row->management : T;
	a->species[a->species_count-1].name = p;
	a->species[a->species_count-1].counter[N_TREE] = row ? row->n : 0;
	a->species[a->species_count-1].counter[N_STUMP] = row ? row->stool : 0;
	a->species[a->species_count-1].value[BIOMASS_FOLIAGE_tDM] = row ? row->wf : INVALID_VALUE;
	a->species[a->species_count-1].value[BIOMASS_COARSE_ROOT_tDM] = row ? row->wrc : INVALID_VALUE;
	a->species[a->species_count-1].value[BIOMASS_FINE_ROOT_tDM] = row ? row->wrf : INVALID_VALUE;
	a->species[a->species_count-1].value[BIOMASS_STEM_tDM] = row ? row->ws : INVALID_VALUE;
	a->species[a->species_count-1].value[BIOMASS_BRANCH_tDM] = row ? row->wbb : INVALID_VALUE;
	a->species[a->species_count-1].value[RESERVE_tDM] = row ? row->wres : INVALID_VALUE;
	a->species[a->species_count-1].value[LAI_PROJ] = row ? row->lai : INVALID_VALUE;

	return 1;
}

static int fill_cell_from_ages(dbh_t* const d, const row_t* const row)
{
	static age_t age = { 0 };

	/* check parameter */
	assert ( d );

	/* alloc memory for dbhs */
	if ( !alloc_struct((void **)&d->ages, &d->ages_count, &d->ages_avail, sizeof(age_t)) )
	{
		return 0;
	}
	d->ages[d->ages_count-1] = age;

	/* set values */
	d->ages[d->ages_count-1].value = row ? row->age : INVALID_VALUE;

	/* add age */
	return fill_cell_from_species(&d->ages[d->ages_count-1], row);
}

static int fill_cell_from_dbhs(height_t* const h, const row_t* const row)
{
	static dbh_t dbh = { 0 };

	/* check parameter */
	assert( h );

	/* alloc memory for dbhs */
	if ( !alloc_struct((void **)&h->dbhs, &h->dbhs_count, &h->dbhs_avail, sizeof(dbh_t)) )
	{
		return 0;
	}
	h->dbhs[h->dbhs_count-1] = dbh;

	/* set values */
	h->dbhs[h->dbhs_count-1].value = row ? row->avdbh : INVALID_VALUE;

	/* add dbh */
	return fill_cell_from_ages(&h->dbhs[h->dbhs_count-1], row);
}

static int fill_cell_from_heights(cell_t *const c, const row_t *const row)
{
	static height_t height = { 0 };

	/* check parameter */
	assert( c );

	/* alloc memory for heights */
	if (!alloc_struct((void **)&c->heights, &c->heights_count, &c->heights_avail, sizeof(height_t)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1] = height;

	/* set values */
	c->heights[c->heights_count-1].value = row ? row->height : INVALID_VALUE;

	/* add dbh */
	return fill_cell_from_dbhs(&c->heights[c->heights_count-1], row);
}

/****************************************************************************/

static int fill_cell_from_soils(cell_t *const c)
{
	static soil_layer_s s = { 0 };

	/* check parameter */
	assert( c);

	if ( g_settings )
	{
		int i;

		for ( i = 0; i < g_settings->number_of_soil_layer; ++i )
		{
			if ( ! alloc_struct((void **)&c->soil_layers, &c->soil_layers_count, &c->s_layers_avail, sizeof(soil_layer_s)) )
			{
				return 0;
			}
			c->soil_layers[c->soil_layers_count-1] = s;
		}

		/* ok */
		return 1;
	}
	else
	{
		return 0;
	}
}
/****************************************************************************/
static int fill_cell(matrix_t* const m, row_t* const row)
{
	int i;
	int index;
	static cell_t cell = { 0 };

	assert( m && row );

	/*  check position */
	index = -1;
	for ( i = 0; i < m->cells_count; ++i )
	{
		if ( (row->x == m->cells[i].x) && (row->y == m->cells[i].y) )
		{
			index = i;
			break;
		}
	}

	if ( -1 == index )
	{
		/* add a new cell */
		if ( ! alloc_struct((void **)&m->cells, &m->cells_count, &m->cells_avail, sizeof(cell_t)) )
		{
			return 0;
		}
		index = m->cells_count-1;
		m->cells[index] = cell;
		m->cells[index].landuse = row->landuse;
		m->cells[index].x = row->x;
		m->cells[index].y = row->y;

		/* add soils */
		if ( ! fill_cell_from_soils(&m->cells[index]) ) {
			return 0;
		}
	}
	
	/* add species */
	return fill_cell_from_heights(&m->cells[index], row);
}

static int species_copy_file(const char* const path, const char* const filename) {
	return file_copy(filename, path);
}

int fill_species_from_file(species_t *const s) {
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

	if ( ! s->name ) return 0;

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

	/* copy file */
	if ( ! species_copy_file(g_sz_parameterization_output_path, filename) ) {
		printf("error: unable to copy species to %s\n", g_sz_parameterization_output_path);
		return 0;
	}

	return 1;
#undef BUFFER_SIZE
}

static int compute_x_y_cells_count(matrix_t* const m) {
	int *px;
	int x_count;
	int *py;
	int y_count;
	int i;
	int ret;

	assert(m);

	px = NULL;
	x_count = 0;
	py = NULL;
	y_count = 0;
	ret = 0;

	px = malloc(m->cells_count*sizeof*px);
	if ( ! px ) {
		puts(sz_err_out_of_memory);
		goto err;
	}

	py = malloc(m->cells_count*sizeof*py);
	if ( ! py ) {
		puts(sz_err_out_of_memory);
		goto err;
	}

	for ( i = 0; i < m->cells_count; ++i ) {
		int z;
		int flag;

		flag = 0;
		for ( z = 0; z < x_count; ++z ) {
			if ( px[z] == m->cells[i].x ) {
				flag = 1;
				break;
			}

		}
		if ( ! flag ) {
			px[x_count++] = m->cells[i].x;
		}

		flag = 0;
		for ( z = 0; z < y_count; ++z ) {
			if ( py[z] == m->cells[i].y ) {
				flag = 1;
				break;
			}

		}
		if ( ! flag ) {
			py[y_count++] = m->cells[i].y;
		}
	}

	/* 
		TODO:
		ALESSIOR

		this condition can be broken by multilayer...
		e.g.
		x,y,...
		0,0...
		0,0...
		so we need to fix it
	if  ( x_count*y_count != m->cells_count ) {
		puts("bad x and y computation!");
		goto err;
	}
	 */
	m->x_cells_count = x_count;
	m->y_cells_count = y_count;
	ret = 1;

	err:
	if ( py ) free(py);
	if ( px ) free(px);
	return ret;
}

matrix_t* matrix_create(const char* const filename) {
	int row;
	int cell;
	int species;
	int age;
	int dbh;
	int height;
	dataset_t *d;
	matrix_t* m;
	int x_cells_count;
	int y_cells_count;

	d = NULL;
	x_cells_count = 0;
	y_cells_count = 0;

	if ( filename )
	{
		/* import txt or nc ? */
		{
			char *p;
			p = strrchr(filename, '.');
			if ( p ) {
				++p;
				if ( ! string_compare_i(p, "nc") || ! string_compare_i(p, "nc4") ) {
					d = dataset_import_nc(filename, &x_cells_count, &y_cells_count);
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
				fputs("year,x,y,landuse,age,species,management,n,stool,avdbg,height,wf,wrc,wrf,ws,wbb,wres,lai\n", f);
				for ( i = 0; i < d->rows_count; ++i ) {
					fprintf(f, "%d,%d,%d,%c,%d,%s,%c,%d,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g\n"
							, d->rows[i].year_stand
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
	}

	m = malloc(sizeof*m);
	if ( ! m ) {
		dataset_free(d);
		return NULL;
	}
	m->cells = NULL;
	m->cells_count = 0;
	m->cells_avail = 0;
	m->x_cells_count = 0;
	m->y_cells_count = 0;

	if ( d ) {
		for ( row = 0; row < d->rows_count; ++row ) {
			if ( ! fill_cell(m, &d->rows[row]) ) {
				dataset_free(d);
				matrix_free(m);
				return NULL;
			}
		}
		dataset_free(d);
		d = NULL;
	} else {
		static cell_t cell = { 0 };

		// add an empty cell
		if ( ! alloc_struct((void **)&m->cells, &m->cells_count, &m->cells_avail, sizeof(cell_t)) )
		{
			return 0;
		}
		row = m->cells_count-1;
		m->cells[row] = cell;
		m->cells[row].landuse = F;
		m->cells[row].x = 0;
		m->cells[row].y = 0;

		if ( ! fill_cell_from_soils(&m->cells[row]) )
		{
			return 0;
		}

		if ( ! fill_cell_from_heights(&m->cells[row], NULL) )
		{
			return 0;
		}
		x_cells_count = 1;
		y_cells_count = 1;
	}
	
	/* fill with species values */
	for ( cell = 0; cell < m->cells_count; ++cell ) {
		for (height = 0; height < m->cells[cell].heights_count; ++height)
		{
			for (dbh = 0; dbh < m->cells[cell].heights[height].dbhs_count; ++dbh)
			{
				for (age = 0; age < m->cells[cell].heights[height].dbhs[dbh].ages_count; ++age ) {
					for ( species = 0; species < m->cells[cell].heights[height].dbhs[dbh].ages[age].species_count; ++species )
					{
						if ( ! fill_species_from_file(&m->cells[cell].heights[height].dbhs[dbh].ages[age].species[species]) )
						{
							if ( filename ) {
								matrix_free(m);
								return NULL;
							}
						}
					}
				}
			}
		}
	}

	/* compute x and y cells count */
	if ( ! compute_x_y_cells_count(m) ) {
		matrix_free(m);
		m = NULL;
	}

	/* check against nc dimension */
	if (	(x_cells_count != m->x_cells_count)
			|| (y_cells_count != m->y_cells_count) ) {
		printf("dimensions differs between nc and check: x(%d,%d), y(%d,%d)\n"
					, x_cells_count
					, m->x_cells_count
					, y_cells_count
					, m->y_cells_count
		);
		matrix_free(m);
		m = NULL;
	}

	return m;
}

void simulation_summary (const matrix_t* const m)
{
	int resol;

	/* cell MUST be in square meters */
	resol = (int)sqrt(g_settings->sizeCell);

	assert(m);

	logger(g_debug_log, "RUN COMPSET\n");

	logger(g_debug_log, "Cell resolution = %d x %d = %g m^2\n", resol, resol, g_settings->sizeCell);
	if (g_settings->version == 'f')
	{
		logger(g_debug_log, "Model version = FEM \n");
	}
	else
	{
		logger(g_debug_log, "Model version = BGC \n");
	}

	if (g_settings->spatial == 's')
	{
		logger(g_debug_log, "Model spatial = spatial \n");
	}
	else
	{
		logger(g_debug_log, "Model spatial = un-spatial \n");
	}
	logger(g_debug_log, "Temporal scale = daily \n");
}

void site_summary(const matrix_t* const m)
{
	assert(m);

	/* Site definition */
	logger(g_debug_log, "***************************************************\n\n");
	logger(g_debug_log, "SITE DATASET\n");
	logger(g_debug_log, "Site Name = %s\n", g_soil_settings->sitename);
	logger(g_debug_log, "Latitude = %f° \n", g_soil_settings->values[SOIL_LAT]);
	logger(g_debug_log, "Longitude = %f° \n", g_soil_settings->values[SOIL_LON]);
	if (g_soil_settings->values[SOIL_LAT] > 0) logger(g_debug_log, "North hemisphere\n");
	else logger(g_debug_log, "South hemisphere\n");
}

void topo_summary(const matrix_t* const m)
{
	assert(m);

	/* Topography definition */
	logger(g_debug_log, "***************************************************\n\n");
	logger(g_debug_log, "TOPOGRAPHY DATASET\n");
	logger(g_debug_log, "Elevation = %g m\n", g_topo->values[TOPO_ELEV]);
}

void soil_summary(const matrix_t* const m, const cell_t* const cell)
{
	assert(m);

	/* Soil definition and initialization */
	logger(g_debug_log, "***************************************************\n\n");
	logger(g_debug_log, "SOIL DATASET\n");
	logger(g_debug_log, "-Number of soil layers = %g\n", g_settings->number_of_soil_layer);
	logger(g_debug_log, "-Soil depth = %g cm\n", g_soil_settings->values[SOIL_DEPTH]);
	logger(g_debug_log, "-Clay Percentage = %g %%\n", g_soil_settings->values[SOIL_CLAY_PERC]);
	logger(g_debug_log, "-Silt Percentage = %g %%\n", g_soil_settings->values[SOIL_SILT_PERC]);
	logger(g_debug_log, "-Sand Percentage = %g %%\n", g_soil_settings->values[SOIL_SAND_PERC]);
	logger(g_debug_log, "-Soil FR = %g\n", g_soil_settings->values[SOIL_FR]);
	logger(g_debug_log, "-Soil FN0 = %g\n", g_soil_settings->values[SOIL_FN0]);
	logger(g_debug_log, "-Soil FNN = %g\n", g_soil_settings->values[SOIL_FNN]);
	logger(g_debug_log, "-Soil M0 = %g\n", g_soil_settings->values[SOIL_M0]);
	logger(g_debug_log, "-Soil SN = %g\n", g_soil_settings->values[SOIL_SN]);

	/* soil initialization */
	initialization_soil (m->cells);

	logger(g_debug_log, "***************************************************\n\n");
}

void forest_summary(const matrix_t* const m, const int cell, const int day, const int month, const int year)
{
	int species;
	int age;
	int dbh;
	int height;

	assert (m);

	logger(g_debug_log, "***************************************************\n");
	logger(g_debug_log, "****FOREST CHARACTERISTICS for cell (%d, %d)****\n", m->cells[cell].x, m->cells[cell].y);
	logger(g_debug_log, "FOREST DATASET\n");

	logger(g_debug_log, "matrix has %d cell%s\n", m->cells_count, (m->cells_count > 1 ? "s" : ""));

	logger(g_debug_log, "- cell n.%02d is at %d, %d and has %d height classes\n",
			cell+1,
			m->cells[cell].x,
			m->cells[cell].y,
			m->cells[cell].heights_count);

	/* loop on each height */
	for ( height = 0; height < m->cells[cell].heights_count; ++height )
	{
		logger(g_debug_log, "**(%d)\n", height + 1);
		logger(g_debug_log, "-- height n.%02d is %g and has %d dbh classes \n",
				height + 1, m->cells[cell].heights[height].value,
				m->cells[cell].heights[height].dbhs_count);

		/* loop on each dbh */
		for ( dbh = 0; dbh < m->cells[cell].heights[height].dbhs_count; ++dbh)
		{
			logger(g_debug_log, "**(%d)\n", dbh + 1);
			logger(g_debug_log, "--- dbh n.%02d is %g and has %d ages classes\n",
					dbh + 1, m->cells[cell].heights[height].dbhs[dbh].value,
					m->cells[cell].heights[height].dbhs[dbh].ages_count);

			/* loop on each age */
			for ( age = 0; age < m->cells[cell].heights[height].dbhs[dbh].ages_count; ++age )
			{
				logger(g_debug_log, "**(%d)\n", age + 1);
				logger(g_debug_log, "---- age n.%02d is %d yrs and has %d species\n",
						age + 1,
						m->cells[cell].heights[height].dbhs[dbh].ages[age].value,
						m->cells[cell].heights[height].dbhs[dbh].ages[age].species_count);

				/* loop on each species */
				for ( species = 0; species < m->cells[cell].heights[height].dbhs[dbh].ages[age].species_count; ++species )
				{
					logger(g_debug_log, "----- species is %s\n",
							m->cells[cell].heights[height].dbhs[dbh].ages[age].species[species].name);
				}
			}
		}
	}

	/*************FOREST INITIALIZATION DATA***********/
	/* initialize power function */
	allometry_power_function (&m->cells[cell]);

	/* initialize carbon pool fraction */
	carbon_pool_fraction (&m->cells[cell]);

	/* initialize forest structure */
	initialization_forest_structure (&m->cells[cell], day, month, year);

	for ( height = 0; height < m->cells[cell].heights_count; ++height )
	{
		for ( dbh = 0; dbh < m->cells[cell].heights[height].dbhs_count; ++dbh )
		{
			for ( age = 0; age < m->cells[cell].heights[height].dbhs[dbh].ages_count; ++age )
			{
				for ( species = 0; species < m->cells[cell].heights[height].dbhs[dbh].ages[age].species_count; ++species )
				{

					/* IF NO BIOMASS INITIALIZATION DATA OR TREE HEIGHTS ARE AVAILABLE FOR STAND
					 * BUT JUST DENDROMETRIC VARIABLES (i.e. AVDBH, HEIGHT, THESE ARE MANDATORY) */
					/* initialise carbon pools */
					initialization_forest_class_C_biomass (&m->cells[cell], height, dbh, age, species);

					/* initialise nitrogen pools */
					initialization_forest_class_N_biomass (&m->cells[cell], height, dbh, age, species);
				}
			}
		}
	}
}

void matrix_free(matrix_t *m)
{
	int cell;
	int height;
	int dbh;
	int age;
	int species;
	int soil;

	assert(m);

	if ( m )
	{
		if ( m->cells_count )
		{
			for ( cell = 0 ; cell < m->cells_count; ++cell )
			{			
				for ( height = 0; height < m->cells[cell].heights_count; ++height )
				{
					if ( m->cells[cell].tree_layers_count )
					{
						free( m->cells[cell].tree_layers );
					}
					if ( m->cells[cell].heights[height].dbhs_count )
					{
						for ( dbh = 0; dbh < m->cells[cell].heights[height].dbhs_count; ++dbh)
						{
							if ( m->cells[cell].heights[height].dbhs[dbh].ages_count )
							{
								for ( age = 0; age < m->cells[cell].heights[height].dbhs[dbh].ages_count; ++age )
								{
									if ( m->cells[cell].heights[height].dbhs[dbh].ages[age].species )
									{
										for ( species = 0; species < m->cells[cell].heights[height].dbhs[dbh].ages[age].species_count; ++species )
										{
											if ( m->cells[cell].heights[height].dbhs[dbh].ages[age].species[species].name )
											{
												free( m->cells[cell].heights[height].dbhs[dbh].ages[age].species[species].name );
											}
										}
										free ( m->cells[cell].heights[height].dbhs[dbh].ages[age].species );
									}
								}
								free ( m->cells[cell].heights[height].dbhs[dbh].ages );
							}
						}
						free ( m->cells[cell].heights[height].dbhs );
					}
				}

				if ( m->cells[cell].heights_count )
				{
					free (m->cells[cell].heights);
				}

				for ( soil = 0; soil < m->cells[cell].soil_layers_count; ++ soil )
				{
					if ( m->cells[cell].soil_layers_count )
					{
						free ( m->cells[cell].soil_layers );
					}
				}
				if ( g_year_start_index != -1 ) {
					free (m->cells[cell].years-g_year_start_index);
				} else {
					free (m->cells[cell].years);
				}
			}
			free (m->cells);
		}
		free (m);
	}
}
