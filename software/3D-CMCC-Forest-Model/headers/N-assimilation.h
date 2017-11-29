/* N-assimilation.h */
#ifndef N_ASSIMILATION_H_
#define N_ASSIMILATION_H_

#include "matrix.h" 

void nitrogen_assimilation(species_t *const s);

void potential_nitrogen_allocation( cell_t *const c, species_t *const s );

#endif /* N_ASSIMILATION_H_ */
