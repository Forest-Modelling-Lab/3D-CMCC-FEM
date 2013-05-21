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
static int scan_path(const char *const path) {
#if defined (_WIN32)
	handle = FindFirstFile(path, &wfd);
	if ( INVALID_HANDLE_VALUE == handle ) {
		return 0;
	}
#elif defined (_linux) || defined (__linux__)
	dir = opendir(path);
	if ( !dir ) {
		return 0;
	}
#endif
	/*ok */
	return 1;
}

/* */
static int get_files_from_path(const char *const path, FILES **files, int *const count, const int grouped) {
	int i;
	FILES *files_no_leak;
	LIST *list_no_leak;
#if defined (_WIN32)
	do {
		if ( !IS_FLAG_SET(wfd.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY) ) {
			/* check length */
			i = strlen(wfd.cFileName);
			if ( i > FILENAME_SIZE ) {
				printf(err_filename_too_big, wfd.cFileName);
				free_files(*files, *count);
				return 0;
			}

			if ( !grouped ) {
				/* alloc memory */
				files_no_leak = realloc(*files, (++*count)*sizeof*files_no_leak);
				if ( !files_no_leak ) {
					puts(err_out_of_memory);
					free_files(*files, *count-1);
					return 0;
				}

				/* assign pointer */
				*files = files_no_leak;
				(*files)[*count-1].list = NULL;
				(*files)[*count-1].count = 1;
			}

			/* allocate memory for list */
			list_no_leak = realloc((*files)[*count-1].list, (grouped ? ++(*files)[*count-1].count : 1) * sizeof*list_no_leak);
			if ( !list_no_leak ) {
				puts(err_out_of_memory);
				free_files(*files, *count);
				return 0;
			}
			(*files)[*count-1].list = list_no_leak;

			/* assign evalues */
			strncpy((*files)[*count-1].list[(*files)[*count-1].count-1].name, wfd.cFileName, i);
			(*files)[*count-1].list[(*files)[*count-1].count-1].name[i] = '\0';

			strcpy((*files)[*count-1].list[(*files)[*count-1].count-1].path, path);

			strcpy((*files)[*count-1].list[(*files)[*count-1].count-1].fullpath, path);
			if ( !mystrcat((*files)[*count-1].list[(*files)[*count-1].count-1].fullpath, wfd.cFileName, PATH_SIZE) ) {
				printf(err_filename_too_big, wfd.cFileName);
				free_files(*files, *count);
				return 0;
			}
		}
	} while ( FindNextFile(handle, &wfd) );

	/* close handle */
	FindClose(handle);
#elif defined (_linux) || defined (__linux__)
	for ( ; ; ) {
		dit = readdir(dir);
		if ( !dit ) {
			return 0;
		}

		if ( dit->d_type == DT_REG ) {
			/* check length */
			i = strlen(dit->d_name);
			if ( i >= FILENAME_SIZE ) {
				printf(err_filename_too_big, dit->d_name);
				free_files(*files, *count);
				return 0;
			}

			if ( !grouped ) {
				/* alloc memory */
				files_no_leak = realloc(*files, (++*count)*sizeof*files_no_leak);
				if ( !files_no_leak ) {
					puts(err_out_of_memory);
					free_files(*files, *count-1);
					return 0;
				}

				/* assign pointer */
				*files = files_no_leak;
				(*files)[*count-1].list = NULL;
				(*files)[*count-1].count = 1;
			}

			/* allocate memory for list */
			list_no_leak = realloc((*files)[*count-1].list, (grouped ? ++(*files)[*count-1].count : 1) * sizeof*list_no_leak);
			if ( !list_no_leak ) {
				puts(err_out_of_memory);
				free_files(*files, *count);
				return 0;
			}
			(*files)[*count-1].list = list_no_leak;

			/* assign evalues */
			strncpy((*files)[*count-1].list[(*files)[*count-1].count-1].name, dit->d_name, i);
			(*files)[*count-1].list[(*files)[*count-1].count-1].name[i] = '\0';

			strcpy((*files)[*count-1].list[(*files)[*count-1].count-1].path, path);

			strcpy((*files)[*count-1].list[(*files)[*count-1].count-1].fullpath, path);
			if ( !mystrcat((*files)[*count-1].list[(*files)[*count-1].count-1].fullpath, dit->d_name, PATH_SIZE) ) {
				printf(err_filename_too_big, dit->d_name);
				free_files(*files, *count);
				return 0;
			}
		}
	}
	/* close handle */
	closedir(dir);
#endif

	/* ok */
	return 1;
}

/*
   free_files
 */
void free_files(FILES *files, const int count)
{
	if ( files )
	{
		int i;
		for ( i = 0 ; i < count; i++ )
		{
			free(files[i].list);
		}
		free(files);
	}
}

/*
CHECK: on ubuntu fopen erroneously open a path (maybe a bug on NTFS partition driver ?)
 */
