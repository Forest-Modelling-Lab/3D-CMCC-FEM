/* light.h */
#ifndef LIGHT_H_
#define LIGHT_H_

#include "matrix.h"

void Radiation(species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species);

#endif /* LIGHT_H_ */
