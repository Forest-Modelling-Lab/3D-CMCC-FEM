/* soil_evaporation.h */
#ifndef SOIL_EVAPORATION_H_
#define SOIL_EVAPORATION_H_

#include "matrix.h"

void Soil_evaporation(cell_t *const c, const meteo_daily_t *const meteo_daily);
void Soil_evaporation_old(cell_t *const c, const meteo_daily_t *const meteo_daily);

#endif /* SOIL_EVAPORATION_H_ */
