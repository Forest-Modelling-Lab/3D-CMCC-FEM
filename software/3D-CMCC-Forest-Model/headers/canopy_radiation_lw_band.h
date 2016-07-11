/*
 * canopy_radiation_lw_band.h
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_RADIATION_LW_BAND_H_
#define HEADERS_CANOPY_RADIATION_LW_BAND_H_

#include "matrix.h"

void canopy_radiation_lw_band (species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species);

#endif /* HEADERS_CANOPY_RADIATION_LW_BAND_H_ */
