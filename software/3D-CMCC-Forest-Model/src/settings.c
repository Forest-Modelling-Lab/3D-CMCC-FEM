/* settings.c */
#include "settings.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum {
	SETTINGS_VERSION
	, SETTINGS_SPATIAL
	, SETTINGS_TIME
	, SETTINGS_SCREEN_OUTPUT
	, SETTINGS_DEBUG_OUTPUT
	, SETTINGS_DAILY_OUTPUT
	, SETTINGS_MONTHLY_OUTPUT
	, SETTINGS_YEARLY_OUTPUT
	, SETTINGS_SOIL_OUTPUT
	, SETTINGS_CO2_MOD
	, SETTINGS_CO2_FIXED
	, SETTINGS_NDEP_FIXED
	, SETTINGS_Q10_FIXED
	, SETTINGS_REGENERATION
	, SETTINGS_MANAGEMENT
	, SETTINGS_PROGN_AUT_RESP
	, SETTINGS_DNDC
	, SETTINGS_SIZECELL
	, SETTINGS_Y
	, SETTINGS_CO2CONC
	, SETTINGS_CO2_INCR
	, SETTINGS_INIT_FRAC_MAXASW
	, SETTINGS_TREE_LAYER_LIMIT
	, SETTINGS_SOIL_LAYER
	, SETTINGS_MIN_LAYER_COVER
	, SETTINGS_MAX_LAYER_COVER
	, SETTINGS_REMAINING_BASAL_AREA
	, SETTINGS_REPLANTED_SPECIES
	, SETTINGS_REPLANTED_MANAGEMENT
	, SETTINGS_REPLANTED_TREE
	, SETTINGS_REPLANTED_AGE
	, SETTINGS_REPLANTED_AVDBH
	, SETTINGS_REPLANTED_LAI
	, SETTINGS_REPLANTED_HEIGHT
	, SETTINGS_REPLANTED_WS
	, SETTINGS_REPLANTED_WCR
	, SETTINGS_REPLANTED_WFR
	, SETTINGS_REPLANTED_WL
	, SETTINGS_REPLANTED_WBB
	, SETTINGS_REGENERATION_SPECIES
	, SETTINGS_REGENERATION_MANAGEMENT
	, SETTINGS_REGENERATION_N_TREE
	, SETTINGS_REGENERATION_AGE
	, SETTINGS_REGENERATION_AVDBH
	, SETTINGS_REGENERATION_LAI
	, SETTINGS_REGENERATION_HEIGHT
	, SETTINGS_REGENERATION_WS
	, SETTINGS_REGENERATION_WCR
	, SETTINGS_REGENERATION_WFR
	, SETTINGS_REGENERATION_WL
	, SETTINGS_REGENERATION_WBB
};

extern const char sz_err_out_of_memory[];

//
// ALESSIOR: fixme
// import based on pointer sucks !
// change it!
//
settings_t* settings_import(const char *const filename) {
#define BUFFER_SIZE	256
	char buffer[BUFFER_SIZE];
	int i;
	int err;
	settings_t* s;
	FILE *f;
	double *p_field;

	f = fopen(filename, "r");
	if ( ! f ) {
		printf("unable to open %s\n", filename);
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

	/* screen output is enabled by default */
	s->screen_output = 1;

	p_field = &s->sizeCell;

	i = 0;
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		char *p;
		char *token;

		/* skip empty lines and comments */
		if (	('\r' == buffer[0])
				|| ('\n' == buffer[0])
				|| ('/' == buffer[0]) ) {
			continue;
		}

		token = string_tokenizer(buffer, " \"", &p);
		token = string_tokenizer(NULL, "\"", &p);

		switch ( i++ ) {
			case SETTINGS_VERSION:
				s->version = *token;
			break;

			case SETTINGS_SPATIAL:
				s->spatial = *token;
			break;

			case SETTINGS_TIME:
				s->time = *token;
			break;

			case SETTINGS_SCREEN_OUTPUT:
				if ( ! string_compare_i(token, "off") ) {
					s->screen_output = 0;
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

			case SETTINGS_CO2_FIXED:
				if ( ! string_compare_i(token, "on") ) {
					s->CO2_fixed = CO2_FIXED_ON;
				} else if ( ! string_compare_i(token, "var") ) {
					s->CO2_fixed = CO2_FIXED_VAR;
				}
			break;

			case SETTINGS_NDEP_FIXED:
				if ( ! string_compare_i(token, "on") ) {
					s->Ndep_fixed = 1;
				}
			break;

			case SETTINGS_Q10_FIXED:
				if ( ! string_compare_i(token, "on") ) {
					s->Q10_fixed = 1;
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

			case SETTINGS_DNDC:
				if ( ! string_compare_i(token, "on") ) {
					s->dndc = 1;
				}
			break;

			case SETTINGS_SIZECELL:
				*p_field = convert_string_to_float(token, &err);
				if ( err ) {
					printf("unable to convert sizeCell: %s\n", p);
					free(s);
					fclose(f);
					return 0;
				}
				// ALESSIOC fill me
				*p_field *= *p_field;
				p_field++;
				break;

			case SETTINGS_REPLANTED_SPECIES:
				strncpy(s->replanted_species, (const char*)token, SETTINGS_REPLANTED_SPECIES_MAX_SIZE-1);
			break;

			// ALESSIOR todo fix, should use e_management from matrix.h
			case SETTINGS_REPLANTED_MANAGEMENT:
				if ( ('T' == token[0]) || ('t' == token[0]) ) {
					s->replanted_management = 0;
				} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
					s->replanted_management = 1;
				} else {
					printf("bad management habitus specified in settings: %s\n", token);
					free(s);
					fclose(f);
					return 0;
				}
			break;

			case SETTINGS_REGENERATION_SPECIES:
				strncpy(s->regeneration_species, (const char*)token, SETTINGS_REGENERATION_SPECIES_MAX_SIZE-1);
			break;

			// ALESSIOR todo fix, should use e_management from matrix.h
			case SETTINGS_REGENERATION_MANAGEMENT:
				if ( ('T' == token[0]) || ('t' == token[0]) ) {
					s->regeneration_management = 0;
				} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
					s->regeneration_management = 1;
				} else {
					printf("bad regeneration habitus specified in settings: %s\n", token);
					free(s);
					fclose(f);
					return 0;
				}
			break;

			default:
				*p_field = convert_string_to_float(token, &err);
				if ( err ) {
					printf("unable to convert value: %s\n", p);
					free(s);
					fclose(f);
					return 0;
				}
				p_field++;
		}					
	}

	fclose(f);
	return s;
#undef BUFFER_SIZE
}
