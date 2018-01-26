// scan.h
//
// written by A.Ribeca
//
// please note that this file must be included only once!
//

#include "dirent.h"

// TODO: should be use constant from OS
#define PATH_SIZE	(256)
#define EXT_SIZE	(256)

#define IS_FLAG_SET(v, m) (((v) & (m)) == (m))

typedef struct
{
	char **filename;
	int count;

} files_t;

void files_free(files_t* files)
{
	assert(files);

	if ( files->filename )
	{
		int i;

		for ( i = 0; i < files->count; ++i )
		{
			free(files->filename[i]);
		}
		free(files->filename);
	}
	free(files);
}

files_t* files_get(const char *const path, const char*const ext, files_t* files)
{
	int flag;
    DIR* dir;
    struct dirent* entry;

	assert(path);

	dir = NULL;

	flag = 0;
	if ( ! files )
	{
		files = malloc(sizeof(files_t));
		if ( ! files ) return NULL;
		memset(files, 0, sizeof(files_t));
		flag = 1; // needed for free memory on error
	}

	dir = opendir(path);
	if ( ! dir )
	{
		if ( flag )
		{
			files_free(files);
			files = NULL;
		}
		goto quit;
	}

    while ( entry = readdir(dir) )
	{
		char buf[PATH_SIZE];

		if ( DT_DIR == entry->d_type )
		{
            if ( ! string_compare_i(entry->d_name, ".") || ! string_compare_i(entry->d_name, "..") )
			{
                continue;
			}
			sprintf(buf, "%s/%s", path, entry->d_name);
            files_get(buf, ext, files);
        }
		else
		{
			char** char_no_leak;

			if ( ext )
			{
				char* p;

				p = strrchr(entry->d_name, '.');
				if ( p && entry->d_name != p && (strlen(entry->d_name) > 1) )
				{
					if ( string_compare_i(++p, ext) )
					{
						continue;
					}
				}
			}

			char_no_leak = realloc(files->filename, (files->count+1)*sizeof*char_no_leak);
			if ( ! char_no_leak )
			{
				if ( flag )
				{
					files_free(files);
					files = NULL;
				}
				goto quit;
			}
			sprintf(buf, "%s/%s", path, entry->d_name);

			files->filename = char_no_leak;
			files->filename[files->count] = string_copy(buf);
			if ( ! files->filename[files->count] )
			{
				if ( flag )
				{
					files_free(files);
					files = NULL;
				}
				goto quit;
			}
			++files->count;
        }
    }
    
quit:
	if ( dir ) closedir(dir);
	return files;
}

#undef EXT_SIZE
#undef PATH_SIZE
