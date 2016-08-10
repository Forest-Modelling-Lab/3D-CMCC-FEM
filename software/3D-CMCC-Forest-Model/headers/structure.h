/* structure.h */
#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "structure.h"

void annual_forest_structure(cell_t *const);
void daily_forest_structure (cell_t *const);
void daily_check_for_veg_period (cell_t *const, const meteo_daily_t *const, const int, const int);
void daily_forest_structure_in_veg (cell_t *const);
void daily_layer_cover(cell_t *const, const meteo_daily_t *const);
void daily_dominant_light(cell_t *const, const int, const int, const int, const int);
void daily_veg_counter(cell_t *const, species_t *const, const int);

#endif /* STRUCTURE_H_ */
