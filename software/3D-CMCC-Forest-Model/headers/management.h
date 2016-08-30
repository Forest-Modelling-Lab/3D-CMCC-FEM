/*
 * management.h
 *
 *  Created on: 18/ago/2016
 *      Author: alessio-cmcc
 */

#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#include "matrix.h"

void forest_management (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int year);

void choose_management(species_t *const s, const int years);

void clearcut_timber_upon_request(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species);

void clearcut_timber_without_request (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int year);

void clearcut_coppice(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species);

void Management (age_t * const a, const int height, const int dbh, const int age, const int species, int years);

#endif /* MANAGEMENT_H_ */
