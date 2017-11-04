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

#define TEST_RESP 0

void initialization_forest_structure(cell_t *const c, const int day, const int month, const int year)
{
	/* note: this function from here are called just for initialization */

	logger(g_debug_log,"\n*******INITIALIZE FOREST STRUCTURE*******\n");

	/* annual forest structure */
	if ( ! annual_forest_structure ( c, year ) )
	{
		puts(sz_err_out_of_memory);
		exit(1);
	}
}

void initialization_forest_class_C (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	double Lai_sun_ratio;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	d = &c->heights[height].dbhs[dbh];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* check that all mandatory variables are initialized */
	CHECK_CONDITION(h->value, <=, 0);
	CHECK_CONDITION(d->value, <=, 0);
	CHECK_CONDITION(a->value, <=, 0);

	/* check parameters values */
	CHECK_CONDITION (s->value[SLA_AVG0],               <, s->value[SLA_AVG1]);
	CHECK_CONDITION (s->value[FRACBB0],                <, s->value[FRACBB1]);
	CHECK_CONDITION (s->value[GROWTHTMIN],             >, s->value[GROWTHTOPT]);
	CHECK_CONDITION (s->value[GROWTHTMIN],             >, s->value[GROWTHTMAX]);
	CHECK_CONDITION (s->value[GROWTHTOPT],             >, s->value[GROWTHTMAX]);
	CHECK_CONDITION (s->value[SWPOPEN],                <, s->value[SWPCLOSE]);
	CHECK_CONDITION (s->value[FRUIT_PERC],             >, 1.);
	CHECK_CONDITION (s->value[LEAF_FALL_FRAC_GROWING], >, 1.);
	CHECK_CONDITION (s->value[LEAF_FROOT_TURNOVER],    >, 1.);
	CHECK_CONDITION (s->value[LIVEWOOD_TURNOVER],      >, 1.);
	CHECK_CONDITION (s->value[S0CTEM] + s->value[R0CTEM] + s->value[F0CTEM], !=, 1);

	/* compute growth respiration fraction */
	growth_respiration_frac ( a, s );

	/* compute age-related specific leaf area */
	specific_leaf_area ( a, s);


	/**************************************************************************************************/

	/* note: special case, model recomputes tree height based on its parameterizations to avoid differences between
	 * modelled and initialized tree height data */

	/* Chapman-Richards function */
	/* for references see also: R. Pilli et al. Forest Ecology and Management 237 (2006) 583–593 */
	/* note: this shouldn't be applied to saplings that are lower than 1.3 meter */
	h->value = DBH_ref + s->value[CRA] * pow (1. - exp ( - s->value[CRB] * d->value) , s->value[CRC]);

	if ( h->value > s->value[CRA] )
	{
		h->value = s->value[CRA];
	}
	logger(g_debug_log, "-Tree Height using Chapman-Richard function = %f m\n", h->value);

	/* check */
	CHECK_CONDITION (h->value, >, s->value[CRA] + eps )
	logger(g_debug_log, "-New Tree Height = %f m\n", h->value);

	/**************************************************************************************************/


	logger(g_debug_log,"\n*******INITIALIZE FOREST CLASS CARBON POOLS (%s)*******\n", s->name);
	logger(g_debug_log, "...checking initial biomass data for class: height %f, dbh %f, age %d, species %s...\n", h->value, d->value, a->value, s->name);

	/* compute mass density */
	s->value[MASS_DENSITY] = s->value[RHO1] + (s->value[RHO0] - s->value[RHO1]) * exp(-LN2 * ((double)a->value / s->value[TRHO]));
	logger(g_debug_log, "-Mass Density = %f tDM/m3\n", s->value[MASS_DENSITY]);

	/* compute volume */
	s->value[TREE_VOLUME] = (Pi * s->value[FORM_FACTOR] * pow((d->value / 100.) , 2.) * h->value) / 4.;
	s->value[VOLUME]      = s->value[TREE_VOLUME] * s->counter[N_TREE];

	logger(g_debug_log, "-Single tree volume = %f m3/tree\n", s->value[TREE_VOLUME]);
	logger(g_debug_log, "-Class volume       = %f m3/cell\n", s->value[VOLUME]);

	/*check for initial biomass*/
	if (s->value[STEM_DM] == 0.0 || s->value[STEM_DM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Stem Biomass Data are available for model initialization\n");
		logger(g_debug_log, "...Generating input Stem Biomass biomass data from DBH = %f cm\n", d->value);

		/* compute stem biomass from DBH */
		if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
		{
			/* use generic stemconst stempower values */
			logger(g_debug_log, "..computing stem biomass from generic stempower and stemconst DBH = %f cm\n", d->value);
			if (d->value < 9)
			{
				s->value[TREE_STEM_C] = (s->value[STEMCONST] * (pow (d->value, STEMPOWER_A))) / GC_GDM / 1e3;
			}
			else if (d->value > 9 && d->value < 15)
			{
				s->value[TREE_STEM_C] = (s->value[STEMCONST] * (pow (d->value, STEMPOWER_B))) / GC_GDM / 1e3;
			}
			else
			{
				s->value[TREE_STEM_C] = (s->value[STEMCONST] * (pow (d->value, STEMPOWER_C))) / GC_GDM / 1e3;
			}
		}

		else
		{
			/* use site specific stemconst stempower values */
			logger(g_debug_log, "..computing stem biomass from site specific stempower and stemconst DBH = %f cm\n", d->value);
			s->value[TREE_STEM_C] = (s->value[STEMCONST_P] * pow (d->value, s->value[STEMPOWER_P])) / GC_GDM / 1e3;

		}
		logger(g_debug_log, "Single tree stem mass = %f tC/tree\n", s->value[TREE_STEM_C]);

		/* tree tC -> class tDM */
		s->value[STEM_DM] = s->value[TREE_STEM_C] * s->counter[N_TREE] * GC_GDM;

		/* tree tC -> class tC */
		s->value[STEM_C]  = s->value[TREE_STEM_C] * s->counter[N_TREE];
	}
	else
	{
		logger(g_debug_log, "Ok stem biomass..\n");

		s->value[STEM_C]  = s->value[STEM_DM] / GC_GDM;
	}
	logger(g_debug_log, "---Stem Biomass = %f tC/tree\n",  s->value[TREE_STEM_C]);
	logger(g_debug_log, "---Stem Biomass = %f tDM/cell\n", s->value[STEM_DM]);
	logger(g_debug_log, "---Stem Biomass = %f tC/cell\n",  s->value[STEM_C]);

	if (s->value[BRANCH_DM] == 0.0 || s->value[BRANCH_DM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Branch and Bark Data are available from model initialization\n"
				"Is the Stem biomass initial value with Branch and Bark?\n");
		if (s->value[FRACBB0] == 0)
		{
			logger(g_debug_log, "I don't have FRACBB0 = FRACBB1 \n");

			s->value[FRACBB0] = s->value[FRACBB1];
			logger(g_debug_log, "FRACBB0 = %f\n", s->value[FRACBB0]);
		}
		else
		{
			s->value[FRACBB]    = s->value[FRACBB1] + (s->value[FRACBB0] - s->value[FRACBB1])* exp(-LN2 * (h->value / s->value[TBB]));
			s->value[BRANCH_DM] = s->value[STEM_DM] * s->value[FRACBB];
			s->value[BRANCH_C]  = s->value[STEM_C] * s->value[FRACBB];
		}
	}
	else
	{
		logger(g_debug_log, "Ok stem branch biomass..\n");
		s->value[BRANCH_C] = s->value[BRANCH_DM] / GC_GDM;
	}

	s->value[TREE_BRANCH_C]      = s->value[BRANCH_C] / (double)s->counter[N_TREE];

	logger(g_debug_log, "---Branch Biomass = %f tC/tree\n",  s->value[TREE_BRANCH_C]);
	logger(g_debug_log, "---Branch Biomass = %f tDM/cell\n", s->value[BRANCH_DM]);
	logger(g_debug_log, "---Branch Biomass = %f tC/cell\n",  s->value[BRANCH_C]);


	if( s->value[CROOT_DM] == 0 || s->value[CROOT_DM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Coarse root Biomass Data are available for model initialization \n");
		logger(g_debug_log, "...Generating input Coarse root Biomass biomass data from DBH data...\n");

		/* compute coarse root biomass using root to shoot ratio */
		s->value[CROOT_DM] = s->value[STEM_DM] * s->value[COARSE_ROOT_STEM];
		s->value[CROOT_C]  = s->value[STEM_C] * s->value[COARSE_ROOT_STEM];
	}
	else
	{
		logger(g_debug_log, "Ok coarse root biomass..\n");
		s->value[CROOT_C] = s->value[CROOT_DM] / GC_GDM;
	}

	s->value[TREE_CROOT_C]    = s->value[CROOT_C] / (double)s->counter[N_TREE];
	logger(g_debug_log, "-Coarse root biomass = %f tC/tree\n",   s->value[TREE_CROOT_C]);
	logger(g_debug_log, "-Coarse root biomass = %f tDM/cell \n", s->value[CROOT_DM]);
	logger(g_debug_log, "-Coarse root biomass = %f tC/cell \n",  s->value[CROOT_C]);

	/* total wood */
	s->value[TOT_WOOD_C]      = s->value[STEM_C] + s->value[CROOT_C] + s->value[BRANCH_C];
	s->value[TREE_TOT_WOOD_C] = s->value[TREE_STEM_C] + s->value[TREE_CROOT_C] + s->value[TREE_BRANCH_C];

	logger(g_debug_log, "-Total wood = %f tC/cell\n",s->value[TOT_WOOD_C]);
	logger(g_debug_log, "-Total wood = %f tC/tree\n",s->value[TREE_TOT_WOOD_C]);

	/* basal area */
	s->value[BASAL_AREA]          = ((pow((d->value / 2.), 2.)) * Pi);
	s->value[BASAL_AREA_m2]       = s->value[BASAL_AREA] * 0.0001;
	s->value[STAND_BASAL_AREA]    = s->value[BASAL_AREA] * s->counter[N_TREE];
	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];

	logger(g_debug_log, "-Basal area                       = %f cm2/tree\n",  s->value[BASAL_AREA]);
	logger(g_debug_log, "-Basal basal_area_m2              = %f m2/tree\n",   s->value[BASAL_AREA_m2]);
	logger(g_debug_log, "-Stand level class basal cell     = %f cm2/class\n", s->value[STAND_BASAL_AREA]);
	logger(g_debug_log, "-Stand level class basal cell     = %f cm2/class\n", s->value[STAND_BASAL_AREA]);

	/* sapwood and heartwood area are based on  Vertessy  et  al.  (1995)  and  Meinzer  et  al.(2001, 2005)*/
	s->value[SAPWOOD_AREA]           = s->value[SAP_A] * pow (d->value, s->value[SAP_B]);
	s->value[HEARTWOOD_AREA]         = s->value[BASAL_AREA] -  s->value[SAPWOOD_AREA];
	s->value[SAPWOOD_PERC]           = s->value[SAPWOOD_AREA] / s->value[BASAL_AREA];

	logger(g_debug_log, "-Sapwood_area                     = %f cm^2\n",       s->value[SAPWOOD_AREA]);
	logger(g_debug_log, "-Heart_wood_area                  = %f cm^2\n",       s->value[HEARTWOOD_AREA]);
	logger(g_debug_log, "-Sapwood perc                     = %f %%\n",         s->value[SAPWOOD_PERC]*100);

	/* sapwood and heartwood biomass */
	/* stem */
	s->value[STEM_SAPWOOD_DM]         = s->value[STEM_DM] * s->value[SAPWOOD_PERC];
	s->value[STEM_SAPWOOD_C]          = s->value[STEM_C]  * s->value[SAPWOOD_PERC];
	s->value[STEM_HEARTWOOD_DM]       = s->value[STEM_DM] - s->value[STEM_SAPWOOD_DM];
	s->value[STEM_HEARTWOOD_C]        = s->value[STEM_C]  - s->value[STEM_SAPWOOD_C];
	s->value[TREE_STEM_SAPWOOD_C]     = s->value[STEM_SAPWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_STEM_HEARTWOOD_C]   = (s->value[STEM_C] - s->value[TREE_STEM_SAPWOOD_C]) / (double)s->counter[N_TREE];

	/* coarse root */
	s->value[CROOT_SAPWOOD_DM]        = s->value[CROOT_DM] * s->value[SAPWOOD_PERC];
	s->value[CROOT_SAPWOOD_C]         = s->value[CROOT_C]  * s->value[SAPWOOD_PERC];
	s->value[CROOT_HEARTWOOD_DM]      = s->value[CROOT_DM] - s->value[CROOT_SAPWOOD_DM];
	s->value[CROOT_HEARTWOOD_C]       = s->value[CROOT_C]  - s->value[CROOT_SAPWOOD_C];
	s->value[TREE_CROOT_SAPWOOD_C]    = s->value[CROOT_SAPWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_CROOT_HEARTWOOD_C]  = (s->value[CROOT_C] - s->value[TREE_CROOT_SAPWOOD_C] ) / (double)s->counter[N_TREE];

	/* branch */
	s->value[BRANCH_SAPWOOD_DM]       = s->value[BRANCH_DM] * s->value[SAPWOOD_PERC];
	s->value[BRANCH_SAPWOOD_C]        = s->value[BRANCH_C]  * s->value[SAPWOOD_PERC];
	s->value[BRANCH_HEARTWOOD_DM]     = s->value[BRANCH_DM] - s->value[BRANCH_SAPWOOD_DM];
	s->value[BRANCH_HEARTWOOD_C]      = s->value[BRANCH_C]  - s->value[BRANCH_SAPWOOD_C];
	s->value[TREE_BRANCH_SAPWOOD_C]   = s->value[BRANCH_SAPWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_BRANCH_HEARTWOOD_C] = (s->value[BRANCH_C] - s->value[TREE_BRANCH_SAPWOOD_C]) / (double)s->counter[N_TREE];

	/* overall */
	s->value[SAPWOOD_DM]              = s->value[STEM_SAPWOOD_DM]   + s->value[CROOT_SAPWOOD_DM]   + s->value[BRANCH_SAPWOOD_DM];
	s->value[SAPWOOD_C]               = s->value[STEM_SAPWOOD_C]    + s->value[CROOT_SAPWOOD_C]    + s->value[BRANCH_SAPWOOD_C];
	s->value[HEARTWOOD_DM]            = s->value[STEM_HEARTWOOD_DM] + s->value[CROOT_HEARTWOOD_DM] + s->value[BRANCH_HEARTWOOD_DM];
	s->value[HEARTWOOD_C]             = s->value[STEM_HEARTWOOD_C]  + s->value[CROOT_HEARTWOOD_C]  + s->value[BRANCH_HEARTWOOD_C];
	s->value[TREE_SAPWOOD_C]          = s->value[TREE_SAPWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_HEARTWOOD_C]        = (s->value[TREE_TOT_WOOD_C] - s->value[TREE_SAPWOOD_C]) / (double)s->counter[N_TREE];


	logger(g_debug_log, "-Sapwood stem biomass             = %f tC/cell\n",  s->value[STEM_SAPWOOD_C]);
	logger(g_debug_log, "-Heartwood stem biomass           = %f tC/cell\n",  s->value[STEM_HEARTWOOD_C]);
	logger(g_debug_log, "-Sapwood coarse root biomass      = %f tC/cell\n",  s->value[CROOT_SAPWOOD_C]);
	logger(g_debug_log, "-Heartwood coarse root biomass    = %f tC/cell\n",  s->value[CROOT_HEARTWOOD_C]);
	logger(g_debug_log, "-Sapwood branch and bark biomass  = %f tC/cell\n",  s->value[BRANCH_SAPWOOD_C]);
	logger(g_debug_log, "-Heartwood branch biomass         = %f tC/cell\n",  s->value[BRANCH_HEARTWOOD_C]);
	logger(g_debug_log, "-Total Sapwood biomass            = %f tC/cell\n",  s->value[SAPWOOD_C]);
	logger(g_debug_log, "-Total Sapwood biomass per tree   = %f tC/tree\n",  s->value[SAPWOOD_C]    / (double)s->counter[N_TREE]);
	logger(g_debug_log, "-Total Sapwood biomass per tree   = %f KgC/tree\n", (s->value[SAPWOOD_C]   / (double)s->counter[N_TREE]) * 1e3);
	logger(g_debug_log, "-Total Heartwood biomass          = %f tC/cell\n",  s->value[HEARTWOOD_C]);
	logger(g_debug_log, "-Total Heartwood biomass per tree = %f tC/tree\n",  s->value[HEARTWOOD_C]  / (double)s->counter[N_TREE]);
	logger(g_debug_log, "-Total Heartwood biomass per tree = %f KgC/tree\n", (s->value[HEARTWOOD_C] / (double)s->counter[N_TREE]) * 1e3);

	/*reserve*/
	if (s->value[RESERVE_DM] == 0 || s->value[RESERVE_DM] == NO_DATA)
	{
		logger(g_debug_log, "\nNo Reserve Biomass Data are available for model initialization \n");
		logger(g_debug_log, "...Generating input Reserve Biomass biomass data\n");

		/* note: these values are taken from: Schwalm and Ek, 2004 Ecological Modelling */
		//see if change with the ratio reported from Barbaroux et al., 2002 (using DryMatter)

		/* IMPORTANT! reserve computation if not in initialized is computed from DryMatter */
		s->value[RESERVE_DM] = s->value[SAPWOOD_DM] * s->value[SAP_WRES];

		//fixme how it does??
		s->value[RESERVE_C]  = s->value[SAPWOOD_DM] * s->value[SAP_WRES];
	}

	s->value[TREE_RESERVE_C]    = s->value[RESERVE_C] / (double)s->counter[N_TREE];

	logger(g_debug_log, "-Reserve = %f tC/tree\n",   s->value[TREE_RESERVE_C]);
	logger(g_debug_log, "-Reserve = %f tC/cell \n",  s->value[RESERVE_C]);
	logger(g_debug_log, "-Reserve = %f KgC/cell \n", s->value[RESERVE_C] * 1e3);
	logger(g_debug_log, "-Reserve = %f gC/tree \n",  s->value[RESERVE_C] * 1e6 / (double)s->counter[N_TREE]);

	/* compute minimum reserve pool */
	s->value[MIN_RESERVE_C]      = s->value[RESERVE_C];
	s->value[TREE_MIN_RESERVE_C] = s->value[MIN_RESERVE_C] / (double)s->counter[N_TREE];

	logger(g_debug_log, "-Minimum reserve = %f tC/cell\n", s->value[MIN_RESERVE_C]);
	logger(g_debug_log, "-Minimum reserve = %f tC/tree\n", s->value[TREE_MIN_RESERVE_C]);


	/* leaf */
	if ( ! s->value[LEAF_DM] || s->value[LEAF_DM] == NO_DATA )
	{
		/* deciduous */
		if ( ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 ) && c->north == 0)
		{
			/* assuming no leaf at 1st of January */
			s->value[LEAF_DM]      = 0.;
			s->value[LEAF_C]       = 0.;
			s->value[LEAF_SUN_C]   = 0.;
			s->value[LEAF_SHADE_C] = 0.;
		}
		/* evergreen */
		else
		{
			logger(g_debug_log, "\nNo Leaf Biomass Data are available for model initialization \n");
			logger(g_debug_log, "...Generating input Leaf Biomass data from LAI\n");

			/* check */
			if( ! s->value[LAI_PROJ] || s->value[LAI_PROJ] == NO_DATA )
			{
				logger_error(g_debug_log,"No Leaf Biomass nor LAI values from initialization file (recompute it using sapwood)!!!!\n");

				/* compute LAI (assuming at peak value) */
				s->value[LAI_PROJ] = ( ( s->value[SAPWOOD_AREA] / 1e5 ) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_PROJ];
				logger(g_debug_log, "PEAK_LAI_PROJ = %f m2/m2\n",s->value[PEAK_LAI_PROJ]);
			}

			/* compute leaf carbon to PEAK LAI down-scaled to canopy cover */
			s->value[LEAF_DM]   = (s->value[LAI_PROJ] / s->value[SLA_AVG] ) / 1e3 * ( s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell );
			logger(g_debug_log, "LEAF_DM        = %f tDM/cell\n", s->value[LEAF_DM]);

			/* convert tDM/cell to tC/cell */
			s->value[LEAF_C]      = s->value[LEAF_DM] / GC_GDM;
			logger(g_debug_log, "LEAF_C         = %f tC/cell\n", s->value[LEAF_C]);

			/* Calculate projected LAI for sunlit and shaded canopy portions */
			s->value[LAI_SUN_PROJ]   = 1. - exp(-s->value[LAI_PROJ]);
			s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];

			logger(g_debug_log, "LAI_PROJ       = %f m2/m2\n", s->value[LAI_PROJ]);
			logger(g_debug_log, "LAI_SUN_PROJ   = %f m2/m2\n", s->value[LAI_SUN_PROJ]);
			logger(g_debug_log, "LAI_SHADE_PROJ = %f m2/m2\n", s->value[LAI_SHADE_PROJ]);

			/* compute total LAI for Exposed Area */
			//fixme error
			s->value[LAI_EXP]       = (s->value[LAI_PROJ] / s->value[SLA_AVG]) / 1e3 * ( s->value[CANOPY_COVER_EXP] * g_settings->sizeCell );
			s->value[LAI_SUN_EXP]   = 1. - exp ( -s->value[LAI_EXP] );
			s->value[LAI_SHADE_EXP] = s->value[LAI_EXP] - s->value[LAI_SUN_EXP];

			logger(g_debug_log, "LAI_EXP        = %f m2/m2\n", s->value[LAI_EXP]);
			logger(g_debug_log, "LAI_SUN_EXP    = %f m2/m2\n", s->value[LAI_SUN_EXP]);
			logger(g_debug_log, "LAI_SHADE_EXP  = %f m2/m2\n", s->value[LAI_SHADE_EXP]);
		}
	}
	else
	{
		s->value[LEAF_C] = s->value[LEAF_DM] / GC_GDM;

		logger(g_debug_log, "Ok Leaf biomass..\n");

		/* if no values for LAI are available */
		if ( !s->value[LAI_PROJ] )
		{
			logger(g_debug_log, "\nNo LAI Data are available for model initialization \n");
			logger(g_debug_log, "...Generating input LAI data from Leaf Biomass\n");

			logger(g_debug_log, "CANOPY_COVER_PROJ = %f\n", s->value[CANOPY_COVER_PROJ]);

			/* Calculate projected LAI for tot and for sunlit and shaded canopy portions*/
			s->value[LAI_PROJ]       = ((s->value[LEAF_C] * 1e3) * s->value[SLA_AVG]) / (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell);
			s->value[LAI_SUN_PROJ]   = 1. - exp(-s->value[LAI_PROJ]);
			s->value[LAI_SHADE_PROJ] = s->value[LAI_PROJ] - s->value[LAI_SUN_PROJ];

			logger(g_debug_log, "LAI_PROJ  = %f\n", s->value[LAI_PROJ]);
			logger(g_debug_log, "LAI SUN   = %f\n", s->value[LAI_SUN_PROJ]);
			logger(g_debug_log, "LAI SHADE = %f\n", s->value[LAI_SHADE_PROJ]);
		}
		else
		{
			logger(g_debug_log, "Ok LAI..\n");
		}

		/* assuming the same proportion for LAI also for leaf carbon */
		Lai_sun_ratio   = s->value[LAI_SUN_PROJ] / s->value[LAI_PROJ];

		/* compute based on proportion Leaf carbon for sun and shaded leaves */
		s->value[LEAF_SUN_C]   = s->value[LEAF_C] * Lai_sun_ratio;
		s->value[LEAF_SHADE_C] = s->value[LEAF_C] - s->value[LEAF_SUN_C];

	}

	/* compute single tree leaf carbon amount */
	s->value[TREE_LEAF_C] = s->value[LEAF_C] / s->counter[N_TREE];

	logger(g_debug_log, "-Leaf Biomass       = %f tC/tree\n",  s->value[TREE_LEAF_C]);
	logger(g_debug_log, "-Leaf Biomass       = %f tDM/cell\n", s->value[LEAF_DM]);
	logger(g_debug_log, "-Leaf Biomass       = %f tC/cell\n",  s->value[LEAF_C]);
	logger(g_debug_log, "-Leaf sun Biomass   = %f tC/cell\n",  s->value[LEAF_SUN_C]);
	logger(g_debug_log, "-Leaf shade Biomass = %f tC/cell\n",  s->value[LEAF_SHADE_C]);

	/* compute all-sided Leaf Area */
	s->value[ALL_LAI_PROJ] = s->value[LAI_PROJ] * s->value[CANOPY_COVER_PROJ];
	logger(g_debug_log, "ALL_LAI_PROJ = %f (m2)\n", s->value[ALL_LAI_PROJ]);
	logger(g_debug_log,"*****************************\n");

	/* note: model assumes that if no fine-root biomass are available the same ratio foliage-fine roots is used */
	if (( s->value[FROOT_DM] == 0.0 || s->value[FROOT_DM] == NO_DATA)
			&& (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		logger(g_debug_log, "\nNo Fine root Biomass Data are available for model initialization \n");

		s->value[FROOT_DM] = s->value[LEAF_DM] * s->value[FINE_ROOT_LEAF];
		s->value[FROOT_C]  = s->value[LEAF_C] * s->value[FINE_ROOT_LEAF];
	}
	else
	{
		/* assuming no fine root at 1st of January */
		s->value[FROOT_DM]     = 0.;
		s->value[FROOT_C]      = 0.;

		logger(g_debug_log, "Ok fine root biomass..\n");
	}

	s->value[TREE_FROOT_C]    = s->value[FROOT_C] / (double)s->counter[N_TREE];

	logger(g_debug_log, "-Fine root Biomass = %f tC/tree\n",  s->value[TREE_FROOT_C]);
	logger(g_debug_log, "-Fine Root Biomass = %f tC/cell\n",  s->value[FROOT_C]);
	logger(g_debug_log, "-Fine Root Biomass = %f tDM/cell\n", s->value[FROOT_DM]);

	/***** COMPUTE LIVE DEAD BIOMASS *****/

	live_total_wood_age( a, s );

	/* stem */
#if TEST_RESP
	//old
	s->value[STEM_LIVEWOOD_DM]     = s->value[STEM_DM] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[STEM_LIVEWOOD_C]      = s->value[STEM_C]  * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[STEM_DEADWOOD_DM]     = s->value[STEM_DM] - s->value[STEM_LIVEWOOD_DM];
	s->value[STEM_DEADWOOD_C]      = s->value[STEM_C]  - s->value[STEM_LIVEWOOD_C];

	s->value[TREE_STEM_LIVE_KgDM]  = s->value[STEM_LIVEWOOD_DM] * 1e3 / (double)s->counter[N_TREE];
	s->value[TREE_STEM_LIVE_C]     = s->value[STEM_LIVEWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_STEM_DEAD_KgDM]  = s->value[STEM_DEADWOOD_DM] * 1e3 / s->counter[N_TREE];
	s->value[TREE_STEM_DEAD_C]     = s->value[STEM_DEADWOOD_C] / (double)s->counter[N_TREE];

	/* check */
	CHECK_CONDITION(fabs((s->value[STEM_C])-(s->value[STEM_LIVEWOOD_C] + s->value[STEM_DEADWOOD_C])), >,eps);

#else
	//new 13 May 2017
	/* class */
	s->value[STEM_LIVEWOOD_DM]     = s->value[STEM_SAPWOOD_DM] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[STEM_LIVEWOOD_C]      = s->value[STEM_SAPWOOD_C]  * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[STEM_DEADWOOD_DM]     = s->value[STEM_SAPWOOD_DM] - s->value[STEM_LIVEWOOD_DM];
	s->value[STEM_DEADWOOD_C]      = s->value[STEM_SAPWOOD_C]  - s->value[STEM_LIVEWOOD_C];

	/* tree */
	s->value[TREE_STEM_LIVEWOOD_C] = s->value[STEM_LIVEWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_STEM_DEADWOOD_C] = s->value[STEM_DEADWOOD_C] / (double)s->counter[N_TREE];

	/* check */
	CHECK_CONDITION(fabs((s->value[STEM_SAPWOOD_DM])-(s->value[STEM_LIVEWOOD_DM] + s->value[STEM_DEADWOOD_DM])), >,eps);
	CHECK_CONDITION(fabs((s->value[STEM_SAPWOOD_C]) -(s->value[STEM_LIVEWOOD_C]  + s->value[STEM_DEADWOOD_C])),  >,eps);

#endif

	logger(g_debug_log, "-Stem Biomass       = %f tC/cell\n", s->value[STEM_C]);
	logger(g_debug_log, "-Live Stem Biomass  = %f tC/cell\n", s->value[STEM_LIVEWOOD_C]);
	logger(g_debug_log, "-Dead Stem Biomass  = %f tC/cell\n", s->value[STEM_DEADWOOD_C]);
	logger(g_debug_log, "-Live Stem Biomass  = %f tC/tree\n", s->value[TREE_STEM_LIVEWOOD_C]);
	logger(g_debug_log, "-Dead Stem Biomass  = %f tC/tree\n", s->value[TREE_STEM_DEADWOOD_C]);


#if TEST_RESP
	//old
	/* coarse root */
	s->value[CROOT_LIVEWOOD_DM]     = s->value[CROOT_DM] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[CROOT_LIVEWOOD_C]      = s->value[CROOT_C] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[CROOT_DEADWOOD_DM]     = s->value[CROOT_DM] -s->value[CROOT_LIVEWOOD_DM];
	s->value[CROOT_DEADWOOD_C]      = s->value[CROOT_C] -s->value[CROOT_LIVEWOOD_C];

	s->value[TREE_CROOT_LIVE_KgDM]  = s->value[CROOT_LIVEWOOD_DM] * 1e3 / (double)s->counter[N_TREE];
	s->value[TREE_CROOT_LIVE_C]     = s->value[CROOT_LIVEWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_CROOT_DEAD_KgDM]  = s->value[CROOT_DEADWOOD_DM] * 1e3 / (double)s->counter[N_TREE];
	s->value[TREE_CROOT_DEAD_C]     = s->value[CROOT_DEADWOOD_C] / (double)s->counter[N_TREE];

	/* check */
	CHECK_CONDITION(fabs((s->value[CROOT_C])-(s->value[CROOT_LIVEWOOD_C] + s->value[CROOT_DEADWOOD_C])), >,eps);

#else
	//new 13 May 2017
	/* class */
	s->value[CROOT_LIVEWOOD_DM]     = s->value[CROOT_SAPWOOD_DM] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[CROOT_LIVEWOOD_C]      = s->value[CROOT_SAPWOOD_C]  * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[CROOT_DEADWOOD_DM]     = s->value[CROOT_SAPWOOD_DM] - s->value[CROOT_LIVEWOOD_DM];
	s->value[CROOT_DEADWOOD_C]      = s->value[CROOT_SAPWOOD_C]  - s->value[CROOT_LIVEWOOD_C];

	/* tree */
	s->value[TREE_CROOT_LIVEWOOD_C] = s->value[CROOT_LIVEWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_CROOT_DEADWOOD_C] = s->value[CROOT_DEADWOOD_C] / (double)s->counter[N_TREE];

	/* check */
	CHECK_CONDITION(fabs((s->value[CROOT_SAPWOOD_DM])-(s->value[CROOT_LIVEWOOD_DM] + s->value[CROOT_DEADWOOD_DM])), >,eps);
	CHECK_CONDITION(fabs((s->value[CROOT_SAPWOOD_C]) -(s->value[CROOT_LIVEWOOD_C]  + s->value[CROOT_DEADWOOD_C])),  >,eps);

#endif

	logger(g_debug_log, "-Coarse root Biomass      = %f tC/cell\n", s->value[CROOT_C]);
	logger(g_debug_log, "-Live Coarse Root Biomass = %f tC/cell\n", s->value[CROOT_LIVEWOOD_C]);
	logger(g_debug_log, "-Dead Coarse Root Biomass = %f tC/cell\n", s->value[CROOT_DEADWOOD_C]);
	logger(g_debug_log, "-Live Coarse Root Biomass = %f tC/tree\n", s->value[TREE_CROOT_LIVEWOOD_C]);
	logger(g_debug_log, "-Dead Coarse Root Biomass = %f tC/tree\n", s->value[TREE_CROOT_DEADWOOD_C]);

#if TEST_RESP
	//old
	/* branch */
	s->value[BRANCH_LIVEWOOD_DM]      = s->value[BRANCH_DM] * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_LIVEWOOD_C]       = s->value[BRANCH_C]  * (s->value[EFF_LIVE_TOTAL_WOOD_FRAC]);
	s->value[BRANCH_DEADWOOD_DM]      = s->value[BRANCH_DM] - s->value[BRANCH_LIVEWOOD_DM];
	s->value[BRANCH_DEADWOOD_C]       = s->value[BRANCH_C]  - s->value[BRANCH_LIVEWOOD_C];

	s->value[TREE_BRANCH_LIVE_KgDM]   = s->value[BRANCH_LIVEWOOD_DM] * 1e3 / (double)s->counter[N_TREE];
	s->value[TREE_BRANCH_LIVE_C]      = s->value[BRANCH_LIVEWOOD_C] / s->counter[N_TREE];
	s->value[TREE_BRANCH_DEAD_KgDM]   = s->value[BRANCH_DEADWOOD_DM] * 1e3 / (double)s->counter[N_TREE];
	s->value[TREE_BRANCH_DEAD_C]      = s->value[BRANCH_DEADWOOD_C] / (double)s->counter[N_TREE];

	/* check */
	CHECK_CONDITION(fabs((s->value[BRANCH_C])-(s->value[BRANCH_LIVEWOOD_C] + s->value[BRANCH_DEADWOOD_C])), >,eps);

