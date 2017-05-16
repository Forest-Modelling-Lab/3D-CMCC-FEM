/*
 * canopy_cover.c
 *
 *  Created on: 02/mag/2017
 *      Author: alessio-cmcc
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "common.h"
#include "g-function.h"
#include "settings.h"
#include "logger.h"
#include "mortality.h"
#include "tree_model.h"
#include "remove_tree_class.h"
#include "biomass.h"
#include "canopy_cover.h"
#include "allometry.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;


void dbhdc_function (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const int year)
{
	double temp_crown_area     = 0.;
	double temp_crown_radius   = 0.;
	double temp_crown_diameter = 0.;
	double previous_dbhdc_eff  = 0.;
	double max_dbhdc_incr      = 0.1;               /* fraction of maximum dbhdc increment */

	dbh_t *d;
	species_t *s;
	d = &c->heights[height].dbhs[dbh];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log,"\n*DBHDC FUNCTION*\n");

	/************************************************************************************************************************/
	/* note: 04 Oct 2016 still USEFULL ?????*/
	/* compute potential maximum and minimum density for DBHDC function */
	//potential_max_min_density ( c );
	/* compute effective dbh/crown diameter */
	/*
		s->value[DBHDC_EFF] = ((s->value[DBHDCMIN] - s->value[DBHDCMAX]) / (s->value[DENMAX] - s->value[DENMIN]) *
				(c->tree_layers[layer].layer_density - s->value[DENMIN]) + s->value[DBHDCMAX]);
		logger(g_debug_log,"-DENMAX = %f\n", s->value[DENMAX]);
		logger(g_debug_log,"-DENMIN = %f\n", s->value[DENMIN]);
		logger(g_debug_log,"-DBHDCMAX = %f\n", s->value[DBHDCMAX]);
		logger(g_debug_log,"-DBHDCMIN = %f\n", s->value[DBHDCMIN]);
		logger(g_debug_log,"-DBHDC effective = %f\n", s->value[DBHDC_EFF]);
	 */

	/************************************************************************************************************************/
	/* note: 04 Oct 2016 */
	/* compute potential maximum and minimum density for DBHDC function */
	/* new DBHDC function */
	/* this function in mainly based on the assumptions that trees tend to occupy */
	/* all space they can, if they cannot then fixed values constrain their crown */
	/* see also: Lhotka and Loewenstein, 1997; Lhotka and Loewenstein, 2008 */

	/* previous dbhdc eff */
	previous_dbhdc_eff = s->value[DBHDC_EFF];

	temp_crown_area = (g_settings->sizeCell * g_settings->max_layer_cover) / (c->tree_layers[layer].layer_density * g_settings->sizeCell);

	temp_crown_radius = sqrt(temp_crown_area / Pi);

	temp_crown_diameter = temp_crown_radius * 2.;

	s->value[DBHDC_EFF] = temp_crown_diameter / d->value;

	/* check if current dbhdc_eff grows to much (case when there's thinning) */
	/* this is checked to avoid unrealistic crown area increment */

	//note max_dbhdc_incr corresponds to an arbitrary increment of n value
	//note: not used in the first year of simulation
	if ( ( s->counter[YOS] ) && ( s->value[DBHDC_EFF] > ( previous_dbhdc_eff + (previous_dbhdc_eff * max_dbhdc_incr ) ) ) )
	{
		s->value[DBHDC_EFF] = previous_dbhdc_eff + ( previous_dbhdc_eff * max_dbhdc_incr );
	}

	/************************************************************************************************************************/
	/* check */
	if (s->value[DBHDC_EFF] > s->value[DBHDCMAX])
	{
		logger(g_debug_log,"-DBHDC effective (%f) > DBHDCMAX (%f) \n", s->value[DBHDC_EFF] , s->value[DBHDCMAX]);
		s->value[DBHDC_EFF] = s->value[DBHDCMAX];
		logger(g_debug_log,"-DBHDC effective = %f\n", s->value[DBHDC_EFF]);
	}

	logger(g_debug_log,"-DBHDC effective = %f\n", s->value[DBHDC_EFF]);

	/* check */
	CHECK_CONDITION (s->value[DBHDC_EFF], < , ZERO);

}

void canopy_cover (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	double eff_canopy_cover;
	double lateral_area;

	double crown_surface_area;
	double canopy_surface_cover;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/*** Canopy cover ***/

	logger(g_debug_log,"\n*CANOPY COVER*\n");

	/* Canopy and Soil Projected Cover using DBH-DC (at zenith angle) */
	s->value[CANOPY_COVER_PROJ] = s->value[CROWN_AREA_PROJ] * s->counter[N_TREE] / g_settings->sizeCell;
	logger(g_debug_log, "-Canopy Projected Cover (at zenith angle) = %f %%\n", s->value[CANOPY_COVER_PROJ] * 100.);

	/* (ORIGINAL) Canopy Projected Cover (integrated all over all viewing angles) */
	/* following Cauchy's theorems Duursma et al., 2012, Tree Phys) */

//	/* (MODIFIED) formulation based on canopy cover */
//	/* note: this is valid ONLY for cylinder shape crowns */
//
//	/* Normalizing CANOPY_COVER and max_layer_cover (0-1) */
//	eff_canopy_cover = s->value[CANOPY_COVER_PROJ] / g_settings->max_layer_cover;
//	logger(g_debug_log, "-eff_canopy_cover = %.4g %%\n", eff_canopy_cover * 100.);
//
//	if ( eff_canopy_cover > 1. ) eff_canopy_cover = 1.;
//
//	/* it considers crown projected area (at zenith angles) plus half of lateral area of a cylinder */
//	/* when canopy tends to closure the later part of the crown area that absorbs light tends to be reduced */
//	lateral_area = ((s->value[CROWN_DIAMETER] * Pi * s->value[CROWN_HEIGHT]) / 2.) * (1. - eff_canopy_cover);
//	logger(g_debug_log, "-Crown Lateral Area = %f \n", lateral_area);

	/* Canopy cover able to absorb light (integrated all over all viewing angles) */
	s->value[CANOPY_COVER_EXP] = (s->value[CROWN_AREA_EXP] * s->counter[N_TREE]) / g_settings->sizeCell ;
	logger(g_debug_log, "-Canopy Cover Projected = %f %%\n", s->value[CANOPY_COVER_PROJ] * 100.);
	logger(g_debug_log, "-Canopy Cover Exposed   = %f %%\n", s->value[CANOPY_COVER_EXP]  * 100.);

}
