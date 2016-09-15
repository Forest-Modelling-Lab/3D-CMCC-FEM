/* g-function.h */
#ifndef G_FUNCTION_H_
#define G_FUNCTION_H_

#include "matrix.h"

int sort_by_years(const void *a, const void *b);

int sort_by_heights_asc(const void* a, const void* b);

int sort_by_heights_desc(const void * a, const void * b);

void carbon_pool_fraction(cell_t *const c);

void Veg_Days(cell_t *const c, const int day, const int month, const int year);

#endif /* G_FUNCTION_H_ */
