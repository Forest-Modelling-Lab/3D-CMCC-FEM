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

void Get_initialization_site_data (CELL *c)
{
	double vwc_fc; //Soil Field Capacity Volumetric Water Content in m3/m3

	Log("\n****GET SITE-SOIL CHARACTERISTICS for cell  (%d, %d)****\n", c->x, c->y);
	/*soil matric potential*/


	//control
	/*
	 if (site->sand_perc + site->clay_perc + site->silt_perc != 100)
	 {
	 	 Log("ERROR IN SOIL PERCENTAGE!!!!!\n");
	 	 exit;
	 }
	 */

	//see BIOME-BGC 4.2

	//FROM BIOME
	/* given a list of site constants and the soil water mass (kg/m2),
	this function returns the soil water potential (MPa)
	inputs:
	ws.soilw           (kg/m2) water mass per unit area
	sitec.soil_depth   (m)     effective soil depth
	sitec.soil_b       (DIM)   slope of log(psi) vs log(rwc)
	sitec.vwc_sat      (DIM)   volumetric water content at saturation
	sitec.psi_sat      (MPa)   soil matric potential at saturation
	output:
	psi_s              (MPa)   soil matric potential

	uses the relation:
	psi_s = psi_sat * (vwc/vwc_sat)^b

	For further discussion see:
	Cosby, B.J., G.M. Hornberger, R.B. Clapp, and T.R. Ginn, 1984.  A
	   statistical exploration of the relationships of soil moisture
	   characteristics to the physical properties of soils.  Water Res.
	   Res. 20:682-690.

	Saxton, K.E., W.J. Rawls, J.S. Romberger, and R.I. Papendick, 1986.
		Estimating generalized soil-water characteristics from texture.
		Soil Sci. Soc. Am. J. 50:1031-1036.
	*/

	/* calculate the soil pressure-volume coefficients from texture data */
	/* Uses the multivariate regressions from Cosby et al., 1984 */
	/* first check that the percentages add to 100.0 */
	//soil_b       (DIM)   slope of log(psi) vs log(rwc)
	//vwc_sat      (DIM)   volumetric water content at saturation
	//psi_sat      (MPa)   soil matric potential at saturation



	
	//double soilw_fc; //maximum volume soil water content in m3/m3

	// (DIM) Clapp-Hornberger "b" parameter
	c->soil_b = -(3.10 + 0.157*site->clay_perc - 0.003*site->sand_perc);
	Log ("soil_b = %f (DIM)\n", c->soil_b);
	// (DIM) Soil volumetric water content at saturation
	c->vwc_sat = (50.5 - 0.142*site->sand_perc - 0.037*site->clay_perc)/100.0;
	Log ("vwc_sat = %f (DIM)\n", c->vwc_sat);
	// (MPa) soil matric potential at saturation
	c->psi_sat = -(exp((1.54 - 0.0095*site->sand_perc + 0.0063*site->silt_perc)*log(10.0))*9.8e-5);
	Log ("psi_sat = %f (MPa)\n", c->psi_sat);
	// (DIM) Soil Field Capacity Volumetric Water Content at field capacity ( = -0.015 MPa)
	vwc_fc =  c->vwc_sat * pow((-0.015/c->psi_sat),1.0/c->soil_b);
	Log ("vwc_fc = %f (DIM)\n", vwc_fc);


	//todo definire quale determina il MAXASW
	// define maximum soilwater content, for outflow calculation
	//converts volumetric water content (m3/m3) --> (kg/m2)
	// (cm)   effective depth of rooting zone
	Log("Soil depth = %f cm\n", site->soil_depth);
	//Log("Soil depth = %f m\n", site->soil_depth / 100);
	// (kgH2O/m2) soil water at field capacity
	c->soilw_fc = (site->soil_depth / 100) * vwc_fc * 1000.0;
	Log ("soilw_fc BIOME (MAXASW FC BIOME)= %f (kgH2O/m2)\n", c->soilw_fc);
	//equal to MAXASW
	// (kgH2O/m2) soilwater at saturation
	c->soilw_sat = (site->soil_depth / 100) * c->vwc_sat * 1000.0;
	Log ("soilw_sat BIOME (MAXASW SAT BIOME)= %f (kgH2O/m2)\n", c->soilw_sat);

	c->max_asw = c->soilw_fc;

	//compute initialization soil water content
	c->available_soil_water = (c->soilw_fc * site->min_frac_maxasw);
	Log("Initialization ASW = %f (mm-kgH2O/m2)\n\n\n", c->available_soil_water);

	//snow initialization
	c->snow_pack = 0;
	//c->snow_subl = 0;

}

