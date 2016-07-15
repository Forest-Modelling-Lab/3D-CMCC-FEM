/* cumulative_balance.h */
#ifndef CUMULATIVE_BALANCE_H_
#define CUMULATIVE_BALANCE_H_

#include "matrix.h"

void EOD_cumulative_balance_cell_level(cell_t *const c, const int years, const int month, const int day, const int cell_index);
void EOM_cumulative_balance_cell_level(cell_t *const c, const int years, const int month, const int cell_index);
void EOY_cumulative_balance_cell_level(cell_t *const c, const int year, const int years_of_simulation, const int cell_index);
void Get_EOD_soil_balance_cell_level(cell_t *const c, const int year, const int month, const int day);

#endif /* CUMULATIVE_BALANCE_H_ */
