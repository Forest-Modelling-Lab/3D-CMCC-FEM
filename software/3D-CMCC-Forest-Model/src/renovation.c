/*
 * renovation.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void seeds_germination (cell_t *const c, const int height, const int dbh, const int age, const int species)
{

}

void renovation (cell_t *const c, const int height, const int dbh, const int age, const int species)
{

	species_t *s;

	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	seeds_germination ( c, height, dbh, age, species);


}




