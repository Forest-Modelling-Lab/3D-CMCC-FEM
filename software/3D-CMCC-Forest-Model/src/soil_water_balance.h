/* soil_water_balance.h */
#ifndef SOIL_WATER_BALANCE_H_
#define SOIL_WATER_BALANCE_H_

#include "matrix.h"

void Soil_water_balance (cell_t *c, const meteo_t *const met, const int month, const int day);

#endif /* SOIL_WATER_BALANCE_H_ */
