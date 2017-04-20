/* print.h */
#ifndef PRINT_H_
#define PRINT_H_

#include "matrix.h"

void print_daily_cell_data(cell_t* const c);

void print_daily_forest_data(cell_t* const c);

void print_daily_forest_class_data(cell_t* const c, const int layer, const int height, const int dbh, const int age, const int species);

void print_new_daily_forest_class_data(cell_t* const c, const int height, const int dbh, const int age, const int species);

void print_daily_met_data(cell_t *c, const int day, const int month, const int year);

#endif /* PRINT_H_ */
