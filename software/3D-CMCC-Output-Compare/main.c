// main.c
// written by A. Ribeca on December 16, 2016
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define START_STRING	"3D-CMCC-CNR Forest Ecosystem Model"
#define END_STRING		"--------------------------------------------------------------------------------"

#define BANNER			"3D-CMCC-CNR Forest Ecosystem Model Output Compare\n"		\
						"by Alessio Ribeca\n"										\
						"compiled on "__DATE__" @ "__TIME__"\n"

typedef enum {
	TYPE_UNKNOWN
	, TYPE_DIRECTORY
	, TYPE_FILE 
} e_type;

e_type get_type(const char* const s) {
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

char* get_type_string(e_type type) {
	if ( TYPE_DIRECTORY == type ) return "directory";
	else if ( TYPE_FILE == type ) return "file";
	return "unknown";
}

unsigned int file_load_in_memory(const char* const filename, char** result) {
	unsigned int size;
	FILE *f;

	*result = NULL;

	size = 0;
	f = fopen(filename, "rb");
	if ( !f )  {
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
		*result = NULL;
		return 0;
	}
	fclose(f);
	(*result)[size] = '\0';

	return size;
}

int compare(const char*const input1, const char*const input2) {
	char* p;
	char* file1;
	char* file2;
	unsigned int i;
	unsigned int file1_size;
	unsigned int file2_size;
	unsigned int start_skip_index;
	unsigned int end_skip_index;
	unsigned int row;
	unsigned int col;

	// load files in memory
	file1_size = file_load_in_memory(input1, &file1);
	if ( ! file1 || ! file1_size ) {
		printf("unable to load %s\n", input1);
		return 0;
	}

	file2_size = file_load_in_memory(input2, &file2);
	if ( ! file2 || ! file2_size ) {
		printf("unable to load %s\n", input2);
		free(file1);
		return 0;
	}

	if ( file1_size != file2_size ) {
		printf("%s differs in size\n", input1);
		free(file2);
		free(file1);
		return 0;
	}

	// set message skip index
	start_skip_index = -1;
	end_skip_index = -1;
	p = strstr(file1, START_STRING);
	if ( p ) {
		start_skip_index = p-file1;
	}
	p = strstr(file1, END_STRING);
	if ( p ) {
		end_skip_index = (p-file1)+strlen(END_STRING);
	} else {
		start_skip_index = -1;
	}

	row = 0;
	col = 0;
	for ( i = 0; i < file1_size; ++i ) {
		if ( file1[i] != file2[i] ) {
			// check skip range
			if ( start_skip_index != -1 ) {
				if ( i < start_skip_index || i > end_skip_index ) {
					printf("%s differs at row %d, col %d (%c,%c)\n", input1, row, col, file1[i], file2[i]);
					free(file2);
					free(file1);
					return 0;
				}
			}
		}

		if ( '\n' == file1[i] ) {
			col = 0;
			++row;
		} else {
			++col;
		}
	}

	printf("%s is ok\n", input1);
	free(file2);
	free(file1);
	return 1;
}

int scan(const char* const input1, const char* const input2) {
	DIR *dir;
	struct dirent *entry;

	dir = opendir(input1);
	if ( ! dir ) {
		printf("unable to open %s\n", input1);
		return 0;
	}

	entry = readdir(dir);
	if ( ! entry ) {
		goto quit;
	}

	do {
		char path1[1024];
		char path2[1024];
		int len;

		// skip current and previous folder
    	if ( 	! strcmp(entry->d_name, ".") 
				|| ! strcmp(entry->d_name, "..") ) {
			continue;
		}

		// compute paths
		len = snprintf(path1, sizeof(path1)-1, "%s/%s", input1, entry->d_name);
		path1[len] = '\0';
		len = snprintf(path2, sizeof(path2)-1, "%s/%s", input2, entry->d_name);
		path2[len] = '\0';

		// is a directory ?
		if ( DT_DIR == entry->d_type ) {       	
			// check if path2 exists!
			if ( TYPE_DIRECTORY != get_type(path2) ) {
				printf("%s not found in %s. skipped.\n", path1, path2);
				continue;
			}
        	scan(path1, path2);
    	} else {
        	// check if path2 exists!
			if ( TYPE_FILE != get_type(path2) ) {
				printf("%s not found in %s. skipped.\n", path1, path2);
				continue;
			}
			compare(path1, path2);
		}
	} while ( (entry = readdir(dir)) );

quit:
	closedir(dir);
	return 1;
}

int main(int argc, char *argv[]) {
	char *input1;
	char *input2;
	int input1_type;
	int input2_type;
	int ret;

	puts(BANNER);

	if ( argc < 3 ) {
		puts("usage: compare input1 input2");
		return 1;
	}

	input1 = argv[1];
	input2 = argv[2];

	if ( ! _stricmp(input1, input2) ) {
		puts("error: same input to compare!");
		return 1;
	}

	input1_type = get_type(input1);
	input2_type = get_type(input2);

	if ( TYPE_UNKNOWN == input1_type ) {
		printf("%s is not a valid file\\directory\n\n", input1);
		return 1;
	}

	if ( TYPE_UNKNOWN == input2_type ) {
		printf("%s is not a valid file\\directory\n\n", input2);
		return 1;
	}

	if ( input1_type != input2_type ) {
		printf("compare is not possible: "
				"'%s' is a %s and '%s' is a %s\n"
				, input1, get_type_string(input1_type)
				, input2, get_type_string(input2_type)
		);
		return 1;
	}

	if ( TYPE_DIRECTORY == input1_type ) {
		ret = scan(input1, input2);
	} else {
		ret = compare(input1, input2);
	}

	puts("\ndone!\n");

	return ret;
}

