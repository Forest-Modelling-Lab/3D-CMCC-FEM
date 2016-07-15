/* heat_fluxes.h */
#ifndef HEAT_FLUXES_H_
#define HEAT_FLUXES_H_

#include "matrix.h"

void Canopy_latent_heat_fluxes (species_t *const s, const meteo_daily_t *const meteo_daily);
void Canopy_sensible_heat_fluxes(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily);
void Latent_heat_flux (cell_t *const c, const meteo_daily_t *const meteo_daily);
void Sensible_heat_flux (cell_t *const c, const meteo_daily_t *const meteo_daily);

#endif /* HEAT_FLUXES_H_ */
