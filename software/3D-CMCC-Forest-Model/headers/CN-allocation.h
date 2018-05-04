/*
 * CN-allocation.h
 *
 *  Created on: 01 lug 2016
 *      Author: alessio
 */

#ifndef CN_ALLOCATION_H_
#define CN_ALLOCATION_H_

#include "matrix.h"

void carbon_allocation   ( cell_t *const c, age_t *const a, species_t *const s, const int day, const int month, const int year );

void nitrogen_allocation ( cell_t *const c, species_t *const s, const int day, const int month, const int year );


#endif /*CN_ALLOCATION_H_ */
