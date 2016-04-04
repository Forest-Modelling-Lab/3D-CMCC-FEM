/* dendrometry.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Dendrometry (SPECIES *const s, HEIGHT *const h, int years)
{
	double oldavDBH;
	double oldTreeHeight;


	Log("\n**DENDROMETRY_ROUTINE**\n");

	Log("\n**Average DBH**\n");
	Log("**Tree Height from CC function**\n");

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
		Log("Using site related stemconst stempower\n");
		s->value[AVDBH] = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM) / s->value[STEMCONST_P], (1.0 / s->value[STEMPOWER_P]));
	}

	Log("-Average stem mass = %f kgC/tree\n", s->value[AV_STEM_MASS_KgC]);
	Log("Old AVDBH = %.10f cm\n", oldavDBH);
	Log("-New Average DBH = %.10f cm\n", s->value[AVDBH]);


	/* control */
	//CHECK_CONDITION(oldavDBH, > s->value[AVDBH]);

	/*compute Tree Height*/
	/*using Chapman_Richards Function*/

	//the terms 1.3 in C-R Function is breast height (1.3 m)
	//CRA, CRB, CRC are species-specific regression coefficients
	//CRA coefficient is the asymptotic maximum height this coefficient represents the theoretic maximum height obtainable
	//for a given stand or plot, this parameter could be used to represent maximum tree height. By modifying equation so that CRA changes
	//as a function of stand age, site index, dominant height, or stand density, equation can be generalized to be more applicable to a wider range of
	//sites and stand ages, or more regionally applied
	//CRB represents exponential decay parameter
	//CRC represents shape parameters
	h->value = 1.3 + s->value[CRA] * pow (1.0 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
	Log("-Tree Height using Chapman-Richard function = %f m\n", h->value);

	if (h->value > s->value[HMAX])
	{
		Log("Chapaman-Richards function for tree height exceeds HMAX\n");
		h->value = s->value[HMAX];
	}
	/*Tree Height using SORTIE */
	/*
	if (s->value[AVDBH]> 10)
	{
		//Log("SORTIE FUNC DBH > 10 cm\n");
		//it is essentially a chapman-richards equation!!
		s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1.0 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
		Log("-Tree Height using Sortie function > 10 cm = %f m\n", s->value[TREE_HEIGHT_SORTIE]);
	}
	else
	{
		//Log("SORTIE FUNC DBH < 10 cm\n");

		s->value[TREE_HEIGHT_SORTIE] = 0.1 + 30 *( 1.0 - exp ( - 0.03 * s->value[AVDBH] ));
		L
*/

	CHECK_CONDITION(oldavDBH, > s->value[AVDBH] + 1e-10);
	/* to avoid that error appears due to differences between measured and computed tree height values */
	if(years > 0)
	{
		CHECK_CONDITION(oldTreeHeight, > h->value + 1e-10);
	}

	/* recompute sapwood-heartwood area */
	Log("\nSAPWOOD CALCULATION using sapwood area\n");
	s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2.0), 2.0)) * Pi);
	Log(" BASAL AREA = %f cm^2\n", s->value[BASAL_AREA]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
	Log(" SAPWOOD_AREA = %f cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	Log(" HEART_WOOD_AREA = %f cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	Log(" sapwood perc = %f%%\n", s->value[SAPWOOD_PERC]*100);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	Log(" Sapwood stem biomass = %f tC class cell \n", s->value[STEM_SAPWOOD_C]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	Log(" Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	Log(" Sapwood branch and bark biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	Log(" Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap_tDM]);
}
void Annual_minimum_reserve (SPECIES *s)
{
	/* recompute annual minimum reserve pool for  year allocation */
	//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
	//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

	/* IMPORTANT! reserve computation if not in init data are computed from DM */
	Log("\n*annual minimum reserve*\n");
	s->value[MIN_RESERVE_tDM] = s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
	//fixme
	s->value[MIN_RESERVE_C]= s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
	Log("--MINIMUM Reserve Biomass = %f t res/cell \n", s->value[RESERVE_C]);
	s->value[AV_MIN_RESERVE_KgDM] = s->value[MIN_RESERVE_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C] *1000.0 /s->counter[N_TREE];
	Log("--Average MINIMUM Reserve Biomass = %f Kgres/tree \n", s->value[RESERVE_C]);
}
