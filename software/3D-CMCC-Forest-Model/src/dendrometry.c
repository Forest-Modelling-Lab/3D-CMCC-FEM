/* dendrometry.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dendometry.h"
#include "common.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void dendrometry(cell_t *const c, const int height, const int age, const int species, const int year)
{
	double oldavDBH;
	double oldTreeHeight;

	height_t *h;
	species_t *s;

	h = &c->heights[height];
	s = &c->heights[height].ages[age].species[species];


	logger(g_log, "\n**DENDROMETRY_ROUTINE**\n");
	logger(g_log, "\n**Average DBH**\n");
	logger(g_log, "**Tree Height from CC function**\n");

	/*compute Tree AVDBH*/

	oldavDBH = s->value[AVDBH];
	oldTreeHeight = h->value;

	if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
	{
		//use generic stemconst stempower values

		if (oldavDBH < 9)
		{
			s->value[AVDBH] = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_A));
		}
		else if (oldavDBH > 9 && oldavDBH < 15)
		{
			s->value[AVDBH] = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_B));
		}
		else
		{
			s->value[AVDBH] = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_C));
		}
	}
	else
	{
		//use site specific stemconst stempower values
		logger(g_log, "Using site related stemconst stempower\n");
		s->value[AVDBH] = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM) / s->value[STEMCONST_P], (1.0 / s->value[STEMPOWER_P]));
	}

	logger(g_log, "-Average stem mass = %f kgC/tree\n", s->value[AV_STEM_MASS_KgC]);
	logger(g_log, "Old AVDBH = %.10f cm\n", oldavDBH);
	logger(g_log, "-New Average DBH = %.10f cm\n", s->value[AVDBH]);


	/* control */
	//CHECK_CONDITION(oldavDBH, > s->value[AVDBH]);

	/*compute Tree Height*/
	/*using Chapman_Richards Function*/

	/* the terms 1.3 in C-R Function is breast height (1.3 m)
	CRA, CRB, CRC are species-specific regression coefficients
	CRA coefficient is the asymptotic maximum height this coefficient represents the theoretic maximum height obtainable
	for a given stand or plot, this parameter could be used to represent maximum tree height. By modifying equation so that CRA changes
	as a function of stand age, site index, dominant height, or stand density, equation can be generalised to be more applicable to a wider range of
	sites and stand ages, or more regionally applied
	CRB represents exponential decay parameter
	CRC represents shape parameters */

	h->value = 1.3 + s->value[CRA] * pow (1.0 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
	logger(g_log, "-Tree Height using Chapman-Richard function = %f m\n", h->value);

	if (h->value > s->value[HMAX])
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
		logger(g_log, "-Tree Height using Sortie function > 10 cm = %f m\n", s->value[TREE_HEIGHT_SORTIE]);
	}
	else
	{
		//logger(g_log, "SORTIE FUNC DBH < 10 cm\n");

		s->value[TREE_HEIGHT_SORTIE] = 0.1 + 30 *( 1.0 - exp ( - 0.03 * s->value[AVDBH] ));
		L
*/

	//FIXME
	//CHECK_CONDITION(oldavDBH, > s->value[AVDBH] + 1e-10);
	/* to avoid that error appears due to differences between measured and computed tree height values */
	if(year > 0)
	{
		CHECK_CONDITION(oldTreeHeight, > h->value + 1e-10);
	}

	/* recompute sapwood-heartwood area */
	logger(g_log, "\nSAPWOOD CALCULATION using sapwood area\n");
	s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2.0), 2.0)) * Pi);
	logger(g_log, " BASAL AREA = %f cm^2\n", s->value[BASAL_AREA]);
	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
	logger(g_log, " BASAL BASAL_AREA_m2 = %f m^2\n", s->value[BASAL_AREA_m2]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
	logger(g_log, " SAPWOOD_AREA = %f cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	logger(g_log, " HEART_WOOD_AREA = %f cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	logger(g_log, " sapwood perc = %f%%\n", s->value[SAPWOOD_PERC]*100);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, " Sapwood stem biomass = %f tC class cell \n", s->value[STEM_SAPWOOD_C]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, " Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, " Sapwood branch and bark biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	logger(g_log, " Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap_tDM]);

	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);

	/* cell level computation */
	/* stand basal area in m2 */
	c->basal_area += s->value[STAND_BASAL_AREA_m2];
	logger(g_log, "Cell level stand basal area = %f m^2/cell\n", c->basal_area);


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
	logger(g_log, "--MINIMUM Reserve Biomass = %f t res/cell \n", s->value[RESERVE_C]);
	s->value[AV_MIN_RESERVE_KgDM] = s->value[MIN_RESERVE_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "--Average MINIMUM Reserve Biomass = %f Kgres/tree \n", s->value[RESERVE_C]);
}


