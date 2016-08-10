/* mortality.h */
#ifndef MORTALITY_H_
#define MORTALITY_H_

#include "matrix.h"

void Crowding_competition_mortality(cell_t *const, const int);
void Stool_mortality(cell_t *const, const int, const int, const int, const int);

#endif /* MORTALITY_H_ */
