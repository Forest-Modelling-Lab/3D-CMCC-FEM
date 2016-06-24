/*
 * canopy_sensible_heat.h
 *
 *  Created on: 24 giu 2016
 *      Author: alessio
 */

#ifndef SRC_CANOPY_SENSIBLE_HEAT_H_
#define SRC_CANOPY_SENSIBLE_HEAT_H_

#include "matrix.h"

void canopy_sensible_heat (species_t *const s, cell_t *const c, const meteo_t *const met, int month, int day, int height, int age, int species);

#endif /* SRC_CANOPY_SENSIBLE_HEAT_H_ */
