#include <stdio.h>
#include <stdarg.h>
#include "types.h"

extern int log_enabled;

static FILE *file_log = NULL;

int logInit(char * logFileName)
{
	file_log = fopen(logFileName, "w");

	if ( !file_log ) return 0;

	return 1;
}

void Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && file_log )
		fputs(szBuffer, file_log);
}

void logClose(void)
{
	if ( file_log )	fclose(file_log);
}
