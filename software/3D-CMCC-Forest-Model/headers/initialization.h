/* initialization.h */
#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include "matrix.h"

void initialization_forest_structure(cell_t *const c, const int height, const int age, const int species);

void initialization_forest_C_biomass(cell_t *const c, const int height, const int age, const int species);

void initialization_forest_N_biomass(cell_t *const c, const int height, const int age, const int species);

void initialization_soil(cell_t *const c);

#endif /* INITIALIZATION_H_ */
