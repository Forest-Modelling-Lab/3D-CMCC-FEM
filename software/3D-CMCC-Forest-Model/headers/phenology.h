/* phenology.h */
#ifndef PHENOLOGY_H_
#define PHENOLOGY_H_

#include "matrix.h"

void prephenology (cell_t *const c, const meteo_daily_t *const meteo_daily, const int day, const int month);

void phenology(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const int month);

#endif /* PHENOLOGY_H_ */
