/* logger.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "common.h"
#include "logger.h"


logger_t* logger_new(const char* const path, ...) {
#define BUFFER_SIZE	1024
	char buffer[BUFFER_SIZE];

	va_list va;
	logger_t* p;

	va_start(va, path);
	vsnprintf(buffer, BUFFER_SIZE, path, va);
	va_end(va);

	if ( ! path_create(buffer) ) {
		return NULL;
	}

	p = malloc(sizeof*p);

	if ( ! p ) return NULL;
	p->file_output = 1;
	p->std_output = 1;

	p->f = fopen(buffer, "w");
	if ( ! p->f ) {
		free(p);
		p = NULL;
	}
	return p;
#undef BUFFER_SIZE
}

void logger(logger_t *p, const char *text, ...) {
#define LOGGER_BUFFER_SIZE	4096
	char buffer[LOGGER_BUFFER_SIZE];
	va_list va;

	va_start(va, text);
	vsnprintf(buffer, LOGGER_BUFFER_SIZE, text, va);
	va_end(va);

	if ( p && p->std_output ) {
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
