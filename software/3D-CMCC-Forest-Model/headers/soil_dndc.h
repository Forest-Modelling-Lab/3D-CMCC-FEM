/* soil_dndc.h */
#ifndef SOILDNDC_H_
#define SOILDNDC_H_

#include "matrix.h"

void soil_dndc_sgm(matrix_t *const m, const int cell, const int day, const int month, const int year, const int years_of_simulation);

#endif /* SOILDNDC_H_ */