#else
	//new 13 May 2017
	/* class */
	s->value[BRANCH_LIVEWOOD_DM]      = s->value[BRANCH_SAPWOOD_DM] * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[BRANCH_LIVEWOOD_C]       = s->value[BRANCH_SAPWOOD_C]  * s->value[EFF_LIVE_TOTAL_WOOD_FRAC];
	s->value[BRANCH_DEADWOOD_DM]      = s->value[BRANCH_SAPWOOD_DM] - s->value[BRANCH_LIVEWOOD_DM];
	s->value[BRANCH_DEADWOOD_C]       = s->value[BRANCH_SAPWOOD_C]  - s->value[BRANCH_LIVEWOOD_C];

	/* tree */
	s->value[TREE_BRANCH_LIVEWOOD_C]  = s->value[BRANCH_LIVEWOOD_C] / (double)s->counter[N_TREE];
	s->value[TREE_BRANCH_DEADWOOD_C]  = s->value[BRANCH_DEADWOOD_C] / (double)s->counter[N_TREE];

	/* check */
	CHECK_CONDITION(fabs((s->value[BRANCH_SAPWOOD_DM])-(s->value[BRANCH_LIVEWOOD_DM] + s->value[BRANCH_DEADWOOD_DM])), >,eps);
	CHECK_CONDITION(fabs((s->value[BRANCH_SAPWOOD_C]) -(s->value[BRANCH_LIVEWOOD_C]  + s->value[BRANCH_DEADWOOD_C])),  >,eps);

