/* soil_settings.h */
#ifndef SOIL_H_
#define SOIL_H_

typedef enum {
	LANDUSE_F		/* forest */
	, LANDUSE_Z		/* crop */
} e_landuse;

enum {
	SOIL_X = 0
	, SOIL_Y
	, SOIL_LAT
	, SOIL_LON
	, SOIL_CLAY_PERC
	, SOIL_SILT_PERC
	, SOIL_SAND_PERC
	, SOIL_DEPTH
	, SOIL_FR
	, SOIL_FN0
	, SOIL_FNN
	, SOIL_M0
	, SOIL_SN
	, SOIL_PH
	, SOIL_NO3
	, SOIL_NH4
	, SOIL_HYDRAULIC_CONDUCTIVITY
	, SOIL_IN_SOC
	, SOIL_LIT_FRACT
	, SOIL_HUMA_FRACT
	, SOIL_HUMU_FRACT
	, SOIL_BIO_FRACT
	, SOIL_RCNRVL
	, SOIL_RCNRL
	, SOIL_RCNRR
	, SOIL_RCNB
	, SOIL_RCNH
	, SOIL_RCNM
	, SOIL_RCNH2
	, SOIL_DC_LITTER
	, SOIL_DC_HUMADS
	, SOIL_DC_HUMUS
	, SOIL_LANDUSE

	, SOIL_VARS_COUNT
};

typedef struct {
	e_landuse landuse;
	double values[SOIL_VARS_COUNT-1]; // -1 for landuse
	//todo lat long and elev should be taken from met netcdf files (but for .txt files??)
} soil_settings_t;

soil_settings_t* soil_settings_import(const char *const filename, int*const soil_settings_count);

#endif /* SOIL_H_ */
