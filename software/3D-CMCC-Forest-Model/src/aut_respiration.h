/* aut_respiration.h */
#ifndef AUT_RESPIRATION_H_
#define AUT_RESPIRATION_H_

#include "matrix.h"

void Maintenance_respiration(species_t *const s, cell_t *const c, const meteo_t *const met, const int month, const int day, const int height);
void Growth_respiration(species_t *const s, cell_t *const c, const int height);
void Autotrophic_respiration (species_t *const s, cell_t *const c, const int height);

#endif /* AUT_RESPIRATION_H_ */