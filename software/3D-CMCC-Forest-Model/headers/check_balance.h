/* check_balance.h */
#ifndef CHECK_BALANCE_H_
#define CHECK_BALANCE_H_

#include "matrix.h"

/** tree class level **/
/* fluxes */
int check_tree_class_radiation_flux_balance ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
int check_tree_class_carbon_flux_balance    ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
int check_tree_class_nitrogen_flux_balance  ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species) ;
int check_tree_class_water_flux_balance     ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
/* mass */
int check_tree_class_carbon_mass_balance    ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );
int check_tree_class_nitrogen_mass_balance  ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species );

/** litter level **/
/* fluxes */
int check_litter_carbon_flux_balance        ( cell_t *const c );
int check_litter_carbon_mass_balance        ( cell_t *const c );
/* mass */
int check_litter_nitrogen_flux_balance      ( cell_t *const c );
int check_litter_nitrogen_mass_balance      ( cell_t *const c );

/** soil level **/
/* fluxes */
int check_soil_radiation_flux_balance       ( cell_t *const c, const meteo_daily_t *const meteo_daily );
int check_soil_carbon_flux_balance          ( cell_t *const c );
int check_soil_nitrogen_flux_balance        ( cell_t *const c );
int check_soil_water_flux_balance           ( cell_t *const c, const meteo_daily_t *const meteo_daily );
/* mass */
int check_soil_carbon_mass_balance          ( cell_t *const c );
int check_soil_nitrogen_mass_balance        ( cell_t *const c );

/** cell level **/
/* fluxes */
int check_cell_radiation_flux_balance       ( cell_t *const c, const meteo_daily_t *const meteo_daily );
int check_cell_carbon_flux_balance          ( cell_t *const c );
int check_cell_nitrogen_flux_balance        ( cell_t *const c, const meteo_daily_t *const meteo_daily );
int check_cell_water_flux_balance           ( cell_t *const c, const meteo_daily_t *const meteo_daily );
/* mass */
int check_cell_carbon_mass_balance          ( cell_t *const c );
int check_cell_nitrogen_mass_balance        ( cell_t *const c, const meteo_daily_t *const meteo_daily );


#endif /* CHECK_BALANCE_H_ */
