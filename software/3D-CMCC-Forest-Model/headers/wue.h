/* wue.h */
#ifndef WUE_H_
#define WUE_H_

#include "matrix.h"

// version 5.7 Saponaro Vincenzo
void water_use_efficiency (cell_t *const c, meteo_daily_t *meteo_daily, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year );

// old version until 5.6-6.606
//void water_use_efficiency_old( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year );

void cell_water_use_efficiency ( cell_t *const c, const int day, const int month, const int year );

#endif /* WUE_H_ */
