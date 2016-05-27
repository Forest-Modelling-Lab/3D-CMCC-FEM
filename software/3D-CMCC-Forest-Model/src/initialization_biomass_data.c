/*
 * initialization_biomass_data.c

 *
 *  Created on: 31/ott/2012
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void Initialization_biomass_data (SPECIES *s, HEIGHT *h)
{
	/*
	if ((s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2) && s->value[LAI] == 0 && s->value[BIOMASS_FOLIAGE] == 0)
	{
		if (settings->spatial == 'u')
		{
			logger(g_log, "MODEL RUN FOR EVERGREEN COULD NOT RUN WITHOUT INITIAL LAI VALUES!!!!!!!!!!!!!!!!!!!!\n");
			exit (1);
		}
	}
	 */

	logger(g_log, "\n\n...checking initial biomass data...\n");

	/*check for initial biomass*/
	if (s->value[BIOMASS_STEM_tDM]== 0.0 )
	{
		logger(g_log, "\nNo Stem Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Stem Biomass biomass data from DBH = %f cm\n", s->value[AVDBH]);
		//compute stem biomass from DBH
		if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
		{
			//use generic stemconst stempower values
			logger(g_log, "..computing stem biomass from generic stempower and stemconst DBH = %f cm\n", s->value[AVDBH]);
			if (s->value[AVDBH] < 9)
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_A));
			}
			else if (s->value[AVDBH] > 9 && s->value[AVDBH] < 15)
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_B));
			}
			else
			{
				//use site specific stemconst stempower values
				logger(g_log, "Using site related stemconst stempower\n");
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (s->value[AVDBH], STEMPOWER_C));
			}
		}
		else
		{
			//use site specific stemconst stempower values
			logger(g_log, "..computing stem biomass from generic stempower and stemconst DBH = %f cm\n", s->value[AVDBH]);
			s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST_P] * pow (s->value[AVDBH], s->value[STEMPOWER_P]);
		}
		//1000 to convert Kg into tons
		s->value[BIOMASS_STEM_tDM] = s->value[AV_STEM_MASS_KgDM] * s->counter[N_TREE] / 1000.0;
		s->value[STEM_C] = s->value[BIOMASS_STEM_tDM] / GC_GDM;
		logger(g_log, "-Class stem Biomass initialization data from DBH = %f tDM cell\n", s->value[BIOMASS_STEM_tDM]);
		logger(g_log, "-Class stem Biomass initialization data from DBH = %f tC/cell\n", s->value[STEM_C]);
	}
	else
	{
		logger(g_log, "Ok stem biomass..\n");
		logger(g_log, "---Stem Biomass from init file = %f tDM/cell\n", s->value[BIOMASS_STEM_tDM]);
		s->value[STEM_C] = s->value[BIOMASS_STEM_tDM]/GC_GDM;
		logger(g_log, "---Stem Biomass from init file = %f tC/cell\n", s->value[STEM_C]);
		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM]* 1000.0 / s->counter[N_TREE];
	}
	s->value[AV_STEM_MASS_KgC] = s->value[STEM_C]* 1000.0 / s->counter[N_TREE];
	logger(g_log, "-Individual stem biomass = %f KgDM\n", s->value[AV_STEM_MASS_KgDM]);
	logger(g_log, "-Individual stem biomass = %f KgC\n", s->value[AV_STEM_MASS_KgC]);

	if (s->value[BIOMASS_BRANCH_tDM] == 0.0)
	{
		logger(g_log, "\nNo Branch and Bark Data are available from model initialization\n"
				"Is the Stem biomass initial value with Branch and Bark?\n");
		if (s->value[FRACBB0] == 0)
		{
			logger(g_log, "I don't have FRACBB0 = FRACBB1 \n");
			s->value[FRACBB0] = s->value[FRACBB1];
			logger(g_log, "FRACBB0 = %f\n", s->value[FRACBB0]);
		}
		else
		{
			s->value[FRACBB] = s->value[FRACBB1]+ (s->value[FRACBB0]- s->value[FRACBB1])* exp(-ln2 * (h->value / s->value[TBB]));
			s->value[BIOMASS_BRANCH_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[FRACBB];
			s->value[BRANCH_C] = s->value[STEM_C] * s->value[FRACBB];
			logger(g_log, "-Stem Branch Biomass initialization data from DBH = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
			logger(g_log, "-Stem Branch Biomass initialization data from DBH = %f tC/cell\n", s->value[BRANCH_C]);
		}
	}
	else
	{
		logger(g_log, "Ok stem branch biomass..\n");
		logger(g_log, "---Stem Branch Biomass from init file = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
		s->value[BRANCH_C] = s->value[BIOMASS_BRANCH_tDM] /2.0;
	}
	s->value[AV_BRANCH_MASS_KgDM] = s->value[BIOMASS_BRANCH_tDM] * 1000.0 / s->counter[N_TREE];
	s->value[AV_BRANCH_MASS_KgC] = s->value[BRANCH_C] * 1000.0 / s->counter[N_TREE];
	logger(g_log, "-Individual branch biomass = %f KgDM\n", s->value[AV_BRANCH_MASS_KgDM]);
	logger(g_log, "-Individual branch biomass = %f KgC\n", s->value[AV_BRANCH_MASS_KgC]);

	/*computing total stem biomass*/
	s->value[BIOMASS_TOT_STEM_tDM] = s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_BRANCH_tDM];
	s->value[TOT_STEM_C] = s->value[STEM_C] + s->value[BRANCH_C];
	logger(g_log, "---Total stem biomass (Ws + Wbb) = %f tDM/cell\n", s->value[BIOMASS_TOT_STEM_tDM]);
	logger(g_log, "---Total stem biomass (Ws + Wbb) = %f tC/cell\n", s->value[TOT_STEM_C]);
	CHECK_CONDITION(fabs((s->value[TOT_STEM_C])-(s->value[STEM_C] + s->value[BRANCH_C])), >1e-4);

	s->value[AV_TOT_STEM_MASS_KgDM] = s->value[BIOMASS_TOT_STEM_tDM] * 1000.0 / s->counter[N_TREE];
	s->value[AV_TOT_STEM_MASS_KgC] = s->value[TOT_STEM_C] * 1000.0 / s->counter[N_TREE];
	logger(g_log, "-Individual total stem biomass = %f KgDM\n", s->value[AV_TOT_STEM_MASS_KgDM]);
	logger(g_log, "-Individual total stem = %f KgC\n", s->value[AV_TOT_STEM_MASS_KgC]);
	CHECK_CONDITION(fabs((s->value[AV_TOT_STEM_MASS_KgC])-(s->value[AV_STEM_MASS_KgC] + s->value[AV_BRANCH_MASS_KgC])), >1e-4);

	if(s->value[BIOMASS_COARSE_ROOT_tDM]== 0)
	{
		logger(g_log, "\nNo Coarse root Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Coarse root Biomass biomass data from DBH data...\n");
		//compute coarse root biomass using root to shoot ratio
		s->value[BIOMASS_COARSE_ROOT_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[COARSE_ROOT_STEM];
		s->value[COARSE_ROOT_C] = s->value[STEM_C] * s->value[COARSE_ROOT_STEM];
		logger(g_log, "--Coarse Root Biomass initialization data from Stem Biomass = %f tDM/cell\n", s->value[BIOMASS_COARSE_ROOT_tDM]);
		logger(g_log, "--Coarse Root Biomass initialization data from Stem Biomass = %f tC/cell\n", s->value[COARSE_ROOT_C]);
	}
	else
	{
		logger(g_log, "Ok coarse root biomass..\n");
		logger(g_log, "---Coarse Root Biomass from init file = %ftDM/cell \n", s->value[BIOMASS_COARSE_ROOT_tDM]);
		s->value[COARSE_ROOT_C] = s->value[BIOMASS_COARSE_ROOT_tDM]/GC_GDM;
		logger(g_log, "---Coarse Root Biomass from init file = %ftC/cell \n", s->value[COARSE_ROOT_C]);
	}
	s->value[AV_COARSE_ROOT_MASS_KgDM] = s->value[BIOMASS_COARSE_ROOT_tDM] * 1000.0 / s->counter[N_TREE];
	s->value[AV_COARSE_ROOT_MASS_KgC] = s->value[COARSE_ROOT_C] * 1000.0 / s->counter[N_TREE];
	logger(g_log, "-Individual coarse root biomass = %f KgDM\n", s->value[AV_COARSE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual coarse root biomass = %f KgC\n", s->value[AV_COARSE_ROOT_MASS_KgC]);

	/*sapwood calculation*/
	logger(g_log, "\nSAPWOOD CALCULATION using sapwood area\n");
	s->value[BASAL_AREA] = ((pow((s->value[AVDBH] / 2.0), 2.0)) * Pi);
	logger(g_log, "   BASAL AREA = %f m^2\n", s->value[BASAL_AREA]);
	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
	logger(g_log, " BASAL BASAL_AREA_m2 = %f m^2\n", s->value[BASAL_AREA_m2]);
	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
	logger(g_log, " Stand level class basal area = %f cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);
	logger(g_log, "   SAPWOOD_AREA = %f cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	logger(g_log, "   HEART_WOOD_AREA = %f cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	logger(g_log, "   sapwood perc = %f%%\n", s->value[SAPWOOD_PERC]*100);
	s->value[WS_sap_tDM] = (s->value[BIOMASS_STEM_tDM] * s->value[SAPWOOD_PERC]);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, "   Sapwood stem biomass = %f tDM class cell \n", s->value[WS_sap_tDM]);
	logger(g_log, "   Sapwood stem biomass = %f tC class cell \n", s->value[STEM_SAPWOOD_C]);
	s->value[WRC_sap_tDM] =  (s->value[BIOMASS_COARSE_ROOT_tDM] * s->value[SAPWOOD_PERC]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, "   Sapwood coarse root biomass = %f tDM class cell \n", s->value[WRC_sap_tDM]);
	logger(g_log, "   Sapwood coarse root biomass = %f tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	s->value[WBB_sap_tDM] = (s->value[BIOMASS_BRANCH_tDM] * s->value[SAPWOOD_PERC]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	logger(g_log, "   Sapwood branch and bark biomass = %f tDM class cell \n", s->value[WBB_sap_tDM]);
	logger(g_log, "   Sapwood branch and bark biomass = %f tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[WTOT_sap_tDM] = s->value[WS_sap_tDM] + s->value[WRC_sap_tDM] + s->value[WBB_sap_tDM];
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	logger(g_log, "   Total Sapwood biomass = %f tDM class cell \n", s->value[WTOT_sap_tDM]);
	logger(g_log, "   Total Sapwood biomass per tree = %f tDM tree \n", s->value[WTOT_sap_tDM]/s->counter[N_TREE]);
	logger(g_log, "   Total Sapwood biomass per tree = %f KgDM tree \n", (s->value[WTOT_sap_tDM]/s->counter[N_TREE])*1000.0);
	logger(g_log, "   Total Sapwood biomass per tree = %f gDM tree \n", (s->value[WTOT_sap_tDM]/s->counter[N_TREE])*1000000.0);
	logger(g_log, "   Total Sapwood biomass = %f tC class cell \n", s->value[TOT_SAPWOOD_C]);
	logger(g_log, "   Total Sapwood biomass per tree = %f tC tree \n", s->value[TOT_SAPWOOD_C]/s->counter[N_TREE]);
	logger(g_log, "   Total Sapwood biomass per tree = %f KgC tree \n", (s->value[TOT_SAPWOOD_C]/s->counter[N_TREE])*1000.0);
	logger(g_log, "   Total Sapwood biomass per tree = %f gC tree \n", (s->value[TOT_SAPWOOD_C]/s->counter[N_TREE])*1000000.0);

	/*reserve*/
	if (s->value[RESERVE_tDM] == 0)
	{
		logger(g_log, "\nNo Reserve Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Reserve Biomass biomass data\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

		/* IMPORTANT! reserve computation if not in init data are computed from DM */
		s->value[RESERVE_tDM] = s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
		//fixme
		s->value[RESERVE_C]= s->value[WTOT_sap_tDM] * s->value[SAP_WRES];

		logger(g_log, "\n!!!!RESERVE ARE COMPUTED AS A COSTANT FRACTION OF SAPWOOD IN DRYMATTER!!!!\n");
		logger(g_log, "-----Reserve initialization data  = %f tDM/cell \n", s->value[RESERVE_tDM]);
		logger(g_log, "-----Reserve initialization data  = %f Kg res/cell \n", s->value[RESERVE_tDM] * 1000);
		logger(g_log, "-----Reserve initialization data  = %f g res/tree \n", (s->value[RESERVE_tDM] * 1000000)/(int)s->counter[N_TREE]);
		logger(g_log, "-----Reserve initialization data  = %f t res/cell \n", s->value[RESERVE_C]);
		logger(g_log, "-----Reserve initialization data  = %f Kg res/cell \n", s->value[RESERVE_C] * 1000);
		logger(g_log, "-----Reserve initialization data  = %f g res/tree \n", (s->value[RESERVE_C] * 1000000)/(int)s->counter[N_TREE]);
	}
	else
	{
		logger(g_log, "Ok reserve biomass..\n");
		logger(g_log, "---Reserve from init file = %f \n", s->value[RESERVE_tDM]);
	}
	s->value[AV_RESERVE_MASS_KgDM] = s->value[RESERVE_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_RESERVE_MASS_KgC] = s->value[RESERVE_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual reserve = %f KgDM/tree\n", s->value[AV_RESERVE_MASS_KgDM]);
	logger(g_log, "-Individual reserve = %f Kg/tree\n", s->value[AV_RESERVE_MASS_KgC]);
	/* compute minimum reserve pool */
	s->value[MIN_RESERVE_C] = s->value[RESERVE_C];
	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C]*1000.0 /s->counter[N_TREE];
	logger(g_log, "-Minimum reserve  = %f tres/tree\n", s->value[MIN_RESERVE_C]);
	logger(g_log, "-Individual minimum reserve = %f KgC/tree\n", s->value[AV_MIN_RESERVE_KgC]);

	/* leaf */
	if (s->value[BIOMASS_FOLIAGE_tDM] == 0.0)
	{
		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			/* assuming no leaf at 1st of January */
			s->value[BIOMASS_FOLIAGE_tDM] = 0.0;
			s->value[LEAF_C] = 0.0;
		}
		else
		{
			logger(g_log, "\nNo Foliage Biomass Data are available for model initialization \n");
			logger(g_log, "...Generating input Foliage Biomass biomass data\n");
			//fixme it seems to not have sense
			s->value[BIOMASS_FOLIAGE_tDM] =  s->value[RESERVE_tDM] * (1.0 - s->value[STEM_LEAF_FRAC]);
			s->value[LEAF_C] =  s->value[RESERVE_C] * (1.0 - s->value[STEM_LEAF_FRAC]);
			logger(g_log, "----Foliage Biomass initialization data  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
			logger(g_log, "----Foliage Biomass initialization data  = %f tC cell\n", s->value[LEAF_C]);
		}
	}
	else
	{
		s->value[LEAF_C] = s->value[BIOMASS_FOLIAGE_tDM]/GC_GDM;
		logger(g_log, "Ok foliage biomass..\n");
		logger(g_log, "---Foliage Biomass from init file  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
		logger(g_log, "---Foliage Biomass from init file  = %f tC cell\n", s->value[LEAF_C]);
	}
	s->value[AV_FOLIAGE_MASS_KgDM] = s->value[BIOMASS_FOLIAGE_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_LEAF_MASS_KgC] = s->value[LEAF_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual foliage biomass = %f KgDM\n", s->value[AV_FOLIAGE_MASS_KgDM]);
	logger(g_log, "-Individual foliage biomass = %f KgC\n", s->value[AV_LEAF_MASS_KgC]);


	//FIXME MODEL ASSUMES TAHT IF NOT BIOMASS FOLIAGE ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	if (s->value[BIOMASS_FINE_ROOT_tDM] == 0.0 && (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		logger(g_log, "\nNo Fine root Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Fine root Biomass biomass data from DBH data...\n");
		//assuming FINE_ROOT_LEAF RATIO AS IN BIOME
		s->value[BIOMASS_FINE_ROOT_tDM] = s->value[BIOMASS_FOLIAGE_tDM] * s->value[FINE_ROOT_LEAF];
		s->value[FINE_ROOT_C] = s->value[LEAF_C] * s->value[FINE_ROOT_LEAF];
		logger(g_log, "---Fine Root Biomass initialization data from Stem Biomass = %f tDM cell\n", s->value[BIOMASS_FINE_ROOT_tDM]);
		logger(g_log, "---Fine Root Biomass initialization data from Stem Biomass = %f tC cell\n", s->value[FINE_ROOT_C]);
	}
	else
	{
		/* assuming no fine root at 1st of January */
		s->value[BIOMASS_FINE_ROOT_tDM] = 0.0;
		s->value[FINE_ROOT_C] = 0.0;
		logger(g_log, "Ok fine root biomass..\n");
		logger(g_log, "---Fine Root Biomass from init file  = %f tDM cell\n", s->value[BIOMASS_FINE_ROOT_tDM]);
		logger(g_log, "---Fine Root Biomass from init file  = %f tC cell\n", s->value[FINE_ROOT_C]);
	}
	s->value[AV_FINE_ROOT_MASS_KgDM] = s->value[BIOMASS_FINE_ROOT_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_FINE_ROOT_MASS_KgC] = s->value[FINE_ROOT_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual fine root biomass = %f KgDM\n", s->value[AV_FINE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual fine root biomass = %f KgC\n", s->value[AV_FINE_ROOT_MASS_KgC]);
	s->value[BIOMASS_ROOTS_TOT_tDM] = s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM];
	s->value[TOT_ROOT_C] = s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C];
	logger(g_log, "---Total Root Biomass = %f tDM cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	logger(g_log, "---Total Root Biomass = %f tC cell\n", s->value[TOT_ROOT_C]);
	s->value[AV_ROOT_MASS_KgDM] = s->value[BIOMASS_ROOTS_TOT_tDM] * 1000.0 /s->counter[N_TREE];
	s->value[AV_ROOT_MASS_KgC] = s->value[TOT_ROOT_C] * 1000.0 /s->counter[N_TREE];
	CHECK_CONDITION(fabs((s->value[TOT_ROOT_C])-(s->value[COARSE_ROOT_C] + s->value[FINE_ROOT_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_ROOT_MASS_KgC])-(s->value[AV_COARSE_ROOT_MASS_KgC] + s->value[AV_FINE_ROOT_MASS_KgC])), >1e-4);

	/*COMPUTE BIOMASS LIVE WOOD*/
	//assuming LIVE_DEAD WOOD RATIO AS IN BIOME
	logger(g_log, "\n*******************************\n");
	/*FOR STEM*/
	logger(g_log, "Total Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_tDM]);
	logger(g_log, "Total Stem Biomass = %f tC cell\n", s->value[STEM_C]);
	s->value[BIOMASS_STEM_LIVE_WOOD_tDM]= s->value[BIOMASS_STEM_tDM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C]= s->value[STEM_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "-Live Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
	logger(g_log, "-Live Stem Biomass = %f tC cell\n", s->value[STEM_LIVE_WOOD_C]);
	s->value[BIOMASS_STEM_DEAD_WOOD_tDM]= s->value[BIOMASS_STEM_tDM] -s->value[BIOMASS_STEM_LIVE_WOOD_tDM];
	s->value[STEM_DEAD_WOOD_C]= s->value[STEM_C] -s->value[STEM_LIVE_WOOD_C];
	logger(g_log, "-Dead Stem Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
	logger(g_log, "-Dead Stem Biomass = %f tC cell\n", s->value[STEM_DEAD_WOOD_C]);
	s->value[AV_LIVE_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_LIVE_STEM_MASS_KgC] = s->value[STEM_LIVE_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live wood biomass = %f KgDM\n", s->value[AV_LIVE_STEM_MASS_KgDM]);
	logger(g_log, "-Individual live wood biomass = %f KgC\n", s->value[AV_LIVE_STEM_MASS_KgC]);
	s->value[AV_DEAD_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_DEAD_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_DEAD_STEM_MASS_KgC] = s->value[STEM_DEAD_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual dead wood biomass = %f KgDM\n", s->value[AV_DEAD_STEM_MASS_KgDM]);
	logger(g_log, "-Individual dead wood biomass = %f KgC\n", s->value[AV_DEAD_STEM_MASS_KgC]);
	CHECK_CONDITION(fabs((s->value[STEM_C])-(s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_STEM_MASS_KgC])-(s->value[AV_LIVE_STEM_MASS_KgC] + s->value[AV_DEAD_STEM_MASS_KgC])), >1e-4);

	/*FOR COARSE ROOT*/
	logger(g_log, "Total Root Biomass = %f tDM cell\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
	logger(g_log, "Total Root Biomass = %f tC cell\n", s->value[TOT_ROOT_C]);
	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]= s->value[BIOMASS_COARSE_ROOT_tDM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[COARSE_ROOT_LIVE_WOOD_C]= s->value[COARSE_ROOT_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "-Live Coarse Root Biomass = %f tDM cell\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
	logger(g_log, "-Live Coarse Root Biomass = %f tC cell\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]= s->value[BIOMASS_COARSE_ROOT_tDM] -s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM];
	s->value[COARSE_ROOT_DEAD_WOOD_C]= s->value[COARSE_ROOT_C] -s->value[COARSE_ROOT_LIVE_WOOD_C];
	logger(g_log, "-Dead Coarse Root Biomass = %f tDM cell\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
	logger(g_log, "-Dead Coarse Root Biomass = %f tC cell\n", s->value[COARSE_ROOT_DEAD_WOOD_C]);
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] = s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] = s->value[COARSE_ROOT_LIVE_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live coarse root biomass = %f KgDM\n", s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual live coarse root biomass = %f KgC\n", s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM]);
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM] = s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM] = s->value[COARSE_ROOT_DEAD_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live coarse root biomass = %f KgDM\n", s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	logger(g_log, "-Individual live coarse root biomass = %f KgC\n", s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM]);
	CHECK_CONDITION(fabs((s->value[COARSE_ROOT_C])-(s->value[COARSE_ROOT_LIVE_WOOD_C] + s->value[COARSE_ROOT_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_COARSE_ROOT_MASS_KgC])-(s->value[AV_LIVE_COARSE_ROOT_MASS_KgDM] + s->value[AV_DEAD_COARSE_ROOT_MASS_KgDM])), >1e-4);

	/*FOR BRANCH*/
	logger(g_log, "Total Branch Biomass = %f tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
	logger(g_log, "Total BB Branch = %f tDM/cell\n", s->value[BRANCH_C]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C]= s->value[BRANCH_C] * (s->value[LIVE_TOTAL_WOOD_FRAC]);
	logger(g_log, "-Live Stem Branch Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	logger(g_log, "-Live Stem Branch Biomass = %f tC cell\n", s->value[BRANCH_LIVE_WOOD_C]);
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] -s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[BRANCH_DEAD_WOOD_C]= s->value[BRANCH_C] -s->value[BRANCH_LIVE_WOOD_C];
	logger(g_log, "-Dead Stem Branch Biomass = %f tDM cell\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
	logger(g_log, "-Dead Stem Branch Biomass = %f tC cell\n", s->value[BRANCH_DEAD_WOOD_C]);
	s->value[AV_LIVE_BRANCH_MASS_KgDM] = s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_LIVE_BRANCH_MASS_KgC] = s->value[BRANCH_LIVE_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual live branch biomass = %f KgDM\n", s->value[AV_LIVE_BRANCH_MASS_KgDM]);
	logger(g_log, "-Individual live branch biomass = %f KgC\n", s->value[AV_LIVE_BRANCH_MASS_KgC]);
	s->value[AV_DEAD_BRANCH_MASS_KgDM] = s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] *1000.0 /s->counter[N_TREE];
	s->value[AV_DEAD_BRANCH_MASS_KgC] = s->value[BRANCH_DEAD_WOOD_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual dead branch biomass = %f KgDM\n", s->value[AV_DEAD_BRANCH_MASS_KgDM]);
	logger(g_log, "-Individual dead branch biomass = %f KgC\n", s->value[AV_DEAD_BRANCH_MASS_KgC]);
	CHECK_CONDITION(fabs((s->value[BRANCH_C])-(s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])), >1e-4);
	CHECK_CONDITION(fabs((s->value[AV_BRANCH_MASS_KgC])-(s->value[AV_LIVE_BRANCH_MASS_KgC] + s->value[AV_DEAD_BRANCH_MASS_KgC])), >1e-4);

	s->value[BIOMASS_LIVE_WOOD_tDM] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM]+
			s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[LIVE_WOOD_C] = s->value[STEM_LIVE_WOOD_C]+
				s->value[COARSE_ROOT_LIVE_WOOD_C]+
				s->value[BRANCH_LIVE_WOOD_C];
	logger(g_log, "---Live biomass = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD_tDM]);
	logger(g_log, "---Live biomass = %f tC/area\n", s->value[LIVE_WOOD_C]);
	s->value[AV_LIVE_WOOD_MASS_KgDM] = s->value[BIOMASS_LIVE_WOOD_tDM] * 1000.0/s->counter[N_TREE];
	s->value[AV_LIVE_WOOD_MASS_KgC] = s->value[LIVE_WOOD_C] * 1000.0/s->counter[N_TREE];
	logger(g_log, "-Individual total live biomass = %f KgDM\n", s->value[AV_LIVE_WOOD_MASS_KgDM]);
	logger(g_log, "-Individual total live biomass = %f KgC\n", s->value[AV_LIVE_WOOD_MASS_KgC]);

	s->value[BIOMASS_DEAD_WOOD_tDM] = s->value[BIOMASS_STEM_DEAD_WOOD_tDM]+
			s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM];
	s->value[DEAD_WOOD_C] = s->value[STEM_DEAD_WOOD_C]+
			s->value[COARSE_ROOT_DEAD_WOOD_C]+
			s->value[BRANCH_DEAD_WOOD_C];
	logger(g_log, "---Dead biomass = %f tDM/area\n", s->value[BIOMASS_DEAD_WOOD_tDM]);
	logger(g_log, "---Dead biomass = %f tC/area\n", s->value[DEAD_WOOD_C]);
	s->value[AV_DEAD_WOOD_MASS_KgDM] = s->value[BIOMASS_DEAD_WOOD_tDM] * 1000.0/s->counter[N_TREE];
	s->value[AV_DEAD_WOOD_MASS_KgC] = s->value[DEAD_WOOD_C] * 1000.0/s->counter[N_TREE];
	logger(g_log, "-Individual total dead biomass = %f KgDM\n", s->value[AV_DEAD_WOOD_MASS_KgDM]);
	logger(g_log, "-Individual total dead biomass = %f KgC\n", s->value[AV_DEAD_WOOD_MASS_KgC]);

	/* compute percentage of live vs total biomass */
	s->value[TOTAL_WOOD_C] = s->value[STEM_C] + s->value[TOT_ROOT_C] + s->value[BRANCH_C];
	s->value[AV_TOT_WOOD_MASS_KgC] = s->value[AV_STEM_MASS_KgC] + s->value[AV_ROOT_MASS_KgC] + s->value[AV_BRANCH_MASS_KgC];
	logger(g_log, "----Total wood = %f tC/cell\n",s->value[TOTAL_WOOD_C]);
	logger(g_log, "----Total wood = %f KgC/tree\n",s->value[AV_TOT_WOOD_MASS_KgC]);
	logger(g_log, "----Live wood vs total biomass = %f %%\n", (s->value[LIVE_WOOD_C] / s->value[TOTAL_WOOD_C]) * 100.0);
	logger(g_log, "----Dead wood vs total biomass = %f %%\n", (s->value[DEAD_WOOD_C] / s->value[TOTAL_WOOD_C]) * 100.0);
	logger(g_log, "----Live wood vs total biomass = %f %%\n", (s->value[AV_LIVE_WOOD_MASS_KgC] / s->value[AV_TOT_WOOD_MASS_KgC]) * 100.0);
	logger(g_log, "----Dead wood vs total biomass = %f %%\n", (s->value[AV_DEAD_WOOD_MASS_KgC] / s->value[AV_TOT_WOOD_MASS_KgC]) * 100.0);

	logger(g_log, "***reserves following live tissues DM (not used) BIOME = %f tDM/area\n", s->value[BIOMASS_LIVE_WOOD_tDM] * s->value[SAP_WRES]);
	logger(g_log, "***reserves following live tissues C (not used) BIOME = %f tC/area\n", s->value[LIVE_WOOD_C] * s->value[SAP_WRES] );

}
