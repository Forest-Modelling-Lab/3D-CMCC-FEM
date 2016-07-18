/* leaffall.h */
#ifndef LEAFFALL_H_
#define LEAFFALL_H_

#include "matrix.h"

void Leaf_fall(cell_t *const c, const int height, const int age, const int species);
void get_vpsat(cell_t *const c, const int day, const int month, const int year, const int index);

#endif /* LEAFFALL_H_ */
