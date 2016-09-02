/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

void daily_growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species );

void annual_growth_efficiency_mortality ( cell_t *const c, const int height, const int dbh, const int age, const int species );

void layer_self_pruning_thinning ( cell_t *const );

int self_pruning (cell_t *const c, const int layer );

void self_thinning (cell_t *const c, const int layer );

void age_mortality (age_t *const a, species_t *const s);

void Stool_mortality(cell_t *const, const int, const int, const int, const int);

#endif /* MORTALITY_H_ */
