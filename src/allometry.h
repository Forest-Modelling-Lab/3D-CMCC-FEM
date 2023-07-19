/* allometry.h */
#ifndef ALLOMETRY_H_
#define ALLOMETRY_H_

#include "matrix.h"

void allometry_power_function(cell_t *const c);

void crown_allometry (cell_t *const c, const int height, const int dbh,const int age, const int species);

#endif /* ALLOMETRY_H_ */
