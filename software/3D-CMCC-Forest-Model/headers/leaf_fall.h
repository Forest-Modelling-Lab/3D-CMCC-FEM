/* leaf_fall.h */
#ifndef LEAF_FALL_H_
#define LEAF_FALL_H_

#include "matrix.h"

void leaf_fall_deciduous(cell_t *const c, const int height, const int dbh, const int age, const int species);

void leaf_fall_evergreen(cell_t *const c, const int height, const int dbh, const int age, const int species);

void get_vpsat(cell_t *const c, const int day, const int month, const int year, const int index);

#endif /* LEAF_FALL_H_ */
