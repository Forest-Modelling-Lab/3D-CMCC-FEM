/* utility.h */
#ifndef UTILITY_H_
#define UTILITY_H_

#include "matrix.h"

void First_day(cell_t *const c, int layer, int height, int age, int species);
void Reset_annual_variables(cell_t *const c, int layer, int height, int age, int species);
void Reset_monthly_variables(cell_t *const c, int layer, int height, int age, int species);
void Reset_daily_variables(cell_t *const c, int layer, int height, int age, int species);

#endif /* UTILITY_H */
