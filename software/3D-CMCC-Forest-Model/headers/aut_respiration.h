/* aut_respiration.h */
#ifndef AUT_RESPIRATION_H_
#define AUT_RESPIRATION_H_

#include "matrix.h"

void maintenance_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily);

void growth_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species);

void autotrophic_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily);

#endif /* AUT_RESPIRATION_H_ */
