/* utility.h */
#ifndef UTILITY_H_
#define UTILITY_H_

#include "matrix.h"

void First_day(cell_t *const c, int layer, int height, int age, int species);

void reset_daily_cell_variables(cell_t *const c);
void reset_monthly_cell_variables(cell_t *const c);
void reset_annual_cell_variables(cell_t *const c);

void reset_daily_layer_variables(cell_t *const c);
void reset_monthly_layer_variables(cell_t *const c);
void reset_annual_layer_variables(cell_t *const c);

void reset_daily_class_variables(cell_t *const c);
void reset_monthly_class_variables(cell_t *const c);
void reset_annual_class_variables(cell_t *const c);


#endif /* UTILITY_H */
