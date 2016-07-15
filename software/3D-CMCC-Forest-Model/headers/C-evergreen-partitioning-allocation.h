/* C-evergreen-partitioning-allocation.h */
#ifndef C_EVERGREEN_PARTITIONING_ALLOCATION_H_
#define C_EVERGREEN_PARTITIONING_ALLOCATION_H_

#include "matrix.h"

void Daily_C_Evergreen_Partitioning_Allocation(cell_t *const c, const int layer, const int height, const int age
		, const int species, const meteo_daily_t *const meteo_daily, const int day, const int year);

#endif /* C_EVERGREEN_PARTITIONING_ALLOCATION_H_ */
