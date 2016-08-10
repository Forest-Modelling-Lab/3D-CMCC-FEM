/* structure.h */
#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "structure.h"

void Annual_Forest_structure(cell_t *const);
void daily_forest_structure (cell_t *const);
void Daily_check_for_veg_period (cell_t *const, const meteo_daily_t *const, const int, const int);
void Daily_numbers_of_layers (cell_t *const);
void Daily_layer_cover(cell_t *const, const meteo_daily_t *const);
void Daily_dominant_Light(cell_t *const, const int, const int, const int, const int);
void Daily_veg_counter(cell_t *const, species_t *const, const int);

#endif /* STRUCTURE_H_ */
