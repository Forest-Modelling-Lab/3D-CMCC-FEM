/*
 * canopy_interception.h
 *
 *  Created on: 19/ago/2016
 *      Author: alessio-cmcc
 */

#ifndef CANOPY_INTERCEPTION_H_
#define CANOPY_INTERCEPTION_H_

#include "matrix.h"

void canopy_interception(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily);

#endif /* CANOPY_INTERCEPTION_H_ */
