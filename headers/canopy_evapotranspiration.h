/* canopy_evapotranspiration.h */
#ifndef CANOPY_EVAPOTRANSPIRATION_H_
#define CANOPY_EVAPOTRANSPIRATION_H_

#include "matrix.h"

void canopy_evapotranspiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, meteo_daily_t *const meteo_daily);

void gs_Jarvis (species_t *const s, const double gl_x, const double g_corr);

void gs_Ball_Berry (species_t *const s, const double gl_x, const double g_corr);

#endif /* CANOPY_EVAPOTRANSPIRATION_H_ */
