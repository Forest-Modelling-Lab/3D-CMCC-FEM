/* dataset.c */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "types.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif



/* constants */
/* please see header.h */
enum {
	X_COLUMN = 0,
	Y_COLUMN,
	LANDUSE_COLUMN,
	AGE_COLUMN,
	SPECIES_COLUMN,
	//PHENOLOGY_COLUMN,
	MANAGEMENT_COLUMN,
	N_COLUMN,
	STOOL_COLUMN,
	AVDBH_COLUMN,
	HEIGHT_COLUMN,
	WF_COLUMN,
	WRC_COLUMN,
	WRF_COLUMN,
	WS_COLUMN,
	WBB_COLUMN,
	WRES_COLUMN,
	LAI_COLUMN,
	COLUMNS
};

/* strings */
static const char header_delimiter[] = ", \r\n";
static const char dataset_delimiter[] = ",\r\n";

/* error strings */
static const char err_redundancy[] = "redundancy: var \"%s\" already founded at column %d.\n";
static const char err_unable_find_column[] = "unable to find column for \"%s\" var.\n";
static const char err_conversion[] = "error during conversion of \"%s\" value at row %d, column %d.\n";
static const char err_bad_landuse_length[] =" bad landuse length at row %d, landuse must be 1 character.\n";
static const char err_bad_landuse[] ="bad landuse %c at row %d\n";
//static const char err_bad_phenology_length[] =" bad phenology length at row %d, phenology must be 1 character.\n";
//static const char err_bad_phenology[] = "bad phenology %c at row %d\n";
static const char err_bad_management_length[] =" bad management length at row %d, management must be 1 character.\n";
static const char err_bad_management[] = "bad management %c at row %d\n";
static const char err_too_many_column[] = "too many columns at row %d\n";


/* extern error strings */
extern const char err_empty_file[];
extern const char err_out_of_memory[];
extern const char err_unable_open_file[];

/* */
//new land use
//version 3D-CMCC Forest Model "pre-spatial"
static const char *header[COLUMNS] = {
		"X",
		"Y",
		"LANDUSE",//land use type (F=Forest, C=crop)
		"AGE",
		"SPECIES",
		"MANAGEMENT",
		"N",
		"STOOL",
		"AVDBH",
		"HEIGHT",
		"WF",
		"WRC",
		"WRF",
		"WS",
		"WBB",
		"WRES",
		"LAI"
};

