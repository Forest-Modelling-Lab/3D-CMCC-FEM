/*
 * initialization _site_data.c
 *
 *  Created on: 29/nov/2012
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

void Get_initialization_site_data (CELL *c, const MET_DATA *const met, int month)
{

	Log("****GET SITE-SOIL CHARACTERISTICS for cell  (%g, %g)****\n", c->x, c->y);
	//see BIOME-BGC 4.2
	//all percentage are converted in 100 %
	float sand = site->sand_perc * 100;
	float clay = site->clay_perc * 100;
	float silt = site->silt_perc * 100;

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



	float vwc_fc; //Soil Field Capacity Volumetric Water Content in m3/m3
	float soilw_fc; //maximum volume soil water content in m3/m3


	c->soil_b = -(3.10 + 0.157*clay - 0.003*sand);
	Log ("soil_b = %g\n", c->soil_b);
	c->vwc_sat = (50.5 - 0.142*sand - 0.037*clay)/100.0;
	Log ("vwc_sat = %g m^3m^-3\n", c->vwc_sat);
	c->psi_sat = -(exp((1.54 - 0.0095*sand + 0.0063*silt)*log(10.0))*9.8e-5);
	Log ("psi_sat = %g\n", c->psi_sat);
	vwc_fc =  c->vwc_sat * pow((-0.015/c->psi_sat),1.0/c->soil_b);
	Log ("vwc_fc = %g m^3m^-3\n", vwc_fc);

	/* define maximum soilwater content, for outflow calculation
	converts volumetric water content (m3/m3) --> (kg/m2) */
	soilw_fc = (site->soil_depth / 100) * vwc_fc * 1000.0;
	Log ("soilw_fc BIOME = %g kg m^-2\n", soilw_fc);
	//equal to MAXASW
	c->soilw_sat = (site->soil_depth / 100) * c->vwc_sat * 1000.0;
	Log ("soilw_sat BIOME (MAXASW BIOME)= %g mm / kg m^-2\n", c->soilw_sat);

	c->max_asw = c->soilw_sat;

	c->available_soil_water = (c->soilw_sat * site->init_frac_maxasw) + met[month].rain;
	Log("Beginning month  %d ASW = %g mm\n", month  + 1 , c->available_soil_water);


}

