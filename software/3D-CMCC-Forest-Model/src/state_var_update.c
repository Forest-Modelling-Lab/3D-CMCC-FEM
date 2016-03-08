/*
 * state_var_update.c
 *
 *  Created on: 07/mar/2016
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Daily_water_state_update (CELL *const c)
{
	/* daily update of the water state variables */
	//Log("\nDAILY WATER UPDATE\n");



	/* precipitation fluxes */
	/*
	c->canopy_water        += c->prcp_to_canopy;
	c->prcp_sum_src        += c->prcp_to_canopy;
	c->asw                 += c->prcp_to_soil;
	c->prcp_sum_src        += c->prcp_to_soil;
	c->snowpack            += c->prcp_to_snow;
	c->prcp_sum_src        += c->prcp_to_snow;
	*/

	/* snowmelt fluxes */
	/*
	c->asw                 += c->snow_to_soil;
	c->snowpack            -= c->snow_to_soil;
	c->snowsubl_snk        += c->snow_subl;
	c->snowpack            -= c->snow_subl;
	*/

	/* bare soil evaporation */
	/*
	c->soilevap_snk        += c->soil_evap;
	c->asw                 -= c->soil_evap;
	*/

	/* canopy intercepted water fluxes */
	/*
	c->canopyevap_snk      += c->canopy_evap;
	c->canopy_water        -= c->canopy_evap;
	c->asw                 += c->canopy_to_soil;
	c->canopy_water        -= c->canopy_to_soil;
	*/

	/* transpiration */
	/*
	c->trans_snk           += c->soil_trans;
	c->asw                 -= c->soil_trans;
	*/

	/* outflow */
	/*
	c->runoff_snk          += c->soil_runoff;
	c->asw                 -= c->soil_runoff;
	*/

	/* the following special case prevents evaporation under very
	dry conditions from causing a negative soilwater content */
	//if (c->asw < 0.0)        /* negative soilwater */
	//{
		/* add back the evaporation and transpiration fluxes, and
		set these fluxes to 0.0 */
		//c->asw              += c->soil_evap;
		//c->soilevap_snk     -= c->soil_evap;
		//c->asw              += c->soil_trans;
		//c->trans_snk        -= c->soil_trans;
		//c->soil_evap = 0.0;
		//c->soil_trans = 0.0;

		/* test again for negative soilwater...should never be true */
		//if (c->asw < 0.0)
		//{
			//ERROR(c->asw, "AVAILABLE SOIL WATER");
		//}
	//}
}
