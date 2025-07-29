/* soil_settings.h */
#ifndef SOIL_H_
#define SOIL_H_

typedef enum {
	LANDUSE_F		/* forest */
	, LANDUSE_Z		/* crop */
} e_landuse;

enum {
// SOIL_LANDUSE                      /* Cell landuse */	
	 SOIL_X = 0
	, SOIL_Y
	, SOIL_LAT                          /* Cell latitude */
	, SOIL_LON                          /* Cell longitude */
	, SOIL_CLAY_PERC                    /* Clay percentage */
	, SOIL_SILT_PERC                    /* Silt percentage */
	, SOIL_SAND_PERC                    /* Sand percentage */
	, SOIL_DEPTH                        /* Soil depth (cm) */
	, SOIL_FR                           /* parameter for soil fertility modifier (3-PG) */
	, SOIL_FN0                          /* parameter for soil fertility modifier (3-PG) */
	, SOIL_FNN                          /* parameter for soil fertility modifier (3-PG) */
	, SOIL_M0                           /* parameter for soil fertility modifier (3-PG) */
	, LITTERC                           /* Litter in tons of Carbon / SizeCell */
	, LITTERN                           /* Litter in tons of Nitrogen / SizeCell */
	, SOILC                             /* Soil in g of Carbon / m2 */	
	, SOILN                             /* Soil in g of Nitrogen / m2 */
	, DEADWOODC                        /* Coarse woody debris in g of Carbon / m2 */
	/* UMM 27 June 2025*/
	, SOIL1C                             /* Soil 1 in g of Carbon / m2 */
	, SOIL2C                             /* Soil 2 in g of Carbon / m2*/
	, SOIL3C                             /* Soil 3 in g of Carbon / m2 */
	, SOIL4C                             /* Soil 4 in g of Carbon / m2*/
        /* UMM 27 June 2025*/
	, DEADWOOD2C                             /* Coarse woody debris 2 in g of Carbon / m2 */
	, DEADWOOD3C                             /* Coarse woody debris 3 in g of Carbon / m2 */
	, DEADWOOD4C                             /* Coarse woody debris  4 in g of Carbon / m2 */
	/* please keep it here! */ // the last place on this record
	, SOIL_LANDUSE                      /* Cell landuse */	
	, SOIL_VARS_COUNT
};


typedef struct {
	e_landuse landuse;
	double values[SOIL_VARS_COUNT-1]; // -1 for landuse
	//todo lat long and elev should be taken from met netcdf files (but for .txt files??)
} soil_settings_t;

soil_settings_t* soil_settings_import(const char *const filename, int*const soil_settings_count);

#endif /* SOIL_H_ */
