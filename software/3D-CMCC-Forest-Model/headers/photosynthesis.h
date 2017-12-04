/* photosynthesis.h */
#ifndef PHOTOSYNTHESIS_H_
#define PHOTOSYNTHESIS_H_

#include "matrix.h"

void photosynthesis (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_annual_t *const meteo_annual);

void total_photosynthesis_biome (cell_t *const c, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual);

double Farquhar (cell_t *const c, species_t *const s, const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual, const double cond_corr, const double leafN, const double ppfd, const double leaf_day_mresp );


#endif /* PHOTOSYNTHESIS_H_ */
