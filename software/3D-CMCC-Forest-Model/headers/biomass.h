/* biomass.h */
#ifndef BIOMASS_H_
#define BIOMASS_H_

#include "matrix.h"

void live_total_wood_age(const age_t *const a, species_t *const s);
void Average_tree_biomass(species_t *const s);
void AGB_BGB_biomass (cell_t *const c, const int height, const int age, const int species);
void Biomass_increment_EOY (cell_t *const c, species_t *const s, const int top_layer, const int z, const int height, const int age);

#endif /* BIOMASS_H */