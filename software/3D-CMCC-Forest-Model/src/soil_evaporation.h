/* soil_evaporation.h */
#ifndef SOIL_EVAPORATION_H_
#define SOIL_EVAPORATION_H_

#include "matrix.h"

void soil_evaporation_biome(cell_t *const c, const meteo_t *const met, const int month, const int day);
void Soil_evaporation(cell_t * c, const meteo_t *const met, int month, int day);

#endif /* SOIL_EVAPORATION_H_ */