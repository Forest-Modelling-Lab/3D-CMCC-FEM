/* print.h */
#ifndef PRINT_H_
#define PRINT_H_

#include "matrix.h"

void Print_parameters(species_t *const s, const int species_count, const int day, const int month, const int years);

void Print_stand_data(cell_t* const c, const int layer, const int height, const int age, const int species);

void Print_daily_met_data(const meteo_daily_t *const meteo_daily, const int day, const int month);

#endif /* PRINT_H_ */
