/* utility.h */
#ifndef UTILITY_H_
#define UTILITY_H_

#include "matrix.h"

void First_day(cell_t *const c, const int layer, const int height, const age, const int species);
void Reset_annual_variables(cell_t *const c, const int layer, const int height, const age, const int species);
void Reset_monthly_variables(cell_t *const c, const int layer, const int height, const age, const int species);
void Reset_daily_variables(cell_t *const c, const int layer, const int height, const age, const int species);

#endif /* UTILITY_H */
