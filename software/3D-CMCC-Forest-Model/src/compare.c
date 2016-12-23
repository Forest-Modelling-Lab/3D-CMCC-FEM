#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include "common.h"

#define BUFFER_SIZE 1024
#define VALUE_AT(d,r,c)	(((r)*(d)->columns_count)+(c))

extern logger_t* g_debug_log;
extern char sz_err_out_of_memory[];

/*
typedef enum {
	FLOAT_TYPE
	, STRING_TYPE
} e_type;
*/

typedef union {
	double v;
	char *s;
} value;

typedef enum {
	TYPE_UNKNOWN
	, TYPE_DIRECTORY
	, TYPE_FILE
} e_type;

typedef struct {
	char **header;
	value *values;
	int rows_count;
	int columns_count;
} dataset_comp_t;

static e_type get_type(const char* const s) {
	struct stat statbuf;

	if ( ! stat(s, &statbuf) ) {
		if ( S_ISDIR(statbuf.st_mode) ) {
			return TYPE_DIRECTORY;
		} else if ( S_ISREG(statbuf.st_mode) ) {
			return TYPE_FILE;
		}
	}
	return TYPE_UNKNOWN;
}

static char* get_type_string(e_type type) {
	if ( TYPE_DIRECTORY == type ) return "directory";
	else if ( TYPE_FILE == type ) return "file";
	return "unknown";
}

static dataset_comp_t* dataset_new(void) {
	dataset_comp_t *p;

	p = malloc(sizeof*p);
	if ( p ) {
		p->header = NULL;
		p->values = NULL;
		p->rows_count = 0;
		p->columns_count = 0;
	}
	return p;
}

static void dataset_free(dataset_comp_t* d) {
	if ( d ) {
		int i;
		int y = -1;

		for ( i = 0; i < d->columns_count; ++i ) {
			if ( ! string_compare_i(d->header[i], "SPECIES") ) {
				y = i;
				break;
			}
		}
		if ( y != -1 ) {
			for ( i = 0; i < d->rows_count; ++i ) {
				free(d->values[VALUE_AT(d,i,y)].s);
			}
		}
		free(d->values);
		for ( i = 0; i < d->columns_count; ++i ) {
			free(d->header[i]);
		}
		free(d->header);
		free(d);
	}
}

static int istab(const int c) {
	return ('\t' == c);
}

