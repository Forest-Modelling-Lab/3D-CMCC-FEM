/* print_output.h */
#ifndef PRINT_OUTPUT_H_
#define PRINT_OUTPUT_H_

#include "matrix.h"

void EOD_print_cumulative_balance_cell_level(cell_t *const, const int, const int, const int);

void EOM_print_cumulative_balance_cell_level(cell_t *const, const int, const int);

void EOY_print_cumulative_balance_cell_level(cell_t *const, const int);

void Get_EOD_soil_balance_cell_level(cell_t *const, const int, const int, const int);

#endif /* PRINT_OUTPUT_H_ */
