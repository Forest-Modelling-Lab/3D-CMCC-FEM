/*
 * CN-allocation.h
 *
 *  Created on: 01 lug 2016
 *      Author: alessio
 */

#ifndef CN_ALLOCATION_H_
#define CN_ALLOCATION_H_

#include "matrix.h"

void carbon_allocation( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month );

void nitrogen_allocation( cell_t *const c, species_t *const s );

#endif /*CN_ALLOCATION_H_ */