static dataset_comp_t* dataset_import(const char*const filename) {
	char buffer[BUFFER_SIZE];
	char* p;
	char* p2;
	char* token;
	int i;
	int row;
	long int file_data_start;
	long int file_data_end;
	FILE *f;
	dataset_comp_t* d;

	const char delimiter[] = " \t\r\n";

	// reset
	f = NULL;
	d = NULL;

	assert(filename);

	d = dataset_new();
	if ( ! d ) {
		logger_error(g_debug_log, "%s\n", sz_err_out_of_memory);
		goto err;
	}

	f = fopen(filename, "r");
	if ( ! f ) {
		logger_error(g_debug_log, "unable to open %s\n", filename);
		goto err;
	}

	// get header ( skip comments )
	do {
		if ( ! fgets(buffer, BUFFER_SIZE, f) ) {
			logger_error(g_debug_log, "%s is empty\n", filename);
			goto err;
		}

		// remove initial spaces and tabs (if any)
		p2 = buffer;
		while ( isspace(*p2) || istab(*p2) ) ++p2;
		// skip empty lines and comments
	} while ( ('\r' == p2[0]) || ('\n' == p2[0]) || ('/' == p2[0]) );
	if ( ! p2 || ! p2[0] ) {
		logger_error(g_debug_log, "%s is empty\n", filename);
		goto err;
	}

	file_data_start = ftell(f);

	// count columns
	{
		char* copy = string_copy(p2);
		if ( ! copy ) {
			logger_error(g_debug_log, "%s\n", sz_err_out_of_memory);
			goto err;
		}
		for ( token = string_tokenizer(copy, delimiter, &p); token; token = string_tokenizer(NULL, delimiter, &p) ) {
			// skip delimiter column
			if ( '*' != token[0] ) {
				++d->columns_count;
			}
		}
		free(copy);
		if ( ! d->columns_count ) {
			logger_error(g_debug_log, "no columns found in %s\n", filename);
			goto err;
		}
	}

	// alloc memory for header
	d->header = malloc(d->columns_count*sizeof*d->header);
	if ( ! d->header ) {
		logger_error(g_debug_log, "%s\n", sz_err_out_of_memory);
		goto err;
	}
	for ( i = 0; i < d->columns_count; ++i ) {
		d->header[i] = NULL;
	}

	// get header
	for ( i = 0, token = string_tokenizer(p2, delimiter, &p); token; token = string_tokenizer(NULL, delimiter, &p), ++i ) {
		// skip delimiter column
		if ( '*' == token[0] ) {
			--i;
			continue;
		}

		d->header[i] = string_copy(token);
		if ( ! d->header[i] ) {
			logger_error(g_debug_log, "%s\n", sz_err_out_of_memory);
			goto err;
		}
	}

	// count rows
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		p = buffer;

		// remove initial spaces and tabs (if any)
		while ( isspace(*p) || istab(*p) ) ++p;

		// remove comment, carriage return and newline
		for ( i = 0; p[i]; ++i ) {
			if ( ('/' == p[i]) || ('\n' == p[i]) || ('\r' == p[i]) ) {
				p[i] = '\0';
				break;
			}
		}

		// skip empty lines
		if ( '\0' == p[0] ) {
			continue;
		}

		// check for EOD (end of data)
		if ( strstr(buffer, PROGRAM_NAME) ) {
			break;
		} else {
			++d->rows_count;
		}
	}

	if ( ! d->rows_count ) {
		logger_error(g_debug_log, "no values founded in %s\n", filename);
		goto err;
	}

	// alloc memory for values
	d->values = malloc(d->rows_count*d->columns_count*sizeof*d->values);
	if ( ! d->values ) {
		logger_error(g_debug_log, "%s\n", sz_err_out_of_memory);
		goto err;
	}
	memset(d->values, 0, sizeof*d->values*d->rows_count*d->columns_count);

	// get EOD
	file_data_end = ftell(f);
	
	// rewind
	fseek(f, file_data_start, 0);

	// import rows
	row = 0;
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		int err;
		double v;

		// EOD ?
		if ( file_data_end == ftell(f) ) {
			break;
		}

		/* remove initial spaces and tabs (if any) */
		p2 = buffer;
		while ( isspace(*p2) || istab(*p2) ) ++p2;

		/* remove comment, carriage return and newline */
		for ( i = 0; p2[i]; ++i ) {
			if ( ('\n' == p2[i]) || ('\r' == p2[i]) ) {
				p2[i] = '\0';
				break;
			}
		}

		/* skip empty lines */
		if ( '\0' == p2[0] ) {
			continue;
		}

		if ( row > d->rows_count ) {
			logger_error(g_debug_log, "too many rows in %s\n", filename);
			goto err;
		}

		for ( i = 0, token = string_tokenizer(p2, delimiter, &p); token; token = string_tokenizer(NULL, delimiter, &p), ++i ) {
			/* skip column delimiter */
			if ( '*' == token[0] ) {
				--i;
				continue;
			}
			v = convert_string_to_float(token, &err);
			if ( err ) {
				// check if is species!
				if ( ! string_compare_i(d->header[i], "SPECIES") ) {
					d->values[VALUE_AT(d,row,i)].s = string_copy(token);
					if ( ! d->values[VALUE_AT(d,row,i)].s ) {
						logger_error(g_debug_log, "%s\n", sz_err_out_of_memory);
						goto err;
					}
				} else {
					logger_error(g_debug_log, "unable to convert '%s' at %d,%d in %s\n"
																, token
																, row+1
																, i+1
																, filename
					);
					goto err;
				}
			} else {
				d->values[VALUE_AT(d,row,i)].v = v;
			}
		}
		++row;
	}

	if ( row != d->rows_count ) {
		logger_error(g_debug_log, "imported rows should be %d not %d in %s\n", d->rows_count, row, filename);
		goto err;
	}
	fclose(f);

	// debug
	/*
	{
		FILE *f = fopen("imported.txt", "w");
		if ( f  ) {
			int i;
			for ( i = 0; i <d->columns_count; ++i ) {
				fprintf(f, "%s", d->header[i]);
				if ( i < d->columns_count-1 ) {
					fputs(",", f);
				}
			}
			fputs("\n", f);
			for ( i = 0; i < d->rows_count; ++i ) {
				int y;
				for ( y = 0; y < d->columns_count; ++y ) {
					if ( ! string_compare_i(d->header[y], "SPECIES") ) {
						fprintf(f, "%s", d->values[VALUE_AT(d,i,y)].s);
					} else {
						fprintf(f, "%g", d->values[VALUE_AT(d,i,y)].v);
					}					
					if ( y < d->columns_count-1 ) {
						fputs(",", f);
					}
				}
				fputs("\n", f);
			}
			fclose(f);
		}	
	}
	*/

	return d;

err:
	if ( f ) fclose(f);
	if ( d ) dataset_free(d);
	return NULL;
}

