/* canopy_evapotranspiration.h */
#ifndef CANOPY_EVAPOTRANSPIRATION_H_
#define CANOPY_EVAPOTRANSPIRATION_H_

#include "matrix.h"

void canopy_evapotranspiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily);

#endif /* CANOPY_EVAPOTRANSPIRATION_H_ */
