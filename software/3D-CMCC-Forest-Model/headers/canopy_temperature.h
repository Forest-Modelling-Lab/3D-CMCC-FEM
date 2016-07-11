/*
 * canopy_temperature.h
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_TEMPERATURE_H_
#define HEADERS_CANOPY_TEMPERATURE_H_

#include "matrix.h"

double canopy_temperature (species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int year);

#endif /* HEADERS_CANOPY_TEMPERATURE_H_ */
