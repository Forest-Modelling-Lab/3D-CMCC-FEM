/*
 * soil_model.h
 *
 *  Created on: 31 ago 2016
 *      Author: alessio
 */

#ifndef HEADERS_SOIL_MODEL_H_
#define HEADERS_SOIL_MODEL_H_

#include "matrix.h"

int Soil_model(matrix_t *const m, const int cell, const int day, const int month, const int year);


#endif /* HEADERS_SOIL_MODEL_H_ */