/* */
ROW *import_dataset(const char *const filename, int *const rows_count) {
	int i = 0;
	int y = 0;
	char buffer[BUFFER_SIZE];
	char *token;
	char *p;
	int error;
	FILE *f;
	int *columns;
	PREC value;
	ROW *rows;
	ROW *rows_no_leak;
	int assigned;

	/* reset */
	*rows_count = 0;
	/* check parameter */
	if ( !filename || '\0' == filename[0] ) {
		return NULL;
	}
	/* open file */
	f = fopen(filename, "r");
	if ( !f ) {
		puts(err_unable_open_file);
		return NULL;
	}
	/* get header */
	if ( !fgets(buffer, BUFFER_SIZE, f) ) {
		puts(err_empty_file);
		fclose(f);
		return 0;
	}
	/* alloc memory for columns */
	columns = malloc(COLUMNS * sizeof *columns);
	if ( !columns ) {
		puts(err_out_of_memory);
		fclose(f);
		return NULL;
	}
	/* reset columns */
	for ( i = 0; i < COLUMNS; i++ ) {
		columns[i] = -1;
	}
	/* get columns */
	for ( y = 0, token = mystrtok(buffer, header_delimiter, &p); token; token = mystrtok(NULL, header_delimiter, &p), ++y ) {
		for ( i = 0; i < COLUMNS; i++ ) {
			if ( 0 == mystricmp(token, header[i]) ) {
				/* check for duplicated columns */
				if ( -1 != columns[i] ) {
					printf(err_redundancy, token, columns[i]+1);
					free(columns);
					fclose(f);
					return NULL;
				}

				/* column founded, assign to array and skip to next token */
				columns[i] = y;
				break;
			}
		}
	}
	/* check for missing values */
	for ( i = 0; i < COLUMNS; i++ ) {
		if ( -1 == columns[i] ) {
			printf(err_unable_find_column, header[i]);
			free(columns);
			fclose(f);
			return 0;
		}
	}
	/* get data */
	rows = NULL; /* mandatory, because I use realloc instead of malloc */
	while ( fgets(buffer, BUFFER_SIZE, f) ) {
		/* remove carriage return and newline */
		for ( i = 0; buffer[i]; i++ ) {
			if ( ('\n' == buffer[i]) || ('\r' == buffer[i]) ) {
				buffer[i] = '\0';
				break;
			}
		}
		/* skip empty lines */
		if ( '\0' == buffer[0] ) {
			continue;
		}
		/* alloc memory */
		rows_no_leak = realloc(rows, (++*rows_count)*sizeof*rows_no_leak);
		if ( !rows_no_leak ) {
			puts(err_out_of_memory);
			free(columns);
			free(rows);
			fclose(f);
			return NULL;
		}

		/* assign pointer */
		rows = rows_no_leak;

		/* get data */
		assigned = 0;
		for ( token = mystrtok(buffer, dataset_delimiter, &p), y = 0; token; token = mystrtok(NULL, dataset_delimiter, &p), ++y ) {
			/* put value at specified columns */
			for ( i = 0; i < COLUMNS; i++ )
			{
				if ( y == columns[i] )
				{
					/* assigned */
					++assigned;
					/* check columns */
					if ( LANDUSE_COLUMN == i )
					{
						// check landuse length
						if ( 1 != strlen(token) )
						{
							printf(err_bad_landuse_length, *rows_count);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;
						}
						// check landuse char
						//F = forest, Z = crop
						if ( ('F' == token[0]) || ('f' == token[0]) )
						{
							rows[*rows_count-1].landuse = F ;
						}
						else if ( ('Z' == token[0]) || ('z' == token[0]) )
						{
							rows[*rows_count-1].landuse = Z;
						}
						//todo add land use types how many land use you want to simulate
						 else
						{
							printf(err_bad_landuse, token[0], *rows_count);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;
						}
					}
					else if ( SPECIES_COLUMN == i )
					{
						rows[*rows_count-1].species = malloc(strlen(token)+1);
						if ( !rows[*rows_count-1].species )
						{
							puts(err_out_of_memory);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;
						}
						strcpy(rows[*rows_count-1].species, token);
						/* todo: add check for strcpy */
					}
					else if ( MANAGEMENT_COLUMN == i )
					{
						/* check management length */
						if ( 1 != strlen(token) )
						{
							printf(err_bad_management_length, *rows_count);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;
						}

						/* check management char */
						if ( ('T' == token[0]) || ('t' == token[0]) ) {
							rows[*rows_count-1].management = T;
						} else if ( ('C' == token[0]) || ('c' == token[0]) ) {
							rows[*rows_count-1].management = C;
						} else {
							printf(err_bad_management, token[0], *rows_count);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;
						}
					}
					else
					{
						/* convert string to prec */
						value = convert_string_to_prec(token, &error);
						if ( error )
						{
							printf(err_conversion, token, *rows_count, y+1);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;
						}

						/* convert nan to invalid value */
						if ( value != value )
						{
							value = INVALID_VALUE;
						}

						/* assign value */
						switch ( i ) {
						case X_COLUMN:
							rows[*rows_count-1].x = (int)value;
							break;

						case Y_COLUMN:
							rows[*rows_count-1].y = (int)value;
							break;

						case AGE_COLUMN:
							rows[*rows_count-1].age = (int)value;
							break;

						case N_COLUMN:
							rows[*rows_count-1].n = (int)value;
							break;

						case STOOL_COLUMN:
							rows[*rows_count-1].stump = (int)value;
							break;

						case AVDBH_COLUMN:
							rows[*rows_count-1].avdbh = value;
							break;

						case HEIGHT_COLUMN:
							rows[*rows_count-1].height = value;
							break;

						case WF_COLUMN:
							rows[*rows_count-1].wf = value;
							break;

						case WRC_COLUMN:
							rows[*rows_count-1].wrc = value;
							break;

						case WRF_COLUMN:
							rows[*rows_count-1].wrf = value;
							break;

						case WS_COLUMN:
							rows[*rows_count-1].ws = value;
							break;

						case WBB_COLUMN:
							rows[*rows_count-1].wbb = value;
							break;

						case WRES_COLUMN:
							rows[*rows_count-1].wres = value;
							break;

						case LAI_COLUMN:
							rows[*rows_count-1].lai = value;
							break;
						default:
							printf(err_too_many_column, *rows_count);
							free(columns);
							free(rows);
							fclose(f);
							return NULL;

						}
					}

					/* skip to next token */
					break;
				}
			}
		}

		/* check columns */
		if ( assigned != COLUMNS ) {
			puts("not all values has been imported");
			free(columns);
			free(rows);
			fclose(f);
			return NULL;
		}
	}

	/* free memory */
	free(columns);

	/* close file */
	fclose(f);

	/* return pointer */
	return rows;
}

