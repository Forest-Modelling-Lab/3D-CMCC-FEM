/* logger.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "logger.h"

logger_t* logger_new(const char* const filename) {
	logger_t* p;

	p = malloc(sizeof*p);
	if ( ! p ) return NULL;
	p->file_output = 1;
	p->std_output = 1;

	p->f = fopen(filename, "w");
	if ( ! p->f ) {
		free(p);
		p = NULL;
	}
	return p;
}

logger_t* logger_new_ex(const char* const filename, const char* const s) {
	char *buffer;
	int i;
	logger_t* p;

	p = malloc(sizeof*p);
	if ( ! p ) return NULL;
	p->file_output = 1;
	p->std_output = 1;

	i = strlen(filename);
	i += strlen(s);
	++i;

	buffer = malloc(i*sizeof*buffer);
	if ( ! buffer ) return NULL;
	sprintf(buffer, "%s%s", filename, s);

	p->f = fopen(buffer, "w");
	free(buffer);
	if ( ! p->f ) {
		free(p);
		p = NULL;
	}
	return p;
}

void logger(logger_t *p, const char *text, ...) {
#define LOGGER_BUFFER_SIZE	1024
	char buffer[LOGGER_BUFFER_SIZE];
	va_list va;

	va_start(va, text);
	vsnprintf(buffer, LOGGER_BUFFER_SIZE, text, va);
	va_end(va);

	if ( ! p || (p && p->std_output) ) {
		fputs(buffer, stdout);
	}

	if ( p && p->file_output && p->f ) {
		fputs(buffer, p->f);
	}
#undef LOGGER_BUFFER_SIZE
}

void logger_close(logger_t* p) {
	if ( p ) {
		if ( p->f ) {
			fclose(p->f);
		}
		free(p);
	}
}
