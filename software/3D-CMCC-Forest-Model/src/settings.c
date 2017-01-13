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
	, SETTINGS_SCREEN_OUTPUT
	, SETTINGS_DEBUG_OUTPUT
	, SETTINGS_DAILY_OUTPUT
	, SETTINGS_MONTHLY_OUTPUT
	, SETTINGS_YEARLY_OUTPUT
	, SETTINGS_YEAR_START
	, SETTINGS_YEAR_END
	, SETTINGS_SOIL_OUTPUT
	, SETTINGS_CO2_MOD
	, SETTINGS_CO2_TRANS
	, SETTINGS_YEAR_START_CO2_FIXED
	, SETTINGS_NDEP_FIXED
	, SETTINGS_Q10_FIXED
	, SETTINGS_REGENERATION
	, SETTINGS_MANAGEMENT
	, SETTINGS_YEAR_START_MANAGEMENT
	, SETTINGS_PROGN_AUT_RESP
	, SETTINGS_DNDC
	, SETTINGS_SIZECELL
	, SETTINGS_Y
	, SETTINGS_CO2CONC
	, SETTINGS_CO2_INCR
	, SETTINGS_INIT_FRAC_MAXASW
	, SETTINGS_TREE_LAYER_LIMIT
	, SETTINGS_SOIL_LAYER
	, SETTINGS_MAX_LAYER_COVER
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

	, SETTINGS_COUNT
};

extern const char sz_err_out_of_memory[];

settings_t* settings_import(const char *const filename) {
#define BUFFER_SIZE	256
	char buffer[BUFFER_SIZE];
	int i;
	int err;
	settings_t* s;
	FILE *f;

	const char delimiter[] = " /\"\t\r\n";

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

	i = 0;
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

		/* get value */
		token = string_tokenizer(NULL, delimiter, &p);

		if ( ! token ) continue;

		switch ( i ) {
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
					printf("bad management habitus specified in settings: %s\n", token);
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
					printf("bad regeneration habitus specified in settings: %s\n", token);
					free(s);
					fclose(f);
					return 0;
				}
			break;

			default:
				value = convert_string_to_float(token, &err);
				if ( err ) {
					printf("unable to convert value: %s\n", token);
					free(s);
					fclose(f);
					return 0;
				}
				switch ( i ) {
					case SETTINGS_YEAR_START:
						s->year_start = (int)value;
					break;

					case SETTINGS_YEAR_END:
						s->year_end = (int)value;
					break;

					case SETTINGS_YEAR_START_CO2_FIXED:
						s->year_start_co2_fixed = (int)value;
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

					case SETTINGS_REPLANTED_WS:
						s->replanted_ws = value;
					break;

					case SETTINGS_REPLANTED_WCR:
						s->replanted_wcr = value;
					break;

					case SETTINGS_REPLANTED_WFR:
						s->replanted_wfr = value;
					break;

					case SETTINGS_REPLANTED_WL:
						s->replanted_wl = value;
					break;

					case SETTINGS_REPLANTED_WBB:
						s->replanted_wbb = value;
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

					case SETTINGS_REGENERATION_WS:
						s->regeneration_ws = value;
					break;

					case SETTINGS_REGENERATION_WCR:
						s->regeneration_wcr = value;
					break;

					case SETTINGS_REGENERATION_WFR:
						s->regeneration_wfr = value;
					break;

					case SETTINGS_REGENERATION_WL:
						s->regeneration_wl = value;
					break;

					case SETTINGS_REGENERATION_WBB:
						s->regeneration_wbb = value;
					break;

					default:
						puts("too many values!");
						free(s);
						fclose(f);
						return 0;
				}
		}
		++i;
	}

	fclose(f);
	return s;
#undef BUFFER_SIZE
}
