/* leaf_fall.h */
#ifndef LEAF_FALL_H_
#define LEAF_FALL_H_

#include "matrix.h"

void leaffall_deciduous (cell_t *const c, const int height, const int dbh, const int age, const int species);

void leaffall_evergreen (cell_t *const c, const int height, const int dbh, const int age, const int species, const int year);

void leaffall           (species_t *const s);

#endif /* LEAF_FALL_H_ */
