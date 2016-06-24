/* logger.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "common.h"
#include "logger.h"

/*
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
*/

logger_t* logger_new(const char* const path, ...) {
#define BUFFER_SIZE	256
	char buffer[BUFFER_SIZE];
	va_list va;
	logger_t* p;

	va_start(va, path);
	vsnprintf(buffer, BUFFER_SIZE, path, va);
	va_end(va);

	/* ALESSIOR TODO REMOVE */
#if defined (_linux) || defined (__linux__)
	{
		char *buffer2;
		char *p2;
		char buffer3[256];
		buffer2 = string_copy(buffer);
		if ( ! buffer2 ) {
			puts("out of memory!");
			return NULL;
		}
		p2 = strrchr(buffer2, '/');
		if ( p2 ) { *p2 = '\0'; }
		sprintf(buffer3, "mkdir -p %s", buffer2);
		system(buffer3);
		free(buffer2);
	}
#else
	if ( ! path_create(buffer) ) {
		printf("unable to create %s\n", path);
		return NULL;
	}
#endif

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
