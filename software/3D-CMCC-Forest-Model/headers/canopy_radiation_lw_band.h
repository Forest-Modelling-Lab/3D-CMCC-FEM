/*
 * canopy_radiation_lw_band.h
 *
 *  Created on: 11 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_RADIATION_LW_BAND_H_
#define HEADERS_CANOPY_RADIATION_LW_BAND_H_

#include "matrix.h"

void canopy_radiation_lw_band(species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int year, const int height, const int age, const int species);
void canopy_lw_band_emit_trans_refl_radiation (cell_t *const c, species_t *const s, double Light_abs_frac, double Light_abs_frac_sun, double Light_abs_frac_shade, double Light_refl_par_frac, double Light_refl_sw_rad_canopy_frac);

#endif /* HEADERS_CANOPY_RADIATION_LW_BAND_H_ */
