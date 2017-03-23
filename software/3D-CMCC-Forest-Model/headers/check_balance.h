/* check_balance.h */
#ifndef CHECK_BALANCE_H_
#define CHECK_BALANCE_H_

#include "matrix.h"

/* class level */
int check_class_radiation_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species);
int check_class_carbon_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species);
int check_class_water_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species);

/* cell level */
int check_radiation_balance (cell_t *const c, const meteo_daily_t *const meteo_daily);
int check_carbon_balance(cell_t *const c);
int check_soil_water_balance(cell_t *const c, const meteo_daily_t *const meteo_daily);

void New_Check_carbon_balance (cell_t *const c, species_t* const s);
void leaf_balance (cell_t *const c, species_t* const s);

#endif /* CHECK_BALANCE_H_ */
