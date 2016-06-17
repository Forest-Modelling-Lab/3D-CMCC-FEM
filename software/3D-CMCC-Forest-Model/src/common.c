/* common.c */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include "common.h"

/* os dependant */
#if defined (_WIN32)
#ifndef STRICT
#define STRICT
#endif /* STRICT */
#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif /* WIN32_MEAN_AND_LEAN */
#include <windows.h>
#pragma comment(lib, "kernel32.lib")
static WIN32_FIND_DATA wfd;
static HANDLE handle;
#elif defined (_linux) || defined (__linux__)
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
static DIR *dir;
static struct dirent *dit;
#endif

/* strings */
static const char filter[] = "*.*";
static const char comma_delimiter[] = ",";
static const char plus_delimiter[] = "+";

/* error strings */
static const char err_unable_open_path[] = "unable to open path: %s\n\n";
static const char err_unable_open_file[] = "unable to open file: %s\n\n";
static const char err_path_too_big[] = "specified path \"%s\" is too big.\n\n";
static const char err_filename_too_big[] = "filename \"%s\" is too big.\n\n";
static const char err_unknown_argument[] = "unknown argument: \"%s\"\n\n";

/* external error strings */
const char err_out_of_memory[] = "out of memory";

/* */
PREC convert_string_to_prec(const char *const string, int *const error) {
	PREC value = 0.0;
	char *p = NULL;

	/* reset */
	*error = 0;

	if ( !string ) {
		*error = 1;
		return 0.0;
	}

	errno = 0;
	value = STRTOD(string, &p);
	STRTOD(p, NULL);
	if ( string == p || *p || errno ) {
		*error = 1;
	}

	return value;
}

/* */
void init_random_seed(void) {
	srand((unsigned int)time(NULL));
}

/* */
int get_random_number(int max) {
	/* taken from http://c-faq.com/lib/randrange.html */
	return (rand() / (RAND_MAX / max + 1));
}

/* */
static int check_for_argument(const char *const string , const char *const pattern, char **param)
{
	char *pptr	=	NULL,
			*sptr	=	NULL,
			*start	=	NULL;

	/* reset */
	*param = NULL;

	for ( start = (char *)string; *start; start++ )
	{
		/* find start of pattern in string */
		for ( ; (*start && (toupper(*start) != toupper(*pattern))); start++)
			;
		if (start != string+1 )
			return 0;

		pptr = (char *)pattern;
		sptr = (char *)start;

		while (toupper(*sptr) == toupper(*pptr))
		{
			sptr++;
			pptr++;

			/* if end of pattern then pattern was found */
			if ( !*pptr )
			{
				if ( !*sptr )
				{
					return 1;
				}
				else
				{
					/* check for next char to be an '=' */
					if ( *sptr == '=' )
					{
						if ( *(++sptr) )
						{
							*param = sptr;
						}
						return 1;
					}
				}

				return 0;
			}
		}
	}

	return 0;
}

int parse_arguments(int argc, char *argv[], const ARGUMENT *const args, const int arg_count)
{
	int i,
	ok;

	char *param;

	while ( argc > 1 )
	{
		/* check for arguments */
		if ( ( '-' != argv[1][0]) && ( '/' != argv[1][0]) )
		{
			printf(err_unknown_argument, argv[1]);
			return 0;
		}

		ok = 0;
		for ( i=0; i<arg_count; i++ )
		{
			if ( check_for_argument(argv[1], args[i].name, &param) )
			{
				ok = 1;

				/* check if function is present */
				assert(args[i].f);

				/* call function */
				if ( !args[i].f(args[i].name, param, args[i].p) )
				{
					return 0;
				}

				break;
			}
		}

		if ( !ok )
		{
			printf(err_unknown_argument, argv[1]+1);
			return 0;
		}

		++argv;
		--argc;
	}

	// ok
	return 1;
}

/* */
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

/* */
char *string_copy(const char *const s) {
	if ( s ) {
		char *p = malloc(strlen(s)+1);
		if ( p ) {
			return strcpy(p, s);
		}
	}
	return NULL;
}

