/* Penman_Monteith.h */
#ifndef PENMAN_MONTEITH_H_
#define PENMAN_MONTEITH_H_

#include "matrix.h"

double Penman_Monteith(const meteo_t *const met, const int month, const int day, const double rh, const double rv, const double net_rad);

#endif /* PENMAN_MONTEITH_H_ */