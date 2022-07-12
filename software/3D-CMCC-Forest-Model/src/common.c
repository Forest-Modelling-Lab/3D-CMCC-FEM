/* common.c */
#include <stdio.h>
#include <stdint.h> //ddalmo august 2021
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
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
static double g_timer_period;
#ifndef uint64
typedef unsigned __int64 uint64;
#endif
#elif defined (linux) || defined (_linux) || defined (__linux__)
#include <unistd.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#ifndef uint64
typedef uint64_t uint64;
#endif
#endif

/* external error strings */
const char sz_err_out_of_memory[] = "out of memory";

double QuadM ( const double a, const double b, const double c, int* const err)
{
	double quadm;

	*err = 0;

	/* Solves the quadratic equation - finds SMALLER root. */

	if ( ( b * b - 4. * a * c ) < 0. )
	{
		puts("Warning:imaginary roots in quadratic");
		quadm = 0.;
		*err  = 1;
	}
	else
	{
		if ( 0. == a )
		{
			if ( 0. ==  b )
			{
				quadm = 0.;

				if ( 0. != c )
				{
					puts("ERROR: CANT SOLVE QUADRATIC");
					exit (1);
				}
			}
			else
			{
				quadm = -c / b;
			}
		}
		else
		{
			quadm = ( -b - sqrt ( b * b - 4. * a * c ) ) / ( 2. * a );

		}
	}
	return quadm;
}

double QuadP ( const double a, const double b, const double c, int* const err )
{
	double quadp;

	*err = 0;

	/* Solves the quadratic equation - finds LARGER root. */

	if ( ( b * b - 4. * a * c ) < 0. )
	{
		puts("Warning:imaginary roots in quadratic");
		quadp = 0.;
		*err  = 1;
	}
	else
	{
		if ( 0. == a )
		{
			if ( 0. == b )
			{
				quadp = 0.;

				if ( 0. != c )
				{
					puts("ERROR: CANT SOLVE QUADRATIC");
					exit (1);
				}
			}
			else
			{
				quadp = -c / b;
			}
		}
		else
		{
			quadp = ( - b + sqrt ( b * b - 4. * a * c ) ) / ( 2. * a );
		}
	}
	return quadp;
}

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

char* strstr_i(char* str1, const char* str2) {
    char* p1 = str1 ;
    const char* p2 = str2 ;
    char* r = *p2 == 0 ? str1 : 0 ;

    while( *p1 != 0 && *p2 != 0 ) {
        if( tolower(*p1) == tolower(*p2) ) {
            if ( ! r ){
                r = p1;
            }
            p2++;
        } else {
            p2 = str2;
            if ( tolower(*p1) == tolower(*p2) ) {
                r = p1;
                p2++;
            } else {
                r = 0;
            }
        }
        p1++;
    }
    return *p2 == 0 ? r : 0;
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

int file_exists(const char *const file) {
#if defined (_WIN32)
	DWORD dwResult;
#endif
	if ( ! file ) {
		return 0;
	}
#if defined (_WIN32)
	dwResult = GetFileAttributes(file);
	if (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY)) {
		return 1;
	}
#elif defined (linux) || defined (_linux) || defined (__linux__)
	if ( ! access(file, W_OK) ) {
		return 1;
	}
#else
	assert(1);
#endif
	return 0;
}

