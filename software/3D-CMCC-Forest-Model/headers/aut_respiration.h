/* aut_respiration.h */
#ifndef AUT_RESPIRATION_H_
#define AUT_RESPIRATION_H_

#include "matrix.h"

void Maintenance_respiration(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_t *const met, const int day, const int month);
void Growth_respiration(cell_t *const c, const int layer, const int height, const int age, const int species);
void Autotrophic_respiration(cell_t *const c, const int layer, const int height, const int age, const int species);

#endif /* AUT_RESPIRATION_H_ */