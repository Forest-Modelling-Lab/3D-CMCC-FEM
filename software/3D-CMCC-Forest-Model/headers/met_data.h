/* met_data.h */
#ifndef MET_DATA_H_
#define MET_DATA_H_

#include "matrix.h"

void Radiation (cell_t *const c, const int day, const int month, const int year);
void Avg_temperature(meteo_t *met, const int day, const int month);
void Daylight_avg_temperature(meteo_t *met, const int day, const int month);
void Nightime_avg_temperature(meteo_t *met, const int day, const int month);
void Thermic_sum (meteo_t *met, const int day, const int month);
void Air_pressure(meteo_t *met, const int day, const int month);
void Air_density (meteo_t *met, const int day, const int month);
void Soil_temperature(meteo_t* met, const int day, const int month);
void Day_Length(cell_t *c, const int day, const int month, const int year);
void Latent_heat(meteo_t *met, const int day, const int month);
void Psychrometric(meteo_t *met, const int day, const int month);
void Sat_vapour_pressure(cell_t *const c, const int day, const int month, const int year);
void Dew_temperature(meteo_t *met, const int day, const int month);
void Annual_met_values(cell_t *const c, const int day, const int month, const int year);
void Annual_CO2_concentration (meteo_t *met, const int day, const int month, const int year);

#endif /* MET_DATA_H_ */
