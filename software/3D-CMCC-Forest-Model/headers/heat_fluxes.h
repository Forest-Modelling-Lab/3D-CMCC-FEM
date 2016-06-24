/* heat_fluxes.h */
#ifndef HEAT_FLUXES_H_
#define HEAT_FLUXES_H_

#include "matrix.h"

void Latent_heat_flux(cell_t *const c, const meteo_t *const met, const int month, const int day);

#endif /* HEAT_FLUXES_H_ */