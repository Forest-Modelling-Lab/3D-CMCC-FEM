/* cropmodel_daily.h */
#ifndef CROPMODEL_DAILY_H_
#define CROPMODEL_DAILY_H_

#include "matrix.h"

int crop_model_D(matrix_t *const m, const int cell, const int year, const int month, const int day, const int years_of_simulation);

#endif /* CROPMODEL_DAILY_H_ */
