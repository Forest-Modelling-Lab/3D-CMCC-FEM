/* yos.h */
#ifndef YOS_H_
#define YOS_H_

#define YOS_MONTHS_COUNT	12
#define YOS_DAYS_COUNT		31

typedef struct {
	int n_days;
	double solar_rad;
	double tavg;          /* (deg C) daily average air temperature */
	double tmax;          /* (deg C) daily maximum air temperature */
	double tmin;          /* (deg C) daily minimum air temperature */
	double tday;          /* (deg C) daylight  daily air temperature */
	double tnight;        /* (deg C) nightime average daily air temperature */
	double tdew;          /* (deg C) dew average daily air temperature */
	double vpd;
	double rh_f;
	double ts_f;
	double prcp;
	double swc;
	double ndvi_lai;
	double daylength;
	double thermic_sum;   /* daily thermic sum */
	double rho_air;
	double tsoil;
	double et;
	double windspeed;
	double lh_vap;
	double lh_vap_soil;
	double lh_fus;
	double lh_sub;
	double air_pressure;
	double co2_conc;
	double es;            /* (KPa) weighted mean saturation vapour pressure at the air temperature */
	double ea;            /* (KPa) actual vapour pressure derived from relative humidity data */
	double psych;         /* (KPa/°C) psychrometric constant */
} meteo_daily_t;

typedef struct {
	meteo_daily_t d[YOS_DAYS_COUNT];
} meteo_t;

typedef struct {
	int year;
	meteo_t m[YOS_MONTHS_COUNT];
} yos_t; /* just for clarification YOS stand for Year Of Simulation ! */

yos_t* yos_import(const char *const file, int *const yos_count, const int x, const int y);

#endif /* YOS_H_ */
