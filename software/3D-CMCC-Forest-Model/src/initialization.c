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
#include "structure.h"
#include "biomass.h"
#include "lai.h"
#include "aut_respiration.h"

extern settings_t *g_settings;
extern logger_t* g_debug_log;
extern soil_settings_t *g_soil_settings;

extern const char sz_err_out_of_memory[];

void initialization_forest_structure(cell_t *const c, const int day, const int month, const int year)
{
	/* note: this function from here are called just for initialization */

	logger(g_debug_log,"\n*******INITIALIZE FOREST STRUCTURE*******\n");

	//	if ( ! day && ! month && ! year)
	//	{
	/* annual forest structure */
	if ( ! annual_forest_structure ( c, year ) )
	{
		puts(sz_err_out_of_memory);
		exit(1);
	}
	//		/* monthly forest structure */
	//		if ( ! monthly_forest_structure ( c ) )
	//		{
	//			puts(sz_err_out_of_memory);
	//			exit(1);
	//		}
	//	}
}

void initialization_forest_class_C (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	d = &c->heights[height].dbhs[dbh];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* compute growth respiration fraction */
	growth_respiration_frac ( a, s );

	logger(g_debug_log,"\n*******INITIALIZE FOREST CLASS CARBON POOLS*******\n");
	logger(g_debug_log, "\n\n...checking initial biomass data for height %g, age %d, species %s...\n", h->value, a->value, s->name);

	/* compute mass density */
	s->value[MASS_DENSITY] = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (a->value / s->value[TRHO]));
	logger(g_debug_log, "-Mass Density = %g tDM/m3\n", s->value[MASS_DENSITY]);

	/* compute single tree volume */
	s->value[TREE_VOLUME] = (Pi * s->value[FORM_FACTOR] * pow((d->value / 100.) , 2.) * h->value) / 4.;
	logger(g_debug_log, "-Single tree volume = %g m3/tree\n", s->value[TREE_VOLUME]);

	/* compute class volume */
	s->value[VOLUME] = s->value[TREE_VOLUME] * s->counter[N_TREE];
	logger(g_debug_log, "-Class volume = %g m3/sizeCell\n", s->value[VOLUME]);

	/*check for initial biomass*/
	if (s->value[BIOMASS_STEM_tDM] == 0.0 || s->value[BIOMASS_STEM_tDM] == NO_DATA)
	{

		logger(g_debug_log, "\nNo Stem Biomass Data are available for model initialization \n");
		logger(g_debug_log, "...Generating input Stem Biomass biomass data from DBH = %g cm\n", d->value);

		//compute stem biomass from DBH
		if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
		{
			//use generic stemconst stempower values
			logger(g_debug_log, "..computing stem biomass from generic stempower and stemconst DBH = %g cm\n", d->value);
			if (d->value < 9)
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (d->value, STEMPOWER_A));
			}
			else if (d->value > 9 && d->value < 15)
			{
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (d->value, STEMPOWER_B));
			}
			else
			{
				//use site specific stemconst stempower values
				logger(g_debug_log, "Using site related stemconst stempower\n");
				s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST] * (pow (d->value, STEMPOWER_C));
			}
		}

		else
		{
			//use site specific stemconst stempower values
			logger(g_debug_log, "..computing stem biomass from generic stempower and stemconst DBH = %g cm\n", d->value);
			s->value[AV_STEM_MASS_KgDM] = s->value[STEMCONST_P] * pow (d->value, s->value[STEMPOWER_P]);
			logger(g_debug_log, "Single tree stem mass = %g KgDM/tree\n", s->value[AV_STEM_MASS_KgDM]);
		}
		//1000 to convert Kg into tons
		s->value[BIOMASS_STEM_tDM] = s->value[AV_STEM_MASS_KgDM] * s->counter[N_TREE] / 1e3;

		s->value[STEM_C] = s->value[BIOMASS_STEM_tDM] / GC_GDM;
		logger(g_debug_log, "-Class stem Biomass initialization (measured)= %g tC/cell\n", s->value[STEM_C]);
	}
	else
	{
		logger(g_debug_log, "Ok stem biomass..\n");
		logger(g_debug_log, "---Stem Biomass from init file = %g tDM/cell\n", s->value[BIOMASS_STEM_tDM]);
		s->value[STEM_C] = s->value[BIOMASS_STEM_tDM] / GC_GDM;
		logger(g_debug_log, "---Stem Biomass from init file = %g tC/cell\n", s->value[STEM_C]);
		s->value[AV_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_tDM] * 1000. / (double)s->counter[N_TREE];
	}
	s->value[AV_STEM_MASS_C] = s->value[STEM_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual stem biomass (measured)= %g tC/tree\n", s->value[AV_STEM_MASS_C]);

	if (s->value[BIOMASS_BRANCH_tDM] == 0.0 || s->value[BIOMASS_BRANCH_tDM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Branch and Bark Data are available from model initialization\n"
				"Is the Stem biomass initial value with Branch and Bark?\n");
		if (s->value[FRACBB0] == 0)
		{
			logger(g_debug_log, "I don't have FRACBB0 = FRACBB1 \n");
			s->value[FRACBB0] = s->value[FRACBB1];
			logger(g_debug_log, "FRACBB0 = %g\n", s->value[FRACBB0]);
		}
		else
		{
			s->value[FRACBB] = s->value[FRACBB1]+ (s->value[FRACBB0] - s->value[FRACBB1])* exp(-ln2 * (h->value / s->value[TBB]));
			s->value[BIOMASS_BRANCH_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[FRACBB];
			s->value[BRANCH_C] = s->value[STEM_C] * s->value[FRACBB];
			logger(g_debug_log, "-Stem Branch Biomass initialization data from DBH = %g tC/cell\n", s->value[BRANCH_C]);
		}
	}
	else
	{
		logger(g_debug_log, "Ok stem branch biomass..\n");
		logger(g_debug_log, "---Stem Branch Biomass from init file = %g tDM/cell\n", s->value[BIOMASS_BRANCH_tDM]);
		s->value[BRANCH_C] = s->value[BIOMASS_BRANCH_tDM] /2.0;
	}
	s->value[AV_BRANCH_MASS_KgDM] = s->value[BIOMASS_BRANCH_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_BRANCH_MASS_C] = s->value[BRANCH_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual branch biomass = %g tC\n", s->value[AV_BRANCH_MASS_C]);


	if(s->value[BIOMASS_CROOT_tDM]== 0 || s->value[BIOMASS_CROOT_tDM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Coarse root Biomass Data are available for model initialization \n");
		logger(g_debug_log, "...Generating input Coarse root Biomass biomass data from DBH data...\n");
		//compute coarse root biomass using root to shoot ratio
		s->value[BIOMASS_CROOT_tDM] = s->value[BIOMASS_STEM_tDM] * s->value[COARSE_ROOT_STEM];
		s->value[CROOT_C] = s->value[STEM_C] * s->value[COARSE_ROOT_STEM];
		logger(g_debug_log, "--Coarse Root Biomass initialization data from Stem Biomass = %g tC/cell\n", s->value[CROOT_C]);
	}
	else
	{
		logger(g_debug_log, "Ok coarse root biomass..\n");
		logger(g_debug_log, "---Coarse Root Biomass from init file = %gtDM/cell \n", s->value[BIOMASS_CROOT_tDM]);
		s->value[CROOT_C] = s->value[BIOMASS_CROOT_tDM] / GC_GDM;
		logger(g_debug_log, "---Coarse Root Biomass from init file = %gtC/cell \n", s->value[CROOT_C]);
	}
	s->value[AV_CROOT_MASS_KgDM] = s->value[BIOMASS_CROOT_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_CROOT_MASS_C] = s->value[CROOT_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual coarse root biomass = %g KgC\n", s->value[AV_CROOT_MASS_C]);

	/*sapwood calculation*/
	logger(g_debug_log, "\nSAPWOOD CALCULATION using sapwood cell\n");
	s->value[BASAL_AREA] = ((pow((d->value / 2.0), 2.0)) * Pi);
	logger(g_debug_log, "BASAL AREA = %g m^2\n", s->value[BASAL_AREA]);
	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
	logger(g_debug_log, "BASAL BASAL_AREA_m2 = %g m^2\n", s->value[BASAL_AREA_m2]);
	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	logger(g_debug_log, "Stand level class basal cell = %g cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
	logger(g_debug_log, "Stand level class basal cell = %g cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (d->value, s->value[SAP_B]);
	logger(g_debug_log, "SAPWOOD_AREA = %g cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	logger(g_debug_log, "HEART_WOOD_AREA = %g cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	logger(g_debug_log, "sapwood perc = %g%%\n", s->value[SAPWOOD_PERC]*100);
	s->value[WS_sap_tDM] = (s->value[BIOMASS_STEM_tDM] * s->value[SAPWOOD_PERC]);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	logger(g_debug_log, "Sapwood stem biomass = %g tC class cell \n", s->value[STEM_SAPWOOD_C]);
	s->value[STEM_HEARTWOOD_C] = (s->value[STEM_C] - s->value[STEM_SAPWOOD_C]);
	logger(g_debug_log, "Heartwood stem biomass = %g tC class cell \n", s->value[STEM_HEARTWOOD_C]);
	s->value[WRC_sap_tDM] =  (s->value[BIOMASS_CROOT_tDM] * s->value[SAPWOOD_PERC]);
	s->value[CROOT_SAPWOOD_C] =  (s->value[CROOT_C] * s->value[SAPWOOD_PERC]);
	logger(g_debug_log, "Sapwood coarse root biomass = %g tDM class cell \n", s->value[CROOT_SAPWOOD_C]);
	s->value[CROOT_HEARTWOOD_C] = (s->value[CROOT_C] - s->value[CROOT_SAPWOOD_C]);
	logger(g_debug_log, "Heartwood coarse root biomass = %g tC class cell \n", s->value[CROOT_HEARTWOOD_C]);
	s->value[WBB_sap_tDM] = (s->value[BIOMASS_BRANCH_tDM] * s->value[SAPWOOD_PERC]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	logger(g_debug_log, "Sapwood branch and bark biomass = %g tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[BRANCH_HEARTWOOD_C] = (s->value[BRANCH_C] - s->value[BRANCH_SAPWOOD_C]);
	logger(g_debug_log, "Heartwood branch biomass = %g tC class cell \n", s->value[BRANCH_HEARTWOOD_C]);
	s->value[WTOT_sap_tDM] = s->value[WS_sap_tDM] + s->value[WRC_sap_tDM] + s->value[WBB_sap_tDM];
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[CROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	logger(g_debug_log, "Total Sapwood biomass = %g tC class cell \n", s->value[TOT_SAPWOOD_C]);
	logger(g_debug_log, "Total Sapwood biomass per tree = %g tC tree \n", s->value[TOT_SAPWOOD_C] / (double)s->counter[N_TREE]);
	logger(g_debug_log, "Total Sapwood biomass per tree = %g KgC tree \n", (s->value[TOT_SAPWOOD_C] / (double)s->counter[N_TREE]) * 1e3);

	s->value[TOT_HEARTWOOD_C] = s->value[STEM_HEARTWOOD_C] + s->value[CROOT_HEARTWOOD_C] + s->value[BRANCH_HEARTWOOD_C];
	logger(g_debug_log, "Total Heartwood biomass = %g tC class cell \n", s->value[TOT_HEARTWOOD_C]);
	logger(g_debug_log, "Total Heartwood biomass per tree = %g tC tree \n", s->value[TOT_HEARTWOOD_C] / (double)s->counter[N_TREE]);
	logger(g_debug_log, "Total Heartwood biomass per tree = %g KgC tree \n", (s->value[TOT_HEARTWOOD_C] / (double)s->counter[N_TREE]) * 1e3);

	s->value[AV_STEM_SAPWOOD_MASS_C]     = s->value[STEM_SAPWOOD_C]   / (double)s->counter[N_TREE];
	s->value[AV_STEM_HEARTWOOD_MASS_C]   = s->value[CROOT_C]          / (double)s->counter[N_TREE];
	s->value[AV_CROOT_SAPWOOD_MASS_C]    = s->value[CROOT_SAPWOOD_C]  / (double)s->counter[N_TREE];
	s->value[AV_CROOT_HEARTWOOD_MASS_C]  = s->value[CROOT_C]          / (double)s->counter[N_TREE];
	s->value[AV_BRANCH_SAPWOOD_MASS_C]   = s->value[BRANCH_SAPWOOD_C] / (double)s->counter[N_TREE];
	s->value[AV_BRANCH_HEARTWOOD_MASS_C] = s->value[CROOT_C]          / (double)s->counter[N_TREE];


	/*reserve*/
	if (s->value[RESERVE_tDM] == 0 || s->value[RESERVE_tDM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Reserve Biomass Data are available for model initialization \n");
		logger(g_debug_log, "...Generating input Reserve Biomass biomass data\n");
		//these values are taken from: following Schwalm and Ek, 2004 Ecological Modelling
		//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

		/* IMPORTANT! reserve computation if not in initialized is computed from DryMatter */
		s->value[RESERVE_tDM] = s->value[WTOT_sap_tDM] * s->value[SAP_WRES];

		//fixme how it does??
		s->value[RESERVE_C]= s->value[WTOT_sap_tDM] * s->value[SAP_WRES];

		logger(g_debug_log, "\n!!!!RESERVE ARE COMPUTED AS A COSTANT FRACTION OF SAPWOOD IN DRYMATTER!!!!\n");
		logger(g_debug_log, "-----Reserve initialization data  = %g t res/cell \n", s->value[RESERVE_C]);
		logger(g_debug_log, "-----Reserve initialization data  = %g Kg res/cell \n", s->value[RESERVE_C] * 1e3);
		logger(g_debug_log, "-----Reserve initialization data  = %g g res/tree \n", s->value[RESERVE_C] * 1e6 / (int)s->counter[N_TREE]);
	}
	else
	{
		logger(g_debug_log, "Ok reserve biomass..\n");
		logger(g_debug_log, "---Reserve from initialization file = %g \n", s->value[RESERVE_tDM]);
	}
	s->value[AV_RESERVE_MASS_KgDM] = s->value[RESERVE_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_RESERVE_MASS_C] = s->value[RESERVE_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual reserve = %g tC/tree\n", s->value[AV_RESERVE_MASS_C]);
	/* compute minimum reserve pool */
	s->value[MIN_RESERVE_C]  = s->value[RESERVE_C];
	s->value[AV_MIN_RESERVE_C] = s->value[MIN_RESERVE_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Minimum reserve  = %g tC/tree\n", s->value[MIN_RESERVE_C]);
	logger(g_debug_log, "-Individual minimum reserve = %g tC/tree\n", s->value[AV_MIN_RESERVE_C]);

	/* leaf */
	if (s->value[BIOMASS_LEAF_tDM] == 0.0 || s->value[BIOMASS_LEAF_tDM] == NO_DATA)
	{
		/* deciduous */
		if ( ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 ) && c->north == 0)
		{
			/* assuming no leaf at 1st of January */
			s->value[BIOMASS_LEAF_tDM] = 0.;
			s->value[LEAF_C]           = 0.;
		}
		/* evergreen */
		else
		{
			logger(g_debug_log, "\nNo Leaf Biomass Data are available for model initialization \n");
			logger(g_debug_log, "...Generating input Leaf Biomass data from LAI\n");

			/* check */
			if( ! s->value[LAI_PROJ] )
			{
				logger_error(g_debug_log,"No Leaf Biomass nor LAI values from initialization file (exit)!!!!\n");
				exit(1);
			}
			/* otherwise use LAI */
			else
			{
				/* compute leaf carbon to LAI down-scaled to canopy cover*/
				s->value[LEAF_C] = (s->value[LAI_PROJ] / s->value[SLA_AVG]);
				logger(g_debug_log, "--Leaf carbon = %g KgC/m2\n", s->value[LEAF_C]);

				//fixme it should takes into account effective cell coverage
				/* convert to tons of C and to cell cell */
				/* note: it uses canopy cover because computed from LAI */
				//fixme it could be that canopy cover isn't initialized!!!!
				s->value[LEAF_C] = s->value[LEAF_C] / 1e3 * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
				logger(g_debug_log, "--Leaf carbon = %g tC/cell size\n", s->value[LEAF_C]);

				/* Calculate projected LAI for sunlit and shaded canopy portions */
				s->value[LAI_SUN_PROJ] = 1.0 - exp(-s->value[LAI_PROJ]);
				s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];
				logger(g_debug_log, "LAI_SUN_PROJ = %g\n", s->value[LAI_SUN_PROJ]);
				logger(g_debug_log, "LAI_SHADE_PROJ = %g\n", s->value[LAI_SHADE_PROJ]);

				/* compute total LAI for Exposed Area */
				s->value[LAI_EXP] = s->value[LAI_PROJ] * ( 1. + s->value[CANOPY_COVER_EXP]);
				logger(g_debug_log, "LAI_EXP = %f m-2\n", s->value[LAI_EXP]);

				/* compute LAI for sunlit and shaded canopy portions for Exposed Area */
				s->value[LAI_SUN_EXP] = s->value[LAI_SUN_PROJ] * ( 1. + s->value[CANOPY_COVER_EXP]);
				s->value[LAI_SHADE_EXP] = s->value[LAI_SHADE_PROJ] * (1 + s->value[CANOPY_COVER_EXP]);
				logger(g_debug_log, "LAI_SUN_EXP = %g m2 m-2\n", s->value[LAI_SUN_EXP]);
				logger(g_debug_log, "LAI_SHADE_EXP = %g m2 m-2\n", s->value[LAI_SHADE_EXP]);
			}
		}
	}
	else
	{
		s->value[LEAF_C] = s->value[BIOMASS_LEAF_tDM]/GC_GDM;
		logger(g_debug_log, "Ok Leaf biomass..\n");
		logger(g_debug_log, "---Leaf Biomass from init file = %g tDM cell\n", s->value[BIOMASS_LEAF_tDM]);
		logger(g_debug_log, "---Leaf Biomass from init file = %f tC cell\n", s->value[LEAF_C]);

		/* if no values for LAI are available */
		if ( !s->value[LAI_PROJ] )
		{
			logger(g_debug_log, "\nNo LAI Data are available for model initialization \n");
			logger(g_debug_log, "...Generating input LAI data from Leaf Biomass\n");

			logger(g_debug_log, "CANOPY_COVER_PROJ = %g\n", s->value[CANOPY_COVER_PROJ]);

			/* Calculate projected LAI for tot and for sunlit and shaded canopy portions*/
			s->value[LAI_PROJ] = ((s->value[LEAF_C] * 1e3) * s->value[SLA_AVG])/(s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
			s->value[LAI_SUN_PROJ] = 1.0 - exp(-s->value[LAI_PROJ]);
			s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];

			logger(g_debug_log, "LAI_PROJ = %f\n", s->value[LAI_PROJ]);
			logger(g_debug_log, "LAI SUN = %f\n", s->value[LAI_SUN_PROJ]);
			logger(g_debug_log, "LAI SHADE = %f\n", s->value[LAI_SHADE_PROJ]);
		}
		else
		{
			logger(g_debug_log, "Ok LAI..\n");
		}
	}

	s->value[AV_LEAF_MASS_C] = s->value[LEAF_C] / s->counter[N_TREE];
	logger(g_debug_log, "-Individual foliage biomass = %g tC\n", s->value[AV_LEAF_MASS_C]);

	//FIXME MODEL ASSUMES THAT IF NO FINE-ROOT BIOMASS ARE AVAILABLE THE SAME RATIO FOLIAGE-FINE ROOTS is used
	if (( s->value[BIOMASS_FROOT_tDM] == 0.0 || s->value[BIOMASS_FROOT_tDM] == NO_DATA)
			&& (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		logger(g_debug_log, "\nNo Fine root Biomass Data are available for model initialization \n");
		logger(g_debug_log, "...Generating input Fine root Biomass biomass data from DBH data...\n");
		/* assuming FINE_ROOT_LEAF RATIO AS IN BIOME */
		s->value[BIOMASS_FROOT_tDM] = s->value[BIOMASS_LEAF_tDM] * s->value[FINE_ROOT_LEAF];
		s->value[FROOT_C] = s->value[LEAF_C] * s->value[FINE_ROOT_LEAF];
		logger(g_debug_log, "---Fine Root Biomass initialization data from Leaf Biomass = %g tC cell\n", s->value[FROOT_C]);
	}
	else
	{
		/* assuming no fine root at 1st of January */
		s->value[BIOMASS_FROOT_tDM]     = 0.0;
		s->value[FROOT_C]               = 0.0;
		logger(g_debug_log, "Ok fine root biomass..\n");
		logger(g_debug_log, "---Fine Root Biomass from init file  = %g tC cell\n", s->value[FROOT_C]);
	}
	s->value[AV_FROOT_MASS_KgDM] = s->value[BIOMASS_FROOT_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_FROOT_MASS_C] = s->value[FROOT_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual fine root biomass = %g tC\n", s->value[AV_FROOT_MASS_C]);

	/***** COMPUTE LIVE BIOMASS *****/

	/* assuming LIVE_DEAD WOOD RATIO AS IN BIOME */
	logger(g_debug_log, "\n*******************************\n");

	/*FOR STEM*/
	live_total_wood_age( a, s );
	logger(g_debug_log, "Total Stem Biomass = %g tC cell\n", s->value[STEM_C]);
	s->value[BIOMASS_STEM_LIVE_WOOD_tDM] = s->value[BIOMASS_STEM_tDM] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[STEM_LIVE_WOOD_C]= s->value[STEM_C] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	logger(g_debug_log, "-Live Stem Biomass = %g tC cell\n", s->value[STEM_LIVE_WOOD_C]);
	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] = s->value[BIOMASS_STEM_tDM] -s->value[BIOMASS_STEM_LIVE_WOOD_tDM];
	s->value[STEM_DEAD_WOOD_C]= s->value[STEM_C] -s->value[STEM_LIVE_WOOD_C];
	logger(g_debug_log, "-Dead Stem Biomass = %g tC cell\n", s->value[STEM_DEAD_WOOD_C]);
	s->value[AV_LIVE_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_LIVE_STEM_MASS_C] = s->value[STEM_LIVE_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual live wood biomass = %g tC\n", s->value[AV_LIVE_STEM_MASS_C]);
	s->value[AV_DEAD_STEM_MASS_KgDM] = s->value[BIOMASS_STEM_DEAD_WOOD_tDM] * 1e3 / s->counter[N_TREE];
	s->value[AV_DEAD_STEM_MASS_C] = s->value[STEM_DEAD_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual dead wood biomass = %g tC\n", s->value[AV_DEAD_STEM_MASS_C]);
	CHECK_CONDITION(fabs((s->value[STEM_C])-(s->value[STEM_LIVE_WOOD_C] + s->value[STEM_DEAD_WOOD_C])), >,eps);
	CHECK_CONDITION(fabs((s->value[AV_STEM_MASS_C])-(s->value[AV_LIVE_STEM_MASS_C] + s->value[AV_DEAD_STEM_MASS_C])), >,eps);

	/* FOR COARSE ROOT */
	s->value[BIOMASS_CROOT_LIVE_WOOD_tDM]= s->value[BIOMASS_CROOT_tDM] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[CROOT_LIVE_WOOD_C]= s->value[CROOT_C] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	logger(g_debug_log, "-Live Coarse Root Biomass = %g tC cell\n", s->value[CROOT_LIVE_WOOD_C]);
	s->value[BIOMASS_CROOT_DEAD_WOOD_tDM]= s->value[BIOMASS_CROOT_tDM] -s->value[BIOMASS_CROOT_LIVE_WOOD_tDM];
	s->value[CROOT_DEAD_WOOD_C]= s->value[CROOT_C] -s->value[CROOT_LIVE_WOOD_C];
	logger(g_debug_log, "-Dead Coarse Root Biomass = %g tC cell\n", s->value[CROOT_DEAD_WOOD_C]);
	s->value[AV_LIVE_CROOT_MASS_KgDM] = s->value[BIOMASS_CROOT_LIVE_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_LIVE_CROOT_MASS_C] = s->value[CROOT_LIVE_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual live coarse root biomass = %g tC\n", s->value[AV_LIVE_CROOT_MASS_C]);
	s->value[AV_DEAD_CROOT_MASS_KgDM] = s->value[BIOMASS_CROOT_DEAD_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_DEAD_CROOT_MASS_C] = s->value[CROOT_DEAD_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual live coarse root biomass = %g tC\n", s->value[AV_DEAD_CROOT_MASS_C]);

	/* check */
	CHECK_CONDITION(fabs((s->value[CROOT_C])-(s->value[CROOT_LIVE_WOOD_C] + s->value[CROOT_DEAD_WOOD_C])), >,eps);
	CHECK_CONDITION(fabs((s->value[AV_CROOT_MASS_C])-(s->value[AV_LIVE_CROOT_MASS_C] + s->value[AV_DEAD_CROOT_MASS_C])), >,eps);

	/* FOR BRANCH */
	logger(g_debug_log, "Total BB Branch = %g tC/cell\n", s->value[BRANCH_C]);
	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVE_WOOD_C]= s->value[BRANCH_C] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	logger(g_debug_log, "-Live Stem Branch Biomass = %g tC cell\n", s->value[BRANCH_LIVE_WOOD_C]);
	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]= s->value[BIOMASS_BRANCH_tDM] - s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[BRANCH_DEAD_WOOD_C]= s->value[BRANCH_C] -s->value[BRANCH_LIVE_WOOD_C];
	logger(g_debug_log, "-Dead Stem Branch Biomass = %g tC cell\n", s->value[BRANCH_DEAD_WOOD_C]);
	s->value[AV_LIVE_BRANCH_MASS_KgDM] = s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_LIVE_BRANCH_MASS_C] = s->value[BRANCH_LIVE_WOOD_C] / s->counter[N_TREE];
	logger(g_debug_log, "-Individual live branch biomass = %g tC\n", s->value[AV_LIVE_BRANCH_MASS_C]);
	s->value[AV_DEAD_BRANCH_MASS_KgDM] = s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_DEAD_BRANCH_MASS_C] = s->value[BRANCH_DEAD_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual dead branch biomass = %g tC\n", s->value[AV_DEAD_BRANCH_MASS_C]);

	/* check */
	CHECK_CONDITION(fabs((s->value[BRANCH_C])-(s->value[BRANCH_LIVE_WOOD_C] + s->value[BRANCH_DEAD_WOOD_C])), >,eps);
	CHECK_CONDITION(fabs((s->value[AV_BRANCH_MASS_C])-(s->value[AV_LIVE_BRANCH_MASS_C] + s->value[AV_DEAD_BRANCH_MASS_C])), >,eps);

	s->value[BIOMASS_LIVE_WOOD_tDM] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM] +
			s->value[BIOMASS_CROOT_LIVE_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
	s->value[LIVE_WOOD_C] = s->value[STEM_LIVE_WOOD_C] +
			s->value[CROOT_LIVE_WOOD_C]+
			s->value[BRANCH_LIVE_WOOD_C];
	logger(g_debug_log, "---Live biomass = %g tC/cell\n", s->value[LIVE_WOOD_C]);
	s->value[AV_LIVE_WOOD_MASS_KgDM] = s->value[BIOMASS_LIVE_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_LIVE_WOOD_MASS_C] = s->value[LIVE_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual total live biomass = %g tC\n", s->value[AV_LIVE_WOOD_MASS_C]);

	s->value[BIOMASS_DEAD_WOOD_tDM] = s->value[BIOMASS_STEM_DEAD_WOOD_tDM]+
			s->value[BIOMASS_CROOT_DEAD_WOOD_tDM]+
			s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM];
	s->value[DEAD_WOOD_C] = s->value[STEM_DEAD_WOOD_C]+
			s->value[CROOT_DEAD_WOOD_C]+
			s->value[BRANCH_DEAD_WOOD_C];
	logger(g_debug_log, "---Dead biomass = %g tC/cell\n", s->value[DEAD_WOOD_C]);
	s->value[AV_DEAD_WOOD_MASS_KgDM] = s->value[BIOMASS_DEAD_WOOD_tDM] * 1e3 / (double)s->counter[N_TREE];
	s->value[AV_DEAD_WOOD_MASS_C] = s->value[DEAD_WOOD_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Individual total dead biomass = %g tC\n", s->value[AV_DEAD_WOOD_MASS_C]);

	/* compute percentage of live vs total biomass */
	s->value[TOT_WOOD_C] = s->value[STEM_C] + s->value[CROOT_C] + s->value[BRANCH_C];

	logger(g_debug_log, "----Total wood = %g tC/cell\n",s->value[TOT_WOOD_C]);
	logger(g_debug_log, "----Total wood = %g tC/tree\n",s->value[AV_TOT_WOOD_MASS_C]);
	logger(g_debug_log, "----Live wood vs total biomass = %g %%\n", (s->value[LIVE_WOOD_C] / s->value[TOT_WOOD_C]) * 100.0);
	logger(g_debug_log, "----Dead wood vs total biomass = %g %%\n", (s->value[DEAD_WOOD_C] / s->value[TOT_WOOD_C]) * 100.0);

	logger(g_debug_log, "***reserves following live tissues C (not used) BIOME = %g tC/cell\n", s->value[LIVE_WOOD_C] * s->value[SAP_WRES] );

	/* fruit */
	s->value[FRUIT_C]         = 0.;
	s->value[AV_FRUIT_MASS_C] = 0.;

	/* compute AGB and BGB */
	logger(g_debug_log, "**AGB & BGB**\n");
	s->value[AGB]      = s->value[STEM_C] + s->value[BRANCH_C] + s->value[LEAF_C] + s->value[FRUIT_C];
	logger(g_debug_log, "Yearly Class AGB = %g tC/cell\n", s->value[AGB]);
	s->value[BGB]      = s->value[FROOT_C] + s->value[CROOT_C];
	logger(g_debug_log, "Yearly Class BGB = %g tC/cell\n", s->value[BGB]);
	s->value[TREE_AGB] = s->value[AGB] / (double)s->counter[N_TREE];
	logger(g_debug_log, "Yearly Class AGB = %g tC/tree\n", s->value[TREE_AGB]);
	s->value[TREE_BGB] = s->value[BGB] / (double)s->counter[N_TREE];
	logger(g_debug_log, "Yearly Class BGB = %g tC/tree\n", s->value[TREE_BGB]);

	/* check that all mandatory variables are initialized */
	CHECK_CONDITION(h->value, ==, 0);
	CHECK_CONDITION(d->value, ==, 0);
	/* note: ISIMIP special case */
	CHECK_CONDITION(a->value, ==, 0);

	/* ONLY for evergreen */
	if ( s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2 )
	{
		CHECK_CONDITION(s->value[FROOT_C],             <=, ZERO);
		CHECK_CONDITION(s->value[LEAF_C],              <=, ZERO);
		CHECK_CONDITION(s->value[LAI_PROJ],            <=, ZERO);
		CHECK_CONDITION(s->value[LAI_SUN_PROJ],        <=, ZERO);
		CHECK_CONDITION(s->value[LAI_SHADE_PROJ],      <=, ZERO);
	}
	CHECK_CONDITION(s->value[STEM_C],                  <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_C],                <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_C],                 <=, ZERO);
	CHECK_CONDITION(s->value[TOT_WOOD_C],              <=, ZERO);
	CHECK_CONDITION(s->value[RESERVE_C],               <=, ZERO);
	CHECK_CONDITION(s->value[MIN_RESERVE_C],           <=, ZERO);
	CHECK_CONDITION(s->value[STEM_SAPWOOD_C],          <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_SAPWOOD_C],         <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_SAPWOOD_C],        <=, ZERO);
	CHECK_CONDITION(s->value[TOT_SAPWOOD_C],           <=, ZERO);
	CHECK_CONDITION(s->value[STEM_LIVE_WOOD_C],        <=, ZERO);
	CHECK_CONDITION(s->value[STEM_DEAD_WOOD_C],        <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_LIVE_WOOD_C],       <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_DEAD_WOOD_C],       <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_LIVE_WOOD_C],      <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_DEAD_WOOD_C],      <=, ZERO);
	CHECK_CONDITION(s->value[LIVE_WOOD_C],             <=, ZERO);
	CHECK_CONDITION(s->value[DEAD_WOOD_C],             <=, ZERO);
	CHECK_CONDITION(s->value[BASAL_AREA],              <=, ZERO);
	CHECK_CONDITION(s->value[AGB],                     <=, ZERO);
	CHECK_CONDITION(s->value[BGB],                     <=, ZERO);
	CHECK_CONDITION(s->value[VOLUME],                  <=, ZERO);
	CHECK_CONDITION(s->value[TREE_VOLUME],             <=, ZERO);



}

void initialization_forest_C (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/*** update at cell level (gC/m2)  ***/
	c->leaf_carbon              += (s->value[LEAF_C]             * 1e6 / g_settings->sizeCell);
	c->froot_carbon             += (s->value[FROOT_C]            * 1e6 / g_settings->sizeCell);
	c->stem_carbon              += (s->value[STEM_C]             * 1e6 / g_settings->sizeCell);
	c->stem_live_wood_carbon    += (s->value[STEM_LIVE_WOOD_C]   * 1e6 / g_settings->sizeCell);
	c->stem_dead_wood_carbon    += (s->value[STEM_DEAD_WOOD_C]   * 1e6 / g_settings->sizeCell);
	c->croot_carbon             += (s->value[CROOT_C]            * 1e6 / g_settings->sizeCell);
	c->croot_live_wood_carbon   += (s->value[CROOT_LIVE_WOOD_C]  * 1e6 / g_settings->sizeCell);
	c->croot_dead_wood_carbon   += (s->value[CROOT_DEAD_WOOD_C]  * 1e6 / g_settings->sizeCell);
	c->branch_carbon            += (s->value[BRANCH_C]           * 1e6 / g_settings->sizeCell);
	c->branch_live_wood_carbon  += (s->value[BRANCH_LIVE_WOOD_C] * 1e6 / g_settings->sizeCell);
	c->branch_dead_wood_carbon  += (s->value[BRANCH_DEAD_WOOD_C] * 1e6 / g_settings->sizeCell);
	c->reserve_carbon           += (s->value[RESERVE_C]          * 1e6 / g_settings->sizeCell);
	c->fruit_carbon             += (s->value[FRUIT_C]            * 1e6 / g_settings->sizeCell);

	/* check */
	CHECK_CONDITION(c->leaf_carbon,                    <=, ZERO);
	CHECK_CONDITION(c->froot_carbon,                   <=, ZERO);
	CHECK_CONDITION(c->stem_carbon,                    <=, ZERO);
	CHECK_CONDITION(c->stem_live_wood_carbon,          <=, ZERO);
	CHECK_CONDITION(c->stem_dead_wood_carbon,          <=, ZERO);
	CHECK_CONDITION(c->croot_carbon,                   <=, ZERO);
	CHECK_CONDITION(c->croot_live_wood_carbon,         <=, ZERO);
	CHECK_CONDITION(c->croot_live_wood_carbon,         <=, ZERO);
	CHECK_CONDITION(c->branch_carbon,                  <=, ZERO);
	CHECK_CONDITION(c->branch_live_wood_carbon,        <=, ZERO);
	CHECK_CONDITION(c->branch_dead_wood_carbon,        <=, ZERO);
	CHECK_CONDITION(c->reserve_carbon,                 <=, ZERO);
	CHECK_CONDITION(c->fruit_carbon,                   <=, ZERO);

}

void initialization_forest_class_N (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log,"\n*******INITIALIZE FOREST NITROGEN POOLS*******\n");

	/* leaf */
	s->value[LEAF_N] = s->value[LEAF_C] / s->value[CN_LEAVES];
	logger(g_debug_log, "----Leaf nitrogen content = %g tN/cell\n", s->value[LEAF_N]);

	/* fine root */
	s->value[FROOT_N] = s->value[FROOT_C] / s->value[CN_FINE_ROOTS];
	logger(g_debug_log, "----Fine root nitrogen content = %g tN/cell\n", s->value[FROOT_N]);

	/* stem */
	s->value[STEM_LIVE_WOOD_N] = s->value[STEM_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "----Live stem nitrogen content = %g tN/cell\n", s->value[STEM_LIVE_WOOD_N]);

	s->value[STEM_DEAD_WOOD_N] = s->value[STEM_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	logger(g_debug_log, "----Dead stem nitrogen content = %g tN/cell\n", s->value[STEM_DEAD_WOOD_N]);

	s->value[STEM_N] = s->value[STEM_LIVE_WOOD_N] + s->value[STEM_DEAD_WOOD_N];
	logger(g_debug_log, "----Stem nitrogen content = %g tN/cell\n", s->value[STEM_N]);

	/* coarse root */
	s->value[CROOT_LIVE_WOOD_N] = s->value[CROOT_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "----Live coarse root nitrogen content = %g tN/cell\n", s->value[CROOT_LIVE_WOOD_N]);

	s->value[CROOT_DEAD_WOOD_N] = s->value[CROOT_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	logger(g_debug_log, "----Dead coarse root nitrogen content = %g tN/cell\n", s->value[CROOT_DEAD_WOOD_N]);

	s->value[CROOT_N] = s->value[CROOT_LIVE_WOOD_N] + s->value[CROOT_DEAD_WOOD_N];
	logger(g_debug_log, "----Coarse root nitrogen content = %g tN/cell\n", s->value[CROOT_N]);

	/* branch */
	s->value[BRANCH_LIVE_WOOD_N] = s->value[BRANCH_LIVE_WOOD_C] / s->value[CN_LIVE_WOODS];
	logger(g_debug_log, "----Live branch nitrogen content = %g tN/cell\n", s->value[BRANCH_LIVE_WOOD_N]);

	s->value[BRANCH_DEAD_WOOD_N] = s->value[BRANCH_DEAD_WOOD_C] / s->value[CN_DEAD_WOODS];
	logger(g_debug_log, "----Dead branch nitrogen content = %g tN/cell\n", s->value[BRANCH_DEAD_WOOD_N]);

	s->value[BRANCH_N] = s->value[BRANCH_LIVE_WOOD_N] + s->value[BRANCH_DEAD_WOOD_N];
	logger(g_debug_log, "----Branch nitrogen content = %g tN/cell\n", s->value[BRANCH_N]);

	/* check that all mandatory variables are initialized */
	CHECK_CONDITION(s->value[STEM_N],          <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_N],         <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_N],        <=, ZERO);

	/* just for evergreen */
	if ( s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2 )
	{
		CHECK_CONDITION(s->value[LEAF_N],      <=, ZERO);
		CHECK_CONDITION(s->value[FROOT_N],     <=, ZERO);
	}
}

void initialization_forest_N (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/*** update at cell level (gN/m2)  ***/
	c->leaf_nitrogen              += (s->value[LEAF_N]             * 1e6 / g_settings->sizeCell);
	c->froot_nitrogen             += (s->value[FROOT_N]            * 1e6 / g_settings->sizeCell);
	c->stem_nitrogen              += (s->value[STEM_N]             * 1e6 / g_settings->sizeCell);
	c->stem_live_wood_nitrogen    += (s->value[STEM_LIVE_WOOD_N]   * 1e6 / g_settings->sizeCell);
	c->stem_dead_wood_nitrogen    += (s->value[STEM_DEAD_WOOD_N]   * 1e6 / g_settings->sizeCell);
	c->croot_nitrogen             += (s->value[CROOT_N]            * 1e6 / g_settings->sizeCell);
	c->croot_live_wood_nitrogen   += (s->value[CROOT_LIVE_WOOD_N]  * 1e6 / g_settings->sizeCell);
	c->croot_dead_wood_nitrogen   += (s->value[CROOT_DEAD_WOOD_N]  * 1e6 / g_settings->sizeCell);
	c->branch_nitrogen            += (s->value[BRANCH_N]           * 1e6 / g_settings->sizeCell);
	c->branch_live_wood_nitrogen  += (s->value[BRANCH_LIVE_WOOD_N] * 1e6 / g_settings->sizeCell);
	c->branch_dead_wood_nitrogen  += (s->value[BRANCH_DEAD_WOOD_N] * 1e6 / g_settings->sizeCell);
	c->reserve_nitrogen           += (s->value[RESERVE_N]          * 1e6 / g_settings->sizeCell);
	c->fruit_nitrogen             += (s->value[FRUIT_N]            * 1e6 / g_settings->sizeCell);

	/* check */
	CHECK_CONDITION(c->leaf_nitrogen,                    <=, ZERO);
	CHECK_CONDITION(c->froot_nitrogen,                   <=, ZERO);
	CHECK_CONDITION(c->stem_nitrogen,                    <=, ZERO);
	CHECK_CONDITION(c->stem_live_wood_nitrogen,          <=, ZERO);
	CHECK_CONDITION(c->stem_dead_wood_nitrogen,          <=, ZERO);
	CHECK_CONDITION(c->croot_nitrogen,                   <=, ZERO);
	CHECK_CONDITION(c->croot_live_wood_nitrogen,         <=, ZERO);
	CHECK_CONDITION(c->croot_live_wood_nitrogen,         <=, ZERO);
	CHECK_CONDITION(c->branch_nitrogen,                  <=, ZERO);
	CHECK_CONDITION(c->branch_live_wood_nitrogen,        <=, ZERO);
	CHECK_CONDITION(c->branch_dead_wood_nitrogen,        <=, ZERO);
	CHECK_CONDITION(c->reserve_nitrogen,                 <=, ZERO);
	CHECK_CONDITION(c->fruit_nitrogen,                   <=, ZERO);


}

void initialization_forest_class_litter_soil (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	double r1;
	double temp_var;                                /* temporary variable */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* check */
	CHECK_CONDITION ( fabs (s->value[LEAF_LITR_LAB_FRAC] + s->value[LEAF_LITR_CEL_FRAC] + s->value[LEAF_LITR_LIGN_FRAC]), >, 1 + eps);
	CHECK_CONDITION ( fabs (s->value[FROOT_LITR_LAB_FRAC] + s->value[FROOT_LITR_CEL_FRAC] + s->value[FROOT_LITR_LIGN_FRAC]), >, 1 + eps);
	CHECK_CONDITION ( fabs (s->value[DEAD_WOOD_CEL_FRAC] + s->value[DEAD_WOOD_LIGN_FRAC]), >, 1 + eps);


	/** calculate shielded and unshielded cellulose fraction **/

	/*** compute leaf litter fractions ***/

	/* partitioning leaf litter carbon cellulose into shielded and unshielded pools */
	r1 = s->value[LEAF_LITR_LIGN_FRAC] / s->value[LEAF_LITR_LAB_FRAC];

	if ( r1 <= 0.45 )
	{
		s->value[LEAF_LITR_SCEL_FRAC]   = 0.;
		s->value[LEAF_LITR_USCEL_FRAC]  = s->value[LEAF_LITR_CEL_FRAC];
	}
	else if ( r1 > 0.45 && r1 < 0.7 )
	{
		temp_var = ( r1 - 0.45 ) * 3.2;
		s->value[LEAF_LITR_SCEL_FRAC]   = temp_var * s->value[LEAF_LITR_CEL_FRAC];
		s->value[LEAF_LITR_USCEL_FRAC]  = ( 1. - temp_var ) * s->value[LEAF_LITR_CEL_FRAC];
	}
	else
	{
		s->value[LEAF_LITR_SCEL_FRAC]   = 0.8 * s->value[LEAF_LITR_CEL_FRAC];
		s->value[LEAF_LITR_USCEL_FRAC]  = 0.2 * s->value[LEAF_LITR_CEL_FRAC];
	}

	/*** compute fine root litter fractions ***/

	/* partitioning fine root litter carbon cellulose into shielded and unshielded pools */
	r1 = s->value[FROOT_LITR_LIGN_FRAC] / s->value[FROOT_LITR_LAB_FRAC];

	if ( r1 <= 0.45 )
	{
		s->value[FROOT_LITR_SCEL_FRAC]   = 0.;
		s->value[FROOT_LITR_USCEL_FRAC]  = s->value[FROOT_LITR_CEL_FRAC];
	}
	else if ( r1 > 0.45 && r1 < 0.7 )
	{
		temp_var = ( r1 - 0.45 ) * 3.2;
		s->value[FROOT_LITR_SCEL_FRAC]   = temp_var * s->value[FROOT_LITR_CEL_FRAC];
		s->value[FROOT_LITR_USCEL_FRAC]  = ( 1. - temp_var ) * s->value[FROOT_LITR_CEL_FRAC];
	}
	else
	{
		s->value[FROOT_LITR_SCEL_FRAC]   = 0.8 * s->value[FROOT_LITR_CEL_FRAC];
		s->value[FROOT_LITR_USCEL_FRAC]  = 0.2 * s->value[FROOT_LITR_CEL_FRAC];
	}

	/*** compute coarse woody debris litter fractions ***/

	/* partitioning coarse woody debris litter carbon cellulose into shielded and unshielded pools */
	r1 = s->value[DEAD_WOOD_LIGN_FRAC] / s->value[DEAD_WOOD_CEL_FRAC];
	if ( r1 <= 0.45 )
	{
		s->value[DEAD_WOOD_SCEL_FRAC]  = 0.;
		s->value[DEAD_WOOD_USCEL_FRAC] = s->value[DEAD_WOOD_CEL_FRAC];
	}
	else if ( r1 > 0.45 && r1 < 0.7 )
	{
		temp_var = ( r1 - 0.45 ) * 3.2;
		s->value[DEAD_WOOD_SCEL_FRAC]  = temp_var * s->value[DEAD_WOOD_CEL_FRAC];
		s->value[DEAD_WOOD_USCEL_FRAC] = (1.0 - temp_var) * s->value[DEAD_WOOD_CEL_FRAC];
	}
	else
	{
		s->value[DEAD_WOOD_SCEL_FRAC]  = 0.8 * s->value[DEAD_WOOD_CEL_FRAC];
		s->value[DEAD_WOOD_USCEL_FRAC] = 0.2 * s->value[DEAD_WOOD_CEL_FRAC];
	}
}

void initialization_soil_physic(cell_t *const c)
{
	float acoeff;
	float bcoeff;
	float sat;
	float volumetric_wilting_point;
	float volumetric_field_capacity;
	float volumetric_saturated_hydraulic_conductivity;

	logger(g_debug_log,"\nINITIALIZE SOIL PHYSIC\n");

	/*soil matric potential*/
	CHECK_CONDITION(fabs((g_soil_settings->values[SOIL_SAND_PERC] + g_soil_settings->values[SOIL_CLAY_PERC] + g_soil_settings->values[SOIL_SILT_PERC]) -100.0 ), >, eps);

	/* BIOME-BGC METHOD */
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

	logger(g_debug_log, "BIOME soil characteristics\n");

	/* (DIM) Clapp-Hornberger "b" parameter */
	c->soil_b = -(3.10 + 0.157*g_soil_settings->values[SOIL_CLAY_PERC] - 0.003*g_soil_settings->values[SOIL_SAND_PERC]); /* ok for schwalm*/
	logger(g_debug_log, "soil_b = %g (DIM)\n", c->soil_b);

	//test
	/* following Rawls et al., 1992 and Schwalm et al., 2004 */
	/*texture-dependent empirical coefficinet */
	// c->soil_b = 11.43 - (0.1034*g_soil_settings->values[SOIL_SAND_PERC) - (0.0687*0.157*g_soil_settings->values[SOIL_silt_perc);

	/* calculate the soil pressure-volume coefficients from texture data */
	/* Uses the multivariate regressions from Cosby et al., 1984 */
	/* (DIM) Soil volumetric water content at saturation */
	c->vwc_sat = (50.5 - 0.142*g_soil_settings->values[SOIL_SAND_PERC] - 0.037*g_soil_settings->values[SOIL_CLAY_PERC])/100.0; /* ok for schwalm*/
	logger(g_debug_log, "volumetric water content at saturation (BIOME) = %g %%(vol)\n", c->vwc_sat);
	/* (MPa) soil matric potential at saturation */
	c->psi_sat = -(exp((1.54 - 0.0095*g_soil_settings->values[SOIL_SAND_PERC] + 0.0063*g_soil_settings->values[SOIL_SILT_PERC])*log(10.0))*9.8e-5); /* ok for schwalm*/
	logger(g_debug_log, "psi_sat = %g MPa \n", c->psi_sat);
	/* Clapp-Hornenberger function 1978 (DIM) Soil Field Capacity Volumetric Water Content at field capacity ( = -0.015 MPa) */
	c->vwc_fc = c->vwc_sat * pow((-0.015/c->psi_sat),(1.0/c->soil_b));
	logger(g_debug_log, "volumetric water content at field capacity (BIOME) = %g %%(vol) \n", c->vwc_fc);

	/* define maximum soilwater content, for outflow calculation */
	/* converts volumetric water content (m3/m3) --> (kg/m2) */

	/* (kgH2O/m2) soil water at field capacity */
	c->soilw_fc = (g_soil_settings->values[SOIL_DEPTH] / 100) * c->vwc_fc * 1e3;
	logger(g_debug_log, "soilw_fc BIOME (MAXASW FC BIOME)= %g (kgH2O/m2)\n", c->soilw_fc);
	//equal to MAXASW

	/* (kgH2O/m2) soil water at saturation */
	c->soilw_sat = (g_soil_settings->values[SOIL_DEPTH] / 100) * c->vwc_sat * 1e3;
	logger(g_debug_log, "soilw_sat BIOME (MAXASW SAT BIOME)= %g (kgH2O/m2)\n", c->soilw_sat);

	/* (kgH2O/m2) maximum soil water at field capacity */
	c->max_asw_fc = c->soilw_fc;

	/* compute initialization soil water content */
	c->asw = (c->soilw_sat * g_settings->init_frac_maxasw);
	logger(g_debug_log, "Initialization ASW = %g (mm-kgH2O/m2)\n\n\n", c->asw);


	/** CENTURY METHOD **/

	/* soil data from https://www.nrel.colostate.edu/projects/century/soilCalculatorHelp.htm */
	/* following Saxton et al 1986, 2006, 2008 */
	logger(g_debug_log, "CENTURY soil characteristics\n");
	acoeff = (float)exp(-4.396 - 0.0715 * g_soil_settings->values[SOIL_CLAY_PERC] - 4.88e-4 * pow(g_soil_settings->values[SOIL_SAND_PERC],2) - 4.285e-5 * pow(g_soil_settings->values[SOIL_SAND_PERC],2)*g_soil_settings->values[SOIL_CLAY_PERC]);
	bcoeff = (float)(-3.14 - 0.00222 * pow(g_soil_settings->values[SOIL_CLAY_PERC],2) - 3.484e-5 * pow(g_soil_settings->values[SOIL_SAND_PERC],2) * g_soil_settings->values[SOIL_CLAY_PERC]);
	sat = (float)(0.332 - 7.251e-4 * g_soil_settings->values[SOIL_SAND_PERC] + 0.1276 * log10(g_soil_settings->values[SOIL_CLAY_PERC]));

	/* volumetric percentage for wilting point */
	volumetric_wilting_point = (float)pow((15.0/acoeff), (1.0/bcoeff));
	/* volumetric percentage for field capacity */
	volumetric_field_capacity = (float)pow((0.333/acoeff),(1.0/bcoeff));
	/* volumetric percentage for saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity = (float)exp((12.012 - 0.0755 * g_soil_settings->values[SOIL_SAND_PERC]) + (-3.895 + 0.03671 * g_soil_settings->values[SOIL_SAND_PERC] - 0.1103 * g_soil_settings->values[SOIL_CLAY_PERC] + 8.7546e-4 * pow(g_soil_settings->values[SOIL_CLAY_PERC],2))/sat);
	/* bulk density g/cm3 */
	c->bulk_density = (1 - sat) * 2.65;

	/* corrections from Steve Del Grosso */
	/* volumetric percentage wilting point */
	volumetric_wilting_point += (float)(-0.15 * volumetric_wilting_point);
	logger(g_debug_log, "volumetric water content at wilting point (CENTURY) = %g %%(vol)\n", volumetric_wilting_point);
	/* (kgH2O/m2) soil water at wilting point */
	c->wilting_point = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_wilting_point * 1e3;
	logger(g_debug_log, "Wilting point (CENTURY) = %g mm/m2\n", c->wilting_point);

	/* volumetric percentage field capacity */
	volumetric_field_capacity += (float)(0.07 * volumetric_field_capacity);
	logger(g_debug_log, "volumetric water content at field capacity (CENTURY) = %g %%(vol)\n", volumetric_field_capacity);
	/* (kgH2O/m2) soil water at field capacity */
	c->field_capacity = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_field_capacity * 1e3;
	logger(g_debug_log, "Field capacity (CENTURY) = %g mm/m2\n", c->field_capacity);

	/* volumetric percentage saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity /= 1500.0;
	logger(g_debug_log, "volumetric water content at saturated hydraulic conductance (CENTURY) = %g %%(vol)\n", volumetric_saturated_hydraulic_conductivity);
	//fixme not clear what it is
	/* (kgH2O/m2) soil water at saturated hydraulic conductivity */
	c->sat_hydr_conduct = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_saturated_hydraulic_conductivity * 1e3;
	logger(g_debug_log, "Saturated hydraulic conductivity (CENTURY) = %g mm/m2\n", c->sat_hydr_conduct);

	/* bulk density g/cm3 */
	c->bulk_density += (-0.08 * c->bulk_density);
	logger(g_debug_log, "Bulk density = %g g/cm^3\n", c->bulk_density);
	logger(g_debug_log, "***************************************************\n\n");

}

void initialization_soil_biogeochemistry (cell_t *const c)
{
	//todo create variables related to percentage of presence for each single species (also at the initialization phase?)

	double r1;
	double temp_var;
	double deadwood_litter_cellulose;

	/* fractions */
	double leaf_froot_frac = 1.;                                          /* (dim) leaf fine root fraction */
	double leaf_litt_scel_frac;                                           /* (dim) leaf litter shielded cellulose fraction */
	double leaf_litt_uscel_frac;                                          /* (dim) leaf litter unshielded cellulose fraction */
	double froot_litt_scel_frac;                                          /* (dim) fine root litter shielded cellulose fraction */
	double froot_litt_uscel_frac;                                         /* (dim) fine root litter unshielded fraction */
	double deadwood_scel_frac;                                            /* (dim) dead wood litter shielded cellulose fraction */
	double deadwood_uscel_frac;                                           /* (dim) dead wood litter unshielded fraction */

	/* todo: if cell is monospecific than use values of species level instead generic */
	/* assumption: arbitrary values for fractions (assuming averaged values among species) */
	double leaf_litter_labile_frac        = 0.20;
	double leaf_litter_cellulose_frac     = 0.50;
	double leaf_litter_lignin_frac        = 0.30;
	double froot_litter_labile_frac       = 0.30;
	double froot_litter_cellulose_frac    = 0.40;
	double froot_litter_lignin_frac       = 0.30;
	double deadwood_litter_cellulose_frac = 0.75;
	double deadwood_litter_lignin_frac    = 0.25;

	/* assumption: arbitrary values for C:N ratio (assuming averaged values among species) */
	double leaf_litter_CN_frac            = 100.;
	double froot_litter_CN_frac           = 60.;
	double deadwood_litter_CN_frac        = 600.;

	/* carbon pools */
	double leaf_litterC;
	double froot_litterC;
	double cwd_litterC;
	double litterC;

	/* nitrogen pools */
	double leaf_litterN;
	double froot_litterN;
	double cwd_litterN;
	double litterN;

	/*************************************** CARBON POOLS ***************************************/
	/* note: BIOME-BGC method for fractions of pools */

	/* share litter carbon among leaf and fine root tC/sizecell -> KgC/m2 */
	//assumption: litter is shared based on "leaf_froot_frac" between leaf and fine root litter pools

	/* compute carbon pools */
	litterC       = g_soil_settings->values[LITTERC] * (1000 / g_settings->sizeCell);
	leaf_litterC  = litterC * ( leaf_froot_frac / 2. );
	froot_litterC = litterC - leaf_litterC;

	/****************** compute leaf litter pool ******************/

	/* check for litter fractions sum to 1.0 */
	CHECK_CONDITION (fabs(leaf_litter_labile_frac + leaf_litter_cellulose_frac + leaf_litter_lignin_frac-1.), > , eps);

	/* leaf litter carbon labile pool */
	c->leaf_litr1C = leaf_litterC * leaf_litter_labile_frac;

	/* leaf litter carbon lignin pool */
	c->leaf_litr4C = leaf_litterC * leaf_litter_lignin_frac;

	/*** compute leaf litter fractions ***/

	/* partitioning leaf litter carbon cellulose into shielded and unshielded pools */
	r1 = leaf_litter_lignin_frac / leaf_litter_labile_frac;

	if ( r1 <= 0.45 )
	{
		leaf_litt_scel_frac  = 0.;
		leaf_litt_uscel_frac = leaf_litter_cellulose_frac;
	}
	else if ( r1 > 0.45 && r1 < 0.7 )
	{
		temp_var             = ( r1 - 0.45 ) * 3.2;
		leaf_litt_scel_frac  = temp_var * leaf_litter_cellulose_frac;
		leaf_litt_uscel_frac = ( 1. - temp_var ) * leaf_litter_cellulose_frac;
	}
	else
	{
		leaf_litt_scel_frac  = 0.8 * leaf_litter_cellulose_frac;
		leaf_litt_uscel_frac = 0.2 * leaf_litter_cellulose_frac;
	}

	/* compute remaining pools */
	/* leaf litter litter unshielded cellulose C */
	c->leaf_litr2C = leaf_litterC * leaf_litt_uscel_frac;

	/* leaf litter litter shielded cellulose C */
	c->leaf_litr3C = leaf_litterC * leaf_litt_scel_frac;

	/* check */
	CHECK_CONDITION ( fabs(c->leaf_litr1C + c->leaf_litr2C + c->leaf_litr3C + c->leaf_litr4C - leaf_litterC), > , eps);

	/****************** compute fine root litter pool ******************/

	/* check for fine root fractions sum to 1.0 */
	CHECK_CONDITION (fabs(froot_litter_labile_frac + froot_litter_cellulose_frac + froot_litter_lignin_frac-1.), > , eps);

	/* litter fine root carbon labile pool */
	c->froot_litr1C = froot_litterC * froot_litter_labile_frac;

	/* litter fine root carbon lignin pool */
	c->froot_litr4C = froot_litterC * froot_litter_lignin_frac;

	/*** compute fine root litter fractions ***/

	/* partitioning fine root litter carbon cellulose into shielded and unshielded pools */
	r1 = froot_litter_lignin_frac / froot_litter_labile_frac;

	if ( r1 <= 0.45 )
	{
		froot_litt_scel_frac  = 0.;
		froot_litt_uscel_frac = froot_litter_cellulose_frac;
	}
	else if ( r1 > 0.45 && r1 < 0.7 )
	{
		temp_var              = ( r1 - 0.45 ) * 3.2;
		froot_litt_scel_frac  = temp_var * froot_litter_cellulose_frac;
		froot_litt_uscel_frac = ( 1. - temp_var ) * froot_litter_cellulose_frac;
	}
	else
	{
		froot_litt_scel_frac  = 0.8 * froot_litter_cellulose_frac;
		froot_litt_uscel_frac = 0.2 * froot_litter_cellulose_frac;
	}

	/* compute remaining pools */
	/* fine root litter litter unshielded cellulose C */
	c->froot_litr2C = froot_litterC * froot_litt_uscel_frac;

	/* fine root litter litter shielded cellulose C */
	c->froot_litr3C = froot_litterC * froot_litt_scel_frac;

	/* check */
	CHECK_CONDITION ( fabs(c->froot_litr1C + c->froot_litr2C + c->froot_litr3C + c->froot_litr4C - froot_litterC), > , eps);

	/****************** coarse woody debris litter pool ******************/

	/* check for coarse woody debris fractions sum to 1.0 */
	CHECK_CONDITION (fabs(deadwood_litter_cellulose_frac + deadwood_litter_lignin_frac-1.), > , eps);

	/* coarse woody debris carbon cellulose pool tC/sizecell -> kgC/m2 */
	cwd_litterC = g_soil_settings->values[LITTERCWDC] * (1000 / g_settings->sizeCell);
	deadwood_litter_cellulose = cwd_litterC * deadwood_litter_cellulose_frac;

	/* coarse woody debris carbon lignin pool */
	c->deadwood_litr4C = cwd_litterC - deadwood_litter_cellulose;

	/*** compute coarse woody debris litter fractions ***/

	/* partitioning coarse woody debris litter carbon cellulose into shielded and unshielded pools */
	r1 = deadwood_litter_lignin_frac / deadwood_litter_cellulose;
	if ( r1 <= 0.45 )
	{
		deadwood_scel_frac  = 0.0;
		deadwood_uscel_frac = deadwood_litter_cellulose_frac;
	}
	else if ( r1 > 0.45 && r1 < 0.7 )
	{
		temp_var            = ( r1 - 0.45 ) * 3.2;
		deadwood_scel_frac  = temp_var * deadwood_litter_cellulose_frac;
		deadwood_uscel_frac = (1.0 - temp_var) * deadwood_litter_cellulose_frac;
	}
	else
	{
		deadwood_scel_frac  = 0.8 * deadwood_litter_cellulose_frac;
		deadwood_uscel_frac = 0.2 * deadwood_litter_cellulose_frac;
	}

	/* compute remaining pools */
	/* coarse_woody debris litter litter unshielded cellulose C */
	c->deadwood_litr2C = cwd_litterC * deadwood_uscel_frac;

	/* coarse_woody debris litter litter shielded cellulose C */
	c->deadwood_litr3C = cwd_litterC * deadwood_scel_frac;

	/* check */
	CHECK_CONDITION ( fabs( c->deadwood_litr2C + c->deadwood_litr3C + c->deadwood_litr4C - cwd_litterC ), > , eps);


	/****************** SUM ALL OVER CARBON POOLS ******************/
	/* compute litter total (litter + cwd) carbon pool */
	c->litrC   = litterC;

	/* compute litter total labile carbon pool */
	c->litr1C = c->leaf_litr1C + c->froot_litr1C;

	/* compute litter total unshielded cellulose carbon pool */
	c->litr2C = c->leaf_litr2C + c->froot_litr2C + c->deadwood_litr2C;

	/* compute litter total shielded cellulose carbon pool */
	c->litr3C = c->leaf_litr3C + c->froot_litr3C + c->deadwood_litr3C;

	/* compute litter total lignin carbon pool */
	c->litr4C = c->leaf_litr4C + c->froot_litr4C + c->deadwood_litr4C;

	c->cwdC   = cwd_litterC;

	/* check */
	CHECK_CONDITION ( fabs((c->litr1C + c->litr2C + c->litr3C + c->litr4C) - (c->litrC)), > , eps);


	/*************************************** NITROGEN POOLS ***************************************/

	//todo check when litterN != 0 (-9999)

	//this case when litterN = 0 (-9999)

	/* compute nitrogen pools */
	leaf_litterN  = leaf_litterC / leaf_litter_CN_frac;
	froot_litterN = froot_litterC / froot_litter_CN_frac;
	cwd_litterN   = cwd_litterC / deadwood_litter_CN_frac;
	litterN       = leaf_litterN + froot_litterN;

	/*** compute leaf litter nitrogen pool ***/
	/* compute leaf litter total labile nitrogen pool */
	c->leaf_litr1N = leaf_litterN * leaf_litter_labile_frac;

	/* leaf litter litter unshielded cellulose nitrogen pool */
	c->leaf_litr2N = leaf_litterN * leaf_litt_uscel_frac;

	/* leaf litter litter shielded cellulose nitrogen pool */
	c->leaf_litr3N = leaf_litterN * leaf_litt_scel_frac;

	/* leaf litter lignin nitrogen pool */
	c->leaf_litr4N = leaf_litterN * leaf_litter_lignin_frac;

	/*** compute fine root litter nitrogen pool ***/
	/* compute fine root litter total labile nitrogen pool */
	c->froot_litr1N = froot_litterN * froot_litter_labile_frac;

	/* fine root litter litter unshielded cellulose nitrogen pool */
	c->froot_litr2N = froot_litterN * froot_litt_uscel_frac;

	/* fine root litter litter shielded cellulose nitrogen pool */
	c->froot_litr3N = froot_litterN * froot_litt_scel_frac;

	/* fine root litter lignin nitrogen pool */
	c->froot_litr4N = froot_litterN * froot_litter_lignin_frac;

	/*** compute coarse woody debris litter nitrogen pool ***/
	/* coarse_woody debris litter litter unshielded cellulose nitrogen pool */
	c->deadwood_litr2N = cwd_litterN * deadwood_uscel_frac;

	/* fine root litter litter shielded cellulose nitrogen pool */
	c->deadwood_litr3N = cwd_litterN * deadwood_scel_frac;

	/* coarse woody debris carbon lignin nitrogen pool */
	c->deadwood_litr4N = cwd_litterN - deadwood_litter_cellulose;

	/****************** SUM ALL OVER NITROGEN POOLS ******************/
	/* compute litter total (litter + cwd) nitrogen pool */
	c->litrN  = litterN;

	/* compute litter total labile nitrogen pool */
	c->litr1N = c->leaf_litr1N + c->froot_litr1N;

	/* compute litter total unshielded cellulose nitrogen pool */
	c->litr2N = c->leaf_litr2N + c->froot_litr2N + c->deadwood_litr2N;

	/* compute litter total shielded cellulose nitrogen pool */
	c->litr3N = c->leaf_litr3N + c->froot_litr3N + c->deadwood_litr3N;

	/* compute litter total lignin nitrogen pool */
	c->litr4N = c->leaf_litr4N + c->froot_litr4N + c->deadwood_litr4N;

	/* check */
	CHECK_CONDITION ( fabs((c->litr1N + c->litr2N + c->litr3N + c->litr4N) - (c->litrN)), > , eps);
}