FILES *get_files(const char *const program_path, char *string, int *const count, int *const error)
{
	int i,
	y,
	plusses_count;

	char *token_by_comma,
	*token_by_plus,
	*p,
	*p2,
	*p3;

	//	token_by_comma = malloc(sizeof(char)*1024);
	//	token_by_plus = malloc(sizeof(char)*1024);


	FILE *f;

	FILES *files,
	*files_no_leak;

	LIST *list_no_leak;

	/* check parameters */
	assert(string && count && error);

	/* reset */
	files	=	NULL;
	*count	=	0;
	*error	=	0;

	/* loop for each commas */
	for ( token_by_comma = mystrtok(string, comma_delimiter, &p); token_by_comma; token_by_comma = mystrtok(NULL, comma_delimiter, &p) )
	{
		/* get token length */
		i = strlen(token_by_comma);

		/* if length is 0 skip to next token */
		if ( !i )
			continue;

		/* scan for plusses */
		plusses_count = 0;
		for ( y=0; y<i; y++ )
		{
			if ( plus_delimiter[0] == token_by_comma[y] )
			{
				/* check if next char is a plus too */
				if ( y < i-1 )
				{
					if ( plus_delimiter[0] == token_by_comma[y+1] )
					{
						++y;
						continue;
					}
				}

				/* plus found! */
				++plusses_count;
			}
		}

		/* no grouping */
		if ( !plusses_count ) // if plusses_count == 0
		{
			/* token is a path ? */
			if ( token_by_comma[i-1] == FOLDER_DELIMITER )
			{
				/* add length of filter */
				i += strlen(filter);

				/* add null terminating char */
				++i;

				/* alloc memory */
				p2 = malloc(i*sizeof*p2);
				if ( !p2 )
				{
					puts(err_out_of_memory);
					*error = 1;
					free_files(files, *count);
					return NULL;
				}

				/* copy token */
				strcpy(p2, token_by_comma);

#if defined (WIN32)
				/* add filter at end */
				strcat(p2, filter);
#endif

				/* scan path */
				if ( !scan_path(p2) )
				{
					printf(err_unable_open_path, p2);
					*error = 1;
					free(p2);
					free_files(files, *count);
					return NULL;
				}

				/* get files */
				if ( !get_files_from_path(token_by_comma, &files, count, 0) )
				{
					printf(err_unable_open_path, token_by_comma);
					*error = 1;
					free(p2);
					free_files(files, *count);
					return NULL;
				}

				/* free memory */
				free(p2);
			}
			else // if token is not a path
			{
				/* check if we can simply token_by_comma */
				f = fopen(token_by_comma, "r");
				if ( !f )
				{
					printf(err_unable_open_file, token_by_comma);
					*error = 1;
					free_files(files, *count);
					return NULL;
				}

				/* close file */
				fclose(f);

				/* check length */
				if ( i >= PATH_SIZE )
				{
					printf(err_path_too_big, token_by_comma);
					*error = 1;
					free_files(files, *count);
					return NULL;
				}

				/* allocate memory */
				files_no_leak = realloc(files, (++*count)*sizeof(FILES));
				if ( !files_no_leak )
				{
					puts(err_out_of_memory);
					free_files(files, *count-1);
					*error = 1;
					return NULL;
				}

				/* assign memory */
				files = files_no_leak;

				/* allocate memory for 1 file */
				files[*count-1].count = 1;
				files[*count-1].list = malloc(sizeof(LIST));
				if ( !files[*count-1].list )
				{
					puts(err_out_of_memory);
					*error = 1;
					free_files(files, *count);
					return NULL;
				}

				/* check if token has a FOLDER_DELIMITER */
				p2 = strrchr(token_by_comma, FOLDER_DELIMITER);
				if ( p2 )
				{
					/* skip FOLDER_DELIMITER */
					++p2;
					/* get length */
					y = strlen(p2);

					/* check filename length */
					if ( y > FILENAME_SIZE )
					{
						printf(err_filename_too_big, p2);
						*error = 1;
						free_files(files, *count);
						return NULL;
					}

					/* assign values */
					strncpy(files[*count-1].list->name, p2, y);
					files[*count-1].list->name[y] = '\0';

					strcpy(files[*count-1].list->fullpath, token_by_comma);
					*p2 = '\0';

					strcpy(files[*count-1].list->path, token_by_comma);

				}
				else
				{
					/* check length */
					if ( i > FILENAME_SIZE )
					{
						printf(err_filename_too_big, token_by_comma);
						*error = 1;
						free_files(files, *count);
						return NULL;
					}

					/* assign values */
					strcpy(files[*count-1].list->name, token_by_comma);
					if ( program_path )
					{
						strcpy(files[*count-1].list->path, program_path);
						strcpy(files[*count-1].list->fullpath, program_path);
						if ( !mystrcat(files[*count-1].list->fullpath, token_by_comma, PATH_SIZE) )
						{
							printf(err_filename_too_big, token_by_comma);
							free_files(files, *count);
							return 0;
						}
					}
					else
					{
						strcpy(files[*count-1].list->path, token_by_comma);
						strcpy(files[*count-1].list->fullpath, token_by_comma);
					}
				}
			}
		}
		else // if plusses_count != 0
		{
			/* alloc memory */
			files_no_leak = realloc(files, (++*count)*sizeof*files_no_leak);
			if ( !files_no_leak )
			{
				puts(err_out_of_memory);
				free_files(files, *count-1);
				return 0;
			}

			/* assign pointer */
			files = files_no_leak;
			files[*count-1].list = NULL;
			files[*count-1].count = 0;

			/* loop for each plus */
			for ( token_by_plus = mystrtok(token_by_comma, plus_delimiter, &p2); token_by_plus; token_by_plus = mystrtok(NULL, plus_delimiter, &p2) )
			{
				/* get token length */
				i = strlen(token_by_plus);
				/* token is a path ? */
				if ( token_by_plus[i-1] == FOLDER_DELIMITER )
				{
					/* add length of filter */
					i += strlen(filter);

					/* add null terminating char */
					++i;

					/* alloc memory */
					p3 = malloc(i*sizeof*p3);
					if ( !p3 )
					{
						puts(err_out_of_memory);
						*error = 1;
						free_files(files, *count);
						return NULL;
					}

					/* copy token */
					strcpy(p3, token_by_plus);

					/* add filter at end */
					strcat(p3, filter);

					/* scan path */
					if ( !scan_path(p3) )
					{
						printf(err_unable_open_path, p3);
						*error = 1;
						free(p3);
						free_files(files, *count);
						return NULL;
					}

					/* get files */
					if ( !get_files_from_path(token_by_plus, &files, count, 1) )
					{
						printf(err_unable_open_path, token_by_plus);
						*error = 1;
						free(p3);
						free_files(files, *count);
						return NULL;
					}

					/* free memory */
					free(p3);
				}
				else // if token is not a path
				{
					/* check if we can simply open path */
					f = fopen(token_by_plus, "r");
					if ( !f )
					{
						printf(err_unable_open_file, token_by_plus);
						*error = 1;
						free_files(files, *count);
						return NULL;
					}

					/* close file */
					fclose(f);

					/* check length */
					if ( i >= PATH_SIZE )
					{
						printf(err_path_too_big, token_by_plus);
						*error = 1;
						free_files(files, *count);
						return NULL;
					}

					/* allocate memory */
					++files[*count-1].count;
					list_no_leak = realloc(files[*count-1].list, files[*count-1].count*sizeof*list_no_leak);
					if ( !list_no_leak )
					{
						puts(err_out_of_memory);
						*error = 1;
						free_files(files, *count);
						return NULL;
					}

					/* assign pointer */
					files[*count-1].list = list_no_leak;

					/* check if token has a FOLDER_DELIMITER */
					p3 = strrchr(token_by_comma, FOLDER_DELIMITER);
					if ( p3 )
					{
						/* skip FOLDER_DELIMITER */
						++p3;
						/* get length */
						y = strlen(p3);

						/* check filename length */
						if ( y > FILENAME_SIZE )
						{
							printf(err_filename_too_big, p3);
							*error = 1;
							free_files(files, *count);
							return NULL;
						}

						/* assign values */
						strncpy(files[*count-1].list[files[*count-1].count-1].name, p3, y);
						files[*count-1].list->name[y] = '\0';

						strcpy(files[*count-1].list[files[*count-1].count-1].fullpath, token_by_plus);
						*p3 = '\0';

						strcpy(files[*count-1].list[files[*count-1].count-1].path, token_by_plus);

					}
					else
					{
						/* check length */
						if ( i > FILENAME_SIZE )
						{
							printf(err_filename_too_big, token_by_plus);
							*error = 1;
							free_files(files, *count);
							return NULL;
						}

						/* assign values */
						strcpy(files[*count-1].list[files[*count-1].count-1].name, token_by_plus);
						if ( program_path )
						{
							strcpy(files[*count-1].list[files[*count-1].count-1].path, program_path);
							strcpy(files[*count-1].list[files[*count-1].count-1].fullpath, program_path);
							if ( !mystrcat(files[*count-1].list[files[*count-1].count-1].fullpath, token_by_plus, PATH_SIZE) )
							{
								printf(err_filename_too_big, token_by_plus);
								free_files(files, *count);
								return 0;
							}
						}
						else
						{
							strcpy(files[*count-1].list[files[*count-1].count-1].path, token_by_plus);
							strcpy(files[*count-1].list[files[*count-1].count-1].fullpath, token_by_plus);
						}
					}
				}
			}
		}
	}

	/* ok */
	return files;
}

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
int mystricmp(const char *str1, const char *str2) {
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
char *mystrdup(const char *const string) {
	char *p;

	/* check for null pointer */
	if ( !string ) {
		return NULL;
	}

	/* allocate memory */
	p = malloc(strlen(string)+1);
	if ( !p ) {
		return NULL;
	}

	/* ok ? */
	return strcpy(p, string);
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
