/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

void daily_growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species );

int annual_growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species );

void self_pruning (cell_t *const c, const int layer );

void self_thinning_mortality (cell_t *const c, const int layer );

void age_mortality (cell_t *const c, const int height, const int dbh, const int age, const int species);

#endif /* MORTALITY_H_ */