#endif

	logger(g_debug_log, "-Branch Biuomass          = %f tC/cell\n", s->value[BRANCH_C]);
	logger(g_debug_log, "-Live Stem Branch Biomass = %f tC/cell\n", s->value[BRANCH_LIVEWOOD_C]);
	logger(g_debug_log, "-Dead Stem Branch Biomass = %f tC/cell\n", s->value[BRANCH_DEADWOOD_C]);
	logger(g_debug_log, "-Live Stem Branch Biomass = %f tC/tree\n", s->value[TREE_BRANCH_LIVEWOOD_C]);
	logger(g_debug_log, "-Dead Stem Branch Biomass = %f tC/tree\n", s->value[TREE_BRANCH_DEADWOOD_C]);

	/** live biomass **/
	/* class */
	s->value[LIVEWOOD_DM]       = s->value[STEM_LIVEWOOD_DM]  + s->value[CROOT_LIVEWOOD_DM] + s->value[BRANCH_LIVEWOOD_DM];
	s->value[LIVEWOOD_C]        = s->value[STEM_LIVEWOOD_C]   + s->value[CROOT_LIVEWOOD_C]  + s->value[BRANCH_LIVEWOOD_C];

	/* tree */
	s->value[TREE_LIVEWOOD_C]   = s->value[LIVEWOOD_C] / (double)s->counter[N_TREE];

	logger(g_debug_log, "-Live biomass = %f tC/cell\n", s->value[LIVEWOOD_C]);
	logger(g_debug_log, "-Live biomass = %f tC/tree\n", s->value[TREE_LIVEWOOD_C]);

	/** dead biomass **/
	/* class */
	s->value[DEADWOOD_DM]        = s->value[STEM_DEADWOOD_DM] + s->value[CROOT_DEADWOOD_DM] + s->value[BRANCH_DEADWOOD_DM];
	s->value[DEADWOOD_C]         = s->value[STEM_DEADWOOD_C]  + s->value[CROOT_DEADWOOD_C]  + s->value[BRANCH_DEADWOOD_C];

	/* tree */
	s->value[TREE_DEADWOOD_C]    = s->value[DEADWOOD_C] / (double)s->counter[N_TREE];

	logger(g_debug_log, "-Dead biomass = %f tC/cell\n", s->value[DEADWOOD_C]);
	logger(g_debug_log, "-Dead biomass = %f tC/tree\n", s->value[TREE_DEADWOOD_C]);

	logger(g_debug_log, "-Live wood vs total biomass = %f %%\n", (s->value[LIVEWOOD_C] / s->value[TOT_WOOD_C]) * 100.);
	logger(g_debug_log, "-Dead wood vs total biomass = %f %%\n", (s->value[DEADWOOD_C] / s->value[TOT_WOOD_C]) * 100.);
	logger(g_debug_log, "-Reserve                    = %f tC/cell\n", s->value[LIVEWOOD_C] * s->value[SAP_WRES] );

	/* fruit */
	s->value[FRUIT_C]      = 0.;
	s->value[TREE_FRUIT_C] = 0.;

	/* compute AGB and BGB */
	s->value[AGB]      = s->value[STEM_C] + s->value[BRANCH_C] + s->value[LEAF_C] + s->value[FRUIT_C];
	s->value[BGB]      = s->value[FROOT_C] + s->value[CROOT_C];
	s->value[TREE_AGB] = s->value[AGB] / (double)s->counter[N_TREE];
	s->value[TREE_BGB] = s->value[BGB] / (double)s->counter[N_TREE];

	logger(g_debug_log, "-Yearly Class AGB = %f tC/cell\n", s->value[AGB]);
	logger(g_debug_log, "-Yearly Class BGB = %f tC/cell\n", s->value[BGB]);
	logger(g_debug_log, "-Yearly Class AGB = %f tC/tree\n", s->value[TREE_AGB]);
	logger(g_debug_log, "-Yearly Class BGB = %f tC/tree\n", s->value[TREE_BGB]);

	/* ONLY for evergreen */
	if ( s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2 )
	{
		CHECK_CONDITION(s->value[LEAF_C],              <=, ZERO);
		CHECK_CONDITION(s->value[FROOT_C],             <=, ZERO);
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
	CHECK_CONDITION(s->value[SAPWOOD_C],               <=, ZERO);
	CHECK_CONDITION(s->value[STEM_LIVEWOOD_C],         <=, ZERO);
	CHECK_CONDITION(s->value[STEM_DEADWOOD_C],         <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_LIVEWOOD_C],        <=, ZERO);
	CHECK_CONDITION(s->value[CROOT_DEADWOOD_C],        <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_LIVEWOOD_C],       <=, ZERO);
	CHECK_CONDITION(s->value[BRANCH_DEADWOOD_C],       <=, ZERO);
	CHECK_CONDITION(s->value[LIVEWOOD_C],              <=, ZERO);
	CHECK_CONDITION(s->value[DEADWOOD_C],              <=, ZERO);
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
	c->stem_live_wood_carbon    += (s->value[STEM_LIVEWOOD_C]    * 1e6 / g_settings->sizeCell);
	c->stem_dead_wood_carbon    += (s->value[STEM_DEADWOOD_C]    * 1e6 / g_settings->sizeCell);
	c->croot_carbon             += (s->value[CROOT_C]            * 1e6 / g_settings->sizeCell);
	c->croot_live_wood_carbon   += (s->value[CROOT_LIVEWOOD_C]   * 1e6 / g_settings->sizeCell);
	c->croot_dead_wood_carbon   += (s->value[CROOT_DEADWOOD_C]   * 1e6 / g_settings->sizeCell);
	c->branch_carbon            += (s->value[BRANCH_C]           * 1e6 / g_settings->sizeCell);
	c->branch_live_wood_carbon  += (s->value[BRANCH_LIVEWOOD_C]  * 1e6 / g_settings->sizeCell);
	c->branch_dead_wood_carbon  += (s->value[BRANCH_DEADWOOD_C]  * 1e6 / g_settings->sizeCell);
	c->reserve_carbon           += (s->value[RESERVE_C]          * 1e6 / g_settings->sizeCell);
	c->fruit_carbon             += (s->value[FRUIT_C]            * 1e6 / g_settings->sizeCell);

	c->agb                      += ((s->value[LEAF_C] + s->value[STEM_C] + s->value[BRANCH_C] + s->value[FRUIT_C]) * 1e6 / g_settings->sizeCell);
	c->bgb                      += ((s->value[FROOT_C] + s->value[CROOT_C]) * 1e6 / g_settings->sizeCell);

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
	CHECK_CONDITION(c->agb,                            <=, ZERO);
	CHECK_CONDITION(c->bgb,                            <=, ZERO);

}

