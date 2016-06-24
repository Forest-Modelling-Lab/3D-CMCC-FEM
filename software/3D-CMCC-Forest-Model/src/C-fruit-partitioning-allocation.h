/*
 * C-fruit-partitioning-allocatio.h
 *
 *  Created on: 24 giu 2016
 *      Author: alessio
 */

#ifndef SRC_C_FRUIT_PARTITIONING_ALLOCATION_H_
#define SRC_C_FRUIT_PARTITIONING_ALLOCATION_H_

#include "matrix.h"

void Daily_C_Evergreen_Partitioning_Allocation (species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month,
		const int years, const int DaysInMonth, const int height, const int age, const int species);

#endif /* SRC_C_FRUIT_PARTITIONING_ALLOCATION_H_ */
