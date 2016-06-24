/* print.h */
#ifndef PRINT_H_
#define PRINT_H_

#include "matrix.h"

void Print_init_month_stand_data(cell_t* const c, const int month, const int year, const int height, const int age, const int species);
void Print_end_month_stand_data(cell_t* const c, const int year, const int height, const int age, const int species);
void Print_met_data(const meteo_t *const met, const int month, const int day);
#endif /* PRINT_H_ */