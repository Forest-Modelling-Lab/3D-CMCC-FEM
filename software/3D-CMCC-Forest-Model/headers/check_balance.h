/* check_balance.h */
#ifndef CHECK_BALANCE_H_
#define CHECK_BALANCE_H_

#include "matrix.h"

/* class level */
/* fluxes */
int check_class_radiation_flux_balance ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
int check_class_carbon_flux_balance    ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
int check_class_water_flux_balance     ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
/* mass */
int check_class_carbon_mass_balance    ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );

/* cell level */
/* fluxes */
int check_radiation_flux_balance       ( cell_t *const c, const meteo_daily_t *const meteo_daily );
int check_carbon_flux_balance          ( cell_t *const c );
int check_water_flux_balance           ( cell_t *const c, const meteo_daily_t *const meteo_daily );
/* mass */
int check_carbon_mass_balance          ( cell_t *const c );


#endif /* CHECK_BALANCE_H_ */
