/*
 * het_respiration.h
 *
 *  Created on: 15 mar 2017
 *      Author: alessio
 */


#ifndef HET_RESPIRATION_H_
#define HET_RESPIRATION_H_

#include "matrix.h"

void heterotrophic_respiration(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily);

#endif /* HET_RESPIRATION_H_ */