// returns -1 if file not found
// returns -2 if out of memory
// returns -3 if read error
int file_load_in_memory(const char* const filename, char** result) {
	unsigned int size;
	FILE *f;

	assert(filename && result);

	*result = NULL;

	size = 0;
	f = fopen(filename, "rb");
	if ( !f )  {
		return -1;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = malloc((size+1)*sizeof**result);
	if ( !*result ) {
		fclose(f);
		return -2;
	}
	if ( size != fread(*result, sizeof(char), size, f) )  {
		free(*result);
		fclose(f);
		return -3;
	}
	fclose(f);
	(*result)[size] = '\0';

	return size;
}

// returns -1 if file not found
// returns -2 if out of memory
// returns -3 if read error
int file_get_rows_count(const char* const filename) {
	int i;
	int size;
	int rows_count;
	char *p;

	assert(filename);

	size = file_load_in_memory(filename, &p);
	if ( size <= 0 ) return size;

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


int file_copy(const char* const filename, const char* const path) {
	char *p;
	char *p2;
	int i;
	char buffer[256];

	assert(filename && path);

	p = strrchr(filename, '\\');
	p2 = strrchr(filename, '/');
	if ( ! p || (p2 && (p2 < p)) ) {
		p = p2;
	}
	//if ( ! p ) p = p2;
	if ( p ) ++p;

	i = strlen(path);
	if ( ('/' == path[i-1]) || ('\\' == path[i-1]) ) {
		i = 1;
	} else {
		i = 0;
	}

	sprintf(buffer, "%s%s%s", path
							, i ? "" : FOLDER_DELIMITER
							, p
	);

	if ( ! path_create(buffer) ) {
		return 0;
	}

#ifdef _WIN32
	return (int)CopyFile(filename, buffer, FALSE);
#else
	{
		int ret;
		FILE* s;
		FILE* d;
		
		ret = 0;
		s = NULL;
		d = NULL;
		
		s = fopen(filename, "r");
		if ( ! s ) goto quit;
		
		d = fopen(buffer, "w");
		if ( ! d ) goto quit;
		
		while ( 1 ) {
			int c = fgetc(s);
			if ( EOF == c ) break;
			fputc(c, d);
		}
		ret = 1;
		
	quit:
		if ( d ) fclose(d);
		if ( s ) fclose(s);
		return ret;
	}
#endif
}

int path_create(const char *const path) {
#ifdef _WIN32
	char folder[MAX_PATH] = { 0 };
	char* p;
	char* p2;

	assert(path);

	p = strchr(path, '\\');
	p2 = strchr(path, '/');
	if ( ! p || (p2 && (p2 < p)) ) {
		p = p2;
	}
	//if ( ! p ) p = p2;

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
		if ( ! p || (p2 && (p2 < p)) ) {
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

const char* datetime_current(void) {
	static char buf[22+1];
	time_t t;
	struct tm tm;

	time(&t);
	tm = *localtime(&t);
	sprintf(buf, "%02d/%02d/%04d at %02d:%02d:%02d"	, tm.tm_mday
													, tm.tm_mon + 1
													, tm.tm_year + 1900
													, tm.tm_hour
													, tm.tm_min
													, tm.tm_sec
	);
		
	return buf;
}

#if defined (linux) || defined (_linux) || defined (__linux__)
static inline double getTimeOfDay()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (double) t.tv_sec + (double) t.tv_usec / 1000000.0;
}
#endif

#if defined (_WIN32)
static double get_timer_period(void) {
	LARGE_INTEGER temp;
	if ( QueryPerformanceFrequency(&temp) != 0 && temp.QuadPart != 0 ) {
		return 1.0 / (double) temp.QuadPart;
	}
	return 0;
}
#endif

void timer_init(void) {
#if defined (_WIN32)
	g_timer_period = get_timer_period();
#endif
}

double timer_get(void)
{
#if defined (linux) || defined (_linux) || defined (__linux__)
	double mt;
	// Check for POSIX timers and monotonic clocks. If not supported, use the gettimeofday fallback.
#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK) \
&& (defined(CLOCK_MONOTONIC_RAW) || defined(CLOCK_MONOTONIC))
	struct timespec t;
#ifdef CLOCK_MONOTONIC_RAW
	clockid_t clk_id = CLOCK_MONOTONIC_RAW;
#else
	clockid_t clk_id = CLOCK_MONOTONIC;
#endif
	if (clock_gettime(clk_id, &t) == 0)
		mt = (double) t.tv_sec + (double) t.tv_nsec / 1000000000.0;
	else
#endif
		mt = getTimeOfDay();
	return mt;
#elif defined _WIN32
	LARGE_INTEGER microTime;
	QueryPerformanceCounter(&microTime);
	return (double)microTime.QuadPart * g_timer_period;
#else
	return 0.;
#endif
}

const char* file_get_name_only(const char* const filename) {
	const char *p;
	const char *p2;

	assert(filename);

	p = strrchr(filename, '\\');
	p2 = strrchr(filename, '/');
	if ( ! p || (p2 && (p2 > p)) ) {
		p = p2;
	}
	if ( p ) ++p;
	if ( ! p ) p = filename;

	return p;
}

int has_path_delimiter(const char* const s) {
	int i;

	assert(s);

	i = strlen(s);
	return (('/' == s[i-1]) || ('\\' == s[i-1]));
}

int istab(const int c) {
	return ('\t' == c);
}

int is_nan(double x)
{
    union { uint64_t u; double f; } ieee754;  //changed from uint64 to uint64_t to have the .exe on windows
    ieee754.f = x;
    return ( (unsigned)(ieee754.u >> 32) & 0x7fffffff ) +
           ( (unsigned)ieee754.u != 0 ) > 0x7ff00000;
}

int is_inf(double x)
{
    union { uint64_t u; double f; } ieee754; //changed from uint64 to uint64_t
    ieee754.f = x;
    return ( (unsigned)(ieee754.u >> 32) & 0x7fffffff ) == 0x7ff00000 &&
           ( (unsigned)ieee754.u == 0 );
}

char* concatenate_path(char* s1, char* s2)
{
	char *p;
	int i;
	int ii;
	int flag;

	assert(s1 && s2);

	i = strlen(s1);
	flag = ! (('\\' == s1[i-1]) || ('/' == s1[i-1]));
	ii = strlen(s2);

	i += flag + ii + 1;

	p = malloc(i*sizeof*p);
	if ( p ) {
		sprintf(p, "%s%s%s", s1
				, flag ? FOLDER_DELIMITER : ""
						, s2);
	}
	return p;
}
