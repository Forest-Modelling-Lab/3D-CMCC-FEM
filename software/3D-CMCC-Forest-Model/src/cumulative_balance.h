/* cumulative_balance.h */
#ifndef CUMULATIVE_BALANCE_H_
#define CUMULATIVE_BALANCE_H_

#include "types.h"

void EOY_cumulative_balance_layer_level (SPECIES *, HEIGHT *);
void EOD_cumulative_balance_cell_level (CELL *, const YOS *const , int, int, int, const int cell_index);
void EOM_cumulative_balance_cell_level (CELL *, const YOS *const , int, int, const int cell_index);
void EOY_cumulative_balance_cell_level (MATRIX *, CELL *, const YOS *const , int, int, const int cell_index);

#endif /* CUMULATIVE_BALANCE_H_ */
