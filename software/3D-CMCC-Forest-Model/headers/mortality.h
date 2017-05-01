/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

int growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species );

int annual_growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species );

void self_pruning                      ( cell_t *const c, const int height, const int dbh, const int age, const int species, const double old_layer_cover );

void self_thinning_mortality           (cell_t *const c, const int layer );

void age_mortality                     (cell_t *const c, const int height, const int dbh, const int age, const int species);

#endif /* MORTALITY_H_ */
