/* treemodel_daily.h */
#ifndef TREEMODEL_DAILY_H_
#define TREEMODEL_DAILY_H_

#include "matrix.h"

int Tree_model_daily(matrix_t *const m, const int cell, const int day, const int month, const int year);

int tree_class_remove(cell_t *const c, const int height, const int dbh, const int age, const int species);

#endif /* TREEMODEL_DAILY_H_ */
