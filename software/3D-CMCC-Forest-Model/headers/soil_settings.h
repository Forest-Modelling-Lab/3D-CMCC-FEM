/* soil_settings.h */
#ifndef SOIL_H_
#define SOIL_H_

enum {
	SOIL_LAT = 0
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

	, SOIL_VARS_COUNT
};

typedef struct {
	//todo this should be used only in txt simulations (is it still necessary due to the signature of output files ?)
	char sitename[1024];
	double values[SOIL_VARS_COUNT];
	//todo lat long and elev should be taken from met netcdf files (but for .txt files??)
} soil_settings_t;

soil_settings_t* soil_settings_new(void);
int soil_settings_import(soil_settings_t *const s, const char *const filename, const int x, const int y);

#endif /* SOIL_H_ */
