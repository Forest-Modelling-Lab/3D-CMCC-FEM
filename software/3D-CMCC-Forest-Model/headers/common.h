/* common.h */
#ifndef COMMON_H
#define COMMON_H

#define PROGRAM_VERSION	"5.3.1"
#define PROGRAM_NAME "3D-CMCC-CNR Forest Ecosystem Model v."PROGRAM_VERSION""

#ifdef _WIN32
#define FOLDER_DELIMITER	"\\"
#define FOLDER_DELIMITER_C	'\\'
#else
#define FOLDER_DELIMITER	"/"
#define FOLDER_DELIMITER_C	'/'
#endif

#define INVALID_VALUE		-9999
#define IS_INVALID_VALUE(x)	((INVALID_VALUE==(x)))
#define IS_LEAP_YEAR(x)		(((x) % 4 == 0 && (x) % 100 != 0) || (x) % 400 == 0)
#define SIZE_OF_ARRAY(a)	sizeof((a))/sizeof((a)[0])

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef round
/* we add 0.5 so if x is > 0.5 we truncate to next integer */
#define ROUND(x)	((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#else
#define ROUND		round
#endif

#define XSTR(a)		STR(a)
#define STR(a)		#a
#define CHECK_CONDITION(x,c,y) {																					\
		if ( (x)c(y) )		{																						\
			logger_error(NULL,"\nerror: condition (%s %s %s) is true, value of %s is %g, value of %s is %g in %s on line %d\n"			\
							, XSTR(x)																				\
							, XSTR(c)																				\
							, XSTR(y)																				\
							, XSTR(x)																				\
							, (double)(x)																			\
							, XSTR(y)																				\
							, (double)(y)																			\
							, __FILE__																				\
							, __LINE__);																			\
			exit(1);																								\
		}																											\
}

#define convert_string_to_int(s, err) ((int)convert_string_to_float((s),(err)))
double convert_string_to_float(const char *const string, int *const error);
int string_compare_i(const char *str1, const char *str2);
char* string_copy(const char *const string);
char* string_tokenizer(char *string, const char *delimiters, char **p);
char* get_current_path(void);
int file_get_rows_count(const char* const filename);
unsigned int file_load_in_memory(const char* const filename, char** result);
int file_copy(const char* const filename, const char* const path);
const char* file_get_name_only(const char* const filename);
int path_create(const char *const path);
const char* datetime_current(void);
void timer_init(void);
double timer_get(void);

#endif /* COMMON_H */
