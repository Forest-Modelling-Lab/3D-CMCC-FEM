/* lai.h */
#ifndef LAI_H_
#define LAI_H_

#include "matrix.h"

void specific_leaf_area ( const age_t *const a, species_t *const s );

void daily_lai          (cell_t *const c, species_t *const s);

#endif /* LAI_H_ */
