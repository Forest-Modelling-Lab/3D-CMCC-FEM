/*
 * canopy_net_radiation.h
 *
 *  Created on: 12 lug 2016
 *      Author: alessio
 */

#ifndef HEADERS_CANOPY_NET_RADIATION_H_
#define HEADERS_CANOPY_NET_RADIATION_H_

#include "matrix.h"

void canopy_net_radiation(species_t *const s, cell_t *const c, const meteo_t *const met, const int day, const int month, const int year, const int height, const int age, const int species);

#endif /* HEADERS_CANOPY_NET_RADIATION_H_ */
