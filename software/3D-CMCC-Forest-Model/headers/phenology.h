/* phenology.h */
#ifndef PHENOLOGY_H_
#define PHENOLOGY_H_

#include "matrix.h"

void simple_phenology_phase (species_t *const s, const meteo_t* const met, const int year, const int month, const int day);

#endif /* PHENOLOGY_H_ */