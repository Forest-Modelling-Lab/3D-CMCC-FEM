/* structure.h */
#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "structure.h"

void daily_forest_structure (cell_t *const c, const int day, const int month, const int year);

void monthly_forest_structure (cell_t* const c, const int day, const int month, const int year);

void annual_forest_structure(cell_t *const);

void potential_max_min_canopy_cover (cell_t *const c);

void daily_check_for_veg_period (cell_t *const, const meteo_daily_t *const, const int, const int);


#endif /* STRUCTURE_H_ */