void initialization_forest_class_N (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log,"\n*******INITIALIZE FOREST CLASS NITROGEN POOLS (%s)*******\n", s->name);

	/* leaf */
	if ( ! s->value[LEAF_C] )
	{
		s->value[LEAF_N]       = 0.;
		s->value[LEAF_SUN_N]   = 0.;
		s->value[LEAF_SHADE_N] = 0.;
	}
	else
	{
		s->value[LEAF_N]       = s->value[LEAF_C]       / s->value[CN_LEAVES];
		s->value[LEAF_SUN_N]   = s->value[LEAF_SUN_C]   / s->value[CN_LEAVES];
		s->value[LEAF_SHADE_N] = s->value[LEAF_SHADE_C] / s->value[CN_LEAVES];
	}

	/* fine root */
	if ( ! s->value[FROOT_C] )
	{
		s->value[FROOT_N] = 0.;
	}
	else
	{
		s->value[FROOT_N] = s->value[FROOT_C] / s->value[CN_FINE_ROOTS];
	}

	/* stem */
	s->value[STEM_LIVEWOOD_N]   = s->value[STEM_LIVEWOOD_C] / s->value[CN_LIVE_WOODS];
	s->value[STEM_DEADWOOD_N]   = s->value[STEM_DEADWOOD_C] / s->value[CN_DEAD_WOODS];
	s->value[STEM_N]            = s->value[STEM_LIVEWOOD_N] + s->value[STEM_DEADWOOD_N];

	/* coarse root */
	s->value[CROOT_LIVEWOOD_N]  = s->value[CROOT_LIVEWOOD_C] / s->value[CN_LIVE_WOODS];
	s->value[CROOT_DEADWOOD_N]  = s->value[CROOT_DEADWOOD_C] / s->value[CN_DEAD_WOODS];
	s->value[CROOT_N]           = s->value[CROOT_LIVEWOOD_N] + s->value[CROOT_DEADWOOD_N];

	/* branch */
	s->value[BRANCH_LIVEWOOD_N] = s->value[BRANCH_LIVEWOOD_C] / s->value[CN_LIVE_WOODS];
	s->value[BRANCH_DEADWOOD_N] = s->value[BRANCH_DEADWOOD_C] / s->value[CN_DEAD_WOODS];
	s->value[BRANCH_N]          = s->value[BRANCH_LIVEWOOD_N] + s->value[BRANCH_DEADWOOD_N];

	logger(g_debug_log, "----LEAF_N             = %f tN/cell\n", s->value[LEAF_N]);
	logger(g_debug_log, "----LEAF_SUN_N         = %f tN/cell\n", s->value[LEAF_SUN_N]);
	logger(g_debug_log, "----LEAF_SHADE_N       = %f tN/cell\n", s->value[LEAF_SHADE_N]);
	logger(g_debug_log, "----FROOT_N            = %f tN/cell\n", s->value[FROOT_N]);
	logger(g_debug_log, "----STEM_LIVE_WOOD_N   = %f tN/cell\n", s->value[STEM_LIVEWOOD_N]);
	logger(g_debug_log, "----STEM_DEAD_WOOD_N   = %f tN/cell\n", s->value[STEM_DEADWOOD_N]);
	logger(g_debug_log, "----STEM_N             = %f tN/cell\n", s->value[STEM_N]);
	logger(g_debug_log, "----CROOT_LIVE_WOOD_N  = %f tN/cell\n", s->value[CROOT_LIVEWOOD_N]);
	logger(g_debug_log, "----CROOT_DEAD_WOOD_N  = %f tN/cell\n", s->value[CROOT_DEADWOOD_N]);
	logger(g_debug_log, "----CROOT_N            = %f tN/cell\n", s->value[CROOT_N]);
	logger(g_debug_log, "----BRANCH_LIVE_WOOD_N = %f tN/cell\n", s->value[BRANCH_LIVEWOOD_N]);
	logger(g_debug_log, "----BRANCH_DEAD_WOOD_N = %f tN/cell\n", s->value[BRANCH_DEADWOOD_N]);
	logger(g_debug_log, "----BRANCH_N           = %f tN/cell\n", s->value[BRANCH_N]);

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
	c->leaf_nitrogen              += (s->value[LEAF_N]            * 1e6 / g_settings->sizeCell);
	c->froot_nitrogen             += (s->value[FROOT_N]           * 1e6 / g_settings->sizeCell);
	c->stem_nitrogen              += (s->value[STEM_N]            * 1e6 / g_settings->sizeCell);
	c->stem_live_wood_nitrogen    += (s->value[STEM_LIVEWOOD_N]   * 1e6 / g_settings->sizeCell);
	c->stem_dead_wood_nitrogen    += (s->value[STEM_DEADWOOD_N]   * 1e6 / g_settings->sizeCell);
	c->croot_nitrogen             += (s->value[CROOT_N]           * 1e6 / g_settings->sizeCell);
	c->croot_live_wood_nitrogen   += (s->value[CROOT_LIVEWOOD_N]  * 1e6 / g_settings->sizeCell);
	c->croot_dead_wood_nitrogen   += (s->value[CROOT_DEADWOOD_N]  * 1e6 / g_settings->sizeCell);
	c->branch_nitrogen            += (s->value[BRANCH_N]          * 1e6 / g_settings->sizeCell);
	c->branch_live_wood_nitrogen  += (s->value[BRANCH_LIVEWOOD_N] * 1e6 / g_settings->sizeCell);
	c->branch_dead_wood_nitrogen  += (s->value[BRANCH_DEADWOOD_N] * 1e6 / g_settings->sizeCell);
	c->reserve_nitrogen           += (s->value[RESERVE_N]         * 1e6 / g_settings->sizeCell);
	c->fruit_nitrogen             += (s->value[FRUIT_N]           * 1e6 / g_settings->sizeCell);

	/* check */
	CHECK_CONDITION(c->leaf_nitrogen,             <=, ZERO);
	CHECK_CONDITION(c->froot_nitrogen,            <=, ZERO);
	CHECK_CONDITION(c->stem_nitrogen,             <=, ZERO);
	CHECK_CONDITION(c->stem_live_wood_nitrogen,   <=, ZERO);
	CHECK_CONDITION(c->stem_dead_wood_nitrogen,   <=, ZERO);
	CHECK_CONDITION(c->croot_nitrogen,            <=, ZERO);
	CHECK_CONDITION(c->croot_live_wood_nitrogen,  <=, ZERO);
	CHECK_CONDITION(c->croot_live_wood_nitrogen,  <=, ZERO);
	CHECK_CONDITION(c->branch_nitrogen,           <=, ZERO);
	CHECK_CONDITION(c->branch_live_wood_nitrogen, <=, ZERO);
	CHECK_CONDITION(c->branch_dead_wood_nitrogen, <=, ZERO);
	CHECK_CONDITION(c->reserve_nitrogen,          <=, ZERO);
	CHECK_CONDITION(c->fruit_nitrogen,            <=, ZERO);


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
		s->value[DEAD_WOOD_USCEL_FRAC] = (1. - temp_var) * s->value[DEAD_WOOD_CEL_FRAC];
	}
	else
	{
		s->value[DEAD_WOOD_SCEL_FRAC]  = 0.8 * s->value[DEAD_WOOD_CEL_FRAC];
		s->value[DEAD_WOOD_USCEL_FRAC] = 0.2 * s->value[DEAD_WOOD_CEL_FRAC];
	}
}
void initialization_forest_litter_soil (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	//fixme todo

}

