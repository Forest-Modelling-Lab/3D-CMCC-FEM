/* photosynthesis.h */
#ifndef PHOTOSYNTHESIS_H_
#define PHOTOSYNTHESIS_H_

#include "matrix.h"

void photosynthesis(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int DaysInMonth, const yos_t *const meteo_annual);

#endif /* PHOTOSYNTHESIS_H_ */
