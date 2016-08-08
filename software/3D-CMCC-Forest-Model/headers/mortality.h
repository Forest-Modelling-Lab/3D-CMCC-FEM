/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

void Crowding_competition_mortality(cell_t *const c, const int layer, const int height, const int age, const int species, const double layer_cover, const int tree_number);

void Stool_mortality(cell_t *const c, const int layer, const int height, const int age, const int species);

#endif /* MORTALITY_H_ */
