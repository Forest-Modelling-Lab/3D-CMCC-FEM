/* check_balance.h */
#ifndef CHECK_BALANCE_H_
#define CHECK_BALANCE_H_

#include "matrix.h"

void Check_class_carbon_balance(cell_t *const c, species_t* const s);
void Check_class_water_balance(cell_t *const c, species_t* const s);
void Check_carbon_balance(cell_t *const c);
void Check_soil_water_balance(cell_t *const c);

void Check_class_biomass_balance(cell_t *const c, species_t* const s);

#endif /* CHECK_BALANCE_H_ */
