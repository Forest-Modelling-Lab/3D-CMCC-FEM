/* print_output.h */
#ifndef PRINT_OUTPUT_H_
#define PRINT_OUTPUT_H_

#include "matrix.h"
#include "logger.h"


void EOD_print_cumulative_balance_cell_level(cell_t *const, const int, const int, const int, const int);

void EOM_print_cumulative_balance_cell_level(cell_t *const, const int, const int, const int);

void EOY_print_cumulative_balance_cell_level(cell_t *const, const int, const int);

void print_model_paths(logger_t *const _log);

void print_model_settings(logger_t*const log);

#endif /* PRINT_OUTPUT_H_ */
