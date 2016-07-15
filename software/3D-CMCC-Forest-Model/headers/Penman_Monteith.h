/* Penman_Monteith.h */
#ifndef PENMAN_MONTEITH_H_
#define PENMAN_MONTEITH_H_

#include "matrix.h"

double Penman_Monteith(const meteo_daily_t *const meteo_daily, const double rh, const double rv, const double net_rad);

#endif /* PENMAN_MONTEITH_H_ */