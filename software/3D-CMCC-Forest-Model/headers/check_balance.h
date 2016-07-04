/* check_balance.h */
#ifndef CHECK_BALANCE_H_
#define CHECK_BALANCE_H_

#include "matrix.h"

/* class level */
void Check_class_radiation_balance(cell_t *const c, species_t* const s);
void Check_class_carbon_balance(cell_t *const c, species_t* const s);
void Check_class_water_balance(cell_t *const c, species_t* const s);

/* cell level */
void Check_radiation_balance (cell_t *const c, const int day, const int month, const int year);
void Check_carbon_balance(cell_t *const c);
void Check_soil_water_balance(cell_t *const c);

void New_Check_carbon_balance (cell_t *const c, species_t* const s);
void leaf_balance (cell_t *const c, species_t* const s);

#endif /* CHECK_BALANCE_H_ */
