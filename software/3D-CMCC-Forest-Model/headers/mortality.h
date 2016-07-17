/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

void Layer_cover_mortality(cell_t *const c, const int layer, const int height, const int age, const int species, const double layer_cover, const int tree_number);

void Stool_mortality(species_t *const s, const int year);

#endif /* MORTALITY_H_ */
