/* tree_model.h */
#ifndef TREE_MODEL_H_
#define TREE_MODEL_H_

#include "matrix.h"

int Tree_model(matrix_t *const m, const int cell, /*const int halfhour, const int hour,*/ const int day, const int month, const int year);

int tree_class_remove(cell_t *const c, const int height, const int dbh, const int age, const int species);

#endif /* TREE_MODEL_H_ */
