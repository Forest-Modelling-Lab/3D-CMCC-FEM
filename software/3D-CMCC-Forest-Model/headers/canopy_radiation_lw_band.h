/*
 * canopy_radiation_lw_band.h
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_RADIATION_LW_BAND_H_
#define HEADERS_CANOPY_RADIATION_LW_BAND_H_

#include "matrix.h"

void canopy_radiation_lw_band(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily);

#endif /* HEADERS_CANOPY_RADIATION_LW_BAND_H_ */
