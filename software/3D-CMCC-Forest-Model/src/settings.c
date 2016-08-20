/* settings.c */
#include "settings.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern const char sz_err_out_of_memory[];

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
	memset(s, 0, sizeof*s);

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
			case 0:
				s->version = *token;
			break;

			case 1:
				s->spatial = *token;
			break;

			case 2:
				s->time = *token;
			break;

			case 3:
				s->symmetric_water_competition = *token;
			break;

			case 4:
				strncpy(s->spin_up, (const char*)token, 3);
			break;

			case 5:
				strncpy(s->CO2_fixed, (const char*)token, 3);
			break;

			case 6:
				strncpy(s->Ndep_fixed, (const char*)token, 3);
			break;

			case 7:
				strncpy(s->management, (const char*)token, 3);
			break;

			case 8:
				strncpy(s->Prog_Aut_Resp, (const char*)token, 3);
			break;

			case 9:
				strncpy(s->dndc, (const char*)token, 3);
			break;

			 /* sizeCell */
			case 10:
				*p_field = convert_string_to_float(token, &err);
				if ( err ) {
					printf("unable to convert sizeCell: %s\n", p);
					free(s);
					fclose(f);
					return 0;
				}
				*p_field *= *p_field;
				p_field++;
			break;

			case 20:
				strncpy(s->replanted_species, (const char*)token, SETTINGS_REPLANTED_SPECIES_MAX_SIZE-1);
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
