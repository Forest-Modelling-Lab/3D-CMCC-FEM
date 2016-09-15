/*
 * renovation.h
 *
 *  Created on: 15 set 2016
 *      Author: alessio
 */

#ifndef HEADERS_RENOVATION_H_
#define HEADERS_RENOVATION_H_

#include "matrix.h"

void renovation(cell_t *const c, const int height, const int dbh, const int age, const int species);

void seeds_germination(cell_t *const c, const int height, const int dbh, const int age, const int species);

#endif /* HEADERS_RENOVATION_H_ */