void initialization_soil_physic(cell_t *const c)
{
	float acoeff;
	float bcoeff;
	float sat;
	float volumetric_wilting_point;
	float volumetric_field_capacity;
	float volumetric_saturated_hydraulic_conductivity;

	logger(g_debug_log,"\n*******INITIALIZE SOIL *******\n");

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
	logger(g_debug_log, "soil_b = %f (DIM)\n", c->soil_b);

	//test
	/* following Rawls et al., 1992 and Schwalm et al., 2004 */
	/*texture-dependent empirical coefficinet */
	// c->soil_b = 11.43 - (0.1034*g_soil_settings->values[SOIL_SAND_PERC) - (0.0687*0.157*g_soil_settings->values[SOIL_silt_perc);

	/* calculate the soil pressure-volume coefficients from texture data */
	/* Uses the multivariate regressions from Cosby et al., 1984 */
	/* (DIM) Soil volumetric water content at saturation */
	c->vwc_sat = (50.5 - 0.142*g_soil_settings->values[SOIL_SAND_PERC] - 0.037*g_soil_settings->values[SOIL_CLAY_PERC])/100.0; /* ok for schwalm*/
	logger(g_debug_log, "volumetric water content at saturation (BIOME) = %f %%(vol)\n", c->vwc_sat);
	/* (MPa) soil matric potential at saturation */
	c->psi_sat = -(exp((1.54 - 0.0095*g_soil_settings->values[SOIL_SAND_PERC] + 0.0063*g_soil_settings->values[SOIL_SILT_PERC])*log(10.0))*9.8e-5); /* ok for schwalm*/
	logger(g_debug_log, "psi_sat = %f MPa \n", c->psi_sat);
	/* Clapp-Hornenberger function 1978 (DIM) Soil Field Capacity Volumetric Water Content at field capacity ( = -0.015 MPa) */
	c->vwc_fc = c->vwc_sat * pow((-0.015/c->psi_sat),(1.0/c->soil_b));
	logger(g_debug_log, "volumetric water content at field capacity (BIOME) = %f %%(vol) \n", c->vwc_fc);

	/* define maximum soilwater content, for outflow calculation */
	/* converts volumetric water content (m3/m3) --> (kg/m2) */

	/* (kgH2O/m2) soil water at field capacity */
	c->soilw_fc = (g_soil_settings->values[SOIL_DEPTH] / 100) * c->vwc_fc * 1e3;
	logger(g_debug_log, "soilw_fc BIOME (MAXASW FC BIOME)= %f (kgH2O/m2)\n", c->soilw_fc);
	//equal to MAXASW

	/* (kgH2O/m2) soil water at saturation */
	c->soilw_sat = (g_soil_settings->values[SOIL_DEPTH] / 100) * c->vwc_sat * 1e3;
	logger(g_debug_log, "soilw_sat BIOME (MAXASW SAT BIOME)= %f (kgH2O/m2)\n", c->soilw_sat);

	/* (kgH2O/m2) maximum soil water at field capacity */
	c->max_asw_fc = c->soilw_fc;

	/* compute initialization soil water content */
	c->asw = (c->soilw_sat * g_settings->init_frac_maxasw);
	logger(g_debug_log, "Initialization ASW = %f (mm-kgH2O/m2)\n\n\n", c->asw);


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
	logger(g_debug_log, "volumetric water content at wilting point (CENTURY) = %f %%(vol)\n", volumetric_wilting_point);
	/* (kgH2O/m2) soil water at wilting point */
	c->wilting_point = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_wilting_point * 1e3;
	logger(g_debug_log, "Wilting point (CENTURY) = %f mm/m2\n", c->wilting_point);

	/* volumetric percentage field capacity */
	volumetric_field_capacity += (float)(0.07 * volumetric_field_capacity);
	logger(g_debug_log, "volumetric water content at field capacity (CENTURY) = %f %%(vol)\n", volumetric_field_capacity);
	/* (kgH2O/m2) soil water at field capacity */
	c->field_capacity = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_field_capacity * 1e3;
	logger(g_debug_log, "Field capacity (CENTURY) = %f mm/m2\n", c->field_capacity);

	/* volumetric percentage saturated hydraulic conductivity */
	volumetric_saturated_hydraulic_conductivity /= 1500.0;
	logger(g_debug_log, "volumetric water content at saturated hydraulic conductance (CENTURY) = %f %%(vol)\n", volumetric_saturated_hydraulic_conductivity);

	/* (kgH2O/m2) soil water at saturated hydraulic conductivity */
	c->sat_hydr_conduct = (g_soil_settings->values[SOIL_DEPTH] / 100) * volumetric_saturated_hydraulic_conductivity * 1e3;
	logger(g_debug_log, "Saturated hydraulic conductivity (CENTURY) = %f mm/m2\n", c->sat_hydr_conduct);

	/* bulk density g/cm3 */
	c->bulk_density += (-0.08 * c->bulk_density);
	logger(g_debug_log, "Bulk density = %f g/cm^3\n", c->bulk_density);
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


