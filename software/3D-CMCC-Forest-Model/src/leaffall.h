/* leaffall.h */
#ifndef LEAFFALL_H_
#define LEAFFALL_H_

#include "matrix.h"

void Leaf_fall(species_t *const s, int* const doy);
void get_vpsat(cell_t *const c, const int day, const int month, const int year, const int index);

#endif /* LEAFFALL_H_ */
