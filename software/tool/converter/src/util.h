// util.h
//
// written by A.Ribeca
//
// please note that this file must be included only once!
//

#define INVALID_VALUE			-9999
#define IS_INVALID_VALUE(x)		((INVALID_VALUE==(x)))
#define IS_LEAP_YEAR(x)			(((x) % 4 == 0 && (x) % 100 != 0) || (x) % 400 == 0)
#define SIZEOF_ARRAY(a)			(sizeof((a))/sizeof((a)[0]))

int string_compare_i(const char *str1, const char *str2)
{
	register signed char __res;

	while ( 1 ) {
		if ( (__res = toupper( *str1 ) - toupper( *str2++ )) != 0 || !*str1++ ) {
			break;
		}
	}

	// returns an integer greater than, equal to, or less than zero
	return __res;
}

double convert_string_to_float(const char* const string, int* const err)
{
	char *p;
	double value;
	
	*err = 0;

	if ( ! string || ! string[0] )
	{
		*err = 1;
		return 0.0;
	}

	// check for inf, nan, ...
	if (	! string_compare_i(string, "inf")
			|| ! string_compare_i(string, "nan") )
	{
		return INVALID_VALUE;
	}

	errno = 0;
	value = strtod(string, &p);
	strtod(p, NULL);
	if ( (string == p) || *p || errno )
	{
		*err = 1;
	}
	return value;
}



char *string_copy(const char*const s)
{
	if ( s )
	{
		char *p = malloc(strlen(s)+1);
		if ( p )
		{
			return strcpy(p, s);
		}
	}
	return NULL;
}

char *string_tokenizer(char* string, const char* delimiters, char** p)
{
	char *sbegin;
	char *send;

	sbegin = string ? string : *p;
	sbegin += strspn(sbegin, delimiters);
	if ( '\0' == *sbegin )
	{
		*p = "";
		return NULL;
	}

	send = sbegin + strcspn(sbegin, delimiters);
	if ( *send != '\0')
	{
		*send++ = '\0';
	}
	*p = send;

	return sbegin;
}

char* strstr_i(char* str1, const char* str2)
{
    char* p1 = str1 ;
    const char* p2 = str2 ;
    char* r = *p2 == 0 ? str1 : 0 ;

    while( *p1 != 0 && *p2 != 0 )
	{
        if ( tolower(*p1) == tolower(*p2) )
		{
            if ( ! r ){
                r = p1;
            }
            p2++;
        }
		else
		{
            p2 = str2;
            if ( tolower(*p1) == tolower(*p2) )
			{
                r = p1;
                p2++;
            }
			else
			{
                r = 0;
            }
        }
        p1++;
    }
    return *p2 == 0 ? r : 0;
}

int path_exists(const char *const path)
{
	assert(path);

	if ( ! access(path, W_OK) )
	{
		return 1;
	}

	return 0;
}
