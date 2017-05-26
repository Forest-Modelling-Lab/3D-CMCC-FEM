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
	// SETTINGS_SPINUP "on" or "off"
	, SETTINGS_SCREEN_OUTPUT
	, SETTINGS_DEBUG_OUTPUT
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
	, SETTINGS_REPLANTED_SPECIES
	, SETTINGS_REPLANTED_MANAGEMENT
	, SETTINGS_REPLANTED_TREE
	, SETTINGS_REPLANTED_AGE
	, SETTINGS_REPLANTED_AVDBH
	, SETTINGS_REPLANTED_LAI
	, SETTINGS_REPLANTED_HEIGHT
	, SETTINGS_REGENERATION_SPECIES
	, SETTINGS_REGENERATION_MANAGEMENT
	, SETTINGS_REGENERATION_N_TREE
	, SETTINGS_REGENERATION_AGE
	, SETTINGS_REGENERATION_AVDBH
	, SETTINGS_REGENERATION_LAI
	, SETTINGS_REGENERATION_HEIGHT

	, SETTINGS_COUNT
};

extern const char sz_err_out_of_memory[];

const char* sz_settings[SETTINGS_COUNT] = {
	"SITENAME"
	, "VERSION"
	, "SPATIAL"
	, "TIME"
	, "SCREEN_OUTPUT"
	, "DEBUG_OUTPUT"
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
	, "REPLANTED_SPECIES"
	, "REPLANTED_MANAGEMENT"
	, "REPLANTED_TREE"
	, "REPLANTED_AGE"
	, "REPLANTED_AVDBH"
	, "REPLANTED_LAI"
	, "REPLANTED_HEIGHT"
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
	, SETTINGS_REGENERATION_SPECIES
	//, SETTINGS_REGENERATION_LAI			// (m2/m2) lai for regeneration trees (mandatory for evergreen, useless for deciduous)
};

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
		if ( -1 == index ) {
			printf("unknown parameter specified in settings: %s. skipped.\n", token);
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

			printf("no value specified for %s", token);
			/* check if is an optional parameter */
			flag = 0;
			for ( i = 0; i < SIZE_OF_ARRAY(optional); ++i ) {
				if ( index == optional[i] ) {
					flag = 1;
					break;
				}
			}
			if ( ! flag ) {
				free(s);
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
				s->time = *token;
				if ( s->time != 'd' ) {
					puts("uncorrect time step choiced!");
					free(s);
					fclose(f);
					return 0;
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
					s->management = 1;
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

			case SETTINGS_REPLANTED_SPECIES:
				strncpy(s->replanted_species, (const char*)token, SETTINGS_REPLANTED_SPECIES_MAX_SIZE-1);
			break;

			case SETTINGS_REPLANTED_MANAGEMENT:
				if ( ('T' == token[0]) || ('t' == token[0]) ) {
					s->replanted_management = T;
				} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
					s->replanted_management = C;
				} else if ( ('N' == token[0]) || ('n' == token[0]) ) {
					s->replanted_management = N;
				} else {
					printf("bad management habitus specified: %s\n", token);
					free(s);
					fclose(f);
					return 0;
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
					free(s);
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

					case SETTINGS_REPLANTED_TREE:
						s->replanted_n_tree = value;
					break;

					case SETTINGS_REPLANTED_AGE:
						s->replanted_age = value;
					break;

					case SETTINGS_REPLANTED_AVDBH:
						s->replanted_avdbh = value;
					break;

					case SETTINGS_REPLANTED_LAI:
						s->replanted_lai = value;
					break;

					case SETTINGS_REPLANTED_HEIGHT:
						s->replanted_height = value;
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
			for ( y = 0; y < SIZE_OF_ARRAY(optional); ++y ) {
				if ( i == optional[y] ) {
					flag = 1;
					break;
				}
			}

			if ( ! flag ) {
				printf("%s was not specified.\n", sz_settings[i]);
				free(s);
				return 0;
			}
		}
	}

	/* check for restart year */
	if ( 0 == s->year_restart ) {
		s->year_restart = -1;
	} else {
		if ( (s->year_restart <= s->year_start) || (s->year_restart >= s->year_end) ) {
			printf("%s must be between %d and %d not %d\n", sz_settings[SETTINGS_YEAR_RESTART]
						, s->year_start+1
						, s->year_end-1
						, s->year_restart
			);
			free(s);
			return NULL;
		}
	}

	return s;
#undef BUFFER_SIZE
}

void settings_free(settings_t* s) {
	if ( s ) free(s);
}
