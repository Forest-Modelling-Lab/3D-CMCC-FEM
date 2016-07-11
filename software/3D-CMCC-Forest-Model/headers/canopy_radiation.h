/*
 * canopy_radiation.h
 *
 *  Created on: 05 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_RADIATION_H_
#define HEADERS_CANOPY_RADIATION_H_

#include "matrix.h"

void canopy_sw_band_abs_trans_refl_radiation(cell_t *const c, species_t *const s, double Light_abs_frac, double Light_abs_frac_sun, double Light_abs_frac_shade, double Light_refl_par_frac, double Light_refl_sw_rad_canopy_frac);
void canopy_radiation(species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species);

#endif /* HEADERS_CANOPY_RADIATION_H_ */
