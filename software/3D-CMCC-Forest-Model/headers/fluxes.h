/* fluxes.h */
#ifndef FLUXES_H_
#define FLUXES_H_

#include "matrix.h"

void carbon_fluxes(cell_t *const c, const int height, const int dbh, const int age, const int species);

void water_fluxes(cell_t *const c, const meteo_daily_t *const meteo_daily);

void net_ecosystem_exchange(cell_t *const c);

#endif /* FLUXES_H_ */
