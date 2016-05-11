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
	float acoeff;
	float bcoeff;
	float sat;

	int i;

	float volumetric_wilting_point;
	float volumetric_field_capacity;
	float volumetric_saturated_hydraulic_conductivity;

	Log("\n****SITE-SOIL CHARACTERISTICS for cell (%d, %d)****\n", c->x, c->y);
	// (cm)   effective depth of rooting zone
	Log("Soil depth = %f cm\n", site->soil_depth);

	/*soil matric potential*/
	CHECK_CONDITION(fabs((site->sand_perc + site->clay_perc + site->silt_perc) -100.0 ), > 1e-4);
	/*
	For further discussion see:
	Cosby, B.J., G.M. Hornberger, R.B. Clapp, and T.R. Ginn, 1984.  A
	   statistical exploration of the relationships of soil moisture
	   characteristics to the physical properties of soils.  Water Res.
	   Res. 20:682-690.

	Saxton, K.E., W.J. Rawls, J.S. Romberger, and R.I. Papendick, 1986.
		Estimating generalized soil-water characteristics from texture.
		Soil Sci. Soc. Am. J. 50:1031-1036.
	*/
	
	Log("BIOME soil characteristics\n");
	//double soilw_fc; //maximum volume soil water content in m3/m3
	// (DIM) Clapp-Hornberger "b" parameter
	c->soil_b = -(3.10 + 0.157*site->clay_perc - 0.003*site->sand_perc);
	Log ("soil_b = %f (DIM)\n", c->soil_b);
	// (DIM) Soil volumetric water content at saturation
	c->vwc_sat = (50.5 - 0.142*site->sand_perc - 0.037*site->clay_perc)/100.0;
	Log ("volumetric water content at saturation (BIOME) = %f %(vol)\n", c->vwc_sat);
	// (MPa) soil matric potential at saturation
	c->psi_sat = -(exp((1.54 - 0.0095*site->sand_perc + 0.0063*site->silt_perc)*log(10.0))*9.8e-5);
	Log ("psi_sat = %f MPa \n", c->psi_sat);
	// (DIM) Soil Field Capacity Volumetric Water Content at field capacity ( = -0.015 MPa)
	c->vwc_fc =  c->vwc_sat * pow((-0.015/c->psi_sat),(1.0/c->soil_b));
	Log ("volumetric water content at field capacity (BIOME) = %f %(vol) \n", c->vwc_fc);

	// define maximum soilwater content, for outflow calculation
	//converts volumetric water content (m3/m3) --> (kg/m2)

	// (kgH2O/m2) soil water at field capacity
	c->soilw_fc = (site->soil_depth / 100) * c->vwc_fc * 1000.0;
	Log ("soilw_fc BIOME (MAXASW FC BIOME)= %f (kgH2O/m2)\n", c->soilw_fc);
	//equal to MAXASW
	// (kgH2O/m2) soil water at saturation
	c->soilw_sat = (site->soil_depth / 100) * c->vwc_sat * 1000.0;
	Log ("soilw_sat BIOME (MAXASW SAT BIOME)= %f (kgH2O/m2)\n", c->soilw_sat);

	c->max_asw_fc = c->soilw_fc;

	//compute initialization soil water content
	c->asw = (c->soilw_sat * settings->init_frac_maxasw);
	Log("Initialization ASW = %f (mm-kgH2O/m2)\n\n\n", c->asw);

	//snow initialization
	c->snow_pack = 0.0;
	//c->snow_subl = 0;

	/* soil data from https://www.nrel.colostate.edu/projects/century/soilCalculatorHelp.htm */
	/* following Saxton et al 1986, 2006, 2008 */
	Log("CENTURY soil characteristics\n");
	acoeff = exp(-4.396 - 0.0715 * site->clay_perc - 4.88e-4 * pow(site->sand_perc,2) - 4.285e-5 * pow(site->sand_perc,2)*site->clay_perc);
	bcoeff = (-3.14 - 0.00222 * pow(site->clay_perc,2) - 3.484e-5 * pow(site->sand_perc,2) * site->clay_perc);
	sat = (0.332 - 7.251e-4 * site->sand_perc + 0.1276 * log10(site->clay_perc));

	/* volumetric percentage for wilting point */
	volumetric_wilting_point = pow((15.0/acoeff), (1.0/bcoeff));
	/* volumetric percentage for field capacity */
	volumetric_field_capacity = pow((0.333/acoeff),(1.0/bcoeff));
	/* volumetric percentage for saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity = exp((12.012 - 0.0755 * site->sand_perc) + (-3.895 + 0.03671 * site->sand_perc - 0.1103 * site->clay_perc + 8.7546e-4 * pow(site->clay_perc,2))/sat);
	/* bulk density g/cm3 */
	c->bulk_density = (1 - sat) * 2.65;

	/* corrections from Steve Del Grosso */
	/* volumetric percentage wilting point */
	volumetric_wilting_point += (-0.15 * volumetric_wilting_point);
	Log("*volumetric water content at wilting point (CENTURY) = %f %(vol)\n", volumetric_wilting_point);
	/* (kgH2O/m2) soil water at wilting point */
	c->wilting_point = (site->soil_depth / 100) * volumetric_wilting_point * 1000.0;
	Log("**Wilting point (CENTURY) = %f mm/m2\n", c->wilting_point);

	/* volumetric percentage field capacity */
	volumetric_field_capacity += (0.07 * volumetric_field_capacity);
	Log("*volumetric water content at field capacity (CENTURY) = %f %(vol)\n", volumetric_field_capacity);
	/* (kgH2O/m2) soil water at field capacity */
	c->field_capacity = (site->soil_depth / 100) * volumetric_field_capacity * 1000.0;
	Log("**Field capacity (CENTURY) = %f mm/m2\n", c->field_capacity);

	/* volumetric percentage saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity /= 1500.0;
	Log("*volumetric water content at saturated hydraulic conductance (CENTURY) = %f %(vol)\n", volumetric_saturated_hydraulic_conductivity);
	//fixme not clear what it is
	/* (kgH2O/m2) soil water at saturated hydraulic conductivity */
	c->sat_hydr_conduct = (site->soil_depth / 100) * volumetric_saturated_hydraulic_conductivity * 1000.0;
	Log("**Saturated hydraulic conductivity (CENTURY) = %f mm/m2\n", c->sat_hydr_conduct);

	/* bulk density g/cm3 */
	c->bulk_density += (-0.08 * c->bulk_density);
	Log("**Bulk density = %f g/cm^3\n", c->bulk_density);

	getchar();

	//test 5 may 2016
	for (i = 0; i < c->soils_count; i++)
	{
		c->soils[i].bulk_density = 0.0;
		c->soils[i].field_capacity = 0.0;
		c->soils[i].wilting_point = 0.0;
		//Log("i= %d and soils_count = %d\n", i, c->soils_count);
	}

}

