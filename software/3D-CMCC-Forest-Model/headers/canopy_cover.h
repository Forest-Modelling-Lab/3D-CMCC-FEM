/*
 * canopy_cover.h
 *
 *  Created on: 02/mag/2017
 *      Author: alessio-cmcc
 */

#ifndef CANOPY_COVER_H_
#define CANOPY_COVER_H_

void dbhdc_function (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int year);

void canopy_cover_projected (cell_t *const c, const int height, const int dbh, const int age, const int species);

#endif /* CANOPY_COVER_H_ */
