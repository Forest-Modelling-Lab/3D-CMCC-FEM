#include <stdio.h>
#include <stdarg.h>
#include "types.h"

extern int log_enabled;

static FILE *file_log ;

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

//for monthly output
static FILE *monthly_file_log ;

int monthly_logInit(char * monthly_logFileName)
{
	monthly_file_log = fopen(monthly_logFileName, "w");

	if ( !monthly_file_log ) return 0;

	return 1;
}

void Monthly_Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && monthly_file_log )
		fputs(szBuffer, monthly_file_log);
}

void monthly_logClose(void)
{
	if ( monthly_file_log )	fclose(monthly_file_log);
}

//for annual output
static FILE *annual_file_log ;

int annual_logInit(char * annual_logFileName)
{
	annual_file_log = fopen(annual_logFileName, "w");

	if ( !annual_file_log ) return 0;

	return 1;
}

void Annual_Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && annual_file_log )
		fputs(szBuffer, annual_file_log);
}

void annual_logClose(void)
{
	if ( annual_file_log )	fclose(annual_file_log);
}
