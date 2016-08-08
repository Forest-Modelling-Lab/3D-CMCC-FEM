/* structure.h */
#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "structure.h"

void Annual_Forest_structure(cell_t *const c);
void Daily_Forest_structure (cell_t *const c, const int day, const int month, const int year);
void Daily_check_for_veg_period (cell_t *const c, const meteo_daily_t *const meteo_daily, const int day, const int month);
void Daily_numbers_of_layers (cell_t *const c);
void Daily_layer_cover(cell_t *const c, const meteo_daily_t *const met);
void Daily_dominant_Light(cell_t *const c, const int layer, const int height, const int age, const int species);
void Daily_veg_counter(cell_t *const c, species_t *const s, const int height);

#endif /* STRUCTURE_H_ */
