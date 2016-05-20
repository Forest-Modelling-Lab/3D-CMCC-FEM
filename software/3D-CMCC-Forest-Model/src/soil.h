/* soil.h */
#ifndef SOIL_H_
#define SOIL_H_

enum {
	SOIL_LAT = 0 /* fuck standard */
	, SOIL_LON
	, SOIL_CLAY_PERC

	, SOIL_VALUES_COUNT
};

typedef struct {
	//todo this should be used only in txt simulations (is it still necessary due to the signature of output files ?)
	char sitename[1024];

	//double values[SOIL_VALUES_COUNT];

	//todo lat long and elev should be taken from met netcdf files (but for .txt files??)
	double lat,
	lon;

	double clay_perc,
	silt_perc,
	sand_perc,
	soil_depth;

	double fr,
	fn0,
	fnn,
	m0,
	sN;

	//DNDC
	//	double RFM;
	double soil_ph,
	soil_no3,
	soil_nh4,
	hydraulicConductivity,
	inSOC,
	litFract,
	humaFract,
	humuFract,
	bioFract,
	rcnrvl,
	rcnrl,
	rcnrr,
	rcnb,
	rcnh,
	rcnm,
	rcnh2,
	DClitter,
	DChumads,
	DChumus;
} soil_t;

soil_t* soil_new(void);
void soil_clear(soil_t* const s);
int soil_import(soil_t *const s, const char *const filename, const int x, const int y);

#endif
