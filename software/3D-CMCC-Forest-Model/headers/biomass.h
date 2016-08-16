/* biomass.h */
#ifndef BIOMASS_H_
#define BIOMASS_H_

#include "matrix.h"

void live_total_wood_age(const age_t *const a, const int species);
void average_tree_biomass(species_t *const s);
void abg_bgb_biomass (cell_t *const c, const int height, const int age, const int species);
void annual_tree_increment(cell_t *const c, const int layer, const int height, const int age, const int species);

#endif /* BIOMASS_H */
