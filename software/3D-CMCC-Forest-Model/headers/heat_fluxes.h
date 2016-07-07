/* heat_fluxes.h */
#ifndef HEAT_FLUXES_H_
#define HEAT_FLUXES_H_

#include "matrix.h"

void Canopy_latent_heat_fluxes (species_t *const s, const meteo_t *const met, const int month, const int day);
void Canopy_sensible_heat_fluxes (cell_t *const c, species_t *const s, const meteo_t *const met, const int month, const int day);
void Latent_heat_flux(cell_t *const c, const meteo_t *const met, const int month, const int day);
void Sensible_heat_flux (cell_t *const c, const meteo_t *const met, const int month, const int day);

#endif /* HEAT_FLUXES_H_ */
