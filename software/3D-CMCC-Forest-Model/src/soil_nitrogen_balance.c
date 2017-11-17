/*
 * soil_nitrogen_balance.c
 *
 *  Created on: 25 nov 2016
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "soil_model.h"

extern logger_t* g_debug_log;

void soil_nitrogen_balance (cell_t *const c)
{

	logger(g_debug_log, "\n**SOIL NITROGEN BALANCE**\n");

#if 0

	//fixme to include in cel struct as a sum of tree model NPP demand
	c->soilN -= c->NPP_gN_demand;

	//fixme to include in cell struct as a sum of soil nitrogen decomp
	c->soilN += c->N_decomp;

#endif



}
