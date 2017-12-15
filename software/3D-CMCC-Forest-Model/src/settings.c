/* settings.c */
#include "settings.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "matrix.h"

enum {
	SETTINGS_SITENAME = 0
	, SETTINGS_VERSION
	, SETTINGS_SPATIAL
	, SETTINGS_TIME
	, SETTINGS_SPINUP
	, SETTINGS_SPINUP_YEARS
	, SETTINGS_SCREEN_OUTPUT
	, SETTINGS_DEBUG_OUTPUT
	, SETTINGS_HALFHOURLY_OUTPUT
	, SETTINGS_DAILY_OUTPUT
	, SETTINGS_MONTHLY_OUTPUT
	, SETTINGS_YEARLY_OUTPUT
	, SETTINGS_SOIL_OUTPUT
	, SETTINGS_YEAR_START
	, SETTINGS_YEAR_END
	, SETTINGS_YEAR_RESTART
	, SETTINGS_CO2_MOD
	, SETTINGS_CO2_TRANS
	, SETTINGS_YEAR_START_CO2_FIXED
	, SETTINGS_NDEP_FIXED
	, SETTINGS_TBASE_RESP
	, SETTINGS_RESP_ACCL
	, SETTINGS_REGENERATION
	, SETTINGS_MANAGEMENT
	, SETTINGS_MANAGEMENT_TYPE
	, SETTINGS_YEAR_START_MANAGEMENT
	, SETTINGS_PROGN_AUT_RESP
	, SETTINGS_SIZECELL
	, SETTINGS_Y
	, SETTINGS_CO2CONC
	, SETTINGS_CO2_INCR
	, SETTINGS_INIT_FRAC_MAXASW
	, SETTINGS_TREE_LAYER_LIMIT
	, SETTINGS_SOIL_LAYER
	, SETTINGS_MAX_LAYER_COVER
	, SETTINGS_THINNING_REGIME
	, SETTINGS_REGENERATION_SPECIES
	, SETTINGS_REGENERATION_MANAGEMENT
	, SETTINGS_REGENERATION_N_TREE
	, SETTINGS_REGENERATION_AGE
	, SETTINGS_REGENERATION_AVDBH
	, SETTINGS_REGENERATION_LAI
	, SETTINGS_REGENERATION_HEIGHT

	, SETTINGS_COUNT
};

enum
{
	SETTINGS_REPLANTED_SPECIES
	, SETTINGS_REPLANTED_MANAGEMENT
	, SETTINGS_REPLANTED_TREE
	, SETTINGS_REPLANTED_AGE
	, SETTINGS_REPLANTED_AVDBH
	, SETTINGS_REPLANTED_LAI
	, SETTINGS_REPLANTED_HEIGHT

	, REPLANTED_SETTINGS_COUNT
};

typedef struct
{
	replanted_vars_t values;
	int assigned_values[REPLANTED_SETTINGS_COUNT];
	int index;

} replanted_temp_t;


extern const char sz_err_out_of_memory[];

const char* sz_settings[SETTINGS_COUNT] = {
	"SITENAME"
	, "VERSION"
	, "SPATIAL"
	, "TIME"
	, "SPINUP"
	, "SPINUP_YEARS"
	, "SCREEN_OUTPUT"
	, "DEBUG_OUTPUT"
	, "HALFHOURLY_OUTPUT"
	, "DAILY_OUTPUT"
	, "MONTHLY_OUTPUT"
	, "ANNUAL_OUTPUT"
	, "SOIL_OUTPUT"
	, "YEAR_START"
	, "YEAR_END"
	, "YEAR_RESTART"
	, "CO2_MOD"
	, "CO2_TRANS"
	, "YEAR_START_CO2_FIXED"
	, "NDEP_FIXED"
	, "TBASE_RESP"
	, "RESP_ACCL"
	, "REGENERATION"
	, "MANAGEMENT"
	, "MANAGEMENT_TYPE"
	, "YEAR_START_MANAGEMENT"
	, "PROGN_AUT_RESP"
	, "SIZECELL"
	, "Y"
	, "CO2CONC"
	, "CO2_INCR"
	, "INIT_FRAC_MAXASW"
	, "TREE_LAYER_LIMIT"
	, "SOIL_LAYER"
	, "MAX_LAYER_COVER"
	, "THINNING_REGIME"
	, "REGENERATION_SPECIES"
	, "REGENERATION_MANAGEMENT"
	, "REGENERATION_N_TREE"
	, "REGENERATION_AGE"
	, "REGENERATION_AVDBH"
	, "REGENERATION_LAI"
	, "REGENERATION_HEIGHT"
};

