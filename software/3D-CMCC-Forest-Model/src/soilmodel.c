/*
 * soilmodel.c
 *
 *  Created on: 16/nov/2012
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"

//extern  const char *szMonth[MONTHS];
//extern int MonthLength[];
//extern int DaysInMonth[];


void soil_model(CELL *c, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	Log("--SOIL MODEL ROUTINE--\n");

	Log("*********GET MONTHLY SOIL WATER BALACE************\n");

	//from BIOME
	/* convert kg/m2 --> m3/m2 --> m3/m3 */
	/*
	vwc = soilw / (1000.0 * sitec->soil_depth);
	*/
	/* calculate psi */
	// *psi = sitec->psi_sat * pow((vwc/sitec->vwc_sat), sitec->soil_b);



}
