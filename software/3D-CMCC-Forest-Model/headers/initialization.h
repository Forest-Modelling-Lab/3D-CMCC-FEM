/* initialization.h */
#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include "matrix.h"

void initialization_forest_structure    (cell_t *const c, const int day, const int month, const int year);

void initialization_forest_class_C      (cell_t *const c, const int height, const int dbh, const int age, const int species);

void initialization_forest_class_N      (cell_t *const c, const int height, const int dbh, const int age, const int species);

void initialization_forest_class_litter (cell_t *const c, const int height, const int dbh, const int age, const int species);

void initialization_forest_cell_C       (cell_t *const c, const int height, const int dbh, const int age, const int species);

void initialization_forest_cell_N       (cell_t *const c, const int height, const int dbh, const int age, const int species);

void initialization_cell_litter_biochem (cell_t *const c);

void initialization_cell_soil_biochem   (cell_t *const c);

void initialization_cell_soil_physic    (cell_t *const c);

#endif /* INITIALIZATION_H_ */
