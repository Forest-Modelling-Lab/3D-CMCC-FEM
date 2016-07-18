/* initialization.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "soil_settings.h"
#include "settings.h"
#include "logger.h"
#include "common.h"

extern settings_t *g_settings;
extern logger_t* g_log;
extern soil_settings_t *g_soil_settings;

void Initialization_biomass_data(cell_t *const c, const int height, const int age, const int species)
{
	height_t *h;
	h = &c->heights[height];

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	logger(g_log, "\n\n...checking initial biomass data...\n");

	/*check for initial biomass*/
	if (s->value[BIOMASS_STEM_tDM] == 0.0 || s->value[BIOMASS_STEM_tDM] == NO_DATA)
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

	if (s->value[BIOMASS_BRANCH_tDM] == 0.0 || s->value[BIOMASS_BRANCH_tDM] == NO_DATA)
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

	if(s->value[BIOMASS_COARSE_ROOT_tDM]== 0 || s->value[BIOMASS_COARSE_ROOT_tDM] == NO_DATA)
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
	if (s->value[RESERVE_tDM] == 0 || s->value[RESERVE_tDM] == NO_DATA)
	{
		logger(g_log, "\nNo Reserve Biomass Data are available for model initialization \n");
		logger(g_log, "...Generating input Reserve Biomass biomass data\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

		/* IMPORTANT! reserve computation if not in initialized is computed from DryMatter */
		s->value[RESERVE_tDM] = s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
		//fixme how it does??
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
		logger(g_log, "---Reserve from initialization file = %f \n", s->value[RESERVE_tDM]);
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
	if (s->value[BIOMASS_FOLIAGE_tDM] == 0.0 || s->value[BIOMASS_FOLIAGE_tDM] == NO_DATA)
	{
		/* deciduous */
		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			/* assuming no leaf at 1st of January */
			s->value[BIOMASS_FOLIAGE_tDM] = 0.0;
			s->value[LEAF_C] = 0.0;
		}
		/* evergreen */
		else
		{
			logger(g_log, "\nNo Leaf Biomass Data are available for model initialization \n");
			logger(g_log, "...Generating input Leaf Biomass biomass data from LAI\n");
			//fixme it seems to not have sense
			/* a very special (and hopefully rare) case in which there'nt data for LAI or LEAF_C */
			if(!s->value[LAI])
			{
				s->value[BIOMASS_FOLIAGE_tDM] =  s->value[RESERVE_tDM] * (1.0 - s->value[STEM_LEAF_FRAC]);
				s->value[LEAF_C] =  s->value[RESERVE_C] * (1.0 - s->value[STEM_LEAF_FRAC]);
				logger(g_log, "----Leaf Biomass initialization data  = %f tDM cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
				logger(g_log, "----Leaf Biomass initialization data  = %f tC cell\n", s->value[LEAF_C]);
			}
			/* otherwise use LAI */
			else
			{
				/* compute leaf carbon to LAI down-scaled to canopy cover*/
				s->value[LEAF_C] = (s->value[LAI] / s->value[SLA_AVG]);
				logger(g_log, "--Leaf carbon  = %g KgC/m2\n", s->value[LEAF_C]);
				//fixme it should takes into account effective cell coverage
				/* convert to tons of C and to cell area*/
				s->value[LEAF_C] = s->value[LEAF_C] / 1000.0 * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell);
				logger(g_log, "--Leaf carbon  = %g tC/cell size\n", s->value[LEAF_C]);

				/* Calculate projected LAI for sunlit and shaded canopy portions */
				s->value[LAI_SUN] = 1.0 - exp(-s->value[LAI]);
				s->value[LAI_SHADE] = s->value[LAI] - s->value[LAI_SUN];
				logger(g_log, "LAI SUN = %g\n", s->value[LAI_SUN]);
				logger(g_log, "LAI SHADE = %g\n", s->value[LAI_SHADE]);
			}
		}
	}
	else
	{
		s->value[LEAF_C] = s->value[BIOMASS_FOLIAGE_tDM]/GC_GDM;
		logger(g_log, "Ok Leaf biomass..\n");
		logger(g_log, "---Leaf Biomass from init file  cell\n", s->value[BIOMASS_FOLIAGE_tDM]);
		logger(g_log, "---Leaf Biomass from init file = %f tC cell\n", s->value[LEAF_C]);
	}
	/* for leaf balance */
	s->value[OLD_LEAF_C] = s->value[LEAF_C];

	s->value[AV_LEAF_MASS_KgC] = s->value[LEAF_C] *1000.0 /s->counter[N_TREE];
	logger(g_log, "-Individual foliage biomass = %f KgC\n", s->value[AV_LEAF_MASS_KgC]);

	//FIXME MODEL ASSUMES THAT IF NO FINE-ROOT BIOMASS ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	if (( s->value[BIOMASS_FINE_ROOT_tDM] == 0.0 || s->value[BIOMASS_FINE_ROOT_tDM] == NO_DATA)
			&& (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
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

void Initialization_site_data(cell_t *const c)
{
	float acoeff;
	float bcoeff;
	float sat;

	float volumetric_wilting_point;
	float volumetric_field_capacity;
	float volumetric_saturated_hydraulic_conductivity;

	logger(g_log, "\n****SITE-SOIL CHARACTERISTICS for cell (%d, %d)****\n", c->x, c->y);
	// (cm)   effective depth of rooting zone
	logger(g_log, "Soil depth = %f cm\n", g_soil_settings->values[SOIL_DEPTH]);

	/*soil matric potential*/
	CHECK_CONDITION(fabs((g_soil_settings->values[SOIL_SAND_PERC] + g_soil_settings->values[SOIL_CLAY_PERC] + g_soil_settings->values[SOIL_SILT_PERC]) -100.0 ), > 1e-4);
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

	logger(g_log, "BIOME soil characteristics\n");
	//double soilw_fc; //maximum volume soil water content in m3/m3
	// (DIM) Clapp-Hornberger "b" parameter
	// ALESSIOC
	//c->soil_b = -(3.10 + 0.157*g_soil_settings->values[SOIL_CLAY_PERC] - 0.003*g_soil_settings->values[SOIL_SAND_PERC]); /* ok for schwalm*/
	//logger(g_log, "soil_b = %f (DIM)\n", c->soil_b);
	///* following Rawls et al., 1992 and Schwalm et al., 2004 */
	///*texture-dependent empirical coefficinet */
	//// c->soil_b = 11.43 - (0.1034*g_soil_settings->values[SOIL_SAND_PERC) - (0.0687*0.157*g_soil_settings->values[SOIL_silt_perc);

	///* calculate the soil pressure-volume coefficients from texture data */
	///* Uses the multivariate regressions from Cosby et al., 1984 */
	//// (DIM) Soil volumetric water content at saturation
	//c->vwc_sat = (50.5 - 0.142*g_soil_settings->values[SOIL_SAND_PERC] - 0.037*g_soil_settings->values[SOIL_CLAY_PERC])/100.0; /* ok for schwalm*/
	//logger(g_log, "volumetric water content at saturation (BIOME) = %f %%(vol)\n", c->vwc_sat);
	//// (MPa) soil matric potential at saturation
	//c->psi_sat = -(exp((1.54 - 0.0095*g_soil_settings->values[SOIL_SAND_PERC] + 0.0063*g_soil_settings->values[SOIL_SILT_PERC])*log(10.0))*9.8e-5); /* ok for schwalm*/
	//logger(g_log, "psi_sat = %f MPa \n", c->psi_sat);
	//// Clapp-Hornenberger function 1978 (DIM) Soil Field Capacity Volumetric Water Content at field capacity ( = -0.015 MPa)
	//c->vwc_fc =  c->vwc_sat * pow((-0.015/c->psi_sat),(1.0/c->soil_b));
	//logger(g_log, "volumetric water content at field capacity (BIOME) = %f %%(vol) \n", c->vwc_fc);

	//// define maximum soilwater content, for outflow calculation
	////converts volumetric water content (m3/m3) --> (kg/m2)

	//// (kgH2O/m2) soil water at field capacity
	//c->soilw_fc = (g_soil_settings->values[SOIL_DEPTH] / 100) * c->vwc_fc * 1000.0;
	//logger(g_log, "soilw_fc BIOME (MAXASW FC BIOME)= %f (kgH2O/m2)\n", c->soilw_fc);
	////equal to MAXASW
	//// (kgH2O/m2) soil water at saturation
	//c->soilw_sat = (g_soil_settings->values[SOIL_DEPTH] / 100) * c->vwc_sat * 1000.0;
	//logger(g_log, "soilw_sat BIOME (MAXASW SAT BIOME)= %f (kgH2O/m2)\n", c->soilw_sat);

	//c->max_asw_fc = c->soilw_fc;

	////compute initialization soil water content
	//c->asw = (c->soilw_sat * g_settings->init_frac_maxasw);
	//logger(g_log, "Initialization ASW = %f (mm-kgH2O/m2)\n\n\n", c->asw);

	////snow initialization
	//c->snow_pack = 0.0;
	//c->snow_subl = 0;

	/* soil data from https://www.nrel.colostate.edu/projects/century/soilCalculatorHelp.htm */
	/* following Saxton et al 1986, 2006, 2008 */
	logger(g_log, "CENTURY soil characteristics\n");
	acoeff = exp(-4.396 - 0.0715 * g_soil_settings->values[SOIL_CLAY_PERC] - 4.88e-4 * pow(g_soil_settings->values[SOIL_SAND_PERC],2) - 4.285e-5 * pow(g_soil_settings->values[SOIL_SAND_PERC],2)*g_soil_settings->values[SOIL_CLAY_PERC]);
	bcoeff = (-3.14 - 0.00222 * pow(g_soil_settings->values[SOIL_CLAY_PERC],2) - 3.484e-5 * pow(g_soil_settings->values[SOIL_SAND_PERC],2) * g_soil_settings->values[SOIL_CLAY_PERC]);
	sat = (0.332 - 7.251e-4 * g_soil_settings->values[SOIL_SAND_PERC] + 0.1276 * log10(g_soil_settings->values[SOIL_CLAY_PERC]));

	/* volumetric percentage for wilting point */
	volumetric_wilting_point = pow((15.0/acoeff), (1.0/bcoeff));
	/* volumetric percentage for field capacity */
	volumetric_field_capacity = pow((0.333/acoeff),(1.0/bcoeff));
	/* volumetric percentage for saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity = exp((12.012 - 0.0755 * g_soil_settings->values[SOIL_SAND_PERC]) + (-3.895 + 0.03671 * g_soil_settings->values[SOIL_SAND_PERC] - 0.1103 * g_soil_settings->values[SOIL_CLAY_PERC] + 8.7546e-4 * pow(g_soil_settings->values[SOIL_CLAY_PERC],2))/sat);
	/* bulk density g/cm3 */
	c->bulk_density = (1 - sat) * 2.65;

	/* corrections from Steve Del Grosso */
	/* volumetric percentage wilting point */
	volumetric_wilting_point += (-0.15 * volumetric_wilting_point);
	logger(g_log, "*volumetric water content at wilting point (CENTURY) = %f %%(vol)\n", volumetric_wilting_point);
	/* (kgH2O/m2) soil water at wilting point */
	c->wilting_point = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_wilting_point * 1000.0;
	logger(g_log, "**Wilting point (CENTURY) = %f mm/m2\n", c->wilting_point);

	/* volumetric percentage field capacity */
	volumetric_field_capacity += (0.07 * volumetric_field_capacity);
	logger(g_log, "*volumetric water content at field capacity (CENTURY) = %f %%(vol)\n", volumetric_field_capacity);
	/* (kgH2O/m2) soil water at field capacity */
	c->field_capacity = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_field_capacity * 1000.0;
	logger(g_log, "**Field capacity (CENTURY) = %f mm/m2\n", c->field_capacity);

	/* volumetric percentage saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity /= 1500.0;
	logger(g_log, "*volumetric water content at saturated hydraulic conductance (CENTURY) = %f %%(vol)\n", volumetric_saturated_hydraulic_conductivity);
	//fixme not clear what it is
	/* (kgH2O/m2) soil water at saturated hydraulic conductivity */
	c->sat_hydr_conduct = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_saturated_hydraulic_conductivity * 1000.0;
	logger(g_log, "**Saturated hydraulic conductivity (CENTURY) = %f mm/m2\n", c->sat_hydr_conduct);

	/* bulk density g/cm3 */
	c->bulk_density += (-0.08 * c->bulk_density);
	logger(g_log, "**Bulk density = %f g/cm^3\n", c->bulk_density);
}


