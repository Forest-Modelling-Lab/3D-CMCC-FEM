/*
 * canopy_sensible_heat.h
 *
 *  Created on: 24 giu 2016
 *      Author: alessio
 */

#ifndef SRC_CANOPY_SENSIBLE_HEAT_H_
#define SRC_CANOPY_SENSIBLE_HEAT_H_

#include "matrix.h"

void canopy_sensible_heat(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily);

#endif /* SRC_CANOPY_SENSIBLE_HEAT_H_ */