// Store site.txt variables inside global struct site_t site
int importSiteFile(char *fileName)
{
	int ret = 0;
	FILE *site_fd = fopen(fileName, "r");

	Log ("Importing Site file...\n");

	if( !site_fd ) // error opening file
	{
		fprintf(stderr, "Error while open %s\n", fileName);
		ret = 2;
	}
	else //Read the file
	{
		double *tmpPointer;
		char *getRet = NULL,
				*buffer = malloc(sizeof(*buffer)*1024);

		site = malloc(sizeof(site_t));
		tmpPointer = &(site->lat);

		if(!buffer)
		{
			fprintf(stderr, "Failed malloc for temporary buffer to read site file\n");
			ret = 2;
		}
		else
		{
			char *pch = NULL;
			int i = 0;

			while((getRet = fgets(buffer, 1024, site_fd)) != NULL)
			{
				if( getRet[0] == '\n' || getRet[0] == '/' ) // Skip empty and commented lines
					continue;
				else
				{
					pch = strtok(buffer, " \"");
					pch = strtok(NULL, "\"");

					switch(i)
					{
					case 0:
						strcpy(site->sitename, pch);
						break;
					default:
						*tmpPointer = atof(pch); // Convert each token in a double
						tmpPointer++;            // Shift the pointer of sizeof(int) to change field of the structure
						break;
					}
					i++;
				}
			}
		}
		free(buffer);
	}
	if( fclose(site_fd) != 0 ) //Close the file
	{
		fprintf(stderr, "Error while closing %s; Continue...\n", fileName);
		ret = 3;
	}

	//	fprintf(stderr, "%s\n", site->sitename);
	//	fprintf(stderr, "%f\n", site->lat);
	//	fprintf(stderr, "%f\n", site->lon);
	//	fprintf(stderr, "%f\n", site->initialAvailableSoilWater);
	//	fprintf(stderr, "%f\n", site->co2Conc);
	//	fprintf(stderr, "%f\n", site->initialLitter);
	//	fprintf(stderr, "%f\n", site->swConst);
	//	fprintf(stderr, "%f\n", site->swPower);
	//	fprintf(stderr, "%f\n", c->max_asw);
	//	fprintf(stderr, "%f\n", site->minAsw);
	//	fprintf(stderr, "%f\n", site->fr);
	//	fprintf(stderr, "%f\n", site->fn0);
	//	fprintf(stderr, "%f\n", site->fnn);
	//	fprintf(stderr, "%f\n", site->m0);
	//	fprintf(stderr, "%f\n", settings->cutTree);

	return ret;
}


