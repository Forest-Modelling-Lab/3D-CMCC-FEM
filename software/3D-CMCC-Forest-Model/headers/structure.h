/* structure.h */
#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "structure.h"

void Annual_numbers_of_layers(cell_t *const c);
void Daily_Forest_structure (cell_t *const c, const int day, const int month, const int year);
void Daily_vegetative_period (cell_t *const c, const meteo_t *const met, const int month, const int day);
void Daily_numbers_of_layers (cell_t *const c);
void Daily_layer_cover(cell_t *const c, const meteo_t *const met, const int month, const int day);
void Daily_dominant_Light(height_t *const heights, cell_t *const c, const int count, const meteo_t *const met, const int month, const int DaysInMonth);
void Daily_veg_counter(cell_t *const c, species_t *const s, const int height);

#endif /* STRUCTURE_H_ */
