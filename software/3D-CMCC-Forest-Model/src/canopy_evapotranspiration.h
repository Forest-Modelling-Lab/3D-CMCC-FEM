/* canopy_evapotranspiration.h */
#ifndef CANOPY_EVAPOTRANSPIRATION_H_
#define CANOPY_EVAPOTRANSPIRATION_H_

#include "matrix.h"

void canopy_evapotranspiration(species_t *const s, cell_t *const c, const meteo_t *const met, const int month, const int day, const int height, const int age, const int species);

#endif /* CANOPY_EVAPOTRANSPIRATION_H_ */