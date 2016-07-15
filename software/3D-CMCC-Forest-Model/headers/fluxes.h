/* fluxes.h */
#ifndef FLUXES_H_
#define FLUXES_H_

#include "matrix.h"

void Carbon_fluxes(species_t *const s);
void Water_fluxes(cell_t *const c);
void get_net_ecosystem_exchange(cell_t *const c);

#endif /* FLUXES_H_ */
