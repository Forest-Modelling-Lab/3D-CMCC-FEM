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

	//FIXME ADD MINERALIZATION AND PLANTS DEMAND

	/*********************************************************************************************************/

	/* update nitrogen fluxes balance of fast soil microbial recycling pool */
	c->daily_to_soil1N          = c->daily_litr1N_to_soil1N
			- ( c->daily_soil1N_to_soil2N );

	/* update nitrogen mass of fast soil microbial pool */
	c->soil1N                  += c->daily_to_soil1N;
	/*********************************************************************************************************/

	/* update nitrogen fluxes balance of medium soil microbial recycling pool */
	c->daily_to_soil2N          = ( c->daily_litr2N_to_soil2N + c->daily_soil1N_to_soil2N )
			- (  c->daily_soil2N_to_soil3N );

	/* update nitrogen mass of medium soil microbial pool */
	c->soil2N                  += c->daily_to_soil2N;
	/*********************************************************************************************************/

	/* update nitrogen fluxes balance of slow soil microbial recycling pool */
	c->daily_to_soil3N          = ( c->daily_litr4N_to_soil3N + c->daily_soil2N_to_soil3N )
			- ( c->daily_soil3N_to_soil4N );

	/* update nitrogen mass of slow soil microbial pool */
	c->soil3N                  += c->daily_to_soil3N;
	/*********************************************************************************************************/

	/* update nitrogen fluxes balance of recalcitrant SOM pool */
	c->daily_to_soil4N          = ( c->daily_soil3N_to_soil4N )
			- ( c->daily_soil4N_to_soilMinN);

	/* update nitrogen mass of recalcitrant SOM pool */
	c->soil4N                  += c->daily_to_soil4N;
	/*********************************************************************************************************/

	c->daily_to_soilN           = c->daily_litr1N_to_soil1N +
			c->daily_litr2N_to_soil2N +
			c->daily_litr4N_to_soil3N ;

	/* total mass */
	c->soilN                    = c->soil1N + c->soil2N + c->soil3N + c->soil4N;

#if 0

	//fixme to include in cel struct as a sum of tree model NPP demand
	c->soilN -= c->NPP_gN_demand;


#endif



}
