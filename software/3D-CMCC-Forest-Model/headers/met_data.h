/* met_data.h */
#ifndef MET_DATA_H_
#define MET_DATA_H_

#include "matrix.h"

typedef enum {
	WEIGHTED_MEAN_TAVG
	, WEIGHTED_MEAN_TDAY
	, WEIGHTED_MEAN_TNIGHT
	, WEIGHTED_MEAN_TSOIL

	, WEIGHTED_MEAN_COUNT
} e_weighted_average_var;
typedef enum {
	AVERAGED_TAVG
	, AVERAGED_TDAY
	, AVERAGED_TNIGHT
	, AVERAGED_TSOIL

	, AVERAGED_COUNT
} e_averaged_var;

void Daily_Radiation (cell_t *const c, const int day, const int month, const int year);
void Daily_avg_temperature(meteo_d_t *met, const int day, const int month);
void Daylight_avg_temperature(meteo_d_t *const met, const int day, const int month);
void Daily_Nightime_avg_temperature(meteo_d_t *const met, const int day, const int month);
void Daily_Thermic_sum (meteo_d_t *met, const int day, const int month, const int year);
void Daily_Air_pressure(meteo_d_t *met, const int day, const int month);
void Daily_Air_density (meteo_d_t *met, const int day, const int month);
void Daily_Day_Length(cell_t *c, const int day, const int month, const int year);
void Daily_Latent_heat(meteo_d_t *met, const int day, const int month);
void Daily_Psychrometric(meteo_d_t *met, const int day, const int month);
void Daily_Sat_vapour_pressure(cell_t *const c, const int day, const int month, const int year);
void Daily_Dew_temperature(meteo_d_t *const met, const int day, const int month);
void Daily_Check_prcp(cell_t *c, const int day, const int month, const int year);
void Daily_Averaged_temperature(const cell_t *const c, const e_averaged_var var, int day, int month, int year);
void Daily_Weighted_average_temperature(const cell_t *const c, const e_weighted_average_var var, int day, int month, int year);
void Daily_Soil_temperature(const cell_t *const c, int day, int month, int year);
void Daily_Ndeposition(const cell_t *const c, int day, int month, int year_iyearndex);

#endif /* MET_DATA_H_ */
