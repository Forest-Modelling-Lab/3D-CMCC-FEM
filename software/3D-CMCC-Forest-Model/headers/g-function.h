/* g-function.h */
#ifndef G_FUNCTION_H_
#define G_FUNCTION_H_

#include "matrix.h"

int sort_by_years(const void *a, const void *b);
int sort_by_heights_asc(const void* a, const void* b);
int sort_by_heights_desc(const void * a, const void * b);
int sort_by_layers_asc(const void* a, const void* b);
int sort_by_layers_desc(const void * a, const void * b);

void Pool_fraction(species_t *const s);
void Tree_period(cell_t* const c, const int layer, const int height, const int age, const int species);
void Veg_Days(cell_t *const c, const int day, const int month, const int year);

#endif /* G_FUNCTION_H_ */
