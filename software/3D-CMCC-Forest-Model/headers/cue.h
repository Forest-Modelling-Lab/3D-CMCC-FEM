/*
 * cue.h
 *
 *  Created on: 25/dic/2016
 *      Author: alessio-cmcc
 */

#ifndef CUE_H_
#define CUE_H_

void carbon_use_efficiency ( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year );
void biomass_production_efficiency ( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year );

#endif /* CUE_H_ */