/* stolen to wikipedia */
char *mystrtok(char *string, const char *delimiters, char **p) {
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

/* */
int convert_string_to_int(const char *const string, int *const error) {
	int value = 0;
	char *p = NULL;

	/* reset */
	*error = 0;

	if ( !string ) {
		*error = 1;
		return 0;
	}

	errno = 0;
	value = (int)strtod(string, &p);
	strtod(p, NULL);
	if ( string == p || *p || errno ) {
		*error = 1;
	}

	return value;
}

/* */
char *get_current_directory(void) {
	char *p;
#if defined (_WIN32)
	p = malloc((MAX_PATH+1)*sizeof *p);
	if ( !p ) {
		return NULL;
	}
	if ( !GetModuleFileName(NULL, p, MAX_PATH) ) {
		free(p);
		return NULL;
	}
	p[(strrchr(p, '\\')-p)+1] = '\0';
	return p;
#elif defined (linux)
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
	if ( p[len-1] != FOLDER_DELIMITER ) {
		if ( !add_char_to_string(p, FOLDER_DELIMITER, MAXPATHLEN) ) {
			free(p);
			return NULL;
		}
	}
	return p;
#else
	return NULL;
#endif
}

/* */
int add_char_to_string(char *const string, char c, const int size) {
	int i;

	/* check for null pointer */
	if ( !string ) {
		return 0;
	}

	/* compute length */
	for ( i = 0; string[i]; i++ );

	/* check length */
	if ( i >= size-1 ) {
		return 0;
	}

	/* add char */
	string[i] = c;
	string[i+1] = '\0';

	/* */
	return 1;
}

/* */
int mystrcat(char *const string, const char *const string_to_add, const int size) {
	int i;
	int y;

	/* check for null pointer */
	if ( !string || !string_to_add ) {
		return 0;
	}

	/* compute lenghts */
	for ( i = 0; string[i] != '\0'; i++ );
	for ( y = 0; string_to_add[y] != '\0'; y++ );

	/* check length */
	if ( i >= size-y-1 ) {
		return 0;
	}

	strcat(string, string_to_add);

	/* */
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
#elif defined (linux)
	if ( !access(path, W_OK) ) {
		return 1;
	}
#endif
	return 0;
}

/* */
PREC get_mean(const PREC *const values, const int count) {
	int i;
	PREC mean;

	/* check for null pointer */
	assert(values && count);

	/* compute mean */
	mean = 0.0;
	for ( i = 0; i < count; i++ ) {
		if ( IS_INVALID_VALUE(values[i]) ) {
			return INVALID_VALUE;
		}
		mean += values[i];
	}
	mean /= count;

	/* check for NAN */
	if ( mean != mean ) {
		mean = INVALID_VALUE;
	}

	/* ok */
	return mean;
}

/* */
PREC get_standard_deviation(const PREC *const values, const int count) {
	int i;
	PREC mean;
	PREC sum;
	PREC sum2;

	/* check for null pointer */
	assert(values && count);

	/* get mean */
	mean = get_mean(values, count);
	if ( IS_INVALID_VALUE(mean) ) {
		return INVALID_VALUE;
	}

	/* compute stddev */
	sum = 0.0;
	sum2 = 0.0;
	for ( i = 0; i < count; i++ ) {
		sum = (values[i] - mean);
		sum *= sum;
		sum2 += sum;
	}
	sum2 /= count-1;
	sum2 = SQRT(sum2);

	/* check for NAN */
	if ( sum2 != sum2 ) {
		sum2 = INVALID_VALUE;
	}

	/* ok */
	return sum2;
}

/* */
PREC get_median(const PREC *const values, const int count, int *const error) {
	int i;
	int median_count;
	PREC *p_median;
	PREC *median_no_leak;
	PREC result;

	/* check for null pointer */
	assert(values);

	/* reset */
	*error = 0;

	/* get valid values */
	p_median = NULL;
	median_count = 0;
	for ( i = 0; i < count; i++ ) {
		if ( !IS_INVALID_VALUE(values[i]) ) {
			median_no_leak = realloc(p_median, (++median_count) * sizeof *median_no_leak);
			if ( !median_no_leak ) {
				*error = 1;
				free(p_median);
				return 0;
			}

			p_median = median_no_leak;
			p_median[median_count-1] = values[i];
		}
	}

	if ( !median_count ) {
		return INVALID_VALUE;
	} else if ( 1 == median_count ) {
		result = p_median[0];
		free(p_median);
		return result;
	}

	/* sort values */
	qsort(p_median, median_count, sizeof *p_median, compare_prec);

	/* get median */
	if ( median_count & 1 ) {
		result = p_median[((median_count+1)/2)-1];
	} else {
		result = ((p_median[(median_count/2)-1] + p_median[median_count/2]) / 2);
	}

	/* free memory */
	free(p_median);

	/* check for NAN */
	if ( result != result ) {
		result = INVALID_VALUE;
	}

	/* */
	return result;
}

/* todo : implement a better comparison for equality */
int compare_prec(const void * a, const void * b) {
	if ( *(PREC *)a < *(PREC *)b ) {
		return -1;
	} else if ( *(PREC *)a > *(PREC *)b ) {
		return 1;
	} else {
		return 0;
	}
}
