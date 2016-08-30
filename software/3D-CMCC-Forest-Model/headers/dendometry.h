/* dendometry.h */
#ifndef DENDOMETRY_H_
#define DENDOMETRY_H_

#include "matrix.h"

void dendrometry(cell_t *const c, const int height, const int dbh, const int age, const int species);

void annual_minimum_reserve (species_t *const s);

#endif /* DENDOMETRY_H_ */