const int optional[] = {
	SETTINGS_YEAR_RESTART
	, SETTINGS_MANAGEMENT_TYPE
	, SETTINGS_REGENERATION_SPECIES
	, SETTINGS_TBASE_RESP
};

static const char* sz_replanted_settings[REPLANTED_SETTINGS_COUNT] =
{
	"REPLANTED_SPECIES"
	, "REPLANTED_MANAGEMENT"
	, "REPLANTED_TREE"
	, "REPLANTED_AGE"
	, "REPLANTED_AVDBH"
	, "REPLANTED_LAI"
	, "REPLANTED_HEIGHT"
};

static int realloc_replanted_temp(replanted_temp_t** t, int* count) {
	replanted_temp_t *no_leak;

	no_leak = realloc(*t, ++*count*sizeof*no_leak);
	if ( ! no_leak )
	{
		--*count;
		return 0;
	}
	*t = no_leak;
	return 1;
}

// index is zero based
static replanted_temp_t* get_current_by_index(replanted_temp_t* p, int count, int index)
{
	int i;

	for ( i = 0; i < count; i++ )
	{
		if ( index == p[i].index )
		{
			return &p[i];
		}
	}
	return NULL;
}

static int settings_replanted_import(const char* const filename, settings_t* s)
{
#define BUFFER_SIZE	256

	char buffer[BUFFER_SIZE];
	int i;
	//int imported[REPLANTED_SETTINGS_COUNT] = { 0 };
	replanted_temp_t* temp;
	int temp_count;

	replanted_vars_t replanted = { 0 };
	FILE* f;

	const char delimiter[] = " /\"\t\r\n";

	assert(filename && s);

	temp = NULL;
	temp_count = 0;

	f = fopen(filename, "r");
	if ( ! f ) {
		puts("file not found!");
		return 0;
	}

	while ( fgets(buffer, BUFFER_SIZE, f) )
	{
		char *p;
		char *p2;
		char *token;
		int index;
		replanted_temp_t* current;

		/* remove initial spaces (if any) */
		p2 = buffer;
		while ( isspace(*p2) ) ++p2;

		/* skip empty lines and comments */
		if (	('\r' == p2[0])
				|| ('\n' == p2[0])
				|| ('/' == p2[0]) ) {
			continue;
		}

		/* get setting name */
		current = NULL;
		token = string_tokenizer(p2, delimiter, &p);
		if ( ! token ) continue;

		/* check for name */
		for ( i = 0; i < REPLANTED_SETTINGS_COUNT; i++ )
		{
			if ( ! string_compare_i(token, sz_replanted_settings[i]) )
			{
				// token found, without index

				current = get_current_by_index(temp, temp_count, 0);
				if ( ! current )
				{
					// add new
					replanted_temp_t* no_leak;
					no_leak = realloc(temp, (temp_count+1)*sizeof*no_leak);
					if ( ! no_leak )
					{
						printf("unable to retrieve index from %s\n", token);
						if ( temp ) free(temp);
						fclose(f);
						return 0;
					}
					temp = no_leak;
					current = &temp[temp_count++];
					current->index = 0;
					{
						int y;
						for ( y = 0; y < REPLANTED_SETTINGS_COUNT; y++ )
						{
							current->assigned_values[y] = 0;
						}
					}
				}
				break;
			}
			else
			{
				// not found ? maybe we have an index ?
				// e.g: REPLANTED_SPECIES_1
				p2 = strstr(token, sz_replanted_settings[i]);
				if ( 0 == p2 - token )
				{
					// get index
					if ( 1 != sscanf(token, "%*[^0-9]%d", &index) )
					{
						printf("unable to retrieve index from %s\n", token);
						if ( temp ) free(temp);
						fclose(f);
						return 0;
					}
					if ( index < 0 )
					{
						printf("bad index specified for %s\n", token);
						if ( temp ) free(temp);
						fclose(f);
						return 0;
					}
					if ( index > temp_count )
					{
						printf("bad index specified for %s: should be %d\n", token, temp_count);
						if ( temp ) free(temp);
						fclose(f);
						return 0;
					}
					current = get_current_by_index(temp, temp_count, index);
					if ( ! current )
					{
						// add new
						replanted_temp_t* no_leak;
						no_leak = realloc(temp, (temp_count+1)*sizeof*no_leak);
						if ( ! no_leak )
						{
							printf("unable to retrieve index from %s\n", token);
							if ( temp ) free(temp);
							fclose(f);
							return 0;
						}
						temp = no_leak;
						current = &temp[temp_count++];
						current->index = index;
						{
							int y;
							for ( y = 0; y < REPLANTED_SETTINGS_COUNT; y++ )
							{
								current->assigned_values[y] = 0;
							}
						}
					}
					break;
				}
			}
		}

		if ( REPLANTED_SETTINGS_COUNT == i )
		{
			continue;
		}

		if ( ! current )
		{
			fclose(f);
			return 0;
		}

		if ( current->assigned_values[i] )
		{
			if ( current->index )
				printf("%s_%d already specified\n", sz_replanted_settings[i], current->index);
			else
				printf("%s already specified\n", sz_replanted_settings[i]);
			free(temp);
			fclose(f);
			return 0;
		}

		/* get value */
		token = string_tokenizer(NULL, delimiter, &p);
		if ( ! token ) {
			if ( current->index )
				printf("no value specified for %s_%d\n", sz_replanted_settings[i], current->index);
			else
				printf("no value specified for %s\n", sz_replanted_settings[i]);
			free(temp);
			fclose(f);
			return 0;
		}

		if ( SETTINGS_REPLANTED_SPECIES == i )
		{
			strncpy(current->values.species, (const char*)token, SETTINGS_REPLANTED_SPECIES_MAX_SIZE-1);
		}
		else if ( i == SETTINGS_REPLANTED_MANAGEMENT )
		{
			if ( ('T' == token[0]) || ('t' == token[0]) ) {
				current->values.management = T;
			} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
				current->values.management = C;
			} else if ( ('N' == token[0]) || ('n' == token[0]) ) {
				current->values.management = N;
			} else {
				printf("bad management habitus specified: %s\n", token);
				free(temp);
				fclose(f);
				return 0;
			}
		} else {
			int err;
			double value;

			value = convert_string_to_float(token, &err);
			if ( err )
			{
				printf("unable to convert value for %s: \"%s\"\n", sz_replanted_settings[i], token);
				free(temp);
				fclose(f);
				return 0;
			}
			switch ( i )
			{
				case SETTINGS_REPLANTED_TREE:
					current->values.n_tree = value;
				break;

				case SETTINGS_REPLANTED_AGE:
					current->values.age = value;
				break;

				case SETTINGS_REPLANTED_AVDBH:
					current->values.avdbh = value;
				break;

				case SETTINGS_REPLANTED_LAI:
					current->values.lai = value;
				break;

				case SETTINGS_REPLANTED_HEIGHT:
					current->values.height = value;
				break;
			}
		}
		current->assigned_values[i] = 1;
	}
	fclose(f);

	// parse imported stuff
	if ( ! temp_count )
	{
		puts("no replanted stuff found!");
		return 0;
	}

	// imported all stuff ?
	for  ( i = 0; i < temp_count; i++ )
	{
		int y;
		int flag = 0;
		for  ( y = 0; y < REPLANTED_SETTINGS_COUNT; y++ )
		{
			if ( ! temp[i].assigned_values[y] )
			{
				if ( temp[i].index )
				{
					printf("%s_%d not specified\n", sz_replanted_settings[y], temp[i].index);
				}
				else
				{
					printf("%s not specified\n", sz_replanted_settings[y]);
				}
				free(temp);
				return 0;
			}
		}
	}

	// assign values
	s->replanted_count = temp_count;
	s->replanted = malloc(s->replanted_count*sizeof*s->replanted);
	if ( ! s->replanted )
	{
		puts("out of memory!");
		free(temp);
		return 0;
	}

	for ( i = 0; i < s->replanted_count; i++ )
	{
		strcpy(s->replanted[i].species, temp[i].values.species);
		s->replanted[i].management = temp[i].values.management;
		s->replanted[i].n_tree = temp[i].values.n_tree;
		s->replanted[i].age = temp[i].values.age;
		s->replanted[i].avdbh = temp[i].values.avdbh;
		s->replanted[i].lai = temp[i].values.lai;
		s->replanted[i].height = temp[i].values.height;
	}

	free(temp);

	return 1;

