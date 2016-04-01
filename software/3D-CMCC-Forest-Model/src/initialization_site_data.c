/*
 * initialization _site_data.c
 *
 *  Created on: 29/nov/2012
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Initialization_site_data (CELL *c)
{
	Log("\n****SITE-SOIL CHARACTERISTICS for cell  (%d, %d)****\n", c->x, c->y);
	/*soil matric potential*/

	CHECK_CONDITION(fabs((site->sand_perc + site->clay_perc + site->silt_perc) -100.0 ), > 1e-4);
	
	//double soilw_fc; //maximum volume soil water content in m3/m3
	// (DIM) Clapp-Hornberger "b" parameter
	c->soil_b = -(3.10 + 0.157*site->clay_perc - 0.003*site->sand_perc);
	Log ("soil_b = %f (DIM)\n", c->soil_b);
	// (DIM) Soil volumetric water content at saturation
	c->vwc_sat = (50.5 - 0.142*site->sand_perc - 0.037*site->clay_perc)/100.0;
	Log ("vwc_sat = %f (DIM)\n", c->vwc_sat);
	// (MPa) soil matric potential at saturation
	c->psi_sat = -(exp((1.54 - 0.0095*site->sand_perc + 0.0063*site->silt_perc)*log(10.0))*9.8e-5);
	// (DIM) Soil Field Capacity Volumetric Water Content at field capacity ( = -0.015 MPa)
	c->vwc_fc =  c->vwc_sat * pow((-0.015/c->psi_sat),1.0/c->soil_b);


	//todo definire quale determina il MAXASW
	// define maximum soilwater content, for outflow calculation
	//converts volumetric water content (m3/m3) --> (kg/m2)
	// (cm)   effective depth of rooting zone
	Log("Soil depth = %f cm\n", site->soil_depth);
	//Log("Soil depth = %f m\n", site->soil_depth / 100);
	// (kgH2O/m2) soil water at field capacity
	c->soilw_fc = (site->soil_depth / 100) * c->vwc_fc * 1000.0;
	Log ("soilw_fc BIOME (MAXASW FC BIOME)= %f (kgH2O/m2)\n", c->soilw_fc);
	//equal to MAXASW
	// (kgH2O/m2) soilwater at saturation
	c->soilw_sat = (site->soil_depth / 100) * c->vwc_sat * 1000.0;
	Log ("soilw_sat BIOME (MAXASW SAT BIOME)= %f (kgH2O/m2)\n", c->soilw_sat);

	c->max_asw = c->soilw_fc;

	//compute initialization soil water content
	c->asw = (c->soilw_sat * site->min_frac_maxasw);
	Log("Initialization ASW = %f (mm-kgH2O/m2)\n\n\n", c->asw);

	//snow initialization
	c->snow_pack = 0;
	//c->snow_subl = 0;

}

