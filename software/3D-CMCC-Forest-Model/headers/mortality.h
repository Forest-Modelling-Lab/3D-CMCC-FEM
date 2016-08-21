/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

int self_pruning (cell_t *const, const int);

void self_thinning(cell_t *const, const int);

void Stool_mortality(cell_t *const, const int, const int, const int, const int);

#endif /* MORTALITY_H_ */