int diff(const char* const input1, const char* const input2) {
	int ret;
	int row;
	int col;
	dataset_comp_t* d1;
	dataset_comp_t* d2;
	
	assert(input1 && input2);

	// defaults to err
	ret = 0;

	// 
	d1 = NULL;
	d2 = NULL;
	d1 = dataset_import(input1);
	if ( ! d1 ) goto quit;

	d2 = dataset_import(input2);
	if ( ! d2 ) goto quit;

	// compare column count
	if ( d1->columns_count != d2->columns_count ) {
		logger_error(g_debug_log, "compare: error! %d vs %d columns count\n"
											, d1->columns_count
											, d2->columns_count
		);
		goto quit;
	}
	// compare rows count
	if ( d1->rows_count != d2->rows_count ) {
		logger_error(g_debug_log, "compare: error! %d vs %d row count\n"
											, d1->rows_count
											, d2->rows_count
		);
		goto quit;
	}

	// compare header
	for ( col = 0; col < d1->columns_count; ++col ) {
		if ( string_compare_i(d1->header[col], d2->header[col]) ) {
			logger_error(g_debug_log, "compare: error! %d vs %d row count\n"
											, d1->header[col]
											, d2->header[col]
			);
			goto quit;
		}
	}

	// create diff file
	{
		char *p;
		FILE *f;
		char buffer[BUFFER_SIZE];

		sprintf(buffer, "%s", input1);
		p = strrchr(buffer, '.');
		if ( ! p ) {
			logger_error(g_debug_log, "unable to create diff file for %s\n", input1);
			goto quit;
		}
		*p = '\0';
		strcat(buffer, "_diff.csv");

		f = fopen(buffer, "w");
		if ( ! f ) {
			logger_error(g_debug_log, "unable to create diff file for %s\n", input1);
			goto quit;
		}

		// write header
		for ( col = 0; col < d1->columns_count; ++col ) {
			fprintf(f, "%s", d1->header[col]);
			if ( col < d1->columns_count-1 ) {
				fputs(",", f);
			}
		}
		fputs("\n", f);

		// write diff
		for ( row = 0; row < d1->rows_count; ++row ) {
			for ( col = 0; col < d1->columns_count; ++col ) {
				if ( ! string_compare_i(d1->header[col], "SPECIES") ) {
					if ( ! string_compare_i(d1->values[VALUE_AT(d1,row,col)].s, d2->values[VALUE_AT(d2,row,col)].s) ) {
						fputs(d1->values[VALUE_AT(d1,row,col)].s, f);
					} else {
						fprintf(f, "%s-%s", d1->values[VALUE_AT(d1,row,col)].s, d2->values[VALUE_AT(d2,row,col)].s);
					}

				} else {
					double v1 = d1->values[VALUE_AT(d1,row,col)].v;
					double v2 = d2->values[VALUE_AT(d2,row,col)].v;

					if ( IS_INVALID_VALUE(v1) && IS_INVALID_VALUE(v2) ) {
						fputs("0", f);
					} else if ( IS_INVALID_VALUE(v1) || IS_INVALID_VALUE(v2) ) {
						fprintf(f, "%g-%g", v1, v2);
					} else {
						fprintf(f, "%g", fabs(v1-v2));
					}
				}
				if ( col < d1->columns_count-1 ) {
					fputs(",", f);
				}
			}
			fputs("\n", f);
		}	
		fclose(f);
	}

	ret = 1;

quit:
	if ( d2 ) dataset_free(d2);
	if ( d1 ) dataset_free(d1);

	return 1;
}

int compare(const char* const input1, const char* const input2) {
	int ret;
	DIR *dir;
	struct dirent *entry;

	dir = NULL;
	// defaults to err
	ret = 0;

	if ( ! input1 || ! input1[0] ) {
		logger_error(g_debug_log, "invalid input path specified\n");
		goto quit;
	}

	if ( ! input2 || ! input2[0] ) {
		logger_error(g_debug_log, "invalid output path specified\n");
		goto quit;
	}

	dir = opendir(input1);
	if ( ! dir ) {
		logger_error(g_debug_log, "unable to open %s\n", input1);
		goto quit;
	}

	entry = readdir(dir);
	if ( ! entry ) {
		logger_error(g_debug_log, "no files to compare!");
		goto quit;
	}

	do {
		char path1[1024];	// should be enough
		char path2[1024];
		int i;
		int len;

		// skip current folder, previous folder, parameterization folder and diff files!
    	if ( 	! strcmp(entry->d_name, ".")
				|| ! strcmp(entry->d_name, "..")
				|| ! strcmp(entry->d_name, "parameterization")
				|| strstr(entry->d_name, "_diff.csv")
		) {
			continue;
		}

		//

		// compute paths
		i = has_path_delimiter(input1);
		len = sprintf(path1, "%s%s%s"
							, input1
							, i ? "" : FOLDER_DELIMITER
							, entry->d_name
		);
		path1[len] = '\0';
		i = has_path_delimiter(input2);
		len = sprintf(path2, "%s%s%s"
							, input2
							, i ? "" : FOLDER_DELIMITER
							, entry->d_name
		);
		path2[len] = '\0';

		// is a directory ?
		if ( DT_DIR == entry->d_type ) {
			// check if path2 exists!
			if ( TYPE_DIRECTORY != get_type(path2) ) {
				logger_error(g_debug_log, "%s not found in %s. skipped.\n", path1, path2);
				continue;
			}
        	compare(path1, path2);
    	} else {
        	// check if path2 exists!
			if ( TYPE_FILE != get_type(path2) ) {
				logger_error(g_debug_log, "%s not found in %s. skipped.\n", path1, path2);
				continue;
			}
			diff(path1, path2);
		}
	} while ( (entry = readdir(dir)) );

	// ok
	ret = 1;

quit:
	if ( dir ) closedir(dir);
	return ret;
}
