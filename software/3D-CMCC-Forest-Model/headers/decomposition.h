/*
 * decomposition.h
 *
 *  Created on: 13 mar 2017
 *      Author: alessio
 */

#ifndef HEADERS_DECOMPOSITION_H_
#define HEADERS_DECOMPOSITION_H_

double decomposition      (cell_t *const c, const meteo_daily_t *const meteo_daily, int flag);

void litter_decomposition (cell_t *const c, const meteo_daily_t *const meteo_daily);

void soil_decomposition   (cell_t *const c, const meteo_daily_t *const meteo_daily);

#endif /* HEADERS_DECOMPOSITION_H_ */
