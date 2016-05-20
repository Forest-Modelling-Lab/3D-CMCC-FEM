/* common.h */
#ifndef COMMON_H
#define COMMON_H

/* includes */
#include <math.h>

/* constants */
#define INVALID_VALUE	-9999
#define LEAP_YEAR_ROWS	17568
#define YEAR_ROWS	17520
//#define BUFFER_SIZE	4096
#define PATH_SIZE 	8192	/* TODO : CHECK IT */
#define FILENAME_SIZE	8192	/* TODO : CHECK IT */

/* defines */
#define IS_LEAP_YEAR(x)		(((x) % 4 == 0 && (x) % 100 != 0) || (x) % 400 == 0)
#define SIZE_OF_ARRAY(a)	sizeof((a))/sizeof((a)[0])
#define IS_INVALID_VALUE(value)	((INVALID_VALUE==(value)))
#define ARE_DOUBLES_EQUAL(a,b)	(((a)==(b)))			/* TODO : CHECK IT */
#define IS_FLAG_SET(v, m)	(((v) & (m)) == (m))
#if defined (_WIN32)
#define FOLDER_DELIMITER '\\'
#ifndef bzero
#define bzero(d,n) memset((d),0,(n))
#endif
#else
#define FOLDER_DELIMITER '/'
#endif
#define PREC		double
#define STRTOD		strtod
#define SQRT		sqrt
#define FABS		fabs

#if defined (_WIN32)
/* round not defined in math.h for VC++ 2008 express
	'cause is not C99 compliant */
/* we add 0.5 so if x is > 0.5 we truncate to next integer */
#define ROUND(x)	((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#else
#define ROUND		round
#endif

/* structures */
typedef struct {
	char name[FILENAME_SIZE+1];
	char path[PATH_SIZE+1];
	char fullpath[PATH_SIZE+1];
} LIST;

/* */
typedef struct {
	LIST *list;
	int count;
} FILES;

typedef struct {
	char *name;
	int (*f)(char *, char *, void *);
	void *p;
} ARGUMENT;

/* prototypes */
PREC convert_string_to_prec(const char *const string, int *const error);
int convert_string_to_int(const char *const string, int *const error);
PREC get_standard_deviation(const PREC *const values, const int count);
PREC get_median(const PREC *const values, const int count, int *const error);
PREC get_mean(const PREC *const values, const int count);
FILES *get_files(const char *const program_path, char *string, int *const count, int *const error);
void free_files(FILES *files, const int count);
void init_random_seed(void);
int get_random_number(int max);
int parse_arguments(int argc, char *argv[], const ARGUMENT *const args, const int arg_count);
int string_compare_i(const char *str1, const char *str2);
char *mystrdup(const char *const string);
char *mystrtok(char *string, const char *delimiters, char **p);
char *get_current_directory(void);
int add_char_to_string(char *const string, char c, const int size);
int mystrcat(char *const string, const char *const string_to_add, const int size);
int path_exists(const char *const path);
int compare_prec(const void * a, const void * b);

/* */
#endif /* COMMON_H */
