/*
 * canopy_radiation.h
 *
 *  Created on: 05 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_RADIATION_H_
#define HEADERS_CANOPY_RADIATION_H_

#include "matrix.h"

void new_canopy_abs_transm_refl_radiation(cell_t *const c, species_t *const s, double LightAbsorb_sun, double LightAbsorb_shade, double LightReflec_par, double LightReflec_net_rad);
void new_canopy_radiation(species_t *const s, cell_t *const c, const meteo_t *const met, const int year, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species);

#endif /* HEADERS_CANOPY_RADIATION_H_ */
