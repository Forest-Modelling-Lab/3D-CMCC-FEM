/* dendrometry.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dendometry.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void dendrometry(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	double oldavDBH;
	double oldTreeHeight;
	double oldBasalArea;

	height_t *h;
	dbh_t *d;
	species_t *s;

	/* this function compute at the temporal scale at which is called:
	 * -average dbh
	 * -tree height
	 * -basal area
	 * -recompute fractions of live and dead wood
	 * */

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* assign previous month values */
	oldavDBH = d->value;
	oldTreeHeight = h->value;
	oldBasalArea = s->value[BASAL_AREA];

	logger(g_log, "\n**DENDROMETRY**\n");

	logger(g_log, "\n**Average DBH**\n");

	/* compute tree AVDBH */

	if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
	{
		/* use generic stemconst stempower values */
		if (oldavDBH < 9)
		{
			d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_A));
		}
		else if (oldavDBH > 9 && oldavDBH < 15)
		{
			d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_B));
		}
		else
		{
			d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_C));
		}
	}
	else
	{
		/* use site specific stemconst stempower values */
		logger(g_log, "Using site related stemconst stempower\n");
		d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM) / s->value[STEMCONST_P], (1.0 / s->value[STEMPOWER_P]));
	}

	logger(g_log, "-Average stem mass = %g kgC/tree\n", s->value[AV_STEM_MASS_KgC]);
	logger(g_log, "-Old AVDBH = %g cm\n", oldavDBH);
	logger(g_log, "-New Average DBH = %g cm\n", d->value);

	/* check */
	CHECK_CONDITION( d->value, < oldavDBH - 1e-6 );

	/*************************************************************************************************************************/

	/* compute tree Height */
	/* using Chapman_Richards Function */
	/*
	* the terms 1.3 in C-R Function is breast height (1.3 m)
 	* CRA, CRB, CRC are species-specific regression coefficients
	*-CRA coefficient is the asymptotic maximum height this coefficient represents the theoretic maximum height obtainable
	* for a given stand or plot, this parameter could be used to represent maximum tree height.
	* By modifying equation so that CRA changes	as a function of stand age, site index, dominant height,
	* or stand density, equation can be generalised to be more applicable to a wider range of	sites and stand ages,
	* or more regionally applied
	*-CRB represents exponential decay parameter
	*-CRC represents shape parameters
	*/

	logger(g_log, "\n**Tree Height from CC function**\n");

	h->value = 1.3 + s->value[CRA] * pow (1.0 - exp ( - s->value[CRB] * d->value) , s->value[CRC]);
	logger(g_log, "-Tree Height using Chapman-Richard function = %g m\n", h->value);

	if ( h->value > s->value[HMAX] )
	{
		logger(g_log, "Chapaman-Richards function for tree height exceeds HMAX\n");
		h->value = s->value[HMAX];
	}
	/*Tree Height using SORTIE */
	/*
	if (s->value[AVDBH]> 10)
	{
		//logger(g_log, "SORTIE FUNC DBH > 10 cm\n");
		//it is essentially a chapman-richards equation!!
		s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1.0 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
		logger(g_log, "-Tree Height using Sortie function > 10 cm = %g m\n", s->value[TREE_HEIGHT_SORTIE]);
	}
	else
	{
		//logger(g_log, "SORTIE FUNC DBH < 10 cm\n");

		s->value[TREE_HEIGHT_SORTIE] = 0.1 + 30 *( 1.0 - exp ( - 0.03 * s->value[AVDBH] ));
	 */

	logger(g_log, "-Old Tree Height = %g m\n", oldTreeHeight);
	logger(g_log, "-New Tree Height = %g m\n", h->value);

	/* check */
	CHECK_CONDITION( h->value, < oldTreeHeight - 1e-6);

	/*************************************************************************************************************************/

	/* compute Basal Area and sapwood-heartwood area */

	logger(g_log, "\n**Basal Area and sapwood-heartwood area**\n");

	s->value[BASAL_AREA] = ((pow((d->value / 2.0), 2.0)) * Pi);
	logger(g_log, " BASAL AREA = %g cm^2\n", s->value[BASAL_AREA]);
	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
	logger(g_log, " BASAL BASAL_AREA_m2 = %g m^2\n", s->value[BASAL_AREA_m2]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (d->value, s->value[SAP_B]);
	logger(g_log, " SAPWOOD_AREA = %g cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] - s->value[SAPWOOD_AREA];
	logger(g_log, " HEART_WOOD_AREA = %g cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	logger(g_log, " sapwood perc = %g%%\n", s->value[SAPWOOD_PERC]*100);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, " Sapwood stem biomass = %g tC class cell \n", s->value[STEM_SAPWOOD_C]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, " Sapwood coarse root biomass = %g tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, " Sapwood branch and bark biomass = %g tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	logger(g_log, " Total Sapwood biomass = %g tc class cell \n", s->value[TOT_SAPWOOD_C]);
	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %g cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area (meters) = %g m^2/class cell\n", s->value[STAND_BASAL_AREA_m2]);

	logger(g_log, "-Old Basal Area = %g cm^2\n", oldBasalArea);
	logger(g_log, "-New Basal Area = %g cm^2\n", s->value[BASAL_AREA]);

	/* check */
	CHECK_CONDITION( s->value[BASAL_AREA], < oldBasalArea - 1e-6 );
	
}
void annual_minimum_reserve (species_t *const s)
{
	/* recompute annual minimum reserve pool for  year allocation */
	/* these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling */
	/* following Krinner et al., 2005 */

	/* IMPORTANT! reserve computation if not in init data are computed from DM */
	logger(g_log, "\n*annual minimum reserve*\n");
	s->value[MIN_RESERVE_tDM] = s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
	//fixme
	s->value[MIN_RESERVE_C]= s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
	logger(g_log, "--MINIMUM Reserve Biomass = %g t res/cell \n", s->value[RESERVE_C]);
	s->value[AV_MIN_RESERVE_KgDM] = s->value[MIN_RESERVE_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "--Average MINIMUM Reserve Biomass = %g Kgres/tree \n", s->value[RESERVE_C]);
}