// Store settings.txt variables inside global struct settings_t settings
int importSettingsFile(char *fileName)
{
	int ret = 0;
	FILE *settings_fd = fopen(fileName, "r");

	Log("Importing setting file...\n");

	if( !settings_fd ) // error opening file
	{
		fprintf(stderr, "Error while open %s\n", fileName);
		ret = 2;
	}
	else //Read the file
	{
		double *tmpPointer;
		char	*getRet = NULL,
				*buffer = malloc(sizeof(*buffer)*1024);

		settings = malloc(sizeof(settings_t));
		tmpPointer = &(settings->sizeCell);

		if(!buffer)
		{
			fprintf(stderr, "Failed malloc for temporary buffer to read settings file\n");
			ret = 2;
		}
		else
		{
			char *pch = NULL;
			int i = 0;

			while((getRet = fgets(buffer, 1024, settings_fd)) != NULL)
			{
				if( getRet[0] == '\n' || getRet[0] == '/' ) // Skip empty and commented lines
					continue;
				else
				{
					pch = strtok(buffer, " \"");
					pch = strtok(NULL, "\"");

					switch(i)
					{
					case 0:
						settings->version = *pch;
						break;
					case 1:
						settings->spatial = *pch;
						break;
					case 2:
						settings->time = *pch;
						break;
						//todo maybe delete it and changed with plot/regional scale spatial resolution
					case 3:
						settings->symmetric_water_competition = *pch;
						break;
					case 4:
						strncpy (settings->spin_up, (const char*)pch, 3);
						break;
					case 5:
						strncpy (settings->CO2_fixed, (const char*)pch, 3);
						break;
					case 6:
						strncpy (settings->Ndep_fixed, (const char*)pch, 3);
						break;
					case 7:
						strncpy (settings->management, (const char*)pch, 3);
						break;
					case 8:
						strncpy (settings->rothC, (const char*)pch, 3);
						break;
					case 9:
						strncpy (settings->dndc, (const char*)pch, 3);
						break;
//					case 10:
//						strncpy (settings->replanted_species, (const char*)pch,53);
//						break;
					case 10:
						*tmpPointer = atof(pch)*atof(pch); // sizeCell
						tmpPointer++;
						break;
					default:
						*tmpPointer = atof(pch); // Convert each token in a double
						tmpPointer++;            // Shift the pointer of sizeof(int) to change field of the structure
						break;
					}
					i++;
				}
			}
		}
		free(buffer);
	}

	if( settings_fd && fclose(settings_fd) != 0 ) //Close the file
	{
		fprintf(stderr, "Error while closing %s; Continue...\n", fileName);
		ret = 3;
	}

	/*fprintf(stderr, "%c\n", settings->spatial);
		fprintf(stderr, "%f\n", settings->sizeCell);
		fprintf(stderr, "%f\n", settings->max_layer_cover);
		fprintf(stderr, "%f\n", settings->avdbh_sapling);
		fprintf(stderr, "%f\n", settings->lai_sapling);
		fprintf(stderr, "%f\n", settings->height_sapling);
		fprintf(stderr, "%f\n", settings->ws_sapling);
		fprintf(stderr, "%f\n", settings->wr_sapling);
		fprintf(stderr, "%f\n", settings->wf_sapling);
		fprintf(stderr, "%f\n", settings->light_estab_very_tolerant);
		fprintf(stderr, "%f\n", settings->light_estab_tolerant);
    	fprintf(stderr, "%f\n", settings->light_estab_intermediate);
		fprintf(stderr, "%f\n", settings->light_estab_intolerant);
		fprintf(stderr, "%f\n", settings->maxlai);
		fprintf(stderr, "%f\n", settings->defaultlai);
		fprintf(stderr, "%f\n", settings->maxdays);
		fprintf(stderr, "%f\n", settings->maxrg);
		fprintf(stderr, "%f\n", settings->maxtavg);
		fprintf(stderr, "%f\n", settings->maxvpd);
		fprintf(stderr, "%f\n", settings->maxprecip);*/

	return ret;
}
