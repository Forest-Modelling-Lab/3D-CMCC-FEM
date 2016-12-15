/* biomass.h */
#ifndef BIOMASS_H_
#define BIOMASS_H_

#include "matrix.h"

void live_total_wood_age(const age_t *const a, const int species);

void average_tree_pools(species_t *const s);

void tree_branch_and_bark (cell_t *const c, const int height, const int dbh, const int age, const int species);

void abg_bgb_biomass (cell_t *const c, const int height, const int dbh, const int age, const int species);

void annual_tree_increment(cell_t *const c, const int height, const int dbh, const int age, const int species, const int year);

void tree_biomass_remove (species_t *const s, const int tree_remove);

#endif /* BIOMASS_H */
