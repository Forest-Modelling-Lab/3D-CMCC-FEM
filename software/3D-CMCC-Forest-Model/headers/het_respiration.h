/*
 * het_respiration.h
 *
 *  Created on: 15 mar 2017
 *      Author: alessio
 */


#ifndef HET_RESPIRATION_H_
#define HET_RESPIRATION_H_

#include "matrix.h"

void heterotrophic_respiration_reichstein(cell_t *const c);

void litter_heterotrophic_respiration_biome (cell_t *const c, const meteo_daily_t *const meteo_daily);

void soil_heterotrophic_respiration_biome (cell_t *const c, const meteo_daily_t *const meteo_daily);

double heterotrophic_respiration_biome (cell_t *const c, const meteo_daily_t *const meteo_daily);

void heterotrophic_respiration (cell_t *const c, const meteo_daily_t *const meteo_daily);


#endif /* HET_RESPIRATION_H_ */