#undef BUFFER_SIZE
}

settings_t* settings_import(const char *const filename) {
#define BUFFER_SIZE	256
	char buffer[BUFFER_SIZE];
	int i;
	int index;
	int err;
	settings_t* s;
	int imported[SETTINGS_COUNT] = { 0 };
	FILE *f;

	const char delimiter[] = " /\"\t\r\n";

	f = fopen(filename, "r");
	if ( ! f ) {
		puts("file not found!");
		return 0;
	}

	s = malloc(sizeof*s);
	if ( ! s  ) {
		puts(sz_err_out_of_memory);
		fclose(f);
		return 0;
	}
	/* all settings values defaults to 0 ( off ) */
	memset(s, 0, sizeof*s);
	memset(imported, 0, (sizeof*imported)*SETTINGS_COUNT);

	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		char *p;
		char *p2;
		char *token;
		double value;

		/* remove initial spaces (if any) */
		p2 = buffer;
		while ( isspace(*p2) ) ++p2;

		/* skip empty lines and comments */
		if (	('\r' == p2[0])
				|| ('\n' == p2[0])
				|| ('/' == p2[0]) ) {
			continue;
		}

		/* get setting name */
		token = string_tokenizer(p2, delimiter, &p);
		if ( ! token ) continue;

		/* check for name */
		index = -1;
		for ( i = 0; i < SETTINGS_COUNT; i++ ) {
			if ( ! string_compare_i(token, sz_settings[i]) ) {
				index = i;
				break;
			}
		}
		// not found ?
		if ( -1 == index )
		{
			//printf("unknown parameter specified in settings: %s. skipped.\n", token);
			continue;
		}

		if ( imported[index] ) {
			printf("%s already specified. skipped.\n", token);
			continue;
		}

		/* get value */
		token = string_tokenizer(NULL, delimiter, &p);
		if ( ! token ) {
			int flag;

			printf("no value specified for %s\n", sz_settings[index]);
			/* check if is an optional parameter */
			flag = 0;
			for ( i = 0; i < (int)SIZE_OF_ARRAY(optional); ++i ) {
				if ( index == optional[i] ) {
					flag = 1;
					break;
				}
			}
			if ( ! flag ) {
				settings_free(s);
				fclose(f);
				return NULL;
			} else {
				puts(". skipped.");
				continue;
			}
		}

		switch ( index ) {
			case SETTINGS_SITENAME:
				strncpy(s->sitename, token, SETTINGS_SITENAME_MAX_SIZE-1);
			break;

			case SETTINGS_VERSION:
				s->version = *token;
			break;

			case SETTINGS_SPATIAL:
				s->spatial = *token;
			break;

			case SETTINGS_TIME:
				switch ( *token )
				{
					//case 'm':
					//case 'M':
					//	s->time = MONTHLY;
					//break;

					case 'd':
					case 'D':
						s->time = DAILY;
					break;

					case 'h':
					case 'H':
						s->time = HOURLY;
					break;

					case 's':
					case 'S':
						s->time = HALFHOURLY;
					break;

					default:
						puts("uncorrect time step choiced!");
						settings_free(s);
						fclose(f);
						return 0;
				}
			break;

			case SETTINGS_SPINUP:
				if ( ! string_compare_i(token, "on") ) {
					s->spinup = 1;
				}
			break;

			case SETTINGS_SCREEN_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->screen_output = 1;
				}
			break;

			case SETTINGS_DEBUG_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->debug_output = 1;
				}
			break;

			case SETTINGS_HALFHOURLY_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->halfhourly_output = 1;
				}
			break;

			case SETTINGS_DAILY_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->daily_output = 1;
				}
			break;

			case SETTINGS_MONTHLY_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->monthly_output = 1;
				}
			break;

			case SETTINGS_YEARLY_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->yearly_output = 1;
				}
			break;

			case SETTINGS_SOIL_OUTPUT:
				if ( ! string_compare_i(token, "on") ) {
					s->soil_output = 1;
				}
			break;

			case SETTINGS_CO2_MOD:
				if ( ! string_compare_i(token, "on") ) {
					s->CO2_mod = 1;
				}
			break;

			case SETTINGS_CO2_TRANS:
				if ( ! string_compare_i(token, "on") ) {
					s->CO2_trans = CO2_TRANS_ON;
				} else if ( ! string_compare_i(token, "var") ) {
					s->CO2_trans = CO2_TRANS_VAR;
				}
			break;

			case SETTINGS_NDEP_FIXED:
				if ( ! string_compare_i(token, "on") ) {
					s->Ndep_fixed = 1;
				}
			break;

			case SETTINGS_RESP_ACCL:
				if ( ! string_compare_i(token, "on") ) {
					s->Resp_accl = 1;
				}
			break;

			case SETTINGS_REGENERATION:
				if ( ! string_compare_i(token, "on") ) {
					s->regeneration = 1;
				}
			break;

			case SETTINGS_MANAGEMENT:
				if ( ! string_compare_i(token, "on") ) {
					s->management = MANAGEMENT_ON;
				} else if ( ! string_compare_i(token, "var") ) {
					s->management = MANAGEMENT_VAR;
				} else if ( ! string_compare_i(token, "var1") ) {
					s->management = MANAGEMENT_VAR1;
				}
			break;

			case SETTINGS_PROGN_AUT_RESP:
				if ( ! string_compare_i(token, "on") ) {
					s->Prog_Aut_Resp = 1;
				}
			break;

			case SETTINGS_THINNING_REGIME:
				if ( ! string_compare_i(token, "below") ) {
					s->thinning_regime = THINNING_REGIME_BELOW;
				}
			break;

			case SETTINGS_REGENERATION_SPECIES:
				strncpy(s->regeneration_species, (const char*)token, SETTINGS_REGENERATION_SPECIES_MAX_SIZE-1);
			break;

			case SETTINGS_REGENERATION_MANAGEMENT:
				if ( ('T' == token[0]) || ('t' == token[0]) ) {
					s->regeneration_management = T;
				} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
					s->regeneration_management = C;
				} else if ( ('N' == token[0]) || ('n' == token[0]) ) {
					s->regeneration_management = N;
				} else {
					printf("bad regeneration habitus specified: %s\n", token);
					settings_free(s);
					fclose(f);
					return 0;
				}
			break;

			default:
				value = convert_string_to_float(token, &err);
				if ( err ) {
					if ( ! ((SETTINGS_YEAR_RESTART == index) && ! string_compare_i(token, "off")) ) {
						printf("unable to convert value for %s: \"%s\"\n", sz_settings[index], token);
						free(s);
						fclose(f);
						return 0;
					}
				}
				switch ( index ) {
					case SETTINGS_SPINUP_YEARS:
						s->spinup_years = (int)value;
					break;

					case SETTINGS_YEAR_START:
						s->year_start = (int)value;
					break;

					case SETTINGS_YEAR_END:
						s->year_end = (int)value;
					break;

					case SETTINGS_YEAR_RESTART:
						s->year_restart = (int)value;
					break;

					case SETTINGS_YEAR_START_CO2_FIXED:
						s->year_start_co2_fixed = (int)value;
					break;

					case SETTINGS_TBASE_RESP:
						s->Tbase_resp = value;
					break;

					case SETTINGS_YEAR_START_MANAGEMENT:
						s->year_start_management = (int)value;
					break;

					case SETTINGS_MANAGEMENT_TYPE:
						s->management_type = (int)value;
					break;

					case SETTINGS_SIZECELL:
						s->sizeCell = value;
						// ALESSIOC fill me
						s->sizeCell *= s->sizeCell;
					break;

					case SETTINGS_Y:
						s->Fixed_Aut_Resp_rate = value;
					break;

					case SETTINGS_CO2CONC:
						s->co2Conc = value;
					break;

					case SETTINGS_CO2_INCR:
						s->co2_incr = value;
					break;

					case SETTINGS_INIT_FRAC_MAXASW:
						s->init_frac_maxasw = value;
					break;

					case SETTINGS_TREE_LAYER_LIMIT:
						s->tree_layer_limit = value;
					break;

					case SETTINGS_SOIL_LAYER:
						s->number_of_soil_layer = value;
					break;

					case SETTINGS_MAX_LAYER_COVER:
						s->max_layer_cover = value;
					break;

					case SETTINGS_REGENERATION_N_TREE:
						s->regeneration_n_tree = value;
					break;

					case SETTINGS_REGENERATION_AGE:
						s->regeneration_age = value;
					break;

					case SETTINGS_REGENERATION_AVDBH:
						s->regeneration_avdbh = value;
					break;

					case SETTINGS_REGENERATION_LAI:
						s->regeneration_lai = value;
					break;

					case SETTINGS_REGENERATION_HEIGHT:
						s->regeneration_height = value;
					break;
				}
		}
		imported[index] = 1;
	}
	fclose(f);

	/* check if all setting values are imported */
	for (  i = 0; i < SETTINGS_COUNT; i++ ) {
		if ( ! imported[i] ) {
			int y;
			int flag;

			/* check for optional parameter */
			flag = 0;
			for ( y = 0; y < (int)SIZE_OF_ARRAY(optional); ++y ) {
				if ( i == optional[y] ) {
					flag = 1;
					break;
				}
			}

			printf("%s was not specified.", sz_settings[i]);

			if ( ! flag ) {
				puts("");
				settings_free(s);
				return 0;
			} else {
				puts(" optional.");
			}
		}
	}

	/* check for restart year */
	if ( 0 == s->year_restart ) {
		s->year_restart = -1;
	} else if ( (s->year_restart <= s->year_start) || (s->year_restart >= s->year_end) ) {
		printf("%s must be between %d and %d not %d\n", sz_settings[SETTINGS_YEAR_RESTART]
					, s->year_start+1
					, s->year_end-1
					, s->year_restart
		);
		settings_free(s);
		return NULL;
	}

	if ( ! settings_replanted_import(filename, s) )
	{
		settings_free(s);
		s = NULL;
	}

	return s;
#undef BUFFER_SIZE
}

void settings_free(settings_t* s) {
	if ( s )
	{
		if ( s->replanted_count )
		{
			free(s->replanted);
		}

		free(s);
	}
}
