/* common.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include "common.h"

/* os dependant */
#if defined _WIN32
#ifndef STRICT
#define STRICT
#endif /* STRICT */
#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif /* WIN32_MEAN_AND_LEAN */
#include <windows.h>
static WIN32_FIND_DATA wfd;
static HANDLE handle;
#elif defined (linux) || defined (_linux) || defined (__linux__)
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

/* external error strings */
const char sz_err_out_of_memory[] = "out of memory";

double convert_string_to_float(const char* const string, int* const err) {
	char *p;
	double value;
	
	*err = 0;

	if ( ! string || ! string[0] ) {
		*err = 1;
		return 0.0;
	}

	errno = 0;
	value = strtod(string, &p);
	strtod(p, NULL);
	if ( (string == p) || *p || errno ) {
		*err = 1;
	}
	return value;
}

int string_compare_i(const char *str1, const char *str2) {
	register signed char __res;

	while ( 1 ) {
		if ( (__res = toupper( *str1 ) - toupper( *str2++ )) != 0 || !*str1++ ) {
			break;
		}
	}

	/* returns an integer greater than, equal to, or less than zero */
	return __res;
}

char *string_copy(const char *const s) {
	if ( s ) {
		char *p = malloc(strlen(s)+1);
		if ( p ) {
			return strcpy(p, s);
		}
	}
	return NULL;
}

char *string_tokenizer(char* string, const char* delimiters, char **p) {
	char *sbegin;
	char *send;

	sbegin = string ? string : *p;
	sbegin += strspn(sbegin, delimiters);
	if ( *sbegin == '\0') {
		*p = "";
		return NULL;
	}

	send = sbegin + strcspn(sbegin, delimiters);
	if ( *send != '\0') {
		*send++ = '\0';
	}
	*p = send;

	return sbegin;
}

int add_char_to_string(char *const string, char c, const int size) {
 int i;

 if ( !string ) {
  return 0;
 }

 i = strlen(string);
 if ( i >= size-1 ) {
  return 0;
 }

 string[i] = c;
 string[i+1] = '\0';

 return 1;
}

char *get_current_path(void) {
	char *p;
#if defined (_WIN32)
	p = malloc((MAX_PATH+1)*sizeof *p);
	if ( ! p ) {
		return NULL;
	}
	if ( ! GetModuleFileName(NULL, p, MAX_PATH) ) {
		free(p);
		return NULL;
	}
	p[(strrchr(p, '\\')-p)+1] = '\0';
	return p;
#elif defined (linux) || defined (_linux) || defined (__linux__)
	int len;
	p = malloc((MAXPATHLEN+1)*sizeof *p);
	if ( !p ) {
		return NULL;
	}
	if ( !getcwd(p, MAXPATHLEN) ) {
		free(p);
		return NULL;
	}
	/* check if last chars is a FOLDER_DELIMITER */
	len = strlen(p);
	if ( !len ) {
		free(p);
		return NULL;
	}
	if ( p[len-1] != FOLDER_DELIMITER_C ) {
		if ( !add_char_to_string(p, FOLDER_DELIMITER_C, MAXPATHLEN) ) {
			free(p);
			return NULL;
		}
	}
	return p;
#else
	assert(1);
	return NULL;
#endif
}

int string_concatenate(char* const s, const char* const s2, const int size) {
	int len;
	int len2;

	if ( ! s || ! s2 ) {
		return 0;
	}

	len = strlen(s);
	len2 = strlen(s2);

	if ( len >= size-len2-1 ) {
		return 0;
	}
	strcat(s, s2);
	return 1;
}

/* */
int path_exists(const char *const path) {
#if defined (_WIN32)
	DWORD dwResult;
#endif
	if ( !path ) {
		return 0;
	}
#if defined (_WIN32)
	dwResult = GetFileAttributes(path);
	if (dwResult != INVALID_FILE_ATTRIBUTES && (dwResult & FILE_ATTRIBUTE_DIRECTORY)) {
		return 1;
	}
#elif defined (linux) || defined (_linux) || defined (__linux__)
	if ( ! access(path, W_OK) ) {
		return 1;
	}
#else
	assert(1);
#endif
	return 0;
}

unsigned int file_load_in_memory(const char* const filename, char** result) {
	unsigned int size;
	FILE *f;

	size = 0;
	f = fopen(filename, "rb");
	if ( !f )  {
		*result = NULL;
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = malloc((size+1)*sizeof**result);
	if ( !*result ) {
		fclose(f);
		return 0;
	}
	if ( size != fread(*result, sizeof(char), size, f) )  {
		free(*result);
		fclose(f);
		return 0;
	}
	fclose(f);
	(*result)[size] = '\0';

	return size;
}

int file_get_rows_count(const char* const filename) {
	unsigned int i;
	unsigned int size;
	int rows_count;
	char *p;

	if ( !filename || ('\0' == filename[0]) ) {
		return -1;
	}

	size = file_load_in_memory(filename, &p);
	if ( ! size ) return -1;

	//
	rows_count = 0;
	for ( i = 0; i < size; i++ ) {
		if ( '\n' == p[i] ) {
			++rows_count;
		}
	}
	if ( p[size-1] != '\n' ) {
		++rows_count;
	}
	free(p);
	return rows_count;
}

int path_create(const char *const path) {
#ifdef _WIN32
	char folder[MAX_PATH] = { 0 };
	char* p;
	char* p2;

	assert(path);

	p = strchr(path, '\\');
	p2 = strchr(path, '/');
	if ( ! p || (p > p2) ) {
		p = p2;
	}
	if ( ! p ) p = p2;

	while ( p ) {
		char *p3;

		strncpy(folder, path, p - path + 1);
		if ( ! CreateDirectory(folder, NULL) ) {
			DWORD err = GetLastError();
			if ( err != ERROR_ALREADY_EXISTS ) {
				return 0;
			}
		}
		++p;
		p3 = strchr(p, '\\');
		p2 = strchr(p, '/');
		p = p3;
		if ( ! p || (p > p2) ) {
			p = p2;
		}
	}
#elif defined (linux) || defined (_linux) || defined (__linux__)
	char* p;
	char* buffer;

	assert(path);

	buffer = string_copy(path);
	if ( ! buffer ) {
		return 0;
	}

	for ( p = strchr(buffer+1, FOLDER_DELIMITER_C); p; p = strchr(p+1, FOLDER_DELIMITER_C) ) {
		*p = '\0';
		if ( -1 == mkdir(buffer, S_IRWXU) ) {
			if ( errno != EEXIST) {
				*p = '/';
				free(buffer);
				return 0;
			}
		}
		*p = '/';
	}
	free(buffer);
#else
	assert(1);
#endif
	return 1;
}
