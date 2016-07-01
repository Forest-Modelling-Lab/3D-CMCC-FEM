/* common.h */
#ifndef COMMON_H
#define COMMON_H

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
#define CHECK_CONDITION(x,c) {																																\
		if ( (x)c )		{																																	\
			logger(g_log, "\nerror: condition (%s %s) is true, value of %s is %g in %s on line %d\n", XSTR(x), XSTR(c), XSTR(x), (double)(x),  __FILE__, __LINE__);	\
			exit(1);																																		\
		}																																					\
}

#define convert_string_to_int(s, err) ((int)convert_string_to_float((s),(err)))
double convert_string_to_float(const char *const string, int *const error);
int string_compare_i(const char *str1, const char *str2);
char* string_copy(const char *const string);
#define string_copy(s)	string_copy_ex((s),0)
char *string_copy_ex(const char *const s, const int n);
char* string_tokenizer(char *string, const char *delimiters, char **p);
char* get_current_directory(void);
int file_get_rows_count(const char* const filename);
unsigned int file_load_in_memory(const char* const filename, char** result);
int path_is_absolute(const char *const path);
int path_create(const char *const path);

#endif /* COMMON_H */
