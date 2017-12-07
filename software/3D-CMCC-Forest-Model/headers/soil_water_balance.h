/* soil_water_balance.h */
#ifndef SOIL_WATER_BALANCE_H_
#define SOIL_WATER_BALANCE_H_

#include "matrix.h"

void soil_water_balance(cell_t *c, const meteo_daily_t *const meteo_daily, int year);

#endif /* SOIL_WATER_BALANCE_H_ */
