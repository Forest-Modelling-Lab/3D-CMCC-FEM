/* soil_respiration.h */
#ifndef SOIL_RESPIRATION_H_
#define SOIL_RESPIRATION_H_

#include "matrix.h"

void soil_respiration_canoak(cell_t *const c, const meteo_daily_t *const meteo_daily);

void soil_respiration_reichstein(cell_t *const c, const meteo_daily_t *const meteo_daily);

#endif /* SOIL_RESPIRATION_H_ */
